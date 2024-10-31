#include <rvh_test.h>
#include <page_tables.h>
#include <csrs.h> 

bool illegal_except_1() {

    TEST_START();

    TEST_SETUP_EXCEPT();

    // //译码过程中decode table查表，若未查询到匹配项，赋予INVALID_INSTR
    // goto_priv(PRIV_M);
    // invalid_instruction();
    // TEST_SETUP_EXCEPT();
    // TEST_ASSERT("decode do not match leads to illegal instruction interrupt",
    //     excpt.triggered == true &&
    //     excpt.cause == CAUSE_ILI
    // ); 

    //访问有效，CSRoptype非跳转行为，且访问CSR地址不在CSR mapping地址范围内
    
    // CSRR(CSR_INVALID);
    // TEST_SETUP_EXCEPT();
    // TEST_ASSERT("CSR addr not match the csr mapping leads to illegal instruction interrupt",
    //     excpt.triggered == true &&
    //     excpt.cause == CAUSE_ILI
    // ); 
    // printf("%d",excpt.triggered);
    // printf("%d",excpt.cause);

    //当前权限满足最低CSR访问权限，CSR写行为，但访问的CSR只读
    goto_priv(PRIV_M);
    CSRW(CSR_MCONFIGPTR,0xfff);
    TEST_ASSERT("write csr which is read-only leads to illegal instruction interrupt",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI 
    ); 


    //当前权限不满足最低CSR访问权限
    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_MIE);
    TEST_ASSERT("low priviliege mode access high priviliege csr leads to illegal instruction interrupt",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 

    TEST_END();

}

bool illegal_except_2() {

    TEST_START();


    //特殊情况，没有开启虚存，在S模式下访问Satp寄存器，但被sstatus.TVM拦截
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS, MSTATUS_TVM);
    CSRW(satp, 0x0);    //关闭虚存
    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();
    CSRR(satp);
    TEST_ASSERT("The Satp register was accessed in S mode without virtual memory enabled leads to ili when tvm=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 

    TEST_END();

}

bool illegal_except_3() {

    TEST_START();

    TEST_SETUP_EXCEPT();

    //执行CSR写指令尝试将非支持值写入WLRL字段时
    printf("%llx\n",CSRR(CSR_MCAUSE));
    CSRW(CSR_MCAUSE,0xfffffffff);     //不支持的值写入将不会写入，没报错
    printf("%llx\n",CSRR(CSR_MCAUSE));
    TEST_ASSERT("write wlrl part of csr leads to illegal instruction interrupt",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 


    TEST_END();
}



bool illegal_except_4() {

    TEST_START();

    //M模式下可以访问任何CSR
    goto_priv(PRIV_M);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_MCAUSE);
    CSRR(CSR_SCAUSE);
    CSRR(CSR_VSCAUSE);
    TEST_ASSERT("m mode access any csr successful",
        excpt.triggered == false
    ); 

    reset_state();

    //尝试访问高特权级别的任何CSR
    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_MCAUSE);
    TEST_ASSERT("hs mode access higher privilege csr leads to ili",
        excpt.triggered == true &&
        excpt.cause==CAUSE_ILI
    ); 

    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_MIDELEG);
    TEST_ASSERT("vs mode access higher privilege csr leads to ili",
        excpt.triggered == true &&
        excpt.cause==CAUSE_ILI
    );

    TEST_END();
}

//当寄存器为32位时才做该测试
/*
    //当V=1且XLEN>32时，尝试访问高半部分监管级CSR
    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    CSRS(CSR_SSTATUS,1ULL << 35);
    TEST_ASSERT("When V=1 and XLEN>32, try to access the higher half of the regulatory CSR results in an illegal instruction exception",
        excpt.triggered == true &&
        excpt.cause==CAUSE_ILI
    );

    //当V=1且XLEN>32时，尝试访问高半部分虚拟化扩展CSR
    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    CSRS(CSR_HSTATUS,1ULL << 35);
    TEST_ASSERT("When V=1 and XLEN>32, try to access the higher half of the Virtualization extension CSR results in an illegal instruction exception",
        excpt.triggered == true &&
        excpt.cause==CAUSE_ILI
    );

    //当V=1且XLEN>32时，尝试访问高半部分VS CSR
    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    CSRS(CSR_VSSTATUS,1ULL << 35);
    TEST_ASSERT("When V=1 and XLEN>32, try to access the higher half of the vs CSR results in an illegal instruction exception",
        excpt.triggered == true &&
        excpt.cause==CAUSE_ILI
    );

    //当V=1且XLEN>32时，尝试访问高半部分非特权CSR
    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    CSRS(CSR_CYCLEH,1ULL << 35);
    TEST_ASSERT("When V=1 and XLEN>32, try to access the higher half of the non-privileged CSR results in an illegal instruction exception",
        excpt.triggered == true &&
        excpt.cause==CAUSE_ILI
    );
*/


