#include <rvh_test.h>
#include <page_tables.h>

bool instruction_page_fault_1(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    goto_priv(PRIV_VS);
    vspt_init();


    uintptr_t vaddr;
    uintptr_t addr ;

    goto_priv(PRIV_HS);
    set_prev_priv(PRIV_VS);

    //执行取指指令时，pte.v=0
    vaddr = hs_page_base(VSI_GI);
    TEST_SETUP_EXCEPT();
    TEST_EXEC_EXCEPT(vaddr); 

    TEST_ASSERT("hs mode fetche instruction when pte.v=0 leads to IPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_IPF
    );


    //执行取指指令时，pte.x=0
    vaddr = vs_page_base(VSRW_GRW);
    TEST_SETUP_EXCEPT();
    TEST_EXEC_EXCEPT(vaddr); 

    TEST_ASSERT("hs mode fetche instruction when pte.x=0 leads to IPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_IPF
    );

    TEST_END();
}

bool instruction_page_fault_2(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    goto_priv(PRIV_VS);
    vspt_init();


    goto_priv(PRIV_HS);

    //执行取指指令时，在HS模式下访问U模式可以访问的页表项
    TEST_SETUP_EXCEPT();
    uintptr_t vaddr = hs_page_base(VSURWX_GURWX);

    
    TEST_EXEC_EXCEPT(vaddr); 
    TEST_ASSERT("hs mode fetche instruction of u mode leads to IPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_IPF
    );

    TEST_END();
}



bool instruction_page_fault_3(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    goto_priv(PRIV_VS);
    vspt_init();


    uintptr_t vaddr;
    uintptr_t addr ;

    goto_priv(PRIV_VS);


    //执行取指指令时，pte.v=0
    vaddr = hs_page_base(VSI_GI);
    TEST_SETUP_EXCEPT();
    TEST_EXEC_EXCEPT(vaddr); 

    TEST_ASSERT("vs mode fetch instruction when pte.v=0 leads to IPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_IPF
    );


    //执行取指指令时，pte.x=0
    vaddr = vs_page_base(VSRW_GRW);
    TEST_SETUP_EXCEPT();
    TEST_EXEC_EXCEPT(vaddr); 

    TEST_ASSERT("vs mode fetch instruction when pte.x=0 leads to IPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_IPF
    );

    TEST_END();
}

bool instruction_page_fault_4(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    goto_priv(PRIV_VS);
    vspt_init();


    goto_priv(PRIV_VS);

    //执行取指指令时，在VS模式下访问U模式可以访问的页表项
    TEST_SETUP_EXCEPT();
    uintptr_t vaddr = hs_page_base(VSURWX_GURWX);

    
    TEST_EXEC_EXCEPT(vaddr); 
    TEST_ASSERT("vs mode fetche instruction of u mode leads to IPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_IPF
    );

    TEST_END();
}

bool load_page_fault_1(){

    TEST_START();

    goto_priv(PRIV_HS); 
    hspt_init();
    hpt_init();
    goto_priv(PRIV_VS);
    vspt_init();



    uintptr_t vaddr;
    uintptr_t addr;

    goto_priv(PRIV_HS);
    set_prev_priv(PRIV_VS);

    //执行取指指令时，pte.v=0
    TEST_SETUP_EXCEPT();

    addr = hs_page_base(VSI_GI);
    hlvb(addr);
    TEST_ASSERT("hs mode load when pte.v=0 leads to LPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_LPF
    );


    //执行hlvd指令时，设置了sstatus.MXR=0，只能从只读页面读取，如果此时pte.r=0
    TEST_SETUP_EXCEPT();
    addr = hs_page_base(VSX_GRWX);

    CSRC(CSR_SSTATUS, SSTATUS_MXR);
    hlvd(addr);   
    TEST_ASSERT("hs mode load when sstatus.mxr=0 and pte.r=0 leads to LPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_LPF
    );

    //执行hlvd指令时，设置了sstatus.MXR=1，能从可读可执行页面读取，如果此时页表项pte.r=0&&pte.x=0

    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();
    addr = hs_page_base(VSV_GV);

    CSRS(CSR_SSTATUS, SSTATUS_MXR);
    hlvd(addr);
    TEST_ASSERT("hs mode load when sstatus.mxr=1 and pte.r=0 and pte.x=0 leads to LPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_LPF
    );
    CSRC(CSR_SSTATUS, SSTATUS_MXR);

    TEST_END();

}

