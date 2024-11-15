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
    uint32_t result = EXECUTE_VCPOP_V2();

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


bool vec_test_3(){              //条件没设置好！！

    TEST_START();
    // 当 V=1 时，vsstatus.VS=OFF && mstatus.VS!=OFF 时，尝试执行向量指令

    CSRS(CSR_MSTATUS, 1ULL << 9);      //设置mstatus.vs != 0
    CSRS(CSR_MSTATUS, 1ULL << 10); 

    int sew = 2;         // SEW = 2 表示 32 位元素
    int lmull = 1;       // LMUL = 1
    int vs1_init = 0;    // 初始化 v1（累加初始值）为 0
    int vs2_init = 1;    // 初始化 v2（源数据）为 1

    // 设置 vredsum.vs 的执行条件
    TEST_SETUP_EXCEPT();
    set_vredsum_vs_conditions(sew, lmull, vs1_init, vs2_init);


    excpt_info();

    CSRS(CSR_MSTATUS, 1ULL << 9);      //设置mstatus.vs!=0
    CSRC(CSR_MSTATUS, 1ULL << 10); 
    CSRC(CSR_VSSTATUS, 1ULL << 9);      //设置vsstatus.vs=0
    CSRC(CSR_VSSTATUS, 1ULL << 10); 

    goto_priv(PRIV_VS);
    // TEST_SETUP_EXCEPT();
    execute_vredsum_vs_v5();           // 执行 vredsum.vs 指令并获取结果
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
    // TEST_SETUP_EXCEPT();
    CSRR(CSR_VTYPE);
    excpt_info();

    TEST_ASSERT("An attempt to access vector register cause to ILI when mstatus.vs=!0 vsstatus.vs=0 and v=1",
        excpt.triggered == true &&  
        excpt.cause == CAUSE_ILI
    );   
    
    TEST_END();
}

bool vec_test_5(){          //条件设置有问题

    TEST_START();
    // 当 V=1 时，vsstatus.VS!=OFF && mstatus.VS=OFF 时，尝试执行向量指令

    CSRS(CSR_MSTATUS, 1ULL << 9);      //设置mstatus.vs != 0
    CSRS(CSR_MSTATUS, 1ULL << 10); 

    int vl = 8;          // 向量长度
    int sew = 2;         // SEW = 2 对应 32 位
    int lmull = 1;       // LMUL = 1
    int v0_init = 1;     // 初始化掩码寄存器 v0 的值为全 1
    int v3_init = 1;     // 初始化寄存器 v3 的值为 1（任意值，只是为了示例）

    // 设置 vfirst.m 的执行条件
    set_vfirst_conditions(vl, sew, lmull, v0_init, v3_init);


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
    int vl = 8;        // 假设需要处理的向量长度为 8
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
    VLMAX 是由以下公式确定的：VLMAX = LMUL * VLEN / SEW
    (该测试用例只是简单弄个大的数来测试)
    */
    CSRS(CSR_MSTATUS, 1ULL << 9);      //设置mstatus.vs=!0

    uint64_t vtype_value = CSRR(CSR_VTYPE);
    uint8_t vill = (vtype_value >> 63) & 0x1;
    uint8_t vsew = (vtype_value >> 3) & 0x7;
    uint8_t vlmul = vtype_value & 0x7;
    printf("%llx , %llx , %llx %llx \n",vill,vsew,vlmul,vtype_value);

    // TEST_SETUP_EXCEPT();
    
    // excpt_info();

    TEST_ASSERT("1",
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
    CSRS(CSR_MSTATUS, 1ULL << 9);      //设置mstatus.vs!=0
    TEST_SETUP_EXCEPT();
    // CSRC(CSR_VTYPE, 1ULL << 63);      //vill位

    TEST_SETUP_EXCEPT();
    execute_vredsum_vs_v5(); 
        excpt_info();

    TEST_END();

}


bool vec_test_88(){

    TEST_START();
    // 当 V=1 时，vsstatus.VS!=off && mstatus.VS!=OFF 时，尝试执行向量指令

    CSRS(CSR_MSTATUS, 1ULL << 9);      //设置mstatus.vs != 0
    CSRS(CSR_MSTATUS, 1ULL << 10); 
    int vl = 8;        // 假设需要处理的向量长度为 8
    int sew = 2;       // SEW = 2 对应 32 位
    int lmull = 1;     // LMUL = 1
    int v0_init = 1;   // 初始化掩码寄存器 v0 的值为 1（全 1）
    int v4_init = 0;   // 初始化源寄存器 v4 的值为 0

    // 设置vmsbf.m执行条件
    set_vmsbfm_conditions(vl, sew, lmull, v0_init, v4_init);

    CSRS(CSR_MSTATUS, 1ULL << 9);      //设置mstatus.vs=0
    CSRS(CSR_MSTATUS, 1ULL << 10); 
    CSRS(CSR_VSSTATUS, 1ULL << 9);      //设置vsstatus.vs=0
    CSRS(CSR_VSSTATUS, 1ULL << 10); 

    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();


    // 执行 vmsbf.m 指令
    execute_vmsbf_m();
    excpt_info();

    TEST_ASSERT("An attempt to execute vector instructions success when mstatus.vs!=0 vsstatus.vs!=0 and v=1",
        excpt.triggered == true &&  
        excpt.cause == CAUSE_ILI
    );   
    
    TEST_END();
}