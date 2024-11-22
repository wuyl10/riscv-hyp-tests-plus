#include <rvh_test.h>

bool vec_vle_vse_func(){
    TEST_START();

    CSRS(CSR_MSTATUS, 1ULL << 9);      //设置mstatus.vs=1

    uint8_t src8[8] = {1, 2, 3, 4, 5, 6, 7, 8}; 
    uint8_t dest8[8] = {0}; 

    int vl = 8;  // 向量长度

    // 调用向量加载和存储函数
    vle8_to_v6(src8, vl);
    excpt_info();
    vse8_from_v6(dest8, vl);

    
    // 打印结果以验证
    printf("After vle8_v:\n");
    for (int i = 0; i < 8; i++) {
        printf("dest8[%d] = %u  ", i, dest8[i]);
    }
    printf("\n");


    TEST_END();
}



bool vec_test_1(){
    
    TEST_START();
    // 当 mstatus.VS 被设置为 Off 时，尝试执行向量指令

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

    excpt_info();
    TEST_ASSERT("An attempt to execute vector instructions cause to ILI when mstatus.vs=0",
        excpt.triggered == true &&  
        excpt.cause == CAUSE_ILI
    );   
    
    TEST_END();
}

bool vec_test_2(){

    TEST_START();
    // 当 mstatus.VS 被设置为 Off 时，尝试访问向量 CSR 

    CSRC(CSR_MSTATUS, 1ULL << 9);      //设置mstatus.vs=0
    CSRC(CSR_MSTATUS, 1ULL << 10); 

    TEST_SETUP_EXCEPT();
    CSRR(CSR_VSTART);
    excpt_info();

    TEST_ASSERT("An attempt to access vector register cause to ILI when mstatus.vs=0",
        excpt.triggered == true &&  
        excpt.cause == CAUSE_ILI
    );   
    
    TEST_END();
}


bool vec_test_3(){             

    TEST_START();
    // 当 V=1 时，vsstatus.VS=OFF && mstatus.VS!=OFF 时，尝试执行向量指令

    CSRS(CSR_MSTATUS, 1ULL << 9);      //设置mstatus.vs != 0
    CSRS(CSR_MSTATUS, 1ULL << 10); 

    int sew = 2;         // SEW = 2 表示 32 位元素
    int lmull = 1;       // LMUL = 1
    int vl = 2;          // 需要处理的向量长度（处理 2 个元素）
    int vs1_init = 0;    // 初始化 v4（累加初始值）为 0
    int vs2_init = 1;    // 初始化 v6（源数据）为 1

    // 设置 vredsum.vs 的执行条件
    set_vredsum_vs_conditions(sew, lmull, vl, vs1_init, vs2_init);

    CSRS(CSR_MSTATUS, 1ULL << 9);      //设置mstatus.vs!=0
    CSRC(CSR_MSTATUS, 1ULL << 10); 
    CSRC(CSR_VSSTATUS, 1ULL << 9);      //设置vsstatus.vs=0
    CSRC(CSR_VSSTATUS, 1ULL << 10); 

    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    execute_vredsum_vs();           // 执行 vredsum.vs 指令并获取结果
    excpt_info();

    TEST_ASSERT("An attempt to execute vector instructions cause to ILI when mstatus.vs!=0 vsstatus.vs=0 and v=1",
        excpt.triggered == true &&  
        excpt.cause == CAUSE_ILI
    );   
    
    TEST_END();
}


bool vec_test_4(){

    TEST_START();
    // 当 V=1 时，vsstatus.VS=OFF && mstatus.VS!=OFF 时，尝试访问向量 CSR 

    CSRS(CSR_MSTATUS, 1ULL << 9);      //设置mstatus.vs=!0
    CSRS(CSR_MSTATUS, 1ULL << 10); 
    CSRC(CSR_VSSTATUS, 1ULL << 9);      //设置vsstatus.vs=0
    CSRC(CSR_VSSTATUS, 1ULL << 10); 

    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_VTYPE);
    excpt_info();

    TEST_ASSERT("An attempt to access vector register cause to ILI when mstatus.vs=!0 vsstatus.vs=0 and v=1",
        excpt.triggered == true &&  
        excpt.cause == CAUSE_ILI
    );   
    
    TEST_END();
}

