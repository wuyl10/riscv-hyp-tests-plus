
#ifndef CSRS_H
#define CSRS_H

#include <util.h>

#define STVEC_MODE_OFF (0)
#define STVEC_MODE_LEN (2)
#define STVEC_MODE_MSK BIT_MASK(STVEC_MODE_OFF, STVEC_MODE_LEN)
#define STVEC_MODE_DIRECT (0)
#define STVEC_MODE_VECTRD (1)

#if (RV64)
#define SATP_MODE_OFF (60)
#define SATP_MODE_DFLT SATP_MODE_39
#define SATP_ASID_OFF (44)
#define SATP_ASID_LEN (16)
#define HGATP_VMID_OFF (44)
#define HGATP_VMID_LEN (14)
#elif (RV32)
#define SATP_MODE_OFF (31)
#define SATP_MODE_DFLT SATP_MODE_32
#define SATP_ASID_OFF (22)
#define SATP_ASID_LEN (9)
#define HGATP_VMID_OFF (22)
#define HGATP_VMID_LEN (7)
#endif


#define MSTATUS_SIE    (1ULL << 1)
#define MSTATUS_MIE    (1ULL << 3)
#define MSTATUS_MPRV_OFF    (17)
#define MSTATUS_MPRV    (1ULL << MSTATUS_MPRV_OFF)
#define MSTATUS_TW_OFF  (21)
#define MSTATUS_TW   (1ULL << MSTATUS_TW_OFF)
#define MSTATUS_GVA_OFF    (38)
#define MSTATUS_GVA    (1ULL << MSTATUS_GVA_OFF)
#define MSTATUS_TVM_OFF    (20)
#define MSTATUS_TVM     (1ULL << MSTATUS_TVM_OFF)
#define MSTATUS_MDT     (1ULL << 42)

#define SATP_MODE_BARE (0ULL << SATP_MODE_OFF)
#define SATP_MODE_32 (1ULL << SATP_MODE_OFF)
#define SATP_MODE_39 (8ULL << SATP_MODE_OFF)
#define SATP_MODE_48 (9ULL << SATP_MODE_OFF)
#define SATP_ASID_MSK BIT_MASK(SATP_ASID_OFF, SATP_ASID_LEN)

#define HGATP_MODE_OFF SATP_MODE_OFF
#define HGATP_MODE_DFLT SATP_MODE_DFLT
#define HGATP_VMID_MSK BIT_MASK(HGATP_VMID_OFF, HGATP_VMID_LEN)

#define SSTATUS_UIE_BIT (1ULL << 0)
#define SSTATUS_SIE_BIT (1ULL << 1)
#define SSTATUS_UPIE_BIT (1ULL << 4)
#define SSTATUS_SPIE_BIT (1ULL << 5)
#define SSTATUS_SPP_BIT (1ULL << 8)
#define SSTATUS_FS_OFF (13)
#define SSTATUS_FS_LEN (2)
#define SSTATUS_FS_MSK BIT_MASK(SSTATUS_FS_OFF, SSTATUS_FS_LEN)
#define SSTATUS_FS_AOFF (0)
#define SSTATUS_FS_INITIAL (1ULL << SSTATUS_FS_OFF)
#define SSTATUS_FS_CLEAN (2ULL << SSTATUS_FS_OFF)
#define SSTATUS_FS_DIRTY (3ULL << SSTATUS_FS_OFF)
#define SSTATUS_XS_OFF (15)
#define SSTATUS_XS_LEN (2)
#define SSTATUS_XS_MSK BIT_MASK(SSTATUS_XS_OFF, SSTATUS_XS_LEN)
#define SSTATUS_XS_AOFF (0)
#define SSTATUS_XS_INITIAL (1ULL << SSTATUS_XS_OFF)
#define SSTATUS_XS_CLEAN (2ULL << SSTATUS_XS_OFF)
#define SSTATUS_XS_DIRTY (3ULL << SSTATUS_XS_OFF)
#define MSTATUS_SUM SSTATUS_SUM
#define SSTATUS_SUM (1ULL << 18)
#define MSTATUS_MXR SSTATUS_MXR
#define SSTATUS_MXR (1ULL << 19)
#define SSTATUS_SDT (1ULL << 24)
#define SSTATUS_SD (1ULL << 63)

#define VSSTATUS_SDT (1ULL << 24)

#define MNSTATUS_NMIE (1ULL << 3)

#define SIE_USIE (1ULL << 0)
#define SIE_SSIE (1ULL << 1)
#define SIE_UTIE (1ULL << 4)
#define SIE_STIE (1ULL << 5)
#define SIE_UEIE (1ULL << 8)
#define SIE_SEIE (1ULL << 9)

