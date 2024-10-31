 #include <rvh_test.h>
#include <page_tables.h>

bool load_guest_page_fault_1(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();



    uintptr_t vaddr;
    uintptr_t addr;

    goto_priv(PRIV_HS);
    set_prev_priv(PRIV_VS);

    //执行取指相关指令二级地址翻译阶段时,第二阶段地址翻译pte.v=0
    TEST_SETUP_EXCEPT();

    vaddr = hs_page_base(VSRWX_GI);
    hlvd(vaddr);
    TEST_ASSERT("hs mode hlvd when the second-stage address translation pte.v=0 leads to LGPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_LGPF
    );


    //执行hlvb指令需要二级地址翻页时，第二阶段地址翻译pte.r=0&&pte.w=1
    TEST_SETUP_EXCEPT();
    vaddr = hs_page_base(VSRWX_GI);

    hlvb(vaddr);   
    TEST_ASSERT("hs mode hlvb when the second-stage address translation pte.v=0 and pte.r=0 leads to LGPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_LGPF
    );

    TEST_END();
}

bool load_guest_page_fault_2(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();



    uintptr_t vaddr;
    uintptr_t addr;

    goto_priv(PRIV_HS);
    set_prev_priv(PRIV_VS);

    //执行hlvh指令需要二级地址翻页时，设置了sstatus.MXR=0，只能从只读页面读取，如果此时第二阶段地址翻译pte.r=0
    TEST_SETUP_EXCEPT();
    vaddr = hs_page_base(VSRWX_GI);


    CSRC(CSR_SSTATUS, SSTATUS_MXR);
    hlvh(vaddr);
    TEST_ASSERT("hs mode hlvh when the second-stage address translation pte.r=0 and sstatus.mxr=0 leads to LGPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_LGPF
    );

    TEST_END();
}

bool load_guest_page_fault_3(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();

    uintptr_t vaddr;
    uintptr_t addr;

    goto_priv(PRIV_HS);
    set_prev_priv(PRIV_VS);
    //执行hlvh指令需要二级地址翻页时，设置了sstatus.MXR=1，能从可读可执行页面读取，如果此时第二阶段地址翻译pte.r=0&&pte.x=0
    TEST_SETUP_EXCEPT();
    vaddr = hs_page_base(VSRWX_GI);

    CSRS(CSR_SSTATUS, SSTATUS_MXR);
    hlvh(vaddr);
    TEST_ASSERT("hs mode hlvh when the second-stage address translation pte.r=0 and pte.x=0 and sstatus.mxr=1 leads to LGPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_LGPF
    );
    TEST_END();
}

bool load_guest_page_fault_4(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();



    uintptr_t vaddr;
    uintptr_t addr;
    goto_priv(PRIV_M);
    CSRC(CSR_VSSTATUS, SSTATUS_SUM);
    

    goto_priv(PRIV_HS);
    set_prev_priv(PRIV_VS);
    //执行hlvd指令需要二级地址翻页时，设置了vsstatus.SUM=0，在S模式下访问U模式可以访问的页表项
    TEST_SETUP_EXCEPT();
    vaddr = hs_page_base(VSURWX_GURWX);


    hlvb(vaddr);
    TEST_ASSERT("hs mode hlvb u mode page when vsstatus.sum=0 leads to LPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_LPF
    );

    TEST_END();
}

bool load_guest_page_fault_5(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();



    uintptr_t vaddr;
    uintptr_t addr;

    goto_priv(PRIV_M);
    CSRS(CSR_VSSTATUS, SSTATUS_SUM);

    goto_priv(PRIV_HS);
    set_prev_priv(PRIV_VS);

    //执行hlvb指令时，设置了vsstatus.SUM=1，在S模式下访问U模式可以访问的页表项

    vaddr = hs_page_base(VSURWX_GURWX);
    
    TEST_SETUP_EXCEPT();
    hlvb(vaddr);
    TEST_ASSERT("hs mode hlvb u mode page when vsstatus.sum=1 successful",
        excpt.triggered == false
    );

    //如果pte.a=0，标识上次A位被清除以来，页面被访问过
    //如果pte.a=1，标识上次A位被清除以来，页面未被访问过

    TEST_END();
}

