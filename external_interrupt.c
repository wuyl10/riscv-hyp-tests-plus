#include <rvh_test.h>
#include <page_tables.h>
#include <csrs.h> 

bool external_interrupt_MEI() {

    TEST_START();

    TEST_SETUP_EXCEPT();

   
    //M模式下产生单bit外部中断，mstatus.mie=1，且mip，mie对应的MEIP、MEIE拉高
    goto_priv(PRIV_M);
    CSRW(CSR_MIDELEG,0);
    CSRS(CSR_MSTATUS,1ULL << 3);     //mie位
    CSRS(CSR_MIE,MIE_MEIE);         
    CSRS(CSR_MIP,MIP_MEIP);     //p33 read-only  is set and cleared by a platform-specific interrupt controller

    goto_priv(PRIV_M);
    TEST_ASSERT("m mode leads to M level external interrupt when mstatus.mie=1 and mip.meip=1 and mie.meie=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_MEI
    ); 

    //M模式下产生单bit外部中断，mstatus.mie=1，且mip.MEIP=1，mie.MEIE=0
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS,1ULL << 3);     
    CSRC(CSR_MIE,MIE_MEIE);
    CSRS(CSR_MIP,MIP_MEIP);

    goto_priv(PRIV_M);
    TEST_ASSERT("m mode leads to no interrupt pending when mstatus.mie=1 and mip.meip=1 and mie.meie=0",
        excpt.triggered == false
    ); 

    //M模式下产生单bit外部中断，mstatus.mie=1，且mip.MEIP=0，mie.MEIE=1
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS,1ULL << 3);     
    CSRS(CSR_MIE,MIE_MEIE);
    CSRC(CSR_MIP,MIP_MEIP);

    goto_priv(PRIV_M);
    TEST_ASSERT("m mode leads to M level external interrupt when mstatus.mie=1 and mip.meip=0 and mie.meie=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_MEI
    ); 
    
    //M模式下产生单bit外部中断，mstatus.mie=0，且mip.MEIP=1，mie.MIE=1
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRC(CSR_MSTATUS,1ULL << 3);     
    CSRS(CSR_MIE,MIE_MEIE);
    CSRS(CSR_MIP,MIP_MEIP);

    goto_priv(PRIV_M);
    TEST_ASSERT("m mode leads to a external interrupt when mstatus.mie=0 and mip.meip=1 and mie.meie=1",
        excpt.triggered == false
    ); 

    //M模式下产生单bit外部中断，mstatus.mie=0，且mip.MEIP=1，mie.MIE=0
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRC(CSR_MSTATUS,1ULL << 3)     
    CSRS(CSR_MIE,MIE_MEIE);
    CSRC(CSR_MIP,MIP_MEIP);

    goto_priv(PRIV_M);
    TEST_ASSERT("m mode leads to a external interrupt when mstatus.mie=0 and mip.meip=1 and mie.meie=0",
        excpt.triggered == false
    ); 
    
    //M模式下产生单bit外部中断，mstatus.mie=0，且mip.MEIP=0，mie.MIE=0
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRC(CSR_MSTATUS,1ULL << 3)     
    CSRC(CSR_MIE,MIE_MEIE);
    CSRC(CSR_MIP,MIP_MEIP);

    goto_priv(PRIV_M);
    TEST_ASSERT("m mode leads to a external interrupt when mstatus.mie=0 and mip.meip=0 and mie.meie=0",
        excpt.triggered == false
    ); 

