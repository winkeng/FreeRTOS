/**
  ******************************************************************************
  * @brief   FreeRTOS ѧϰ
  * @date    2018/12/18
  ******************************************************************************/

#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "timer.h"



#define START_TASK_PRIO  1      //�������ȼ�
#define START_STK_SIZE   256    //�����ջ��С
TaskHandle_t StartTask_Handler;     //������
void start_task(void *pvParameters);  //������

#define INTERRUPT_TASK_PRIO   2      //�������ȼ�
#define INTERRUPT_STK_SIZE    256    //�����ջ��С
TaskHandle_t INTERRUPTTask_Handler;  //������
void INTERRUPT_task(void *p_arg);    //������


/****
	* @func main program
	*/
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4	
	delay_init();
	uart_init(115200);					//��ʼ������
	LED_Init();
	TIM3_Int_Init(10000-1,7200-1);		//��ʼ����ʱ��3����ʱ������1S
	TIM4_Int_Init(10000-1,7200-1);		//��ʼ����ʱ��5����ʱ������1S		
	//������ʼ������
	xTaskCreate( (TaskFunction_t )start_task,         //������
							 (const char*    )"start_task",       //��������
							 (uint16_t       )START_STK_SIZE,     //�����ջ��С
							 (void*          )NULL,               //���ݸ��������Ĳ���
							 (UBaseType_t    )START_TASK_PRIO,    //�������ȼ�
							 (TaskHandle_t*  )&StartTask_Handler  //������
						 );
	vTaskStartScheduler(); //�����������
}




//��ʼ������
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //�����ٽ���
    //�����жϲ�������
    xTaskCreate((TaskFunction_t )INTERRUPT_task,     	
                (const char*    )"INTERRUPT_task",
                (uint16_t       )INTERRUPT_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )INTERRUPT_TASK_PRIO,	
                (TaskHandle_t*  )&INTERRUPTTask_Handler);    
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}


//�жϲ���������
void INTERRUPT_task(void *pvParameters)
{
	static u32 total_num=0;
	while(1)
	{
		total_num+=1;
		if(total_num==5)
		{
			printf("�ر��ж�.............\r\n");
			portDISABLE_INTERRUPTS();				//�ر��ж�
			delay_xms(5000);						//��ʱ5s
			printf("���ж�.............\r\n");	//���ж�
			portENABLE_INTERRUPTS();			
		}
        LED12=~LED12;
        vTaskDelay(1000);		
	}
}



















