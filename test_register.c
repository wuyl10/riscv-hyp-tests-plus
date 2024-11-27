#include <rvh_test.h>
#include <page_tables.h>

#ifdef future
// TEST_REGISTER(mstatus_csr_tests_1); 
// TEST_REGISTER(mstatus_csr_tests_2); 
// TEST_REGISTER(mstatus_csr_tests_3); 
// TEST_REGISTER(mstatus_csr_tests_4); 
// TEST_REGISTER(mstatus_csr_tests_5); 
// TEST_REGISTER(hedeleg_csr_tests_1); 
// TEST_REGISTER(mhpmevent_csr_tests_1); 
// TEST_REGISTER(tselect_csr_tests_1); 
// TEST_REGISTER(mhpmevent_csr_tests_1);
// TEST_REGISTER(software_interrupt_msi);
// TEST_REGISTER(external_interrupt_VSEI);
// TEST_REGISTER(external_interrupt_SEI);
// TEST_REGISTER(external_interrupt_MEI);
// TEST_REGISTER(virtual_instruction_13);
// TEST_REGISTER(virtual_instruction_14);
// TEST_REGISTER(virtual_instruction_15);
// TEST_REGISTER(virtual_instruction_16);
#endif


// TEST_REGISTER(vec_vle_vse_func); 
// TEST_REGISTER(vec_test_1); 
// TEST_REGISTER(vec_test_2); 
// TEST_REGISTER(vec_test_3); 
// TEST_REGISTER(vec_test_4); 
// TEST_REGISTER(vec_test_5); 
// TEST_REGISTER(vec_test_6); 
// TEST_REGISTER(vec_test_7); 
// TEST_REGISTER(vec_test_8); 
// TEST_REGISTER(vec_test_9);       
// TEST_REGISTER(vec_test_10); 
// TEST_REGISTER(vec_test_11); 
// TEST_REGISTER(vec_test_12); 
// TEST_REGISTER(vec_test_13); 
// TEST_REGISTER(vec_test_14); 
// TEST_REGISTER(vec_test_15); 
// TEST_REGISTER(vec_test_16); 
// TEST_REGISTER(vec_test_17); 
// TEST_REGISTER(vec_test_18); 
// TEST_REGISTER(vec_test_19); 
// TEST_REGISTER(vec_test_20); 
// TEST_REGISTER(vec_test_21); 
// TEST_REGISTER(vec_test_22); 
// TEST_REGISTER(vec_test_23); 
// TEST_REGISTER(vec_test_24);


// TEST_REGISTER(priv_instruction); 

// // 重新考虑不同模式下的自定义寄存器
// TEST_REGISTER(stateen_C_test_1); 
// TEST_REGISTER(stateen_C_test_2); 
// TEST_REGISTER(stateen_C_test_3); 
// TEST_REGISTER(stateen_C_test_4);   
// TEST_REGISTER(stateen_C_test_5); 
// TEST_REGISTER(stateen_C_test_6); 
// TEST_REGISTER(stateen_C_test_7); 

// TEST_REGISTER(stateen_IMSIC_test_1); 
// TEST_REGISTER(stateen_IMSIC_test_2);   
// TEST_REGISTER(stateen_IMSIC_test_3); 
// TEST_REGISTER(stateen_IMSIC_test_4);    //第一条用例difftest报错，rtl为虚拟指令异常（rtl符合预期）
// TEST_REGISTER(stateen_IMSIC_test_5);    //difftest报错,rtl产生非法指令异常（rtl符合预期）
// TEST_REGISTER(stateen_IMSIC_test_6);    //hu mode 要依赖sstatten
// TEST_REGISTER(stateen_IMSIC_test_7); 