//---------------------------------------------------------
    

    //HS模式下产生单bit外部中断，mstatus.mie=1，且mip，mie对应的MEIP、MEIE拉高
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS,1ULL << 3);     
    CSRS(CSR_MIE,MIE_MEIE);
    CSRS(CSR_MIP,MIP_MEIP);

    
    goto_priv(PRIV_HS);

    TEST_ASSERT("HS mode leads to external interrupt when mstatus.mie=1 and mip.meip=1 and mie.meie=1",
        excpt.triggered == true &&
        excpt.priv == PRIV_M
    ); 

    //HS模式下产生单bit外部中断，mstatus.mie=1，且mip，mie对应的MEIP、MEIE拉高,有hideleg代理
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS,1ULL << 3);     
    CSRS(CSR_MIE,MIE_MEIE);
    CSRW(CSR_HIDELEG,(uint64_t)-1);
    CSRS(CSR_MIP,MIP_MEIP);

    goto_priv(PRIV_HS);
    TEST_ASSERT("HS mode leads to external interrupt when mstatus.mie=1 and mip.meip=1 and mie.meie=1 ",
        excpt.triggered == true &&
        excpt.priv == PRIV_HS
    ); 

    //HS模式下产生高特权级别单bit外部中断，mstatus.mie=1，且mip.MEIP=1，mie.MEIE=0，无代理
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS,1ULL << 3);     
    CSRC(CSR_MIE,MIE_MEIE);
    CSRW(CSR_MIDELEG,0);
    CSRW(CSR_HIDELEG,0);
    CSRS(CSR_MIP,MIP_MEIP);

    goto_priv(PRIV_HS);
    TEST_ASSERT("HS mode leads to external interrupt when mstatus.mie=1 and mip.meip=1 and mie.meie=0 ",
        excpt.triggered == false
    ); 


    //HS模式下产生高特权级别单bit外部中断，mstatus.mie=1，且mip.MEIP=0，mie.MEIE=1，无代理
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS,1ULL << 3);     
    CSRS(CSR_MIE,MIE_MEIE);
    CSRC(CSR_MIP,MIP_MEIP);

    goto_priv(PRIV_HS);
    TEST_ASSERT("HS mode leads to external interrupt when mstatus.mie=1 and mip.meip=0 and mie.meie=1 ",
        excpt.triggered == true &&
        excpt.cause == CAUSE_MEI &&
        excpt.priv == PRIV_HS
    ); 


    //HS模式下产生高特权级别单bit外部中断，mstatus.mie=0，且mip.MEIP，mie.MIE为任意值，无代理
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRC(CSR_MSTATUS,1ULL << 3);     
    CSRS(CSR_MIE,MIE_MEIE);
    CSRC(CSR_MIDELEG,(uint64_t)-1);
    CSRC(CSR_MEDELEG,(uint64_t)-1
    );
    CSRC(CSR_MIP,MIP_MEIP);

    goto_priv(PRIV_HS);
    TEST_ASSERT("HS mode leads to no interrupt when mstatus.mie=0 and mip.meip=0 and mie.meie=1 ",
        excpt.triggered == false
    ); 




//vs-mode下
    //VS模式下产生单bit外部中断，mstatus.mie=1，且mip，mie对应的MEIP、MEIE拉高
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS,1ULL << 3);     
    CSRS(CSR_MIE,MIE_MEIE);
    CSRS(CSR_MIP,MIP_MEIP);

    
    goto_priv(PRIV_VS);

    TEST_ASSERT("VS mode leads to M level external interrupt when mstatus.mie=1 and mip.meip=1 and mie.meie=1",
        excpt.triggered == true &&
        excpt.priv == PRIV_M
    ); 

    //VS模式下产生单bit外部中断，mstatus.mie=1，且mip，mie对应的MEIP、MEIE拉高,有hideleg代理
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS,1ULL << 3);     
    CSRS(CSR_MIE,MIE_MEIE);
    CSRW(CSR_HIDELEG,(uint64_t)-1);
    CSRS(CSR_MIP,MIP_MEIP);

    goto_priv(PRIV_VS);
    TEST_ASSERT("VS mode leads to M level external interrupt when mstatus.mie=1 and mip.meip=1 and mie.meie=1 ",
        excpt.triggered == true &&
        excpt.priv == PRIV_VS
    ); 

    //VS模式下产生高特权级别单bit外部中断，mstatus.mie=1，且mip.MEIP=1，mie.MEIE=0，无代理
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS,1ULL << 3);     
    CSRC(CSR_MIE,MIE_MEIE);
    CSRW(CSR_MIDELEG,0);
    CSRW(CSR_HIDELEG,0);
    CSRS(CSR_MIP,MIP_MEIP);

    goto_priv(PRIV_VS);
    TEST_ASSERT("VS mode leads to M level external interrupt when mstatus.mie=1 and mip.meip=1 and mie.meie=0 ",
        excpt.triggered == false
    ); 


    //VS模式下产生高特权级别单bit外部中断，mstatus.mie=1，且mip.MEIP=0，mie.MEIE=1，无代理
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS,1ULL << 3);     
    CSRS(CSR_MIE,MIE_MEIE);
    CSRC(CSR_MIP,MIP_MEIP);

    goto_priv(PRIV_VS);
    TEST_ASSERT("VS mode leads to M level external interrupt when mstatus.mie=1 and mip.meip=0 and mie.meie=1 ",
        excpt.triggered == true &&
        excpt.cause == CAUSE_MEI &&
        excpt.priv == PRIV_VS
    ); 

    //VS模式下产生高特权级别单bit外部中断，mstatus.mie=0，且mip.MEIP，mie.MIE为任意值，无代理
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRC(CSR_MSTATUS,1ULL << 3);     
    CSRS(CSR_MIE,MIE_MEIE);
    CSRC(CSR_MIDELEG,(uint64_t)-1);
    CSRC(CSR_MEDELEG,(uint64_t)-1);
    CSRC(CSR_MIP,MIP_MEIP);

    goto_priv(PRIV_VS);
    TEST_ASSERT("VS mode leads to M level external interrupt when mstatus.mie=0 and mip.meip=0 and mie.meie=1 ",
        excpt.triggered == false
    ); 
  
