#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <util.h>

#define INS_OPCODE  BIT_MASK(0, 7)
#define INS_RD      BIT_MASK(7, 5)
#define INS_FUNCT3  BIT_MASK(12, 3)
#define INS_RS1     BIT_MASK(15, 5)
#define INS_RS2     BIT_MASK(20, 5)
#define INS_FUNCT7  BIT_MASK(25, 7)

#define INS_C_OPCODE        BIT_MASK(0, 2)  
#define INS_C_FUNCT3_OFF    (13)
#define INS_C_FUNCT3_LEN    (3)
#define INS_C_FUNCT3_MASK   BIT_MASK(INS_C_FUNCT3_OFF, INS_C_FUNCT3_LEN)
#define INS_C_FUNCT3        INS_C_FUNCT3_MASK
#define INS_C_RS1_OFF       (7)
#define INS_C_RS1_LEN       (3)
#define INS_C_RS1_MASK      BIT_MASK(INS_C_RS1_OFF, INS_C_RS1_LEN)
#define INS_C_RDRS2_OFF     (2)
#define INS_C_RDRS2_LEN     (3)
#define INS_C_RDRS2_MASK    BIT_MASK(INS_C_RDRS2_OFF, INS_C_RDRS2_LEN)
#define INS_C_IMM0_OFF      (5)
#define INS_C_IMM0_LEN      (2)
#define INS_C_IMM0_MASK     BIT_MASK(INS_C_IMM0_OFF, INS_C_IMM0_LEN)
#define INS_C_IMM1_OFF      (10)
#define INS_C_IMM1_LEN      (3)
#define INS_C_IMM1_MASK     BIT_MASK(INS_C_IMM1_OFF, INS_C_IMM1_LEN)

#define INS_COMPRESSED(ins) (((ins) & INS_C_OPCODE) != 0b11)

#ifndef __ASSEMBLER__

#define CSR_STR(s) _CSR_STR(s)
#define _CSR_STR(s) #s

#define CSRR(csr)                                     \
    ({                                                \
        uint64_t _temp;                               \
        asm volatile("csrr  %0, " CSR_STR(csr) "\n\r" \
                     : "=r"(_temp)::"memory");        \
        _temp;                                        \
    })

#define CSRW(csr, rs) \
    asm volatile("csrw  " CSR_STR(csr) ", %0\n\r" ::"rK"(rs) : "memory");

#define CSRS(csr, rs) \
    asm volatile("csrs  " CSR_STR(csr) ", %0\n\r" ::"rK"(rs) : "memory");

#define CSRC(csr, rs) \
    asm volatile("csrc  " CSR_STR(csr) ", %0\n\r" ::"rK"(rs) : "memory");

#define RDCYCLE()                                     \
    ({                                                \
        uint64_t _temp;                               \
        asm volatile("rdcycle  %0\n\r"                \
                     : "=r"(_temp)::"memory");        \
        _temp;                                        \
    })

#define RDTIME()                                     \
    ({                                                \
        uint64_t _temp;                               \
        asm volatile("rdtime  %0\n\r"                \
                     : "=r"(_temp)::"memory");        \
        _temp;                                        \
    })

#define RDINSTRET()                                     \
    ({                                                \
        uint64_t _temp;                               \
        asm volatile("rdinstret  %0\n\r"                \
                     : "=r"(_temp)::"memory");        \
        _temp;                                        \
    })


#define GET_PC()                                      \
    ({                                                \
        uint64_t _temp;                               \
        asm volatile(                                 \
            "auipc %0, 0\n\t"                         \
            "addi  %0, %0, 0\n\t"                     \
            : "=r"(_temp)                             \
            :: "memory");                             \
        _temp;                                        \
    })

// static inline void jalr_direct(uintptr_t addr) {
//     asm volatile(
//         "mv x1, %0\n\t" // 将虚拟地址存储到寄存器 x1
//         ".insn i 0x67, 0, x0, x1, 0\n\t"  // 使用 x1 中的地址生成 jalr 指令
//         : // 无输出操作数
//         : "r"(addr)  // 输入操作数：虚拟地址
//         : "x1", "memory"  // 通知编译器该内联汇编可能修改 x1 寄存器和内存
//     );
// }



static inline void sfence(){
    asm volatile ("sfence.vma \n\t");
}

static inline void sfence_vma(){
    asm volatile(
        ".insn r 0x73, 0x0, 0x09, x0, x0, x0\n\t"
        ::: "memory");
}

static inline void sfence_vmid(){
    
}

static inline void sfence_vma_vmid(){
    
}

static inline void sfence_va(){
    
}

static inline void Sinval_vma(){
    asm volatile(
        ".insn r 0x73, 0x0, 0x0B, x0, x0, x0\n\t"
        ::: "memory");
}

