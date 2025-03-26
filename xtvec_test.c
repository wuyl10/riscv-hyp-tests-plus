/*
该文件测试分为4个模块
1、xtvec测试
2、smrnmi扩展测试
3、smdbltrp扩展测试
4、ssdbltrp扩展测试
*/

#include <rvh_test.h>
#include <page_tables.h>
#include <csrs.h> 
#include <stdio.h>



static inline uint64_t next_instruction(uint64_t epc){
    if(((*(uint16_t*)epc) & 0b11) == 0b11) return epc + 4;
    else return epc + 2;
}

static int binary_to_power_of_two(unsigned int num) {
    if (num == 0 || (num & (num - 1)) != 0) {
        return -1;  // 不是 2^x 的形式
    }
    int x = 0;
    while (num > 1) {
        num >>= 1;  // 右移
        x++;
    }
    return x;
}

// #define HANDLE_INTERRUPT(mode, num) \
//     void mode##_test_entry_two_##num() { \
//         printf("进入%s模式tvec指向入口(%d号中断)\n", #mode, num); \
//         mode##handler(); \
//     }

/*
    通过控制trap来控制是否产生double trap
    trap=0 代表在handler中不产生异常或者中断
    trap=1 代表在handler中产生一个中断
    trap=2 代表在handler中产生一个异常
*/
int trap = 0;
#define HANDLE_INTERRUPT(mode, num) \
    void mode##_test_entry_two_##num() { \
        printf("进入%s模式tvec指向入口(%d号中断)\n", #mode, num); \
        if (trap == 1) { \
            printf("处理中断号 %d,产生中断\n", num); \
            if(#mode == "m"){\
                trap = 0; \
                CSRC(CSR_MSTATUS , MSTATUS_MDT);\
                CSRS(CSR_MSTATUS , MSTATUS_MIE);\
                CSRS(mie, 1ULL << 1);\
                CSRS(mip, MIP_SSIP);\
            }\
        }  \
        if (trap == 2) { \
            printf("处理中断号 %d,执行非法指令\n", num); \
            trap = 0; \
            CSRW(CSR_MCONFIGPTR,0xfff); \
        }  \
        TEST_SETUP_EXCEPT(); \
        mode##handler(); \
    }\


HANDLE_INTERRUPT(m,1)
HANDLE_INTERRUPT(m,2)
HANDLE_INTERRUPT(m,3)
HANDLE_INTERRUPT(m,4)
HANDLE_INTERRUPT(m,5)
HANDLE_INTERRUPT(m,6)
HANDLE_INTERRUPT(m,7)
HANDLE_INTERRUPT(m,8)

HANDLE_INTERRUPT(hs,1)
HANDLE_INTERRUPT(hs,2)
HANDLE_INTERRUPT(hs,3)
HANDLE_INTERRUPT(hs,4)
HANDLE_INTERRUPT(hs,5)
HANDLE_INTERRUPT(hs,6)
HANDLE_INTERRUPT(hs,7)
HANDLE_INTERRUPT(hs,8)

HANDLE_INTERRUPT(vs,1)
HANDLE_INTERRUPT(vs,2)
HANDLE_INTERRUPT(vs,3)
HANDLE_INTERRUPT(vs,4)
HANDLE_INTERRUPT(vs,5)
HANDLE_INTERRUPT(vs,6)
HANDLE_INTERRUPT(vs,7)
HANDLE_INTERRUPT(vs,8)


void m_test_entry_two_0() { 
    TEST_SETUP_EXCEPT();

    printf("进入mtvec指向入口(异常)\n");

    if (trap == 1) { 
        trap = 0; 
        printf("mhandler中发生软件中断\n"); 
        CSRC(CSR_MSTATUS , MSTATUS_MDT);
        CSRS(CSR_MSTATUS , MSTATUS_MIE);
        CSRS(mie, 1ULL << 1);
        CSRS(mip, MIP_SSIP);

    }  

    if (trap == 2) { 
        trap = 0; 
        printf("mhandler中发生异常\n");
        CSRW(CSR_MCONFIGPTR,0xfff); 
    }  

    if (trap == 3) {        //供m_double_trap_3测试使用
        trap = 0;
        bool test_status = true;//使得assert能正常运行
        CSRC(CSR_MSTATUS, MSTATUS_MDT);
        CSRS(CSR_MSTATUS, MSTATUS_MIE);
        TEST_ASSERT("当 MDT 位已为0,通过显式的CSR写操作将 MIE 位设置为1",
            (CSRR(CSR_MSTATUS) & MSTATUS_MIE) != 0
        ); 
    }

    if (trap == 4) {        //供m_double_trap_4和5测试使用
        trap = 0;
        bool check1 = ((CSRR(CSR_MSTATUS) & MSTATUS_MDT) != 0);
        if (check1 == true)  trap = -1;
    }

    if (trap == 5) {        //供s_double_trap_3测试使用
        trap = 0;
        bool test_status = true;//使得assert能正常运行
        CSRC(CSR_SSTATUS, SSTATUS_SDT);
        CSRS(CSR_MSTATUS, MSTATUS_SIE);
        TEST_ASSERT("当 SDT 位已为0,通过显式的CSR写操作将 SIE 位设置为1",
            (CSRR(CSR_MSTATUS) & MSTATUS_SIE) != 0
        ); 
    }
    TEST_SETUP_EXCEPT();
    mhandler();
}
void hs_test_entry_two_0() { 
    printf("进入stvec指向入口(异常)\n");     

    if (trap == 1) { 
        trap = 0; 
        printf("hshandler中发生软件中断\n"); 
        CSRC(CSR_SSTATUS , SSTATUS_SDT);
        CSRS(CSR_SSTATUS , SSTATUS_SIE_BIT);
        CSRS(sie, 1ULL << 1);
        CSRS(sip, SIP_SSIP);

    }  

    if (trap == 2) { 
        trap = 0; 
        printf("hshandler中发生异常\n");
        CSRW(CSR_MCONFIGPTR,0xfff); 
        TEST_SETUP_EXCEPT();
    }  

    if (trap == 4) {        //供s_double_trap_4测试使用
        trap = 0;
        bool check1 = ((CSRR(CSR_SSTATUS) & SSTATUS_SDT) != 0);
        if (check1 == true)  trap = -1;
    }

    if (trap == 5) {   
        trap = 0;
        printf("hshandler中发生页错误异常\n");
        hspt_init();
        hpt_init();
        vspt_init();
        uintptr_t addr = hs_page_base(VSV_GV);
        CSRS(CSR_SSTATUS, SSTATUS_MXR);
        hlvd(addr);
    }

    TEST_SETUP_EXCEPT();
    hshandler();
}
void vs_test_entry_two_0() { 
    printf("进入vstvec指向入口(异常)\n"); 
    vshandler();
}
extern void m_test_entry();
extern void hs_test_entry();
extern void vs_test_entry();



//-------------------------------xtvec_test_begin-------------------------------------

bool mtvec_test_1() {

    //m模式下，当mtvec.mode=vectored，未开启中断代理，发生2号软件中断  
    TEST_START();

    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS,(1ULL << 3));
    CSRS(CSR_MSTATUS,(1ULL << 1));

    printf("mstatus=%llx\n",CSRR(CSR_MSTATUS));

    CSRS(mie, 1ULL << 1);

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);
    printf("mtvec=%llx \n",CSRR(CSR_MTVEC));

    TEST_SETUP_EXCEPT();
    printf("入口：");
    CSRS(mip, 1ULL << 1);

    TEST_ASSERT("m模式下,mtvec.mode=vectored.未开启中断代理,产生2号软件中断",
        excpt.triggered == true 
    ); 
}

bool mtvec_test_2() {

    TEST_START();

    //m模式下，当mtvec.mode=vectored，未开启异常代理，发生异常  

    goto_priv(PRIV_M);
    printf("mstatus=%llx\n",CSRR(CSR_MSTATUS));

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);
    printf("mtvec=%llx \n",CSRR(CSR_MTVEC));

    TEST_SETUP_EXCEPT();
    printf("入口：");
    CSRW(CSR_MCONFIGPTR,0xfff);     //访问只读csr

    TEST_ASSERT("m模式下,mtvec.mode=vectored,未开启异常代理,产生异常",
        excpt.triggered == true 
    ); 
}