bool vec_test_5(){          

    TEST_START();
    // 当 V=1 时，vsstatus.VS!=OFF && mstatus.VS=OFF 时，尝试执行向量指令

    CSRS(CSR_MSTATUS, 1ULL << 9);      //设置mstatus.vs != 0
    CSRS(CSR_MSTATUS, 1ULL << 10); 

    int sew = 2;       // SEW = 2 表示 32 位元素
    int lmull = 1;     // LMUL = 1
    int vl = 2;        // 向量长度（处理 2 个元素）
    int v0_init = 1;   // 初始化 v0 的值为 1
    int v4_init = 2;   // 初始化 v4 的值为 2

    TEST_SETUP_EXCEPT();

    // 设置 vfirst.m 的执行条件
    set_vfirst_m_conditions(vl, sew, lmull, v0_init, v4_init);
    excpt_info();

    CSRC(CSR_MSTATUS, 1ULL << 9);      //设置mstatus.vs=0
    CSRC(CSR_MSTATUS, 1ULL << 10); 
    CSRS(CSR_VSSTATUS, 1ULL << 9);      //设置vsstatus.vs!=0
    CSRC(CSR_VSSTATUS, 1ULL << 10); 

    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    EXECUTE_VFIRST_M_V3();
    excpt_info();

    TEST_ASSERT("An attempt to execute vector instructions cause to ILI when mstatus.vs=0 vsstatus.vs!=0 and v=1",
        excpt.triggered == true &&  
        excpt.cause == CAUSE_ILI
    );   
    
    TEST_END();
}


bool vec_test_6(){

    TEST_START();
    // 当 V=1 时，vsstatus.VS!=OFF && mstatus.VS=OFF 时，尝试访问向量 CSR 

    CSRC(CSR_MSTATUS, 1ULL << 9);      //设置mstatus.vs=0
    CSRC(CSR_MSTATUS, 1ULL << 10); 
    CSRS(CSR_VSSTATUS, 1ULL << 9);      //设置vsstatus.vs!=0
    CSRS(CSR_VSSTATUS, 1ULL << 10); 

    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_VXSAT);
    excpt_info();

    TEST_ASSERT("An attempt to access vector register cause to ILI when mstatus.vs=0 vsstatus.vs=!0 and v=1",
        excpt.triggered == true &&  
        excpt.cause == CAUSE_ILI
    );   
    
    TEST_END();
}

bool vec_test_7(){

    TEST_START();
    // 当 V=1 时，vsstatus.VS=OFF && mstatus.VS=OFF 时，尝试执行向量指令

    CSRS(CSR_MSTATUS, 1ULL << 9);      //设置mstatus.vs != 0
    CSRS(CSR_MSTATUS, 1ULL << 10); 
    int vl = 8;        // 需要处理的向量长度为 8
    int sew = 2;       // SEW = 2 对应 32 位
    int lmull = 1;     // LMUL = 1
    int v0_init = 1;   // 初始化掩码寄存器 v0 的值为 1（全 1）
    int v4_init = 0;   // 初始化源寄存器 v4 的值为 0

    // 设置vmsbf.m执行条件
    set_vmsbfm_conditions(vl, sew, lmull, v0_init, v4_init);

    CSRC(CSR_MSTATUS, 1ULL << 9);      //设置mstatus.vs=0
    CSRC(CSR_MSTATUS, 1ULL << 10); 
    CSRC(CSR_VSSTATUS, 1ULL << 9);      //设置vsstatus.vs=0
    CSRC(CSR_VSSTATUS, 1ULL << 10); 

    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();


    // 执行 vmsbf.m 指令
    execute_vmsbf_m();
    excpt_info();

    TEST_ASSERT("An attempt to execute vector instructions cause to ILI when mstatus.vs=0 vsstatus.vs=0 and v=1",
        excpt.triggered == true &&  
        excpt.cause == CAUSE_ILI
    );   
    
    TEST_END();
}


