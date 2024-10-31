#include <rvh_test.h>
#include <page_tables.h>




bool load_access_fault_1(){

    TEST_START();

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


    TEST_END();
}

bool load_access_fault_2(){

    TEST_START();

    goto_priv(PRIV_M);

    //pmpcfg.L被设置，当前特权级是HS，访问没有执行权限的PMP区域获取指令，pmpcfg.R=0

    CSRW(CSR_PMPCFG0,(uint64_t)0x0);

    CSRS(CSR_PMPCFG0,1ULL << 0 );      //pmp0cfg的R位
    CSRS(CSR_PMPCFG0,1ULL << 1 );      //pmp0cfg的W位
    CSRS(CSR_PMPCFG0,1ULL << 2 );      //pmp0cfg的X位
    CSRS(CSR_PMPCFG0,1ULL << 3 );      //pmp0cfg的TOR模式

    CSRC(CSR_PMPCFG0,1ULL << 8 );      //pmp1cfg的R位
    CSRS(CSR_PMPCFG0,1ULL << 9 );      //pmp1cfg的W位
    CSRS(CSR_PMPCFG0,1ULL << 10 );      //pmp1cfg的X位
    CSRS(CSR_PMPCFG0,1ULL << 11 );      //pmp1cfg的TOR模式

    CSRW(CSR_PMPADDR0, 0x88000000UL);
    CSRW(CSR_PMPADDR1, 0x89000000UL);


    CSRS(CSR_PMPCFG0,1ULL << 7 );       //pmp0cfg的L位 
    CSRC(CSR_PMPCFG0,1ULL << 15 );      //pmp1cfg的L位 

    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();    

    lb(0x88100000UL << 2);

    TEST_ASSERT("hs mode lb when pmpcfg.R=0 and pmpcfg.L=1 leads to LAF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_LAF
    );


    TEST_END();
}


bool load_access_fault_3(){

    TEST_START();

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


    TEST_END();
}

bool load_access_fault_4(){

    TEST_START();

    goto_priv(PRIV_M);

    //pmpcfg.L被设置，当前特权级是VU，访问没有执行权限的PMP区域获取指令，pmpcfg.R=0
    
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

    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();    
    
    lb(0x80000100UL << 2);

    TEST_ASSERT("VU mode lb when pmpcfg.R=0 and pmpcfg.L=1 leads to LAF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_LAF
    );


    TEST_END();
}

bool load_access_fault_5(){

    TEST_START();

    goto_priv(PRIV_M);
    printf("pmpcfg0=%llx\n",CSRR(CSR_PMPCFG0));

    //pmpcfg.L被设0，当前特权级是M，访问没有执行权限的PMP区域获取指令，pmpcfg.R=0
    
    CSRW(CSR_PMPCFG0,(uint64_t)0x0);
    
    CSRC(CSR_PMPCFG0,1ULL << 8 );      //pmp1cfg的R位
    CSRS(CSR_PMPCFG0,1ULL << 9 );      //pmp1cfg的W位
    CSRS(CSR_PMPCFG0,1ULL << 10 );      //pmp1cfg的X位
    CSRS(CSR_PMPCFG0,1ULL << 11 );      //pmp1cfg的TOR模式

    CSRW(CSR_PMPADDR0, (uintptr_t)0x80000000);
    CSRW(CSR_PMPADDR1, (uintptr_t)0x81000000);

    CSRC(CSR_PMPCFG0,1ULL << 15 );       //pmp1cfg的L位 

    TEST_SETUP_EXCEPT();
    
    lb(0x80000100UL << 2);

    printf("%d\n",excpt.triggered);
    printf("%d\n",excpt.cause);



    TEST_ASSERT("m mode lb successful when pmpcfg.R=0 and pmpcfg.L=0 ",
        excpt.triggered == false
    );

    TEST_END();
}