bool mtvec_test_3() {

    TEST_START();

    //HS模式下，当mtvec.mode=vectored，未开启异常代理，发生异常  

    goto_priv(PRIV_M);
    printf("mstatus=%llx\n",CSRR(CSR_MSTATUS));

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);
    printf("mtvec=%llx \n",CSRR(CSR_MTVEC));

    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_HS);
    printf("入口：");
    CSRR(CSR_MSTATUS);  

    TEST_ASSERT("HS模式下,mtvec.mode=vectored,未开启异常代理,产生异常",
        excpt.triggered == true 
    ); 
}

bool mtvec_test_4() {

    TEST_START();

    //HU模式下，当mtvec.mode=vectored，未开启异常代理，发生异常  

    goto_priv(PRIV_M);
    printf("mstatus=%llx\n",CSRR(CSR_MSTATUS));

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);
    printf("mtvec=%llx \n",CSRR(CSR_MTVEC));

    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();
    printf("入口：");
    hfence_vvma();

    TEST_ASSERT("HU模式下,mtvec.mode=vectored,未开启异常代理,产生异常",
        excpt.triggered == true 
    ); 
}

bool mtvec_test_5() {

    TEST_START();

    //VS模式下，当mtvec.mode=vectored，未开启异常代理，发生异常  

    goto_priv(PRIV_M);
    printf("mstatus=%llx\n",CSRR(CSR_MSTATUS));

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);
    printf("mtvec=%llx \n",CSRR(CSR_MTVEC));

    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    printf("入口：");
    hfence_vvma();

    TEST_ASSERT("VS模式下,mtvec.mode=vectored,未开启异常代理,产生异常",
        excpt.triggered == true 
    ); 
}

bool mtvec_test_6() {

    TEST_START();

    //VU模式下，当mtvec.mode=vectored，未开启异常代理，发生异常  

    goto_priv(PRIV_M);
    printf("mstatus=%llx\n",CSRR(CSR_MSTATUS));

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);
    printf("mtvec=%llx \n",CSRR(CSR_MTVEC));

    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    printf("入口：");
    hfence_gvma();

    TEST_ASSERT("VU模式下,mtvec.mode=vectored,未开启异常代理,产生异常",
        excpt.triggered == true 
    ); 
}

bool mtvec_test_7() {

    TEST_START();

    //HS模式下，当mtvec.mode=vectored，未开启中断代理，发生中断  

    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS,1ULL << 3);
    CSRS(CSR_MSTATUS,1ULL << 1);
    CSRS(CSR_SSTATUS,1ULL << 1);
    CSRC(CSR_MIDELEG,1ULL << 1);
    printf("mstatus=%llx\n",CSRR(CSR_MSTATUS));

    TEST_SETUP_EXCEPT();
    CSRS(CSR_MVIEN,1ULL << 1);
    excpt_info();

    CSRS(mie, 1ULL << 1);

    // CSRS(sie, 1ULL << 1);
    printf("sie=%llx \n",CSRR(sie));

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);
    printf("mtvec=%llx \n",CSRR(CSR_MTVEC));

    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_HS);
    printf("入口：");
    CSRS(sip, 1ULL << 1);
    
    TEST_ASSERT("HS模式下,mtvec.mode=vectored,未开启中断代理,产生2号软件中断",
        excpt.triggered == true 
    ); 
}


// bool mtvec_test_8() {

//     TEST_START();

//     //HU模式下，当mtvec.mode=vectored，未开启中断代理，发生中断  

//     goto_priv(PRIV_M);
//     CSRS(CSR_MSTATUS,1ULL << 3);
//     CSRS(CSR_MSTATUS,1ULL << 1);
//     CSRS(CSR_SSTATUS,1ULL << 1);
//     CSRS(CSR_VSSTATUS,1ULL << 1);
//     CSRC(CSR_MIDELEG,1ULL << 1);
//     CSRC(CSR_MIDELEG,1ULL << 2);
//     CSRC(CSR_HIDELEG,1ULL << 2);
//     printf("mstatus=%llx\n",CSRR(CSR_MSTATUS));

//     CSRS(mie, 1ULL << 3);
//     CSRS(sie, 1ULL << 1);
//     CSRS(vsie, 1ULL << 1);

//     CSRW(CSR_VSTVEC,vs_test_entry);
//     CSRW(CSR_STVEC,hs_test_entry);
//     CSRW(CSR_MTVEC,m_test_entry);
//     CSRS(CSR_MTVEC , 1ULL << 0);
//     printf("mtvec=%llx \n",CSRR(CSR_MTVEC));

//     TEST_SETUP_EXCEPT();
//     printf("mtime=%llx \n",CSRR(CSR_TIME));
//     printf("mtime=%llx \n",CSRR(CSR_TIME));
//     printf("mtime=%llx \n",CSRR(CSR_TIME));
//     printf("mtime=%llx \n",CSRR(CSR_TIME));

//     goto_priv(PRIV_HU);

//     // TEST_END();
// }



bool stvec_test_1() {

    TEST_START();

    //hs模式下，开启了mideleg中断代理，关闭了hideleg中断代理，发生2号软件中断  

    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS,1ULL << 3);
    CSRS(CSR_MSTATUS,1ULL << 1);
    CSRS(CSR_SSTATUS,1ULL << 1);
    CSRS(CSR_MIDELEG,1ULL << 1);
    CSRS(CSR_HIDELEG,1ULL << 1);
    printf("mstatus=%llx\n",CSRR(CSR_MSTATUS));

    CSRS(mie, 1ULL << 1);
    CSRS(sie, 1ULL << 1);

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRS(CSR_STVEC , 1ULL << 0);
    printf("stvec=%llx \n",CSRR(CSR_STVEC));

    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_HS);
    printf("入口：");
    CSRS(sip, 1ULL << 1);
    
    TEST_ASSERT("HS模式下,mtvec.mode=vectored,mideleg=1,hideleg=0,产生2号软件中断",
        excpt.triggered == true 
    ); 
}

bool stvec_test_2() {

    TEST_START();

    //HS模式下，开启了medeleg代理，关闭hedeleg异常代理，发生异常

    goto_priv(PRIV_M);

    CSRS(CSR_MEDELEG,1ULL << 21);

    CSRW(CSR_MTVEC,m_test_entry);
    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRS(CSR_STVEC , 1ULL << 0);
    printf("stvec=%llx \n",CSRR(CSR_STVEC));


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
    printf("入口：");
    hlvh(vaddr);
    TEST_ASSERT("HS模式下,mtvec.mode=vectored,medeleg=1,hedeleg=0,产生异常",
        excpt.triggered == true 
    );
    
}

bool stvec_test_3() {

    TEST_START();

    //HU模式下，开启了medeleg代理，关闭hedeleg异常代理，发生异常

    goto_priv(PRIV_M);

    CSRS(CSR_MEDELEG,1ULL << 2);

    CSRW(CSR_MTVEC,m_test_entry);
    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRS(CSR_STVEC , 1ULL << 0);
    printf("stvec=%llx \n",CSRR(CSR_STVEC));

    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();      
    printf("入口：");
    CSRR(CSR_MSTATUS);
    
    TEST_ASSERT("HU模式下,mtvec.mode=vectored,medeleg=1,hedeleg=0,产生异常",
        excpt.triggered == true 
    );
}

bool stvec_test_4() {

    TEST_START();

    //VS模式下，开启了medeleg代理，关闭hedeleg异常代理，发生异常

    goto_priv(PRIV_M);
    CSRS(CSR_HSTATUS, HSTATUS_VTVM);

    CSRS(CSR_MEDELEG,1ULL << 22);

    CSRW(CSR_MTVEC,m_test_entry);
    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRS(CSR_STVEC , 1ULL << 0);
    printf("stvec=%llx \n",CSRR(CSR_STVEC));

    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    printf("入口：");
    sfence_vma();
    
    TEST_ASSERT("VS模式下,mtvec.mode=vectored,medeleg=1,hedeleg=0,产生异常",
        excpt.triggered == true 
    );
}