bool vec_test_8(){

    TEST_START();
    // 当 V=1 时，vsstatus.VS=OFF && mstatus.VS=OFF 时，尝试访问向量 CSR 

    CSRC(CSR_MSTATUS, 1ULL << 9);      //设置mstatus.vs=0
    CSRC(CSR_MSTATUS, 1ULL << 10); 
    CSRC(CSR_VSSTATUS, 1ULL << 9);      //设置vsstatus.vs=0
    CSRC(CSR_VSSTATUS, 1ULL << 10); 

    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_VXRM);
    excpt_info();

    TEST_ASSERT("An attempt to access vector register cause to ILI when mstatus.vs=0 vsstatus.vs=0 and v=1",
        excpt.triggered == true &&  
        excpt.cause == CAUSE_ILI
    );   
    
    TEST_END();
}


bool vec_test_9(){

    TEST_START();
    // 如果vstart超出界限，建议实现陷阱
    /*
    向量配置的最大长度 VLMAX。这个值取决于向量长度寄存器（vl）、元素宽度（SEW）、和向量组乘数（LMUL）。
    (该测试用例只是简单弄个大的数来测试)
    */
    CSRS(CSR_MSTATUS, 1ULL << 9);      //设置mstatus.vs!=0
    CSRC(CSR_MSTATUS, 1ULL << 10);      

    // 设置 vadd 执行条件
    int sew = 2;        // SEW = 2 表示 32 位元素
    int lmull = 1;      // LMUL = 1
    int vl = 8;         // 向量长度（处理 8 个元素）
    int v0_init = 1;    // 初始化 v0 掩码寄存器的值为 1（全1）
    int v4_init = 3;    // 初始化 v4 的值为 3
    int v6_init = 5;    // 初始化 v6 的值为 5

    // 设置 vadd 的执行条件
    set_vadd_conditions(sew, lmull, vl, v0_init, v4_init, v6_init);

    CSRW(CSR_VSTART,(uint64_t)-1);
    printf("vstart=%llx\n",CSRR(CSR_VSTART));
    TEST_SETUP_EXCEPT();
    
    // 执行 vadd.vv 指令
    execute_vadd_vv();

    excpt_info();

    TEST_ASSERT("An attempt to execute vadd.vv cause to ILI when VSTART!=0",
        excpt.triggered == true &&  
        excpt.cause == CAUSE_ILI
    );   
    
    TEST_END();
}


bool vec_test_10(){

    TEST_START();
    //当尝试执行某个向量指令时，如果 vstart 的值是该实现在使用相同 vtype 设置执行该指令时不可能产生的


    TEST_END();

}


bool vec_test_11(){

    TEST_START();
    //如果 vill 位被设置位1，执行依赖 vtype 的向量指令

    CSRS(CSR_MSTATUS, 1ULL << 9);      //设置mstatus.vs != 0
    CSRS(CSR_MSTATUS, 1ULL << 10); 


    int vl = 8;        // 需要处理的向量长度为 8
    int sew = 2;       // SEW = 2 对应 32 位
    int vill = 1;       // SEW = 2 对应 32 位
    int lmull = 1;     // LMUL = 1
    int v0_init = 1;   // 初始化掩码寄存器 v0 的值为 1（全 1）
    int v4_init = 3;   // 初始化源寄存器 v4 的值为 0

    // 设置vmsbf.m执行条件
    printf("vtype=%llx\n",CSRR(CSR_VTYPE));

    TEST_SETUP_EXCEPT();
    execute_vmsbf_m(); 
    excpt_info();
    
    TEST_ASSERT("An attempt to execute vmsbf.m cause to ILI when vtype.vill=1",
        excpt.triggered == true &&  
        excpt.cause == CAUSE_ILI
    );   

    TEST_END();

}

