#include <rvh_test.h>
#include <page_tables.h>
#include <csrs.h> 

bool addr_unaligned() {

    TEST_START();

    TEST_SETUP_EXCEPT();
    
    
    hspt_init();        
    goto_priv(PRIV_HS);     
    uintptr_t vaddr_f = hs_page_base(VSRWX_GRWX) + 1;      
    uint64_t value = 0xdeadbeef;

    //load byte地址不会发生未对齐
    TEST_SETUP_EXCEPT();        
    value = lb(vaddr_f);
    TEST_ASSERT("load byte address is not aligned successful",         
        excpt.triggered == false
    );

    //load half地址未对齐引发lam
    TEST_SETUP_EXCEPT();        
    value = lh(vaddr_f);
    TEST_ASSERT(" load half address is not aligned result in a lam",         
        excpt.triggered == true&&
        excpt.cause == CAUSE_LAM
    );
    
    //load word地址未对齐引发lam
    TEST_SETUP_EXCEPT();        
    value = lw(vaddr_f);
    TEST_ASSERT("load word address is not aligned result in a lam",         
        excpt.triggered == true&&
        excpt.cause == CAUSE_LAM
    );

    //load double word地址未对齐引发lam
    TEST_SETUP_EXCEPT();        
    value = ld(vaddr_f);
    TEST_ASSERT("load double word address is not aligned result in a lam",         
        excpt.triggered == true&&
        excpt.cause == CAUSE_LAM
    );

    //store byte地址不会发生未对齐
    TEST_SETUP_EXCEPT();        
    sb(vaddr_f,value);
    TEST_ASSERT("store byte address is not aligned successful",         
        excpt.triggered == false
    );

    //store half地址未对齐引发sam
    TEST_SETUP_EXCEPT();        
    sh(vaddr_f,value);
    TEST_ASSERT("store half address is not aligned result in a sam",         
        excpt.triggered == true&&
        excpt.cause == CAUSE_SAM
    );
    
    //store word地址未对齐引发sam
    TEST_SETUP_EXCEPT();        
    sw(vaddr_f,value);
    TEST_ASSERT("store word address is not aligned result in a sam",         
        excpt.triggered == true&&
        excpt.cause == CAUSE_SAM
    );

    //store double word地址未对齐引发sam
    TEST_SETUP_EXCEPT();        
    sd(vaddr_f,value);
    TEST_ASSERT("store double word address is not aligned result in a sam",         
        excpt.triggered == true&&
        excpt.cause == CAUSE_SAM
    );

    TEST_END();

}