//unknown ecall错误
    // //VS模式下产生高特权级别单bit外部中断，mstatus.mie=1，且mip.MEIP=0，mie.MIE=1，有m代理
    // TEST_SETUP_EXCEPT();
    // goto_priv(PRIV_M);
    // CSRS(CSR_MSTATUS,1ULL << 3);     
    // CSRS(CSR_MIE,MIE_MEIE);
    // CSRW(CSR_MIDELEG,(uint64_t)-1);
    // CSRW(CSR_MEDELEG,(uint64_t)-1);
    // CSRC(CSR_MIP,MIP_MEIP);

    // goto_priv(PRIV_VS);
    // TEST_ASSERT("VS mode leads to M level external interrupt when mstatus.mie=1 and mip.meip=0 and mie.meie=1 ",
    //     excpt.triggered == true &&
    //     excpt.cause == CAUSE_MEI &&
    //     excpt.priv == PRIV_VS
    // ); 

    // //HS模式下产生高特权级别单bit外部中断，mstatus.mie=1，且mip.MEIP=0，mie.MIE=1，有m代理
    // TEST_SETUP_EXCEPT();
    // goto_priv(PRIV_M);
    // CSRS(CSR_MSTATUS,1ULL << 3);     
    // CSRS(CSR_MIE,MIE_MEIE);
    // CSRW(CSR_MIDELEG,(uint64_t)-1);
    // CSRW(CSR_MEDELEG,(uint64_t)-1);
    // CSRC(CSR_MIP,MIP_MEIP);

    // goto_priv(PRIV_HS);
    // TEST_ASSERT("HS mode leads to M level external interrupt when mstatus.mie=1 and mip.meip=0 and mie.meie=1 ",
    //     excpt.triggered == true &&
    //     excpt.cause == CAUSE_MEI &&
    //     excpt.priv == PRIV_HS
    // ); 


    TEST_END();
}