bool vec_test_12(){

    TEST_START();
    //实现必须提供与所有受支持的 SEW 设置对应的 EEW 向量加载和存储。设置 EEW 宽度不受支持，执行向量加载/存储编码
    CSRS(CSR_MSTATUS, 1ULL << 9);      //设置mstatus.vs != 0
    CSRC(CSR_MSTATUS, 1ULL << 10); 

    // set_vtype_with_invalid_eew();       //环境不支持设置



    TEST_END();

}

bool vec_test_13(){

    TEST_START();
    //设置不支持的 EEW 值，执行 vl<nf>r 指令

    TEST_END();

}



bool vec_test_14(){

    TEST_START();
    //浮点单元状态字段 mstatus.FS 被设置为 Off，任何尝试执行向量浮点指令
    CSRS(CSR_MSTATUS, 1ULL << 9);      //设置mstatus.vs != 0
    CSRC(CSR_MSTATUS, 1ULL << 10); 

    CSRS(CSR_MSTATUS, 1ULL << 13);      //设置mstatus.fs != 0
    CSRC(CSR_MSTATUS, 1ULL << 14); 

    int sew = 2;        // SEW = 2 表示 32 位元素
    int lmull = 1;      // LMUL = 1
    int vl = 8;         // 向量长度（处理 8 个元素）
    float v0_init = 1.0; // 初始化 v0 的值为 1.0（全1）
    float v4_init = 3.0; // 初始化 v4 的值为 3.0
    float v6_init = 2.0; // 初始化 v6 的值为 2.0

    // 设置 vfadd 执行条件
    set_vfadd_conditions(sew, lmull, vl, v0_init, v4_init, v6_init);

    CSRC(CSR_MSTATUS, 1ULL << 13);      //设置mstatus.fs = 0
    CSRC(CSR_MSTATUS, 1ULL << 14); 

    TEST_SETUP_EXCEPT();
    // 执行 vfadd 指令
    execute_vfadd();

    TEST_ASSERT("An attempt to execute vfadd cause to ILI when mstatus.fs=0",
        excpt.triggered == true &&  
        excpt.cause == CAUSE_ILI
    );   

    TEST_END();

}


bool vec_test_15(){

    TEST_START();
    //如果实现了虚拟化扩展且 V=1，如果 vsstatus.FS 或 mstatus.FS 被设置为 Off，任何尝试执行向量浮点指令的操作
    CSRS(CSR_MSTATUS, 1ULL << 9);      //设置mstatus.vs != 0
    CSRC(CSR_MSTATUS, 1ULL << 10); 

    CSRS(CSR_MSTATUS, 1ULL << 13);      //设置mstatus.fs != 0
    CSRC(CSR_MSTATUS, 1ULL << 14); 

    int sew = 2;        // SEW = 2 表示 32 位元素
    int lmull = 1;      // LMUL = 1
    int vl = 8;         // 向量长度（处理 8 个元素）
    float v0_init = 1.0; // 初始化 v0 的值为 1.0（全1）
    float v4_init = 3.0; // 初始化 v4 的值为 3.0
    float v6_init = 2.0; // 初始化 v6 的值为 2.0

    // 设置 vfadd 执行条件
    set_vfadd_conditions(sew, lmull, vl, v0_init, v4_init, v6_init);

    CSRS(CSR_MSTATUS, 1ULL << 13);      //设置mstatus.fs != 0
    CSRS(CSR_MSTATUS, 1ULL << 14); 
    CSRC(CSR_VSSTATUS, 1ULL << 13);      //设置vsstatus.fs = 0
    CSRC(CSR_VSSTATUS, 1ULL << 14); 

    TEST_SETUP_EXCEPT();
    // 执行 vfadd 指令
    goto_priv(PRIV_VU);
    execute_vfadd();
    excpt_info();
    TEST_ASSERT("An attempt to execute vfadd cause to ILI when mstatus.fs=0 or vsstatus.fs=0",
        excpt.triggered == true &&  
        excpt.cause == CAUSE_ILI
    );   

    TEST_END();

}






