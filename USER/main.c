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

#define SEMAPGIVE_TASK_PRIO		    2      //�������ȼ�
#define SEMAPGIVE_STK_SIZE		    128    //�����ջ��С
TaskHandle_t SemapGiveTask_Handler;		   //������
void SemapGive_task(void *pvParameters);   //������

#define SEMAPTAKE_TASK_PRIO	3      //�������ȼ�
#define SEMAPTAKE_STK_SIZE		128    //�����ջ��С
TaskHandle_t SemapTakeTask_Handler;		   //������
void SemapTake_task(void *pvParameters);   //������


SemaphoreHandle_t CountSemaphore;//�������ź���



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
	
		//�����������ź���
		CountSemaphore=xSemaphoreCreateCounting(255, 0);		
	
    //����TASK1����
    xTaskCreate((TaskFunction_t )SemapGive_task,     	
                (const char*    )"SemapGive_task",
                (uint16_t       )SEMAPGIVE_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )SEMAPGIVE_TASK_PRIO,	
                (TaskHandle_t*  )&SemapGiveTask_Handler);    
    //����TASK1����
    xTaskCreate((TaskFunction_t )SemapTake_task,     	
                (const char*    )"SemapTake_task",
                (uint16_t       )SEMAPTAKE_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )SEMAPTAKE_TASK_PRIO,	
                (TaskHandle_t*  )&SemapTakeTask_Handler);    
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}


//�ͷż������ź���������
void SemapGive_task(void* pvParameters)
{
	uint8_t key;
	BaseType_t err;
	
	while(1)
	{
		key = key_scan();
		
		if(CountSemaphore != NULL)
		{
			if(key == 0x02)
			{
				err = xSemaphoreGive(CountSemaphore);//�ͷż������ź���
				if(err == pdFALSE)
				{
					printf("�ź����ͷ�ʧ��!!!\r\n");
				}				
			}
			
			vTaskDelay(200); 
		}
	}
	
}

//��ȡ�������ź���������
void SemapTake_task(void* pvParameters)
{
	uint8_t num;
	uint8_t semavalue;

	while(1)
	{
		xSemaphoreTake(CountSemaphore,portMAX_DELAY); 	//�ȴ���ֵ�ź���
		num++;
		semavalue=uxSemaphoreGetCount(CountSemaphore); 	//��ȡ��ֵ�ź���ֵ		
		printf("semavalue=%d\r\n", semavalue);
		vTaskDelay(1000);                               //��ʱ1s��Ҳ����1000��ʱ�ӽ���	
	}
}



















