#include <rvh_test.h>

bool check_xip_regs_1(){

    TEST_START();

    CSRW(mideleg, 0);

    int64_t mtime_mask = ~((int64_t)0x80);

    CSRW(mideleg, (uint64_t)-1);
    VERBOSE("setting mideleg and hideleg\n");
    CSRW(CSR_HIDELEG, (uint64_t)-1);
    check_csr_wrrd("vsip", CSR_VSIP, (uint64_t) -1, 0x2002);
    check_csr_wrrd("vsie", CSR_VSIE, (uint64_t) -1, 0x2222);        //开启Shlcofideleg 扩展则为2222，否则为222
    
    TEST_END();
}

bool check_xip_regs_2(){

    TEST_START();

    CSRC(CSR_MENVCFG,1ULL<<63);                   //若menvcfg.stce=0,则mip的stip可写，若为1则不可写

    CSRW(mideleg, (uint64_t)-1);
    VERBOSE("setting mideleg and hideleg\n");
    
    CSRW(CSR_HIDELEG, (uint64_t)-1);

    VERBOSE("setting all in mip\n");
    CSRW(mip, (uint64_t)-1);
    check_csr_rd("hip", CSR_HIP, 0x4);
    check_csr_rd("sip", sip, 0x2222);    

    // check_csr_rd_mask("mip", mip, 0x226, mtime_mask); // only test when nemu don't use difftest because spike, as ref, shut up time interrupt
    check_csr_rd("vsip", CSR_VSIP, 0x2002);
    goto_priv(PRIV_VS);
    check_csr_rd("sip (vs perspective)", sip, 0x2002);
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

    TEST_END();
}

bool check_xip_regs_3(){

    TEST_START();

    CSRW(mideleg, (uint64_t)-1);
    VERBOSE("setting mideleg and hideleg\n");
    CSRW(CSR_HIDELEG, (uint64_t)-1);

    VERBOSE("setting all in hvip\n");
    CSRW(CSR_HVIP, (uint64_t)-1);
    check_csr_rd("hvip", CSR_HVIP, 0x2444);
    check_csr_rd("hip", CSR_HIP, 0x444);
    check_csr_rd("sip", sip, 0x0);
    // check_csr_rd_mask("mip", mip, 0x444, mtime_mask);
    check_csr_rd("vsip", CSR_VSIP, 0x222);
    goto_priv(PRIV_VS);
    check_csr_rd("sip (vs perspective)", sip, 0x222);
    goto_priv(PRIV_M);

    VERBOSE("clearing all in hvip\n");
    CSRW(CSR_HVIP, (uint64_t)0);
    check_csr_rd("hip", CSR_HIP, 0x0);
    check_csr_rd("sip", sip, 0x0);
    // check_csr_rd_mask("mip", mip, 0x000, mtime_mask);
    check_csr_rd("vsip", CSR_VSIP, 0x0);
    goto_priv(PRIV_VS);
    check_csr_rd("sip (vs perspective)", sip, 0x0);
    goto_priv(PRIV_M);

    TEST_END();
}

bool interrupt_tests(){

    TEST_START();

    /**
     * Test trigerring VSSI without delegating it. 
     * It assumes it is already delegated in miedeleg (it should be hardwired)
     */
    
    goto_priv(PRIV_HS);
    CSRC(sstatus, SSTATUS_SPIE_BIT | SSTATUS_SIE_BIT); 
    CSRS(CSR_HIE, 0x4);
    CSRS(CSR_HIP, 0x4);
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_VS);
    //CSRS(sstatus, SSTATUS_SIE_BIT);
    TEST_ASSERT("vs sw irq with no delegation", 
        excpt.triggered && excpt.cause == CAUSE_VSSI && excpt.priv == PRIV_HS);
   
    /**
     * Test trigerring VSSI and delegating it. Should trap to VS with cause SSI.
     * It assumes it is already delegated in miedeleg (it should be hardwired)
     */
    goto_priv(PRIV_HS);
    CSRS(CSR_HIDELEG, 0x4);
    CSRS(CSR_HIP, 0x4);
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_VS);
    CSRS(sie, 0x2);
    CSRS(sstatus, 0x2);
    TEST_ASSERT("vs sw irq with delegation", 
        excpt.triggered && excpt.cause == CAUSE_SSI && excpt.priv == PRIV_VS);


    //M模式下产生单bit外部中断，mstatus.mie=1，且mip，mie对应的MEIP、MEIE拉高
    

    TEST_END();
}