bool vec_test_16(){

    TEST_START();
    //如果 vstart 非零，执行vector reduction instructions操作


    CSRS(CSR_MSTATUS, 1ULL << 9);      //设置mstatus.vs != 0
    CSRS(CSR_MSTATUS, 1ULL << 10); 

    int sew = 2;         // SEW = 2 表示 32 位元素
    int lmull = 1;       // LMUL = 1
    int vl = 2;          // 需要处理的向量长度（处理 2 个元素）
    int vs1_init = 0;    // 初始化 v4（累加初始值）为 0
    int vs2_init = 1;    // 初始化 v6（源数据）为 1


    // 设置 vredsum.vs 的执行条件
    set_vredsum_vs_conditions(sew, lmull, vl, vs1_init, vs2_init);
    excpt_info();

    CSRW(CSR_VSTART,(uint64_t)-1)       //设置vstart非零
    printf("vstart=%llx\n",CSRR(CSR_VSTART));

    TEST_SETUP_EXCEPT();
    execute_vredsum_vs();           // 执行 vredsum.vs 指令并获取结果
    excpt_info();

    TEST_ASSERT("An attempt to execute vredsum.vs cause to ILI when vstart!=0",
        excpt.triggered == true &&  
        excpt.cause == CAUSE_ILI
    );   

    TEST_END();

}



bool vec_test_17(){

    TEST_START();
    //如果 vstart 非零，执行vcpop.m指令


    CSRS(CSR_MSTATUS, 1ULL << 9);      //设置mstatus.vs != 0
    CSRS(CSR_MSTATUS, 1ULL << 10); 



    int vl = 8;          // 向量长度
    int sew = 2;         // SEW = 32 位
    int lmull = 1;       // LMUL = 1
    int v0_init = 1;     // 初始化掩码寄存器 v0 的值为全 1
    int v2_init = 1;     // 初始化 v2 的值为全 1

    // 设置 vcpop.m 的执行条件
    set_vcpop_conditions(vl, sew, lmull, v0_init, v2_init);

    CSRW(CSR_VSTART,(uint64_t)-1)       //设置vstart非零
    printf("vstart=%llx\n",CSRR(CSR_VSTART));

    // 执行 vcpop.m 指令并返回结果
    TEST_SETUP_EXCEPT();
    uint32_t result = execute_vcpop_v2();

    excpt_info();

    TEST_ASSERT("An attempt to execute vcpop.m cause to ILI when vstart!=0",
        excpt.triggered == true &&  
        excpt.cause == CAUSE_ILI
    );   

    TEST_END();

}



bool vec_test_18(){

    TEST_START();
    //如果 vstart 非零，执行vfirst指令


    CSRS(CSR_MSTATUS, 1ULL << 9);      //设置mstatus.vs != 0
    CSRS(CSR_MSTATUS, 1ULL << 10); 

    int sew = 2;       // SEW = 2 表示 32 位元素
    int lmull = 1;     // LMUL = 1
    int vl = 2;        // 向量长度（处理 2 个元素）
    int v0_init = 1;   // 初始化 v0 的值为 1
    int v4_init = 2;   // 初始化 v4 的值为 2

    TEST_SETUP_EXCEPT();

    // 设置 vfirst.m 的执行条件
    set_vfirst_m_conditions(vl, sew, lmull, v0_init, v4_init);
    excpt_info();

    CSRW(CSR_VSTART,(uint64_t)-1)       //设置vstart非零
    printf("vstart=%llx\n",CSRR(CSR_VSTART));

    TEST_SETUP_EXCEPT();
    EXECUTE_VFIRST_M_V3();
    excpt_info();


    TEST_ASSERT("An attempt to execute vfirst.m cause to ILI when vstart!=0",
        excpt.triggered == true &&  
        excpt.cause == CAUSE_ILI
    );   

    TEST_END();

}

