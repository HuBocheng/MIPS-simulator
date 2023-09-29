#!/usr/bin/python3

import os
# import sys
import pickle as pkl

def getBranchOffset(instructions,targetlabel,curInstruction):
    target=targetlabel+':'
    curIndex=instructions.index(curInstruction)
    targetIndex=instructions.index(target+'\n')
    offset=int(targetIndex)-int(curIndex)
    for i in range(curIndex,targetIndex+1):
        print(instructions[i])
        if ':' in instructions[i]:
            offset=offset-1
    return offset

startEnable=False
with open('codemap.pkl','rb') as f:
    codesMap=pkl.load(f)

# with open (sys.argv[1]) as f:
with open ('inputs/brtest2.s') as f:
# 需要加inputs/，解决这个问题最好就是用sys的angr参数
    lines = f.readlines()
        
    for line in lines:
        if len(line.strip().split())>0:
            flag=line.strip().split()[0]
        if flag=='main:' or flag=='__start:':
            startEnable=True
            continue
        if startEnable:
           originLine=line # 替换之前的line
           line=line.replace(',',' ') # 逗号换空格便于切割（按，和空格切割）
           elements=line.strip().split()
           regNum=line.count('$') # 寄存器个数
           if line=='syscall': # 退出指令的编码
               totalcodes='00000000000000000000000000001100'
           if ':' in line: # 跳过lable标签
               continue
           if regNum==3: # R型指令
               name=elements[0]
               para1=elements[1]
               para2=elements[2]
               para3=elements[3]

               opcode='000000'
               rscode=codesMap.get(para1,'error')
               rtcode=codesMap.get(para2,'error')
               rdcode=codesMap.get(para3,'error')
               sacode='00000'
               functcode=codesMap.get(name,'error')
               
               if name in ['slr','sra','sll']:
                  rscode='00000' 
                  sacode=bin(int(para3))[2:].zfill(5) # shamt字段转换二进制串并保证五位长
               # 得到最后的机器码！
               totalcodes=opcode+rscode+rtcode+rdcode+sacode+functcode

           elif regNum==2: # I型指令
               name=elements[0]
               para1=elements[1]
               para2=elements[2]
               para3=elements[3]

               opcode=codesMap.get(name,'error')
               rscode=codesMap.get(para1,'error')
               rtcode=codesMap.get(para2,'error')

               if name in ['beq','bne']:
                   offset=getBranchOffset(lines,para3,originLine)
                   immcode=bin(offset)[2:].zfill(16)

               else:
                   immcode=bin(int(para3,16))[2:].zfill(16)

               totalcodes=opcode+rscode+rtcode+immcode
           elif regNum==0:
               name=elements[0]
               Jlable=elements[1]

           
     
        
       