// // hstatten未控制对应的访问
// TEST_REGISTER(stateen_AIA_test_1); 
// TEST_REGISTER(stateen_AIA_test_2); 
// TEST_REGISTER(stateen_AIA_test_3); 
// TEST_REGISTER(stateen_AIA_test_4); 
// TEST_REGISTER(stateen_AIA_test_5);  //difftest报错,rtl产生非法指令异常(除了第三条)（rtl符合预期）   （第三条用例都不符合）
// TEST_REGISTER(stateen_AIA_test_6);  //fail的涉及sstatten
// TEST_REGISTER(stateen_AIA_test_7);  //不知VU是否涉及sstatten

// TEST_REGISTER(stateen_CSRIND_test);     //测试点如何写

// TEST_REGISTER(stateen_ENVCFG_test_1); 
// TEST_REGISTER(stateen_ENVCFG_test_2); 
// TEST_REGISTER(stateen_ENVCFG_test_3); 
// TEST_REGISTER(stateen_ENVCFG_test_4);       //difftest报错,rtl产生虚拟指令异常（rtl符合预期）
// TEST_REGISTER(stateen_ENVCFG_test_5);       //difftest报错,rtl产生非法指令异常（rtl符合预期）
// TEST_REGISTER(stateen_ENVCFG_test_6);       //hu mode 要依赖sstatten
// TEST_REGISTER(stateen_ENVCFG_test_7);       //第一条不符合预期  不知VU是否涉及sstatten


// // 在所处特权级本身就无法访问高特权级CSR
// TEST_REGISTER(stateen_SE0_test_1); 
// TEST_REGISTER(stateen_SE0_test_2); 
// TEST_REGISTER(stateen_SE0_test_3); 
// TEST_REGISTER(stateen_SE0_test_4); 
// TEST_REGISTER(stateen_SE0_test_5); 
// TEST_REGISTER(stateen_SE0_test_6); 
// TEST_REGISTER(stateen_SE0_test_7); 
// TEST_REGISTER(stateen_SE0_test_8); 


// TEST_REGISTER(cycle_test_1);
// TEST_REGISTER(cycle_test_2);
// TEST_REGISTER(cycle_test_3);
// TEST_REGISTER(cycle_test_4);
// TEST_REGISTER(cycle_test_5);
// TEST_REGISTER(cycle_test_6);
// TEST_REGISTER(cycle_test_7);
// TEST_REGISTER(cycle_test_8);
// TEST_REGISTER(cycle_test_9);
// TEST_REGISTER(cycle_test_10);
// TEST_REGISTER(instret_test_1);
// TEST_REGISTER(instret_test_2);
// TEST_REGISTER(instret_test_3);
// TEST_REGISTER(instret_test_4);
// TEST_REGISTER(instret_test_5);
// TEST_REGISTER(instret_test_6);
// TEST_REGISTER(instret_test_7);
// TEST_REGISTER(instret_test_8);
// TEST_REGISTER(instret_test_9);
// TEST_REGISTER(instret_test_10);
// TEST_REGISTER(time_test_1);
// TEST_REGISTER(time_test_2);
// TEST_REGISTER(time_test_3);
// TEST_REGISTER(time_test_4);
// TEST_REGISTER(time_test_5);
// TEST_REGISTER(time_test_6);
// TEST_REGISTER(time_test_7);
// TEST_REGISTER(time_test_8);
// TEST_REGISTER(time_test_9);
// TEST_REGISTER(time_test_10);
// TEST_REGISTER(timecmp_test_1);
// TEST_REGISTER(timecmp_test_2);
// TEST_REGISTER(timecmp_test_3);
// TEST_REGISTER(timecmp_test_4);
// TEST_REGISTER(timecmp_test_5);
// TEST_REGISTER(timecmp_test_6);
// TEST_REGISTER(timecmp_test_7);
// TEST_REGISTER(timecmp_test_8);
// TEST_REGISTER(timecmp_test_9);
// TEST_REGISTER(timecmp_test_10);
// TEST_REGISTER(timecmp_test_11);
// TEST_REGISTER(timecmp_test_12);
// TEST_REGISTER(timecmp_test_13);
// TEST_REGISTER(timecmp_test_14);
// TEST_REGISTER(timecmp_test_15);
// TEST_REGISTER(timecmp_test_16);
// TEST_REGISTER(timecmp_test_17);
// TEST_REGISTER(timecmp_test_18);
// TEST_REGISTER(timecmp_test_19);
// TEST_REGISTER(timecmp_test_20);
// TEST_REGISTER(timecmp_test_21);