bool load_guest_page_fault_6(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();



    uintptr_t vaddr;
    uintptr_t addr;

    goto_priv(PRIV_VS);

    //执行lb指令二级地址翻译阶段时,第二阶段地址翻译pte.v=0
    TEST_SETUP_EXCEPT();

    vaddr = hs_page_base(VSRWX_GI);
    lb(vaddr);
    TEST_ASSERT("vs mode lb when the second-stage address translation pte.v=0 leads to LGPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_LGPF
    );


    //执行ld指令需要二级地址翻页时，第二阶段地址翻译pte.r=0&&pte.w=1
    TEST_SETUP_EXCEPT();
    vaddr = hs_page_base(VSRWX_GI);

    ld(vaddr);   
    TEST_ASSERT("vs mode ld when the second-stage address translation pte.v=0 and pte.r=0 leads to LGPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_LGPF
    );

    TEST_END();
}

bool load_guest_page_fault_7(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();



    uintptr_t vaddr;
    uintptr_t addr;

    goto_priv(PRIV_VS);

    //执行lh指令需要二级地址翻页时，设置了sstatus.MXR=0，只能从只读页面读取，如果此时第二阶段地址翻译pte.r=0
    TEST_SETUP_EXCEPT();
    vaddr = hs_page_base(VSRWX_GI);


    CSRC(CSR_SSTATUS, SSTATUS_MXR);
    lh(vaddr);
    TEST_ASSERT("vs mode lh when the second-stage address translation pte.r=0 and sstatus.mxr=0 leads to LGPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_LGPF
    );

    TEST_END();
}

bool load_guest_page_fault_8(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();

    uintptr_t vaddr;
    uintptr_t addr;

    goto_priv(PRIV_VS);

    //执行hlvh指令需要二级地址翻页时，设置了sstatus.MXR=1，能从可读可执行页面读取，如果此时第二阶段地址翻译pte.r=0&&pte.x=0
    TEST_SETUP_EXCEPT();
    vaddr = hs_page_base(VSRWX_GI);

    CSRS(CSR_SSTATUS, SSTATUS_MXR);
    lh(vaddr);
    TEST_ASSERT("vs mode lh when the second-stage address translation pte.r=0 and pte.x=0 and sstatus.mxr=1 leads to LGPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_LGPF
    );
    TEST_END();
}

bool load_guest_page_fault_9(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();



    uintptr_t vaddr;
    uintptr_t addr;
    goto_priv(PRIV_M);
    CSRC(CSR_VSSTATUS, SSTATUS_SUM);
    

    goto_priv(PRIV_VS);
    //执行lb指令需要二级地址翻页时，设置了vsstatus.SUM=0，在S模式下访问U模式可以访问的页表项
    TEST_SETUP_EXCEPT();
    vaddr = hs_page_base(VSURWX_GURWX);


    lb(vaddr)  ;
    TEST_ASSERT("vs mode lb u mode page when vsstatus.sum=0 leads to LPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_LPF
    );

    TEST_END();
}

bool load_guest_page_fault_10(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();



    uintptr_t vaddr;
    uintptr_t addr;

    goto_priv(PRIV_M);
    CSRS(CSR_VSSTATUS, SSTATUS_SUM);

    goto_priv(PRIV_VS);

    //执行lb指令时，设置了vsstatus.SUM=1，在S模式下访问U模式可以访问的页表项

    vaddr = hs_page_base(VSURWX_GURWX);
    
    TEST_SETUP_EXCEPT();
    lb(vaddr);

    TEST_ASSERT("vs mode lb u mode page when vsstatus.sum=1 successful",
        excpt.triggered == false
    );

    //如果pte.a=0，标识上次A位被清除以来，页面被访问过
    //如果pte.a=1，标识上次A位被清除以来，页面未被访问过

    TEST_END();
}



bool load_guest_page_fault_11(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();

    uintptr_t vaddr;
    uintptr_t addr;

    goto_priv(PRIV_M);
    CSRW(CSR_VSATP,(uint64_t)0);

    goto_priv(PRIV_HS);
    set_prev_priv(PRIV_VS);

    //特权模式在HS模式下，执行取指相关指令，设置vsatp=0（禁用VS阶段翻译），hgatp配置sv39模式，第二阶段地址翻译pte.v=0
    TEST_SETUP_EXCEPT();

    vaddr = vs_page_base(VSI_GI);
    hlvd(vaddr);

    TEST_ASSERT("hs mode hlvd when the second-stage address translation pte.v=0 leads to LGPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_LGPF
    );



    TEST_END();
}

