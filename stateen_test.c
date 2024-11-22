#include <rvh_test.h>


bool stateen_C_test_1(){

    TEST_START();
    
    goto_priv(PRIV_M);
    //当mstateen.C=0,hu mode 不可访问自定义寄存器
    CSRC(CSR_MSTATEEN0 , MSTATEEN_C);

    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();
    CSRR(0X5C2);

    excpt_info();

    TEST_ASSERT("HU mode accesss Custom register cause to illegal except when mstateen.c=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 

    TEST_END();

}

bool stateen_C_test_2(){

    TEST_START();
    
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATEEN0 , MSTATEEN_C);
    CSRC(CSR_HSTATEEN0 , MSTATEEN_C);

    //当mstateen.C=1,hstateen.C=0,v=1时不可访问自定义寄存器
    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    CSRR(0X5C3);

    excpt_info();

    TEST_ASSERT("VU mode accesss Custom register cause to VRTI when mstateen.c=1 hstateen.c=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 


    TEST_END();

}

bool stateen_C_test_3(){

    //当mstateen.C=0,m mode 可访问自定义寄存器

    TEST_START();
    
    goto_priv(PRIV_M);
    CSRC(CSR_MSTATEEN0 , MSTATEEN_C);
    goto_priv(PRIV_M);
    TEST_SETUP_EXCEPT();
    CSRR(0X5C1);

    excpt_info();

    TEST_ASSERT("m mode accesss Custom register success  when mstateen.c=0",
        excpt.triggered == false
    ); 

    TEST_END();

}

bool stateen_C_test_4(){

    //当mstateen.C=1,hstateen.C=0,v=1时不可访问自定义寄存器

    TEST_START();
    
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATEEN0 , MSTATEEN_C);
    CSRC(CSR_HSTATEEN0 , MSTATEEN_C);
    printf("mstateen=%llx\n",CSRR(CSR_MSTATEEN0));
    printf("hstateen=%llx\n",CSRR(CSR_HSTATEEN0));

    goto_priv(PRIV_VS);

    TEST_SETUP_EXCEPT();
    CSRR(0X5C4);

    excpt_info();

    TEST_ASSERT("vs mode accesss Custom register cause to VRTI when mstateen.c=1 hstateen.c=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 

    TEST_END();

}

bool stateen_C_test_5(){

    //当mstateen.C=0,hs mode 不可访问自定义寄存器

    TEST_START();
    
    goto_priv(PRIV_M);
    CSRC(CSR_MSTATEEN0 , MSTATEEN_C);

    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();
    CSRR(0X5C0);

    excpt_info();

    TEST_ASSERT("hs mode accesss Custom register cause to illegal except when mstateen.c=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 

    TEST_END();

}


bool stateen_C_test_6(){

    TEST_START();
    
    goto_priv(PRIV_M);
    //当mstateen.C=1,v=0时,可访问自定义寄存器
    CSRS(CSR_MSTATEEN0 , MSTATEEN_C);
    CSRC(CSR_HSTATEEN0 , MSTATEEN_C);

    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();
    CSRR(0X5C0);

    excpt_info();

    TEST_ASSERT("hs mode accesss Custom register successful when  mstateen.c=1 hstateen.c=0",
        excpt.triggered == false
    ); 

#ifdef sstateen

    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();
    CSRR(0X5C1);

    excpt_info();

    TEST_ASSERT("hu mode accesss Custom register successful when mstateen.c=1 sstatten.c=1",
        excpt.triggered == false
    ); 

#endif

    goto_priv(PRIV_M);
    TEST_SETUP_EXCEPT();
    CSRR(0X5C3);

    excpt_info();

    TEST_ASSERT("m mode accesss Custom register successful when mstateen.c=1 hstateen.c=0",
        excpt.triggered == false
    ); 

    TEST_END();

}


bool stateen_C_test_7(){

    TEST_START();
    
    goto_priv(PRIV_M);
    //当mstateen.C=1 hstateen.C=1,v=1 时可访问自定义寄存器(vs,vu自定义寄存器是哪些)
    CSRS(CSR_MSTATEEN0 , MSTATEEN_C);
    CSRS(CSR_HSTATEEN0 , MSTATEEN_C);

    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    CSRR(0X5C1);

    excpt_info();

    TEST_ASSERT("vs mode accesss Custom register successful when mstateen.c=1 hstateen.c=1",
        excpt.triggered == false
    ); 


    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    CSRR(0X5C2);

    excpt_info();

    TEST_ASSERT("vu mode accesss Custom register successful when mstateen.c=1 hstateen.c=1",
        excpt.triggered == false
    ); 
    TEST_END();
}



//----



bool stateen_IMSIC_test_1(){

    TEST_START();
    
    goto_priv(PRIV_M);
    //当mstateen.IMSIC=0,hu mode 不可访问stopei、vstopei寄存器
    CSRC(CSR_MSTATEEN0 , MSTATEEN_IMSIC);

    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_STOPEI);

    excpt_info();

    TEST_ASSERT("HU mode accesss stopei register cause to illegal except when mstateen.imsic=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 

    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_VSTOPEI);

    excpt_info();

    TEST_ASSERT("HU mode accesss vstopei register cause to illegal except when mstateen.imsic=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 

    TEST_END();

}

bool stateen_IMSIC_test_2(){

    TEST_START();
    
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATEEN0 , MSTATEEN_IMSIC);
    CSRC(CSR_HSTATEEN0 , MSTATEEN_IMSIC);

    //当mstateen.IMSIC=1,hstateen.IMSIC=0,v=1时,不可访问stopei、vstopei寄存器
    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_STOPEI);

    excpt_info();

    TEST_ASSERT("VU mode accesss stopei register cause to VRTI when mstateen.imsic=1 hstateen.imsic=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 

    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_VSTOPEI);

    excpt_info();

    TEST_ASSERT("VU mode accesss vstopei register cause to VRTI when mstateen.imsic=1 hstateen.imsic=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 


    TEST_END();

}

bool stateen_IMSIC_test_3(){

    //当mstateen.IMSIC=0,m mode 可访问stopei、vstopei寄存器

    TEST_START();
    
    goto_priv(PRIV_M);
    CSRC(CSR_MSTATEEN0 , MSTATEEN_IMSIC);
    goto_priv(PRIV_M);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_STOPEI);

    excpt_info();

    TEST_ASSERT("m mode accesss stopei register success  when mstateen.imsic=0",
        excpt.triggered == false
    ); 

    goto_priv(PRIV_M);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_VSTOPEI);

    excpt_info();

    TEST_ASSERT("m mode accesss vstopei register success  when mstateen.imsic=0",
        excpt.triggered == false
    ); 
    TEST_END();

}

