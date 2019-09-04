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
#include "limits.h"

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
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

//�����¼�λ������
void eventsetbit_task(void *pvParameters)
{
	u8 key;
	
	while(1)
	{
		if(EventGroupTask_Handler!=NULL)
		{
			key=key_scan();
			switch(key)
			{
				case 0x02:
					xTaskNotify((TaskHandle_t	)EventGroupTask_Handler,//��������֪ͨ��������
								      (uint32_t		)EVENTBIT_1,			        //Ҫ���µ�bit
								      (eNotifyAction	)eSetBits);				    //����ָ����bit
					break;
				case 0x03:
					xTaskNotify((TaskHandle_t	)EventGroupTask_Handler,//��������֪ͨ��������
								      (uint32_t		)EVENTBIT_2,			        //Ҫ���µ�bit
								      (eNotifyAction	)eSetBits);				    //����ָ����bit
					break;	
			}
		}
    
		vTaskDelay(200); //��ʱ200ms��Ҳ����10��ʱ�ӽ���
	}
}


//�¼���־�鴦������
void eventgroup_task(void *pvParameters)
{
	BaseType_t err;
	uint32_t NotifyValue;
	
	while(1)
	{
		//��ȡ����ֵ֪ͨ
		err=xTaskNotifyWait((uint32_t	)0x00,    //���뺯����ʱ���������bit
							(uint32_t	)ULONG_MAX,			    //�˳�������ʱ��������е�bit
							(uint32_t*	)&NotifyValue,		//��������ֵ֪ͨ
							(TickType_t	)portMAX_DELAY);	//����ʱ��
		
		if(err==pdPASS)	   //����֪ͨ��ȡ�ɹ�
		{
			if((NotifyValue&EVENTBIT_1)!=0)	//�¼�1����	
			{
				printf("�¼�1����\r\n");
			}
			else if((NotifyValue&EVENTBIT_2)!=0)	//�¼�2����	
			{
				printf("�¼�2����\r\n");
			}
		}
	}
}
















