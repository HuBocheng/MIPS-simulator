#include <stdio.h>
#include "shell.h"
#include "sim.h"
#include <string.h>
#include <math.h>

#define MIPS_REGS 32

char *regNames[MIPS_REGS] = {
    "$zero[$0]", // 常量 0
    "$at[$1]",   // 汇编器临时变量
    "$v0[$2]",   // 函数返回值
    "$v1[$3]",   // 函数返回值
    "$a0[$4]",   // 函数参数
    "$a1[$5]",   // 函数参数
    "$a2[$6]",   // 函数参数
    "$a3[$7]",   // 函数参数
    "$t0[$8]",   // 临时变量
    "$t1[$9]",   // 临时变量
    "$t2[$10]",  // 临时变量
    "$t3[$11]",  // 临时变量
    "$t4[$12]",  // 临时变量
    "$t5[$13]",  // 临时变量
    "$t6[$14]",  // 临时变量
    "$t7[$15]",  // 临时变量
    "$s0[$16]",  // 保存寄存器
    "$s1[$17]",  // 保存寄存器
    "$s2[$18]",  // 保存寄存器
    "$s3[$19]",  // 保存寄存器
    "$s4[$20]",  // 保存寄存器
    "$s5[$21]",  // 保存寄存器
    "$s6[$22]",  // 保存寄存器
    "$s7[$23]",  // 保存寄存器
    "$t8[$24]",  // 临时变量
    "$t9[$25]",  // 临时变量
    "$k0[$26]",  // 内核使用
    "$k1[$27]",  // 内核使用
    "$gp[$28]",  // 全局指针
    "$sp[$29]",  // 栈指针
    "$fp[$30]",  // 帧指针（或 $s8）
    "$ra[$31]"   // 返回地址
};