bool stvec_test_5() {

    TEST_START();

    //VU模式下，开启了medeleg代理，关闭hedeleg异常代理，发生异常

    goto_priv(PRIV_M);
    CSRS(CSR_HSTATUS, HSTATUS_VTVM);

    CSRS(CSR_MEDELEG,1ULL << 22);

    CSRW(CSR_MTVEC,m_test_entry);
    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRS(CSR_STVEC , 1ULL << 0);
    printf("stvec=%llx \n",CSRR(CSR_STVEC));

    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    printf("入口：");
    uint64_t tmp = hlvb(0);
    
    TEST_ASSERT("VU模式下,mtvec.mode=vectored,medeleg=1,hedeleg=0,产生异常",
        excpt.triggered == true 
    );
}




bool vstvec_test_1() {

    TEST_START();

    //vs模式下，当vstvec.mode=vectored，开启了mideleg和hideleg中断代理,,发生2号软件中断  

    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS,1ULL << 3);
    CSRS(CSR_MSTATUS,1ULL << 1);
    CSRS(CSR_SSTATUS,1ULL << 1);
    CSRS(CSR_VSSTATUS,1ULL << 1);
    CSRS(CSR_MIDELEG,1ULL << 1);
    CSRS(CSR_MIDELEG,1ULL << 2);
    CSRS(CSR_HIDELEG,1ULL << 2);
    printf("mstatus=%llx\n",CSRR(CSR_MSTATUS));

    CSRS(mie, 1ULL << 3);
    CSRS(sie, 1ULL << 1);
    CSRS(vsie, 1ULL << 1);

    CSRW(CSR_VSTVEC,vs_test_entry);
    printf("vstvec=%llx \n",CSRR(CSR_VSTVEC));
    CSRS(CSR_VSTVEC , 1ULL << 0);
    printf("vstvec=%llx \n",CSRR(CSR_VSTVEC));

    TEST_SETUP_EXCEPT();
    CSRS(hip, 1ULL << 2);       //当hideleg=1，hvip.VSSIP是hip.VSSIP的别名
    printf("入口：");
    goto_priv(PRIV_VS);

    TEST_ASSERT("vs模式下,mtvec.mode=vectored.,mideleg=1,hideleg=1,产生2号软件中断",
        excpt.triggered == true 
    ); 
    

}

bool vstvec_test_2() {

    TEST_START();

    //VS模式下，开启了medeleg代理和hedeleg异常代理，发生异常

    goto_priv(PRIV_M);
    CSRS(CSR_HSTATUS, HSTATUS_VTVM);

    CSRS(CSR_MEDELEG,1ULL << 13);
    CSRS(CSR_HEDELEG,1ULL << 13);

    CSRW(CSR_MTVEC,m_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRS(CSR_VSTVEC , 1ULL << 0);
    printf("vstvec=%llx \n",CSRR(CSR_VSTVEC));

    hspt_init();
    hpt_init();
    vspt_init();

    uintptr_t vaddr;
    uintptr_t addr;

    goto_priv(PRIV_VS);

    //执行取指指令时，pte.v=0
    TEST_SETUP_EXCEPT();

    printf("VS模式下,mtvec.mode=vectored,medeleg=1,hedeleg=1,产生异常\n\n");

    addr = hs_page_base(VSI_GI);
    printf("入口：");
    lb(addr);   

    // TEST_ASSERT("VS模式下,mtvec.mode=vectored,medeleg=1,hedeleg=1,产生异常",
    //     excpt.triggered == true 
    // );

}

bool vstvec_test_3() {      //待修改

    TEST_START();

    //VU模式下，开启了medeleg代理和hedeleg异常代理，发生异常

    goto_priv(PRIV_M);
    
    CSRS(CSR_HSTATUS, HSTATUS_VTVM);

    CSRS(CSR_MEDELEG,1ULL << 22);
    CSRS(CSR_HEDELEG,1ULL << 22);
    CSRS(CSR_MEDELEG,1ULL << 2);
    CSRS(CSR_HEDELEG,1ULL << 2);

    CSRW(CSR_MTVEC,m_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRS(CSR_VSTVEC , 1ULL << 0);
    printf("vstvec=%llx \n",CSRR(CSR_VSTVEC));

    goto_priv(PRIV_VU);

    //执行取指指令时，pte.v=0
    TEST_SETUP_EXCEPT();     

    printf("VU模式下,mtvec.mode=vectored,medeleg=1,hedeleg=1,产生异常\n\n");

    printf("入口：");   
    CSRR(CSR_MSTATUS);

    // TEST_ASSERT("VU模式下,mtvec.mode=vectored,medeleg=1,hedeleg=1,产生异常",
    //     excpt.triggered == true 
    // );

}



bool vstvec_test_4() {

    TEST_START();

    //vs模式下，当vstvec.mode=direct，开启了mideleg和hideleg中断代理,发生2号软件中断  

    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS,1ULL << 3);
    CSRS(CSR_MSTATUS,1ULL << 1);
    CSRS(CSR_SSTATUS,1ULL << 1);
    CSRS(CSR_VSSTATUS,1ULL << 1);
    CSRS(CSR_MIDELEG,1ULL << 1);
    CSRS(CSR_MIDELEG,1ULL << 2);
    CSRS(CSR_HIDELEG,1ULL << 2);
    printf("mstatus=%llx\n",CSRR(CSR_MSTATUS));

    CSRS(mie, 1ULL << 3);
    CSRS(sie, 1ULL << 1);
    CSRS(vsie, 1ULL << 1);

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRC(CSR_VSTVEC , 1ULL << 0);
    printf("vstvec=%llx \n",CSRR(CSR_VSTVEC));

    TEST_SETUP_EXCEPT();
    CSRS(hip, 1ULL << 2);       //当hideleg=1，hvip.VSSIP是hip.VSSIP的别名

    printf("Vs模式下,mtvec.mode=direct,medeleg=1,hedeleg=1,产生2号软件中断\n\n");

    printf("入口：");
    goto_priv(PRIV_VS);

    // TEST_ASSERT("vs模式下,mtvec.mode=direct.,mideleg=1,hideleg=1,产生2号软件中断",
    //     excpt.triggered == true 
    // );
}

bool vstvec_test_5() {

    TEST_START();

    //vu模式下，当vstvec.mode=direct，开启了mideleg和hideleg中断代理,,发生2号软件中断  

    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS,1ULL << 3);
    CSRS(CSR_MSTATUS,1ULL << 1);
    CSRS(CSR_SSTATUS,1ULL << 1);
    CSRS(CSR_VSSTATUS,1ULL << 1);
    CSRS(CSR_MIDELEG,1ULL << 1);
    CSRS(CSR_MIDELEG,1ULL << 2);
    CSRS(CSR_HIDELEG,1ULL << 2);
    printf("mstatus=%llx\n",CSRR(CSR_MSTATUS));

    CSRS(mie, 1ULL << 3);
    CSRS(sie, 1ULL << 1);
    CSRS(vsie, 1ULL << 1);

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRC(CSR_VSTVEC , 1ULL << 0);
    printf("vstvec=%llx \n",CSRR(CSR_VSTVEC));

    TEST_SETUP_EXCEPT();
    CSRS(hip, 1ULL << 2);       //当hideleg=1，hvip.VSSIP是hip.VSSIP的别名

    printf("VU模式下,mtvec.mode=direct,medeleg=1,hedeleg=1,产生2号软件中断\n\n");

    printf("入口：");
    goto_priv(PRIV_VU);

    // TEST_ASSERT("vu模式下,mtvec.mode=direct.,mideleg=1,hideleg=1,产生2号软件中断",
    //     excpt.triggered == true 
    // );
}

