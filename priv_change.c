#include <rvh_test.h>
#include <csrs.h> 
#include <page_tables.h> 
bool priv_change_1(){
    
    TEST_START();


    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    goto_priv(PRIV_VS);
    vspt_init(); 

    //V=1，VS模式下发生异常，关闭代理，切换到M态处理异常，异常结束恢复到VS态
    goto_priv(PRIV_M);
    CSRW(medeleg,0);
    CSRW(mideleg,0);     
    CSRW(CSR_HIDELEG,0);
    CSRW(CSR_HEDELEG,0);   

    goto_priv(PRIV_VS);     
    uintptr_t vaddr_f = vs_page_base(VSI_GI);      
    uint64_t value = 0xdeadbeef;

    TEST_SETUP_EXCEPT();        
    value = lb(vaddr_f);
    TEST_ASSERT("vs trigger except that priv change to m mod and sret to vs mode when medeleg/mideleg==0 and hedeleg/hideleg==0",         
        excpt.triggered == true && 
        excpt.cause == CAUSE_LPF &&
        excpt.priv == PRIV_M &&
        curr_priv == PRIV_VS
    );

    TEST_END();
}

bool priv_change_2(){
    
    TEST_START();


    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    goto_priv(PRIV_VS);
    vspt_init(); 

    uintptr_t vaddr_f = vs_page_base(VSI_GI);      
    uint64_t value = 0xdeadbeef;
    //V=1，VS模式下发生异常，关闭所有代理，切换到M态处理异常
    goto_priv(PRIV_M);
    CSRC(medeleg, 1 << CAUSE_LPF);         
    goto_priv(PRIV_VS);     

    vaddr_f = vs_page_base(VSI_GI);      
    value = 0xdeadbeef;
    
    TEST_SETUP_EXCEPT();        
    value = lb(vaddr_f);
    TEST_ASSERT("vs trigger except that priv change to m mode when medeleg==0",         
        excpt.triggered == true && 
        excpt.cause == CAUSE_LPF &&
        excpt.priv == PRIV_M &&
        curr_priv == PRIV_VS
    );    

    TEST_END();
}


bool priv_change_3(){
    
    TEST_START();


    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    goto_priv(PRIV_VS);
    vspt_init(); 

    uintptr_t vaddr_f = vs_page_base(VSI_GI);      
    uint64_t value = 0xdeadbeef;

    //V=0，HS模式下发生异常，关闭代理，切换到M态处理异常，处理结束执行mret返回HS态
    TEST_SETUP_EXCEPT();  
    goto_priv(PRIV_M);
    CSRW(medeleg,0);
    CSRW(mideleg,0);
    goto_priv(PRIV_HS);     
    vaddr_f = hs_page_base(VSI_GI);      
    value = 0xdeadbeef;

    TEST_SETUP_EXCEPT();        
    value = lb(vaddr_f);
    //mret();
    TEST_ASSERT("hs trigger except that priv change to m mod and mret to hs mode when medeleg/mideleg==0 and hedeleg/hideleg==0",         
        excpt.triggered == true &&
        curr_priv == PRIV_HS &&
        excpt.priv == PRIV_M
    ); 

    TEST_END();
}

bool priv_change_4(){
    
    TEST_START();


    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    goto_priv(PRIV_VS);
    vspt_init(); 

    uintptr_t vaddr_f = vs_page_base(VSI_GI);      
    uint64_t value = 0xdeadbeef;

    //V=0，HS模式下发生异常，关闭所有代理，切换到M态处理异常
    goto_priv(PRIV_M);
    CSRC(medeleg, 1 << CAUSE_LPF);         
    goto_priv(PRIV_HS);     
    vaddr_f = hs_page_base(VSI_GI);      
    value = 0xdeadbeef;

    TEST_SETUP_EXCEPT();        
    value = lb(vaddr_f);
    TEST_ASSERT("hs trigger except that priv change to m mode when medeleg==0",         
        excpt.triggered == true && 
        excpt.cause == CAUSE_LPF &&
        excpt.priv==PRIV_M
    );  
    
    TEST_END();
}