static inline void hinval_gvma(){
    asm volatile(
        ".insn r 0x73, 0x0, 0x33, x0, x0, x0\n\t"
        ::: "memory");
}


static inline void hinval_vvma(){
    asm volatile(
        ".insn r 0x73, 0x0, 0x13, x0, x0, x0\n\t"
        ::: "memory");
}

static inline void jal(uintptr_t vaddr) {
    // 使用内联汇编将传入的vaddr保存到寄存器x10，然后跳转到该地址
    asm volatile(
        "mv x10, %0\n\t"  // 将vaddr的值移动到x10寄存器
        "jalr x0, x10, 0\n\t"  // 从x10寄存器的地址跳转，偏移量为0
        :  // 输出列表（这里没有输出）
        : "r" (vaddr)  // 输入列表，将vaddr作为输入操作数
        : "x10", "memory");  // 破坏列表，x10寄存器和内存可能会被这条汇编指令修改
}


/*
0x73：操作码opcode
0x31： funct7字段，是一个7位的函数码，用于指定具体的RISC-V指令。
0x0： funct3字段，是一个3位的函数码。
x0, x0, x0：分别是rs1、rs2和rd寄存器参数，这里都是x0，表示使用寄存器x0（在RISC-V中通常用作临时寄存器）。
\n\t是换行符和制表符，用于格式化汇编代码。

        ::: "memory");
这是内联汇编的输出操作。:::表示没有输入操作数。
"memory"是一个clobber列表，告诉编译器这个汇编代码可能会修改所有的内存位置，因此编译器需要确保在这条汇编指令前后的内存操作不会和它重叠。
*/
static inline void hfence_gvma() {
    asm volatile(
        ".insn r 0x73, 0x0, 0x31, x0, x0, x0\n\t"
        ::: "memory");
}

static inline void hfence_vvma() {
    asm volatile(
        ".insn r 0x73, 0x0, 0x11, x0, x0, x0\n\t"
        ::: "memory");
}



// static inline void invalid_instruction() {
//     asm volatile(
//         ".insn r 0xff, 0x0, 0x11, x0, x0, x0\n\t"
//         ::: "memory");
// }

static inline void sret() {
    asm volatile(
        "sret\n\t"
        ::: "memory");
}

static inline void mret() {
    asm volatile(
        "mret\n\t"
        ::: "memory");
}

static inline void ecall_1() {
    asm volatile(
        "ecall\n\t"
        ::: "memory");
}


static inline void ebreak() {
    asm volatile ("ebreak");
}

static inline void hfence() {
    hfence_vvma();
    hfence_gvma();
}

static inline uint64_t hlvb(uintptr_t addr){
    uint64_t value;
    asm volatile(
        ".insn r 0x73, 0x4, 0x30, %0, %1, x0\n\t"
        : "=r"(value): "r"(addr) : "memory");
    return value;
}

static inline uint64_t hlvbu(uintptr_t addr){
    uint64_t value;
    asm volatile(
        ".insn r 0x73, 0x4, 0x30, %0, %1, x1\n\t"
        : "=r"(value): "r"(addr) : "memory");
    return value;
}


static inline uint64_t hlvh(uintptr_t addr){
    uint64_t value;
    asm volatile(
        ".insn r 0x73, 0x4, 0x32, %0, %1, x0\n\t"
        : "=r"(value): "r"(addr) : "memory");
    return value;
}

static inline uint64_t hlvhu(uintptr_t addr){
    uint64_t value;
    asm volatile(
        ".insn r 0x73, 0x4, 0x32, %0, %1, x1\n\t"
        : "=r"(value): "r"(addr) : "memory");
    return value;
}

static inline uint64_t hlvxhu(uintptr_t addr){
    uint64_t value;
    asm volatile(
        ".insn r 0x73, 0x4, 0x32, %0, %1, x3\n\t"
        : "=r"(value): "r"(addr) : "memory");
    return value;
}

static inline uint64_t hlvw(uintptr_t addr){
    uint64_t value;
    asm volatile(
        ".insn r 0x73, 0x4, 0x34, %0, %1, x0\n\t"
        : "=r"(value): "r"(addr) : "memory");
    return value;
}

static inline uint64_t hlvwu(uintptr_t addr){
    uint64_t value;
    asm volatile(
        ".insn r 0x73, 0x4, 0x34, %0, %1, x1\n\t"
        : "=r"(value): "r"(addr) : "memory");
    return value;
}

static inline uint64_t hlvxwu(uintptr_t addr){
    uint64_t value;
    asm volatile(
        ".insn r 0x73, 0x4, 0x34, %0, %1, x3\n\t"
        : "=r"(value): "r"(addr) : "memory");
    return value;
}