bool external_interrupt_SEI() {

    TEST_START();

    TEST_SETUP_EXCEPT();

   
    //HS模式下产生单bit外部中断，sstatus.sie=1，且mip，sie对应的SEIP、SEIE拉高
    goto_priv(PRIV_HS);
    CSRS(CSR_SSTATUS,1ULL << 1);     //sie位
    CSRS(CSR_SIE,SIE_SEIE);
    CSRS(CSR_SIP,SIP_SEIP);

    goto_priv(PRIV_HS);
    TEST_ASSERT("hs mode leads to hs level external interrupt when sstatus.sie=1 and sip.seip=1 and sie.seie=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SEI &&
        excpt.priv == PRIV_M
    ); 

    // //HS模式下产生单bit外部中断，sstatus.sie=1，且sip，sie对应的SEIP、SEIE拉高，有代理
    // goto_priv(PRIV_M);
    // CSRW(CSR_MIDELEG,(uint64_t)-1);
    // CSRS(CSR_SSTATUS,1ULL << 1);     
    // CSRS(CSR_SIE,SIE_SEIE);
    // CSRS(CSR_SIP,SIP_SEIP);

    // goto_priv(PRIV_HS);
    // TEST_ASSERT("hs mode leads to hs level external interrupt when sstatus.sie=1 and sip.seip=1 and sie.seie=1 and mideleg=1",
    //     excpt.triggered == true &&
    //     excpt.cause == CAUSE_SEI &&
    //     excpt.priv == PRIV_HS
    // ); 

    //HS模式下产生单bit外部中断，sstatus.sie=1，且sip.SEIP=1，sie.SEIE=0
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_HS);
    CSRS(CSR_SSTATUS,1ULL << 1);     
    CSRC(CSR_SIE,SIE_SEIE);
    CSRS(CSR_SIP,SIP_SEIP);

    goto_priv(PRIV_HS);
    TEST_ASSERT("hs mode leads to no interrupt  when sstatus.sie=1 and sip.seip=1 and sie.seie=0",
        excpt.triggered == false
    ); 

    //HS模式下产生单bit外部中断，sstatus.sie=1，且sip.SEIP=0，sie.SEIE=1
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRW(CSR_MIDELEG,0);
    CSRS(CSR_SSTATUS,1ULL << 1);     
    CSRS(CSR_SIE,SIE_SEIE);
    CSRC(CSR_SIP,SIP_SEIP);

    goto_priv(PRIV_HS);
    TEST_ASSERT("hs mode leads to hs level external interrupt when sstatus.sie=1 and sip.seip=0 and sie.seie=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SEI &&
        excpt.priv == PRIV_M
    ); 

    // //HS模式下产生单bit外部中断，sstatus.sie=1，且sip.SEIP=0，sie.SEIE=1,有代理
    // TEST_SETUP_EXCEPT();
    // goto_priv(PRIV_M);
    // CSRW(CSR_MIDELEG,(uint64_t)-1);
    // CSRS(CSR_SSTATUS,1ULL << 1);     
    // CSRS(CSR_SIE,SIE_SEIE);
    // CSRC(CSR_SIP,SIP_SEIP);

    // goto_priv(PRIV_HS);
    // TEST_ASSERT("hs mode leads to hs level external interrupt when sstatus.sie=1 and sip.seip=0 and sie.seie=1 and mideleg=1",
    //     excpt.triggered == true &&
    //     excpt.cause == CAUSE_SEI &&
    //     excpt.priv == PRIV_HS
    // ); 
    

    //HS模式下产生单bit外部中断，sstatus.mie=1，且sip.SEIP=0，sie.SIE=0
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_SSTATUS,1ULL << 1)     
    CSRC(CSR_SIE,SIE_SEIE);
    CSRC(CSR_SIP,SIP_SEIP);

    goto_priv(PRIV_HS);
    TEST_ASSERT("hs mode leads to no interrupt when sstatus.sie=0 and sip.seip=1 and sie.seie=0",
        excpt.triggered == false
    ); 
    
    //HS模式下产生单bit外部中断，sstatus.sie=0，且sip.SEIP=0，sie.SIE=0
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRC(CSR_SSTATUS,1ULL << 1)     
    CSRC(CSR_SIE,SIE_SEIE);
    CSRC(CSR_SIP,SIP_SEIP);

    goto_priv(PRIV_HS);
    TEST_ASSERT("hs mode leads to no interrupt when sstatus.sie=0 and sip.seip=0 and sie.seie=0",
        excpt.triggered == false
    ); 

