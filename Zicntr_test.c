#include <rvh_test.h>
#include <csrs.h> 

bool time_test_1() {

    TEST_START();    
    goto_priv(PRIV_M);
    CSRC(CSR_MCOUNTEREN, HCOUNTEREN_TM);

    goto_priv(PRIV_VS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_TIME);

    TEST_ASSERT("vs access to time casuses illegal instruction exception when mcounteren.tm=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    );

    goto_priv(PRIV_HS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_TIME);

    TEST_ASSERT("hs access to time casuses illegal instruction exception when mcounteren.tm=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    );

    goto_priv(PRIV_VU);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_TIME);

    TEST_ASSERT("vu access to time casuses illegal instruction exception when mcounteren.tm=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    );
    
    goto_priv(PRIV_HU);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_TIME);

    TEST_ASSERT("hu access to time casuses illegal instruction exception when mcounteren.tm=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    );

    TEST_END(); 
}

bool time_test_2() {

    TEST_START();    
    goto_priv(PRIV_M);
    CSRS(CSR_MCOUNTEREN, HCOUNTEREN_TM);

    goto_priv(PRIV_HS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_TIME);

    TEST_ASSERT("hs access to time successful when mcounteren.tm=1",
        excpt.triggered == false
    );
    TEST_END(); 
}

bool time_test_3() {

    TEST_START();    
    goto_priv(PRIV_M);
    CSRC(CSR_SCOUNTEREN, HCOUNTEREN_TM); 
    CSRS(CSR_MCOUNTEREN, HCOUNTEREN_TM); 

    goto_priv(PRIV_HU);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_TIME);

    TEST_ASSERT("hu access to time casuses illegal instruction exception when scounteren.tm=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    );

    TEST_END(); 
}

bool time_test_4() {

    TEST_START();    
    goto_priv(PRIV_M);
    CSRS(CSR_SCOUNTEREN, HCOUNTEREN_TM); 
    CSRS(CSR_MCOUNTEREN, HCOUNTEREN_TM); 

    goto_priv(PRIV_HU);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_TIME);

    TEST_ASSERT("hu access to time successful when mcounteren.tm=1 scounteren.tm=1",
        excpt.triggered == false
    );

    TEST_END(); 
}

bool time_test_5() {

    TEST_START();    
    goto_priv(PRIV_M);
    CSRC(CSR_HCOUNTEREN, HCOUNTEREN_TM); 
    CSRS(CSR_MCOUNTEREN, HCOUNTEREN_TM); 

    goto_priv(PRIV_VS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_TIME);

    TEST_ASSERT("vs access to time casuses virtual instruction exception when mcounteren.tm=1 hcounteren.tm=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    );

    TEST_END(); 
}

bool time_test_6() {

    TEST_START();    
    goto_priv(PRIV_M);
    CSRS(CSR_HCOUNTEREN, HCOUNTEREN_TM); 
    CSRS(CSR_MCOUNTEREN, HCOUNTEREN_TM); 

    goto_priv(PRIV_VS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_TIME);

    TEST_ASSERT("vs access to time successful when mcounteren.tm=1 hcounteren.tm=1",
        excpt.triggered == false
    );

    TEST_END(); 
}

bool time_test_7() {

    TEST_START();    
    goto_priv(PRIV_M);
    CSRS(CSR_HCOUNTEREN, HCOUNTEREN_TM); 
    CSRS(CSR_MCOUNTEREN, HCOUNTEREN_TM); 
    CSRS(CSR_SCOUNTEREN, HCOUNTEREN_TM); 

    goto_priv(PRIV_VU);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_TIME);

    TEST_ASSERT("vu access to time successful when mcounteren.tm=1 hcounteren.tm=1 scounteren.tm=1",
        excpt.triggered == false
    );

    TEST_END(); 
}

bool time_test_8() {

    TEST_START();    
    goto_priv(PRIV_M);
    CSRC(CSR_HCOUNTEREN, HCOUNTEREN_TM); 
    CSRS(CSR_MCOUNTEREN, HCOUNTEREN_TM); 
    CSRS(CSR_SCOUNTEREN, HCOUNTEREN_TM); 

    goto_priv(PRIV_VU);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_TIME);

    TEST_ASSERT("vu access to time casuses virtual instruction exception when mcounteren.tm=1 hcounteren.tm=0 scounteren.tm=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    );

    TEST_END(); 
}

bool time_test_9() {

    TEST_START();    
    goto_priv(PRIV_M);
    CSRS(CSR_HCOUNTEREN, HCOUNTEREN_TM); 
    CSRS(CSR_MCOUNTEREN, HCOUNTEREN_TM); 
    CSRC(CSR_SCOUNTEREN, HCOUNTEREN_TM); 

    goto_priv(PRIV_HU);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_TIME);

    TEST_ASSERT("hu access to time casuses illegal instruction exception when mcounteren.tm=1 hcounteren.tm=1 scounteren.tm=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    );

    TEST_END(); 
}

bool time_test_10() {

    TEST_START();    
    goto_priv(PRIV_M);
    CSRS(CSR_HCOUNTEREN, HCOUNTEREN_TM); 
    CSRS(CSR_MCOUNTEREN, HCOUNTEREN_TM); 

    goto_priv(PRIV_VS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_TIME);

    TEST_ASSERT("vu access to time successful exception when mcounteren.tm=1 hcounteren.tm=1",
        excpt.triggered == false
    );

    TEST_END(); 
}


bool cycle_test_1() {

    TEST_START();    
    goto_priv(PRIV_M);
    CSRC(CSR_MCOUNTEREN, HCOUNTEREN_CY);

    goto_priv(PRIV_VS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_CYCLE);

    TEST_ASSERT("vs access to cycle casuses illegal instruction exception when mcounteren.cy=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    );

    goto_priv(PRIV_HS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_CYCLE);

    TEST_ASSERT("hs access to cycle casuses illegal instruction exception when mcounteren.cy=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    );

    goto_priv(PRIV_VU);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_CYCLE);

    TEST_ASSERT("vu access to cycle casuses illegal instruction exception when mcounteren.cy=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    );
    
    goto_priv(PRIV_HU);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_CYCLE);

    TEST_ASSERT("hu access to cycle casuses illegal instruction exception when mcounteren.cy=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    );

    TEST_END(); 
}

bool cycle_test_2() {

    TEST_START();    
    goto_priv(PRIV_M);
    CSRS(CSR_MCOUNTEREN, HCOUNTEREN_CY);

    goto_priv(PRIV_HS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_CYCLE);

    TEST_ASSERT("hs access to cycle successful when mcounteren.cy=1",
        excpt.triggered == false
    );
    TEST_END(); 
}

bool cycle_test_3() {

    TEST_START();    
    goto_priv(PRIV_M);
    CSRC(CSR_SCOUNTEREN, HCOUNTEREN_CY); 
    CSRS(CSR_MCOUNTEREN, HCOUNTEREN_CY); 

    goto_priv(PRIV_HU);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_CYCLE);

    TEST_ASSERT("hu access to cycle casuses illegal instruction exception when scounteren.cy=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    );

    TEST_END(); 
}

bool cycle_test_4() {

    TEST_START();    
    goto_priv(PRIV_M);
    CSRS(CSR_SCOUNTEREN, HCOUNTEREN_CY); 
    CSRS(CSR_MCOUNTEREN, HCOUNTEREN_CY); 

    goto_priv(PRIV_HU);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_CYCLE);

    TEST_ASSERT("hu access to cycle successful when mcounteren.cy=1 scounteren.cy=1",
        excpt.triggered == false
    );

    TEST_END(); 
}

bool cycle_test_5() {

    TEST_START();    
    goto_priv(PRIV_M);
    CSRC(CSR_HCOUNTEREN, HCOUNTEREN_CY); 
    CSRS(CSR_MCOUNTEREN, HCOUNTEREN_CY); 

    goto_priv(PRIV_VS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_CYCLE);

    TEST_ASSERT("vs access to cycle casuses virtual instruction exception when mcounteren.cy=1 hcounteren.cy=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    );

    TEST_END(); 
}

bool cycle_test_6() {

    TEST_START();    
    goto_priv(PRIV_M);
    CSRS(CSR_HCOUNTEREN, HCOUNTEREN_CY); 
    CSRS(CSR_MCOUNTEREN, HCOUNTEREN_CY); 

    goto_priv(PRIV_VS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_CYCLE);

    TEST_ASSERT("vs access to cycle successful when mcounteren.cy=1 hcounteren.cy=1",
        excpt.triggered == false
    );

    TEST_END(); 
}

bool cycle_test_7() {

    TEST_START();    
    goto_priv(PRIV_M);
    CSRS(CSR_HCOUNTEREN, HCOUNTEREN_CY); 
    CSRS(CSR_MCOUNTEREN, HCOUNTEREN_CY); 
    CSRS(CSR_SCOUNTEREN, HCOUNTEREN_CY); 

    goto_priv(PRIV_VU);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_CYCLE);

    TEST_ASSERT("vu access to cycle successful when mcounteren.cy=1 hcounteren.cy=1 scounteren.cy=1",
        excpt.triggered == false
    );

    TEST_END(); 
}

bool cycle_test_8() {

    TEST_START();    
    goto_priv(PRIV_M);
    CSRC(CSR_HCOUNTEREN, HCOUNTEREN_CY); 
    CSRS(CSR_MCOUNTEREN, HCOUNTEREN_CY); 
    CSRS(CSR_SCOUNTEREN, HCOUNTEREN_CY); 

    goto_priv(PRIV_VU);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_CYCLE);

    TEST_ASSERT("vu access to cycle casuses virtual instruction exception when mcounteren.cy=1 hcounteren.cy=0 scounteren.cy=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    );

    TEST_END(); 
}

bool cycle_test_9() {

    TEST_START();    
    goto_priv(PRIV_M);
    CSRS(CSR_MCOUNTEREN, HCOUNTEREN_CY); 
    CSRC(CSR_SCOUNTEREN, HCOUNTEREN_CY); 

    goto_priv(PRIV_HU);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_CYCLE);

    TEST_ASSERT("hu access to cycle casuses illegal instruction exception when mcounteren.cy=1 scounteren.cy=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    );

    TEST_END(); 
}

bool cycle_test_10() {

    TEST_START();    
    goto_priv(PRIV_M);
    CSRS(CSR_HCOUNTEREN, HCOUNTEREN_CY); 
    CSRS(CSR_MCOUNTEREN, HCOUNTEREN_CY); 

    goto_priv(PRIV_VS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_CYCLE);

    TEST_ASSERT("vu access to time successful exception when mcounteren.cy=1 hcounteren.cy=1",
        excpt.triggered == false
    );

    TEST_END(); 
}


bool instret_test_1() {

    TEST_START();    
    goto_priv(PRIV_M);
    CSRC(CSR_MCOUNTEREN, HCOUNTEREN_IR);

    goto_priv(PRIV_VS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_INSTRET);

    TEST_ASSERT("vs access to instret casuses illegal instruction exception when mcounteren.ir=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    );

    goto_priv(PRIV_HS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_INSTRET);

    TEST_ASSERT("hs access to instret casuses illegal instruction exception when mcounteren.ir=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    );

    goto_priv(PRIV_VU);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_INSTRET);

    TEST_ASSERT("vu access to instret casuses illegal instruction exception when mcounteren.ir=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    );
    
    goto_priv(PRIV_HU);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_INSTRET);

    TEST_ASSERT("hu access to instret casuses illegal instruction exception when mcounteren.ir=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    );

    TEST_END(); 
}

bool instret_test_2() {

    TEST_START();    
    goto_priv(PRIV_M);
    CSRS(CSR_MCOUNTEREN, HCOUNTEREN_IR);

    goto_priv(PRIV_HS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_INSTRET);

    TEST_ASSERT("hs access to instret successful when mcounteren.ir=1",
        excpt.triggered == false
    );
    TEST_END(); 
}

bool instret_test_3() {

    TEST_START();    
    goto_priv(PRIV_M);
    CSRC(CSR_SCOUNTEREN, HCOUNTEREN_IR); 
    CSRS(CSR_MCOUNTEREN, HCOUNTEREN_IR); 

    goto_priv(PRIV_HU);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_INSTRET);

    TEST_ASSERT("hu access to instret casuses illegal instruction exception when scounteren.ir=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    );

    TEST_END(); 
}

bool instret_test_4() {

    TEST_START();    
    goto_priv(PRIV_M);
    CSRS(CSR_SCOUNTEREN, HCOUNTEREN_IR); 
    CSRS(CSR_MCOUNTEREN, HCOUNTEREN_IR); 

    goto_priv(PRIV_HU);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_INSTRET);

    TEST_ASSERT("hu access to instret successful when mcounteren.ir=1 scounteren.ir=1",
        excpt.triggered == false
    );

    TEST_END(); 
}

bool instret_test_5() {

    TEST_START();    
    goto_priv(PRIV_M);
    CSRC(CSR_HCOUNTEREN, HCOUNTEREN_IR); 
    CSRS(CSR_MCOUNTEREN, HCOUNTEREN_IR); 

    goto_priv(PRIV_VS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_INSTRET);

    TEST_ASSERT("vs access to instret casuses virtual instruction exception when mcounteren.ir=1 hcounteren.ir=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    );

    TEST_END(); 
}

bool instret_test_6() {

    TEST_START();    
    goto_priv(PRIV_M);
    CSRS(CSR_HCOUNTEREN, HCOUNTEREN_IR); 
    CSRS(CSR_MCOUNTEREN, HCOUNTEREN_IR); 

    goto_priv(PRIV_VS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_INSTRET);

    TEST_ASSERT("vs access to instret successful when mcounteren.ir=1 hcounteren.ir=1",
        excpt.triggered == false
    );

    TEST_END(); 
}

bool instret_test_7() {

    TEST_START();    
    goto_priv(PRIV_M);
    CSRS(CSR_HCOUNTEREN, HCOUNTEREN_IR); 
    CSRS(CSR_MCOUNTEREN, HCOUNTEREN_IR); 
    CSRS(CSR_SCOUNTEREN, HCOUNTEREN_IR); 

    goto_priv(PRIV_VU);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_INSTRET);

    TEST_ASSERT("vu access to instret successful when mcounteren.ir=1 hcounteren.ir=1 scounteren.ir=1",
        excpt.triggered == false
    );

    TEST_END(); 
}

bool instret_test_8() {

    TEST_START();    
    goto_priv(PRIV_M);
    CSRC(CSR_HCOUNTEREN, HCOUNTEREN_IR); 
    CSRS(CSR_MCOUNTEREN, HCOUNTEREN_IR); 
    CSRS(CSR_SCOUNTEREN, HCOUNTEREN_IR); 

    goto_priv(PRIV_VU);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_INSTRET);

    TEST_ASSERT("vu access to instret casuses virtual instruction exception when mcounteren.ir=1 hcounteren.ir=0 scounteren.ir=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    );

    TEST_END(); 
}

bool instret_test_9() {

    TEST_START();    
    goto_priv(PRIV_M);
    CSRS(CSR_MCOUNTEREN, HCOUNTEREN_IR); 
    CSRC(CSR_SCOUNTEREN, HCOUNTEREN_IR); 

    goto_priv(PRIV_HU);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_INSTRET);

    TEST_ASSERT("hu access to instret casuses illegal instruction exception when mcounteren.ir=1 scounteren.ir=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    );

    TEST_END(); 
}

bool instret_test_10() {

    TEST_START();    
    goto_priv(PRIV_M);
    CSRS(CSR_HCOUNTEREN, HCOUNTEREN_IR); 
    CSRS(CSR_MCOUNTEREN, HCOUNTEREN_IR); 

    goto_priv(PRIV_VS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_INSTRET);

    TEST_ASSERT("vu access to instret successful exception when mcounteren.ir=1 hcounteren.ir=1",
        excpt.triggered == false
    );

    TEST_END(); 
}


bool timecmp_test_1() {

    TEST_START();    
    CSRS(CSR_MENVCFG, 1ULL << 63);  //STCE位 
    CSRC(CSR_MCOUNTEREN, HCOUNTEREN_TM); 

    goto_priv(PRIV_HS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_VSTIMECMP);

    TEST_ASSERT("hs access to vstimecmp casuses illegal instruction exception when menvcfg.stce=1 mcounteren.tm=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    );

    goto_priv(PRIV_HU);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_VSTIMECMP);

    TEST_ASSERT("hu access to vstimecmp casuses illegal instruction exception when menvcfg.stce=1 mcounteren.tm=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    );

    TEST_END(); 
}

bool timecmp_test_2() {

    TEST_START();    
    CSRC(CSR_MENVCFG, 1ULL << 63);  //STCE位 

    goto_priv(PRIV_HS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_VSTIMECMP);

    TEST_ASSERT("hs access to vstimecmp casuses illegal instruction exception when menvcfg.stce=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    );

    TEST_END(); 
}

bool timecmp_test_3() {

    TEST_START();    
    CSRC(CSR_MENVCFG, 1ULL << 63);  //STCE位 

    goto_priv(PRIV_VS); 

    TEST_SETUP_EXCEPT();
    CSRR(CSR_VSTIMECMP);

    TEST_ASSERT("vs access to vstimecmp casuses illegal instruction exception when menvcfg.stce=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    );

    TEST_END(); 
}

bool timecmp_test_4() {

    TEST_START();    
    CSRC(CSR_MENVCFG, 1ULL << 63);  //STCE位 

    goto_priv(PRIV_HU);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_VSTIMECMP);

    TEST_ASSERT("hu access to vstimecmp casuses illegal instruction exception when menvcfg.stce=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    );

    TEST_END(); 
}

bool timecmp_test_5() {

    TEST_START();    
    CSRC(CSR_MENVCFG, 1ULL << 63);  //STCE位 

    goto_priv(PRIV_VU);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_VSTIMECMP);

    TEST_ASSERT("vu access to vstimecmp casuses illegal instruction exception when menvcfg.stce=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    );

    TEST_END(); 
}

bool timecmp_test_6() {

    TEST_START();    
    CSRC(CSR_MCOUNTEREN, HCOUNTEREN_TM); 

    goto_priv(PRIV_HS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_STIMECMP);

    TEST_ASSERT("hs access to stimecmp casuses illegal instruction exception when mcounteren.tm=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    );

    TEST_END(); 
}

bool timecmp_test_7() {

    TEST_START();    
    CSRC(CSR_MCOUNTEREN, HCOUNTEREN_TM); 

    goto_priv(PRIV_VS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_STIMECMP);

    TEST_ASSERT("vs access to stimecmp casuses illegal instruction exception when mcounteren.tm=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    );

    TEST_END(); 
}

bool timecmp_test_8() {

    TEST_START();    

    CSRS(CSR_MENVCFG,  1ULL << 63);     //stce位
    CSRS(CSR_HENVCFG,  1ULL << 63);     //stce位
    CSRS(CSR_MCOUNTEREN, HCOUNTEREN_TM); 
    CSRC(CSR_HCOUNTEREN, HCOUNTEREN_TM); 

    goto_priv(PRIV_VS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_STIMECMP);

    TEST_ASSERT("vs access to stimecmp casuses virtual instruction exception when menvcfg.stce=1 henvcfg.stce=1 mcounteren.tm=1 hcounteren.tm=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    );

    TEST_END(); 
}

bool timecmp_test_9() {

    TEST_START();    
    CSRS(CSR_MENVCFG,  1ULL << 63);     //stce位
    CSRC(CSR_HENVCFG,  1ULL << 63);     //stce位
    CSRS(CSR_MCOUNTEREN, HCOUNTEREN_TM); 
    CSRS(CSR_HCOUNTEREN, HCOUNTEREN_TM); 

    goto_priv(PRIV_VS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_STIMECMP);

    TEST_ASSERT("vs access to stimecmp casuses virtual instruction exception when menvcfg.stce=1 henvcfg.stce=0 mcounteren.tm=1 hcounteren.tm=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    );

    TEST_END(); 
}

bool timecmp_test_10() {

    TEST_START();    
    CSRS(CSR_MENVCFG,  1ULL << 63);     //stce位
    CSRS(CSR_HENVCFG,  1ULL << 63);     //stce位
    CSRS(CSR_MCOUNTEREN, HCOUNTEREN_TM); 
    CSRS(CSR_HCOUNTEREN, HCOUNTEREN_TM); 

    goto_priv(PRIV_VS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_STIMECMP);

    TEST_ASSERT("vs access to stimecmp successful when menvcfg.stce=1 henvcfg.stce=1 mcounteren.tm=1 hcounteren.tm=1",
        excpt.triggered == false
    );

    TEST_END(); 
}

bool timecmp_test_11() {

    TEST_START();    
    CSRS(CSR_MENVCFG,  1ULL << 63);     //stce位
    CSRS(CSR_MCOUNTEREN, HCOUNTEREN_TM); 

    goto_priv(PRIV_HS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_STIMECMP);
    CSRR(CSR_VSTIMECMP);

    TEST_ASSERT("vs access to stimecmp/vstimecmp successful when menvcfg.stce=1 mcounteren.tm=1",
        excpt.triggered == false
    );

    TEST_END(); 
}

bool timecmp_test_12() {

    TEST_START();    
    CSRS(CSR_MENVCFG, 1ULL << 63);  //STCE位 
    CSRS(CSR_MCOUNTEREN, HCOUNTEREN_TM); 

    goto_priv(PRIV_HS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_VSTIMECMP);

    TEST_ASSERT("hs access to vstimecmp successful exception when menvcfg.stce=1 mcounteren.tm=1",
        excpt.triggered == false
    );

    TEST_END(); 
}

bool timecmp_test_13() {

    TEST_START();    
    CSRS(CSR_MCOUNTEREN, HCOUNTEREN_TM); 
    CSRS(CSR_MENVCFG,  1ULL << 63);     //stce位
    CSRC(CSR_HENVCFG,  1ULL << 63);     //stce位
    printf("%llx\n",CSRR(CSR_MENVCFG));    printf("%llx\n",CSRR(CSR_HENVCFG));

    goto_priv(PRIV_VU);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_VSTIMECMP);
    printf("trigger=%d\n",excpt.triggered);
    printf("cause=%d\n",excpt.cause);
    TEST_ASSERT("vu access to vstimecmp casuses virtual instruction exception when menvcfg.stce=1 henvcfg.stce=0 mcounteren.tm=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    );

    TEST_END(); 
}

bool timecmp_test_14() {

    TEST_START();    
    CSRS(CSR_MCOUNTEREN, HCOUNTEREN_TM); 
    CSRS(CSR_MENVCFG,  1ULL << 63);     //stce位
    CSRC(CSR_HENVCFG,  1ULL << 63);     //stce位
    printf("%llx\n",CSRR(CSR_MENVCFG));    printf("%llx\n",CSRR(CSR_HENVCFG));

    goto_priv(PRIV_VS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_VSTIMECMP);
    printf("trigger=%d\n",excpt.triggered);
    printf("cause=%d\n",excpt.cause);
    TEST_ASSERT("vs access to vstimecmp casuses virtual instruction exception when menvcfg.stce=1 henvcfg.stce=0 mcounteren.tm=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    );

    TEST_END(); 
}


bool timecmp_test_15() {

    TEST_START();    
    CSRC(CSR_MCOUNTEREN, HCOUNTEREN_TM); 
    CSRS(CSR_MENVCFG,  1ULL << 63);     //stce位
    CSRC(CSR_HENVCFG,  1ULL << 63);     //stce位
    printf("%llx\n",CSRR(CSR_MENVCFG));    printf("%llx\n",CSRR(CSR_HENVCFG));

    goto_priv(PRIV_VU);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_VSTIMECMP);
    printf("trigger=%d\n",excpt.triggered);
    printf("cause=%d\n",excpt.cause);
    TEST_ASSERT("vu access to vstimecmp casuses illegal instruction exception when menvcfg.stce=1 henvcfg.stce=0 mcounteren.tm=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    );

    TEST_END(); 
}

bool timecmp_test_16() {

    TEST_START();    
    CSRC(CSR_MCOUNTEREN, HCOUNTEREN_TM); 
    CSRS(CSR_MENVCFG,  1ULL << 63);     //stce位
    CSRC(CSR_HENVCFG,  1ULL << 63);     //stce位
    printf("%llx\n",CSRR(CSR_MENVCFG));    printf("%llx\n",CSRR(CSR_HENVCFG));

    goto_priv(PRIV_VS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_VSTIMECMP);
    printf("trigger=%d\n",excpt.triggered);
    printf("cause=%d\n",excpt.cause);
    TEST_ASSERT("vs access to vstimecmp casuses illegal instruction exception when menvcfg.stce=1 henvcfg.stce=0 mcounteren.tm=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    );

    TEST_END(); 
}

bool timecmp_test_17() {

    TEST_START();    
    CSRS(CSR_MCOUNTEREN, HCOUNTEREN_TM); 
    CSRS(CSR_MENVCFG,  1ULL << 63);     //stce位
    CSRC(CSR_HENVCFG,  1ULL << 63);     //stce位
    printf("%llx\n",CSRR(CSR_MENVCFG));    printf("%llx\n",CSRR(CSR_HENVCFG));

    goto_priv(PRIV_VU);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_VSTIMECMP);
    printf("trigger=%d\n",excpt.triggered);
    printf("cause=%d\n",excpt.cause);
    TEST_ASSERT("vu access to vstimecmp casuses virtual instruction exception when menvcfg.stce=1 henvcfg.stce=0 mcounteren.tm=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    );

    TEST_END(); 
}

bool timecmp_test_18() {

    TEST_START();    
    CSRS(CSR_MCOUNTEREN, HCOUNTEREN_TM); 
    CSRS(CSR_MENVCFG,  1ULL << 63);     //stce位
    CSRC(CSR_HENVCFG,  1ULL << 63);     //stce位
    printf("%llx\n",CSRR(CSR_MENVCFG));    printf("%llx\n",CSRR(CSR_HENVCFG));

    goto_priv(PRIV_VS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_VSTIMECMP);
    printf("trigger=%d\n",excpt.triggered);
    printf("cause=%d\n",excpt.cause);
    TEST_ASSERT("vs access to vstimecmp casuses virtual instruction exception when menvcfg.stce=1 henvcfg.stce=0 mcounteren.tm=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    );

    TEST_END(); 
}


bool timecmp_test_19() {

    TEST_START();    
    CSRC(CSR_MCOUNTEREN, HCOUNTEREN_TM); 
    CSRS(CSR_MENVCFG,  1ULL << 63);     //stce位
    CSRC(CSR_HENVCFG,  1ULL << 63);     //stce位
    printf("%llx\n",CSRR(CSR_MENVCFG));    printf("%llx\n",CSRR(CSR_HENVCFG));

    goto_priv(PRIV_VU);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_VSTIMECMP);
    printf("trigger=%d\n",excpt.triggered);
    printf("cause=%d\n",excpt.cause);
    TEST_ASSERT("vu access to vstimecmp casuses illegal instruction exception when menvcfg.stce=1 henvcfg.stce=0 mcounteren.tm=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    );

    TEST_END(); 
}

bool timecmp_test_20() {

    TEST_START();    
    CSRC(CSR_MCOUNTEREN, HCOUNTEREN_TM); 
    CSRS(CSR_MENVCFG,  1ULL << 63);     //stce位
    CSRC(CSR_HENVCFG,  1ULL << 63);     //stce位
    printf("%llx\n",CSRR(CSR_MENVCFG));    printf("%llx\n",CSRR(CSR_HENVCFG));

    goto_priv(PRIV_VS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_VSTIMECMP);
    printf("trigger=%d\n",excpt.triggered);
    printf("cause=%d\n",excpt.cause);
    TEST_ASSERT("vs access to vstimecmp casuses illegal instruction exception when menvcfg.stce=1 henvcfg.stce=0 mcounteren.tm=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    );

    TEST_END(); 
}

bool timecmp_test_21() {

    TEST_START();    
    CSRS(CSR_MENVCFG, 1ULL << 63);  //STCE位 
    CSRC(CSR_MCOUNTEREN, HCOUNTEREN_TM); 

    goto_priv(PRIV_VS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_VSTIMECMP);

    TEST_ASSERT("vs access to vstimecmp casuses illegal instruction exception when menvcfg.stce=1 mcounteren.tm=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    );

    goto_priv(PRIV_VU);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_VSTIMECMP);

    TEST_ASSERT("vu access to vstimecmp casuses illegal instruction exception when menvcfg.stce=1 mcounteren.tm=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    );

    TEST_END(); 
}