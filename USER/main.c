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
#include "limits.h"

#define START_TASK_PRIO  1      		//�������ȼ�
#define START_STK_SIZE   128    		//�����ջ��С
TaskHandle_t StartTask_Handler;    		//������
void start_task(void *pvParameters);  	//������

#define TASK1_TASK_PRIO		    2      //�������ȼ�
#define TASK1_STK_SIZE		    128    //�����ջ��С
TaskHandle_t Task1Task_Handler;		   //������
void task1_task(void *pvParameters);   //������

#define KEYPROCESS_TASK_PRIO	3      //�������ȼ�
#define KEYPROCESS_STK_SIZE		128    //�����ջ��С
TaskHandle_t KeyProcess_Handler;		   //������
void KeyProcess_task(void *pvParameters);   //������


//������Ϣ���е�����
#define KEYMSG_Q_NUM	1		//������Ϣ���е�����
#define MESSAGE_Q_NUM	4		//�������ݵ���Ϣ���е�����


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
    xTaskCreate((TaskFunction_t )KeyProcess_task,     	
                (const char*    )"KeyProcess_task",
                (uint16_t       )KEYPROCESS_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )KEYPROCESS_TASK_PRIO,	
                (TaskHandle_t*  )&KeyProcess_Handler);    
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}


//task1 ������ 
void task1_task(void* pvParameters)
{
	u8 key;
	BaseType_t err;
	
	while(1)
	{
		key = key_scan();

		if((KeyProcess_Handler!=NULL) && (key != 0x00))   
		{
			err=xTaskNotify((TaskHandle_t	)KeyProcess_Handler,		    //��������֪ͨ��������
											(uint32_t		)key,						              //����ֵ֪ͨ
											(eNotifyAction	)eSetValueWithOverwrite);	//��д�ķ�ʽ��������֪ͨ			
			if(err == pdFAIL)
			{
				printf("����֪ͨ����ʧ��\r\n");
			}
		}
		
		vTaskDelay(200);
	}
	
}

//KeyProcess_task ������ 
void KeyProcess_task(void* pvParameters)
{
	BaseType_t err;
	uint32_t NotifyValue;
	
	while(1)
	{
		err=xTaskNotifyWait((uint32_t	)0x00,	            //���뺯����ʱ���������bit
							          (uint32_t	)ULONG_MAX,			    //�˳�������ʱ��������е�bit
							          (uint32_t*	)&NotifyValue,		//��������ֵ֪ͨ
							          (TickType_t	)portMAX_DELAY);	//����ʱ��		
		
		if(err == pdTRUE)
		{
				switch(NotifyValue)
				{
					case 0x02: printf("key 2\r\n"); break;
					
					case 0x03: printf("key 3\r\n"); break;
				}
		}
		
		vTaskDelay(10);
	}
}



