//---------------------------------------------------------
    

    //VS模式下产生单bit外部中断，sstatus.sie=1，且sip，sie对应的SEIP、SEIE拉高
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_SSTATUS,1ULL << 1);     
    CSRS(CSR_SIE,SIE_SEIE);
    CSRS(CSR_SIP,SIP_SEIP);

    
    goto_priv(PRIV_VS);

    TEST_ASSERT("VS mode leads to external interrupt when sstatus.sie=1 and sip.seip=1 and sie.seie=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SEI &&
        excpt.priv == PRIV_M
    ); 

    // //VS模式下产生单bit外部中断，sstatus.sie=1，且sip，sie对应的SEIP、SEIE拉高, 有代理
    // TEST_SETUP_EXCEPT();
    // goto_priv(PRIV_M);
    // CSRS(CSR_SSTATUS,1ULL << 1);     
    // CSRW(CSR_MIDELEG,(uint64_t)-1);
    // CSRS(CSR_SIE,SIE_SEIE);
    // CSRS(CSR_SIP,SIP_SEIP);

    
    // goto_priv(PRIV_VS);
    // TEST_ASSERT("VS mode leads to external interrupt when sstatus.sie=1 and sip.seip=1 and sie.seie=1 and mideleg=1",
    //     excpt.triggered == true &&
    //     excpt.cause == CAUSE_SEI &&
    //     excpt.priv == PRIV_HS
    // ); 

    //vs模式下产生HS级单bit外部中断，sstatus.sie=1，且sip=1，sie=0
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_SSTATUS,1ULL << 1);     
    CSRC(CSR_SIE,SIE_SEIE);
    CSRS(CSR_SIP,SIP_SEIP);

    
    goto_priv(PRIV_VS);

    TEST_ASSERT("VS mode leads to external interrupt when sstatus.sie=1 and sip.seip=1 and sie.seie=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SEI &&
        excpt.priv == PRIV_M
    ); 

    //vs模式下产生HS级单bit外部中断，sstatus.sie=1，且sip=0，sie=1
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_SSTATUS,1ULL << 1);     
    CSRS(CSR_SIE,SIE_SEIE);
    CSRC(CSR_SIP,SIP_SEIP);

    
    goto_priv(PRIV_VS);

    TEST_ASSERT("VS mode leads to external interrupt when sstatus.sie=1 and sip.seip=0 and sie.seie=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SEI &&
        excpt.priv == PRIV_M
    ); 

    // //vs模式下产生HS级单bit外部中断，sstatus.sie=1，且sip=0，sie=1,有代理
    // TEST_SETUP_EXCEPT();
    // goto_priv(PRIV_M);
    // CSRS(CSR_SSTATUS,1ULL << 1);     
    // CSRW(CSR_MIDELEG,(uint64_t)-1);
    // CSRS(CSR_SIE,SIE_SEIE);
    // CSRC(CSR_SIP,SIP_SEIP);

    
    // goto_priv(PRIV_VS);

    // TEST_ASSERT("VS mode leads to external interrupt when sstatus.sie=1 and sip.seip=0 and sie.seie=1 and mideleg=1",
    //     excpt.triggered == true &&
    //     excpt.cause == CAUSE_SEI &&
    //     excpt.priv == PRIV_HS
    // ); 

    //低特权模式下产生HS级单bit外部中断，sstatus.sie=1，且sip=0，sie=0
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_SSTATUS,1ULL << 1);     
    CSRC(CSR_SIE,SIE_SEIE);
    CSRC(CSR_SIP,SIP_SEIP);

    
    goto_priv(PRIV_VS);

    TEST_ASSERT("VS mode leads to no interrupt when sstatus.sie=1 and sip.seip=0 and sie.seie=0",
        excpt.triggered == false
    ); 

    //低特权模式下产生HS级单bit外部中断，sstatus.sie=0，且sip=0，sie=0
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRC(CSR_SSTATUS,1ULL << 1);     
    CSRC(CSR_SIE,SIE_SEIE);
    CSRC(CSR_SIP,SIP_SEIP);

    
    goto_priv(PRIV_VS);

    TEST_ASSERT("VS mode leads to no interrupt when sstatus.sie=1 and sip.seip=0 and sie.seie=0",
        excpt.triggered == false
    ); 

    TEST_END();
}

