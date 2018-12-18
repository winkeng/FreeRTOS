#include "led.h"
#include "delay.h"

static uint8_t toggle_cnt=0;
	
/****
	* @func	LED管脚初始化	
	*/
void LED_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
		
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);	 

	/*LED灯初始化*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	
	GPIO_SetBits(GPIOB, GPIO_Pin_12); //关闭LED						 
	GPIO_SetBits(GPIOC, GPIO_Pin_13); //关闭LED						 
}
 
void LED_Toggle(void)
{
	for(toggle_cnt=0;toggle_cnt<5;toggle_cnt++)
	{
			GPIO_ResetBits(GPIOB,GPIO_Pin_12);
			delay_ms(200);
			GPIO_SetBits(GPIOB,GPIO_Pin_12);
			delay_ms(200);
	}
			toggle_cnt=0;
}
