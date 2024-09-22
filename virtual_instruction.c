#include <rvh_test.h>
#include <page_tables.h> 


bool virtual_instruction_1() {

    TEST_START();


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

    TEST_END();
}
    ////////////////////////////////////////////////////////////////////////
bool virtual_instruction_2() {

    TEST_START();

    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    volatile uint64_t tmp = hlvd(0);
    TEST_ASSERT("vs hlvd leads to virtual instruction exception",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 

    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    hsvb(0, 0xdeadbeef);
    TEST_ASSERT("vs hsvb leads to virtual instruction exception",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 
    
    TEST_END();

}
    ////////////////////////////////////////////////////////////////////////
bool virtual_instruction_3() {

    TEST_START();
    goto_priv(PRIV_M);
    CSRS(CSR_HSTATUS, HSTATUS_VTSR);
    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    TEST_EXEC_SRET();
    TEST_ASSERT("vs sret leads to virtual instruction exception when vtsr set",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    );  
    goto_priv(PRIV_M);
    CSRC(CSR_HSTATUS, HSTATUS_VTSR);

    TEST_END();
}
    ////////////////////////////////////////////////////////////////////////
bool virtual_instruction_4() {

    TEST_START();

    goto_priv(PRIV_M);
    CSRS(CSR_HSTATUS, HSTATUS_VTVM);
    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    sfence();
    TEST_ASSERT("vs sfence leads to virtual instruction exception when vtvm set",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    );  

    TEST_SETUP_EXCEPT();
    CSRW(satp, 0x0);
    TEST_ASSERT("vs satp acess leads to virtual instruction exception when vtvm set",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 

    
    // goto_priv(PRIV_M);
    // reset_state();
    // CSRC(CSR_HSTATUS, HSTATUS_VTVM);
    // CSRC(CSR_MSTATUS, MSTATUS_TVM);
    // goto_priv(PRIV_VS);
    // TEST_SETUP_EXCEPT();
    // CSRS(CSR_VSIE, 1ULL << 1);

    // TEST_ASSERT("vs mode access vs level CSR low part leads to virtual instruction exception when mstatus_TVM=0",
    //     excpt.triggered == true &&
    //     excpt.cause == CAUSE_VRTI
    // );  


    // TEST_SETUP_EXCEPT();
    // CSRW(CSR_HSTATUS, 1ULL << 1);
    // TEST_ASSERT("vs mode access h level low part leads to virtual instruction exception when mstatus_TVM=0",
    //     excpt.triggered == true &&
    //     excpt.cause == CAUSE_VRTI
    // );      
    
    TEST_END();
}
    ////////////////////////////////////////////////////////////////////////

bool virtual_instruction_5() {

    TEST_START();

    goto_priv(PRIV_M);
    CSRS(CSR_HSTATUS, HSTATUS_VTW);
    CSRC(CSR_MSTATUS, MSTATUS_TW);
    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    asm("wfi");
    TEST_ASSERT("vs wfi leads to virtual instruction exception when vtw=1 and tw=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    );  

    goto_priv(PRIV_M);
    printf("%llx",CSRR(CSR_MTVAL));

    TEST_END();
}


bool virtual_instruction_6() {

    TEST_START();

//vu-mode
    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    hfence_vvma();
    TEST_ASSERT("vu executing hfence.vvma leads to virtual instruction exception",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 
    
    
    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    hfence_gvma();
    TEST_ASSERT("vu executing hfence.gvma leads to virtual instruction exception",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 


    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    Sinval_vma();
    TEST_ASSERT("vu executing sinval.vma leads to virtual instruction exception when vtvm=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 

    TEST_END();
}
    ////////////////////////////////////////////////////////////////////////
bool virtual_instruction_7() {

    TEST_START();

    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    uint64_t tmp = hlvb(0);

    TEST_ASSERT("vu hlvb leads to virtual instruction exception",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 

    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
     tmp = hlvbu(0);

    TEST_ASSERT("vu hlvbu leads to virtual instruction exception",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 

    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
     tmp = hlvh(0);

    TEST_ASSERT("vu hlvh leads to virtual instruction exception",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 

    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
     tmp = hlvw(0);

    TEST_ASSERT("vu hlvw leads to virtual instruction exception",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 

    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
     tmp = hlvd(0);

    TEST_ASSERT("vu hlvd leads to virtual instruction exception",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 

    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    hsvb(0, 0xdeadbeef);
    printf("%d\n",excpt.cause);

    TEST_ASSERT("vu hsvb leads to virtual instruction exception",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 

    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    hsvd(0, 0xdeadbeef);
    printf("%d\n",excpt.cause);

    TEST_ASSERT("vu hsvb leads to virtual instruction exception",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 

    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    hsvw(0, 0xdeadbeef);
    printf("%d\n",excpt.cause);

    TEST_ASSERT("vu hsvb leads to virtual instruction exception",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 

    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    hsvh(0, 0xdeadbeef);
    printf("%d\n",excpt.cause);

    TEST_ASSERT("vu hsvb leads to virtual instruction exception",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 
    
    TEST_END();
}

    ////////////////////////////////////////////////////////////////////////
//在mstatus.TW=0  时执行WFI   (即使允许执行WFI指令),或者执行S级监管级指令 (SRET或者SFENCE)   , 引发虚拟异常(在虚拟用户模式下，软件不应该 由执行这些监管模式指令的权限)
bool virtual_instruction_8() {

    TEST_START();

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
 
    TEST_END();
}
bool virtual_instruction_9() {

    TEST_START();

    goto_priv(PRIV_M);
    CSRC(CSR_MSTATUS,MSTATUS_TW);
    
    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    sfence();
    
    TEST_ASSERT("vu sfence leads to virtual instruction exception when mstatus.TW=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    );  

    TEST_END();
}
bool virtual_instruction_10() {

    TEST_START();


    goto_priv(PRIV_M);
    CSRS(CSR_HSTATUS, HSTATUS_VTW);
    CSRC(CSR_MSTATUS, MSTATUS_TW);
    
    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    wfi();
    TEST_ASSERT("vu wfi leads to virtual instruction exception when vtw=1 and tw=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    );  

    TEST_END();
}
    ////////////////////////////////////////////////////////////////////////

bool virtual_instruction_11() {

    TEST_START();    
    goto_priv(PRIV_M);
    CSRC(CSR_HSTATUS, HSTATUS_VTVM);
    CSRC(CSR_HSTATUS, MSTATUS_TVM);
    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    CSRS(CSR_VSIE,1ULL << 1);
    TEST_ASSERT("vu mode access vs level CSR low part leads to virtual instruction exception when mstatus_TVM=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    );  

    // goto_priv(PRIV_M);
    // reset_state();
    // goto_priv(PRIV_M);
    // CSRC(CSR_HSTATUS, MSTATUS_TVM);
    // goto_priv(PRIV_VU);
    // CSRS(CSR_HSTATUS,1ULL << 1);
    // TEST_ASSERT("vu mode access h level CSR low part leads to virtual instruction exception when mstatus_TVM=0 ",
    //     excpt.triggered == true &&
    //     excpt.cause == CAUSE_VRTI
    // );      



    TEST_END(); 
}

bool virtual_instruction_12() {

    TEST_START();


    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();

    CSRR(CSR_SATP);
    TEST_ASSERT("vu mode access satp leads to virtual instruction exception",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 

    TEST_END();
}



    /**
     * We can test this the other way around because it will stall the program
     */

    ////////////////////////////////////////////////////////////////////////


// bool virtual_instruction_12() {

//     TEST_START();    
//     goto_priv(PRIV_M);
//     CSRC(mcounteren, HCOUNTEREN_TM);

//     goto_priv(PRIV_VS);
//     TEST_SETUP_EXCEPT();
//     volatile uint64_t time = CSRR(time);
//     TEST_ASSERT("vs access to time casuses virtual instruction exception when mcounteren.tm=0 and hcounteren.tm=0",
//         excpt.triggered == true &&
//         excpt.cause == CAUSE_VRTI
//     );

//     TEST_END(); 
// }

// bool virtual_instruction_13() {

//     TEST_START();    
//     goto_priv(PRIV_M);
//     CSRC(mcounteren, HCOUNTEREN_TM);

//     goto_priv(PRIV_HS);
//     TEST_SETUP_EXCEPT();
//     volatile uint64_t time = CSRR(time);
//     TEST_ASSERT("vs access to time casuses virtual instruction exception when mcounteren.tm=0 and hcounteren.tm=0",
//         excpt.triggered == true &&
//         excpt.cause == CAUSE_VRTI
//     );

//     TEST_END(); 
// }

// bool virtual_instruction_14() {

//     TEST_START();    
//     goto_priv(PRIV_M);
//     CSRS(mcounteren, HCOUNTEREN_TM);
//     CSRS(CSR_HCOUNTEREN, HCOUNTEREN_TM);
//     goto_priv(PRIV_VS);
//     TEST_SETUP_EXCEPT();
//     volatile uint64_t time = CSRR(time);
//     TEST_ASSERT("vs access to time casuses succsseful with mcounteren.tm and hcounteren.tm set",
//         excpt.triggered == false
//     );

//     TEST_END(); 
// }

// bool virtual_instruction_15() {

//     TEST_START();    

//     goto_priv(PRIV_M);
//     CSRW(mcounteren, 0);
//     CSRW(CSR_HCOUNTEREN, 0);
//     goto_priv(PRIV_VS);
//     TEST_SETUP_EXCEPT();
//     CSRW(sscratch, 1);
//     volatile uint64_t cycle = CSRR(cycle);
//     TEST_ASSERT("vs access to cycle casuses virtual instruction exception",
//         excpt.triggered == true &&
//         excpt.cause == CAUSE_ILI
//     );
//     TEST_END(); 
// }

// bool virtual_instruction_16() {

//     TEST_START();    
//     goto_priv(PRIV_M);
//     CSRS(mcounteren, HCOUNTEREN_CY);
//     CSRW(CSR_HCOUNTEREN, 0);
//     goto_priv(PRIV_VS);
//     TEST_SETUP_EXCEPT();
//     CSRW(sscratch, 1);
//     volatile uint64_t cycle = CSRR(cycle);
//     TEST_ASSERT("vs access to cycle casuses virtual instruction exception when mcounteren.cy set",
//         excpt.triggered == true &&
//         excpt.cause == CAUSE_VRTI
//     );

//     TEST_END(); 
// }

// bool virtual_instruction_17() {

//     TEST_START();    
//     goto_priv(PRIV_M);
//     CSRS(mcounteren, HCOUNTEREN_CY);
//     CSRS(CSR_HCOUNTEREN, HCOUNTEREN_CY);
//     goto_priv(PRIV_VS);
//     TEST_SETUP_EXCEPT();
//     CSRW(sscratch, 1);
//     volatile uint64_t cycle = CSRR(cycle);
//     TEST_ASSERT("vs access to cycle casuses succsseful when mcounteren.cy and hcounteren.cy set",
//         excpt.triggered == false
//     );

//     TEST_END(); 
// }

#ifdef Xiangshan
    goto_priv(PRIV_M);
    CSRW(mcounteren, 0);
    CSRW(CSR_HCOUNTEREN, 0);

    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    volatile uint64_t time = CSRR(time);
    TEST_ASSERT("vs access to time casuses virtual instruction exception",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    );

    goto_priv(PRIV_M);
    CSRS(mcounteren, HCOUNTEREN_TM);
    CSRS(CSR_HCOUNTEREN, HCOUNTEREN_TM);
    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    time = CSRR(time);
    TEST_ASSERT("vs access to time casuses succsseful with mcounteren.tm and hcounteren.tm set",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    );

    //////////////////////////////////////////////////////////////////////

    goto_priv(PRIV_M);
    CSRW(mcounteren, 0);
    CSRW(CSR_HCOUNTEREN, 0);
    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    CSRW(sscratch, 1);
    volatile uint64_t cycle = CSRR(cycle);
    TEST_ASSERT("vs access to cycle casuses virtual instruction exception",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    );

    goto_priv(PRIV_M);
    CSRS(mcounteren, HCOUNTEREN_CY);
    CSRW(CSR_HCOUNTEREN, 0);
    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    CSRW(sscratch, 1);
    cycle = CSRR(cycle);
    TEST_ASSERT("vs access to cycle casuses virtual instruction exception when mcounteren.cy set",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    );


    goto_priv(PRIV_M);
    CSRS(mcounteren, HCOUNTEREN_CY);
    CSRS(CSR_HCOUNTEREN, HCOUNTEREN_CY);
    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    CSRW(sscratch, 1);
    cycle = CSRR(cycle);
    TEST_ASSERT("vs access to cycle casuses succsseful when mcounteren.cy and hcounteren.cy set",
        excpt.triggered == false
    );
#endif