bool load_guest_page_fault_12(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    // vspt_init();



    uintptr_t vaddr;
    uintptr_t addr;

    goto_priv(PRIV_M);
    CSRW(CSR_VSATP,(uint64_t)0);

    goto_priv(PRIV_HS);
    set_prev_priv(PRIV_VS);

    //特权模式在HS模式下，执行取指相关指令，设置vsatp=0（禁用VS阶段翻译），hgatp配置sv39模式，设置了sstatus.MXR=0，只能从只读页面读取，如果此时第二阶段地址翻译pte.r=0
    TEST_SETUP_EXCEPT();
    vaddr = vs_page_base(VSI_GUX);

    CSRC(CSR_SSTATUS, SSTATUS_MXR);
    hlvh(vaddr);

    printf("%llx\n",vaddr);
    printf("%d\n",excpt.cause);
    printf("%d\n",excpt.triggered);
    TEST_ASSERT("hs mode hlvh when the second-stage address translation pte.r=0 and sstatus.mxr=0 leads to LGPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_LGPF
    );

    TEST_END();
}

bool load_guest_page_fault_13(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();

    uintptr_t vaddr;
    uintptr_t addr;

    goto_priv(PRIV_M);
    CSRW(CSR_VSATP,(uint64_t)0);

    goto_priv(PRIV_HS);
    set_prev_priv(PRIV_VS);
    //特权模式在HS模式下，执行取指相关指令，设置vsatp=0（禁用VS阶段翻译），hgatp配置sv39模式，设置了sstatus.MXR=1，能从可读可执行页面读取，如果此时第二阶段地址翻译pte.r=0&&pte.x=0
    TEST_SETUP_EXCEPT();
    vaddr = vs_page_base(VSI_GI);

    CSRS(CSR_SSTATUS, SSTATUS_MXR);
    hlvh(vaddr);
    TEST_ASSERT("hs mode hlvh when the second-stage address translation pte.r=0 and pte.x=0 and sstatus.mxr=1 leads to LGPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_LGPF
    );
    TEST_END();
}

bool load_guest_page_fault_14(){

    TEST_START();

    goto_priv(PRIV_HS);
    hpt_init();
    hspt_init();
    vspt_init();



    uintptr_t vaddr;
    uintptr_t addr;
    goto_priv(PRIV_M);
    CSRC(CSR_VSSTATUS, SSTATUS_SUM);
    CSRW(CSR_VSATP,(uint64_t)0);
    
    goto_priv(PRIV_HS);
    set_prev_priv(PRIV_VS);

    //特权模式在HS模式下，执行取指相关指令，设置vsatp=0（禁用VS阶段翻译），hgatp配置sv39模式，执行hlvd指令需要二级地址翻页时，设置了vsstatus.SUM=0，在S模式下访问U模式可以访问的页表项
    TEST_SETUP_EXCEPT();
    vaddr = vs_page_base(VSI_GURWX);


    hlvb(vaddr);
    printf("%d\n",excpt.cause);
    printf("%d\n",excpt.triggered);
    TEST_ASSERT("hs mode hlvb u mode page when vsstatus.sum=0 leads to LGPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_LGPF
    );

    TEST_END();
}

bool load_guest_page_fault_15(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();



    uintptr_t vaddr;
    uintptr_t addr;

    goto_priv(PRIV_M);
    CSRS(CSR_VSSTATUS, SSTATUS_SUM);
    CSRW(CSR_VSATP,(uint64_t)0);

    goto_priv(PRIV_HS);
    set_prev_priv(PRIV_VS);

    //特权模式在HS模式下，执行取指相关指令，设置vsatp=0（禁用VS阶段翻译），hgatp配置sv39模式，执行hlvb指令时，设置了vsstatus.SUM=1，在S模式下访问U模式可以访问的页表项

    vaddr = vs_page_base(VSURWX_GURWX);
    
    TEST_SETUP_EXCEPT();
    hlvb(vaddr);

    TEST_ASSERT("hs mode hlvb u mode page when vsstatus.sum=1 successful",
        excpt.triggered == false
    );


    TEST_END();
}