void process_instruction()
{
    /* execute one instructionCode here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. */

    // 读32位指令，并把这个32位指令转换成二进制（因为mem_read_32函数返回的是32位指令的十进制形式）
    // printf("%u\n",mem_read_32(CURRENT_STATE.PC));
    uintToStr(mem_read_32(CURRENT_STATE.PC));
    strncpy(instructionCode, conversion, 33);

    // 如果指令为BREAK，那么就停止运行，设定$v0的值为10,RUN_BIT运行标记位false并返回
    // 对应每个.s文件中syscall的命令
    if (str2Uint(0, 31) == 12)
    {
        if (NEXT_STATE.REGS[2] == 10)
        {
            RUN_BIT = 0;
            return;
        }
    }
    // int type = convert(6);
    int type = str2Uint(0, 5);
    // printf("%s %d","type:",type);
    if (type == 0)
    { // 指令为R型指令
        // int rs = convert(5), rt = convert(5), rd = convert(5), shamt = convert(5), funct = convert(6);
        int rs = str2Uint(6, 10);
        int rt = str2Uint(11, 15);
        int rd = str2Uint(16, 20);
        int shamt = str2Uint(21, 25);
        int funct = str2Uint(26, 31);

        char *rsName = regNames[rs];
        char *rtName = regNames[rt];
        char *rdName = regNames[rd];

        switch (funct)
        {
        case 32: // ADD
            // strcpy(instruction, "ADD");
            sprintf(instruction, "%s %s %s %s %s", "instruction ADD", rdName, rsName, rtName, "executed");
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
            break;
        case 33: // ADDU
            sprintf(instruction, "%s %s %s %s %s", "instruction ADDU", rdName, rsName, rtName, "executed");
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
            break;
        case 37: // OR
            sprintf(instruction, "%s %s %s %s %s", "instruction OR", rdName, rsName, rtName, "executed");
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt];
            break;
        case 36: // AND
            sprintf(instruction, "%s %s %s %s %s", "instruction AND", rdName, rsName, rtName, "executed");
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] & CURRENT_STATE.REGS[rt];
            break;
        case 38: // XOR
            sprintf(instruction, "%s %s %s %s %s", "instruction XOR", rdName, rsName, rtName, "executed");
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] ^ CURRENT_STATE.REGS[rt];
            break;
        case 34: // SUB
            sprintf(instruction, "%s %s %s %s %s", "instruction SUB", rdName, rsName, rtName, "executed");
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
            break;
        case 35: // SUBU
            sprintf(instruction, "%s %s %s %s %s", "instruction SUBU", rdName, rsName, rtName, "executed");
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
            break;

        case 0: // SLL
            sprintf(instruction, "%s %s %s %d %s", "instruction SLL", rdName, rsName, shamt, "executed");
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << shamt;
            break;

        case 2: // SRL
            sprintf(instruction, "%s %s %s %d %s", "instruction SRL", rdName, rsName, shamt, "executed");
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> shamt;
            break;

        case 3: // SRA
            sprintf(instruction, "%s %s %s %d %s", "instruction SRA", rdName, rsName, shamt, "executed");
            uint32_t result = CURRENT_STATE.REGS[rt] >> shamt; // 先逻辑右移
            if (CURRENT_STATE.REGS[rt] & 0x80000000)           // 检查最高位是否设置
            {
                result |= (~0U << (32 - shamt)); // 设定高几位为1
            }

            NEXT_STATE.REGS[rd] = result;
            break;

        case 4:
            // sllv，变位移量的逻辑左移
            sprintf(instruction, "%s %s %s %s %s", "instruction SLLV", rdName, rtName, rsName, "executed");
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << CURRENT_STATE.REGS[rs];
            break;

        case 6:
            // srlv，变位移量的逻辑右移
            sprintf(instruction, "%s %s %s %s %s", "instruction SRLV", rdName, rtName, rsName, "executed");
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> CURRENT_STATE.REGS[rs];
            break;

        case 7:
            // srav，变位移量的算术右移
            sprintf(instruction, "%s %s %s %s %s", "instruction SRAV", rdName, rtName, rsName, "executed");
            uint32_t result2 = CURRENT_STATE.REGS[rt] >> CURRENT_STATE.REGS[rs];
            if (CURRENT_STATE.REGS[rt] & 0x80000000)
            {
                result2 |= (~0U << (32 - CURRENT_STATE.REGS[rs]));
            }
            NEXT_STATE.REGS[rd] = result2;
            break;

        case 8: // JR,跳转到寄存器rs中的地址
            sprintf(instruction, "%s %s %s", "instruction JR", rsName, "executed");
            NEXT_STATE.PC = (uint32_t)CURRENT_STATE.REGS[rs] - 4;
            break;
        case 9: // JALR，跳转到寄存器rs中的地址，并将返回地址存储在寄存器rd中
            sprintf(instruction, "%s %s %s %s", "instruction JALR", rdName, rsName, "executed");
            NEXT_STATE.REGS[rd] = CURRENT_STATE.PC;
            NEXT_STATE.PC = CURRENT_STATE.REGS[rs];
            break;

        case 16: // MFHI,将HI寄存器的内容移动到一个普通的寄存器中
            sprintf(instruction, "%s %s %s", "instruction MFHI", rdName, "executed");
            NEXT_STATE.REGS[rd] = CURRENT_STATE.HI;
            break;

        case 17: // MTHI,将一个普通寄存器的内容移动到HI寄存器中。
            sprintf(instruction, "%s %s %s", "instruction MTHI", rsName, "executed");
            NEXT_STATE.HI = CURRENT_STATE.REGS[rs];
            break;

        case 18: // MFLO，将LO寄存器的内容移动到一个普通的寄存器中。
            sprintf(instruction, "%s %s %s", "instruction MFLO", rdName, "executed");
            NEXT_STATE.REGS[rd] = CURRENT_STATE.LO;
            break;

        case 19: // MTLO，将一个普通寄存器的内容移动到LO寄存器中
            sprintf(instruction, "%s %s %s", "instruction MTLO", rsName, "executed");
            NEXT_STATE.LO = CURRENT_STATE.REGS[rs];
            break;

        case 24: // MULT
            sprintf(instruction, "%s %s %s %s", "instruction MULT", rsName, rtName, "executed");
            int64_t result3 = (int64_t)CURRENT_STATE.REGS[rs] * (int64_t)CURRENT_STATE.REGS[rt];
            NEXT_STATE.HI = result3 >> 32;
            NEXT_STATE.LO = result3 & 0xffffffff;
            break;

        case 25: // MULTU
            sprintf(instruction, "%s %s %s %s", "instruction MULTU", rsName, rtName, "executed");
            uint64_t result4 = (uint64_t)CURRENT_STATE.REGS[rs] * (uint64_t)CURRENT_STATE.REGS[rt];
            NEXT_STATE.HI = result4 >> 32;
            NEXT_STATE.LO = result4 & 0xffffffff;
            break;

        case 26: // DIV
            sprintf(instruction, "%s %s %s %s", "instruction DIV", rsName, rtName, "executed");
            NEXT_STATE.LO = CURRENT_STATE.REGS[rs] / CURRENT_STATE.REGS[rt];
            NEXT_STATE.HI = CURRENT_STATE.REGS[rs] % CURRENT_STATE.REGS[rt];
            break;

        case 27: // DIVU
            sprintf(instruction, "%s %s %s %s", "instruction DIVU", rsName, rtName, "executed");
            NEXT_STATE.LO = CURRENT_STATE.REGS[rs] / CURRENT_STATE.REGS[rt];
            NEXT_STATE.HI = CURRENT_STATE.REGS[rs] % CURRENT_STATE.REGS[rt];
            break;
        }
    }
    else if (type == 2 || type == 3)
    { // J - TYPE
        // int address = convert(26) * 4;
        int address = str2Uint(6, 31) * 4;
        switch (type)
        {
        case 2: // J
            // strcpy(instruction, "J");
            NEXT_STATE.PC = (uint32_t)address - 4;
            sprintf(instruction, "%s %d %s", "instruction J", address, "executed");
            break;
        case 3: // JAL，跳转到指定地址，并将返回地址存储在寄存器31（%ra）中
            // Salvando RA
            // strcpy(instruction, "JAL");
            NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
            // ***这里需要+4，JAL放入PC的是下一条指令的地址
            // ***死循环就是因为这里没+4
            NEXT_STATE.PC = address - 4;
            // ***这里要-4，因为switch出去之后+4
            sprintf(instruction, "%s %d %s", "instruction JAL", address, "executed");
            break;
        }
    }
    else // I型指令
    {
        int rs, rt, immediate, immediate2;
        char subbuff[50];
        // rs = convert(5);
        // rt = convert(5);
        rs = str2Uint(6, 10);
        rt = str2Uint(11, 15);
        immediate = complemento2(16, 31);
        immediate2 = str2Uint(16, 31);

        char *rsName = regNames[rs];
        char *rtName = regNames[rt];
        // printf("%s %d %s %d\n","rs:",rs,"rt:",rt);
        switch (type)
        {
            // ***下面使用complemento2函数为了求补码，一个数求两次补码变成本身，mips指令中I型指令立即数为补码形式

        case 1: // BLTZ
            // strcpy(instruction, "BLTZ");
            if (rt == 0)
            { // BLTZ
                sprintf(instruction, "%s %s %d %s", "instruction BLTZ", rsName, immediate, "executed");
                if (NEXT_STATE.REGS[rs] & 0x80000000)
                {
                    NEXT_STATE.PC += immediate * 4;
                }
            }
            else if (rt == 1)
            { // BGEZ
                sprintf(instruction, "%s %s %d %s", "instruction BGEZ", rsName, immediate, "executed");
                if (!(NEXT_STATE.REGS[rs] & 0x80000000))
                {
                    NEXT_STATE.PC += immediate * 4;
                }
            }
            else if (rt == 16)
            { // BLTZAL
                sprintf(instruction, "%s %s %d %s", "instruction BLTZAL", rsName, immediate, "executed");
                if (NEXT_STATE.REGS[rs] & 0x80000000)
                {
                    NEXT_STATE.PC += immediate * 4;
                    NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 8;
                }
            }
            else if (rt == 17)
            { // BGEZAL
                sprintf(instruction, "%s %s %d %s", "instruction BGEZAL", rsName, immediate, "executed");
                if (!(NEXT_STATE.REGS[rs] & 0x80000000))
                {
                    NEXT_STATE.PC += immediate * 4;
                    NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 8;
                }
            }
            else
            {
                sprintf(instruction, "%s", "Invalid instruction");
            }
            break;

        case 4: // BEQ
            // strcpy(instruction, "BEQ");
            if (NEXT_STATE.REGS[rs] == CURRENT_STATE.REGS[rt])
            {
                NEXT_STATE.PC += complemento2(16, 31) * 4;
            }
            sprintf(instruction, "%s %s %s %d %s", "instruction BEQ", rsName, rtName, immediate, "executed");
            break;
        case 5: // BNE
            // strcpy(instruction, "BNE");
            if (NEXT_STATE.REGS[rs] != CURRENT_STATE.REGS[rt])
            {
                NEXT_STATE.PC += complemento2(16, 31) * 4;
            }
            sprintf(instruction, "%s %s %s %d %s", "instruction BNE", rsName, rtName, immediate, "executed");
            break;
        case 6: // BLEZ 小于等于0跳转
            strcpy(instruction, "BLEZ");
            if (NEXT_STATE.REGS[rs] & 0x80000000)
            {
                NEXT_STATE.PC += complemento2(16, 31) * 4;
            }
            sprintf(instruction, "%s %s %d %s", "instruction BLEZ", rsName, immediate, "executed");
            break;

        case 7: // BGTZ 大于等于0跳转
            strcpy(instruction, "BGTZ");
            if (!(NEXT_STATE.REGS[rs] & 0x80000000))
            {
                NEXT_STATE.PC += complemento2(16, 31) * 4;
            }
            sprintf(instruction, "%s %s %d %s", "instruction BGTZ", rsName, immediate, "executed");
            break;
        case 8: // ADDI
            // strcpy(instruction, "ADDI");
            printf("%d\n", immediate);
            NEXT_STATE.REGS[rt] = immediate + CURRENT_STATE.REGS[rs];
            sprintf(instruction, "%s %s %s %d %s", "instruction ADDI", rtName, rsName, immediate, "executed");
            break;
        case 9: // ADDIU
            // strcpy(instruction, "ADDIU");
            NEXT_STATE.REGS[rt] = immediate + CURRENT_STATE.REGS[rs];
            sprintf(instruction, "%s %s %s %d %s", "instruction ADDIU", rtName, rsName, immediate, "executed");
            break;
        case 10: // SLTI
            // strcpy(instruction, "SLTI");
            NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] < complemento2(16, 31) ? 1 : 0;
            sprintf(instruction, "%s %s %s %d %s", "instruction SLTI", rsName, rtName, immediate, "executed");
            break;
        case 11: // SLTIU
            // strcpy(instruction, "SLTIU");
            NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] < str2Uint(16, 31) ? 1 : 0;
            sprintf(instruction, "%s %s %s %d %s", "instruction SLTIU", rtName, rsName, immediate, "executed");
            break;
        case 12: // ANDI
            // strcpy(instruction, "ANDI");
            NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] & str2Uint(16, 31);
            sprintf(instruction, "%s %s %s %d %s", "instruction ANDI", rtName, rsName, immediate, "executed");
            break;
        case 13: // ORI
            // strcpy(instruction, "ORI");
            NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] | str2Uint(16, 31);
            sprintf(instruction, "%s %s %s %d %s", "instruction ORI", rtName, rsName, immediate, "executed");
            break;
        case 14: // XORI
            // strcpy(instruction, "XORI");
            NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] ^ str2Uint(16, 31);
            sprintf(instruction, "%s %s %s %d %s", "instruction XORI", rtName, rsName, immediate, "executed");
            break;
        case 15: // LUI
            // strcpy(instruction, "LUI");
            NEXT_STATE.REGS[rt] = complemento2(16, 31) << 16;
            sprintf(instruction, "%s %s %d %s", "instruction LUI", rtName, immediate, "executed");
            break;
        case 32: // LB
            // strcpy(instruction, "LB");
            uintToStr(mem_read_32(complemento2(16, 31) + CURRENT_STATE.REGS[rs]));
            strncpy(subbuff, conversion, 8);
            NEXT_STATE.REGS[rt] = complemento2WithChar(subbuff, 8);
            sprintf(instruction, "%s %s %d (%s) %s", "instruction LB", rtName, immediate, rsName, "executed");
            break;
        case 33: // LH
            // strcpy(instruction, "LH");
            uintToStr(mem_read_32(complemento2(16, 31) + CURRENT_STATE.REGS[rs]));
            strncpy(subbuff, conversion, 16);
            NEXT_STATE.REGS[rt] = complemento2WithChar(subbuff, 16);
            sprintf(instruction, "%s %s %d (%s) %s", "instruction LH", rtName, immediate, rsName, "executed");
            break;
        case 35: // LW
            // strcpy(instruction, "LW");
            NEXT_STATE.REGS[rt] = mem_read_32(complemento2(16, 31) + CURRENT_STATE.REGS[rs]);
            sprintf(instruction, "%s %s %d (%s) %s", "instruction LW", rtName, immediate, rsName, "executed");
            break;
        case 36: // LBU
            // strcpy(instruction, "LBU");
            uintToStr(mem_read_32(complemento2(16, 31) + CURRENT_STATE.REGS[rs]));
            strncpy(subbuff, conversion, 8);
            NEXT_STATE.REGS[rt] = complemento2WithChar(subbuff, 8);
            sprintf(instruction, "%s %s %d (%s) %s", "instruction LBU", rtName, immediate, rsName, "executed");
            break;
        case 37: // LHU
            // strcpy(instruction, "LHU");
            uintToStr(mem_read_32(complemento2(16, 31) + CURRENT_STATE.REGS[rs]));
            strncpy(subbuff, conversion, 16);
            NEXT_STATE.REGS[rt] = complemento2WithChar(subbuff, 16);
            sprintf(instruction, "%s %s %d (%s) %s", "instruction LHU", rtName, immediate, rsName, "executed");
            break;
        case 40: // SB
            // strcpy(instruction, "SB");
            uintToStr(NEXT_STATE.REGS[rt]);
            strncpy(subbuff, conversion, 8);
            mem_write_32(complemento2(16, 31) + CURRENT_STATE.REGS[rs], complemento2WithChar(subbuff, 8));
            sprintf(instruction, "%s %s %d (%s) %s", "instruction SB", rtName, immediate, rsName, "executed");
            break;
        case 41: // SH
            // strcpy(instruction, "SH");
            uintToStr(NEXT_STATE.REGS[rt]);
            strncpy(subbuff, conversion, 16);
            mem_write_32(complemento2(16, 31) + CURRENT_STATE.REGS[rs], complemento2WithChar(subbuff, 16));
            sprintf(instruction, "%s %s %d (%s) %s", "instruction SH", rtName, immediate, rsName, "executed");
            break;
        case 43: // SW
            // strcpy(instruction, "SW");
            mem_write_32(complemento2(16, 31) + CURRENT_STATE.REGS[rs], NEXT_STATE.REGS[rt]);
            sprintf(instruction, "%s %s %d (%s) %s", "instruction SW", rtName, immediate, rsName, "executed");
            break;
        }
    }
    printf("%s\n", instruction);
    printf("%s%s\n\n", "The corresponding code is ", instructionCode);

    NEXT_STATE.PC += 4;
}