bool vstvec_test_6() {

    TEST_START();

    //vu模式下，当vstvec.mode=vectored，开启了mideleg和hideleg中断代理,,发生2号软件中断  

    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS,1ULL << 3);
    CSRS(CSR_MSTATUS,1ULL << 1);
    CSRS(CSR_SSTATUS,1ULL << 1);
    CSRS(CSR_VSSTATUS,1ULL << 1);
    CSRS(CSR_MIDELEG,1ULL << 1);
    CSRS(CSR_MIDELEG,1ULL << 2);
    CSRS(CSR_HIDELEG,1ULL << 2);
    printf("mstatus=%llx\n",CSRR(CSR_MSTATUS));

    CSRS(mie, 1ULL << 3);
    CSRS(sie, 1ULL << 1);
    CSRS(vsie, 1ULL << 1);

    CSRW(CSR_VSTVEC,vs_test_entry);
    printf("vstvec=%llx \n",CSRR(CSR_VSTVEC));
    CSRS(CSR_VSTVEC , 1ULL << 0);
    printf("vstvec=%llx \n",CSRR(CSR_VSTVEC));

    TEST_SETUP_EXCEPT();
    CSRS(hip, 1ULL << 2);       //当hideleg=1，hvip.VSSIP是hip.VSSIP的别名
    printf("入口：");
    goto_priv(PRIV_VU);

    TEST_ASSERT("vu模式下,mtvec.mode=vectored.,mideleg=1,hideleg=1,产生2号软件中断",
        excpt.triggered == true 
    );
}


bool mnret_test_1() {
    //m模式下，当mtvec.mode=vectored，未开启中断代理，发生2号软件中断 ,执行mnret返回
    TEST_START();

    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS,(1ULL << 3));
    CSRS(CSR_MSTATUS,(1ULL << 1));

    printf("mstatus=%llx\n",CSRR(CSR_MSTATUS));

    CSRS(mie, 1ULL << 1);

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);


    printf("mtvec=%llx \n",CSRR(CSR_MTVEC));

    TEST_SETUP_EXCEPT();
    printf("入口：");
    CSRS(mip, 1ULL << 1);

    TEST_ASSERT("m模式下,mtvec.mode=vectored.未开启中断代理,产生2号软件中断,执行mnret返回",      //要修改xtvec_test_asm.S中的返回指令
        excpt.triggered == true 
    ); 
}

bool mnret_test_2() {
    //m模式下，当mtvec.mode=vectored，mstatus.MDT=1,mnstatus.NMIE=1时，产生异常
    TEST_START();

    goto_priv(PRIV_M);

    printf("mstatus=%llx\n",CSRR(CSR_MSTATUS));

    CSRS(mie, 1ULL << 1);

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);
    CSRS(CSR_MSTATUS , 1ULL << 42); 
    CSRS(CSR_MNSTATUS , 1ULL << 3);


    printf("mstatus=%llx \n",CSRR(CSR_MSTATUS));
    printf("mnstatus=%llx \n",CSRR(CSR_MNSTATUS));

    printf("mtvec=%llx \n",CSRR(CSR_MTVEC));

    TEST_SETUP_EXCEPT();
    printf("入口：");
    CSRW(CSR_MCONFIGPTR,0xfff);
    excpt_info();
    TEST_ASSERT("m模式下,mtvec.mode=vectored,mstatus.MDT=1,mnstatus.NMIE=1时产生异常",      
        excpt.triggered == true 
    ); 
}

//-------------------------------xtvec_test_end-------------------------------------



//-------------------------------smrnmi_test_begin-------------------------------------

bool smrnmi_test_1() {
    //m模式下，当mtvec.mode=vectored，未开启中断代理，发生2号软件中断 ,执行mnret返回
    TEST_START();

    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS, MSTATUS_MIE);
    CSRS(CSR_MSTATUS, MSTATUS_SIE);

    CSRS(mie, MIE_SSIE);

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);


    printf("mtvec=%llx \n",CSRR(CSR_MTVEC));

    TEST_SETUP_EXCEPT();
    printf("入口：");
    CSRS(mip, 1ULL << 1);

    TEST_ASSERT("m模式下,mtvec.mode=vectored.未开启中断代理,产生2号软件中断,执行mnret返回",      
        excpt.triggered == true 
    ); 
}

bool smrnmi_test_2() {
    //m模式下，当mtvec.mode=vectored，mstatus.MDT=1,mnstatus.NMIE=1时，产生异常
    TEST_START();

    goto_priv(PRIV_M);

    printf("mstatus=%llx\n",CSRR(CSR_MSTATUS));

    CSRS(mie, 1ULL << 1);

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);
    CSRS(CSR_MSTATUS , 1ULL << 42); 
    CSRS(CSR_MNSTATUS , 1ULL << 3);


    printf("mstatus=%llx \n",CSRR(CSR_MSTATUS));
    printf("mnstatus=%llx \n",CSRR(CSR_MNSTATUS));

    printf("mtvec=%llx \n",CSRR(CSR_MTVEC));

    TEST_SETUP_EXCEPT();
    printf("入口：");
    CSRW(CSR_MCONFIGPTR,0xfff);
    excpt_info();
    TEST_ASSERT("m模式下,mtvec.mode=vectored,mstatus.MDT=1,mnstatus.NMIE=1时产生异常",      
        excpt.triggered == true 
    ); 
}

//-------------------------------smrnmi_test_end-------------------------------------

//-------------------------------smdbltrp_test_begin-------------------------------------

bool m_double_trap_1() {

    //显式CSR写 MDT 位为1时，MIE（机器中断使能）位被清除为0
    TEST_START();

    goto_priv(PRIV_M);

    CSRS(CSR_MSTATUS, MSTATUS_MIE);

    CSRS(CSR_MSTATUS, MSTATUS_MDT);
    TEST_ASSERT("显式CSR写 MDT 位为1时，MIE（机器中断使能）位被清除为0",
        (CSRR(CSR_MSTATUS) & MSTATUS_MIE) == 0
    ); 

}



bool m_double_trap_2() {

    //当 MDT 位已为0时，通过显式的CSR写操作将 MIE 位设置为1
    TEST_START();

    goto_priv(PRIV_M);

    CSRS(CSR_MSTATUS, MSTATUS_MIE);

    TEST_ASSERT("当 MDT 位已为0，通过显式的CSR写操作将 MIE 位设置为1",
        (CSRR(CSR_MSTATUS) & MSTATUS_MIE) == 1
    ); 

}

bool m_double_trap_3() {

    //当 MDT 位（对于RV64）在同一次写操作中被设置为0时，通过显式的CSR写操作将 MIE 位设置为1
    TEST_START();

    goto_priv(PRIV_M);
    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);

    TEST_SETUP_EXCEPT();

    trap=3;
    CSRW(CSR_MCONFIGPTR,0xfff); //陷入异常使mdt为1

}

bool m_double_trap_4() {

    //当在M模式下执行MRET指令时，MDT位被设置为0
    TEST_START();

    goto_priv(PRIV_M);
    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);

    TEST_SETUP_EXCEPT();

    trap=4;
    CSRW(CSR_MCONFIGPTR,0xfff); 

    bool check1,check2;

    if(trap == -1) check1 = true;
    else check1 = false;

    check2 = ((CSRR(CSR_MSTATUS) & MSTATUS_MDT) == 0);

    TEST_ASSERT("当在M模式下执行MRET和SRET指令时，MDT位被设置为0",
        check1 &&
        check2
    ); 
    trap = 0;
}

bool m_double_trap_5() {

    //当执行MRET指令，返回新特权模式S时，MDT位被设置为0
    TEST_START();

    goto_priv(PRIV_M);
    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_STVEC , 1ULL << 0);

    TEST_SETUP_EXCEPT();
    printf("%llx\n",CSRR(CSR_MEDELEG));

    goto_priv(PRIV_HS);    

    trap=4;
    TEST_SETUP_EXCEPT();

    CSRW(CSR_MCONFIGPTR,0xfff); 

    bool check1,check2;

    if(trap == -1) check1 = true;
    else check1 = false;

    goto_priv(PRIV_M);

    check2 = ((CSRR(CSR_MSTATUS) & MSTATUS_MDT) == 0);

    TEST_ASSERT("当执行MRET指令，返回新特权模式S时，MDT位被设置为0",
        check1 &&
        check2
    ); 
    trap = 0;
}