bool store_guest_page_fault_1(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();



    uintptr_t vaddr;
    uintptr_t addr;

    goto_priv(PRIV_HS);
    set_prev_priv(PRIV_VS);

    //执行store指令二级地址翻译阶段时,第二阶段地址翻译pte.v=0
    TEST_SETUP_EXCEPT();

    vaddr = hs_page_base(VSRWX_GI);
    hsvd(vaddr,0xdeadbeef);
    TEST_ASSERT("hs mode hsvd when the second-stage address translation pte.v=0 leads to SGPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SGPF
    );


    //执行store相关指令需要二级地址翻页时，第二阶段地址翻译pte.r=0&&pte.w=1
    TEST_SETUP_EXCEPT();
    addr = hs_page_base(VSRWX_GI);

    hsvh(addr,0xbeef);   
    TEST_ASSERT("hs mode hsvh when the second-stage address translation pte.v=0 and pte.r=0 leads to SGPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SGPF
    );

    TEST_END();

}

bool store_guest_page_fault_2(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();



    uintptr_t vaddr;
    uintptr_t addr;

    goto_priv(PRIV_HS);
    set_prev_priv(PRIV_VS);

    //执行store相关指令需要二级地址翻页时，设置了sstatus.MXR=0，只能从只读页面读取，如果此时第二阶段地址翻译pte.r=0
    TEST_SETUP_EXCEPT();
    vaddr = hs_page_base(VSRWX_GI);

    CSRC(CSR_SSTATUS, SSTATUS_MXR);
    hsvd(vaddr,0xdeadbeef);
    TEST_ASSERT("hs mode hsvd when the second-stage address translation pte.r=0 and sstatus.mxr=0 leads to SGPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SGPF
    );

    TEST_END();
}

bool store_guest_page_fault_3(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();



    uintptr_t vaddr;
    uintptr_t addr;

    goto_priv(PRIV_HS);
    set_prev_priv(PRIV_VS);


    //执行store相关指令需要二级地址翻页时，设置了sstatus.MXR=1，能从可读可执行页面读取，如果此时第二阶段地址翻译pte.r=0&&pte.x=0
    TEST_SETUP_EXCEPT();
    vaddr = hs_page_base(VSRWX_GI);

    CSRS(CSR_SSTATUS, SSTATUS_MXR);
    hsvd(vaddr,0xdeadbeef);
    TEST_ASSERT("hs mode hsvd when the second-stage address translation pte.r=0 and pte.x=0 and sstatus.mxr=1 leads to SGPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SGPF
    );
    CSRC(CSR_SSTATUS, SSTATUS_MXR);

    TEST_END();
}

bool store_guest_page_fault_4(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();



    uintptr_t vaddr;
    uintptr_t addr;

    goto_priv(PRIV_M);
    CSRC(CSR_VSSTATUS, SSTATUS_SUM);

    goto_priv(PRIV_HS);
    set_prev_priv(PRIV_VS);

    //执行hsvd指令需要二级地址翻页时，设置了vsstatus.SUM=0，在S模式下访问U模式可以访问的页表项
    TEST_SETUP_EXCEPT();
    vaddr = hs_page_base(VSURWX_GUX);


    hsvd(vaddr,0xdeadbeef);
    TEST_ASSERT("hs mode hsvd u mode page when vsstatus.sum=0 leads to SPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SPF
    );

    TEST_END();
}

bool store_guest_page_fault_5(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();

    goto_priv(PRIV_M);
    CSRS(CSR_VSSTATUS, SSTATUS_SUM);

    uintptr_t vaddr;
    uintptr_t addr;

    goto_priv(PRIV_HS);
    set_prev_priv(PRIV_VS);

    //执行hsvd指令时，设置了vsstatus.SUM=1，在S模式下访问U模式可以访问的页表项
    vaddr = hs_page_base(VSURWX_GURWX);
    
    TEST_SETUP_EXCEPT();
    hsvb(vaddr,0xdeadbeef);
    TEST_ASSERT("hs mode hsvd u mode page when vsstatus.sum=1 successful",
        excpt.triggered == false
    );


    //如果pte.d=0，标识上次A位被清除以来，页面被访问过
    //如果pte.d=1，标识上次A位被清除以来，页面未被访问过

    TEST_END();
}

