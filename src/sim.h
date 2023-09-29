#include <stdio.h>
#include "shell.h"
#include <string.h>

char instructionCode[320];
char conversion[100];
char instruction[100];
extern char *regNames[MIPS_REGS];

int str2Uint(int start, int end);            // 截取指令start到end字段的编码，将其转换为整数返回
int complemento2(int start, int end);        // 截取指令start到end字段的编码（补码形式），再取一次补码返回原数据
int complemento2WithChar(char *c, int size); // // 截取c字符串start到end字段的编码（补码形式），再取一次补码返回原数据
void uintToStr(uint32_t u);                  // 把一个无符号整数转变为二进制编码（十->十六->二）