// 计算从start到end这些位代表的二进制数的补码
int complemento2(int start, int end)
{
    int length = end - start + 1;
    int temp = 0;

    // 如果最高位是1，那么它是一个负数，所以我们从最高位的权重减去
    if (instructionCode[start] == '1')
    {
        temp -= (int)pow(2, length - 1);
    }

    // 从下一个位开始，将每一位转换为其对应的十进制值
    for (int i = start + 1; i <= end; i++)
    {
        if (instructionCode[i] == '1')
        {
            temp += (int)pow(2, end - i);
        }
    }

    return temp;
}

int complemento2WithChar(char *c, int size)
{
    int temp = 0;
    if (c[0] == '1')
    {
        temp -= pow(2, size - 1);
    }
    // 从下一个位开始，将每一位转换为其对应的十进制值
    int p = 1;
    while (c[p] != 0)
    {
        if (c[p] == '1')
        {
            temp += pow(2, size - 1 - p);
        }
        p++;
    }
    return temp;
}

// 把从start到end这些位表示的二进制数变成32位无符号整数
int str2Uint(int start, int end)
{ // 取1-5位：索引0到4
    int temp = 0;
    int length = end - start + 1;
    for (int i = start; i <= end; i++)
    {
        if (instructionCode[i] == '1')
        {
            temp += (int)pow(2, length + start - 1 - i);
            // printf("%s %d\n","add",(int)pow(2, length - 1 - i));
        }
    }
    return temp;
}