bool load_access_fault_6(){

    TEST_START();

    goto_priv(PRIV_M);

    //pmpcfg.L被设0，当前特权级是HS，访问没有执行权限的PMP区域获取指令，pmpcfg.R=0
    
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

    CSRS(CSR_PMPCFG0,1ULL << 7 );           //pmp0cfg的L位 
    CSRC(CSR_PMPCFG0,1ULL << 15 );          //pmp1cfg的L位 

    printf("%llx\n",CSRR(CSR_PMPADDR0));
    printf("%llx\n",CSRR(CSR_PMPADDR1));
    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();    
    
    lb(0x80000100UL << 2);

    TEST_ASSERT("hs mode lb when pmpcfg.R=0 and pmpcfg.L=0 leads to LAF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_LAF
    );


    TEST_END();
}


bool load_access_fault_7(){

    TEST_START();

    goto_priv(PRIV_M);

    //pmpcfg.L被设0，当前特权级是HU，访问没有执行权限的PMP区域获取指令，pmpcfg.R=0
    
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
    CSRC(CSR_PMPCFG0,1ULL << 15 );       //pmp1cfg的L位 

    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();    
    
    lb(0x80000100UL << 2);

    TEST_ASSERT("HU mode lb when pmpcfg.R=0 and pmpcfg.L=0 leads to LAF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_LAF
    );


    TEST_END();
}

bool load_access_fault_8(){

    TEST_START();

    goto_priv(PRIV_M);

    //pmpcfg.L被设0，当前特权级是VU，访问没有执行权限的PMP区域获取指令，pmpcfg.R=0
    
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
    CSRC(CSR_PMPCFG0,1ULL << 15 );       //pmp1cfg的L位 

    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();    
    
    lb(0x80000100UL << 2);

    TEST_ASSERT("VU mode lb when pmpcfg.R=0 and pmpcfg.L=0 leads to LAF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_LAF
    );


    TEST_END();
}

bool load_access_fault_9(){

    TEST_START();

    goto_priv(PRIV_M);

    //跨越了两个具有不同权限的内存区域，一部分访问成功，一部分失败
    
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
    CSRC(CSR_PMPCFG0,1ULL << 15 );       //pmp1cfg的L位 

    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();    
    
    ld(0x1fffffffeUL << 2);

    TEST_ASSERT("Spanning two memory regions with different permissions, some accessed successfully and some failed leads to laf",
        excpt.triggered == true &&
        excpt.cause == CAUSE_LAF
    );


    TEST_END();
}

bool load_access_fault_10(){

    TEST_START();

    goto_priv(PRIV_M);

    //访问了无效的地址范围，不在正确的pmpaddr范围内
    
    CSRW(CSR_PMPCFG0,(uint64_t)0x0);
    
    CSRS(CSR_PMPCFG0,1ULL << 0 );      //pmp0cfg的R位
    CSRS(CSR_PMPCFG0,1ULL << 1 );      //pmp0cfg的W位
    CSRS(CSR_PMPCFG0,1ULL << 2 );      //pmp0cfg的X位
    CSRS(CSR_PMPCFG0,1ULL << 3 );      //pmp0cfg的TOR模式

    CSRW(CSR_PMPADDR0, (uintptr_t)0x80000000);
    
    CSRS(CSR_PMPCFG0,1ULL << 7 );     //pmp0cfg的L位 

    printf("pmpcfg0=%llx \n",CSRR(CSR_PMPCFG0));
    printf("pmpcfg2=%llx \n",CSRR(CSR_PMPCFG2));


    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();    

    ld(0x8f000000UL << 2);

    printf("%d\n",excpt.triggered);
    printf("%d\n",excpt.cause);

    TEST_ASSERT("An invalid address range was accessed and is not in the correct pmpaddr range leads to laf",
        excpt.triggered == true &&
        excpt.cause == CAUSE_LAF
    );


    TEST_END();
}