bool stateen_IMSIC_test_4(){

    //当mstateen.IMSIC=1,hstateen.IMSIC=0,v=1时不可访问stopei、vstopei寄存器

    TEST_START();
    
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATEEN0 , MSTATEEN_IMSIC);
    CSRC(CSR_HSTATEEN0 , MSTATEEN_IMSIC);

    printf("mstateen=%llx\n",CSRR(CSR_MSTATEEN0));
    printf("hstateen=%llx\n",CSRR(CSR_HSTATEEN0));

    goto_priv(PRIV_VS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_STOPEI);

    excpt_info();

    TEST_ASSERT("vs mode accesss stopei register cause to VRTI when mstateen.imsic=1 hstateen.imsic=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 

    goto_priv(PRIV_VS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_VSTOPEI);

    excpt_info();

    TEST_ASSERT("vs mode accesss vstopei register cause to VRTI when mstateen.imsic=1 hstateen.imsic=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 

    TEST_END();

}

bool stateen_IMSIC_test_5(){

    //当mstateen.IMSIC=0,hs mode 不可访问stopei、vstopei寄存器

    TEST_START();
    
    goto_priv(PRIV_M);
    CSRC(CSR_MSTATEEN0 , MSTATEEN_IMSIC);
    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_STOPEI);

    excpt_info();

    TEST_ASSERT("hs mode accesss stopei register cause to illegal except when mstateen.imsic=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 

    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_VSTOPEI);

    excpt_info();

    TEST_ASSERT("hs mode accesss vstopei register cause to illegal except when mstateen.imsic=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 

    TEST_END();

}


bool stateen_IMSIC_test_6(){

    TEST_START();
    
    goto_priv(PRIV_M);
    //当mstateen.IMSIC=1,v=0时,可访问stopei、vstopei寄存器
    CSRS(CSR_MSTATEEN0 , MSTATEEN_IMSIC);
    CSRC(CSR_HSTATEEN0 , MSTATEEN_IMSIC);

    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_STOPEI);

    excpt_info();

    TEST_ASSERT("hs mode accesss stopei register successful when  mstateen.imsic=1 hstateen.imsic=0",
        excpt.triggered == false
    ); 

    CSRR(CSR_VSTOPEI);

    excpt_info();

    TEST_ASSERT("hs mode accesss vstopei register successful when  mstateen.imsic=1 hstateen.imsic=0",
        excpt.triggered == false
    ); 


    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_STOPEI);

    excpt_info();

    TEST_ASSERT("hu mode accesss stopei register successful when mstateen.imsic=1  hstateen.imsic=0 sstateen.imsic=0 ",
        excpt.triggered == false
    ); 

    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_VSTOPEI);

    excpt_info();

    TEST_ASSERT("hu mode accesss vstopei register successful when mstateen.imsic=1 hstateen.imsic=0 sstateen.imsic=0",
        excpt.triggered == false
    ); 


    goto_priv(PRIV_M);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_STOPEI);

    excpt_info();

    TEST_ASSERT("m mode accesss stopei register successful when mstateen.imsic=1 hstateen.imsic=0",
        excpt.triggered == false
    ); 

    goto_priv(PRIV_M);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_VSTOPEI);

    excpt_info();

    TEST_ASSERT("m mode accesss vstopei register successful when mstateen.imsic=1 hstateen.imsic=0",
        excpt.triggered == false
    ); 

    TEST_END();

}


