#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"



#define KEY2()  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)//读取按键2 
#define KEY3()  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_15)//读取按键3 

 


void KEY_Init(void);//IO初始化
    
#endif