// 将一个32位无符号整数转换为其二进制字符串表示形式
void uintToStr(uint32_t u)
{
    char c[50];
    char h[50];
    int size = 0;
    sprintf(c, "%x", mem_read_32(NEXT_STATE.PC));
    // Tamanho a partir do primeiro não nulo
    while (c[size] != '\0')
    {
        size++;
    }
    // Poe zeros à esquera
    for (int i = 0; i < 8 - size; i++)
    {
        h[i] = '0';
    }
    for (int i = 8 - size; i < 8; i++)
    {
        h[i] = c[i - 8 + size];
    }
    h[8] = '\0';
    printf("%s 0x%s\n", "Hexadecimal instruction code is", h);
    // Converte de HEX para BINARIO
    int j;
    for (int i = 0; i < 8; i++)
    {
        j = i * 4;
        switch (h[i])
        {
        case '0':
            conversion[j] = '0';
            conversion[j + 1] = '0';
            conversion[j + 2] = '0';
            conversion[j + 3] = '0';
            break;
        case '1':
            conversion[j] = '0';
            conversion[j + 1] = '0';
            conversion[j + 2] = '0';
            conversion[j + 3] = '1';
            break;
        case '2':
            conversion[j] = '0';
            conversion[j + 1] = '0';
            conversion[j + 2] = '1';
            conversion[j + 3] = '0';
            break;
        case '3':
            conversion[j] = '0';
            conversion[j + 1] = '0';
            conversion[j + 2] = '1';
            conversion[j + 3] = '1';
            break;
        case '4':
            conversion[j] = '0';
            conversion[j + 1] = '1';
            conversion[j + 2] = '0';
            conversion[j + 3] = '0';
            break;
        case '5':
            conversion[j] = '0';
            conversion[j + 1] = '1';
            conversion[j + 2] = '0';
            conversion[j + 3] = '1';
            break;
        case '6':
            conversion[j] = '0';
            conversion[j + 1] = '1';
            conversion[j + 2] = '1';
            conversion[j + 3] = '0';
            break;
        case '7':
            conversion[j] = '0';
            conversion[j + 1] = '1';
            conversion[j + 2] = '1';
            conversion[j + 3] = '1';
            break;
        case '8':
            conversion[j] = '1';
            conversion[j + 1] = '0';
            conversion[j + 2] = '0';
            conversion[j + 3] = '0';
            break;
        case '9':
            conversion[j] = '1';
            conversion[j + 1] = '0';
            conversion[j + 2] = '0';
            conversion[j + 3] = '1';
            break;
        case 'a':
            conversion[j] = '1';
            conversion[j + 1] = '0';
            conversion[j + 2] = '1';
            conversion[j + 3] = '0';
            break;
        case 'b':
            conversion[j] = '1';
            conversion[j + 1] = '0';
            conversion[j + 2] = '1';
            conversion[j + 3] = '1';
            break;
        case 'c':
            conversion[j] = '1';
            conversion[j + 1] = '1';
            conversion[j + 2] = '0';
            conversion[j + 3] = '0';
            break;
        case 'd':
            conversion[j] = '1';
            conversion[j + 1] = '1';
            conversion[j + 2] = '0';
            conversion[j + 3] = '1';
            break;
        case 'e':
            conversion[j] = '1';
            conversion[j + 1] = '1';
            conversion[j + 2] = '1';
            conversion[j + 3] = '0';
            break;
        case 'f':
            conversion[j] = '1';
            conversion[j + 1] = '1';
            conversion[j + 2] = '1';
            conversion[j + 3] = '1';
            break;
        }
    }
}