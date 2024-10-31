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


    goto_priv(PRIV_M);     

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

    TEST_END();

}