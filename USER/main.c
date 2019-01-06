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



#define START_TASK_PRIO  1      		//�������ȼ�
#define START_STK_SIZE   128    		//�����ջ��С
TaskHandle_t StartTask_Handler;    		//������
void start_task(void *pvParameters);  	//������

#define TASK1_TASK_PRIO		    2      //�������ȼ�
#define TASK1_STK_SIZE		    128    //�����ջ��С
TaskHandle_t Task1Task_Handler;		   //������
void task1_task(void *pvParameters);   //������

#define TASK2_TASK_PRIO		    3      //�������ȼ�
#define TASK2_STK_SIZE		    128    //�����ջ��С
TaskHandle_t Task2Task_Handler;		   //������
void task2_task(void *pvParameters);   //������


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
    //����TASK1����
    xTaskCreate((TaskFunction_t )task1_task,     	
                (const char*    )"task1_task",
                (uint16_t       )TASK1_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )TASK1_TASK_PRIO,	
                (TaskHandle_t*  )&Task1Task_Handler);    
    //����TASK1����
    xTaskCreate((TaskFunction_t )task2_task,     	
                (const char*    )"task2_task",
                (uint16_t       )TASK2_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )TASK2_TASK_PRIO,	
                (TaskHandle_t*  )&Task2Task_Handler);    
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}


//task1 ������ 
void task1_task(void* pvParameters)
{
	u8 task1_num=0;
	while(1)
	{
		task1_num++;
		LED12 = !LED12;
		printf("����1��ִ�У�%d\r\n",task1_num);
		
		if(task1_num==5)
		{
			vTaskDelete(Task2Task_Handler); //����1ִ��5��ɾ������2
			printf("����1ɾ��������2!!!\r\n");
		}
		vTaskDelay(1000);  //��ʱ1s
	}
}

//task2 ������ 
void task2_task(void* pvParameters)
{
	u8 task2_num=0;
	while(1)
	{
		task2_num++;
		LED13 = !LED13;
		printf("����2��ִ�У�%d\r\n",task2_num);
		
		vTaskDelay(1000);  //��ʱ1s
	}
}



