bool vec_test_19(){

    TEST_START();
    //如果 vstart 非零，执行vmsbf指令


    CSRS(CSR_MSTATUS, 1ULL << 9);      //设置mstatus.vs != 0
    CSRS(CSR_MSTATUS, 1ULL << 10); 

    int vl = 8;        // 需要处理的向量长度为 8
    int sew = 2;       // SEW = 2 对应 32 位
    int lmull = 1;     // LMUL = 1
    int v0_init = 1;   // 初始化掩码寄存器 v0 的值为 1（全 1）
    int v4_init = 0;   // 初始化源寄存器 v4 的值为 0

    // 设置vmsbf.m执行条件
    set_vmsbfm_conditions(vl, sew, lmull, v0_init, v4_init);

    CSRW(CSR_VSTART,(uint64_t)-1)       //设置vstart非零
    printf("vstart=%llx\n",CSRR(CSR_VSTART));

    TEST_SETUP_EXCEPT();
    // 执行 vmsbf.m 指令
    execute_vmsbf_m();
    excpt_info();

    TEST_ASSERT("An attempt to execute vmsbf.m cause to ILI when vstart!=0",
        excpt.triggered == true &&  
        excpt.cause == CAUSE_ILI
    );   

    TEST_END();

}

bool vec_test_20(){

    TEST_START();
    //如果 vstart 非零，执行vmsif指令

    CSRS(CSR_MSTATUS, 1ULL << 9);      //设置mstatus.vs != 0
    CSRS(CSR_MSTATUS, 1ULL << 10); 

    int sew = 2;        // SEW = 2 表示 32 位元素
    int lmull = 1;      // LMUL = 1
    int vl = 8;         // 向量长度（处理 8 个元素）
    int v0_init = 1;    // 初始化 v0 的值为 1（全1）
    int v4_init = 3;    // 初始化 v4 的值为 3

    // 设置 vmsif.m 的执行条件
    set_vmsifm_conditions(vl, sew, lmull, v0_init, v4_init);

    CSRW(CSR_VSTART,(uint64_t)-1)       //设置vstart非零
    printf("vstart=%llx\n",CSRR(CSR_VSTART));

    // 执行 vmsif.m 指令
    TEST_SETUP_EXCEPT();
    execute_vmsif_m();
    excpt_info();
    TEST_ASSERT("An attempt to execute vmsif.m cause to ILI when vstart!=0",
        excpt.triggered == true &&  
        excpt.cause == CAUSE_ILI
    );   

    TEST_END();

}

bool vec_test_21(){

    TEST_START();
    //如果 vstart 非零，执行vmsof指令


    CSRS(CSR_MSTATUS, 1ULL << 9);      //设置mstatus.vs != 0
    CSRS(CSR_MSTATUS, 1ULL << 10); 

    int sew = 2;        // SEW = 2 表示 32 位元素
    int lmull = 1;      // LMUL = 1
    int vl = 8;         // 向量长度（处理 8 个元素）
    int v0_init = 1;    // 初始化 v0 的值为 1（全1）
    int v4_init = 3;    // 初始化 v4 的值为 3

    // 设置 vmsof.m 的执行条件
    set_vmsofm_conditions(vl, sew, lmull, v0_init, v4_init);

    CSRW(CSR_VSTART,(uint64_t)-1)       //设置vstart非零
    printf("vstart=%llx\n",CSRR(CSR_VSTART));

    // 执行 vmsof.m 指令
    TEST_SETUP_EXCEPT();
    execute_vmsof_m();
    excpt_info();

    TEST_ASSERT("An attempt to execute vmsif.m cause to ILI when vstart!=0",
        excpt.triggered == true &&  
        excpt.cause == CAUSE_ILI
    );   

    TEST_END();

}

