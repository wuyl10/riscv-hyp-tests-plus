#include <rvh_test.h>
#include <csrs.h> 
#include <page_tables.h>

bool mix_instruction_1(){

    TEST_START();

    goto_priv(PRIV_M);
    CSRS(CSR_MENVCFG,  1ULL << 63);     //stce位
    CSRC(CSR_HENVCFG,  1ULL << 63);     //stce位
    CSRS(CSR_MCOUNTEREN, HCOUNTEREN_TM); 
    CSRS(CSR_HCOUNTEREN, HCOUNTEREN_TM); 

    goto_priv(PRIV_VS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_STIMECMP);

    TEST_ASSERT("vs access to stimecmp casuses virtual instruction exception when menvcfg.stce=1 and henvcfg.stce=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    );

//----------------------------------------------------------------------
    goto_priv(PRIV_M);
    CSRC(CSR_MCOUNTEREN, HCOUNTEREN_TM); 

    goto_priv(PRIV_HS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_TIME);

    TEST_ASSERT("hs access to time casuses illegal instruction exception when mcounteren.tm=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    );

//----------------------------------------------------------------------

    // //V=0&mstatus.TW=1时，如果实现了S/HS模式，在U模式下执行WFI指令，没有在一个约定时间内产生中断
    // TEST_SETUP_EXCEPT();
    // goto_priv(PRIV_M);
    // CSRW(CSR_HIE, 0);
    // CSRW(CSR_HIP,0);
    // CSRS(mstatus, MSTATUS_TW);

    // goto_priv(PRIV_HU);
    // wfi();
    
    // TEST_ASSERT("HU-mode wfi causes illegal instruction exception when mstatus.tw=1",
    //     excpt.triggered == true &&  
    //     excpt.cause == CAUSE_ILI
    // ); 
    
//----------------------------------------------------------------------

    goto_priv(PRIV_M);
    CSRS(CSR_HSTATUS, HSTATUS_VTSR);
    CSRC(CSR_MSTATUS, MSTATUS_TW);
    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    sret();
    TEST_ASSERT("vu sret leads to virtual instruction exception when mstatus.TW=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    );  
//----------------------------------------------------------------------

    goto_priv(PRIV_M);
    uint64_t val, valu;
    uintptr_t vaddr;
    uintptr_t addr;

    hspt_init();
    hpt_init(); 
    vspt_init();

    vaddr = vs_page_base(VSURWX_GURWX);
    addr = phys_page_base(VSURWX_GURWX);

    goto_priv(PRIV_HS);

    set_prev_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    write64(addr, 0x1107ec0ffee);
    val = hlvd(vaddr);
    TEST_ASSERT("hs hlvd to vu page successful when spvp = 0",
        excpt.triggered == false
    );

    set_prev_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    write64(addr, 0x1107ec0ffee);
    val = hlvd(vaddr);
    TEST_ASSERT("hs hlvd to vu page leads to exception when spvp = 1",
        excpt.triggered == true&& 
        excpt.cause == CAUSE_LPF
    );


//----------------------------------------------------------------------
    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    goto_priv(PRIV_VS);  
    vspt_init();

    printf("nihao\n");
    goto_priv(PRIV_M);      //unknown_ecall

    CSRC(CSR_MSTATUS, SSTATUS_SUM);

    goto_priv(PRIV_HS);

    //执行sd指令时，设置了mstatus.SUM=0，在S模式下访问U模式可以访问的页表项
    TEST_SETUP_EXCEPT();
    vaddr = hs_page_base(VSURWX_GURWX);

    
    sd(vaddr, 0xdeadbeef);
    TEST_ASSERT("hs mode sd u mode page when mstatus.sum=0 leads to LPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SPF
    );
//----------------------------------------------------------------------
    goto_priv(PRIV_M);
    CSRC(CSR_MENVCFG,1ULL<<63);                   //若menvcfg.stce=0,则mip的stip可写，若为1则不可写

    CSRW(mideleg, (uint64_t)-1);
    VERBOSE("setting mideleg and hideleg\n");
    
    CSRW(CSR_HIDELEG, (uint64_t)-1);

    VERBOSE("setting all in mip\n");
    CSRW(mip, (uint64_t)-1);
    check_csr_rd("hip", CSR_HIP, 0x4);
    check_csr_rd("sip", sip, 0x222);    

    // check_csr_rd_mask("mip", mip, 0x226, mtime_mask); // only test when nemu don't use difftest because spike, as ref, shut up time interrupt
    check_csr_rd("vsip", CSR_VSIP, 0x2);
    goto_priv(PRIV_VS);
    check_csr_rd("sip (vs perspective)", sip, 0x2);
    goto_priv(PRIV_M);

    VERBOSE("clearing all in mip\n");
    CSRW(mip, (uint64_t)0);
    check_csr_rd("hip", CSR_HIP, 0x0);
    check_csr_rd("sip", sip, 0x0);
    // check_csr_rd_mask("mip", mip, 0x000, mtime_mask);
    check_csr_rd("vsip", CSR_VSIP, 0x0);
    goto_priv(PRIV_VS);
    check_csr_rd("sip (vs perspective)", sip, 0x0);
    goto_priv(PRIV_M);   

//----------------------------------------------------------------------
    //satp.MODE只读0，执行sfence.vma
    TEST_SETUP_EXCEPT();

    goto_priv(PRIV_HS);
    CSRW(CSR_SATP,0ULL);
    sfence_vma();
    printf("%d",excpt.triggered);
    TEST_ASSERT("hs mode sfence.vma when satp.mode=0 cause to illegal instruction interrupt",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 
//----------------------------------------------------------------------
    //执行取指指令二级地址翻译阶段时，pte.x=0
    goto_priv(PRIV_VS);
    vaddr = vs_page_base(VSRWX_GRW);
    TEST_SETUP_EXCEPT();
    TEST_EXEC_EXCEPT(vaddr); 

    TEST_ASSERT("vs mode first fetche instruction when the second-stage address translation pte.x=0 leads to IGPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_IGPF
    );
//----------------------------------------------------------------------
    TEST_SETUP_EXCEPT();
    
    
    hspt_init();        
    goto_priv(PRIV_HS);     
    uintptr_t vaddr_f = hs_page_base(VSRWX_GRWX) + 1;      
    uint64_t value = 0xdeadbeef;

    //load byte地址不会发生未对齐
    TEST_SETUP_EXCEPT();        
    value = lb(vaddr_f);
    TEST_ASSERT("load byte address is not aligned successful",         
        excpt.triggered == false
    );
//----------------------------------------------------------------------

    goto_priv(PRIV_M);

    //pmpcfg.L被设置，当前特权级是HU，访问没有执行权限的PMP区域获取指令，pmpcfg.R=0
    
    CSRW(CSR_PMPCFG0,(uint64_t)0x0);
    
    CSRS(CSR_PMPCFG0,1ULL << 0 );      //pmp0cfg的R位
    CSRS(CSR_PMPCFG0,1ULL << 1 );      //pmp0cfg的W位
    CSRS(CSR_PMPCFG0,1ULL << 2 );      //pmp0cfg的X位
    CSRS(CSR_PMPCFG0,1ULL << 3 );      //pmp0cfg的TOR模式

    CSRC(CSR_PMPCFG0,1ULL << 8 );      //pmp1cfg的R位
    CSRS(CSR_PMPCFG0,1ULL << 9 );      //pmp1cfg的W位
    CSRS(CSR_PMPCFG0,1ULL << 10 );      //pmp1cfg的X位
    CSRS(CSR_PMPCFG0,1ULL << 11 );      //pmp1cfg的TOR模式

    CSRW(CSR_PMPADDR0, (uintptr_t)0x80000000);
    CSRW(CSR_PMPADDR1, (uintptr_t)0x81000000);

    CSRS(CSR_PMPCFG0,1ULL << 7 );     //pmp0cfg的L位 
    CSRS(CSR_PMPCFG0,1ULL << 15 );       //pmp1cfg的L位 

    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();    
    
    lb(0x80000100UL << 2);

    TEST_ASSERT("HU mode lb when pmpcfg.R=0 and pmpcfg.L=1 leads to LAF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_LAF
    );

//----------------------------------------------------------------------
    //V=0，U模式下发生异常，打开代理medeleg/mideleg，切换到HS态处理异常，异常处理结束执行sret恢复到U态
    goto_priv(PRIV_M);
    reset_state();
    CSRW(medeleg,(uint64_t)-1);
    CSRW(mideleg,(uint64_t)-1);     
    CSRW(CSR_HIDELEG,0);
    CSRW(CSR_HEDELEG,0);   

    goto_priv(PRIV_HU);     
    
    TEST_SETUP_EXCEPT();        
    CSRR(CSR_MSTATUS);

    TEST_ASSERT("hu trigger except that priv change to m mod and mret to hu mode when medeleg/mideleg==1 and hedeleg/hideleg==0",         
        excpt.triggered == true &&
        excpt.priv == PRIV_HS &&
        curr_priv == PRIV_HU
    );
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//----------------------------------------------------------------------

    TEST_END();

}