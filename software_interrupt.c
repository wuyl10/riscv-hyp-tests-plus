#include <rvh_test.h>
#include <page_tables.h>
#include <csrs.h> 

bool software_interrupt_msi() {

    TEST_START();

    TEST_SETUP_EXCEPT();

   
    //M模式下产生软件中断，mstatus.mie=1，且mip，mie对应的MSIP、MSIE拉高
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS,1ULL << 3);     //mie位
    CSRS(CSR_MIE,MIE_MSIE);
    CSRW(CSR_MIDELEG,0);
    CSRS(CSR_MIP,MIP_MSIP);     //read-only

    goto_priv(PRIV_M);
    TEST_ASSERT("m mode leads to M level software interrupt when mstatus.mie=1 and mip.msip=1 and mie.msie=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_MSI &&
        excpt.priv == PRIV_M
    ); 


    //M模式下产生软件中断，mstatus.mie=1，且mip.MSIP=1，mie.MSIE=0
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS,1ULL << 3);     
    CSRC(CSR_MIE,MIE_MSIE);
    CSRS(CSR_MIP,MIP_MSIP);

    goto_priv(PRIV_M);
    TEST_ASSERT("m mode leads to interrupt pending when mstatus.mie=1 and mip.msip=1 and mie.msie=0",
        excpt.triggered == false
    ); 


    //M模式下产生软件中断，mstatus.mie=1，且mip.MSIP=0，mie.MSIE=1
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS,1ULL << 3);     //mie位
    CSRS(CSR_MIE,MIE_MSIE);
    CSRC(CSR_MIP,MIP_MSIP);

    goto_priv(PRIV_M);
    TEST_ASSERT("m mode leads to M level software interrupt when mstatus.mie=1 and mip.msip=0 and mie.msie=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_MSI
    ); 

    //M模式下产生软件中断，mstatus.mie=0，且mip.MSIP=1，mie.MSIE=1
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRC(CSR_MSTATUS,1ULL << 3);     
    CSRS(CSR_MIE,MIE_MSIE);
    CSRS(CSR_MIP,MIP_MSIP);

    goto_priv(PRIV_M);
    TEST_ASSERT("m mode leads to no interrupt when mstatus.mie=0 and mip.msip=1 and mie.msie=1",
        excpt.triggered == false
    ); 

    //M模式下产生软件中断，mstatus.mie=0，且mip.MSIP=1，mie.MSIE=0
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRC(CSR_MSTATUS,1ULL << 3);     
    CSRC(CSR_MIE,MIE_MSIE);
    CSRS(CSR_MIP,MIP_MSIP);

    goto_priv(PRIV_M);
    TEST_ASSERT("m mode leads to no interrupt when mstatus.mie=0 and mip.msip=1 and mie.msie=0",
        excpt.triggered == false
    ); 

    //M模式下产生软件中断，mstatus.mie=0，且mip.MSIP=0，mie.MSIE=0
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRC(CSR_MSTATUS,1ULL << 3);     
    CSRC(CSR_MIE,MIE_MSIE);
    CSRC(CSR_MIP,MIP_MSIP);

    goto_priv(PRIV_M);
    TEST_ASSERT("m mode leads to no interrupt when mstatus.mie=0 and mip.msip=0 and mie.msie=0",
        excpt.triggered == false
    ); 