static inline uint64_t hlvd(uintptr_t addr){
    uint64_t value;
    asm volatile(
        ".insn r 0x73, 0x4, 0x36, %0, %1, x0\n\t"
        : "=r"(value): "r"(addr) : "memory");
    return value;
}

static inline uint64_t hsvb(uintptr_t addr, uint64_t value){
    asm volatile(
        ".insn r 0x73, 0x4, 0x31, x0, %1, %0\n\t"
        :: "r"(value), "r"(addr) : "memory");
    return value;
}

static inline uint64_t hsvh(uintptr_t addr, uint64_t value){
    asm volatile(
        ".insn r 0x73, 0x4, 0x33, x0, %1, %0\n\t"
        :: "r"(value), "r"(addr): "memory");
    return value;
}

static inline uint64_t hsvw(uintptr_t addr, uint64_t value){
    asm volatile(
        ".insn r 0x73, 0x4, 0x35, x0, %1, %0\n\t"
        :: "r"(value), "r"(addr): "memory");
    return value;
}

static inline uint64_t hsvd(uintptr_t addr, uint64_t value){
    asm volatile(
        ".insn r 0x73, 0x4, 0x37, x0, %1, %0\n\t"
        :: "r"(value), "r"(addr) : "memory");
    return value;
}

static inline void wfi() {
    asm ("wfi" ::: "memory");
}

#define LOAD_INSTRUCTION(name, instruction, type) \
    static inline type name(uintptr_t addr){ \
        type value; \
        asm volatile( \
            ".option push\n\t" \
            ".option norvc\n\t" \
            instruction " %0, 0(%1)\n\t" \
            ".option pop\n\t" \
            : "=r"(value) : "r"(addr) : "memory"); \
        return value; \
    }

LOAD_INSTRUCTION(lb, "lb", uint8_t);
LOAD_INSTRUCTION(lbu, "lbu", uint8_t);
LOAD_INSTRUCTION(lh, "lh", uint16_t);
LOAD_INSTRUCTION(lhu, "lhu", uint16_t); 
LOAD_INSTRUCTION(lw, "lw", uint32_t); 
LOAD_INSTRUCTION(lwu, "lwu", uint32_t);
LOAD_INSTRUCTION(ld, "ld", uint64_t);




#define STORE_INSTRUCTION(name, instruction, type) \
    static inline void name(uintptr_t addr, type value){ \
        asm volatile( \
            ".option push\n\t" \
            ".option norvc\n\t" \
            instruction " %0, 0(%1)\n\t" \
            ".option pop\n\t" \
            :: "r"(value), "r"(addr):  "memory" \
        ); \
    }

STORE_INSTRUCTION(sb, "sb", uint8_t);
STORE_INSTRUCTION(sh, "sh", uint16_t);
STORE_INSTRUCTION(sw, "sw", uint32_t);
STORE_INSTRUCTION(sd, "sd", uint64_t);

/**
 * For compressed instructions there is no constraint to guarantee
 * the asm extended chosen registers are part of the subset allowed
 * so we have to make sure.
 */

static inline uint64_t c_lw(uintptr_t addr){
    register uint64_t _addr asm("a0") = addr;
    register uint64_t value asm("a1");
    asm volatile(
        "c.lw    %0, 0(%1)\n\t"
        :"=r"(value): "r"(_addr) : "memory"
    );
    return value;
}

static inline uint64_t c_ld(uintptr_t addr){
    register uint64_t _addr asm("a0") = addr;
    register uint64_t value asm("a1");
    asm volatile(
        "c.ld    %0, 0(%1)\n\t"
        :"=r"(value): "r"(_addr) : "memory"
    );
    return value;
}

static inline void c_sw(uintptr_t addr, uint64_t value){
    register uint64_t _addr asm("a0") = addr;
    register uint64_t _value asm("a1") = value;
    asm volatile(
        "c.sw    %0, 0(%1)\n\t"
        :: "r"(_value), "r"(_addr): "memory"
    );
}

static inline void c_sd(uintptr_t addr, uint64_t value){
    register uint64_t _addr asm("a0") = addr;
    register uint64_t _value asm("a1") = value;
    asm volatile(
        "c.sd    %0, 0(%1)\n\t"
        :: "r"(_value), "r"(_addr): "memory"
    );
}

static inline uint32_t lr_w(uintptr_t addr){
    uint32_t value;
    asm volatile(
        "lr.w %0, 0(%1)\n\t"
        : "=r"(value) : "r"(addr): "memory"
    );
    return value;
}

static inline uint32_t sc_w(uintptr_t addr, uint64_t value){
    asm volatile(
        "sc.w    %0, %0, 0(%1)\n\t"
        : "+r"(value) : "r"(addr): "memory"
    );
    return value;
}

