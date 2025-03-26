#define LOAD ld
#define STORE sd
#define REGLEN (8)

.macro SAVE_CONTEXT

    add sp, sp, -(REGLEN*31)

    STORE   x1, 0*REGLEN(sp)
    STORE   x3, 2*REGLEN(sp)
    STORE   x4, 3*REGLEN(sp)
    STORE   x5, 4*REGLEN(sp)
    STORE   x6, 5*REGLEN(sp)
    STORE   x7, 6*REGLEN(sp)
    STORE   x8, 7*REGLEN(sp)
    STORE   x9, 8*REGLEN(sp)
    STORE   x10, 9*REGLEN(sp)
    STORE   x11, 10*REGLEN(sp)
    STORE   x12, 11*REGLEN(sp)
    STORE   x13, 12*REGLEN(sp)
    STORE   x14, 13*REGLEN(sp)
    STORE   x15, 14*REGLEN(sp)
    STORE   x16, 15*REGLEN(sp)
    STORE   x17, 16*REGLEN(sp)
    STORE   x18, 17*REGLEN(sp)
    STORE   x19, 18*REGLEN(sp)
    STORE   x20, 19*REGLEN(sp)
    STORE   x21, 20*REGLEN(sp)
    STORE   x22, 21*REGLEN(sp)
    STORE   x23, 22*REGLEN(sp)
    STORE   x24, 23*REGLEN(sp)
    STORE   x25, 24*REGLEN(sp)
    STORE   x26, 25*REGLEN(sp)
    STORE   x27, 26*REGLEN(sp)
    STORE   x28, 27*REGLEN(sp)
    STORE   x29, 28*REGLEN(sp)
    STORE   x30, 29*REGLEN(sp)
    STORE   x31, 29*REGLEN(sp)
.endm

.macro RESTORE_CONTEXT
    LOAD   x1, 0*REGLEN(sp)
    LOAD   x3, 2*REGLEN(sp)
    LOAD   x4, 3*REGLEN(sp)
    LOAD   x5, 4*REGLEN(sp)
    LOAD   x6, 5*REGLEN(sp)
    LOAD   x7, 6*REGLEN(sp)
    LOAD   x8, 7*REGLEN(sp)
    LOAD   x9, 8*REGLEN(sp)
    LOAD   x10, 9*REGLEN(sp)
    LOAD   x11, 10*REGLEN(sp)
    LOAD   x12, 11*REGLEN(sp)
    LOAD   x13, 12*REGLEN(sp)
    LOAD   x14, 13*REGLEN(sp)
    LOAD   x15, 14*REGLEN(sp)
    LOAD   x16, 15*REGLEN(sp)
    LOAD   x17, 16*REGLEN(sp)
    LOAD   x18, 17*REGLEN(sp)
    LOAD   x19, 18*REGLEN(sp)
    LOAD   x20, 19*REGLEN(sp)
    LOAD   x21, 20*REGLEN(sp)
    LOAD   x22, 21*REGLEN(sp)
    LOAD   x23, 22*REGLEN(sp)
    LOAD   x24, 23*REGLEN(sp)
    LOAD   x25, 24*REGLEN(sp)
    LOAD   x26, 25*REGLEN(sp)
    LOAD   x27, 26*REGLEN(sp)
    LOAD   x28, 27*REGLEN(sp)
    LOAD   x29, 28*REGLEN(sp)
    LOAD   x30, 29*REGLEN(sp) 
    LOAD   x31, 30*REGLEN(sp) 

    add sp, sp, (REGLEN*31) 
.endm

.balign 0x4
.global m_test_entry
m_test_entry:
    .balign 4
    j mhandler_0
    j mhandler_1
    j mhandler_2
    j mhandler_3
    j mhandler_4
    j mhandler_5
    j mhandler_6
    j mhandler_7
    j mhandler_8

mhandler_0:
    SAVE_CONTEXT
    jal m_test_entry_two_0
    j _mnreturn