//HS mode 下

    TEST_SETUP_EXCEPT();
    //HS模式下产生高特权级别软件中断，mstatus.mie=1，且mip，mie对应的MSIP、MSIE拉高，无代理
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS,1ULL << 3);     //mie位
    CSRS(CSR_MIE,MIE_MSIE);
    CSRS(CSR_MIP,MIP_MSIP);
    CSRW(CSR_MIDELEG,0);
    CSRW(CSR_HIDELEG,0);

    goto_priv(PRIV_HS);
    TEST_ASSERT("hs mode leads to m level software interrupt when mstatus.mie=1 and mip.msip=1 and mie.msie=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_MSI &&
        excpt.priv == PRIV_M
    ); 

    
    //HS模式下产生高特权级别软件中断，mstatus.mie=1，且mip，mie对应的MSIP、MSIE拉高，有mideleg代理
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS,1ULL << 3);   
    CSRS(CSR_MIE,MIE_MSIE);
    CSRS(CSR_MIP,MIP_MSIP);
    CSRW(CSR_MIDELEG,(uint64_t)-1);
    CSRW(CSR_HIDELEG,0);

    goto_priv(PRIV_HS);
    TEST_ASSERT("hs mode leads to m level software interrupt when mstatus.mie=1 and mip.msip=1 and mie.msie=1 and mideleg=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_MSI &&
        excpt.priv == PRIV_HS
    ); 

    //HS模式下产生高特权级别软件中断，mstatus.mie=1，且mip.MSIP=1，mie.MSIE=0，无代理
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS,1ULL << 3);   
    CSRC(CSR_MIE,MIE_MSIE);
    CSRS(CSR_MIP,MIP_MSIP);
    CSRW(CSR_MIDELEG,0);
    CSRW(CSR_HIDELEG,0);

    goto_priv(PRIV_HS);
    TEST_ASSERT("hs mode leads to interrupt pending when mstatus.mie=1 and mip.msip=1 and mie.msie=0 and mideleg=1",
        excpt.triggered == false
    ); 

    //HS模式下产生高特权级别软件中断，mstatus.mie=1，且mip.MESIP=0，mie.MSIE=1，无代理
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS,1ULL << 3);   
    CSRS(CSR_MIE,MIE_MSIE);
    CSRC(CSR_MIP,MIP_MSIP);
    CSRW(CSR_MIDELEG,0);
    CSRW(CSR_HIDELEG,0);

    goto_priv(PRIV_HS);
    TEST_ASSERT("hs mode leads to m level software interrupt when mstatus.mie=1 and mip.msip=0 and mie.msie=1 and mideleg=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_MSI &&
        excpt.priv == PRIV_M
    ); 

    //HS模式下产生高特权级别软件中断，mstatus.mie=1，且mip.MSIP=0，mie.MSIE=1，无代理
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS,1ULL << 3);   
    CSRS(CSR_MIE,MIE_MSIE);
    CSRC(CSR_MIP,MIP_MSIP);
    CSRW(CSR_MIDELEG,1);
    CSRW(CSR_HIDELEG,0);

    goto_priv(PRIV_HS);
    TEST_ASSERT("hs mode leads to m level software interrupt when mstatus.mie=1 and mip.msip=0 and mie.msie=1 and mideleg=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_MSI &&
        excpt.priv == PRIV_HS
    ); 

    //HS模式下产生高特权级别软件中断，mstatus.mie=0，且mip.MSIP，mie.MSIE为任意值，无代理
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRC(CSR_MSTATUS,1ULL << 3);   
    CSRC(CSR_MIE,MIE_MSIE);
    CSRC(CSR_MIP,MIP_MSIP);
    CSRW(CSR_MIDELEG,0);
    CSRW(CSR_HIDELEG,0);

    goto_priv(PRIV_HS);
    TEST_ASSERT("hs mode leads to no interrupt when mstatus.mie=0 and mip.msip=0 and mie.msie=0 and mideleg=0",
        excpt.triggered == false
    ); 