bool stateen_IMSIC_test_7(){

    TEST_START();
    
    goto_priv(PRIV_M);
    //当mstateen.IMSIC=1 hstateen.IMSIC=1,v=1 时可访问stopei、vstopei寄存器
    CSRS(CSR_MSTATEEN0 , MSTATEEN_IMSIC);
    CSRS(CSR_HSTATEEN0 , MSTATEEN_IMSIC);

    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_STOPEI);

    excpt_info();

    TEST_ASSERT("vs mode accesss stopei register successful when mstateen.imsic=1 hstateen.imsic=1",
        excpt.triggered == false
    ); 

    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_VSTOPEI);

    excpt_info();

    TEST_ASSERT("vs mode accesss vstopei register successful when mstateen.imsic=1 hstateen.imsic=1",
        excpt.triggered == false
    ); 


    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_STOPEI);

    excpt_info();

    TEST_ASSERT("vu mode accesss stopei register successful when mstateen.imsic=1 hstateen.imsic=1",
        excpt.triggered == false
    ); 

    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_VSTOPEI);

    excpt_info();

    TEST_ASSERT("vu mode accesss vstopei register successful when mstateen.imsic=1 hstateen.imsic=1",
        excpt.triggered == false
    ); 

    TEST_END();
}





//--

bool stateen_AIA_test_1(){

    TEST_START();
    
    goto_priv(PRIV_M);
    //当mstateen.AIA=0,hu mode 不可访问hvien、hvictl、hviprio1/2、stopi、vstopi寄存器
    CSRC(CSR_MSTATEEN0 , MSTATEEN_AIA);

    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HVIEN);

    excpt_info();

    TEST_ASSERT("HU mode accesss hvien register cause to illegal except when mstateen.aia=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 

    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HVICTL);

    excpt_info();

    TEST_ASSERT("HU mode accesss hvictl register cause to illegal except when mstateen.aia=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 

    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_STOPI);

    excpt_info();

    TEST_ASSERT("HU mode accesss stopi register cause to illegal except when mstateen.aia=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 

    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_VSTOPI);

    excpt_info();

    TEST_ASSERT("HU mode accesss vstopi register cause to illegal except when mstateen.aia=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 

    TEST_END();

}

bool stateen_AIA_test_2(){

    TEST_START();
    
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATEEN0 , MSTATEEN_AIA);
    CSRC(CSR_HSTATEEN0 , MSTATEEN_AIA);

    //当mstateen.AIA=1,hstateen.AIA=0,v=1时,不可访问hvien、hvictl、hviprio1/2、stopi、vstopi寄存器
    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HVIEN);

    excpt_info();

    TEST_ASSERT("VU mode accesss hvien register cause to VRTI when mstateen.aia=1 hstateen.aia=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 

    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HVICTL);

    excpt_info();

    TEST_ASSERT("VU mode accesss hvictl register cause to VRTI when mstateen.aia=1 hstateen.aia=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 

    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_STOPI);

    excpt_info();

    TEST_ASSERT("VU mode accesss stopi register cause to VRTI when mstateen.aia=1 hstateen.aia=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 

    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_VSTOPI);

    excpt_info();

    TEST_ASSERT("VU mode accesss vstopi register cause to VRTI when mstateen.aia=1 hstateen.aia=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 


    TEST_END();

}

bool stateen_AIA_test_3(){

    //当mstateen.AIA=0,m mode 可访问hvien、hvictl、hviprio1/2、stopi、vstopi寄存器

    TEST_START();
    
    goto_priv(PRIV_M);
    CSRC(CSR_MSTATEEN0 , MSTATEEN_AIA);
    goto_priv(PRIV_M);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HVIEN);

    excpt_info();

    TEST_ASSERT("m mode accesss hvien register success  when mstateen.aia=0",
        excpt.triggered == false
    ); 

    goto_priv(PRIV_M);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HVICTL);

    excpt_info();

    TEST_ASSERT("m mode accesss hvictl register success  when mstateen.aia=0",
        excpt.triggered == false
    ); 

    goto_priv(PRIV_M);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_STOPI);

    excpt_info();

    TEST_ASSERT("m mode accesss stopi register success  when mstateen.aia=0",
        excpt.triggered == false
    ); 

    goto_priv(PRIV_M);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_VSTOPI);

    excpt_info();

    TEST_ASSERT("m mode accesss vstopi register success  when mstateen.aia=0",
        excpt.triggered == false
    ); 

    TEST_END();

}