bool priv_change_5(){
    
    TEST_START();


    //V=0，M模式下发生异常，M态处理异常
    goto_priv(PRIV_M);
    reset_state();
    CSRW(medeleg, 0); 

    TEST_SETUP_EXCEPT();       
    ecall_1(); 
    TEST_ASSERT("m trigger except that priv no change when medeleg==0",         
        excpt.triggered == true && 
        excpt.priv==PRIV_M
    ); 
    
    printf("niuh");

    TEST_END();
}

bool priv_change_6(){
    
    TEST_START();


    //V=0，U模式下发生异常，关闭所有代理，切换到M态处理异常
    goto_priv(PRIV_M);
    CSRW(medeleg,0);         

    goto_priv(PRIV_HU); 
    
    TEST_SETUP_EXCEPT();      
    CSRR(CSR_MSTATUS);
    TEST_ASSERT("hu trigger except that priv change to m mode when medeleg=0",         
        excpt.triggered == true && 
        excpt.priv==PRIV_M
    );

    TEST_END();
}

bool priv_change_7(){
    
    TEST_START();


    //V=1，VU模式下发生异常，关闭所有代理，切换到M态处理异常
    goto_priv(PRIV_M);
    CSRW(medeleg,0);      

    goto_priv(PRIV_VU);     
    TEST_SETUP_EXCEPT();        
    CSRR(CSR_MSTATUS);

    TEST_ASSERT("vu trigger except that priv change to m mode when medeleg=0",         
        excpt.triggered == true && 
        excpt.priv==PRIV_M
    );   


    TEST_END();
}

bool priv_change_8(){
    
    TEST_START();


    //V=1，VU模式下发生异常，关闭代理，切换到M态处理异常，异常结束执行mret恢复到VU态
    goto_priv(PRIV_M);
    reset_state();
    CSRW(medeleg,0);
    CSRW(mideleg,0);     
    CSRW(CSR_HIDELEG,0);
    CSRW(CSR_HEDELEG,0);   

    goto_priv(PRIV_VU);     
    
    TEST_SETUP_EXCEPT();        
    CSRW(medeleg, 0); 
    TEST_ASSERT("vu trigger except that priv change to m mod and sret to vu mod when medeleg/mideleg==0 and hedeleg/hideleg==0",         
        excpt.triggered == true &&
        excpt.priv == PRIV_M &&
        curr_priv == PRIV_VU
    );



    TEST_END();
}



bool priv_change_9(){
    TEST_START();

    //V=1，VU模式下发生异常，打开代理medeleg/mideleg，打开代理hedeleg/hideleg，切换到VS态处理异常，异常结束执行sret恢复到VU态
    goto_priv(PRIV_M);
    CSRW(medeleg,(uint64_t)-1);
    CSRW(mideleg,(uint64_t)-1);     
    CSRW(CSR_HIDELEG,(uint64_t)-1);
    CSRW(CSR_HEDELEG,(uint64_t)-1);   

    goto_priv(PRIV_VU);     
    
    TEST_SETUP_EXCEPT();        
    CSRR(CSR_MSTATUS);
    
    TEST_ASSERT("vu trigger except that priv change to vs mod and sret to vu mod when medeleg/mideleg==1 and hedeleg/hideleg==1",         
        excpt.triggered == true &&
        excpt.priv == PRIV_VS &&
        curr_priv == PRIV_VU
    );


    TEST_END();
}


bool priv_change_10(){
    TEST_START();


    //V=0，U模式下发生异常，打开代理medeleg/mideleg，切换到HS态处理异常，异常处理结束执行sret恢复到U态
    goto_priv(PRIV_M);
    reset_state();
    CSRW(medeleg,(uint64_t)-1);
    CSRW(mideleg,(uint64_t)-1);     
    CSRW(CSR_HIDELEG,0);
    CSRW(CSR_HEDELEG,0);   

    goto_priv(PRIV_HU);     
    
    TEST_SETUP_EXCEPT();        
    CSRR(CSR_MSTATUS);

    TEST_ASSERT("hu trigger except that priv change to m mod and mret to hu mode when medeleg/mideleg==1 and hedeleg/hideleg==0",         
        excpt.triggered == true &&
        excpt.priv == PRIV_HS &&
        curr_priv == PRIV_HU
    );

    TEST_END();
}