bool store_guest_page_fault_6(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();



    uintptr_t vaddr;
    uintptr_t addr;

    goto_priv(PRIV_VS);

    //执行store指令二级地址翻译阶段时,第二阶段地址翻译pte.v=0
    TEST_SETUP_EXCEPT();

    vaddr = hs_page_base(VSRWX_GI);
    sd(vaddr,0xdeadbeef);
    TEST_ASSERT("vs mode sd when the second-stage address translation pte.v=0 leads to SGPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SGPF
    );


    //执行store相关指令需要二级地址翻页时，第二阶段地址翻译pte.r=0&&pte.w=1
    TEST_SETUP_EXCEPT();
    addr = hs_page_base(VSRWX_GI);

    sh(addr,0xbeef);   
    TEST_ASSERT("vs mode sh when the second-stage address translation pte.v=0 and pte.r=0 leads to SGPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SGPF
    );

    TEST_END();

}

bool store_guest_page_fault_7(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();



    uintptr_t vaddr;
    uintptr_t addr;

    goto_priv(PRIV_VS);

    //执行store相关指令需要二级地址翻页时，设置了sstatus.MXR=0，只能从只读页面读取，如果此时第二阶段地址翻译pte.r=0
    TEST_SETUP_EXCEPT();
    vaddr = hs_page_base(VSRWX_GI);

    CSRC(CSR_SSTATUS, SSTATUS_MXR);
    sd(vaddr,0xdeadbeef);
    TEST_ASSERT("vs mode sd when the second-stage address translation pte.r=0 and sstatus.mxr=0 leads to SGPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SGPF
    );

    TEST_END();
}

bool store_guest_page_fault_8(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();



    uintptr_t vaddr;
    uintptr_t addr;

    goto_priv(PRIV_VS);


    //执行store相关指令需要二级地址翻页时，设置了sstatus.MXR=1，能从可读可执行页面读取，如果此时第二阶段地址翻译pte.r=0&&pte.x=0
    TEST_SETUP_EXCEPT();
    vaddr = hs_page_base(VSRWX_GI);

    CSRS(CSR_SSTATUS, SSTATUS_MXR);
    sh(vaddr,0xbeef);
    TEST_ASSERT("vs mode sh when the second-stage address translation pte.r=0 and pte.x=0 and sstatus.mxr=1 leads to SGPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SGPF
    );
    CSRC(CSR_SSTATUS, SSTATUS_MXR);

    TEST_END();
}

bool store_guest_page_fault_9(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();



    uintptr_t vaddr;
    uintptr_t addr;

    goto_priv(PRIV_M);
    CSRC(CSR_VSSTATUS, SSTATUS_SUM);

    goto_priv(PRIV_VS);

    //执行hsvd指令需要二级地址翻页时，设置了vsstatus.SUM=0，在S模式下访问U模式可以访问的页表项
    TEST_SETUP_EXCEPT();
    vaddr = hs_page_base(VSURWX_GUX);


    sd(vaddr,0xdeadbeef);
    TEST_ASSERT("hs mode sd u mode page when vsstatus.sum=0 leads to SPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SPF
    );

    TEST_END();
}

bool store_guest_page_fault_10(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();

    goto_priv(PRIV_M);
    CSRS(CSR_VSSTATUS, SSTATUS_SUM);

    uintptr_t vaddr;
    uintptr_t addr;

    goto_priv(PRIV_VS);

    //执行hsvd指令时，设置了vsstatus.SUM=1，在S模式下访问U模式可以访问的页表项
    vaddr = hs_page_base(VSURWX_GURWX);
    
    TEST_SETUP_EXCEPT();
    sd(vaddr,0xdeadbeef);
    TEST_ASSERT("vs mode sd u mode page when vsstatus.sum=1 successful",
        excpt.triggered == false
    );


    //如果pte.d=0，标识上次A位被清除以来，页面被访问过
    //如果pte.d=1，标识上次A位被清除以来，页面未被访问过

    TEST_END();
}