//vs mode 下

    TEST_SETUP_EXCEPT();
    //VS模式下产生高特权级别软件中断，mstatus.mie=1，且mip，mie对应的MSIP、MSIE拉高，无代理
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS,1ULL << 3);     //mie位
    CSRS(CSR_MIE,MIE_MSIE);
    CSRS(CSR_MIP,MIP_MSIP);
    CSRW(CSR_MIDELEG,0);
    CSRW(CSR_HIDELEG,0);

    goto_priv(PRIV_VS);
    TEST_ASSERT("vs mode leads to m level software interrupt when mstatus.mie=1 and mip.msip=1 and mie.msie=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_MSI &&
        excpt.priv == PRIV_M
    ); 


    //vs模式下产生高特权级别软件中断，mstatus.mie=1，且mip，mie对应的MSIP、MSIE拉高，有mideleg代理
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS,1ULL << 3);   
    CSRS(CSR_MIE,MIE_MSIE);
    CSRS(CSR_MIP,MIP_MSIP);
    CSRW(CSR_MIDELEG,(uint64_t)-1);
    CSRW(CSR_HIDELEG,0);

    goto_priv(PRIV_VS);
    TEST_ASSERT("vs mode leads to m level software interrupt when mstatus.mie=1 and mip.msip=1 and mie.msie=1 and mideleg=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_MSI &&
        excpt.priv == PRIV_HS
    ); 

    //VS模式下产生高特权级别软件中断，mstatus.mie=1，且mip.MSIP=1，mie.MSIE=0，无代理
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS,1ULL << 3);   
    CSRC(CSR_MIE,MIE_MSIE);
    CSRS(CSR_MIP,MIP_MSIP);
    CSRW(CSR_MIDELEG,0);
    CSRW(CSR_HIDELEG,0);

    goto_priv(PRIV_VS);
    TEST_ASSERT("VS mode leads to  interrupt pending when mstatus.mie=1 and mip.msip=1 and mie.msie=0 and mideleg=1",
        excpt.triggered == false
    ); 

    //VS模式下产生高特权级别软件中断，mstatus.mie=1，且mip.MESIP=0，mie.MSIE=1，无代理
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS,1ULL << 3);   
    CSRS(CSR_MIE,MIE_MSIE);
    CSRC(CSR_MIP,MIP_MSIP);
    CSRW(CSR_MIDELEG,0);
    CSRW(CSR_HIDELEG,0);

    goto_priv(PRIV_VS);
    TEST_ASSERT("VS mode leads to m level software interrupt when mstatus.mie=1 and mip.msip=0 and mie.msie=1 and mideleg=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_MSI &&
        excpt.priv == PRIV_M
    ); 

    //VS模式下产生高特权级别软件中断，mstatus.mie=1，且mip.MSIP=0，mie.MSIE=1，无代理
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS,1ULL << 3);   
    CSRS(CSR_MIE,MIE_MSIE);
    CSRC(CSR_MIP,MIP_MSIP);
    CSRW(CSR_MIDELEG,1);
    CSRW(CSR_HIDELEG,0);

    goto_priv(PRIV_VS);
    TEST_ASSERT("VS mode leads to m level software interrupt when mstatus.mie=1 and mip.msip=0 and mie.msie=1 and mideleg=1 and",
        excpt.triggered == true &&
        excpt.cause == CAUSE_MSI &&
        excpt.priv == PRIV_HS
    ); 

    //VS模式下产生高特权级别软件中断，mstatus.mie=0，且mip.MSIP，mie.MSIE为任意值，无代理
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRC(CSR_MSTATUS,1ULL << 3);   
    CSRC(CSR_MIE,MIE_MSIE);
    CSRC(CSR_MIP,MIP_MSIP);
    CSRW(CSR_MIDELEG,0);
    CSRW(CSR_HIDELEG,0);

    goto_priv(PRIV_VS);
    TEST_ASSERT("VS mode leads to no interrupt when mstatus.mie=0 and mip.msip=0 and mie.msie=0 and mideleg=0",
        excpt.triggered == false
    );

    TEST_END();
}