bool illegal_except_5() {

    TEST_START();
    //TVM=1，在S模式下，读写satp 寄存器
    goto_priv(PRIV_M);
    CSRW(satp, 0x0);
    CSRS(CSR_MSTATUS, MSTATUS_TVM);
    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();
    CSRR(satp);
    TEST_ASSERT("The Satp register was accessed in S mode without virtual memory enabled leads to ili when tvm=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 

    TEST_END();
}

bool illegal_except_6() {

    TEST_START();
    //当mstatus.TVM=1时，在HS模式下执行时尝试读取或写入hgatp
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS, MSTATUS_TVM);
    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();
    CSRW(CSR_HGATP,(uint64_t)-1);         //没报错
    TEST_ASSERT("The hgatp register was accessed in hS mode without virtual memory enabled leads to ili when tvm=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 

    TEST_END();
}

bool illegal_except_7() {

    TEST_START();

    //TVM=1，在S模式下，执行SFENCE.VMA或SINVAL.VMA指令
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS,MSTATUS_TVM);
    CSRS(CSR_HSTATUS,HSTATUS_VTVM);

    goto_priv(PRIV_HS); 
    TEST_SETUP_EXCEPT();
    sfence_vma();
    TEST_ASSERT("s mode sfence.vma leads to ili when tvm=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 

    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();
    Sinval_vma();
    TEST_ASSERT("s mode sinval.vma leads to ili when tvm=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 

    TEST_END();
}


bool illegal_except_8() {

    TEST_START();
    //M模式下（无论mstatus.TVM的值如何）执行HFENCE.GVMA
    goto_priv(PRIV_M);
    TEST_SETUP_EXCEPT();
    hfence_gvma();
    TEST_ASSERT("m mode hfence.gvma successful",
        excpt.triggered == false
    ); 

    TEST_END();
}

bool illegal_except_9() {

    TEST_START();
    //HS模式下mstatus.TVM=0时执行HFENCE.GVMA
    goto_priv(PRIV_M);
    CSRC(CSR_MSTATUS,MSTATUS_TVM);
    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();
    hfence_gvma();
    TEST_ASSERT("hs mode hfence.gvma leads to ili when mstatus.tvm=0",
        excpt.triggered == false
    ); 

    //mstatus.TVM=1时，在HS模式下执行HFENCE.GVMA
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS,MSTATUS_TVM);
    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();
    hfence_gvma();
    TEST_ASSERT("hs mode hfence.gvma successful when mstatus.tvm=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 

    TEST_END();
}

bool illegal_except_10() {

    TEST_START();
    
    //mstatus.TVM=1，尝试在S模式或HS模式下执行SINVAL.VMA或HINVAL.GVMA
    goto_priv(PRIV_M);
    reset_state();
    CSRS(CSR_MSTATUS,MSTATUS_TVM);
    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();
    Sinval_vma();
    TEST_ASSERT("hs mode sinval.vma successful when mstatus.tvm=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 

    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS,MSTATUS_TVM);
    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();
    hinval_gvma();
    TEST_ASSERT("hs mode hinval.gvma successful when mstatus.tvm=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 

    TEST_END();
}

bool illegal_except_11() {

    TEST_START();

    //U模式下，执行HFENCE.VVMA或HFENCE.GVMA
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    reset_state();
    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();
    hfence_vvma();
    TEST_ASSERT("hu mode hfence.vvma cause to illegal instruction interrupt",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 

    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    reset_state();
    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();
    hfence_gvma();
    TEST_ASSERT("hu mode hfence.gvma cause to illegal instruction interrupt",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 

    TEST_END();
}

bool illegal_except_12() {

    TEST_START();

    //在U模式下执行SINVAL.VMA、HINVAL.VVMA和HINVAL.GVMA中的任何一条
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    reset_state();
    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();
    Sinval_vma();
    TEST_ASSERT("hu mode sinval.vma cause to illegal instruction interrupt",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 

    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    reset_state();
    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();
    hinval_gvma();
    TEST_ASSERT("hu mode hinval.gvma cause to illegal instruction interrupt",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 

    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    reset_state();
    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();
    hinval_vvma();
    TEST_ASSERT("hu mode hinval.vvma cause to illegal instruction interrupt",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 

    TEST_END();
}


bool illegal_except_13() {

    TEST_START();
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

    TEST_END();
}




    // uintptr_t vaddr_f = hs_page_base(VSI_GI);  


    // //当V=1时，设置vsstatus.FS=0，HS级别的sstatus.FS=1时，尝试执行浮点指令（vsstatus.FS和HS级别的sstatus.FS同时生效才可以执行浮点指令）
    // goto_priv(PRIV_M);
    // TEST_SETUP_EXCEPT();
    // CSRS(CSR_MISA,1ULL << 7);        //开启h扩展(v=1)
    // CSRS(CSR_MISA,1ULL << 5 | 1ULL << 3);        //开启float和double
    // CSRC(CSR_VSSTATUS, (1ULL << 13) | (1ULL << 14));
    // CSRS(CSR_SSTATUS, (1ULL << 13) | (1ULL << 14));
    // flw(vaddr_f);
    // TEST_ASSERT("Execute floating-point instruction leads to ili when v=1 and vsstatus.fs=0 and sstatus.fs=1",
    //     excpt.triggered == true &&
    //     excpt.cause == CAUSE_ILI
    // ); 


    //当V=1时，设置vsstatus.FS=1，HS级别的sstatus.FS=0时，尝试执行浮点指令
    //当V=1时，设置vsstatus.FS=0，HS级别的sstatus.FS=0时，尝试执行浮点指令



    // //当V=1时，设置vsstatus.VS=0，HS级别的sstatus.VS=1时，尝试执行向量指令（vsstatus.VS和HS级别的sstatus.VS同时生效才可以执行向量指令）
    // goto_priv(PRIV_M);
    // TEST_SETUP_EXCEPT();
    // CSRS(CSR_MISA,1ULL << 7);        //开启h扩展(v=1)
    // CSRS(CSR_MISA,1ULL << 5 | 1ULL << 3);        //开启float和double
    // CSRC(CSR_VSSTATUS, (1ULL << 9) | (1ULL << 10));
    // CSRS(CSR_SSTATUS, (1ULL << 9) | (1ULL << 10));
    // vle32_v();
    // TEST_ASSERT("Execution vector instruction leads to ili when v=1 and vsstatus.fs=0 and sstatus.fs=1",
    //     excpt.triggered == true &&
    //     excpt.cause == CAUSE_ILI
    // ); 


    //当V=1时，设置vsstatus.VS=1，HS级别的sstatus.VS=0时，尝试执行向量指令（vsstatus.VS和HS级别的sstatus.VS同时生效才可以执行向量指令）
    //当V=1时，设置vsstatus.VS=0，HS级别的sstatus.VS=0时，尝试执行向量指令（vsstatus.VS和HS级别的sstatus.VS同时生效才可以执行向量指令）
    //扩展FS的状态设置为0时，尝试读取或者写入浮点对应的状态指令
    //扩展VS的状态设置为0时，尝试读取或者写入向量对应的状态指令
    //扩展XS的状态设置为0时，尝试读取或者写入其他扩展对应的状态指令
    //设置HU=0时，在U模式执行超级虚拟机指令

bool illegal_except_14() {

    TEST_START();
    //mstatus.TSR=1时，执行sret指令
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS,1ULL << 22);    //TSR位

    goto_priv(PRIV_HS);
    set_prev_priv(PRIV_VU);

    //TEST_EXEC_SRET();
    sret();

    TEST_ASSERT("hs mode sret cause to ili when mstatus.TSR=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 

    TEST_END();

}

bool illegal_except_15() {

    TEST_START();
    //mstatus.TSR=1时，执行sret指令
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRC(CSR_MSTATUS,1ULL << 22);    //TSR位

    goto_priv(PRIV_HS);
    set_prev_priv(PRIV_VS);

    TEST_EXEC_SRET();


    TEST_ASSERT("hs mode sret successful when mstatus.TSR=0",
        excpt.triggered == false
    ); 

    TEST_END();

}



    // //menvcfg.STCE=0,尝试在非M模式下访问stimecmp或vstimecmp
    // TEST_SETUP_EXCEPT();
    // goto_priv(PRIV_M);