bool stateen_AIA_test_4(){

    //当mstateen.AIA=1,hstateen.AIA=0,v=1时不可访问hvien、hvictl、hviprio1/2、stopi、vstopi寄存器

    TEST_START();
    
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATEEN0 , MSTATEEN_AIA);
    CSRC(CSR_HSTATEEN0 , MSTATEEN_AIA);
    printf("mstateen=%llx\n",CSRR(CSR_MSTATEEN0));
    printf("hstateen=%llx\n",CSRR(CSR_HSTATEEN0));

    goto_priv(PRIV_VS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_HVIEN);

    excpt_info();

    TEST_ASSERT("vs mode accesss hvien register cause to VRTI when mstateen.aia=1 hstateen.aia=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 

    goto_priv(PRIV_VS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_HVICTL);

    excpt_info();

    TEST_ASSERT("vs mode accesss hvictl register cause to VRTI when mstateen.aia=1 hstateen.aia=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 

    goto_priv(PRIV_VS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_STOPI);

    excpt_info();

    TEST_ASSERT("vs mode accesss stopi register cause to VRTI when mstateen.aia=1 hstateen.aia=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 

    goto_priv(PRIV_VS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_VSTOPI);

    excpt_info();

    TEST_ASSERT("vs mode accesss vstopi register cause to VRTI when mstateen.aia=1 hstateen.aia=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 

    TEST_END();

}

bool stateen_AIA_test_5(){

    //当mstateen.AIA=0,hs mode 不可访问hvien、hvictl、hviprio1/2、stopi、vstopi寄存器

    TEST_START();
    
    goto_priv(PRIV_M);
    CSRC(CSR_MSTATEEN0 , MSTATEEN_AIA);

    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HVIEN);

    excpt_info();

    TEST_ASSERT("hs mode accesss hvien register cause to illegal except when mstateen.aia=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 

    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HVICTL);

    excpt_info();

    TEST_ASSERT("hs mode accesss hvictl register cause to illegal except when mstateen.aia=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 

    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_STOPI);

    excpt_info();

    TEST_ASSERT("hs mode accesss stopi register cause to illegal except when mstateen.aia=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 

    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_VSTOPI);

    excpt_info();

    TEST_ASSERT("hs mode accesss VSTOPI register cause to illegal except when mstateen.aia=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 

    TEST_END();

}


bool stateen_AIA_test_6(){

    TEST_START();
    
    goto_priv(PRIV_M);
    //当mstateen.AIA=1,v=0时,可访问hvien、hvictl、hviprio1/2、stopi、vstopi寄存器
    CSRS(CSR_MSTATEEN0 , MSTATEEN_AIA);
    CSRC(CSR_HSTATEEN0 , MSTATEEN_AIA);

    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HVIEN);

    excpt_info();

    TEST_ASSERT("hs mode accesss hvien register successful when  mstateen.aia=1 hstateen.aia=0",
        excpt.triggered == false
    ); 

    CSRR(CSR_HVICTL);

    excpt_info();

    TEST_ASSERT("hs mode accesss hvictl register successful when  mstateen.aia=1 hstateen.aia=0",
        excpt.triggered == false
    ); 


    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HVIEN);

    excpt_info();

    TEST_ASSERT("hu mode accesss hvien register successful when mstateen.aia=1 hstateen.aia=0",
        excpt.triggered == false
    ); 

    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HVICTL);

    excpt_info();

    TEST_ASSERT("hu mode accesss hvictl register successful when mstateen.aia=1 hstateen.aia=0",
        excpt.triggered == false
    ); 


    goto_priv(PRIV_M);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HVIEN);

    excpt_info();

    TEST_ASSERT("m mode accesss hvien register successful when mstateen.aia=1 hstateen.aia=0",
        excpt.triggered == false
    ); 

    goto_priv(PRIV_M);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HVICTL);

    excpt_info();

    TEST_ASSERT("m mode accesss hvictl register successful when mstateen.aia=1 hstateen.aia=0",
        excpt.triggered == false
    ); 


    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_STOPI);

    excpt_info();

    TEST_ASSERT("hs mode accesss stopi register successful when  mstateen.aia=1 hstateen.aia=0",
        excpt.triggered == false
    ); 

    CSRR(CSR_VSTOPI);

    excpt_info();

    TEST_ASSERT("hs mode accesss vstopi register successful when  mstateen.aia=1 hstateen.aia=0",
        excpt.triggered == false
    ); 


    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_STOPI);

    excpt_info();

    TEST_ASSERT("hu mode accesss stopi register successful when mstateen.aia=1 hstateen.aia=0",
        excpt.triggered == false
    ); 

    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_VSTOPI);

    excpt_info();

    TEST_ASSERT("hu mode accesss vstopi register successful when mstateen.aia=1 hstateen.aia=0",
        excpt.triggered == false
    ); 


    goto_priv(PRIV_M);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_STOPI);

    excpt_info();

    TEST_ASSERT("m mode accesss stopi register successful when mstateen.aia=1 hstateen.aia=0",
        excpt.triggered == false
    ); 

    goto_priv(PRIV_M);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_VSTOPI);

    excpt_info();

    TEST_ASSERT("m mode accesss vstopi register successful when mstateen.aia=1 hstateen.aia=0",
        excpt.triggered == false
    ); 

    TEST_END();

}