#define AMO_INSTRUCTION(name, instruction, type) \
    static inline type name(uintptr_t addr, type value){ \
        asm volatile( \
            instruction " %0, %0, 0(%1)\n\t" \
            : "+r"(value) : "r"(addr): "memory" \
        ); \
        return value; \
    }

AMO_INSTRUCTION(amoswap_w, "amoswap.w", uint32_t);
AMO_INSTRUCTION(amoadd_w, "amoadd.w", uint32_t);
AMO_INSTRUCTION(amoxor_w, "amoxor.w", uint32_t);
AMO_INSTRUCTION(amoand_w, "amoand.w", uint32_t);
AMO_INSTRUCTION(amoor_w, "amoor.w", uint32_t);
AMO_INSTRUCTION(amomin_w, "amomin.w", uint32_t);
AMO_INSTRUCTION(amomax_w, "amomax.w", uint32_t);
AMO_INSTRUCTION(amominu_w, "amominu.w", uint32_t);
AMO_INSTRUCTION(amomaxu_w, "amomaxu.w", uint32_t);
AMO_INSTRUCTION(amoswap_d, "amoswap.d", uint64_t);
AMO_INSTRUCTION(amoadd_d, "amoadd.d", uint64_t);
AMO_INSTRUCTION(amoxor_d, "amoxor.d", uint64_t);
AMO_INSTRUCTION(amoand_d, "amoand.d", uint64_t);
AMO_INSTRUCTION(amoor_d, "amoor.d", uint64_t);
AMO_INSTRUCTION(amomin_d, "amomin.d", uint64_t);
AMO_INSTRUCTION(amomax_d, "amomax.d", uint64_t);
AMO_INSTRUCTION(amominu_d, "amominu.d", uint64_t);
AMO_INSTRUCTION(amomaxu_d, "amomaxu.d", uint64_t);


static inline void jmp(uintptr_t addr){
     asm volatile (
        "jalr x0, %0"
        : /* no output */
        : "r" (addr)
    );
}



static inline int* add_address(uintptr_t addr, int offset) {
    int* result;
    asm volatile (
        "add %0, %1, %2"
        : "=r" (result)
        : "r" (addr), "r" (offset)
    );
    return result;
}



// --------------------- 分割线: 向量指令相关begin ---------------------


// -----------vector load and store(begin)------------

#define VTYPE(SEW, LMUL) ((SEW << 3) | LMUL)

// 定义向量加载宏，将内存中的向量数据加载到寄存器 v6
// 参数说明：src代表要从src加载到v6寄存器，vl指定加载的向量长度为多少
#define LOAD_VECTOR_TO_REGISTER(name, instruction, type, sew_data) \
    static inline void name(const type *src, int vl) { \
        int sew = sew_data; \
        int vtype = VTYPE(sew, 1); \
        asm volatile( \
            ".option push\n\t" \
            ".option norvc\n\t" \
            "vsetvl t0, %1, %2\n\t" \
            instruction " v6, (%0)\n\t" \
            ".option pop\n\t" \
            : /* No output operands */ \
            : "r"(src), "r"(vl), "r"(vtype) /* Input operands */ \
            : "t0", "memory" /* Clobbered registers */ \
        ); \
    }

// 定义向量存储宏，将寄存器 v6 中的数据存储到内存
// 参数说明：src代表要从v6寄存器加载到dest向量中（数组保存），vl指定向量长度为多少
#define STORE_VECTOR_FROM_REGISTER(name, instruction, type ,sew_data) \
    static inline void name(type *dest, int vl) { \
        int sew = sew_data; \
        int vtype = VTYPE(sew, 1); \
        asm volatile( \
            ".option push\n\t" \
            ".option norvc\n\t" \
            "vsetvl t0, %1, %2\n\t" \
            instruction " v6, (%0)\n\t" \
            ".option pop\n\t" \
            : "+r" (dest) /* Output operand: destination address */ \
            : "r"(vl), "r"(vtype) /* Input operands */ \
            : "t0", "memory" /* Clobbered registers */ \
        ); \
    }

// 定义具体的加载和存储指令函数（固定使用 v6 寄存器,其它v指令同固定）
LOAD_VECTOR_TO_REGISTER(vle8_to_v6, "vle8.v", uint8_t, 8);
LOAD_VECTOR_TO_REGISTER(vle16_to_v6, "vle16.v", uint16_t, 16);
LOAD_VECTOR_TO_REGISTER(vle32_to_v6, "vle32.v", uint32_t, 32);
LOAD_VECTOR_TO_REGISTER(vle64_to_v6, "vle64.v", uint64_t, 64);