mhandler_1:
    SAVE_CONTEXT
    jal m_test_entry_two_1
    j _mnreturn

mhandler_2:
    SAVE_CONTEXT
    jal m_test_entry_two_2
    j _mnreturn

mhandler_3:
    SAVE_CONTEXT
    jal m_test_entry_two_3
    j _mnreturn

mhandler_4:
    SAVE_CONTEXT
    jal m_test_entry_two_4
    j _mnreturn

mhandler_5:
    SAVE_CONTEXT
    jal m_test_entry_two_5
    j _mnreturn

mhandler_6:
    SAVE_CONTEXT
    jal m_test_entry_two_6
    j _mnreturn

mhandler_7:
    SAVE_CONTEXT
    jal m_test_entry_two_7
    j _mnreturn
    
mhandler_8:
    SAVE_CONTEXT
    jal m_test_entry_two_8
    j _mnreturn

.balign 0x4
.global hs_test_entry
hs_test_entry:
    .balign 4
    j hshandler_0
    j hshandler_1
    j hshandler_2
    j hshandler_3
    j hshandler_4
    j hshandler_5
    j hshandler_6
    j hshandler_7
    j hshandler_8

hshandler_0:
    SAVE_CONTEXT
    jal hs_test_entry_two_0
    j _return

hshandler_1:
    SAVE_CONTEXT
    jal hs_test_entry_two_1
    j _return

hshandler_2:
    SAVE_CONTEXT
    jal hs_test_entry_two_2
    j _return

hshandler_3:
    SAVE_CONTEXT
    jal hs_test_entry_two_3
    j _return

hshandler_4:
    SAVE_CONTEXT
    jal hs_test_entry_two_4
    j _return

hshandler_5:
    SAVE_CONTEXT
    jal hs_test_entry_two_5
    j _return

hshandler_6:
    SAVE_CONTEXT
    jal hs_test_entry_two_6
    j _return

hshandler_7:
    SAVE_CONTEXT
    jal hs_test_entry_two_7
    j _return
    
hshandler_8:
    SAVE_CONTEXT
    jal hs_test_entry_two_8
    j _return


.balign 0x4
.global vs_test_entry
vs_test_entry:
    .balign 4
    j vshandler_0
    j vshandler_1
    j vshandler_2
    j vshandler_3
    j vshandler_4
    j vshandler_5
    j vshandler_6
    j vshandler_7
    j vshandler_8

vshandler_0:
    SAVE_CONTEXT
    jal vs_test_entry_two_0
    j _return

vshandler_1:
    SAVE_CONTEXT
    jal vs_test_entry_two_1
    j _return

vshandler_2:
    SAVE_CONTEXT
    jal vs_test_entry_two_2
    j _return

vshandler_3:
    SAVE_CONTEXT
    jal vs_test_entry_two_3
    j _return

vshandler_4:
    SAVE_CONTEXT
    jal vs_test_entry_two_4
    j _return

vshandler_5:
    SAVE_CONTEXT
    jal vs_test_entry_two_5
    j _return

vshandler_6:
    SAVE_CONTEXT
    jal vs_test_entry_two_6
    j _return

vshandler_7:
    SAVE_CONTEXT
    jal vs_test_entry_two_7
    j _return
    
vshandler_8:
    SAVE_CONTEXT
    jal vs_test_entry_two_8
    j _return
    

_mnreturn:
    csrr    t1, 0x744
    srli    t1, t1, 3
    andi    t1, t1, 1       //读出mnstatus的第三位
    bnez    t1, _first_return
    RESTORE_CONTEXT
    .word   0x70200073      //mnret

_return:    //目前代理到hs和vs还没写好代码支持doubletrap的mnret返回
    j _first_return     


_first_return:
    li t0, 4 //make sure PRIV_M is 4
    beq a0, t0, _return_from_m 
_return_from_s:
    RESTORE_CONTEXT
    sret
    j   .
_return_from_m:
    RESTORE_CONTEXT
    mret