bool stateen_AIA_test_7(){

    TEST_START();
    
    goto_priv(PRIV_M);
    //当mstateen.AIA=1 hstateen.AIA=1,v=1 时可访问hvien、hvictl、hviprio1/2、stopi、vstopi寄存器
    CSRS(CSR_MSTATEEN0 , MSTATEEN_AIA);
    CSRS(CSR_HSTATEEN0 , MSTATEEN_AIA);

    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HVIEN);

    excpt_info();

    TEST_ASSERT("vs mode accesss hvien register successful when mstateen.aia=1 hstateen.aia=1",
        excpt.triggered == false
    ); 

    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HVICTL);

    excpt_info();

    TEST_ASSERT("vs mode accesss hvictl register successful when mstateen.aia=1 hstateen.aia=1",
        excpt.triggered == false
    ); 


    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HVIEN);

    excpt_info();

    TEST_ASSERT("vu mode accesss hvien register successful when mstateen.aia=1 hstateen.aia=1",
        excpt.triggered == false
    ); 

    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HVICTL);

    excpt_info();

    TEST_ASSERT("vu mode accesss hvictl register successful when mstateen.aia=1 hstateen.aia=1",
        excpt.triggered == false
    ); 

    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_STOPI);

    excpt_info();

    TEST_ASSERT("vs mode accesss stopi register successful when mstateen.aia=1 hstateen.aia=1",
        excpt.triggered == false
    ); 

    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_VSTOPI);

    excpt_info();

    TEST_ASSERT("vs mode accesss vstopi register successful when mstateen.aia=1 hstateen.aia=1",
        excpt.triggered == false
    ); 


    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_STOPI);

    excpt_info();

    TEST_ASSERT("vu mode accesss stopi register successful when mstateen.aia=1 hstateen.aia=1",
        excpt.triggered == false
    ); 

    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_VSTOPI);

    excpt_info();

    TEST_ASSERT("vu mode accesss vstopi register successful when mstateen.aia=1 hstateen.aia=1",
        excpt.triggered == false
    ); 

    TEST_END();
}



//mstateen.csrind 和 hstateen.csrind 在xiangshan中默认为1，且RO
bool stateen_CSRIND_test(){

    TEST_START();
    
    goto_priv(PRIV_M);
    //当mstateen.CSRIND=1 hstateen.CSRIND=1,v=1 时可访问iselect，ireg等寄存器

    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_SISELECT);

    excpt_info();

    TEST_ASSERT("vs mode accesss siselect register successful when mstateen.csrind=1 hstateen.csrind=1",
        excpt.triggered == false
    ); 

    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_SIREG);

    excpt_info();

    TEST_ASSERT("vs mode accesss sireg register successful when mstateen.csrind=1 hstateen.csrind=1",
        excpt.triggered == false
    ); 


    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_SISELECT);

    excpt_info();

    TEST_ASSERT("hs mode accesss siselect register successful when mstateen.csrind=1 hstateen.csrind=1",
        excpt.triggered == false
    ); 

    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_SIREG);

    excpt_info();

    TEST_ASSERT("hs mode accesss sireg register successful when mstateen.csrind=1 hstateen.csrind=1",
        excpt.triggered == false
    ); 

    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_SISELECT);

    excpt_info();

    TEST_ASSERT("hu mode accesss siselect register successful when mstateen.csrind=1 hstateen.csrind=1",
        excpt.triggered == false
    ); 

    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_SIREG);

    excpt_info();

    TEST_ASSERT("hu mode accesss sireg register successful when mstateen.csrind=1 hstateen.csrind=1",
        excpt.triggered == false
    ); 


    goto_priv(PRIV_M);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_SISELECT);

    excpt_info();

    TEST_ASSERT("m mode accesss siselect register successful when mstateen.csrind=1 hstateen.csrind=1",
        excpt.triggered == false
    ); 

    goto_priv(PRIV_M);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_SIREG);

    excpt_info();

    TEST_ASSERT("m mode accesss sireg register successful when mstateen.csrind=1 hstateen.csrind=1",
        excpt.triggered == false
    ); 

    TEST_END();
}

// ----------


bool stateen_ENVCFG_test_1(){

    TEST_START();
    
    goto_priv(PRIV_M);
    //当mstateen.ENVCFG=0,hu mode 不可访问Henvcfg和Senvcfg寄存器
    CSRC(CSR_MSTATEEN0 , MSTATEEN_ENVCFG);

    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HENVCFG);

    excpt_info();

    TEST_ASSERT("HU mode accesss envcfg register cause to illegal except when mstateen.envcfg=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 

    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_SENVCFG);

    excpt_info();

    TEST_ASSERT("HU mode accesss senvcfg register cause to illegal except when mstateen.envcfg=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 


    TEST_END();

}

bool stateen_ENVCFG_test_2(){

    TEST_START();
    
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATEEN0 , MSTATEEN_ENVCFG);
    CSRC(CSR_HSTATEEN0 , MSTATEEN_ENVCFG);

    //当mstateen.ENVCFG=1,hstateen.ENVCFG=0,v=1时,不可访问Henvcfg和Senvcfg寄存器
    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HENVCFG);

    excpt_info();

    TEST_ASSERT("VU mode accesss henvcfg register cause to VRTI when mstateen.envcfg=1 hstateen.envcfg=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 

    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_SENVCFG);

    excpt_info();

    TEST_ASSERT("VU mode accesss senvcfg register cause to VRTI when mstateen.envcfg=1 hstateen.envcfg=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 


    TEST_END();

}