bool load_page_fault_2(){

    TEST_START();


    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    goto_priv(PRIV_VS);
    vspt_init();

    goto_priv(PRIV_M);
    CSRC(CSR_VSSTATUS, SSTATUS_SUM);

    goto_priv(PRIV_HS);
    set_prev_priv(PRIV_VS);

    //执行hlvd指令时，设置了vsstatus.SUM=0，在S模式下访问U模式可以访问的页表项
    TEST_SETUP_EXCEPT();
    uintptr_t addr = hs_page_base(VSURWX_GURWX);

    
    hlvd(addr);
    TEST_ASSERT("hs mode hlvd u mode page when vsstatus.sum=0 leads to LPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_LPF
    );

    TEST_END();
}

bool load_page_fault_3(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    goto_priv(PRIV_VS);
    vspt_init();

    goto_priv(PRIV_M);
    CSRS(CSR_VSSTATUS, SSTATUS_SUM);

    //执行hlvb指令时，设置了vsstatus.SUM=1，在S模式下访问U模式可以访问的页表项

    goto_priv(PRIV_HS);
    set_prev_priv(PRIV_VS);
    
    uintptr_t addr = hs_page_base(VSURWX_GURWX);
    TEST_SETUP_EXCEPT();
    hlvb(addr);

    TEST_ASSERT("hs mode hlvb u mode page when vsstatus.sum=1 successful",
        excpt.triggered == false
    );


    //如果pte.a=0，标识上次A位被清除以来，页面被访问过




    //如果pte.a=1，标识上次A位被清除以来，页面未被访问过

    TEST_END();
}


bool load_page_fault_4(){

    TEST_START();

    goto_priv(PRIV_HS); 
    hspt_init();
    hpt_init();
    goto_priv(PRIV_VS);
    vspt_init();



    uintptr_t vaddr;
    uintptr_t addr;

    goto_priv(PRIV_VS);

    //执行取指指令时，pte.v=0
    TEST_SETUP_EXCEPT();

    addr = hs_page_base(VSI_GI);
    lb(addr);
    TEST_ASSERT("vs mode lb when pte.v=0 leads to LPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_LPF
    );


    //执行ld令时，设置了sstatus.MXR=0，只能从只读页面读取，如果此时pte.r=0
    TEST_SETUP_EXCEPT();
    addr = hs_page_base(VSX_GRWX);

    CSRC(CSR_SSTATUS, SSTATUS_MXR);
    ld(addr);   
    TEST_ASSERT("vs mode ld when sstatus.mxr=0 and pte.r=0 leads to LPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_LPF
    );

    //执行lh指令时，设置了sstatus.MXR=1，能从可读可执行页面读取，如果此时页表项pte.r=0&&pte.x=0

    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    addr = hs_page_base(VSV_GV);

    CSRS(CSR_SSTATUS, SSTATUS_MXR);
    lh(addr);
    TEST_ASSERT("vs mode lh when sstatus.mxr=1 and pte.r=0 and pte.x=0 leads to LPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_LPF
    );
    CSRC(CSR_SSTATUS, SSTATUS_MXR);

    TEST_END();

}

bool load_page_fault_5(){

    TEST_START();


    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    goto_priv(PRIV_VS);
    vspt_init();

    goto_priv(PRIV_M);
    CSRC(CSR_VSSTATUS, SSTATUS_SUM);

    goto_priv(PRIV_VS);

    //执行lbu指令时，设置了vsstatus.SUM=0，在S模式下访问U模式可以访问的页表项
    TEST_SETUP_EXCEPT();
    uintptr_t addr = hs_page_base(VSURWX_GURWX);

    
    lbu(addr);
    TEST_ASSERT("vs mode lbu u mode page when vsstatus.sum=0 leads to LPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_LPF
    );

    TEST_END();
}

bool load_page_fault_6(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    goto_priv(PRIV_VS);
    vspt_init();

    goto_priv(PRIV_M);
    CSRS(CSR_VSSTATUS, SSTATUS_SUM);

    //执行lbu指令时，设置了vsstatus.SUM=1，在S模式下访问U模式可以访问的页表项

    goto_priv(PRIV_VS);
    
    uintptr_t addr = hs_page_base(VSURWX_GURWX);
    TEST_SETUP_EXCEPT();
    lbu(addr);

    TEST_ASSERT("vs mode lbu u mode page when vsstatus.sum=1 successful",
        excpt.triggered == false
    );


    TEST_END();
}