#define SIP_USIP SIE_USIE
#define SIP_SSIP SIE_SSIE
#define SIP_UTIP SIE_UTIE
#define SIP_STIP SIE_STIE
#define SIP_UEIP SIE_UEIE
#define SIP_SEIP SIE_SEIE

#define HIE_VSSIE (1ULL << 2)
#define HIE_VSTIE (1ULL << 6)
#define HIE_VSEIE (1ULL << 10)
#define HIE_MEIE (1ULL << 11)
#define HIE_SGEIE (1ULL << 12)

#define MIE_MEIE (1ULL << 11)
#define MIP_MEIP (1ULL << 11)
#define MIE_MSIE (1ULL << 3)
#define MIE_SSIE (1ULL << 1)
#define MIP_SSIP (1ULL << 1)

#define HIP_VSSIP HIE_VSSIE
#define MIP_MSIP MIE_MSIE
#define HIP_VSTIP HIE_VSTIE
#define HIP_VSEIP HIE_VSEIE
#define HIP_SGEIP HIE_SGEIE
#define HIP_MEIP HIE_MEIE




/*
CAUSE_USI (0 | CAUSE_INT_BIT)：用户模式软件中断（User Software Interrupt）。
CAUSE_SSI (1 | CAUSE_INT_BIT)：监督模式软件中断（Supervisor Software Interrupt）。
CAUSE_VSSI (2 | CAUSE_INT_BIT)：虚拟监督模式软件中断（Virtual Supervisor Software Interrupt）。
CAUSE_UTI (4 | CAUSE_INT_BIT)：用户模式计时器中断（User Timer Interrupt）。
CAUSE_STI (5 | CAUSE_INT_BIT)：监督模式计时器中断（Supervisor Timer Interrupt）。
CAUSE_VSTI (6 | CAUSE_INT_BIT)：虚拟监督模式计时器中断（Virtual Supervisor Timer Interrupt）。
CAUSE_UEI (8 | CAUSE_INT_BIT)：用户模式外部中断（User External Interrupt）。
CAUSE_SEI (9 | CAUSE_INT_BIT)：监督模式外部中断（Supervisor External Interrupt）。
CAUSE_VSEI (10 | CAUSE_INT_BIT)：虚拟监督模式外部中断（Virtual Supervisor External Interrupt）。
CAUSE_MEI (11 | CAUSE_INT_BIT)：机器模式外部中断（Machine External Interrupt）。
*/
#define CAUSE_INT_BIT (1ULL << 63)
#define CAUSE_MSK (CAUSE_INT_BIT - 1)
#define CAUSE_USI (0 | CAUSE_INT_BIT)
#define CAUSE_SSI (1 | CAUSE_INT_BIT)
#define CAUSE_VSSI (2 | CAUSE_INT_BIT)
#define CAUSE_MSI (3 | CAUSE_INT_BIT)
#define CAUSE_UTI (4 | CAUSE_INT_BIT)
#define CAUSE_STI (5 | CAUSE_INT_BIT)
#define CAUSE_VSTI (6 | CAUSE_INT_BIT)
#define CAUSE_UEI (8 | CAUSE_INT_BIT)
#define CAUSE_SEI (9 | CAUSE_INT_BIT)
#define CAUSE_VSEI (10 | CAUSE_INT_BIT)
#define CAUSE_MEI (11 | CAUSE_INT_BIT)


/*
CAUSE_IAM (0): 指令地址不对齐（Instruction Address Misaligned）
CAUSE_IAF (1): 指令访问故障（Instruction Access Fault）
CAUSE_ILI (2): 非法指令（Illegal Instruction）
CAUSE_BKP (3): 断点（Breakpoint）
CAUSE_LAM (4): 载入地址不对齐（Load Address Misaligned）
CAUSE_LAF (5): 载入访问故障（Load Access Fault）
CAUSE_SAM (6): 存储地址不对齐（Store/AMO Address Misaligned）
CAUSE_SAF (7): 存储/原子操作访问故障（Store/AMO Access Fault）
CAUSE_ECU (8): 环境调用来自用户模式（Environment Call from U-mode）
CAUSE_ECS (9): 环境调用来自监管模式（Environment Call from S-mode）
CAUSE_ECV (10): 环境调用来自虚拟监管模式（Environment Call from VS-mode）
CAUSE_ECM (11): 环境调用来自机器模式（Environment Call from M-mode）
CAUSE_IPF (12): 指令页故障（Instruction Page Fault）
CAUSE_LPF (13): 载入页故障（Load Page Fault）
CAUSE_SPF (15): 存储页故障（Store/AMO Page Fault）
CAUSE_IGPF (20): 指令全局页故障（Instruction Guest-Page Fault）
CAUSE_LGPF (21): 载入全局页故障（Load Guest-Page Fault）
CAUSE_VRTI (22): 虚拟指令异常
CAUSE_SGPF (23): 存储全局页故障（Store/AMO Guest-Page Fault）
*/
#define CAUSE_IAM (0)
#define CAUSE_IAF (1)
#define CAUSE_ILI (2)
#define CAUSE_BKP (3)
#define CAUSE_LAM (4)
#define CAUSE_LAF (5)
#define CAUSE_SAM (6)
#define CAUSE_SAF (7)
#define CAUSE_ECU (8)
#define CAUSE_ECS (9)
#define CAUSE_ECV (10)
#define CAUSE_ECM (11)
#define CAUSE_IPF (12)
#define CAUSE_LPF (13)
#define CAUSE_SPF (15)
#define CAUSE_IGPF (20)
#define CAUSE_LGPF (21)
#define CAUSE_VRTI (22)
#define CAUSE_SGPF (23)