bool stateen_ENVCFG_test_3(){

    //当mstateen.ENVCFG=0,m mode 可访问Henvcfg和Senvcfg寄存器

    TEST_START();
    
    goto_priv(PRIV_M);
    CSRC(CSR_MSTATEEN0 , MSTATEEN_ENVCFG);
    goto_priv(PRIV_M);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HENVCFG);

    excpt_info();

    TEST_ASSERT("m mode accesss henvcfg register success  when mstateen.envcfg=0",
        excpt.triggered == false
    ); 

    goto_priv(PRIV_M);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_SENVCFG);

    excpt_info();

    TEST_ASSERT("m mode accesss senvcfg register success  when mstateen.envcfg=0",
        excpt.triggered == false
    ); 



    TEST_END();

}


bool stateen_ENVCFG_test_4(){

    //当mstateen.ENVCFG=1,hstateen.ENVCFG=0,v=1时不可访问Henvcfg和Senvcfg寄存器

    TEST_START();
    
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATEEN0 , MSTATEEN_ENVCFG);
    CSRC(CSR_HSTATEEN0 , MSTATEEN_ENVCFG);
    printf("mstateen=%llx\n",CSRR(CSR_MSTATEEN0));
    printf("hstateen=%llx\n",CSRR(CSR_HSTATEEN0));

    goto_priv(PRIV_VS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_HENVCFG);

    excpt_info();

    TEST_ASSERT("vs mode accesss henvcfg register cause to VRTI when mstateen.envcfg=1 hstateen.envcfg=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 

    goto_priv(PRIV_VS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_SENVCFG);

    excpt_info();

    TEST_ASSERT("vs mode accesss senvcfg register cause to VRTI when mstateen.envcfg=1 hstateen.envcfg=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 



    TEST_END();

}

bool stateen_ENVCFG_test_5(){

    //当mstateen.ENVCFG=0,hs mode 不可访问Henvcfg和Senvcfg寄存器

    TEST_START();
    
    goto_priv(PRIV_M);
    CSRC(CSR_MSTATEEN0 , MSTATEEN_ENVCFG);

    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HENVCFG);

    excpt_info();

    TEST_ASSERT("hs mode accesss henvcfg register cause to illegal except when mstateen.envcfg=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 

    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_SENVCFG);

    excpt_info();

    TEST_ASSERT("hs mode accesss senvcfg register cause to illegal except when mstateen.envcfg=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 



    TEST_END();

}


bool stateen_ENVCFG_test_6(){

    TEST_START();
    
    goto_priv(PRIV_M);
    //当mstateen.ENVCFG=1,v=0时,可访问Henvcfg和Senvcfg寄存器
    CSRS(CSR_MSTATEEN0 , MSTATEEN_ENVCFG);
    CSRC(CSR_HSTATEEN0 , MSTATEEN_ENVCFG);

    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HENVCFG);

    excpt_info();

    TEST_ASSERT("hs mode accesss henvcfg register successful when  mstateen.envcfg=1 hstateen.envcfg=0",
        excpt.triggered == false
    ); 

    CSRR(CSR_SENVCFG);

    excpt_info();

    TEST_ASSERT("hs mode accesss senvcfg register successful when  mstateen.envcfg=1 hstateen.envcfg=0",
        excpt.triggered == false
    ); 


    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HENVCFG);

    excpt_info();

    TEST_ASSERT("hu mode accesss henvcfg register successful when mstateen.envcfg=1 hstateen.envcfg=0",
        excpt.triggered == false
    ); 

    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_SENVCFG);

    excpt_info();

    TEST_ASSERT("hu mode accesss senvcfg register successful when mstateen.envcfg=1 hstateen.envcfg=0",
        excpt.triggered == false
    ); 


    goto_priv(PRIV_M);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HENVCFG);

    excpt_info();

    TEST_ASSERT("m mode accesss henvcfg register successful when mstateen.envcfg=1 hstateen.envcfg=0",
        excpt.triggered == false
    ); 

    goto_priv(PRIV_M);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_SENVCFG);

    excpt_info();

    TEST_ASSERT("m mode accesss senvcfg register successful when mstateen.envcfg=1 hstateen.envcfg=0",
        excpt.triggered == false
    ); 


    TEST_END();

}


bool stateen_ENVCFG_test_7(){

    TEST_START();
    
    goto_priv(PRIV_M);
    //当mstateen.ENVCFG=1 hstateen.ENVCFG=1,v=1 时可访问Henvcfg和Senvcfg寄存器
    CSRS(CSR_MSTATEEN0 , MSTATEEN_ENVCFG);
    CSRS(CSR_HSTATEEN0 , MSTATEEN_ENVCFG);

    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HENVCFG);

    excpt_info();

    TEST_ASSERT("vs mode accesss henvcfg register successful when mstateen.envcfg=1 hstateen.envcfg=1",
        excpt.triggered == false
    ); 

    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_SENVCFG);

    excpt_info();

    TEST_ASSERT("vs mode accesss senvcfg register successful when mstateen.envcfg=1 hstateen.envcfg=1",
        excpt.triggered == false
    ); 


    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HENVCFG);

    excpt_info();

    TEST_ASSERT("vu mode accesss henvcfg register successful when mstateen.envcfg=1 hstateen.envcfg=1",
        excpt.triggered == false
    ); 

    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_SENVCFG);

    excpt_info();

    TEST_ASSERT("vu mode accesss senvcfg register successful when mstateen.envcfg=1 hstateen.envcfg=1",
        excpt.triggered == false
    ); 

    TEST_END();
}








