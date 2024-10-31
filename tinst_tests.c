#include <rvh_test.h>
#include <page_tables.h>

/**
 * TODO:
 *  [] test traps to hs
 *  [] test fp load/store instructions
 *  [] test hypervisor load/store instructions
 *  [] test address offset field for implementations that support
 * unaligned acceses or have highest priority for page fault than for
 * unaligned access exceptions.
 *  [] test pseudo-instructions
 *  [] test tinst is written zero on:
 *      - interrupts
 *      - other cases (?)
 */

#define TINST_LOAD(ins) ((ins) & (INS_OPCODE | INS_RD | INS_FUNCT3))
#define TINST_STORE(ins) ((ins) & (INS_OPCODE | INS_FUNCT3 | INS_RS2))
#define TINST_AMO(ins) ((ins) & (~TINST_ADDROFF))
#define TINST_CHECK(CHECK) (excpt.tinst == 0 || excpt.tinst == CHECK(read_instruction(excpt.epc)))
#define TINST_CHECK_COMPRESSED(CHECK) (excpt.tinst == 0 ||\
    excpt.tinst == CHECK(expand_compressed_instruction(read_instruction(excpt.epc)) & ~0b10ULL))



bool tinst_tests_pf(){
    
    TEST_START();

    hspt_init();        
    goto_priv(PRIV_HS);     
    uintptr_t vaddr_f = hs_page_base(VSI_GI);      
    uint64_t value = 0xdeadbeef;

    TEST_SETUP_EXCEPT();        
    
    // uint8_t src8[] = {1, 2, 3, 4};
    // uint8_t dest8[4];
    // vle8_v(dest8, (uintptr_t)src8);


    value = lb(vaddr_f);
    TEST_ASSERT("correct tinst when executing a lb which results in a lpf",         
        excpt.triggered == true && 
        excpt.cause == CAUSE_LPF &&
        TINST_CHECK(TINST_LOAD)
    );

    TEST_SETUP_EXCEPT();
    value = lbu(vaddr_f);
    TEST_ASSERT("correct tinst when executing a lbu which results in a lpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_LPF &&
        TINST_CHECK(TINST_LOAD)
    );

    TEST_SETUP_EXCEPT();
    value = lh(vaddr_f);
    TEST_ASSERT("correct tinst when executing a lh which results in a lpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_LPF &&
        TINST_CHECK(TINST_LOAD)
    );

    TEST_SETUP_EXCEPT();
    value = lhu(vaddr_f);
    TEST_ASSERT("correct tinst when executing a lhu which results in a lpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_LPF &&
        TINST_CHECK(TINST_LOAD)
    );

    TEST_SETUP_EXCEPT();
    lw(vaddr_f);
    TEST_ASSERT("correct tinst when executing a lw which results in a lpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_LPF &&
        TINST_CHECK(TINST_LOAD)
    );

    TEST_SETUP_EXCEPT();
    value = lwu(vaddr_f);
    TEST_ASSERT("correct tinst when executing a lwu which results in a lpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_LPF &&
        TINST_CHECK(TINST_LOAD)
    );

    TEST_SETUP_EXCEPT();
    value = ld(vaddr_f);
    TEST_ASSERT("correct tinst when executing a ld which results in a lpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_LPF &&
        TINST_CHECK(TINST_LOAD)
    );

    value = 0xdeadbeef;

    TEST_SETUP_EXCEPT();
    sb(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a sb which results in a spf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SPF &&
        TINST_CHECK(TINST_STORE)
    );

    TEST_SETUP_EXCEPT();
    sh(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a sh which results in a spf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SPF &&
        TINST_CHECK(TINST_STORE)
    );

    TEST_SETUP_EXCEPT();
    sw(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a sw which results in a spf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SPF &&
        TINST_CHECK(TINST_STORE)
    );

    TEST_SETUP_EXCEPT();
    sd(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a sd which results in a spf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SPF &&
        TINST_CHECK(TINST_STORE)
    );

    TEST_SETUP_EXCEPT();
    value = c_lw(vaddr_f);
    TEST_ASSERT("correct tinst when executing a c.lw which results in a lpf",
        excpt.triggered == true &&
        excpt.cause == CAUSE_LPF &&
        TINST_CHECK_COMPRESSED(TINST_LOAD)
    );

    TEST_SETUP_EXCEPT();
    value = c_ld(vaddr_f);
    TEST_ASSERT("correct tinst when executing a c.ld which results in a lpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_LPF &&
        TINST_CHECK_COMPRESSED(TINST_LOAD)
    );

    TEST_SETUP_EXCEPT();
    c_sw(vaddr_f, value);       
    TEST_ASSERT("correct tinst when executing a c.lw which results in a lpf",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SPF &&
        TINST_CHECK_COMPRESSED(TINST_STORE)
    );

    TEST_SETUP_EXCEPT();
    c_sd(vaddr_f, value);
    TEST_ASSERT("correct tinst when executing a c.sd which results in a lpf",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SPF &&
        TINST_CHECK_COMPRESSED(TINST_STORE)
    );

    TEST_SETUP_EXCEPT();
    value = lr_w(vaddr_f);
    TEST_ASSERT("correct tinst when executing a lr.w which results in a lpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_LPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = sc_w(vaddr_f, value);
    TEST_ASSERT("correct tinst when executing a sc.w which results in a spf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoswap_w(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoswap.w which results in a spf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoadd_w(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoadd.w which results in a spf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoxor_w(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoxor.w which results in a spf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SPF &&
       TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoand_w(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoand.w which results in a spf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoor_w(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoor.w which results in a spf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amomin_w(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amomin.w which results in a spf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amomax_w(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amomax.w which results in a spf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amominu_w(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amominu.w which results in a spf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amomaxu_w(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amomaxu.w which results in a spf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoswap_d(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoswap.d which results in a spf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoadd_d(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoadd.d which results in a spf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoxor_d(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoxor.d which results in a spf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoand_d(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoand.d which results in a spf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoor_d(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoor.d which results in a spf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amomin_d(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amomin.d which results in a spf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amomax_d(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amomax.d which results in a spf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amominu_d(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amominu.d which results in a spf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amomaxu_d(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amomaxu.d which results in a spf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_END();
}

bool tinst_tests_gpf(){
    
    TEST_START();

    goto_priv(PRIV_HS);
    hspt_init();
    hpt_init();
    vspt_init();

    goto_priv(PRIV_VS);   
    uintptr_t vaddr_f = hs_page_base(VSRWX_GI);      
    printf("%llx\n",vaddr_f);
    uint64_t value = 0xdeadbeef;

    TEST_SETUP_EXCEPT();        
    
    // uint8_t src8[] = {1, 2, 3, 4};
    // uint8_t dest8[4];
    // vle8_v(dest8, (uintptr_t)src8);


    value = lb(vaddr_f);    
    printf("%llx\n",excpt.tinst);
    printf("%llx\n",excpt.triggered);
    printf("%llx\n",excpt.cause);
    printf("%llx\n",excpt.tval2);
    printf("%llx\n",excpt.tval);

    TEST_ASSERT("correct tinst when executing a lb which results in a lgpf",         
        excpt.triggered == true && 
        excpt.cause == CAUSE_LGPF 
    );

    TEST_SETUP_EXCEPT();
    value = lbu(vaddr_f);
    TEST_ASSERT("correct tinst when executing a lbu which results in a lgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_LGPF &&
        TINST_CHECK(TINST_LOAD)
    );

    TEST_SETUP_EXCEPT();
    value = lh(vaddr_f);
    TEST_ASSERT("correct tinst when executing a lh which results in a lgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_LGPF &&
        TINST_CHECK(TINST_LOAD)
    );

    TEST_SETUP_EXCEPT();
    value = lhu(vaddr_f);
    TEST_ASSERT("correct tinst when executing a lhu which results in a lgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_LGPF &&
        TINST_CHECK(TINST_LOAD)
    );

    TEST_SETUP_EXCEPT();
    lw(vaddr_f);
    TEST_ASSERT("correct tinst when executing a lw which results in a lgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_LGPF &&
        TINST_CHECK(TINST_LOAD)
    );

    TEST_SETUP_EXCEPT();
    value = lwu(vaddr_f);
    TEST_ASSERT("correct tinst when executing a lwu which results in a lgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_LGPF &&
        TINST_CHECK(TINST_LOAD)
    );

    TEST_SETUP_EXCEPT();
    value = ld(vaddr_f);
    TEST_ASSERT("correct tinst when executing a ld which results in a lgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_LGPF &&
        TINST_CHECK(TINST_LOAD)
    );

    value = 0xdeadbeef;

    TEST_SETUP_EXCEPT();
    sb(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a sb which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_STORE)
    );

    TEST_SETUP_EXCEPT();
    sh(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a sh which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_STORE)
    );

    TEST_SETUP_EXCEPT();
    sw(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a sw which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_STORE)
    );

    TEST_SETUP_EXCEPT();
    sd(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a sd which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_STORE)
    );

    TEST_SETUP_EXCEPT();
    value = c_lw(vaddr_f);
    TEST_ASSERT("correct tinst when executing a c.lw which results in a lgpf",
        excpt.triggered == true &&
        excpt.cause == CAUSE_LGPF &&
        TINST_CHECK_COMPRESSED(TINST_LOAD)
    );

    TEST_SETUP_EXCEPT();
    value = c_ld(vaddr_f);
    TEST_ASSERT("correct tinst when executing a c.ld which results in a lgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_LGPF &&
        TINST_CHECK_COMPRESSED(TINST_LOAD)
    );

    TEST_SETUP_EXCEPT();
    c_sw(vaddr_f, value);       
    TEST_ASSERT("correct tinst when executing a c.lw which results in a lgpf",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK_COMPRESSED(TINST_STORE)
    );

    TEST_SETUP_EXCEPT();
    c_sd(vaddr_f, value);
    TEST_ASSERT("correct tinst when executing a c.sd which results in a lgpf",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK_COMPRESSED(TINST_STORE)
    );

    TEST_SETUP_EXCEPT();
    value = lr_w(vaddr_f);
    TEST_ASSERT("correct tinst when executing a lr.w which results in a lgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_LGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = sc_w(vaddr_f, value);
    TEST_ASSERT("correct tinst when executing a sc.w which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoswap_w(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoswap.w which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoadd_w(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoadd.w which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoxor_w(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoxor.w which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
       TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoand_w(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoand.w which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoor_w(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoor.w which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amomin_w(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amomin.w which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amomax_w(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amomax.w which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amominu_w(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amominu.w which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amomaxu_w(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amomaxu.w which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoswap_d(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoswap.d which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoadd_d(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoadd.d which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoxor_d(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoxor.d which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoand_d(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoand.d which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amoor_d(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amoor.d which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amomin_d(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amomin.d which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amomax_d(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amomax.d which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amominu_d(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amominu.d which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_SETUP_EXCEPT();
    value = amomaxu_d(vaddr_f,value);
    TEST_ASSERT("correct tinst when executing a amomaxu.d which results in a sgpf",
        excpt.triggered == true && 
        excpt.cause == CAUSE_SGPF &&
        TINST_CHECK(TINST_AMO)
    );

    TEST_END();
}