bool load_page_fault_7(){

    TEST_START();


    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    goto_priv(PRIV_VS);
    vspt_init();

    goto_priv(PRIV_M);
    CSRC(CSR_MSTATUS, SSTATUS_SUM);

    goto_priv(PRIV_HS);

    //执行hlvd指令时，设置了vsstatus.SUM=0，在S模式下访问U模式可以访问的页表项
    TEST_SETUP_EXCEPT();
    uintptr_t addr = hs_page_base(VSURWX_GURWX);

    
    ld(addr);
    TEST_ASSERT("hs mode ld u mode page when mstatus.sum=0 leads to LPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_LPF
    );

    TEST_END();
}

bool load_page_fault_8(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    goto_priv(PRIV_VS);
    vspt_init();

    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS, SSTATUS_SUM);

    //执行ld指令时，设置了mstatus.SUM=1，在S模式下访问U模式可以访问的页表项

    goto_priv(PRIV_HS);
    
    uintptr_t addr = hs_page_base(VSURWX_GURWX);
    TEST_SETUP_EXCEPT();
    ld(addr);

    TEST_ASSERT("hs mode ld u mode page when mstatus.sum=1 successful",
        excpt.triggered == false
    );


    //如果pte.a=0，标识上次A位被清除以来，页面被访问过




    //如果pte.a=1，标识上次A位被清除以来，页面未被访问过

    TEST_END();
}





bool store_page_fault_1(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    goto_priv(PRIV_VS);
    vspt_init();



    uintptr_t vaddr;
    uintptr_t addr;

    goto_priv(PRIV_HS);
    set_prev_priv(PRIV_VS);

    //执行store指令时，pte.v=0
    TEST_SETUP_EXCEPT();

    addr = hs_page_base(VSI_GI);
    hsvd(addr, 0xdeadbeef);
    TEST_ASSERT("hs mode hsvd when pte.v=0 leads to LPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SPF
    );


    //执行store指令时，如果此时pte.w=0
    TEST_SETUP_EXCEPT();
    addr = hs_page_base(VSRX_GRWX);

    hsvb(addr, 0xdeadbeef);
    TEST_ASSERT("hs mode hsvb when and pte.w=0 leads to LPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SPF
    );

    //执行store指令时，pte.w=1
    TEST_SETUP_EXCEPT();
    addr = hs_page_base(VSRW_GURW);

    hsvh(addr, 0xdeadbeef);
    TEST_ASSERT("hs mode hsvh successful when pte.w=1",
        excpt.triggered == false
    );

    TEST_END();
}

bool store_page_fault_2(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    goto_priv(PRIV_VS);
    vspt_init();

    goto_priv(PRIV_M);
    CSRC(CSR_VSSTATUS, SSTATUS_SUM);

    goto_priv(PRIV_HS);
    set_prev_priv(PRIV_VS);
    //执行hsvd指令时，设置了vsstatus.SUM=0，在S模式下访问U模式可以访问的页表项
    TEST_SETUP_EXCEPT();
    uintptr_t vaddr = hs_page_base(VSURWX_GURWX);

    
    hsvd(vaddr, 0xdeadbeef);
    TEST_ASSERT("hs mode hsvd u mode page when vsstatus.sum=0 leads to LPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SPF
    );


    TEST_END();
}

bool store_page_fault_3(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    goto_priv(PRIV_VS);
    vspt_init();


    //执行hsvb指令时，设置了vsstatus.SUM=1，在S模式下访问U模式可以访问的页表项
    goto_priv(PRIV_M);
    CSRS(CSR_VSSTATUS, SSTATUS_SUM);

    goto_priv(PRIV_HS);
    set_prev_priv(PRIV_VS);
    uintptr_t addr = hs_page_base(VSURWX_GURWX);
    
    TEST_SETUP_EXCEPT();
    hsvb(addr, 0xdeadbeef);
    TEST_ASSERT("hs mode hsvb u mode page when vsstatus.sum=1 successful",
        excpt.triggered == false
    );


    //如果pte.a=0，标识上次A位被清除以来，页面被访问过
    //如果pte.a=1，标识上次A位被清除以来，页面未被访问过

    TEST_END();
}


bool store_page_fault_4(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    goto_priv(PRIV_VS);
    vspt_init();



    uintptr_t vaddr;
    uintptr_t addr;

    goto_priv(PRIV_VS);

    //执行取指指令时，pte.v=0
    TEST_SETUP_EXCEPT();

    addr = hs_page_base(VSI_GI);
    sd(addr, 0xdeadbeef);
    TEST_ASSERT("vs mode sd when pte.v=0 leads to LPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SPF
    );


    //执行sd指令时，如果此时pte.w=0
    TEST_SETUP_EXCEPT();
    addr = hs_page_base(VSRX_GRWX);

    CSRC(CSR_SSTATUS, SSTATUS_MXR);
    sd(addr, 0xdeadbeef);
    TEST_ASSERT("vs mode sd when pte.w=0 leads to LPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SPF
    );

    //执行sd指令时，如果此时pte.w=1
    TEST_SETUP_EXCEPT();
    addr = hs_page_base(VSRW_GURW);

    sd(addr, 0xdeadbeef);
    TEST_ASSERT("vs mode hsvh successful when pte.w=1",
        excpt.triggered == false
    );

    TEST_END();
}