bool software_interrupt_ssi() {

    TEST_START();

    TEST_SETUP_EXCEPT();

    //HS模式下产生软件中断，sstatus.sie=1，且sip，sie对应的SSIP、SSIE拉高
    goto_priv(PRIV_M);
    CSRS(CSR_SSTATUS,SSTATUS_SIE_BIT);    
    CSRS(CSR_SIE,SIE_SSIE);
    CSRW(CSR_MIDELEG,0);
    CSRS(CSR_SIP,SIP_SSIP);     //read-only

    goto_priv(PRIV_HS);
    TEST_ASSERT("hs mode leads to hs level software interrupt when sstatus.sie=1 and sip.ssip=1 and sie.ssie=1 and mideleg=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SSI &&
        excpt.priv == PRIV_M
    ); 

    //HS模式下产生软件中断，sstatus.sie=1，且sip，sie对应的SSIP、SSE拉高，有代理
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_SSTATUS,SSTATUS_SIE_BIT);  
    CSRW(CSR_MIDELEG,(uint64_t)-1);  
    CSRS(CSR_SIE,SIE_SSIE);
    CSRS(CSR_SIP,SIP_SSIP);     //read-only

    goto_priv(PRIV_HS);
    TEST_ASSERT("hs mode leads to hs level software interrupt when sstatus.sie=1 and sip.ssip=1 and sie.ssie=1 and mideleg=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SSI &&
        excpt.priv == PRIV_HS
    ); 


    //HS模式下产生软件中断，sstatus.sie=1，且sip.SSIP=1，sie.SSIE=0
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_SSTATUS,SSTATUS_SIE_BIT);  
    CSRC(CSR_SIE,SIE_SSIE);
    CSRS(CSR_SIP,SIP_SSIP);    

    goto_priv(PRIV_HS);
    TEST_ASSERT("hs mode leads to interrupt pending when sstatus.sie=1 and sip.ssip=1 and sie.ssie=0",
        excpt.triggered == false
    );

    //HS模式下产生软件中断，sstatus.sie=1，且sip.SSIP=0，sie.SSIE=1
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_SSTATUS,SSTATUS_SIE_BIT);  
    CSRW(CSR_MIDELEG,0);  
    CSRS(CSR_SIE,SIE_SSIE);
    CSRC(CSR_SIP,SIP_SSIP);    

    goto_priv(PRIV_HS);
    TEST_ASSERT("hs mode leads to hs level software interrupt when sstatus.sie=1 and sip.ssip=0 and sie.ssie=1 and mideleg=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SSI &&
        excpt.priv == PRIV_M
    );

    //HS模式下产生软件中断，sstatus.sie=1，且sip.SSIP=0，sie.SSIE=1，有代理
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_SSTATUS,SSTATUS_SIE_BIT);  
    CSRW(CSR_MIDELEG,(uint64_t)-1);  
    CSRS(CSR_SIE,SIE_SSIE);
    CSRC(CSR_SIP,SIP_SSIP);    

    goto_priv(PRIV_HS);
    TEST_ASSERT("hs mode leads to no interrupt when sstatus.sie=1 and sip.ssip=0 and sie.ssie=1 and mideleg=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SSI &&
        excpt.priv == PRIV_HS
    );


    //HS模式下产生软件中断，sstatus.sie=1，且sip.SSIP=0，sie.SSIE=0
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_SSTATUS,SSTATUS_SIE_BIT);  
    CSRC(CSR_SIE,SIE_SSIE);
    CSRC(CSR_SIP,SIP_SSIP);    

    goto_priv(PRIV_HS);
    TEST_ASSERT("hs mode leads to no interrupt when sstatus.sie=1 and sip.ssip=0 and sie.ssie=1",
        excpt.triggered == false
    );

    //HS模式下产生软件中断，sstatus.sie=0，且sip.SSIP=0，sie.SSIE=0
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_SSTATUS,SSTATUS_SIE_BIT);  
    CSRC(CSR_SIE,SIE_SSIE);
    CSRC(CSR_SIP,SIP_SSIP);    

    goto_priv(PRIV_HS);
    TEST_ASSERT("hs mode leads to no interrupt when sstatus.sie=0 and sip.ssip=0 and sie.ssie=0",
        excpt.triggered == false
    );

