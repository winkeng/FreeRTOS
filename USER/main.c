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
#include "timers.h"
#include "event_groups.h"

#define START_TASK_PRIO  1      		//�������ȼ�
#define START_STK_SIZE   128    		//�����ջ��С
TaskHandle_t StartTask_Handler;    		//������
void start_task(void *pvParameters);  	//������

#define EVENTSETBIT_TASK_PRIO		    2      //�������ȼ�
#define EVENTSETBIT_STK_SIZE		    256    //�����ջ��С
TaskHandle_t EventSetBit_Handler;		   //������
void eventsetbit_task(void *pvParameters);   //������

#define EVENTGROUP_TASK_PRIO		    3      //�������ȼ�
#define EVENTGROUP_STK_SIZE		    256    //�����ջ��С
TaskHandle_t EventGroupTask_Handler;		   //������
void eventgroup_task(void *pvParameters);   //������

#define EVENTQUERY_TASK_PRIO		    4      //�������ȼ�
#define EVENTQUERY_STK_SIZE		    256    //�����ջ��С
TaskHandle_t EventQueryTask_Handler;		   //������
void eventquery_task(void *pvParameters);   //������

////////////////////////////////////////////////////////
EventGroupHandle_t EventGroupHandler;	//�¼���־����


//�¼�λ	
#define EVENTBIT_1	(1<<1)
#define EVENTBIT_2	(1<<2)
#define EVENTBIT_ALL	(EVENTBIT_1|EVENTBIT_2)


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
	
			//�����¼���־��
		EventGroupHandler=xEventGroupCreate();	 //�����¼���־��
	
	  //���������¼�λ������
    xTaskCreate((TaskFunction_t )eventsetbit_task,             
                (const char*    )"eventsetbit_task",           
                (uint16_t       )EVENTSETBIT_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )EVENTSETBIT_TASK_PRIO,        
                (TaskHandle_t*  )&EventSetBit_Handler);   	
    //�����¼���־�鴦������
    xTaskCreate((TaskFunction_t )eventgroup_task,             
                (const char*    )"eventgroup_task",           
                (uint16_t       )EVENTGROUP_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )EVENTGROUP_TASK_PRIO,        
                (TaskHandle_t*  )&EventGroupTask_Handler);  
	  //�����¼���־���ѯ����
    xTaskCreate((TaskFunction_t )eventquery_task,             
                (const char*    )"eventquery_task",           
                (uint16_t       )EVENTQUERY_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )EVENTQUERY_TASK_PRIO,        
                (TaskHandle_t*  )&EventQueryTask_Handler);    
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

//�����¼�λ������
void eventsetbit_task(void *pvParameters)
{
	u8 key;
	while(1)
	{
		if(EventGroupHandler!=NULL)
		{
			key=key_scan();
			switch(key)
			{
				case 0x02:
					xEventGroupSetBits(EventGroupHandler,EVENTBIT_1);
					break;
				case 0x03:
					xEventGroupSetBits(EventGroupHandler,EVENTBIT_2);
					break;	
			}
		}
    
		vTaskDelay(50); //��ʱ50ms��Ҳ����10��ʱ�ӽ���
	}
}


//�¼���־�鴦������
void eventgroup_task(void *pvParameters)
{
	EventBits_t EventValue;
	
	while(1)
	{
		vTaskDelay(2000); //��ʱ2s
		
		if(EventGroupHandler!=NULL)
		{
			//�ȴ��¼����е���Ӧ�¼�λ
			EventValue=xEventGroupWaitBits((EventGroupHandle_t	)EventGroupHandler,		
										   (EventBits_t			)EVENTBIT_ALL,
										   (BaseType_t			)pdTRUE,				
										   (BaseType_t			)pdTRUE,
								       (TickType_t			)portMAX_DELAY);	
			printf("�¼���־���ֵ:%d\r\n",EventValue);
		}
	}
}


//�¼���ѯ����
void eventquery_task(void *pvParameters)
{	
	EventBits_t NewValue,LastValue;
	
	while(1)
	{
		if(EventGroupHandler!=NULL)
		{
			NewValue=xEventGroupGetBits(EventGroupHandler);	//��ȡ�¼����
			
			if(NewValue != LastValue)
			{
				LastValue=NewValue;
				printf("**�¼���־���ֵ:%d\r\n",NewValue);
			}
		}

		vTaskDelay(50); //��ʱ50ms��Ҳ����50��ʱ�ӽ���
	}
}