bool store_page_fault_5(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    goto_priv(PRIV_VS);
    vspt_init();

    goto_priv(PRIV_M);
    CSRC(CSR_VSSTATUS, SSTATUS_SUM);

    goto_priv(PRIV_VS);
    //执行sd指令时，设置了vsstatus.SUM=0，在S模式下访问U模式可以访问的页表项
    TEST_SETUP_EXCEPT();
    uintptr_t vaddr = hs_page_base(VSURWX_GURWX);

    
    sd(vaddr, 0xdeadbeef);
    TEST_ASSERT("vs mode sd u mode page when vsstatus.sum=0 leads to LPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SPF
    );


    TEST_END();
}

bool store_page_fault_6(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    goto_priv(PRIV_VS);
    vspt_init();


    //执行hsvb指令时，设置了vsstatus.SUM=1，在S模式下访问U模式可以访问的页表项
    goto_priv(PRIV_M);
    CSRS(CSR_VSSTATUS, SSTATUS_SUM);

    goto_priv(PRIV_VS);
    uintptr_t addr = hs_page_base(VSURWX_GURWX);
    
    TEST_SETUP_EXCEPT();
    sd(addr, 0xdeadbeef);
    TEST_ASSERT("vs mode sd u mode page when vsstatus.sum=1 successful",
        excpt.triggered == false
    );


    //如果pte.a=0，标识上次A位被清除以来，页面被访问过
    //如果pte.a=1，标识上次A位被清除以来，页面未被访问过

    TEST_END();
}

bool store_page_fault_7(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    goto_priv(PRIV_VS);
    vspt_init();

    goto_priv(PRIV_M);
    CSRC(CSR_MSTATUS, SSTATUS_SUM);

    goto_priv(PRIV_HS);
    //执行sd指令时，设置了mstatus.SUM=0，在S模式下访问U模式可以访问的页表项
    TEST_SETUP_EXCEPT();
    uintptr_t vaddr = hs_page_base(VSURWX_GURWX);

    
    sd(vaddr, 0xdeadbeef);
    TEST_ASSERT("hs mode sd u mode page when mstatus.sum=0 leads to LPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SPF
    );


    TEST_END();
}

bool store_page_fault_8(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    goto_priv(PRIV_VS);
    vspt_init();


    //执行sd指令时，设置了msstatus.SUM=1，在S模式下访问U模式可以访问的页表项
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS, SSTATUS_SUM);

    goto_priv(PRIV_HS);
    uintptr_t addr = hs_page_base(VSURWX_GURWX);
    
    TEST_SETUP_EXCEPT();
    sd(addr, 0xdeadbeef);
    TEST_ASSERT("hs mode sd u mode page when mstatus.sum=1 successful",
        excpt.triggered == false
    );


    //如果pte.a=0，标识上次A位被清除以来，页面被访问过
    //如果pte.a=1，标识上次A位被清除以来，页面未被访问过

    TEST_END();
}





bool amo_page_fault_1(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    goto_priv(PRIV_VS);
    vspt_init();



    uintptr_t vaddr;
    uintptr_t addr;

    uint64_t value = 0xdeadbeef;  
    goto_priv(PRIV_HS);
    set_prev_priv(PRIV_VS);

    //执行取指指令时，pte.v=0
    TEST_SETUP_EXCEPT();

    addr = hs_page_base(VSI_GI);
    value = amoadd_w(addr,value);
    TEST_ASSERT("hs mode execute amoadd_w when pte.v=0 leads to SPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SPF
    );


    //执行amoand_d指令时，如果此时pte.w=0
    TEST_SETUP_EXCEPT();
    addr = hs_page_base(VSRX_GRWX);

    value = amoand_d(addr,value);
    TEST_ASSERT("hs mode execute amoand_d when pte.w=0 leads to LPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SPF
    );

    //执行amoand_d指令时，如果此时pte.w=1
    TEST_SETUP_EXCEPT();
    addr = hs_page_base(VSRWX_GURWX);

    value = amomax_d(addr,value);
    TEST_ASSERT("hs mode execute amomax_d successful when pte.w=0",
        excpt.triggered == false
    );

    TEST_END();
}