bool store_access_fault_1(){

    TEST_START();

    goto_priv(PRIV_M);

    //pmpcfg.L被设置，当前特权级是M，访问没有执行权限的PMP区域获取指令，pmpcfg.W=0
    
    CSRW(CSR_PMPCFG0,(uint64_t)0x0);
    
    CSRS(CSR_PMPCFG0,1ULL << 8 );      //pmp1cfg的R位
    CSRC(CSR_PMPCFG0,1ULL << 9 );      //pmp1cfg的W位
    CSRS(CSR_PMPCFG0,1ULL << 10 );      //pmp1cfg的X位
    CSRS(CSR_PMPCFG0,1ULL << 11 );      //pmp1cfg的TOR模式

    CSRW(CSR_PMPADDR0, (uintptr_t)0x80000000);
    CSRW(CSR_PMPADDR1, (uintptr_t)0x81000000);

    CSRS(CSR_PMPCFG0,1ULL << 15 );       //pmp1cfg的L位 

    sfence();
    hfence();
    TEST_SETUP_EXCEPT();
    
    sb(0x80000100UL << 2 , 0x0);

    printf("%d\n",excpt.triggered);
    printf("%d\n",excpt.cause);

    TEST_ASSERT("m mode sb when pmpcfg.W=0 and pmpcfg.L=1 leads to SAF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SAF
    );


    TEST_END();
}

bool store_access_fault_2(){

    TEST_START();

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
    
    sb(0x80000100UL << 2, 0x0);

    TEST_ASSERT("hs mode sb when pmpcfg.W=0 and pmpcfg.L=1 leads to SAF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SAF
    );


    TEST_END();
}


bool store_access_fault_3(){

    TEST_START();

    goto_priv(PRIV_M);

    //pmpcfg.L被设置，当前特权级是HU，访问没有执行权限的PMP区域获取指令，pmpcfg.W=0
    
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

    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();    
    
    sb(0x80000100UL << 2, 0x0);

    TEST_ASSERT("HU mode sb when pmpcfg.W=0 and pmpcfg.L=1 leads to SAF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SAF
    );


    TEST_END();
}

bool store_access_fault_4(){

    TEST_START();

    goto_priv(PRIV_M);

    //pmpcfg.L被设置，当前特权级是VU，访问没有执行权限的PMP区域获取指令，pmpcfg.W=0
    
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

    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();    
    
    sb(0x80000100UL << 2, 0x0);

    TEST_ASSERT("VU mode sb when pmpcfg.W=0 and pmpcfg.L=1 leads to SAF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SAF
    );


    TEST_END();
}

bool store_access_fault_5(){

    TEST_START();

    goto_priv(PRIV_M);

    //pmpcfg.L被设0，当前特权级是M，访问没有执行权限的PMP区域获取指令，pmpcfg.W=0
    
    CSRW(CSR_PMPCFG0,(uint64_t)0x0);
    
    CSRS(CSR_PMPCFG0,1ULL << 8 );      //pmp1cfg的R位
    CSRC(CSR_PMPCFG0,1ULL << 9 );      //pmp1cfg的W位
    CSRS(CSR_PMPCFG0,1ULL << 10 );      //pmp1cfg的X位
    CSRS(CSR_PMPCFG0,1ULL << 11 );      //pmp1cfg的TOR模式

    CSRW(CSR_PMPADDR0, (uintptr_t)0x80000000);
    CSRW(CSR_PMPADDR1, (uintptr_t)0x81000000);

    CSRC(CSR_PMPCFG0,1ULL << 15 );       //pmp1cfg的L位 

    TEST_SETUP_EXCEPT();
    
    sb(0x80000100UL << 2, 0x0);

    printf("%d\n",excpt.triggered);
    printf("%d\n",excpt.cause);

    TEST_ASSERT("m mode sb successful when pmpcfg.W=0 and pmpcfg.L=0 ",
        excpt.triggered == false
    );


    TEST_END();
}


bool store_access_fault_6(){

    TEST_START();

    goto_priv(PRIV_M);

    //pmpcfg.L被设0，当前特权级是HS，访问没有执行权限的PMP区域获取指令，pmpcfg.W=0
    
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

    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();    
    
    sb(0x80000100UL << 2, 0x0);

    TEST_ASSERT("hs mode sb when pmpcfg.W=0 and pmpcfg.L=0 leads to SAF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SAF
    );


    TEST_END();
}


