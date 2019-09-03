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

#define START_TASK_PRIO  1      		//�������ȼ�
#define START_STK_SIZE   128    		//�����ջ��С
TaskHandle_t StartTask_Handler;    		//������
void start_task(void *pvParameters);  	//������

#define TIMERCONTROL_TASK_PRIO		    2      //�������ȼ�
#define TIMERCONTROL_STK_SIZE		    128    //�����ջ��С
TaskHandle_t TimerControlTask_Handler;		   //������
void timercontrol_task(void *pvParameters);   //������



TimerHandle_t AutoReloadTimer_Handle;		//���ڶ�ʱ�����
TimerHandle_t	OneShotTimer_Handle;			//���ζ�ʱ�����

void AutoReloadCallback(TimerHandle_t xTimer); 	//���ڶ�ʱ���ص�����
void OneShotCallback(TimerHandle_t xTimer);		  //���ζ�ʱ���ص�����


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
	
    //����������ڶ�ʱ��
    AutoReloadTimer_Handle=xTimerCreate((const char*		)"AutoReloadTimer",
									        (TickType_t			)1000,
							            (UBaseType_t		)pdTRUE,
							            (void*				)1,
							            (TimerCallbackFunction_t)AutoReloadCallback); //���ڶ�ʱ��������1s(1000��ʱ�ӽ���)������ģʽ
		//�������ζ�ʱ��
		OneShotTimer_Handle=xTimerCreate((const char*			)"OneShotTimer",
												 (TickType_t			)2000,
												 (UBaseType_t			)pdFALSE,
												 (void*					)2,
												 (TimerCallbackFunction_t)OneShotCallback); //���ζ�ʱ��������2s(2000��ʱ�ӽ���)������ģʽ					  
		//������ʱ����������
		xTaskCreate((TaskFunction_t )timercontrol_task,             
								(const char*    )"timercontrol_task",           
								(uint16_t       )TIMERCONTROL_STK_SIZE,        
								(void*          )NULL,                  
								(UBaseType_t    )TIMERCONTROL_TASK_PRIO,        
								(TaskHandle_t*  )&TimerControlTask_Handler);    
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}

//TimerControl��������
void timercontrol_task(void *pvParameters)
{
	u8 key;
	
	while(1)
	{
		//ֻ��������ʱ���������ɹ��˲��ܶ�����в���
		if((AutoReloadTimer_Handle!=NULL)&&(OneShotTimer_Handle!=NULL))
		{
			key = key_scan();
			switch(key)
			{
				case 0x02:     //��key_up���µĻ������ڶ�ʱ��
					xTimerStart(AutoReloadTimer_Handle,0);	//�������ڶ�ʱ��
					printf("������ʱ��1\r\n");
					break;
				
				case 0x03:		//��key0���µĻ��򿪵��ζ�ʱ��
					xTimerStart(OneShotTimer_Handle,0);		//�������ζ�ʱ��
					printf("������ʱ��2\r\n");
					break;
				
				case 0x01:		//��key1���»��͹رն�ʱ��
					xTimerStop(AutoReloadTimer_Handle,0); 	//�ر����ڶ�ʱ��
					xTimerStop(OneShotTimer_Handle,0); 		  //�رյ��ζ�ʱ��
					printf("�رն�ʱ��1��2\r\n");
					break;	
			}
		}

    vTaskDelay(200); //��ʱ10ms��Ҳ����10��ʱ�ӽ���
	}
}


//���ڶ�ʱ���Ļص�����
void AutoReloadCallback(TimerHandle_t xTimer)
{
	printf("���ڶ�ʱ������\r\n");
}

//���ζ�ʱ���Ļص�����
void OneShotCallback(TimerHandle_t xTimer)
{
  printf("���ζ�ʱ������\r\n");
}



















