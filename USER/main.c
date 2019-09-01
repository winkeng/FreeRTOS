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
#include "semphr.h"
#include "key.h"
#include "string.h"


#define START_TASK_PRIO  1      		//�������ȼ�
#define START_STK_SIZE   128    		//�����ջ��С
TaskHandle_t StartTask_Handler;    		//������
void start_task(void *pvParameters);  	//������

#define TASK1_TASK_PRIO		    2      //�������ȼ�
#define TASK1_STK_SIZE		    128    //�����ջ��С
TaskHandle_t Task1Task_Handler;		   //������
void task1_task(void *pvParameters);   //������

#define DATAPROCESS_TASK_PRIO	3      //�������ȼ�
#define DATAPROCESS_STK_SIZE		128    //�����ջ��С
TaskHandle_t DataProcess_Handler;		   //������
void DataProcess_task(void *pvParameters);   //������


SemaphoreHandle_t BinarySemaphore;	//��ֵ�ź������


extern u8 USART_RX_BUF[USART_REC_LEN];

/****
	* @func main program
	*/
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4	
	delay_init();
	uart_init(115200);					//��ʼ������
	KEY_Init();
	LED_Init();
	
	printf("create start task!!!\r\n");
	
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
	
		//������ֵ�ź���
		BinarySemaphore=xSemaphoreCreateBinary();		
	
    //����TASK1����
    xTaskCreate((TaskFunction_t )task1_task,     	
                (const char*    )"task1_task",
                (uint16_t       )TASK1_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )TASK1_TASK_PRIO,	
                (TaskHandle_t*  )&Task1Task_Handler);    
    //����TASK1����
    xTaskCreate((TaskFunction_t )DataProcess_task,     	
                (const char*    )"DataProcess_task",
                (uint16_t       )DATAPROCESS_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )DATAPROCESS_TASK_PRIO,	
                (TaskHandle_t*  )&DataProcess_Handler);    
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}


//task1 ������ 
void task1_task(void* pvParameters)
{
	while(1)
	{
		LED1_Toggle();
		vTaskDelay(500);
	}
	
}

//DataProcess_task ������ 
void DataProcess_task(void* pvParameters)
{
	BaseType_t err=pdFALSE;

	while(1)
	{
		if(BinarySemaphore!=NULL)
		{
			err=xSemaphoreTake(BinarySemaphore,portMAX_DELAY);	//��ȡ�ź���
			if(err==pdTRUE)										//��ȡ�ź����ɹ�
			{
				if(strncmp((char *)USART_RX_BUF, "LED", strlen("LED")) == 0)
				{
					printf("LED CMD\r\n");
				}
				else if(strncmp((char *)USART_RX_BUF, "KEY", strlen("KEY")) == 0)
				{
					printf("KEY CMD\r\n");
				}
				else
				{
					printf("Invalid CMD!!!\r\n");
				}
			}

			USART_RX_STA=0;
			memset(USART_RX_BUF,0,USART_REC_LEN);			//���ڽ��ջ���������
		}
		else if(err==pdFALSE)
		{
			vTaskDelay(10);      //��ʱ10ms��Ҳ����10��ʱ�ӽ���	
		}
	}
}



