bool amo_page_fault_2(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    goto_priv(PRIV_VS);
    vspt_init();

    goto_priv(PRIV_M);
    CSRC(CSR_MSTATUS, SSTATUS_SUM);

    goto_priv(PRIV_HS);
    set_prev_priv(PRIV_VS);    
    //执行amomin_d指令时，设置了mstatus.SUM=0，在S模式下访问U模式可以访问的页表项(sstatus配置不进去，需要配置mstatus)
    TEST_SETUP_EXCEPT();
    uintptr_t addr = hs_page_base(VSURWX_GURWX);


    uint64_t value = amomin_d(addr,value);
    TEST_ASSERT("hs mode execute amomin_d of u mode page when sstatus.sum=0 leads to LPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SPF
    );

    TEST_END();
}

bool amo_page_fault_3(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    goto_priv(PRIV_VS);
    vspt_init();

 

    //执行amoadd_w指令时，设置了mstatus.SUM=1，在S模式下访问U模式可以访问的页表项
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS, SSTATUS_SUM);

    uintptr_t addr = hs_page_base(VSURWX_GURWX);

    goto_priv(PRIV_HS);
    set_prev_priv(PRIV_VS);   
    
    TEST_SETUP_EXCEPT();
    uint64_t value = amoadd_w(addr,value);
    TEST_ASSERT("hs mode execute amoadd_w of u mode page when sstatus.sum=1 successful",
        excpt.triggered == false
    );


    //如果pte.a=0，标识上次A位被清除以来，页面被访问过
    //如果pte.a=1，标识上次A位被清除以来，页面未被访问过

    TEST_END();
}

bool amo_page_fault_4(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    goto_priv(PRIV_VS);
    vspt_init();



    uintptr_t vaddr;
    uintptr_t addr;

    uint64_t value = 0xdeadbeef;  
    goto_priv(PRIV_VS);

    //执行取指指令时，pte.v=0
    TEST_SETUP_EXCEPT();

    addr = hs_page_base(VSI_GI);
    value = amoadd_w(addr,value);
    TEST_ASSERT("vs mode execute amoadd_w when pte.v=0 leads to SPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SPF
    );


    //执行amoand_d指令时，如果此时pte.w=0
    TEST_SETUP_EXCEPT();
    addr = hs_page_base(VSRX_GURWX);

    value = amoand_d(addr,value);

    TEST_ASSERT("vs mode execute amoand_d pte.w=0 leads to LPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SPF
    );

    //执行amoand_d指令时，如果此时pte.w=1
    TEST_SETUP_EXCEPT();
    addr = hs_page_base(VSRWX_GURWX);

    value = amomax_d(addr,value);
    TEST_ASSERT("vs mode execute amomax_d successful when pte.w=0",
        excpt.triggered == false
    );

    TEST_END();
}

bool amo_page_fault_5(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    goto_priv(PRIV_VS);
    vspt_init();

    goto_priv(PRIV_M);
    CSRC(CSR_VSSTATUS, SSTATUS_SUM);

    goto_priv(PRIV_VS);
   
    //执行amomin_d指令时，设置了vsstatus.SUM=0，在S模式下访问U模式可以访问的页表项(sstatus配置不进去，需要配置mstatus)
    TEST_SETUP_EXCEPT();
    uintptr_t addr = hs_page_base(VSURWX_GURWX);


    uint64_t value = amomin_d(addr,value);
    TEST_ASSERT("vs mode execute amomin_d of u mode page when vsstatus.sum=0 leads to LPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SPF
    );

    TEST_END();
}

bool amo_page_fault_6(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    goto_priv(PRIV_VS);
    vspt_init();

 

    //执行amoadd_w指令时，设置了mstatus.SUM=1，在S模式下访问U模式可以访问的页表项
    goto_priv(PRIV_M);
    CSRS(CSR_VSSTATUS, SSTATUS_SUM);

    uintptr_t addr = hs_page_base(VSURWX_GURWX);

    goto_priv(PRIV_VS);   
    
    TEST_SETUP_EXCEPT();
    uint64_t value = amoadd_w(addr,value);
    TEST_ASSERT("vs mode execute amoadd_w of u mode page when vsstatus.sum=1 successful",
        excpt.triggered == false
    );


    //如果pte.a=0，标识上次A位被清除以来，页面被访问过
    //如果pte.a=1，标识上次A位被清除以来，页面未被访问过

    TEST_END();
}