//VS-mode下

    TEST_SETUP_EXCEPT();
    //VS模式下产生软件中断，sstatus.sie=1，且sip，sie对应的SSIP、SSIE拉高
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_SSTATUS,SSTATUS_SIE_BIT);    
    CSRS(CSR_SIE,SIE_SSIE);
    CSRW(CSR_MIDELEG,0);
    CSRS(CSR_SIP,SIP_SSIP);     //read-only

    goto_priv(PRIV_VS);
    TEST_ASSERT("VS mode leads to hs level software interrupt when sstatus.sie=1 and sip.ssip=1 and sie.ssie=1 and mideleg=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SSI &&
        excpt.priv == PRIV_M
    ); 

    //VS模式下产生软件中断，sstatus.sie=1，且sip，sie对应的SSIP、SSE拉高，有代理
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_SSTATUS,SSTATUS_SIE_BIT);  
    CSRW(CSR_MIDELEG,(uint64_t)-1);  
    CSRS(CSR_SIE,SIE_SSIE);
    CSRS(CSR_SIP,SIP_SSIP);     //read-only

    goto_priv(PRIV_VS);
    TEST_ASSERT("VS mode leads to hs level software interrupt when sstatus.sie=1 and sip.ssip=1 and sie.ssie=1 and mideleg=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SSI &&
        excpt.priv == PRIV_HS
    ); 


    //VS模式下产生软件中断，sstatus.sie=1，且sip.SSIP=1，sie.SSIE=0
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_SSTATUS,SSTATUS_SIE_BIT);  
    CSRC(CSR_SIE,SIE_SSIE);
    CSRS(CSR_SIP,SIP_SSIP);    

    goto_priv(PRIV_VS);
    TEST_ASSERT("VS mode leads to interrupt pending when sstatus.sie=1 and sip.ssip=1 and sie.ssie=0",
        excpt.triggered == false
    );

    //VS模式下产生软件中断，sstatus.sie=1，且sip.SSIP=0，sie.SSIE=1
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_SSTATUS,SSTATUS_SIE_BIT);  
    CSRW(CSR_MIDELEG,0);  
    CSRS(CSR_SIE,SIE_SSIE);
    CSRC(CSR_SIP,SIP_SSIP);    

    goto_priv(PRIV_VS);
    TEST_ASSERT("VS mode leads to hs level software interrupt when sstatus.sie=1 and sip.ssip=0 and sie.ssie=1 and mideleg=0",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SSI &&
        excpt.priv == PRIV_M
    );

    //VS模式下产生软件中断，sstatus.sie=1，且sip.SSIP=0，sie.SSIE=1，有代理
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_SSTATUS,SSTATUS_SIE_BIT);  
    CSRW(CSR_MIDELEG,(uint64_t)-1);  
    CSRS(CSR_SIE,SIE_SSIE);
    CSRC(CSR_SIP,SIP_SSIP);    

    goto_priv(PRIV_VS);
    TEST_ASSERT("VS mode leads to no interrupt when sstatus.sie=1 and sip.ssip=0 and sie.ssie=1 and mideleg=1",
        excpt.triggered == true &&
        excpt.cause == CAUSE_SSI &&
        excpt.priv == PRIV_HS
    );


    //VS模式下产生软件中断，sstatus.sie=1，且sip.SSIP=0，sie.SSIE=0
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_SSTATUS,SSTATUS_SIE_BIT);  
    CSRC(CSR_SIE,SIE_SSIE);
    CSRC(CSR_SIP,SIP_SSIP);    

    goto_priv(PRIV_VS);
    TEST_ASSERT("VS mode leads to no interrupt when sstatus.sie=1 and sip.ssip=0 and sie.ssie=1",
        excpt.triggered == false
    );

    //VS模式下产生软件中断，sstatus.sie=0，且sip.SSIP=0，sie.SSIE=0
    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_M);
    CSRS(CSR_SSTATUS,SSTATUS_SIE_BIT);  
    CSRC(CSR_SIE,SIE_SSIE);
    CSRC(CSR_SIP,SIP_SSIP);    

    goto_priv(PRIV_VS);
    TEST_ASSERT("VS mode leads to no interrupt when sstatus.sie=0 and sip.ssip=0 and sie.ssie=0",
        excpt.triggered == false
    );

    TEST_END();
}


bool software_interrupt_vssi() {

    TEST_START();

    TEST_SETUP_EXCEPT();

    TEST_END();


}