// TEST_REGISTER(priv_change_16);    
// TEST_REGISTER(priv_change_15);    
// TEST_REGISTER(priv_change_14);    
// TEST_REGISTER(priv_change_13);   
// TEST_REGISTER(priv_change_12);    
// TEST_REGISTER(priv_change_11);    
// TEST_REGISTER(priv_change_10);    
// TEST_REGISTER(priv_change_9); 

// TEST_REGISTER(mix_instruction_1); 
// TEST_REGISTER(instruction_access_fault_10);
// TEST_REGISTER(instruction_access_fault_9);
// TEST_REGISTER(instruction_access_fault_8);
// TEST_REGISTER(instruction_access_fault_7);
// TEST_REGISTER(instruction_access_fault_6);
// TEST_REGISTER(instruction_access_fault_5);
// TEST_REGISTER(instruction_access_fault_4);
// TEST_REGISTER(instruction_access_fault_3);
// TEST_REGISTER(instruction_access_fault_2);
// TEST_REGISTER(instruction_access_fault_1);
// TEST_REGISTER(store_access_fault_10);
// TEST_REGISTER(store_access_fault_9);
// TEST_REGISTER(store_access_fault_8);
// TEST_REGISTER(store_access_fault_7);
// TEST_REGISTER(store_access_fault_6);
// TEST_REGISTER(store_access_fault_5);
// TEST_REGISTER(store_access_fault_4);
// TEST_REGISTER(store_access_fault_3);
// TEST_REGISTER(store_access_fault_2);
// TEST_REGISTER(store_access_fault_1);
// TEST_REGISTER(amo_access_fault_10);
// TEST_REGISTER(amo_access_fault_9);
// TEST_REGISTER(amo_access_fault_8);
// TEST_REGISTER(amo_access_fault_7);
// TEST_REGISTER(amo_access_fault_6);
// TEST_REGISTER(amo_access_fault_5); 
// TEST_REGISTER(amo_access_fault_4);
// TEST_REGISTER(amo_access_fault_3);
// TEST_REGISTER(amo_access_fault_2);
// TEST_REGISTER(amo_access_fault_1);
// TEST_REGISTER(load_access_fault_10);
// TEST_REGISTER(load_access_fault_9);
// TEST_REGISTER(load_access_fault_8);
// TEST_REGISTER(load_access_fault_7);
// TEST_REGISTER(load_access_fault_6);
// TEST_REGISTER(load_access_fault_5);
// TEST_REGISTER(load_access_fault_4);
// TEST_REGISTER(load_access_fault_3);
// TEST_REGISTER(load_access_fault_2);
// TEST_REGISTER(load_access_fault_1);