// -------------




bool stateen_SE0_test_1(){

    TEST_START();
    
    goto_priv(PRIV_M);
    //当mstateen.SE0=0,hu mode 不可访问Hstateen0和Sstateen0寄存器
    CSRC(CSR_MSTATEEN0 , MSTATEEN_SE0);

    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HSTATEEN0);

    excpt_info();

    TEST_ASSERT("HU mode accesss hstateen0 register cause to illegal except when mstateen.SE0=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 

    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_SSTATEEN0);

    excpt_info();

    TEST_ASSERT("HU mode accesss sstateen0 register cause to illegal except when mstateen.SE0=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 


    TEST_END();

}

bool stateen_SE0_test_2(){

    TEST_START();
    
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATEEN0 , MSTATEEN_SE0);
    CSRS(CSR_HSTATEEN0 , MSTATEEN_SE0);

    //当mstateen.SE0=1,hstateen.SE0=0,v=1时,不可访问Hstateen0和Sstateen0寄存器
    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HSTATEEN0);

    excpt_info();

    TEST_ASSERT("VU mode access hstateen0 register cause to VRTI when mstateen.SE0=1 hstateen.SE0=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 

    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_SSTATEEN0);

    excpt_info();

    TEST_ASSERT("VU mode accesss sstateen0 register cause to VRTI when mstateen.SE0=1 hstateen.SE0=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 


    TEST_END();

}

bool stateen_SE0_test_3(){

    //当mstateen.SE0=0,m mode 可访问Hstateen0和Sstateen0寄存器

    TEST_START();
    
    goto_priv(PRIV_M);
    CSRC(CSR_MSTATEEN0 , MSTATEEN_SE0);
    goto_priv(PRIV_M);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HSTATEEN0);

    excpt_info();

    TEST_ASSERT("m mode access hstateen0 register success  when mstateen.SE0=0",
        excpt.triggered == false
    ); 

    goto_priv(PRIV_M);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_SSTATEEN0);

    excpt_info();

    TEST_ASSERT("m mode accesss sstateen0 register success  when mstateen.SE0=0",
        excpt.triggered == false
    ); 



    TEST_END();

}


bool stateen_SE0_test_4(){

    //当mstateen.SE0=1,hstateen.SE0=0,v=1时不可访问Hstateen0和Sstateen0寄存器

    TEST_START();
    
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATEEN0 , MSTATEEN_SE0);
    CSRC(CSR_HSTATEEN0 , MSTATEEN_SE0);
    printf("mstateen=%llx\n",CSRR(CSR_MSTATEEN0));
    printf("hstateen=%llx\n",CSRR(CSR_HSTATEEN0));

    goto_priv(PRIV_VS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_HSTATEEN0);

    excpt_info();

    TEST_ASSERT("vs mode access hstateen0 register cause to VRTI when mstateen.SE0=1 hstateen.SE0=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 

    goto_priv(PRIV_VS);

    TEST_SETUP_EXCEPT();
    CSRR(CSR_SSTATEEN0);

    excpt_info();

    TEST_ASSERT("vs mode accesss sstateen0 register cause to VRTI when mstateen.SE0=1 hstateen.SE0=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VRTI
    ); 



    TEST_END();

}

bool stateen_SE0_test_5(){

    //当mstateen.SE0=0,hs mode 不可访问Hstateen0和Sstateen0寄存器

    TEST_START();
    
    goto_priv(PRIV_M);
    CSRC(CSR_MSTATEEN0 , MSTATEEN_SE0);

    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HSTATEEN0);

    excpt_info();

    TEST_ASSERT("hs mode access hstateen0 register cause to illegal except when mstateen.SE0=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 

    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_SSTATEEN0);

    excpt_info();

    TEST_ASSERT("hs mode accesss sstateen0 register cause to illegal except when mstateen.SE0=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_ILI
    ); 



    TEST_END();

}