STORE_VECTOR_FROM_REGISTER(vse8_from_v6, "vse8.v", uint8_t, 8);
STORE_VECTOR_FROM_REGISTER(vse16_from_v6, "vse16.v", uint16_t, 16);
STORE_VECTOR_FROM_REGISTER(vse32_from_v6, "vse32.v", uint32_t, 32);
STORE_VECTOR_FROM_REGISTER(vse64_from_v6, "vse64.v", uint64_t, 64);

// -----------vector load and store(end)------------




// -----------vector reduction instructions(begin)------------


// -----------vredsum.vs instructions(begin)------------




// 设置 vredsum.vs 执行条件
static inline void set_vredsum_vs_conditions(int sew, int lmull, int vl, int vs1_init, int vs2_init) {
    // 计算 vtype
    int vtype = (sew << 3) | lmull;  // 根据传入的 LMUL 设置 vtype

    asm volatile (
        ".option push\n\t"
        ".option norvc\n\t"                    // 关闭 RISC-V 压缩指令模式，确保向量指令正常

        "vsetvli t0, %0, %1\n\t"              // 设置向量长度寄存器为指定长度，使用 vtype
        "vmv.v.i v0, 1\n\t"                     // 初始化 v0 掩码寄存器为全 1（确保所有元素有效）
        "vmv.v.i v4, %2\n\t"                    // 初始化 v4 向量寄存器（初始累加值）
        "vmv.v.i v6, %3\n\t"                    // 初始化 v6 向量寄存器（源数据）

        ".option pop\n\t"                       // 恢复之前的汇编器选项
        :                                           // 无输出操作数
        : "r"(vl), "i"(vtype), "I"(vs1_init), "I"(vs2_init)  // 输入操作数
        : "t0", "v0", "v4", "v6", "memory"      // 被修改的寄存器
    );
}


// 设置 vredsum.vs 执行条件的函数
static inline void execute_vredsum_vs() {
    asm volatile (
        ".option push\n\t"
        ".option norvc\n\t"                    // 关闭 RISC-V 压缩指令模式，确保向量指令正常

        "vredsum.vs v2, v2, v4, v0.t\n\t"      // 执行 vredsum.vs 指令，将 v4 中所有元素相加，并累加到 v2[0] 中

        ".option pop\n\t"                       // 恢复之前的汇编器选项
        :                                       // 无输出操作数
        :                                       // 无输入操作数
        : "v2", "v4", "v0", "memory"            // 被修改的寄存器
    );
}

// -----------vredsum.vs instructions(end)------------




// -----------vector reduction instructions(end)------------





// -----------vcpop.m instructions(begin)------------

static inline void set_vcpop_conditions(int vl, int sew, int lmull, int v0_init, int v2_init) {
    // 计算 vtype
    int vtype = (sew << 3) | lmull;

    asm volatile (
        ".option push\n\t"
        ".option norvc\n\t"                    // 关闭 RISC-V 压缩指令模式，确保向量指令正常

        "vsetvl t0, %0, %1\n\t"                 // 设置向量长度寄存器 vl 和 vtype
        "vmv.v.i v0, %2\n\t"                    // 初始化 v0 掩码寄存器
        "vmv.v.i v2, %3\n\t"                    // 初始化 v2 向量寄存器

        ".option pop\n\t"                       // 恢复之前的汇编器选项
        :                                       // 无输出操作数
        : "r"(vl), "r"(vtype), "I"(v0_init), "I"(v2_init)  // 输入操作数，%0 对应 vl, %1 对应 vtype, %2 对应 v0_init, %3 对应 v2_init
        : "t0", "v0", "v2", "memory"            // 被修改的寄存器
    );
}

#define execute_vcpop_v2()                          \
    ({                                              \
        uint32_t _temp;                             \
        asm volatile (                              \
            ".option push\n\t"                      \
            ".option norvc\n\t"                     \
            "vcpop.m %0, v2, v0.t\n\t"              \
            ".option pop\n\t"                       \
            : "=r"(_temp)                           \
            :                                       \
            : "v2", "v0", "memory"                  \
        );                                          \
        _temp;                                      \
    })



// -----------vcpop.m instructions(end)------------



// -----------vfirst_m_v3 instructions(begin)------------

// 设置 vfirst.m 执行条件
static inline void set_vfirst_m_conditions(int sew, int lmull, int vl, int v0_init, int v4_init) {
    // 计算 vtype
    int vtype = (sew << 3) | lmull;  // 根据传入的 LMUL 设置 vtype

    asm volatile (
        ".option push\n\t"
        ".option norvc\n\t"                    // 关闭 RISC-V 压缩指令模式，确保向量指令正常

        "vsetvl t0, %0, %1\n\t"                 // 设置向量长度寄存器 vl 和 vtype
        "vmv.v.i v0, %2\n\t"                    // 初始化 v0 掩码寄存器
        "vmv.v.i v4, %3\n\t"                    // 初始化 v4 源寄存器

        ".option pop\n\t"                       // 恢复之前的汇编器选项
        :                                       // 无输出操作数
        : "r"(vl), "r"(vtype), "I"(v0_init), "I"(v4_init)  // 输入操作数
        : "t0", "v4", "v0", "memory"            // 被修改的寄存器
    );
}