bool m_double_trap_6() {

    //当执行MNRET指令，返回新特权模式S时，MDT位被设置为0
    TEST_START();

    goto_priv(PRIV_M);
    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_STVEC , 1ULL << 0);

    TEST_SETUP_EXCEPT();

    goto_priv(PRIV_HS);    

    trap=4;
    TEST_SETUP_EXCEPT();

    CSRW(CSR_MCONFIGPTR,0xfff); 

    bool check1,check2;

    if(trap == -1) check1 = true;
    else check1 = false;

    goto_priv(PRIV_M);

    check2 = ((CSRR(CSR_MSTATUS) & MSTATUS_MDT) == 0);

    TEST_ASSERT("当执行MNRET指令，返回新特权模式S时，MDT位被设置为0",   //需要改对应.S文件
        check1 &&
        check2
    ); 
    trap = 0;
}

bool m_double_trap_7() {

    //当执行MRET指令，返回新特权模式HU时，MDT位设置为0，sstatus.SDT=0
    TEST_START();

    goto_priv(PRIV_M);
    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_STVEC , 1ULL << 0);

    TEST_SETUP_EXCEPT();
    printf("%llx\n",CSRR(CSR_MEDELEG));

    goto_priv(PRIV_HU);    

    trap=4;
    TEST_SETUP_EXCEPT();

    CSRW(CSR_MCONFIGPTR,0xfff); 

    bool check1,check2,check3;

    if(trap == -1) check1 = true;
    else check1 = false;


    goto_priv(PRIV_M);

    check2 = ((CSRR(CSR_SSTATUS) & SSTATUS_SDT) == 0);
    check3 = ((CSRR(CSR_MSTATUS) & MSTATUS_MDT) == 0);

    TEST_ASSERT("当执行MRET指令，返回新特权模式HU时，MDT位设置为0，sstatus.SDT=0",
        check1 &&
        check2 &&
        check3
    ); 
    trap = 0;
}


bool m_double_trap_8() {

    //当执行MNRET指令，返回新特权模式HU时，MDT位被设置为0，sstatus.SDT=0
    TEST_START();

    goto_priv(PRIV_M);
    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_STVEC , 1ULL << 0);

    TEST_SETUP_EXCEPT();

    goto_priv(PRIV_HU);    

    trap=4;
    TEST_SETUP_EXCEPT();

    CSRW(CSR_MCONFIGPTR,0xfff); 

    bool check1,check2,check3;

    if(trap == -1) check1 = true;
    else check1 = false;


    goto_priv(PRIV_M);

    check2 = ((CSRR(CSR_SSTATUS) & SSTATUS_SDT) == 0);
    check3 = ((CSRR(CSR_MSTATUS) & MSTATUS_MDT) == 0);

    TEST_ASSERT("当执行MNRET指令，返回新特权模式HU时，MDT位被设置为0，sstatus.SDT=0",   //需要改对应.S文件
        check1 &&
        check2 &&
        check3
    ); 
    trap = 0;
}

bool m_double_trap_9() {

    //当执行MRET指令，返回新特权模式VS时，MDT位被设置为0，sstatus.SDT=0
    TEST_START();

    goto_priv(PRIV_M);
    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_STVEC , 1ULL << 0);

    TEST_SETUP_EXCEPT();
    printf("%llx\n",CSRR(CSR_MEDELEG));

    goto_priv(PRIV_VS);    

    trap=4;
    TEST_SETUP_EXCEPT();

    CSRW(CSR_MCONFIGPTR,0xfff); 

    bool check1,check2,check3;

    if(trap == -1) check1 = true;
    else check1 = false;


    goto_priv(PRIV_M);

    check2 = ((CSRR(CSR_SSTATUS) & SSTATUS_SDT) == 0);
    check3 = ((CSRR(CSR_MSTATUS) & MSTATUS_MDT) == 0);

    TEST_ASSERT("当执行MRET指令，返回新特权模式VS时，MDT位被设置为0，sstatus.SDT=0",
        check1 &&
        check2 &&
        check3
    ); 
    trap = 0;
}


bool m_double_trap_10() {

    //当执行MNRET指令，返回新特权模式VS时，MDT位被设置为0
    TEST_START();

    goto_priv(PRIV_M);
    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_STVEC , 1ULL << 0);

    TEST_SETUP_EXCEPT();

    goto_priv(PRIV_VS);    

    trap=4;
    TEST_SETUP_EXCEPT();

    CSRW(CSR_MCONFIGPTR,0xfff); 

    bool check1,check2,check3;

    if(trap == -1) check1 = true;
    else check1 = false;


    goto_priv(PRIV_M);

    check2 = ((CSRR(CSR_SSTATUS) & SSTATUS_SDT) == 0);
    check3 = ((CSRR(CSR_MSTATUS) & MSTATUS_MDT) == 0);

    TEST_ASSERT("当执行MNRET指令，返回新特权模式VS时，MDT位被设置为0，sstatus.SDT=0",   //需要改对应.S文件
        check1 &&
        check2 &&
        check3
    ); 
    trap = 0;
}

bool m_double_trap_11() {

    //当执行MRET指令，返回新特权模式VU时，MDT位被设置为0
    TEST_START();

    goto_priv(PRIV_M);
    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_STVEC , 1ULL << 0);

    TEST_SETUP_EXCEPT();
    printf("%llx\n",CSRR(CSR_MEDELEG));

    goto_priv(PRIV_VS);    

    trap=4;
    TEST_SETUP_EXCEPT();

    CSRW(CSR_MCONFIGPTR,0xfff); 

    bool check1,check2,check3;

    if(trap == -1) check1 = true;
    else check1 = false;


    goto_priv(PRIV_M);

    check3 = ((CSRR(CSR_VSSTATUS) & VSSTATUS_SDT) == 0);
    check2 = ((CSRR(CSR_SSTATUS) & SSTATUS_SDT) == 0);
    check3 = ((CSRR(CSR_MSTATUS) & MSTATUS_MDT) == 0);

    TEST_ASSERT("当执行MRET指令，返回新特权模式VU时，MDT位被设置为0，sstatus.SDT=0，vsstatus.SDT=0",
        check1 &&
        check2
    ); 
    trap = 0;
}


bool m_double_trap_12() {

    //当执行MRET指令，返回新特权模式VU时，MDT位被设置为0
    TEST_START();

    goto_priv(PRIV_M);
    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_STVEC , 1ULL << 0);

    TEST_SETUP_EXCEPT();
    printf("%llx\n",CSRR(CSR_MEDELEG));

    goto_priv(PRIV_VS);    

    trap=4;
    TEST_SETUP_EXCEPT();

    CSRW(CSR_MCONFIGPTR,0xfff); 

    bool check1,check2,check3;

    if(trap == -1) check1 = true;
    else check1 = false;


    goto_priv(PRIV_M);

    check3 = ((CSRR(CSR_VSSTATUS) & VSSTATUS_SDT) == 0);
    check2 = ((CSRR(CSR_SSTATUS) & SSTATUS_SDT) == 0);
    check3 = ((CSRR(CSR_MSTATUS) & MSTATUS_MDT) == 0);

    TEST_ASSERT("当执行MRET指令，返回新特权模式VU时，MDT位被设置为0，sstatus.SDT=0，vsstatus.SDT=0",    //需要改对应.S文件
        check1 &&
        check2
    ); 
    trap = 0;
}



bool m_double_trap_13() {   

    //m模式下，当mtvec.mode=vectored，未开启中断代理，发生2号软件中断，再产生异常，产生double trap  
    TEST_START();

    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS, MSTATUS_MIE);
    CSRS(CSR_MSTATUS, MSTATUS_SIE);

    printf("mstatus=%llx\n",CSRR(CSR_MSTATUS));

    CSRS(mie, 1ULL << 1);

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);
    printf("mtvec=%llx \n",CSRR(CSR_MTVEC));

    TEST_SETUP_EXCEPT();
    printf("入口：");
    trap = 2;
    double_trap_enabled = true;    //用来控制handler中的处理
    CSRS(mip, 1ULL << 1);

    TEST_ASSERT("m模式下,mtvec.mode=vectored.未开启中断代理,产生2号软件中断处理过程中产生一个非法指令异常",
        excpt.triggered == true 
    ); 
}