bool store_access_fault_7(){

    TEST_START();

    goto_priv(PRIV_M);

    //pmpcfg.L被设0，当前特权级是HU，访问没有执行权限的PMP区域获取指令，pmpcfg.W=0
    
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

    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();    
    
    sb(0x80000100UL << 2, 0x0);

    TEST_ASSERT("HU mode sb when pmpcfg.W=0 and pmpcfg.L=0 leads to SAF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SAF
    );


    TEST_END();
}

bool store_access_fault_8(){

    TEST_START();

    goto_priv(PRIV_M);

    //pmpcfg.L被设0，当前特权级是VU，访问没有执行权限的PMP区域获取指令，pmpcfg.W=0
    
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

    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();    
    
    sb(0x80000100UL << 2, 0x0);

    TEST_ASSERT("VU mode sb when pmpcfg.W=0 and pmpcfg.L=0 leads to SAF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SAF
    );


    TEST_END();
}

bool store_access_fault_9(){

    TEST_START();

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

    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();    
    
    sd(0x1fffffffeUL << 2 ,0xdeadbeef);

    TEST_ASSERT("Spanning two memory regions with different permissions, some accessed successfully and some failed leads to saf",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SAF
    );


    TEST_END();
}


bool store_access_fault_10(){

    TEST_START();

    goto_priv(PRIV_M);

    //访问了无效的地址范围，不在正确的pmpaddr范围内
    
    CSRW(CSR_PMPCFG0,(uint64_t)0x0);
    
    CSRS(CSR_PMPCFG0,1ULL << 0 );      //pmp0cfg的R位
    CSRS(CSR_PMPCFG0,1ULL << 1 );      //pmp0cfg的W位
    CSRS(CSR_PMPCFG0,1ULL << 2 );      //pmp0cfg的X位
    CSRS(CSR_PMPCFG0,1ULL << 3 );      //pmp0cfg的TOR模式

    CSRW(CSR_PMPADDR0, (uintptr_t)0x80000000);

    CSRS(CSR_PMPCFG0,1ULL << 7 );     //pmp0cfg的L位 

    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();    
    
    sd(0x80300000UL << 2,0xdeadbeef);

    TEST_ASSERT("An invalid address range was accessed and is not in the correct pmpaddr range leads to saf",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SAF
    );


    TEST_END();
}




bool amo_access_fault_1(){

    TEST_START();

    goto_priv(PRIV_M);

    //pmpcfg.L被设置，当前特权级是M，访问没有执行权限的PMP区域获取指令，pmpcfg.W=0
    
    CSRW(CSR_PMPCFG0,(uint64_t)0x0);
    
    CSRS(CSR_PMPCFG0,1ULL << 8 );      //pmp1cfg的R位
    CSRC(CSR_PMPCFG0,1ULL << 9 );      //pmp1cfg的W位
    CSRS(CSR_PMPCFG0,1ULL << 10 );      //pmp1cfg的X位
    CSRS(CSR_PMPCFG0,1ULL << 11 );      //pmp1cfg的TOR模式

    CSRW(CSR_PMPADDR0, (uintptr_t)0x80000000);
    CSRW(CSR_PMPADDR1, (uintptr_t)0x81000000);

    CSRS(CSR_PMPCFG0,1ULL << 15 );       //pmp1cfg的L位 

    TEST_SETUP_EXCEPT();
    
    amoadd_d(0x80000100UL << 2 , 0xdeadbeef);

    printf("%d\n",excpt.triggered);
    printf("%d\n",excpt.cause);

    TEST_ASSERT("m mode amoadd_d when pmpcfg.W=0 and pmpcfg.L=1 leads to SAF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SAF
    );


    TEST_END();
}

bool amo_access_fault_2(){

    TEST_START();

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


    TEST_END();
}


bool amo_access_fault_3(){

    TEST_START();

    goto_priv(PRIV_M);

    //pmpcfg.L被设置，当前特权级是HU，访问没有执行权限的PMP区域获取指令，pmpcfg.W=0
    
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

    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();    
    
    amoadd_d(0x80000100UL << 2 , 0xdeadbeef);

    TEST_ASSERT("HU mode amoadd_d when pmpcfg.W=0 and pmpcfg.L=1 leads to SAF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SAF
    );


    TEST_END();
}

