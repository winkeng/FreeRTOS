#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"



#define KEY2()  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_13)//��ȡ����2 
#define KEY3()  GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_15)//��ȡ����3 

 


void KEY_Init(void);//IO��ʼ��
    
#endif
