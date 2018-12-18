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



#define START_TASK_PRIO  1      //�������ȼ�
#define START_STK_SIZE   128    //�����ջ��С
TaskHandle_t StartTask_Handler;     //������
void start_task(void *pvParameters);  //������

#define LED0_TASK_PRIO   2      //�������ȼ�
#define LED0_STK_SIZE    50     //�����ջ��С
TaskHandle_t LED0Task_Handler;  //������
void led0_task(void *pvParameters);  //������

#define LED1_TASK_PRIO   3      //�������ȼ�
#define LED1_STK_SIZE    50     //�����ջ��С
TaskHandle_t LED1Task_Handler;  //������
void led1_task(void *pvParameters);  //������


/****
	* @func main program
	*/
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4	
	delay_init();
	uart_init(115200);					//��ʼ������
	LED_Init();
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
    //����LED0����
    xTaskCreate((TaskFunction_t )led0_task,     	
                (const char*    )"led0_task",   	
                (uint16_t       )LED0_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )LED0_TASK_PRIO,	
                (TaskHandle_t*  )&LED0Task_Handler);   
    //����LED1����
    xTaskCreate((TaskFunction_t )led1_task,     
                (const char*    )"led1_task",   
                (uint16_t       )LED1_STK_SIZE, 
                (void*          )NULL,
                (UBaseType_t    )LED1_TASK_PRIO,
                (TaskHandle_t*  )&LED1Task_Handler);         
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

//LED0������
void led0_task(void *pvParameters)
{
	while(1)
	{
		LED12=~LED12;
		vTaskDelay(500);
	}
}

//LED1������
void led1_task(void *pvParameters)
{
	while(1)
	{
		LED12=1;
		vTaskDelay(200);
		LED13=0;
		vTaskDelay(800);
		LED13=1;
	}
}



