// vfirst.m 执行
#define EXECUTE_VFIRST_M_V3()                          \
    ({                                                 \
        uint32_t _temp;                                \
        asm volatile (                                 \
            ".option push\n\t"                         \
            ".option norvc\n\t"                        \
            "vfirst.m %0, v3, v0.t\n\t"                \
            ".option pop\n\t"                          \
            : "=r"(_temp)                              \
            :                                          \
            : "v3", "v0", "memory"                     \
        );                                             \
        _temp;                                         \
    })


// -----------vfirst_m_v3 instructions(end)------------


    

// -----------vmsbf_m instructions(begin)------------

// 设置执行 vmsbf.m 指令条件，包括向量长度、掩码寄存器和源寄存器
static inline void set_vmsbfm_conditions(int vl, int sew, int lmull, int v0_init, int v4_init) {
    // 计算 vtype
    int vtype = (sew << 3) | lmull;

    asm volatile (
        ".option push\n\t"
        ".option norvc\n\t"                    // 关闭 RISC-V 压缩指令模式，确保向量指令正常

        "vsetvl t0, %0, %1\n\t"                 // 设置向量长度寄存器 vl 和 vtype
        "vmv.v.i v0, %2\n\t"                    // 初始化 v0 掩码寄存器
        "vmv.v.i v4, %3\n\t"                    // 初始化 v4 源寄存器

        ".option pop\n\t"                       // 恢复之前的汇编器选项
        :                                       // 无输出操作数
        : "r"(vl), "r"(vtype), "I"(v0_init), "I"(v4_init)  // 输入操作数，%0 对应 vl, %1 对应 vtype, %2 对应 v0_init, %3 对应 v4_init
        : "t0", "v4", "v0", "memory"            // 被修改的寄存器
    );
}

// 执行 vmsbf.m 指令
static inline void execute_vmsbf_m() {
    asm volatile (
        ".option push\n\t"
        ".option norvc\n\t"                    // 关闭 RISC-V 压缩指令模式，确保向量指令正常

        "vmsbf.m v3, v4, v0.t\n\t"              // 执行向量掩码生成指令，生成掩码存到 v3

        ".option pop\n\t"                       // 恢复之前的汇编器选项
        :                                       // 无输出操作数
        :                                       // 无输入操作数
        : "v3", "v4", "v0", "memory"            // 被修改的寄存器
    );
}

// -----------vmsbf_m instructions(end)------------



// -----------vmsif_m instructions(begin)------------


// 设置 vmsif.m 执行条件
static inline void set_vmsifm_conditions(int vl, int sew, int lmull, int v0_init, int v4_init) {
    // 计算 vtype
    int vtype = (sew << 3) | lmull;  // 根据传入的 LMUL 设置 vtype

    asm volatile (
        ".option push\n\t"
        ".option norvc\n\t"                    // 关闭 RISC-V 压缩指令模式，确保向量指令正常

        "vsetvl t0, %0, %1\n\t"                 // 设置向量长度寄存器 vl 和 vtype
        "vmv.v.i v0, %2\n\t"                    // 初始化 v0 掩码寄存器
        "vmv.v.i v4, %3\n\t"                    // 初始化 v4 源寄存器

        ".option pop\n\t"                       // 恢复之前的汇编器选项
        :                                           // 无输出操作数
        : "r"(vl), "r"(vtype), "I"(v0_init), "I"(v4_init)  // 输入操作数
        : "t0", "v4", "v0", "memory"         // 被修改的寄存器
    );
}

// 执行 vmsif.m 指令
static inline void execute_vmsif_m() {
    asm volatile (
        ".option push\n\t"
        ".option norvc\n\t"                    // 关闭 RISC-V 压缩指令模式，确保向量指令正常

        "vmsif.m v5, v4, v0.t\n\t"              // 执行 vmsif.m 指令，基于掩码生成 v4 的内容

        ".option pop\n\t"                       // 恢复之前的汇编器选项
        :                                         // 无输出操作数
        :                                         // 无输入操作数
        : "v5", "v4", "v0", "memory"              // 被修改的寄存器
    );
}

// -----------vmsif_m instructions(end)------------


// -----------vmsif_m instructions(begin)------------