#define HIDELEG_USI SIP_USIP
#define HIDELEG_SSI SIP_SSIP
#define HIDELEG_UTI SIP_UTIP
#define HIDELEG_STI SIP_STIP
#define HIDELEG_UEI SIP_UEIP
#define HIDELEG_SEI SIP_SEIP
#define HIDELEG_VSSI HIP_VSSIP
#define HIDELEG_MSI HIP_MSIP
#define HIDELEG_VSTI HIP_VSTIP
#define HIDELEG_VSEI HIP_VSEIP
#define HIDELEG_SGEI HIP_SGEIP
#define HIDELEG_MEI HIP_MEIP


#define HEDELEG_IAM (1ULL << 0)
#define HEDELEG_IAF (1ULL << 1)
#define HEDELEG_ILI (1ULL << 2)
#define HEDELEG_BKP (1ULL << 3)
#define HEDELEG_LAM (1ULL << 4)
#define HEDELEG_LAF (1ULL << 5)
#define HEDELEG_SAM (1ULL << 6)
#define HEDELEG_SAF (1ULL << 7)
#define HEDELEG_ECU (1ULL << 8)
#define HEDELEG_ECS (1ULL << 9)
#define HEDELEG_ECV (1ULL << 10)
#define HEDELEG_IPF (1ULL << 12)
#define HEDELEG_LPF (1ULL << 13)
#define HEDELEG_SPF (1ULL << 15)

#define MISA_H (1ULL << 7)

#define HSTATUS_VSBE (1ULL << 5)
#define HSTATUS_GVA_OFF (6)
#define HSTATUS_GVA (1ULL << HSTATUS_GVA_OFF)
#define HSTATUS_SPV (1ULL << 7)
#define HSTATUS_SPVP (1ULL << 8)
#define HSTATUS_HU (1ULL << 9)
#define HSTATUS_VTVM (1ULL << 20)
#define HSTATUS_VTW (1ULL << 21)
#define HSTATUS_VTSR (1ULL << 22)
#define HSTATUS_VSXL_OFF (32)
#define HSTATUS_VSXL_LEN (2)
#define HSTATUS_VSXL_MSK (BIT_MASK(HSTATUS_VSXL_OFF, HSTATUS_VSXL_LEN))
#define HSTATUS_VSXL_32 (1ULL << HSTATUS_VSXL_OFF)
#define HSTATUS_VSXL_64 (2ULL << HSTATUS_VSXL_OFF)

#define HCOUNTEREN_CY   (1ULL << 0)
#define HCOUNTEREN_TM   (1ULL << 1)   
#define HCOUNTEREN_IR   (1ULL << 2)
#define HCOUNTEREN(N)   (1ULL << (N))

#define MENVCFG_STCE  (1ULL << 63)
#define MENVCFG_DTE  (1ULL << 59)

#define TINST_ADDROFF_OFF (15)
#define TINST_ADDROFF_LEN (5)
#define TINST_ADDROFF_MASK BIT_MASK(TINST_ADDROFF_OFF, TINST_ADDROFF_LEN)
#define TINST_ADDROFF TINST_ADDROFF_MASK

#define MSTATEEN_C (1ULL << 0)
#define MSTATEEN_FCSR (1ULL << 1)
#define MSTATEEN_JVT (1ULL << 2)
#define MSTATEEN_P1P13 (1ULL << 56)
#define MSTATEEN_CONTEXT (1ULL << 57)
#define MSTATEEN_IMSIC (1ULL << 58)
#define MSTATEEN_AIA (1ULL << 59)
#define MSTATEEN_CSRIND (1ULL << 60)
#define MSTATEEN_ENVCFG (1ULL << 62)
#define MSTATEEN_SE0 (1ULL << 63)

#endif /* __ARCH_CSRS_H__ */