bool m_double_trap_14() {

    //m模式下，当mtvec.mode=vectored，未开启中断代理，发生2号软件中断，再产生其他类型中断，产生double trap  
    TEST_START();

    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS, MSTATUS_MIE);
    CSRS(CSR_MSTATUS, MSTATUS_SIE);

    printf("mstatus=%llx\n",CSRR(CSR_MSTATUS));

    CSRS(mie, 1ULL << 1);

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);
    printf("mtvec=%llx \n",CSRR(CSR_MTVEC));

    TEST_SETUP_EXCEPT();
    printf("入口：");
    trap = 1;
    double_trap_enabled = true;    //用来控制handler中的处理
    CSRS(mip, 1ULL << 1);

    TEST_ASSERT("m模式下,mtvec.mode=vectored.未开启中断代理,产生2号软件中断处理过程中产生一个其他类型中断",
        excpt.triggered == true 
    ); 
}

bool m_double_trap_15() {

    //m模式下，当mtvec.mode=vectored，未开启异常代理，发生异常，再产生中断，产生double trap  
    TEST_START();

    goto_priv(PRIV_M);

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);
    printf("mtvec=%llx \n",CSRR(CSR_MTVEC));

    TEST_SETUP_EXCEPT();
    printf("入口：");
    trap = 1;
    double_trap_enabled = true;    //用来控制handler中的处理
    CSRW(CSR_MCONFIGPTR,0xfff);

    TEST_ASSERT("m模式下,mtvec.mode=vectored.未开启异常代理,产生一个非法指令异常处理过程中产生2号软件中断",
        excpt.triggered == true 
    ); 
}

bool m_double_trap_16() {

    //m模式下，当mtvec.mode=vectored，未开启异常代理，发生异常，再产生异常，产生double trap  
    TEST_START();

    goto_priv(PRIV_M);

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);
    printf("mtvec=%llx \n",CSRR(CSR_MTVEC));

    TEST_SETUP_EXCEPT();
    printf("入口：");
    trap = 2;
    double_trap_enabled = true;    //用来控制handler中的处理
    CSRW(CSR_MCONFIGPTR,0xfff);

    TEST_ASSERT("m模式下,当mtvec.mode=vectored,未开启异常代理,发生异常,再产生异常,产生double trap",
        excpt.triggered == true 
    ); 
}

bool m_double_trap_17() {

    //当发生陷阱异常需进入 M-mode 时，若当前 MDT = 1 ,则视为 意外陷阱（Unexpected Trap），触发双陷阱处理机制。
    TEST_START();

    goto_priv(PRIV_M);

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);
    CSRS(CSR_MSTATUS , MSTATUS_MDT);
    printf("mtvec=%llx \n",CSRR(CSR_MTVEC));

    TEST_SETUP_EXCEPT();
    printf("入口：");
    CSRW(CSR_MCONFIGPTR,0xfff);

    TEST_ASSERT("当发生异常需进入 M-mode 时，若当前 MDT = 1 ,结果发生unexpected trap",
        excpt.triggered == true 
    ); 
}


bool m_double_trap_18() {

    //当发生陷阱中断需进入 M-mode 时，若当前 MDT = 1 ,则视为 意外陷阱（Unexpected Trap），触发双陷阱处理机制。
    TEST_START();

    goto_priv(PRIV_M);

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);
    CSRS(CSR_MSTATUS, MSTATUS_MIE);
    CSRS(CSR_MSTATUS, MSTATUS_SIE);
    CSRS(CSR_MSTATUS , MSTATUS_MDT);

    CSRS(mie, 1ULL << 1);
    printf("mtvec=%llx \n",CSRR(CSR_MTVEC));

    TEST_SETUP_EXCEPT();
    printf("入口：");
    CSRS(mip, 1ULL << 1);

    TEST_ASSERT("当发生中断需进入 M-mode 时，若当前 MDT = 1 ,结果发生unexpected trap",
        excpt.triggered == true 
    ); 
}


bool m_double_trap_19() {

    //实现smrnmi扩展(才有RNMI)，若在 M-mode 中执行且 mnstatus.NMIE = 0，任何陷阱均视为unexpected trap
    TEST_START();

    goto_priv(PRIV_M);

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);
    CSRS(CSR_MSTATUS, MSTATUS_MIE);
    CSRS(CSR_MSTATUS, MSTATUS_SIE);
    CSRC(CSR_MNSTATUS , MNSTATUS_NMIE);
    CSRS(mie, 1ULL << 1);
    printf("mtvec=%llx \n",CSRR(CSR_MTVEC));

    TEST_SETUP_EXCEPT();
    printf("入口：");
    CSRS(mip, 1ULL << 1);

    TEST_ASSERT("实现smrnmi扩展,若在 M-mode 中执行且 mnstatus.NMIE = 0,中断视为unexpected trap",
        excpt.triggered == true 
    ); 
}

bool m_double_trap_20() {

    //实现smrnmi扩展(才有RNMI)，若在 M-mode 中执行且 mnstatus.NMIE = 0，任何陷阱均视为unexpected trap
    TEST_START();

    goto_priv(PRIV_M);

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);
    CSRC(CSR_MNSTATUS , MNSTATUS_NMIE);
    printf("mtvec=%llx \n",CSRR(CSR_MTVEC));

    TEST_SETUP_EXCEPT();
    printf("入口：");
    CSRW(CSR_MCONFIGPTR,0xfff);

    TEST_ASSERT("实现smrnmi扩展,若在 M-mode 中执行且 mnstatus.NMIE = 0,异常视为unexpected trap",
        excpt.triggered == true 
    ); 
}

bool m_double_trap_21() {

    //实现 Smrnmi 扩展且 mnstatus.NMIE = 1,发生 unexpected trap,硬件跳转到 RNMI 处理程序
    TEST_START();

    goto_priv(PRIV_M);

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);
    CSRS(CSR_MSTATUS , MSTATUS_MDT);
    CSRS(CSR_MNSTATUS , MNSTATUS_NMIE);
    printf("mtvec=%llx \n",CSRR(CSR_MTVEC));

    TEST_SETUP_EXCEPT();
    printf("入口：");
    CSRW(CSR_MCONFIGPTR,0xfff);

    TEST_ASSERT("实现 Smrnmi 扩展且 mnstatus.NMIE = 1,发生 unexpected trap,硬件跳转到 RNMI 处理程序",
        excpt.triggered == true 
    ); 
}






bool m_double_trap_122() {    

    TEST_START();

    //HS模式下，当mtvec.mode=vectored，未开启中断代理，发生中断后发生double trap  

    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS,1ULL << 3);
    CSRS(CSR_MSTATUS,1ULL << 1);
    CSRS(CSR_SSTATUS,1ULL << 1);
    CSRC(CSR_MIDELEG,1ULL << 1);
    printf("mstatus=%llx\n",CSRR(CSR_MSTATUS));

    CSRS(mie, 1ULL << 1);

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);
    printf("mtvec=%llx \n",CSRR(CSR_MTVEC));

    TEST_SETUP_EXCEPT();
    goto_priv(PRIV_HS);
    printf("入口：");
    trap = 2;
    double_trap_enabled = true;    //用来控制handler中的处理

    CSRS(sip, 1ULL << 1);
    
    TEST_ASSERT("HS模式下,mtvec.mode=vectored,未开启中断代理,产生2号软件中断,发生double trap",
        excpt.triggered == true 
    ); 
}


//-------------------------------smdbltrp_test_end-------------------------------------

//-------------------------------ssdbltrp_test_begin-------------------------------------


bool s_double_trap_1() {

    //显式CSR写 MDT 位为1时，MIE（机器中断使能）位被清除为0
    TEST_START();

    goto_priv(PRIV_M);

    CSRS(CSR_MSTATUS, MSTATUS_SIE);

    CSRS(CSR_SSTATUS, SSTATUS_SDT);
    TEST_ASSERT("显式CSR写 SDT 位为1时,SIE(监管模式中断使能)位被清除为0",
        (CSRR(CSR_MSTATUS) & MSTATUS_SIE) == 0
    ); 

}