// 设置 vmsof.m 执行条件
static inline void set_vmsofm_conditions(int vl, int sew, int lmull, int v0_init, int v4_init) {
    // 计算 vtype
    int vtype = (sew << 3) | lmull;  // 根据传入的 LMUL 设置 vtype

    asm volatile (
        ".option push\n\t"
        ".option norvc\n\t"                    // 关闭 RISC-V 压缩指令模式，确保向量指令正常

        "vsetvl t0, %0, %1\n\t"                 // 设置向量长度寄存器 vl 和 vtype
        "vmv.v.i v0, %2\n\t"                    // 初始化 v0 掩码寄存器
        "vmv.v.i v4, %3\n\t"                    // 初始化 v4 源寄存器

        ".option pop\n\t"                       // 恢复之前的汇编器选项
        :                                           // 无输出操作数
        : "r"(vl), "r"(vtype), "I"(v0_init), "I"(v4_init)  // 输入操作数
        : "t0", "v4", "v0", "memory"         // 被修改的寄存器
    );
}

// 执行 vmsif.m 指令
static inline void execute_vmsof_m() {
    asm volatile (
        ".option push\n\t"
        ".option norvc\n\t"                    // 关闭 RISC-V 压缩指令模式，确保向量指令正常

        "vmsof.m v5, v4, v0.t\n\t"              // 执行 vmsof.m 指令，基于掩码生成 v4 的内容

        ".option pop\n\t"                       // 恢复之前的汇编器选项
        :                                         // 无输出操作数
        :                                         // 无输入操作数
        : "v5", "v4", "v0", "memory"              // 被修改的寄存器
    );
}

// -----------vmsof_m instructions(end)------------





// -----------vadd_vv instructions(begin)------------



// 设置 vadd 执行条件
static inline void set_vadd_conditions(int sew, int lmull, int vl, int v0_init, int v4_init, int v6_init) {
    // 计算 vtype
    int vtype = (sew << 3) | lmull;  // 根据传入的 LMUL 设置 vtype

    asm volatile (
        ".option push\n\t"
        ".option norvc\n\t"                    // 关闭 RISC-V 压缩指令模式，确保向量指令正常

        "vsetvli t0, %0, %1\n\t"                 // 设置向量长度寄存器 vl 和 vtype
        "vmv.v.i v0, %2\n\t"                    // 初始化 v0 掩码寄存器
        "vmv.v.i v4, %3\n\t"                    // 初始化 v4 源寄存器 1
        "vmv.v.i v6, %4\n\t"                    // 初始化 v6 源寄存器 2

        ".option pop\n\t"                       // 恢复之前的汇编器选项
        :                                           // 无输出操作数
        : "r"(vl), "rI"(vtype), "I"(v0_init), "I"(v4_init), "I"(v6_init)  // 输入操作数，vtype 改为立即数或寄存器
        : "t0", "v0", "v4", "v6", "memory"             // 被修改的寄存器
    );
}




// 执行 vadd.vv 指令
static inline void execute_vadd_vv() {
    asm volatile (
        ".option push\n\t"
        ".option norvc\n\t"                    // 关闭 RISC-V 压缩指令模式，确保向量指令正常

        "vadd.vv v8, v4, v6\n\t"               // 执行向量加法指令，将 v4 和 v6 的对应元素相加，结果存入 v8

        ".option pop\n\t"                       // 恢复之前的汇编器选项
        :                                         // 无输出操作数
        :                                         // 无输入操作数
        : "v8", "v4", "v6", "memory"        // 被修改的寄存器
    );
}





// -----------vadd_vv instructions(end)------------



// -----------vfadd instructions(begin)------------


// 设置向量浮点加法指令的执行条件
static inline void set_vfadd_conditions(int sew, int lmull, int vl, float v0_init, float v4_init, float v6_init) {
    // 计算 vtype
    int vtype = (sew << 3) | lmull;  // 根据传入的 LMUL 设置 vtype

    asm volatile (
        ".option push\n\t"
        ".option norvc\n\t"                    // 关闭 RISC-V 压缩指令模式，确保向量指令正常

        "vsetvli t0, %0, %1\n\t"                 // 设置向量长度寄存器 vl 和 vtype
        "vfmv.v.f v0, %2\n\t"                    // 初始化 v0 掩码寄存器
        "vfmv.v.f v4, %3\n\t"                    // 初始化 v4 源寄存器 1
        "vfmv.v.f v6, %4\n\t"                    // 初始化 v6 源寄存器 2

        ".option pop\n\t"                       // 恢复之前的汇编器选项
        :                                           // 无输出操作数
        : "r"(vl), "rI"(vtype), "f"(v0_init), "f"(v4_init), "f"(v6_init)  // 输入操作数，vtype 改为立即数或寄存器
        : "t0", "v0", "v4", "v6", "memory"             // 被修改的寄存器
    );
}

