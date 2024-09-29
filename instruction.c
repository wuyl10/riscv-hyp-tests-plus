#include <stdint.h>
#include <stdio.h>
#include <rvh_test.h>
#include <instructions.h> 
#include <csrs.h>

void random_m_instruction(){
    int rand1=CSRR(time);
    int rand2=CSRR(cycle);
    int num0=rand1+rand2;
    int num=num0%10;

    switch (num)
    {
    case 0:
        printf("这是0号指令\n");
        break;
    case 1:
        printf("这是1号指令\n");
        break;
    case 2:
        printf("这是2号指令\n");
        break;
    case 3:
        printf("这是3号指令\n");
        break;
    case 4:
        printf("这是4号指令\n");
        break;
    case 5:
        printf("这是5号指令\n");
        break;
    case 6:
        printf("这是6号指令\n");
        break;
    case 7:
        printf("这是7号指令\n");
        break;
    case 8:
        printf("这是8号指令\n");
        break;
    case 9:
        printf("这是9号指令\n");
        break;
    
    
    default:
        break;
    }
}