bool s_double_trap_2() {

    //当 SDT 位已为0,通过显式的CSR写操作将 SIE 位设置为1
    TEST_START();

    goto_priv(PRIV_M);

    CSRS(CSR_MSTATUS, MSTATUS_SIE);

    TEST_ASSERT("当 SDT 位已为0,通过显式的CSR写操作将 SIE 位设置为1",
        (CSRR(CSR_MSTATUS) & MSTATUS_SIE) == 1
    ); 

}

bool s_double_trap_3() {

    //当 MDT 位（对于RV64）在同一次写操作中被设置为0时，通过显式的CSR写操作将 MIE 位设置为1
    TEST_START();

    goto_priv(PRIV_M);
    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);

    TEST_SETUP_EXCEPT();

    trap=5;
    CSRW(CSR_MCONFIGPTR,0xfff); //陷入异常使sdt为1

}

bool s_double_trap_4() {

    //当在HS模式下执行SRET指令时，SDT位被设置为0
    TEST_START();

    goto_priv(PRIV_M);
    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);

    printf("menvcfg=%llx\n",CSRR(CSR_MENVCFG));
    TEST_SETUP_EXCEPT();

    trap=4;
    goto_priv(PRIV_HS);
    CSRW(CSR_MCONFIGPTR,0xfff); 

    bool check1,check2;

    if(trap == -1) check1 = true;
    else check1 = false;

    check2 = ((CSRR(CSR_SSTATUS) & SSTATUS_SDT) == 0);
    printf("%d\n",check1);
    printf("%d\n",check2);


    TEST_ASSERT("当在HS模式下执行SRET指令时,SDT位被设置为0",
        check1 &&
        check2
    ); 
    trap = 0;
}

bool s_double_trap_5() {   

    //S/U模式下发生中断（非NMI），不开代理，trap到M模式，再发生异常，MDT/SDT=1，使用RNMI处理程序，执行MNRET指令
    TEST_START();

    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS, MSTATUS_MIE);
    CSRS(CSR_MSTATUS, MSTATUS_SIE);


    CSRS(mie, 1ULL << 1);

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);

    TEST_SETUP_EXCEPT();
    printf("入口：");
    trap = 2;
    double_trap_enabled = true;    //用来控制handler中的处理

    goto_priv(PRIV_HS);
    CSRS(sip, 1ULL << 1);

    TEST_ASSERT("s模式下,mtvec.mode=vectored.未开启中断代理,产生2号supervisor软件中断处理过程中产生一个非法指令异常",
        excpt.triggered == true 
    ); 
}

bool s_double_trap_7() {   

    //S/U模式下发生中断（非NMI），不开代理，trap到M模式，发生M模式中断
    TEST_START();

    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS, MSTATUS_MIE);
    CSRS(CSR_MSTATUS, MSTATUS_SIE);


    CSRS(mie, 1ULL << 1);

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);

    TEST_SETUP_EXCEPT();
    printf("入口：");
    trap = 1;
    double_trap_enabled = true;    //用来控制handler中的处理

    goto_priv(PRIV_HS);
    CSRS(sip, 1ULL << 1);

    TEST_ASSERT("S/U模式下发生2号supervisor软件中断(非NMI),不开代理,trap到M模式,发生M模式中断",
        excpt.triggered == true 
    ); 
}


bool s_double_trap_8() {

    //s/u模式下，当mtvec.mode=vectored，未开启异常代理，发生异常，再产生中断，产生double trap  
    TEST_START();

    goto_priv(PRIV_M);

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);
    printf("mtvec=%llx \n",CSRR(CSR_MTVEC));

    TEST_SETUP_EXCEPT();
    printf("入口：");
    trap = 1;
    double_trap_enabled = true;    //用来控制handler中的处理
    goto_priv(PRIV_HS);
    CSRW(CSR_MCONFIGPTR,0xfff);

    TEST_ASSERT("s模式下,mtvec.mode=vectored.未开启异常代理,产生一个非法指令异常处理过程中产生2号软件中断",
        excpt.triggered == true 
    ); 
}

bool s_double_trap_9() {

    //u模式下，当mtvec.mode=vectored，未开启异常代理，发生异常，再产生中断，产生double trap  
    TEST_START();

    goto_priv(PRIV_M);

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);
    printf("mtvec=%llx \n",CSRR(CSR_MTVEC));

    TEST_SETUP_EXCEPT();
    printf("入口：");
    trap = 1;
    double_trap_enabled = true;    //用来控制handler中的处理
    goto_priv(PRIV_HU);
    CSRW(CSR_MCONFIGPTR,0xfff);

    TEST_ASSERT("u模式下,mtvec.mode=vectored.未开启异常代理,产生一个非法指令异常处理过程中产生2号软件中断",
        excpt.triggered == true 
    ); 
}

bool s_double_trap_10() {

    //S模式下发生异常，不开代理，trap到M模式，在进入陷阱处理的早期阶段发生异常
    TEST_START();

    goto_priv(PRIV_M);

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);
    printf("mtvec=%llx \n",CSRR(CSR_MTVEC));

    TEST_SETUP_EXCEPT();
    printf("入口：");
    trap = 2;
    double_trap_enabled = true;    //用来控制handler中的处理
    goto_priv(PRIV_HS);
    CSRW(CSR_MCONFIGPTR,0xfff);

    TEST_ASSERT("s模式下,当mtvec.mode=vectored,未开启异常代理,发生异常,再产生异常,产生double trap",
        excpt.triggered == true 
    ); 
}

bool s_double_trap_11() {

    //U模式下发生异常，不开代理，trap到M模式，在进入陷阱处理的早期阶段发生异常
    TEST_START();

    goto_priv(PRIV_M);

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);
    printf("mtvec=%llx \n",CSRR(CSR_MTVEC));

    TEST_SETUP_EXCEPT();
    printf("入口：");
    trap = 2;
    double_trap_enabled = true;    //用来控制handler中的处理
    goto_priv(PRIV_HU);
    CSRW(CSR_MCONFIGPTR,0xfff);

    TEST_ASSERT("u模式下,当mtvec.mode=vectored,未开启异常代理,发生异常,再产生异常,产生double trap",
        excpt.triggered == true 
    ); 
}

bool s_double_trap_66() {       //nemu和spike不一致，可测

    //m模式下，当mtvec.mode=vectored，未开启中断代理，发生2号软件中断，再产生异常，产生double trap  
    TEST_START();

    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS, MSTATUS_MIE);
    CSRS(CSR_MSTATUS, MSTATUS_SIE);

    printf("mstatus=%llx\n",CSRR(CSR_MSTATUS));

    CSRS(mie, 1ULL << 1);

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);
    printf("mtvec=%llx \n",CSRR(CSR_MTVEC));

    TEST_SETUP_EXCEPT();
    printf("入口：");
    trap = 2;
    CSRS(mip, 1ULL << 1);

    TEST_ASSERT("m模式下,mtvec.mode=vectored.未开启中断代理,产生2号软件中断处理过程中产生一个非法指令异常",
        excpt.triggered == true 
    ); 
}


bool s_double_trap_12() {   

    //S/U模式下发生中断（非NMI），开代理，trap到HS模式，SDT=1，在进入陷阱处理的早期阶段发生异常
    TEST_START();

    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS, MSTATUS_MIE);
    CSRS(CSR_MSTATUS, MSTATUS_SIE);
    CSRS(CSR_SSTATUS,1ULL << 1);
    CSRS(CSR_MIDELEG,1ULL << 1);
    CSRS(CSR_HIDELEG,1ULL << 1);

    CSRS(mie, 1ULL << 1);

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);

    TEST_SETUP_EXCEPT();
    printf("入口：");
    trap = 2;
    double_trap_enabled = true;    //用来控制handler中的处理

    goto_priv(PRIV_HS);
    CSRS(sip, 1ULL << 1);

    TEST_ASSERT("s模式下,mtvec.mode=vectored.未开启中断代理,产生2号supervisor软件中断处理过程中产生一个非法指令异常",
        excpt.triggered == true 
    ); 
}