// 执行向量浮点加法指令
static inline void execute_vfadd() {
    asm volatile (
        ".option push\n\t"
        ".option norvc\n\t"                    // 关闭 RISC-V 压缩指令模式，确保向量指令正常

        "vfadd.vv v8, v4, v6\n\t"               // 执行浮点加法，结果存储到 v8 中

        ".option pop\n\t"                       // 恢复之前的汇编器选项
        :                                           // 无输出操作数
        :                                           // 无输入操作数
        : "v8", "v4", "v6", "memory"          // 被修改的寄存器
    );
}




// -----------vfadd instructions(end)------------











// -----------viota.m instructions(begin)------------


// 设置 viota.m 执行条件
static inline void set_viota_m_conditions(int vl, int sew, int lmull, int v0_init) {
    // 计算 vtype
    int vtype = (sew << 3) | lmull;  // 根据传入的 LMUL 设置 vtype

    asm volatile (
        ".option push\n\t"
        ".option norvc\n\t"                    // 关闭 RISC-V 压缩指令模式，确保向量指令正常

        "vsetvl t0, %0, %1\n\t"                 // 设置向量长度寄存器 vl 和 vtype
        "vmv.v.i v0, %2\n\t"                    // 初始化 v0 掩码寄存器

        ".option pop\n\t"                       // 恢复之前的汇编器选项
        :                                           // 无输出操作数
        : "r"(vl), "r"(vtype), "I"(v0_init)  // 输入操作数
        : "t0", "v0", "memory"                // 被修改的寄存器
    );
}

// 执行 viota.m 指令
static inline void execute_viota_m() {
    asm volatile (
        ".option push\n\t"
        ".option norvc\n\t"                    // 关闭 RISC-V 压缩指令模式，确保向量指令正常

        "viota.m v6, v0\n\t"                  // 执行 viota.m 指令，将掩码向量 v0 的索引结果存入 v6 中

        ".option pop\n\t"                       // 恢复之前的汇编器选项
        :                                         // 无输出操作数
        :                                         // 无输入操作数
        : "v6", "v0", "memory"                // 被修改的寄存器
    );
}


// -----------viota.m instructions(end)------------





// -----------vcompress instructions(begin)------------


// 设置 vcompress 执行条件
static inline void set_vcompress_conditions(int vl, int sew, int lmull, int v0_init, int v4_init, int v8_init) {
    // 计算 vtype
    int vtype = (sew << 3) | lmull;  // 根据传入的 LMUL 设置 vtype

    asm volatile (
        ".option push\n\t"
        ".option norvc\n\t"                    // 关闭 RISC-V 压缩指令模式，确保向量指令正常

        "vsetvl t0, %0, %1\n\t"                 // 设置向量长度寄存器 vl 和 vtype
        "vmv.v.i v0, %2\n\t"                    // 初始化 v0 掩码寄存器
        "vmv.v.i v4, %3\n\t"                    // 初始化 v4 源寄存器
        "vmv.v.i v8, %4\n\t"                    // 初始化 v8 目标寄存器

        ".option pop\n\t"                       // 恢复之前的汇编器选项
        :                                           // 无输出操作数
        : "r"(vl), "r"(vtype), "I"(v0_init), "I"(v4_init), "I"(v8_init)  // 输入操作数
        : "t0", "v0", "v4", "v8", "memory"  // 被修改的寄存器
    );
}

// 执行 vcompress 指令
static inline void execute_vcompress() {
    asm volatile (
        ".option push\n\t"
        ".option norvc\n\t"                    // 关闭 RISC-V 压缩指令模式，确保向量指令正常

        "vcompress.vm v8, v4, v0\n\t"        // 执行 vcompress 指令，将 v4 压缩后存储到 v8 中

        ".option pop\n\t"                       // 恢复之前的汇编器选项
        :                                         // 无输出操作数
        :                                         // 无输入操作数
        : "v8", "v4", "v0", "memory"         // 被修改的寄存器
    );
}

// -----------vcompress instructions(ends)------------



// // 使用不支持的 SEW 和 LMUL 配置来设置 vtype
// static inline void set_vtype_with_invalid_eew() {
//     asm volatile (
//         ".option push\n\t"
//         ".option norvc\n\t"                    // 关闭 RISC-V 压缩指令模式，确保向量指令正常

//         "vsetvli t0, zero, e128, m1, ta, mu\n\t"  // 设置不支持的 SEW = e128 (128位) 和 LMUL = 1

//         ".option pop\n\t"                       // 恢复之前的汇编器选项
//         :                                       // 无输出操作数
//         :                                       // 无输入操作数
//         : "t0", "memory"                        // 被修改的寄存器
//     );
// }






// --------------------- 分割线: 向量指令相关end ---------------------






void random_m_instruction();

#endif /* __ASSEMBLER__ */

#endif /* INSTRUCTIONS_H */