// TEST_REGISTER(priv_change_8);    
// TEST_REGISTER(priv_change_7);   
// TEST_REGISTER(priv_change_6);    
// TEST_REGISTER(priv_change_5);    
// TEST_REGISTER(priv_change_4);    
// TEST_REGISTER(priv_change_3);   
// TEST_REGISTER(priv_change_2);    
// TEST_REGISTER(priv_change_1); 
// TEST_REGISTER(instruction_guest_page_fault_1);       
// TEST_REGISTER(instruction_guest_page_fault_2); 
// TEST_REGISTER(instruction_guest_page_fault_3);  
// TEST_REGISTER(instruction_guest_page_fault_4);
// TEST_REGISTER(instruction_guest_page_fault_5);  
// TEST_REGISTER(instruction_guest_page_fault_6);  
// TEST_REGISTER(instruction_guest_page_fault_7);  
// TEST_REGISTER(amo_guest_page_fault_1);
// TEST_REGISTER(amo_guest_page_fault_2);
// TEST_REGISTER(amo_guest_page_fault_3);
// TEST_REGISTER(amo_guest_page_fault_4);
// TEST_REGISTER(amo_guest_page_fault_5);
// TEST_REGISTER(store_guest_page_fault_1);
// TEST_REGISTER(store_guest_page_fault_2);
// TEST_REGISTER(store_guest_page_fault_3);
// TEST_REGISTER(store_guest_page_fault_4);
// TEST_REGISTER(store_guest_page_fault_5);
// TEST_REGISTER(store_guest_page_fault_6);
// TEST_REGISTER(store_guest_page_fault_7);
// TEST_REGISTER(store_guest_page_fault_8);
// TEST_REGISTER(store_guest_page_fault_9);
// TEST_REGISTER(store_guest_page_fault_10);
// TEST_REGISTER(load_guest_page_fault_1);
// TEST_REGISTER(load_guest_page_fault_2);
// TEST_REGISTER(load_guest_page_fault_3);
// TEST_REGISTER(load_guest_page_fault_4);
// TEST_REGISTER(load_guest_page_fault_5);
// TEST_REGISTER(load_guest_page_fault_6);
// TEST_REGISTER(load_guest_page_fault_7);
// TEST_REGISTER(load_guest_page_fault_8);
// TEST_REGISTER(load_guest_page_fault_9);
// TEST_REGISTER(load_guest_page_fault_10);
// TEST_REGISTER(load_guest_page_fault_11);
// TEST_REGISTER(load_guest_page_fault_12);
// TEST_REGISTER(load_guest_page_fault_13);
// TEST_REGISTER(load_guest_page_fault_14);
// TEST_REGISTER(load_guest_page_fault_15);
// TEST_REGISTER(instruction_page_fault_1);
// TEST_REGISTER(instruction_page_fault_2);
// TEST_REGISTER(instruction_page_fault_3);     
// TEST_REGISTER(instruction_page_fault_4);
// TEST_REGISTER(amo_page_fault_1);
// TEST_REGISTER(amo_page_fault_2);
// TEST_REGISTER(amo_page_fault_3);
// TEST_REGISTER(amo_page_fault_4);
// TEST_REGISTER(amo_page_fault_5);
// TEST_REGISTER(amo_page_fault_6);
// TEST_REGISTER(store_page_fault_1);
// TEST_REGISTER(store_page_fault_2);
// TEST_REGISTER(store_page_fault_3);
// TEST_REGISTER(store_page_fault_4);
// TEST_REGISTER(store_page_fault_5);
// TEST_REGISTER(store_page_fault_6);
// TEST_REGISTER(store_page_fault_7);
// TEST_REGISTER(store_page_fault_8);
// TEST_REGISTER(load_page_fault_1);
// TEST_REGISTER(load_page_fault_2);
// TEST_REGISTER(load_page_fault_3);
// TEST_REGISTER(load_page_fault_4);
// TEST_REGISTER(load_page_fault_5);
// TEST_REGISTER(load_page_fault_6);
// TEST_REGISTER(load_page_fault_7);
// TEST_REGISTER(load_page_fault_8);
// TEST_REGISTER(addr_unaligned);
// TEST_REGISTER(two_stage_translation_1);   
// TEST_REGISTER(two_stage_translation_2);   //need that xiangshan run it alone
// TEST_REGISTER(two_stage_translation_3);   //need that xiangshan run it alone