bool s_double_trap_13() {   

    //S模式下发生异常，开代理，trap到HS模式，SDT=1，再发生中断非NMI
    TEST_START();

    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS, MSTATUS_MIE);
    CSRS(CSR_MSTATUS, MSTATUS_SIE);
    CSRS(CSR_SSTATUS,1ULL << 1);
    CSRS(CSR_MEDELEG,1ULL << 2);
    CSRC(CSR_HEDELEG,1ULL << 2);

    CSRS(mie, 1ULL << 1);

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);

    printf("入口：");
    trap = 1;
    double_trap_enabled = true;    //用来控制handler中的处理

    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();
    CSRW(CSR_MCONFIGPTR,0xfff);

    TEST_ASSERT("S模式下发生异常,开代理,trap到HS模式,SDT=1,再发生中断非NMI",
        excpt.triggered == true 
    ); 
}

bool s_double_trap_14() {   

    //U模式下发生异常，开代理，trap到HS模式，SDT=1，再发生中断非NMI
    TEST_START();

    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS, MSTATUS_MIE);
    CSRS(CSR_MSTATUS, MSTATUS_SIE);
    CSRS(CSR_SSTATUS,1ULL << 1);
    CSRS(CSR_MEDELEG,1ULL << 2);
    CSRC(CSR_HEDELEG,1ULL << 2);

    CSRS(mie, 1ULL << 1);

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);

    printf("入口：");
    trap = 1;
    double_trap_enabled = true;    //用来控制handler中的处理

    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();
    CSRW(CSR_MCONFIGPTR,0xfff);

    TEST_ASSERT("U模式下发生异常,开代理,trap到HS模式,SDT=1,再发生中断非NMI",
        excpt.triggered == true 
    ); 
}


bool s_double_trap_15() {   

    //S模式下发生异常，开代理，trap到HS模式，SDT=1，再发生异常
    TEST_START();

    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS, MSTATUS_MIE);
    CSRS(CSR_MSTATUS, MSTATUS_SIE);
    CSRS(CSR_SSTATUS,1ULL << 1);
    CSRS(CSR_MEDELEG,1ULL << 2);
    CSRC(CSR_HEDELEG,1ULL << 2);

    CSRS(mie, 1ULL << 1);

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);

    printf("入口：");
    trap = 5;
    double_trap_enabled = true;    //用来控制handler中的处理

    goto_priv(PRIV_HS);
    TEST_SETUP_EXCEPT();
    CSRW(CSR_MCONFIGPTR,0xfff);

    TEST_ASSERT("S模式下发生异常,开代理,trap到HS模式,SDT=1,再发生异常",
        excpt.triggered == true 
    ); 
}

bool s_double_trap_16() {   

    //U模式下发生异常，开代理，trap到HS模式，SDT=1，再发生异常
    TEST_START();

    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS, MSTATUS_MIE);
    CSRS(CSR_MSTATUS, MSTATUS_SIE);
    CSRS(CSR_SSTATUS,1ULL << 1);
    CSRS(CSR_MEDELEG,1ULL << 2);
    CSRC(CSR_HEDELEG,1ULL << 2);

    CSRS(mie, 1ULL << 1);

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);

    printf("入口：");
    trap = 2;
    double_trap_enabled = true;    //用来控制handler中的处理

    goto_priv(PRIV_HU);
    TEST_SETUP_EXCEPT();
    CSRW(CSR_MCONFIGPTR,0xfff);

    TEST_ASSERT("U模式下发生异常,开代理,trap到HS模式,SDT=1,再发生异常",
        excpt.triggered == true 
    ); 
}


bool s_double_trap_17() {   

    //VS模式下发生异常，开代理，trap到HS模式，在进入陷阱处理阶段发生异常
    TEST_START();

    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS, MSTATUS_MIE);
    CSRS(CSR_MSTATUS, MSTATUS_SIE);
    CSRS(CSR_SSTATUS,1ULL << 1);
    CSRS(CSR_MEDELEG,1ULL << 2);
    CSRC(CSR_HEDELEG,1ULL << 2);

    CSRS(mie, 1ULL << 1);

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);

    printf("入口：");
    trap = 2;
    double_trap_enabled = true;    //用来控制handler中的处理

    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    CSRW(CSR_MCONFIGPTR,0xfff);

    TEST_ASSERT("VS模式下发生异常,开代理,trap到HS模式,在进入陷阱处理阶段发生异常",
        excpt.triggered == true 
    ); 
}

bool s_double_trap_18() {   

    //VU模式下发生异常，开代理，trap到HS模式，在进入陷阱处理阶段发生异常
    TEST_START();

    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS, MSTATUS_MIE);
    CSRS(CSR_MSTATUS, MSTATUS_SIE);
    CSRS(CSR_SSTATUS,1ULL << 1);
    CSRS(CSR_MEDELEG,1ULL << 2);
    CSRC(CSR_HEDELEG,1ULL << 2);

    CSRS(mie, 1ULL << 1);

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);

    printf("入口：");
    trap = 2;
    double_trap_enabled = true;    //用来控制handler中的处理

    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    CSRW(CSR_MCONFIGPTR,0xfff);

    TEST_ASSERT("VU模式下发生异常,开代理,trap到HS模式,在进入陷阱处理阶段发生异常",
        excpt.triggered == true 
    ); 
}


bool s_double_trap_19() {   

    //VS模式下发生异常，开代理，trap到VS模式，在进入陷阱处理阶段发生异常
    TEST_START();

    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS, MSTATUS_MIE);
    CSRS(CSR_MSTATUS, MSTATUS_SIE);
    CSRS(CSR_SSTATUS,1ULL << 1);
    CSRS(CSR_MEDELEG,1ULL << 2);
    CSRC(CSR_HEDELEG,1ULL << 2);

    CSRS(mie, 1ULL << 1);

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);

    printf("入口：");
    trap = 2;
    double_trap_enabled = true;    //用来控制handler中的处理

    goto_priv(PRIV_VS);
    TEST_SETUP_EXCEPT();
    CSRW(CSR_MCONFIGPTR,0xfff);

    TEST_ASSERT("VS模式下发生异常,开代理,trap到HS模式,在进入陷阱处理阶段发生异常",
        excpt.triggered == true 
    ); 
}

bool s_double_trap_20() {   

    //VU模式下发生异常，开代理，trap到VS模式，在进入陷阱处理阶段发生异常
    TEST_START();

    goto_priv(PRIV_M);
    CSRS(CSR_MSTATUS, MSTATUS_MIE);
    CSRS(CSR_MSTATUS, MSTATUS_SIE);
    CSRS(CSR_SSTATUS,1ULL << 1);
    CSRS(CSR_MEDELEG,1ULL << 2);
    CSRC(CSR_HEDELEG,1ULL << 2);

    CSRS(mie, 1ULL << 1);

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRS(CSR_MTVEC , 1ULL << 0);

    printf("入口：");
    trap = 2;
    double_trap_enabled = true;    //用来控制handler中的处理

    goto_priv(PRIV_VU);
    TEST_SETUP_EXCEPT();
    CSRW(CSR_MCONFIGPTR,0xfff);

    TEST_ASSERT("VU模式下发生异常,开代理,trap到HS模式,在进入陷阱处理阶段发生异常",
        excpt.triggered == true 
    ); 
}






bool rnnmi_handler() {    

    //mnstatus.NMIE is 1、mstatus.mdt=1时，trap到m模式
    TEST_START();
    goto_priv(PRIV_M);
    CSRS(CSR_MNSTATUS,1ULL << 3);
    CSRS(CSR_MSTATUS,1ULL << 42);

    CSRW(CSR_VSTVEC,vs_test_entry);
    CSRW(CSR_MTVEC,m_test_entry);
    CSRW(CSR_STVEC,hs_test_entry);
    printf("mnstatus.NMIE is 1、mstatus.mdt=1时,trap到m模式,预期进入rnnmi处理程序(检查mnepc和mncause)\n");
    printf("入口：");
    TEST_SETUP_EXCEPT();
    CSRW(CSR_MCONFIGPTR,0xfff);
    printf("mnepc=%llx\n",CSRR(CSR_MNEPC));
    printf("mncause=%llx\n",CSRR(CSR_MNCAUSE));

}

//-------------------------------ssdbltrp_test_end-------------------------------------


