#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"
								    
//������ʼ������
//key2 - PB13  key3 - PC15
void KEY_Init(void) //IO��ʼ��
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC,ENABLE);

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

