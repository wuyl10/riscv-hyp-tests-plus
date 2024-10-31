#include <rvh_test.h>
#include <page_tables.h>

bool mstatus_csr_tests_1(){

TEST_START();

//暂时无法测
  /*
    //mstatus.mie 控制中断响应
    TEST_SETUP_EXCEPT();

    printf("%d\n",excpt.triggered);
    CSRS(CSR_MSTATUS,1ULL << 3);    //mie位
    CSRS(CSR_MIE, 1ULL << 11);
    CSRS(CSR_MIP, 1ULL << 11);
    printf("%d\n",excpt.triggered);
    TEST_ASSERT("mstatus.mie cause",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SPF
    );


    //mstatus.GVA 控制异常种类时写0或1
    TEST_SETUP_EXCEPT();
    
    CSRS(CSR_MSTATUS,MSTATUS_GVA);
    CSRR(CSR_SSTATUS);
    printf("%d\n",CSR_MTVAL);
    printf("%d\n",excpt.cause);
    printf("%d\n",excpt.triggered);

    CSRC(CSR_MSTATUS,MSTATUS_GVA);
    ecall1();
    printf("%d\n",CSR_MTVAL);
    printf("%d\n",excpt.cause);


    //mstatus.MPV 控制异常发生时的虚拟化模式
    goto_priv(PRIV_M);
    TEST_SETUP_EXCEPT();
    int x=(CSRR(CSR_MSTATUS) >> 39) & 1;
    printf("%d\n",x);
    goto_priv(PRIV_VS);
    hfence_vvma();
    goto_priv(PRIV_M);
    x=(CSRR(CSR_MSTATUS) >> 39) & 1;
    printf("%d\n",x);

    reset_state();
    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_MIE);
    printf("bugai\n");
    printf("%d\n",excpt.triggered);
    goto_priv(PRIV_M);
    x=(CSRR(CSR_MSTATUS) >> 39) & 1;
    printf("%d\n",x);
*/

//mstatus.SUM 阻止S对U访问，缺页异常
    
    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();

    //执行load指令时，设置了mstatus.SUM=0，在S模式下访问U模式可以访问的页表项
    goto_priv(PRIV_M);
    CSRC(CSR_MSTATUS, MSTATUS_SUM);
    uintptr_t addr = hs_page_base(VSURWX_GURWX);

    TEST_SETUP_EXCEPT();

    goto_priv(PRIV_HS);
    hlvd(addr);
    TEST_ASSERT("s mode load u mode page when mstatus.sum=0 leads to LPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_LPF
    );

    TEST_END();
}

bool mstatus_csr_tests_2(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();

    //执行load指令时，设置了mstatus.SUM=1，在S模式下访问U模式可以访问的页表项
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS, MSTATUS_SUM);
    uintptr_t addr = hs_page_base(VSURWX_GURWX);

    TEST_SETUP_EXCEPT();

    goto_priv(PRIV_HS);
    set_prev_priv(PRIV_VS);
    hlvd(addr);
        printf("%d\n",excpt.triggered);
    printf("%d\n",excpt.cause);
    TEST_ASSERT("s mode load u mode page when mstatus.sum=1 successful",
        excpt.triggered == false
    );


    TEST_END();
}

bool mstatus_csr_tests_3(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();
    //执行load指令时，设置了mstatus.MXR=0，只能从只读页面读取，如果此时pte.r=0
    goto_priv(PRIV_M);

    uintptr_t addr = hs_page_base(VSX_GRWX);

    CSRC(CSR_MSTATUS, MSTATUS_MXR);
    goto_priv(PRIV_HS);
    set_prev_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    hlvd(addr);   
    printf("%d\n",excpt.triggered);
    printf("%d\n",excpt.cause);
    TEST_ASSERT("s mode load when mstatus.mxr=0 and pte.r=0 leads to LPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_LPF
    );


    TEST_END();
}

bool mstatus_csr_tests_4(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();
    //执行load指令时，设置了mstatus.MXR=1，能从可读可执行页面读取，如果此时页表项pte.r=0&&pte.x=0

    uintptr_t addr = hs_page_base(VSV_GV);

    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS, MSTATUS_MXR);
    goto_priv(PRIV_HS);
    set_prev_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    hlvd(addr);
    TEST_ASSERT("s mode load when mstatus.mxr=1 and pte.r=0 and pte.x=0 leads to LPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_LPF
    );
    CSRC(CSR_SSTATUS, SSTATUS_MXR);


TEST_END();

}

bool mstatus_csr_tests_5(){

    TEST_START();
    //mstatus.TSR=1时，执行sret指令
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS,1ULL << 22);    //TSR位

    goto_priv(PRIV_HS);
    set_prev_priv(PRIV_VU);

    TEST_EXEC_SRET();

    TEST_ASSERT("hs mode sret cause to ili when mstatus.TSR=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 

    TEST_END();

}

bool hedeleg_csr_tests_1(){

    TEST_START();
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRW(CSR_HEDELEG, (uint64_t)-1);
    printf("%llx\n",CSRR(CSR_HEDELEG));

    TEST_ASSERT("hedeleg right",
        CSRR(CSR_HEDELEG) == 0xb1ff
    ); 

    TEST_END();

}

bool mhpmevent_csr_tests_1(){

    TEST_START();
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRW(CSR_MHPMEVENT3, (uint64_t)-1);
    printf("%llx\n",CSRR(CSR_MHPMEVENT3));

    TEST_END();

}

bool tselect_csr_tests_1(){

    TEST_START();
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRW(CSR_TSELECT, (uint64_t)-1);
    printf("%llx\n",CSRR(CSR_TSELECT));

    TEST_ASSERT("tselect right",
        CSRR(CSR_HEDELEG) == 0xb1ff
    ); 

    TEST_END();

}


bool random_instruction_tests(){

    TEST_START();
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);

    for (int i = 0; i < 5; i++)
    {
        random_m_instruction();
    }
    

    TEST_END();

}