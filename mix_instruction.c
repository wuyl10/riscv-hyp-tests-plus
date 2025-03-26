#include <rvh_test.h>
#include <csrs.h> 
#include <page_tables.h>



#define TINST_LOAD(ins) ((ins) & (INS_OPCODE | INS_RD | INS_FUNCT3))
#define TINST_STORE(ins) ((ins) & (INS_OPCODE | INS_FUNCT3 | INS_RS2))
#define TINST_AMO(ins) ((ins) & (~TINST_ADDROFF))
#define TINST_CHECK(CHECK) (excpt.tinst == 0 || excpt.tinst == CHECK(read_instruction(excpt.epc)))
#define TINST_CHECK_COMPRESSED(CHECK) (excpt.tinst == 0 ||\
    excpt.tinst == CHECK(expand_compressed_instruction(read_instruction(excpt.epc)) & ~0b10ULL))



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


bool mix_instruction_2(){

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
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    sret();
    TEST_ASSERT("vs sret leads to virtual instruction exception when mstatus.TW=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
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
 
    // 当 mstatus.VS 被设置为 Off 时，尝试执行向量指令
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS, 1ULL << 9);      //设置mstatus.vs!=0
    CSRC(CSR_MSTATUS, 1ULL << 10); 

    int vl = 8;          // 向量长度
    int sew = 2;         // SEW = 32 位
    int lmull = 1;       // LMUL = 1
    int v0_init = 1;     // 初始化掩码寄存器 v0 的值为全 1
    int v2_init = 1;     // 初始化 v2 的值为全 1

    // 设置 vcpop.m 的执行条件
    set_vcpop_conditions(vl, sew, lmull, v0_init, v2_init);

    CSRC(CSR_MSTATUS, 1ULL << 9);      //设置mstatus.vs=0
    CSRC(CSR_MSTATUS, 1ULL << 10); 

    TEST_SETUP_EXCEPT();
    // 执行 vcpop.m 指令并返回结果
    uint32_t result = execute_vcpop_v2();

    TEST_ASSERT("An attempt to execute vector instructions cause to ILI when mstatus.vs=0",
        excpt.triggered == true &&  
        excpt.cause == CAUSE_ILI
    );   

//----------------------------------------------------------------------

    //m模式下执行ebreak指令
    goto_priv(PRIV_M);
    TEST_SETUP_EXCEPT();
    
    ebreak();

    TEST_ASSERT("m mode ebreak cause to Breakpoint",
        excpt.triggered == true &&
        excpt.cause == CAUSE_BKP
    ); 


//--------------------------------------------------------------------
    TEST_SETUP_EXCEPT();

    goto_priv(PRIV_HS);

    goto_priv(PRIV_VS);   
    vaddr_f = hs_page_base(VSRWX_GI);      
    value = 0xdeadbeef;

    TEST_SETUP_EXCEPT();        

    // uint8_t src8[] = {1, 2, 3, 4};
    // uint8_t dest8[4];
    // vle8_v(dest8, (uintptr_t)src8);


    value = lb(vaddr_f);    

    TEST_ASSERT("correct tinst when executing a lb which results in a lgpf",         
        excpt.triggered == true && 
        excpt.cause == CAUSE_LGPF 
    );

    TEST_SETUP_EXCEPT();
    value = lbu(vaddr_f);
    TEST_ASSERT("correct tinst when executing a lbu which results in a lgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_LGPF &&
        TINST_CHECK(TINST_LOAD)
    );

    TEST_SETUP_EXCEPT();
    value = lh(vaddr_f);
    TEST_ASSERT("correct tinst when executing a lh which results in a lgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_LGPF &&
        TINST_CHECK(TINST_LOAD)
    );

    TEST_SETUP_EXCEPT();
    value = lhu(vaddr_f);
    TEST_ASSERT("correct tinst when executing a lhu which results in a lgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_LGPF &&
        TINST_CHECK(TINST_LOAD)
    );

    TEST_SETUP_EXCEPT();
    lw(vaddr_f);
    TEST_ASSERT("correct tinst when executing a lw which results in a lgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_LGPF &&
        TINST_CHECK(TINST_LOAD)
    );

    TEST_SETUP_EXCEPT();
    value = lwu(vaddr_f);
    TEST_ASSERT("correct tinst when executing a lwu which results in a lgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_LGPF &&
        TINST_CHECK(TINST_LOAD)
    );

    TEST_SETUP_EXCEPT();
    value = ld(vaddr_f);
    TEST_ASSERT("correct tinst when executing a ld which results in a lgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_LGPF &&
        TINST_CHECK(TINST_LOAD)
    );

    value = 0xdeadbeef;

    TEST_SETUP_EXCEPT();
    sb(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a sb which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_STORE)
    );

    TEST_SETUP_EXCEPT();
    sh(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a sh which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_STORE)
    );

    TEST_SETUP_EXCEPT();
    sw(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a sw which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_STORE)
    );

    TEST_SETUP_EXCEPT();
    sd(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a sd which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_STORE)
    );

    TEST_SETUP_EXCEPT();
    value = c_lw(vaddr_f);
    TEST_ASSERT("correct tinst when executing a c.lw which results in a lgpf",
        excpt.triggered == true &&
        excpt.cause == CAUSE_LGPF &&
        TINST_CHECK_COMPRESSED(TINST_LOAD)
    );

    TEST_SETUP_EXCEPT();
    value = c_ld(vaddr_f);
    TEST_ASSERT("correct tinst when executing a c.ld which results in a lgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_LGPF &&
        TINST_CHECK_COMPRESSED(TINST_LOAD)
    );

    TEST_SETUP_EXCEPT();
    c_sw(vaddr_f, value);       
    TEST_ASSERT("correct tinst when executing a c.lw which results in a lgpf",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK_COMPRESSED(TINST_STORE)
    );

    TEST_SETUP_EXCEPT();
    c_sd(vaddr_f, value);
    TEST_ASSERT("correct tinst when executing a c.sd which results in a lgpf",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK_COMPRESSED(TINST_STORE)
    );

    TEST_SETUP_EXCEPT();
    value = lr_w(vaddr_f);
    TEST_ASSERT("correct tinst when executing a lr.w which results in a lgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_LGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = sc_w(vaddr_f, value);
    TEST_ASSERT("correct tinst when executing a sc.w which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoswap_w(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoswap.w which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoadd_w(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoadd.w which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoxor_w(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoxor.w which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
    TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoand_w(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoand.w which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoor_w(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoor.w which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amomin_w(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amomin.w which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amomax_w(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amomax.w which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amominu_w(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amominu.w which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amomaxu_w(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amomaxu.w which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoswap_d(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoswap.d which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoadd_d(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoadd.d which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoxor_d(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoxor.d which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoand_d(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoand.d which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoor_d(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoor.d which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amomin_d(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amomin.d which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amomax_d(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amomax.d which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amominu_d(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amominu.d which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amomaxu_d(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amomaxu.d which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );


//----------------------------------------------------------------------------------

    goto_priv(PRIV_M);
    
    //pmpcfg.L被设置，当前特权级是M，访问没有执行权限的PMP区域获取指令，pmpcfg.R=0
    CSRW(CSR_PMPCFG0,(uint64_t)0x0);

    CSRC(CSR_PMPCFG0,1ULL << 8 );      //pmp1cfg的R位（清除）
    CSRS(CSR_PMPCFG0,1ULL << 9 );      //pmp1cfg的W位
    CSRS(CSR_PMPCFG0,1ULL << 10 );      //pmp1cfg的X位
    CSRS(CSR_PMPCFG0,1ULL << 11 );      //pmp1cfg的TOR模式

    CSRW(CSR_PMPADDR0, (uintptr_t)0x80000000);
    CSRW(CSR_PMPADDR1, (uintptr_t)0x81000000);
    // sfence();
    // hfence();
    CSRS(CSR_PMPCFG0,1ULL << 15 );       //pmp1cfg的L位 

    TEST_SETUP_EXCEPT();

    lb(0x80000100UL << 2);    //访问区域内地址


    TEST_ASSERT("m mode lb when pmpcfg.R=0 and pmpcfg.L=1 leads to LAF",        //预期产生load access fault
        excpt.triggered == true &&
        excpt.cause == CAUSE_LAF
    );

//------------------------------------------------------------

    goto_priv(PRIV_M);

    //pmpcfg.L被设置，当前特权级是HS，访问没有执行权限的PMP区域获取指令，pmpcfg.W=0
    
    CSRW(CSR_PMPCFG0,(uint64_t)0x0);
    
    CSRS(CSR_PMPCFG0,1ULL << 0 );      //pmp0cfg的R位
    CSRS(CSR_PMPCFG0,1ULL << 1 );      //pmp0cfg的W位
    CSRS(CSR_PMPCFG0,1ULL << 2 );      //pmp0cfg的X位
    CSRS(CSR_PMPCFG0,1ULL << 3 );      //pmp0cfg的TOR模式


    CSRS(CSR_PMPCFG0,1ULL << 8 );      //pmp1cfg的R位
    CSRC(CSR_PMPCFG0,1ULL << 9 );      //pmp1cfg的W位
    CSRS(CSR_PMPCFG0,1ULL << 10 );      //pmp1cfg的X位
    CSRS(CSR_PMPCFG0,1ULL << 11 );      //pmp1cfg的TOR模式

    CSRW(CSR_PMPADDR0, (uintptr_t)0x80000000);
    CSRW(CSR_PMPADDR1, (uintptr_t)0x81000000);

    CSRS(CSR_PMPCFG0,1ULL << 7 );     //pmp0cfg的L位 
    CSRS(CSR_PMPCFG0,1ULL << 15 );       //pmp1cfg的L位 


    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();    
    
    amoadd_d(0x80000100UL << 2 , 0xdeadbeef);

    TEST_ASSERT("hs mode amoadd_d when pmpcfg.W=0 and pmpcfg.L=1 leads to SAF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SAF
    );

//-------------------------------------------------------------

    goto_priv(PRIV_M);

    //pmpcfg.L被设置，当前特权级是HS，访问没有执行权限的PMP区域获取指令，pmpcfg.X=0
    
    CSRW(CSR_PMPCFG0,(uint64_t)0x0);
    
    CSRS(CSR_PMPCFG0,1ULL << 0 );      //pmp0cfg的R位
    CSRS(CSR_PMPCFG0,1ULL << 1 );      //pmp0cfg的W位
    CSRS(CSR_PMPCFG0,1ULL << 2 );      //pmp0cfg的X位
    CSRS(CSR_PMPCFG0,1ULL << 3 );      //pmp0cfg的TOR模式


    CSRS(CSR_PMPCFG0,1ULL << 8 );      //pmp1cfg的R位
    CSRS(CSR_PMPCFG0,1ULL << 9 );      //pmp1cfg的W位
    CSRC(CSR_PMPCFG0,1ULL << 10 );      //pmp1cfg的X位
    CSRS(CSR_PMPCFG0,1ULL << 11 );      //pmp1cfg的TOR模式

    CSRW(CSR_PMPADDR0, (uintptr_t)0x80000000);
    CSRW(CSR_PMPADDR1, (uintptr_t)0x81000000);

    CSRS(CSR_PMPCFG0,1ULL << 7 );     //pmp0cfg的L位 
    CSRC(CSR_PMPCFG0,1ULL << 15 );       //pmp1cfg的L位 

    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();    
    
    TEST_EXEC_EXCEPT(0x80000100UL << 2);

    TEST_ASSERT("hs mode fetch instruction when pmpcfg.X=0 and pmpcfg.L=1 leads to IAF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_IAF
    );

//--------------------------------------------------------------

    goto_priv(PRIV_M);

    //跨越了两个具有不同权限的内存区域，一部分访问成功，一部分失败
    
    CSRW(CSR_PMPCFG0,(uint64_t)0x0);
    
    CSRS(CSR_PMPCFG0,1ULL << 0 );      //pmp0cfg的R位
    CSRS(CSR_PMPCFG0,1ULL << 1 );      //pmp0cfg的W位
    CSRS(CSR_PMPCFG0,1ULL << 2 );      //pmp0cfg的X位
    CSRS(CSR_PMPCFG0,1ULL << 3 );      //pmp0cfg的TOR模式


    CSRS(CSR_PMPCFG0,1ULL << 8 );      //pmp1cfg的R位
    CSRC(CSR_PMPCFG0,1ULL << 9 );      //pmp1cfg的W位
    CSRS(CSR_PMPCFG0,1ULL << 10 );      //pmp1cfg的X位
    CSRS(CSR_PMPCFG0,1ULL << 11 );      //pmp1cfg的TOR模式

    CSRW(CSR_PMPADDR0, (uintptr_t)0x80000000);
    CSRW(CSR_PMPADDR1, (uintptr_t)0x81000000);

    CSRS(CSR_PMPCFG0,1ULL << 7 );     //pmp0cfg的L位 
    CSRC(CSR_PMPCFG0,1ULL << 15 );       //pmp1cfg的L位 

    TEST_SETUP_EXCEPT();    
    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();    
    
    sd(0x1fffffffeUL << 2 ,0xdeadbeef);

    TEST_ASSERT("Spanning two memory regions with different permissions, some accessed successfully and some failed leads to saf",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SAF
    );

//-------------------------------------------------------------------------


    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    hfence_vvma();
    TEST_ASSERT("vs executing hfence.vvma leads to virtual instruction exception",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 
    
    
    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    hfence_gvma();
    TEST_ASSERT("vs executing hfence.gvma leads to virtual instruction exception",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 

    goto_priv(PRIV_M);  
    CSRS(CSR_HSTATUS, HSTATUS_VTVM);
    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    sfence_vma();
    TEST_ASSERT("vs executing sfence.vma leads to virtual instruction exception when vtvm=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 

    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    Sinval_vma();
    TEST_ASSERT("vs executing sinval.vma leads to virtual instruction exception when vtvm=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 


//------------------------------------------------------------------------------------------------

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
    reset_state();
    CSRS(CSR_HSTATUS, HSTATUS_VTSR);
    CSRC(CSR_MSTATUS, MSTATUS_TW);
    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    sret();
    TEST_ASSERT("vs sret leads to virtual instruction exception when mstatus.TW=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    );  
//----------------------------------------------------------------------

    goto_priv(PRIV_M);


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
     vaddr_f = hs_page_base(VSRWX_GRWX) + 1;      
     value = 0xdeadbeef;

    //load byte地址不会发生未对齐
    TEST_SETUP_EXCEPT();        
    value = lb(vaddr_f);
    TEST_ASSERT("load byte address is not aligned successful",         
        excpt.triggered == false
    );
//----------------------------------------------------------------------

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
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    sret();
    TEST_ASSERT("vs sret leads to virtual instruction exception when mstatus.TW=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
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
     vaddr_f = hs_page_base(VSRWX_GRWX) + 1;      
     value = 0xdeadbeef;

    //load byte地址不会发生未对齐
    TEST_SETUP_EXCEPT();        
    value = lb(vaddr_f);
    TEST_ASSERT("load byte address is not aligned successful",         
        excpt.triggered == false
    );
//----------------------------------------------------------------------
 
    // 当 mstatus.VS 被设置为 Off 时，尝试执行向量指令
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS, 1ULL << 9);      //设置mstatus.vs!=0
    CSRC(CSR_MSTATUS, 1ULL << 10); 

     vl = 8;          // 向量长度
     sew = 2;         // SEW = 32 位
     lmull = 1;       // LMUL = 1
     v0_init = 1;     // 初始化掩码寄存器 v0 的值为全 1
     v2_init = 1;     // 初始化 v2 的值为全 1

    // 设置 vcpop.m 的执行条件
    set_vcpop_conditions(vl, sew, lmull, v0_init, v2_init);

    CSRC(CSR_MSTATUS, 1ULL << 9);      //设置mstatus.vs=0
    CSRC(CSR_MSTATUS, 1ULL << 10); 

    TEST_SETUP_EXCEPT();
    // 执行 vcpop.m 指令并返回结果
     result = execute_vcpop_v2();

    TEST_ASSERT("An attempt to execute vector instructions cause to ILI when mstatus.vs=0",
        excpt.triggered == true &&  
        excpt.cause == CAUSE_ILI
    );   

//----------------------------------------------------------------------

    //m模式下执行ebreak指令
    goto_priv(PRIV_M);
    TEST_SETUP_EXCEPT();
    
    ebreak();

    TEST_ASSERT("m mode ebreak cause to Breakpoint",
        excpt.triggered == true &&
        excpt.cause == CAUSE_BKP
    ); 


//--------------------------------------------------------------------
    TEST_SETUP_EXCEPT();

    goto_priv(PRIV_HS);

    goto_priv(PRIV_VS);   
    vaddr_f = hs_page_base(VSRWX_GI);      
    value = 0xdeadbeef;

    TEST_SETUP_EXCEPT();        

    // uint8_t src8[] = {1, 2, 3, 4};
    // uint8_t dest8[4];
    // vle8_v(dest8, (uintptr_t)src8);


    value = lb(vaddr_f);    

    TEST_ASSERT("correct tinst when executing a lb which results in a lgpf",         
        excpt.triggered == true && 
        excpt.cause == CAUSE_LGPF 
    );

    TEST_SETUP_EXCEPT();
    value = lbu(vaddr_f);
    TEST_ASSERT("correct tinst when executing a lbu which results in a lgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_LGPF &&
        TINST_CHECK(TINST_LOAD)
    );

    TEST_SETUP_EXCEPT();
    value = lh(vaddr_f);
    TEST_ASSERT("correct tinst when executing a lh which results in a lgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_LGPF &&
        TINST_CHECK(TINST_LOAD)
    );

    TEST_SETUP_EXCEPT();
    value = lhu(vaddr_f);
    TEST_ASSERT("correct tinst when executing a lhu which results in a lgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_LGPF &&
        TINST_CHECK(TINST_LOAD)
    );

    TEST_SETUP_EXCEPT();
    lw(vaddr_f);
    TEST_ASSERT("correct tinst when executing a lw which results in a lgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_LGPF &&
        TINST_CHECK(TINST_LOAD)
    );

    TEST_SETUP_EXCEPT();
    value = lwu(vaddr_f);
    TEST_ASSERT("correct tinst when executing a lwu which results in a lgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_LGPF &&
        TINST_CHECK(TINST_LOAD)
    );

    TEST_SETUP_EXCEPT();
    value = ld(vaddr_f);
    TEST_ASSERT("correct tinst when executing a ld which results in a lgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_LGPF &&
        TINST_CHECK(TINST_LOAD)
    );

    value = 0xdeadbeef;

    TEST_SETUP_EXCEPT();
    sb(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a sb which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_STORE)
    );

    TEST_SETUP_EXCEPT();
    sh(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a sh which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_STORE)
    );

    TEST_SETUP_EXCEPT();
    sw(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a sw which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_STORE)
    );

    TEST_SETUP_EXCEPT();
    sd(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a sd which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_STORE)
    );

    TEST_SETUP_EXCEPT();
    value = c_lw(vaddr_f);
    TEST_ASSERT("correct tinst when executing a c.lw which results in a lgpf",
        excpt.triggered == true &&
        excpt.cause == CAUSE_LGPF &&
        TINST_CHECK_COMPRESSED(TINST_LOAD)
    );

    TEST_SETUP_EXCEPT();
    value = c_ld(vaddr_f);
    TEST_ASSERT("correct tinst when executing a c.ld which results in a lgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_LGPF &&
        TINST_CHECK_COMPRESSED(TINST_LOAD)
    );

    TEST_SETUP_EXCEPT();
    c_sw(vaddr_f, value);       
    TEST_ASSERT("correct tinst when executing a c.lw which results in a lgpf",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK_COMPRESSED(TINST_STORE)
    );

    TEST_SETUP_EXCEPT();
    c_sd(vaddr_f, value);
    TEST_ASSERT("correct tinst when executing a c.sd which results in a lgpf",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK_COMPRESSED(TINST_STORE)
    );

    TEST_SETUP_EXCEPT();
    value = lr_w(vaddr_f);
    TEST_ASSERT("correct tinst when executing a lr.w which results in a lgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_LGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = sc_w(vaddr_f, value);
    TEST_ASSERT("correct tinst when executing a sc.w which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoswap_w(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoswap.w which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoadd_w(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoadd.w which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoxor_w(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoxor.w which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
    TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoand_w(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoand.w which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoor_w(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoor.w which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amomin_w(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amomin.w which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amomax_w(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amomax.w which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amominu_w(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amominu.w which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amomaxu_w(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amomaxu.w which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoswap_d(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoswap.d which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoadd_d(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoadd.d which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoxor_d(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoxor.d which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoand_d(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoand.d which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoor_d(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoor.d which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amomin_d(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amomin.d which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amomax_d(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amomax.d which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amominu_d(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amominu.d which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amomaxu_d(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amomaxu.d which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );


//----------------------------------------------------------------------------------

    goto_priv(PRIV_M);
    
    //pmpcfg.L被设置，当前特权级是M，访问没有执行权限的PMP区域获取指令，pmpcfg.R=0
    CSRW(CSR_PMPCFG0,(uint64_t)0x0);

    CSRC(CSR_PMPCFG0,1ULL << 8 );      //pmp1cfg的R位（清除）
    CSRS(CSR_PMPCFG0,1ULL << 9 );      //pmp1cfg的W位
    CSRS(CSR_PMPCFG0,1ULL << 10 );      //pmp1cfg的X位
    CSRS(CSR_PMPCFG0,1ULL << 11 );      //pmp1cfg的TOR模式

    CSRW(CSR_PMPADDR0, (uintptr_t)0x80000000);
    CSRW(CSR_PMPADDR1, (uintptr_t)0x81000000);
    // sfence();
    // hfence();
    CSRS(CSR_PMPCFG0,1ULL << 15 );       //pmp1cfg的L位 

    TEST_SETUP_EXCEPT();

    lb(0x80000100UL << 2);    //访问区域内地址


    TEST_ASSERT("m mode lb when pmpcfg.R=0 and pmpcfg.L=1 leads to LAF",        //预期产生load access fault
        excpt.triggered == true &&
        excpt.cause == CAUSE_LAF
    );

//------------------------------------------------------------

    goto_priv(PRIV_M);

    //pmpcfg.L被设置，当前特权级是HS，访问没有执行权限的PMP区域获取指令，pmpcfg.W=0
    
    CSRW(CSR_PMPCFG0,(uint64_t)0x0);
    
    CSRS(CSR_PMPCFG0,1ULL << 0 );      //pmp0cfg的R位
    CSRS(CSR_PMPCFG0,1ULL << 1 );      //pmp0cfg的W位
    CSRS(CSR_PMPCFG0,1ULL << 2 );      //pmp0cfg的X位
    CSRS(CSR_PMPCFG0,1ULL << 3 );      //pmp0cfg的TOR模式


    CSRS(CSR_PMPCFG0,1ULL << 8 );      //pmp1cfg的R位
    CSRC(CSR_PMPCFG0,1ULL << 9 );      //pmp1cfg的W位
    CSRS(CSR_PMPCFG0,1ULL << 10 );      //pmp1cfg的X位
    CSRS(CSR_PMPCFG0,1ULL << 11 );      //pmp1cfg的TOR模式

    CSRW(CSR_PMPADDR0, (uintptr_t)0x80000000);
    CSRW(CSR_PMPADDR1, (uintptr_t)0x81000000);

    CSRS(CSR_PMPCFG0,1ULL << 7 );     //pmp0cfg的L位 
    CSRS(CSR_PMPCFG0,1ULL << 15 );       //pmp1cfg的L位 


    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();    
    
    amoadd_d(0x80000100UL << 2 , 0xdeadbeef);

    TEST_ASSERT("hs mode amoadd_d when pmpcfg.W=0 and pmpcfg.L=1 leads to SAF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SAF
    );

//-------------------------------------------------------------

    goto_priv(PRIV_M);

    //pmpcfg.L被设置，当前特权级是HS，访问没有执行权限的PMP区域获取指令，pmpcfg.X=0
    
    CSRW(CSR_PMPCFG0,(uint64_t)0x0);
    
    CSRS(CSR_PMPCFG0,1ULL << 0 );      //pmp0cfg的R位
    CSRS(CSR_PMPCFG0,1ULL << 1 );      //pmp0cfg的W位
    CSRS(CSR_PMPCFG0,1ULL << 2 );      //pmp0cfg的X位
    CSRS(CSR_PMPCFG0,1ULL << 3 );      //pmp0cfg的TOR模式


    CSRS(CSR_PMPCFG0,1ULL << 8 );      //pmp1cfg的R位
    CSRS(CSR_PMPCFG0,1ULL << 9 );      //pmp1cfg的W位
    CSRC(CSR_PMPCFG0,1ULL << 10 );      //pmp1cfg的X位
    CSRS(CSR_PMPCFG0,1ULL << 11 );      //pmp1cfg的TOR模式

    CSRW(CSR_PMPADDR0, (uintptr_t)0x80000000);
    CSRW(CSR_PMPADDR1, (uintptr_t)0x81000000);

    CSRS(CSR_PMPCFG0,1ULL << 7 );     //pmp0cfg的L位 
    CSRC(CSR_PMPCFG0,1ULL << 15 );       //pmp1cfg的L位 

    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();    
    
    TEST_EXEC_EXCEPT(0x80000100UL << 2);

    TEST_ASSERT("hs mode fetch instruction when pmpcfg.X=0 and pmpcfg.L=1 leads to IAF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_IAF
    );

//--------------------------------------------------------------

    goto_priv(PRIV_M);

    //跨越了两个具有不同权限的内存区域，一部分访问成功，一部分失败
    
    CSRW(CSR_PMPCFG0,(uint64_t)0x0);
    
    CSRS(CSR_PMPCFG0,1ULL << 0 );      //pmp0cfg的R位
    CSRS(CSR_PMPCFG0,1ULL << 1 );      //pmp0cfg的W位
    CSRS(CSR_PMPCFG0,1ULL << 2 );      //pmp0cfg的X位
    CSRS(CSR_PMPCFG0,1ULL << 3 );      //pmp0cfg的TOR模式


    CSRS(CSR_PMPCFG0,1ULL << 8 );      //pmp1cfg的R位
    CSRC(CSR_PMPCFG0,1ULL << 9 );      //pmp1cfg的W位
    CSRS(CSR_PMPCFG0,1ULL << 10 );      //pmp1cfg的X位
    CSRS(CSR_PMPCFG0,1ULL << 11 );      //pmp1cfg的TOR模式

    CSRW(CSR_PMPADDR0, (uintptr_t)0x80000000);
    CSRW(CSR_PMPADDR1, (uintptr_t)0x81000000);

    CSRS(CSR_PMPCFG0,1ULL << 7 );     //pmp0cfg的L位 
    CSRC(CSR_PMPCFG0,1ULL << 15 );       //pmp1cfg的L位 

    TEST_SETUP_EXCEPT();    
    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();    
    
    sd(0x1fffffffeUL << 2 ,0xdeadbeef);

    TEST_ASSERT("Spanning two memory regions with different permissions, some accessed successfully and some failed leads to saf",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SAF
    );

//-------------------------------------------------------------------------


    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    hfence_vvma();
    TEST_ASSERT("vs executing hfence.vvma leads to virtual instruction exception",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 
    
    
    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    hfence_gvma();
    TEST_ASSERT("vs executing hfence.gvma leads to virtual instruction exception",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 

    goto_priv(PRIV_M);  
    CSRS(CSR_HSTATUS, HSTATUS_VTVM);
    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    sfence_vma();
    TEST_ASSERT("vs executing sfence.vma leads to virtual instruction exception when vtvm=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 

    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    Sinval_vma();
    TEST_ASSERT("vs executing sinval.vma leads to virtual instruction exception when vtvm=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 


//------------------------------------------------------------------------------------------------


    TEST_END();

}