bool external_interrupt_VSEI() {

    TEST_START();

    TEST_SETUP_EXCEPT();

   
    //VS模式下产生单bit外部中断，hip，hie对应的SSEIP、SSEIE拉高
    goto_priv(PRIV_M);
    CSRS(CSR_HIE,HIE_VSEIE);
    CSRS(CSR_HIP,HIP_VSEIP);

    goto_priv(PRIV_VS);
    TEST_ASSERT("vs mode leads to vs level external interrupt when hip.vseip=1 and hie.vseie=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VSEI &&
        excpt.priv == PRIV_M
    ); 

    //VS模式下产生单bit外部中断，hip，hie对应的SSEIP、SSEIE拉高 ,开mideleg
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_HIE,HIE_VSEIE);
    CSRS(CSR_MIDELEG,(uint64_t)-1);
    CSRS(CSR_HIP,HIP_VSEIP);

    goto_priv(PRIV_VS);
    TEST_ASSERT("vs mode leads to vs level external interrupt when hip.vseip=1 and hie.vseie=1 and mideleg=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VSEI &&
        excpt.priv == PRIV_HS
    ); 


    //VS模式下产生单bit外部中断，hip，hie对应的SSEIP、SSEIE拉高 ,开mideleg,hideleg
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_HIE,HIE_VSEIE);
    CSRS(CSR_MIDELEG,(uint64_t)-1);
    CSRS(CSR_HIDELEG,(uint64_t)-1);
    CSRS(CSR_HIP,HIP_VSEIP);

    goto_priv(PRIV_VS);
    TEST_ASSERT("vs mode leads to vs level external interrupt when hip.vseip=1 and hie.vseie=1 and mideleg=1 and hideleg=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_VSEI &&
        excpt.priv == PRIV_VS
    ); 

    //VS模式下产生单bit外部中断，hip.SSEIP=1，hie.SSEIE=0
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRC(CSR_HIE,HIE_VSEIE);
    CSRC(CSR_MIDELEG,(uint64_t)-1);
    CSRC(CSR_HIDELEG,(uint64_t)-1);
    CSRS(CSR_HIP,HIP_VSEIP);

    goto_priv(PRIV_VS);
    TEST_ASSERT("vs mode leads to no interrupt when hip.vseip=1 and hie.vseie=0",
        excpt.triggered == false
    ); 

    //VS模式下产生单bit外部中断，hip.SSEIP=0，hie.SSEIE=1
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_HIE,HIE_VSEIE);
    CSRC(CSR_HIP,HIP_VSEIP);

    goto_priv(PRIV_VS);
    TEST_ASSERT("vs mode leads to vs level external interrupt when hip.vseip=1 and hie.vseie=0",
        excpt.triggered == false &&
        excpt.cause == CAUSE_VSEI &&
        excpt.priv == PRIV_M
    ); 

    //VS模式下产生单bit外部中断，hip.SSEIP=0，hie.SSEIE=1，开mideleg
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_HIE,HIE_VSEIE);
    CSRC(CSR_HIP,HIP_VSEIP);
    CSRW(CSR_MIDELEG,(uint64_t)-1);

    goto_priv(PRIV_VS);
    TEST_ASSERT("vs mode leads to vs level external interrupt when hip.vseip=1 and hie.vseie=0 and mideleg=1",
        excpt.triggered == false &&
        excpt.cause == CAUSE_VSEI &&
        excpt.priv == PRIV_HS
    ); 

    //VS模式下产生单bit外部中断，hip.SSEIP=0，hie.SSEIE=1，开mideleg,hideleg
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_HIE,HIE_VSEIE);
    CSRC(CSR_HIP,HIP_VSEIP);
    CSRW(CSR_MIDELEG,(uint64_t)-1);
    CSRW(CSR_HIDELEG,(uint64_t)-1);

    goto_priv(PRIV_VS);
    TEST_ASSERT("vs mode leads to vs level external interrupt when hip.vseip=1 and hie.vseie=0 and mideleg=1 and hideleg=1",
        excpt.triggered == false &&
        excpt.cause == CAUSE_VSEI &&
        excpt.priv == PRIV_VS
    ); 

    //VS模式下产生单bit外部中断，hip.SSEIP=0，hie.SSEIE=0
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRC(CSR_HIE,HIE_VSEIE);
    CSRC(CSR_HIP,HIP_VSEIP);

    goto_priv(PRIV_VS);
    TEST_ASSERT("vs mode leads to vs level external interrupt when hip.vseip=0 and hie.vseie=0",
        excpt.triggered == false
    ); 





    TEST_END();
}