bool amo_access_fault_4(){

    TEST_START();

    goto_priv(PRIV_M);

    //pmpcfg.L被设置，当前特权级是VU，访问没有执行权限的PMP区域获取指令，pmpcfg.W=0
    
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

    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();    
    
    amoadd_d(0x80000100UL << 2 , 0xdeadbeef);

    TEST_ASSERT("VU mode amoadd_d when pmpcfg.W=0 and pmpcfg.L=1 leads to SAF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SAF
    );


    TEST_END();
}

bool amo_access_fault_5(){

    TEST_START();

    goto_priv(PRIV_M);

    //pmpcfg.L被设0，当前特权级是M，访问没有执行权限的PMP区域获取指令，pmpcfg.W=0
    
    CSRW(CSR_PMPCFG0,(uint64_t)0x0);
    
    CSRS(CSR_PMPCFG0,1ULL << 8 );      //pmp1cfg的R位
    CSRC(CSR_PMPCFG0,1ULL << 9 );      //pmp1cfg的W位
    CSRS(CSR_PMPCFG0,1ULL << 10 );      //pmp1cfg的X位
    CSRS(CSR_PMPCFG0,1ULL << 11 );      //pmp1cfg的TOR模式

    CSRW(CSR_PMPADDR0, (uintptr_t)0x80000000);
    CSRW(CSR_PMPADDR1, (uintptr_t)0x81000000);

    CSRC(CSR_PMPCFG0,1ULL << 15 );       //pmp1cfg的L位 

    TEST_SETUP_EXCEPT();
    
    amoadd_d(0x80000100UL << 2 , 0xdeadbeef);

    printf("%d\n",excpt.triggered);
    printf("%d\n",excpt.cause);

    TEST_ASSERT("m mode amoadd_d successful when pmpcfg.W=0 and pmpcfg.L=0 ",
        excpt.triggered == false
    );


    TEST_END();
}


bool amo_access_fault_6(){

    TEST_START();

    goto_priv(PRIV_M);

    //pmpcfg.L被设0，当前特权级是HS，访问没有执行权限的PMP区域获取指令，pmpcfg.W=0
    
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

    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();    
    
    amoadd_d(0x80000100UL << 2 , 0xdeadbeef);

    TEST_ASSERT("hs mode amoadd_d when pmpcfg.W=0 and pmpcfg.L=0 leads to SAF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SAF
    );


    TEST_END();
}


bool amo_access_fault_7(){

    TEST_START();

    goto_priv(PRIV_M);

    //pmpcfg.L被设0，当前特权级是HU，访问没有执行权限的PMP区域获取指令，pmpcfg.W=0
    
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

    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();    
    
    amoadd_d(0x80000100UL << 2 , 0xdeadbeef);

    TEST_ASSERT("HU mode amoadd_d when pmpcfg.W=0 and pmpcfg.L=0 leads to SAF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SAF
    );


    TEST_END();
}

bool amo_access_fault_8(){

    TEST_START();

    goto_priv(PRIV_M);

    //pmpcfg.L被设0，当前特权级是VU，访问没有执行权限的PMP区域获取指令，pmpcfg.W=0
    
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

    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();    
    
    amoor_d(0x80000100UL << 2 , 0xdeadbeef);

    TEST_ASSERT("VU mode amoor_d when pmpcfg.W=0 and pmpcfg.L=0 leads to SAF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SAF
    );


    TEST_END();
}

bool amo_access_fault_9(){

    TEST_START();

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

    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();    
    
    amoand_d(0x1fffffffeUL << 2 ,0xdeadbeef);

    TEST_ASSERT("Spanning two memory regions with different permissions, some accessed successfully and some failed leads to saf(amo)",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SAF
    );


    TEST_END();
}