bool priv_change_11(){
    TEST_START();

    //V=1，VS模式下发生异常，打开代理medeleg/mideleg，打开代理hedeleg/hideleg，切换到VS态处理异常
    goto_priv(PRIV_M);
    reset_state();
    CSRW(medeleg,(uint64_t)-1);
    CSRW(mideleg,(uint64_t)-1);     
    CSRW(CSR_HIDELEG,(uint64_t)-1);
    CSRW(CSR_HEDELEG,(uint64_t)-1);   

    goto_priv(PRIV_VS);    
    

    TEST_SETUP_EXCEPT();        
    CSRW(medeleg, 0); 
    TEST_ASSERT("vs trigger except that priv no change  when medeleg/mideleg==1 and hedeleg/hideleg==1",         
        excpt.triggered == true && 
        excpt.priv==PRIV_VS
    ); 


    TEST_END();
}



bool priv_change_12(){
    TEST_START();



    //V=0，HS模式下发生异常，打开代理medeleg/mideleg，关闭代理hedeleg/hideleg，切换到HS态处理异常

    goto_priv(PRIV_M);     
    CSRW(medeleg,(uint64_t)-1);
    CSRW(mideleg,(uint64_t)-1);     
    CSRW(CSR_HIDELEG,0);
    CSRW(CSR_HEDELEG,0);   

    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();        
    CSRR(CSR_MSTATUS);
    TEST_ASSERT("hs trigger except that priv no change when medeleg/mideleg==1 and hedeleg/hideleg==0",         
        excpt.triggered == true &&
        excpt.priv == PRIV_HS
    ); 


    TEST_END();

}

bool priv_change_13(){
    TEST_START();

    //V=1，VU模式下发生异常，打开代理medeleg/mideleg，关闭代理hedeleg/hideleg，切换到HS态处理异常，异常结束执行sret恢复到VU态
    goto_priv(PRIV_M);
    reset_state();
    CSRW(medeleg,(uint64_t)-1);
    CSRW(mideleg,(uint64_t)-1);     
    CSRW(CSR_HIDELEG,0);
    CSRW(CSR_HEDELEG,0);   

    TEST_SETUP_EXCEPT();  
    goto_priv(PRIV_VU);      
    CSRR(CSR_MSTATUS);
    
    TEST_ASSERT("vu trigger except that priv change to hs mod and sret to vu mod when medeleg/mideleg==1 and hedeleg/hideleg==0",         
        excpt.triggered == true &&
        excpt.priv == PRIV_HS &&
        curr_priv == PRIV_VU
    );

    TEST_END();
}

bool priv_change_14(){
    TEST_START();

    //V=1，VU模式下发生异常，打开代理medeleg/mideleg，关闭理hedeleg/hideleg，切换到HS态处理异常，异常处理结束恢复执行sret到VU态
    goto_priv(PRIV_M);
    reset_state();
    CSRW(medeleg,(uint64_t)-1);
    CSRW(mideleg,(uint64_t)-1);     
    CSRW(CSR_HIDELEG,0);
    CSRW(CSR_HEDELEG,0);   
    
    goto_priv(PRIV_VU);     

    TEST_SETUP_EXCEPT();        
    CSRW(medeleg, 0); 

    TEST_ASSERT("vu trigger except that priv change to HS mod and sret to vu mode when medeleg/mideleg==1 and hedeleg/hideleg==0",         
        excpt.triggered == true &&
        excpt.priv == PRIV_HS &&
        curr_priv == PRIV_VU
    );

    TEST_END();

}

bool priv_change_15(){
    TEST_START();

    //V=1，VS模式下发生异常，打开代理medeleg/mideleg，关闭代理hedeleg/hideleg，切换到HS态处理异常，异常结束执行sret恢复到VU态
    goto_priv(PRIV_M);
    reset_state();
    CSRW(medeleg,(uint64_t)-1);
    CSRW(mideleg,(uint64_t)-1);     
    CSRW(CSR_HIDELEG,0);
    CSRW(CSR_HEDELEG,0);   

    TEST_SETUP_EXCEPT();    
    goto_priv(PRIV_VS);     
    CSRR(CSR_MSTATUS);
    
    TEST_ASSERT("vs trigger except that priv change to hs mode and sret to vs mod when medeleg/mideleg==1 and hedeleg/hideleg==0",         
        excpt.triggered == true && 
        excpt.priv == PRIV_HS &&
        curr_priv == PRIV_VS
    );

    TEST_END();

}