// TEST_REGISTER(second_stage_only_translation);
// TEST_REGISTER(m_and_hs_using_vs_access_1);
// TEST_REGISTER(m_and_hs_using_vs_access_2);
// TEST_REGISTER(m_and_hs_using_vs_access_3);
// TEST_REGISTER(m_and_hs_using_vs_access_4);
// TEST_REGISTER(m_and_hs_using_vs_access_5);
// TEST_REGISTER(m_and_hs_using_vs_access_6);
// TEST_REGISTER(m_and_hs_using_vs_access_7);
// TEST_REGISTER(m_and_hs_using_vs_access_8);
// TEST_REGISTER(m_and_hs_using_vs_access_9);
// TEST_REGISTER(m_and_hs_using_vs_access_10);
// TEST_REGISTER(m_and_hs_using_vs_access_11);
// TEST_REGISTER(m_and_hs_using_vs_access_12);
// TEST_REGISTER(check_xip_regs_3);
// TEST_REGISTER(check_xip_regs_2);
// TEST_REGISTER(check_xip_regs_1);
// TEST_REGISTER(interrupt_tests);
// TEST_REGISTER(illegal_except_1);
// TEST_REGISTER(illegal_except_2);
// TEST_REGISTER(illegal_except_3);
// TEST_REGISTER(illegal_except_4);
// TEST_REGISTER(illegal_except_5);
// TEST_REGISTER(illegal_except_6);
// TEST_REGISTER(illegal_except_7);
// TEST_REGISTER(illegal_except_8);
// TEST_REGISTER(illegal_except_9);
// TEST_REGISTER(illegal_except_10);
// TEST_REGISTER(illegal_except_11);
// TEST_REGISTER(illegal_except_12);
// TEST_REGISTER(illegal_except_13);
// TEST_REGISTER(illegal_except_14);
// TEST_REGISTER(illegal_except_15);      
// TEST_REGISTER(virtual_instruction_1);
// TEST_REGISTER(virtual_instruction_2);
// TEST_REGISTER(virtual_instruction_3);
// TEST_REGISTER(virtual_instruction_4);
// TEST_REGISTER(virtual_instruction_5);
// TEST_REGISTER(virtual_instruction_6);
// TEST_REGISTER(virtual_instruction_7);
// TEST_REGISTER(virtual_instruction_8);
// TEST_REGISTER(virtual_instruction_9);
// TEST_REGISTER(virtual_instruction_10);
// TEST_REGISTER(virtual_instruction_11);
// TEST_REGISTER(virtual_instruction_12);
// TEST_REGISTER(hfence_test); //need that xiangshan run it alone
// TEST_REGISTER(wfi_exception_tests_1);
// TEST_REGISTER(wfi_exception_tests_2);
// TEST_REGISTER(wfi_exception_tests_3);
// TEST_REGISTER(wfi_exception_tests_4);
// TEST_REGISTER(wfi_exception_tests_5);
// TEST_REGISTER(wfi_exception_tests_6);
// TEST_REGISTER(wfi_exception_tests_7);
// TEST_REGISTER(wfi_exception_tests_8);
// TEST_REGISTER(wfi_exception_tests_9);
// TEST_REGISTER(wfi_exception_tests_10);
// TEST_REGISTER(wfi_exception_tests_11);
// TEST_REGISTER(wfi_exception_tests_12);
// TEST_REGISTER(wfi_exception_tests_13);
// TEST_REGISTER(wfi_exception_tests_14);
// TEST_REGISTER(wfi_exception_tests_15);
// TEST_REGISTER(wfi_exception_tests_16);
// TEST_REGISTER(wfi_exception_tests_17);
// TEST_REGISTER(wfi_exception_tests_18);
// TEST_REGISTER(wfi_exception_tests_19);
// TEST_REGISTER(wfi_exception_tests_20);
// TEST_REGISTER(wfi_exception_tests_21);    
// TEST_REGISTER(wfi_exception_tests_22);    
// TEST_REGISTER(tinst_tests_pf);
// TEST_REGISTER(tinst_tests_gpf);
// TEST_REGISTER(random_instruction_tests);

// TEST_REGISTER(ebreak_tests_1);
// TEST_REGISTER(ebreak_tests_2);
// TEST_REGISTER(ebreak_tests_3);
// TEST_REGISTER(ebreak_tests_4);
// TEST_REGISTER(ebreak_tests_5);