bool amo_guest_page_fault_1(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();



    uintptr_t vaddr;
    uintptr_t addr;

    uint64_t value = 0x1;  
    goto_priv(PRIV_VS);


    //执行原子指令二级地址翻译阶段时,第二阶段地址翻译pte.v=0
    TEST_SETUP_EXCEPT();


    addr = hs_page_base(VSRWX_GI);
    value = amoadd_w(addr,value);
    
    TEST_ASSERT("s mode amoadd_w when the second-stage address translation pte.v=0 leads to SGPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SGPF
    );


    //执行原子相关指令需要二级地址翻页时，第二阶段地址翻译pte.r=0&&pte.w=1
    TEST_SETUP_EXCEPT();
    vaddr = hs_page_base(VSRWX_GI);
    value = amomax_d(vaddr,value);

    TEST_ASSERT("s mode amomax_d when the second-stage address translation pte.v=0 and pte.r=0 leads to SGPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SGPF
    );

    TEST_END();
}

bool amo_guest_page_fault_2(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();



    uintptr_t vaddr;
    uintptr_t addr;

    uint64_t value = 0xdeadbeef;  
    goto_priv(PRIV_VS);
    //执行原子相关指令需要二级地址翻页时，设置了sstatus.MXR=0，只能从只读页面读取，如果此时第二阶段地址翻译pte.r=0
    TEST_SETUP_EXCEPT();
    addr = hs_page_base(VSRWX_GI);

    CSRC(CSR_SSTATUS, SSTATUS_MXR);
    value = amoswap_d(addr,value);
    TEST_ASSERT("s mode amoswap_d when the second-stage address translation pte.r=0 and sstatus.mxr=0 leads to SGPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SGPF
    );

    TEST_END();
}

bool amo_guest_page_fault_3(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();



    uintptr_t vaddr;
    uintptr_t addr;

    uint64_t value = 0xdeadbeef;  
    goto_priv(PRIV_VS);


    //执行原子相关指令需要二级地址翻页时，设置了sstatus.MXR=1，能从可读可执行页面读取，如果此时第二阶段地址翻译pte.r=0&&pte.x=0
    TEST_SETUP_EXCEPT();
    vaddr = hs_page_base(VSRWX_GI);

    CSRS(CSR_SSTATUS, SSTATUS_MXR);
    value = amoor_d(vaddr,value);
    TEST_ASSERT("s mode amoor_d when the second-stage address translation pte.r=0 and pte.x=0 and sstatus.mxr=1 leads to SGPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SGPF
    );

    CSRC(CSR_SSTATUS, SSTATUS_MXR);

    TEST_END();
}

bool amo_guest_page_fault_4(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();



    uintptr_t vaddr;
    uintptr_t addr;

    uint64_t value = 0xdeadbeef;  

    goto_priv(PRIV_M);
    CSRC(CSR_MSTATUS, SSTATUS_SUM);

    goto_priv(PRIV_HS);


    //执行原子相关指令需要二级地址翻页时，设置了mstatus.SUM=0，在S模式下访问U模式可以访问的页表项
    TEST_SETUP_EXCEPT();
    addr = hs_page_base(VSURWX_GUX);

    value = amomin_d(addr,value);
    TEST_ASSERT("s mode amomin_d u mode page when mstatus.sum=0 leads to SPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SPF
    );

    TEST_END();
}

bool amo_guest_page_fault_5(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();



    uintptr_t vaddr;
    uintptr_t addr;

    uint64_t value = 0xdeadbeef;  
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS, SSTATUS_SUM);

    goto_priv(PRIV_HS);


    //执行原子指令时，设置了mstatus.SUM=1，在S模式下访问U模式可以访问的页表项

    vaddr = hs_page_base(VSURWX_GURWX);
    
    TEST_SETUP_EXCEPT();
    value = amominu_d(vaddr,value);

    TEST_ASSERT("s mode amominu_d u mode page when mstatus.sum=1 successful",
        excpt.triggered == false
    );


    //如果pte.d=0，标识上次A位被清除以来，页面被访问过
    //如果pte.d=1，标识上次A位被清除以来，页面未被访问过

    TEST_END();
}


bool instruction_guest_page_fault_1(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();


    uintptr_t vaddr;
    uintptr_t addr ;

    goto_priv(PRIV_VS);

    //执行取指指令二级地址翻译阶段时，pte.v=0
    vaddr = hs_page_base(VSRWX_GI);
    TEST_SETUP_EXCEPT();
    TEST_EXEC_EXCEPT(vaddr); 


    TEST_ASSERT("vs mode fetche instruction when the second-stage address translation pte.v=0 leads to IGPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_IGPF
    );

    TEST_END();

}