bool amo_access_fault_10(){

    TEST_START();

    goto_priv(PRIV_M);

    //访问了无效的地址范围，不在正确的pmpaddr范围内
    
    CSRW(CSR_PMPCFG0,(uint64_t)0x0);
    
    CSRS(CSR_PMPCFG0,1ULL << 0 );      //pmp0cfg的R位
    CSRS(CSR_PMPCFG0,1ULL << 1 );      //pmp0cfg的W位
    CSRS(CSR_PMPCFG0,1ULL << 2 );      //pmp0cfg的X位
    CSRS(CSR_PMPCFG0,1ULL << 3 );      //pmp0cfg的TOR模式

    CSRW(CSR_PMPADDR0, (uintptr_t)0x80000000);

    CSRS(CSR_PMPCFG0,1ULL << 7 );     //pmp0cfg的L位 

    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();    
    
    amoand_d(0x80000000UL << 2 ,0xdeadbeef);

    TEST_ASSERT("An invalid address range was accessed and is not in the correct pmpaddr range leads to store guest fault(amo)",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SAF
    );


    TEST_END();
}


bool instruction_access_fault_1(){

    TEST_START();

    goto_priv(PRIV_M);

    //pmpcfg.L被设置，当前特权级是M，访问没有执行权限的PMP区域获取指令，pmpcfg.X=0
    
    CSRW(CSR_PMPCFG0,(uint64_t)0x0);
    
    CSRS(CSR_PMPCFG0,1ULL << 8 );      //pmp1cfg的R位
    CSRS(CSR_PMPCFG0,1ULL << 9 );      //pmp1cfg的W位
    CSRC(CSR_PMPCFG0,1ULL << 10 );      //pmp1cfg的X位
    CSRS(CSR_PMPCFG0,1ULL << 11 );      //pmp1cfg的TOR模式

    CSRW(CSR_PMPADDR0, (uintptr_t)0x80000000);
    CSRW(CSR_PMPADDR1, (uintptr_t)0x81000000);

    CSRS(CSR_PMPCFG0,1ULL << 15 );       //pmp1cfg的L位 

    TEST_SETUP_EXCEPT();
    
    TEST_EXEC_EXCEPT(0x80000100UL << 2);

    printf("%d\n",excpt.triggered);
    printf("%d\n",excpt.cause);

    TEST_ASSERT("m mode fetch instruction when pmpcfg.X=0 and pmpcfg.L=1 leads to IAF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_IAF
    );


    TEST_END();
}

bool instruction_access_fault_2(){

    TEST_START();

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


    TEST_END();
}


bool instruction_access_fault_3(){

    TEST_START();

    goto_priv(PRIV_M);

    //pmpcfg.L被设置，当前特权级是HU，访问没有执行权限的PMP区域获取指令，pmpcfg.X=0
    
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

    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();    
    
    TEST_EXEC_EXCEPT(0x80000100UL << 2);

    TEST_ASSERT("HU mode fetch instruction when pmpcfg.X=0 and pmpcfg.L=1 leads to IAF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_IAF
    );


    TEST_END();
}

bool instruction_access_fault_4(){

    TEST_START();

    goto_priv(PRIV_M);

    //pmpcfg.L被设置，当前特权级是VU，访问没有执行权限的PMP区域获取指令，pmpcfg.X=0
    
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

    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();    
    
    TEST_EXEC_EXCEPT(0x80000100UL << 2);

    TEST_ASSERT("VU mode fetch instruction when pmpcfg.X=0 and pmpcfg.L=1 leads to IAF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_IAF
    );


    TEST_END();
}

bool instruction_access_fault_5(){

    TEST_START();

    goto_priv(PRIV_M);

    //pmpcfg.L被设0，当前特权级是M，访问没有执行权限的PMP区域获取指令，pmpcfg.X=0
    
    CSRW(CSR_PMPCFG0,(uint64_t)0x0);
    
    CSRS(CSR_PMPCFG0,1ULL << 0 );      //pmp0cfg的R位
    CSRS(CSR_PMPCFG0,1ULL << 1 );      //pmp0cfg的W位
    CSRS(CSR_PMPCFG0,1ULL << 2 );      //pmp0cfg的X位
    CSRS(CSR_PMPCFG0,1ULL << 3 );      //pmp0cfg的TOR模式
    
    CSRS(CSR_PMPCFG0,1ULL << 8 );      //pmp1cfg的R位
    CSRS(CSR_PMPCFG0,1ULL << 9 );      //pmp1cfg的W位
    CSRC(CSR_PMPCFG0,1ULL << 10 );      //pmp1cfg的X位
    CSRS(CSR_PMPCFG0,1ULL << 11 );      //pmp1cfg的TOR模式

    CSRW(CSR_PMPADDR0, (uintptr_t)0x80000000UL);
    CSRW(CSR_PMPADDR1, (uintptr_t)0x81000000UL);

    CSRC(CSR_PMPCFG0,1ULL << 15 );       //pmp1cfg的L位 

    TEST_SETUP_EXCEPT();
    
    TEST_EXEC_EXCEPT(0x80000100UL << 2);

    printf("%d\n",excpt.triggered);
    printf("%d\n",excpt.cause);

    TEST_ASSERT("m mode fetch instruction successful when pmpcfg.X=0 and pmpcfg.L=0 ",
        excpt.triggered == false
    );

    TEST_END();
}


