#include <rvh_test.h>
#include <page_tables.h>
#include <csrs.h> 

bool ebreak_tests_1() {

    TEST_START();

    //m模式下执行ebreak指令
    goto_priv(PRIV_M);
    TEST_SETUP_EXCEPT();
    
    ebreak();

    TEST_ASSERT("m mode ebreak cause to Breakpoint",
        excpt.triggered == true &&
        excpt.cause == CAUSE_BKP
    ); 

    TEST_END();

}

bool ebreak_tests_2() {

    TEST_START();

    //vs模式下执行ebreak指令
    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();

    ebreak();

    TEST_ASSERT("vs mode ebreak cause to Breakpoint",
        excpt.triggered == true &&
        excpt.cause == CAUSE_BKP
    ); 

    TEST_END();

}

bool ebreak_tests_3() {

    TEST_START();

    //HS模式下执行ebreak指令
    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();

    ebreak();

    TEST_ASSERT("hs mode ebreak cause to Breakpoint",
        excpt.triggered == true &&
        excpt.cause == CAUSE_BKP
    ); 

    TEST_END();

}

bool ebreak_tests_4() {

    TEST_START();

    //HU模式下执行ebreak指令
    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();

    ebreak();

    TEST_ASSERT("hu mode ebreak cause to Breakpoint",
        excpt.triggered == true &&
        excpt.cause == CAUSE_BKP
    ); 

    TEST_END();

}

bool ebreak_tests_5() {

    TEST_START();

    //VU模式下执行ebreak指令
    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();

    ebreak();

    TEST_ASSERT("vu mode ebreak cause to Breakpoint",
        excpt.triggered == true &&
        excpt.cause == CAUSE_BKP
    ); 

    TEST_END();

}