bool stateen_SE0_test_6(){

    TEST_START();
    
    goto_priv(PRIV_M);
    //当mstateen.SE0=1,v=0时,可访问Hstateen0和Sstateen0寄存器
    CSRS(CSR_MSTATEEN0 , MSTATEEN_SE0);
    CSRC(CSR_HSTATEEN0 , MSTATEEN_SE0);

    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HSTATEEN0);

    excpt_info();

    TEST_ASSERT("hs mode access hstateen0 register successful when  mstateen.SE0=1 hstateen.SE0=0",
        excpt.triggered == false
    ); 

    CSRR(CSR_SSTATEEN0);

    excpt_info();

    TEST_ASSERT("hs mode accesss sstateen0 register successful when  mstateen.SE0=1 hstateen.SE0=0",
        excpt.triggered == false
    ); 


    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HSTATEEN0);

    excpt_info();

    TEST_ASSERT("hu mode access hstateen0 register successful when mstateen.SE0=1 hstateen.SE0=0",
        excpt.triggered == false
    ); 

    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_SSTATEEN0);

    excpt_info();

    TEST_ASSERT("hu mode accesss sstateen0 register successful when mstateen.SE0=1 hstateen.SE0=0",
        excpt.triggered == false
    ); 


    goto_priv(PRIV_M);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HSTATEEN0);

    excpt_info();

    TEST_ASSERT("m mode access hstateen0 register successful when mstateen.SE0=1 hstateen.SE0=0",
        excpt.triggered == false
    ); 

    goto_priv(PRIV_M);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_SSTATEEN0);

    excpt_info();

    TEST_ASSERT("m mode accesss sstateen0 register successful when mstateen.SE0=1 hstateen.SE0=0",
        excpt.triggered == false
    ); 


    TEST_END();

}


bool stateen_SE0_test_7(){

    TEST_START();
    
    goto_priv(PRIV_M);
    //当mstateen.SE0=1 hstateen.SE0=1,v=1 时可访问Hstateen0和Sstateen0寄存器
    CSRS(CSR_MSTATEEN0 , MSTATEEN_SE0);
    CSRS(CSR_HSTATEEN0 , MSTATEEN_SE0);

    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HSTATEEN0);

    excpt_info();

    TEST_ASSERT("vs mode access hstateen0 register successful when mstateen.SE0=1 hstateen.SE0=1",
        excpt.triggered == false
    ); 

    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_SSTATEEN0);

    excpt_info();

    TEST_ASSERT("vs mode accesss sstateen0 register successful when mstateen.SE0=1 hstateen.SE0=1",
        excpt.triggered == false
    ); 


    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HSTATEEN0);

    excpt_info();

    TEST_ASSERT("vu mode access hstateen0 register successful when mstateen.SE0=1 hstateen.SE0=1",
        excpt.triggered == false
    ); 

    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_SSTATEEN0);

    excpt_info();

    TEST_ASSERT("vu mode accesss sstateen0 register successful when mstateen.SE0=1 hstateen.SE0=1",
        excpt.triggered == false
    ); 

    TEST_END();
}




bool stateen_SE0_test_8(){

    TEST_START();
    
    goto_priv(PRIV_M);
    //当mstateen.SE0=1,v=0时,可访问Hstateen0和Sstateen0寄存器
    CSRS(CSR_MSTATEEN0 , MSTATEEN_SE0);
    CSRS(CSR_HSTATEEN0 , MSTATEEN_SE0);
    CSRS(CSR_SSTATEEN0 , MSTATEEN_SE0);

    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HSTATEEN0);

    excpt_info();

    TEST_ASSERT("hs mode access hstateen0 register successful when  mstateen.SE0=1 hstateen.SE0=1",
        excpt.triggered == false
    ); 

    CSRR(CSR_SSTATEEN0);

    excpt_info();

    TEST_ASSERT("hs mode accesss sstateen0 register successful when  mstateen.SE0=1 hstateen.SE0=1",
        excpt.triggered == false
    ); 


    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HSTATEEN0);

    excpt_info();

    TEST_ASSERT("hu mode access hstateen0 register successful when mstateen.SE0=1 hstateen.SE0=1",
        excpt.triggered == false
    ); 

    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_SSTATEEN0);

    excpt_info();

    TEST_ASSERT("hu mode accesss sstateen0 register successful when mstateen.SE0=1 hstateen.SE0=1",
        excpt.triggered == false
    ); 


    goto_priv(PRIV_M);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HSTATEEN0);

    excpt_info();

    TEST_ASSERT("m mode access hstateen0 register successful when mstateen.SE0=1 hstateen.SE0=1",
        excpt.triggered == false
    ); 

    goto_priv(PRIV_M);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_SSTATEEN0);

    excpt_info();

    TEST_ASSERT("m mode accesss sstateen0 register successful when mstateen.SE0=1 hstateen.SE0=1",
        excpt.triggered == false
    ); 


    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HSTATEEN0);

    excpt_info();

    TEST_ASSERT("vs mode access hstateen0 register successful when mstateen.SE0=1 hstateen.SE0=1",
        excpt.triggered == false
    ); 

    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_SSTATEEN0);

    excpt_info();

    TEST_ASSERT("vs mode accesss sstateen0 register successful when mstateen.SE0=1 hstateen.SE0=1",
        excpt.triggered == false
    ); 


    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_HSTATEEN0);

    excpt_info();

    TEST_ASSERT("hs mode access hstateen0 register successful when mstateen.SE0=1 hstateen.SE0=1",
        excpt.triggered == false
    ); 

    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();
    CSRR(CSR_SSTATEEN0);

    excpt_info();

    TEST_ASSERT("hs mode accesss sstateen0 register successful when mstateen.SE0=1 hstateen.SE0=1",
        excpt.triggered == false
    ); 


    TEST_END();

}