bool instruction_access_fault_6(){

    TEST_START();

    goto_priv(PRIV_M);

    //pmpcfg.L被设0，当前特权级是HS，访问没有执行权限的PMP区域获取指令，pmpcfg.X=0
    
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

    TEST_ASSERT("hs mode fetch instruction when pmpcfg.X=0 and pmpcfg.L=0 leads to IAF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_IAF
    );


    TEST_END();
}


bool instruction_access_fault_7(){

    TEST_START();

    goto_priv(PRIV_M);

    //pmpcfg.L被设0，当前特权级是HU，访问没有执行权限的PMP区域获取指令，pmpcfg.X=0
    
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


    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();    
    
    TEST_EXEC_EXCEPT(0x80000100UL << 2);

    TEST_ASSERT("HU mode fetch instruction when pmpcfg.X=0 and pmpcfg.L=0 leads to IAF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_IAF
    );

    TEST_END();
}

bool instruction_access_fault_8(){

    TEST_START();

    goto_priv(PRIV_M);

    //pmpcfg.L被设0，当前特权级是VU，访问没有执行权限的PMP区域获取指令，pmpcfg.X=0
    
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

    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();    
    
    TEST_EXEC_EXCEPT(0x80000100UL << 2);

    TEST_ASSERT("VU mode lb when pmpcfg.X=0 and pmpcfg.L=0 leads to IAF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_IAF
    );


    TEST_END();
}

bool instruction_access_fault_9(){

    TEST_START();

    goto_priv(PRIV_M);

    //跨越了两个具有不同权限的内存区域，一部分访问成功，一部分失败
    
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
    CSRS(CSR_PMPCFG0,1ULL << 15 );       //pmp1cfg的L位 

    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();    
    
    TEST_EXEC_EXCEPT(0x1fffffffeUL << 2);

    TEST_ASSERT("Spanning two memory regions with different permissions, some accessed successfully and some failed leads to IAF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_IAF
    );


    TEST_END();
}

bool instruction_access_fault_10(){

    TEST_START();

    goto_priv(PRIV_M);

    //访问了无效的地址范围，不在正确的pmpaddr范围内
    
    CSRW(CSR_PMPCFG0,(uint64_t)0x0);
    
    CSRS(CSR_PMPCFG0,1ULL << 0 );      //pmp0cfg的R位
    CSRS(CSR_PMPCFG0,1ULL << 1 );      //pmp0cfg的W位
    CSRS(CSR_PMPCFG0,1ULL << 2 );      //pmp0cfg的X位
    CSRS(CSR_PMPCFG0,1ULL << 3 );      //pmp0cfg的TOR模式

    CSRW(CSR_PMPADDR0, (uintptr_t)0x90000000);

    CSRS(CSR_PMPCFG0,1ULL << 7 );     //pmp0cfg的L位 
    printf("pmpcfg0=%llx\n",CSRR(CSR_PMPCFG0));printf("pmpcfg2=%llx\n",CSRR(CSR_PMPCFG2));
    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();    
    
    TEST_EXEC_EXCEPT(0x9f000000UL << 2);

    TEST_ASSERT("An invalid address range was accessed and is not in the correct pmpaddr range leads to IAF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_IAF
    );


    TEST_END();
}