bool instruction_guest_page_fault_2(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();


    uintptr_t vaddr;
    uintptr_t addr ;

    goto_priv(PRIV_VS);


    //执行取指指令二级地址翻译阶段时，pte.x=0
    vaddr = vs_page_base(VSRWX_GRW);
    TEST_SETUP_EXCEPT();
    TEST_EXEC_EXCEPT(vaddr); 

    TEST_ASSERT("vs mode fetche instruction when the second-stage address translation pte.x=0 leads to IGPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_IGPF
    );

    TEST_END();
}

bool instruction_guest_page_fault_3(){

    TEST_START();

    CSRC(CSR_VSSTATUS,SSTATUS_SUM);

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();


    uintptr_t vaddr;
    uintptr_t addr ;

    goto_priv(PRIV_VS);

    //执行取指指令时，在S模式下访问U模式可以访问的页表项
    TEST_SETUP_EXCEPT();
    vaddr = hs_page_base(VSURWX_GURWX);


    TEST_EXEC_EXCEPT(vaddr); 
    TEST_ASSERT("vs mode fetche instruction of u mode leads to IPF when vsstatus.sum=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_IPF
    );

    TEST_END();
}


bool instruction_guest_page_fault_4(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();


    uintptr_t vaddr;
    uintptr_t addr ;

    goto_priv(PRIV_M);
    CSRW(CSR_VSATP,(uint64_t)0);
    goto_priv(PRIV_VS);


    //特权模式在VS模式下，执行取指相关指令，设置vsatp=0（禁用VS阶段翻译），hgatp配置sv39模式，G阶段地址翻译一级页表项没有执行权限的页面中执行，pte.x=0
    addr = vs_page_base(VSURWX_GRW);
    TEST_SETUP_EXCEPT();
    TEST_EXEC_EXCEPT(addr); 

    printf("%d\n",excpt.cause);
    printf("%d\n",excpt.triggered);

    TEST_ASSERT("vs mode fetche instruction when the only second-stage address translation pte.x=0 leads to IGPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_IGPF
    );

    TEST_END();
}


bool instruction_guest_page_fault_5(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();


    uintptr_t vaddr;
    uintptr_t addr ;
    goto_priv(PRIV_M);
    CSRW(CSR_VSATP,(uint64_t)0);

    goto_priv(PRIV_VS);

    //特权模式在VS模式下，执行取指相关指令，设置vsatp=0（禁用VS阶段翻译），hgatp配置sv39模式，G阶段地址翻译一级页表项无效，pte.v=0
    vaddr = vs_page_base(VSI_GI);
    TEST_SETUP_EXCEPT();
    TEST_EXEC_EXCEPT(vaddr); 


    TEST_ASSERT("vs mode fetche instruction when the only second-stage address translation and pte.v=0 leads to IGPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_IGPF
    );

    TEST_END();

}



bool instruction_guest_page_fault_6(){

    TEST_START();
    // CSRS(CSR_VSSTATUS, SSTATUS_SUM);
    // CSRS(CSR_MSTATUS, SSTATUS_SUM);


    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();


    uintptr_t vaddr;
    uintptr_t addr ;


    goto_priv(PRIV_VS);

    //执行取指指令时，在VS模式下
    vaddr = vs_page_base(VSRWX_GURWX);
    
    TEST_SETUP_EXCEPT();
    TEST_EXEC_EXCEPT(vaddr); 

    TEST_ASSERT("vs mode fetche instruction successful when pte.x=1",
        excpt.triggered == false
    );


    TEST_END();
}

bool instruction_guest_page_fault_7(){

    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();


    uintptr_t vaddr;
    uintptr_t addr ;

    goto_priv(PRIV_VS);


    //执行取指指令二级地址翻译阶段时，pte.x=0
    vaddr = vs_page_base(VSRWX_GRW);
    TEST_SETUP_EXCEPT();
    TEST_EXEC_EXCEPT(vaddr); 

    TEST_ASSERT("vs mode first fetche instruction when the second-stage address translation pte.x=0 leads to IGPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_IGPF
    );

    TEST_SETUP_EXCEPT();
    TEST_EXEC_EXCEPT(vaddr); 

    TEST_ASSERT("vs mode second fetche instruction when the second-stage address translation pte.x=0 leads to IGPF",
        excpt.triggered == true &&
        excpt.cause == CAUSE_IGPF
    );

    TEST_END();
}