bool vec_test_22(){

    TEST_START();
    //如果 vstart 非零，执行viota.m 指令


    CSRS(CSR_MSTATUS, 1ULL << 9);      //设置mstatus.vs != 0
    CSRS(CSR_MSTATUS, 1ULL << 10); 

    int sew = 2;        // SEW = 2 表示 32 位元素
    int lmull = 1;      // LMUL = 1
    int vl = 8;         // 向量长度（处理 8 个元素）
    int v0_init = 1;    // 初始化 v0 的值为 1（全1）

    // 设置 viota.m 的执行条件
    set_viota_m_conditions(vl, sew, lmull, v0_init);

    CSRW(CSR_VSTART,(uint64_t)-1)       //设置vstart非零
    printf("vstart=%llx\n",CSRR(CSR_VSTART));

    TEST_SETUP_EXCEPT();
    // 执行 viota.m 指令
    execute_viota_m();

    excpt_info();



    TEST_ASSERT("An attempt to execute viota.m cause to ILI when vstart!=0",
        excpt.triggered == true &&  
        excpt.cause == CAUSE_ILI
    );   

    TEST_END();

}



bool vec_test_23(){

    TEST_START();
    //如果 vstart 非零，执行vcompress 指令


    CSRS(CSR_MSTATUS, 1ULL << 9);      //设置mstatus.vs != 0
    CSRS(CSR_MSTATUS, 1ULL << 10); 

    int sew = 2;        // SEW = 2 表示 32 位元素
    int lmull = 1;      // LMUL = 1
    int vl = 8;         // 向量长度（处理 8 个元素）
    int v0_init = 1;    // 初始化 v0 的值为 1（全1）
    int v4_init = 3;    // 初始化 v4 的值为 3
    int v8_init = 0;    // 初始化 v8 的值为 0

    // 设置 vcompress 的执行条件
    set_vcompress_conditions(vl, sew, lmull, v0_init, v4_init, v8_init);

    CSRW(CSR_VSTART,(uint64_t)-1)       //设置vstart非零
    printf("vstart=%llx\n",CSRR(CSR_VSTART));

    TEST_SETUP_EXCEPT();
    // 执行 vcompress 指令
    execute_vcompress();
    excpt_info();

    TEST_ASSERT("An attempt to execute vcompress cause to ILI when vstart!=0",
        excpt.triggered == true &&  
        excpt.cause == CAUSE_ILI
    );   

    TEST_END();

}



bool vec_test_24(){

    TEST_START();
    //当执行一个向量指令时，如设置 EGS > VLMAX


    CSRS(CSR_MSTATUS, 1ULL << 9);      //设置mstatus.vs != 0
    CSRS(CSR_MSTATUS, 1ULL << 10); 


    int sew = 8;        // SEW = 8 表示 8 位元素
    int lmull = 16;     // LMUL = 16 表示寄存器组倍数较大，以确保 EGS > VLMAX
    int vl = 32;       // 向量长

    // 计算 VLMAX
    int vlen = 128;     // 硬件的向量寄存器长度（128 位）
    int vlmax = (vlen / sew) * lmull;
    printf("Calculated VLMAX: %d elements\n", vlmax);

    // 设置 EGS > VLMAX 的执行条件
    // set_egs_greater_vlmax_conditions(vl, sew, lmull);

    excpt_info();

    TEST_ASSERT("An attempt to execute vcompress cause to ILI when vstart!=0",
        excpt.triggered == true &&  
        excpt.cause == CAUSE_ILI
    );   

    TEST_END();

}








