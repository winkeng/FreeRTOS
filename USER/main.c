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

#define LOW_TASK_PRIO		    2      //�������ȼ�
#define LOW_STK_SIZE		    128    //�����ջ��С
TaskHandle_t LowTask_Handler;		   //������
void low_task(void *pvParameters);   //������

#define MIDDLE_TASK_PRIO	3      //�������ȼ�
#define MIDDLE_STK_SIZE		128    //�����ջ��С
TaskHandle_t MiddleTask_Handler;		   //������
void middle_task(void *pvParameters);   //������

#define HIGH_TASK_PRIO	4      //�������ȼ�
#define HIGH_STK_SIZE		128    //�����ջ��С
TaskHandle_t HighTask_Handler;		   //������
void high_task(void *pvParameters);   //������


//��ֵ�ź������
SemaphoreHandle_t MutexSemaphore;	//��ֵ�ź���



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
	
		//���������ź���
		MutexSemaphore=xSemaphoreCreateMutex();
	
    //����TASK1����
    xTaskCreate((TaskFunction_t )high_task,     	
                (const char*    )"high_task",
                (uint16_t       )HIGH_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )HIGH_TASK_PRIO,	
                (TaskHandle_t*  )&HighTask_Handler);    
    //����TASK1����
    xTaskCreate((TaskFunction_t )middle_task,     	
                (const char*    )"middle_task",
                (uint16_t       )MIDDLE_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )MIDDLE_TASK_PRIO,	
                (TaskHandle_t*  )&MiddleTask_Handler);    
    //����TASK1����
    xTaskCreate((TaskFunction_t )low_task,     	
                (const char*    )"low_task",
                (uint16_t       )LOW_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )LOW_TASK_PRIO,	
                (TaskHandle_t*  )&LowTask_Handler);    
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}


//�����ȼ������������
void high_task(void *pvParameters)
{	
	while(1)
	{	
		vTaskDelay(500); 
		printf("high task Pend Sem\r\n");
		xSemaphoreTake(MutexSemaphore,portMAX_DELAY);	//��ȡ�����ź���
		printf("high task Running!\r\n");
		xSemaphoreGive(MutexSemaphore);				//�ͷ��ź���
		vTaskDelay(500); 
	}
}

//�е����ȼ������������
void middle_task(void *pvParameters)
{
	while(1)
	{
		printf("middle task Running!\r\n");
		vTaskDelay(1000);                               //��ʱ1s��Ҳ����1000��ʱ�ӽ���	
	}
}

//�����ȼ������������
void low_task(void *pvParameters)
{
	static u32 times;

	while(1)
	{
		xSemaphoreTake(MutexSemaphore,portMAX_DELAY);	//��ȡ�����ź���
		printf("low task Running!\r\n");
		for(times=0;times<5000000;times++)				//ģ������ȼ�����ռ�ö�ֵ�ź���
		{
			taskYIELD();								//�����������
		}
		xSemaphoreGive(MutexSemaphore);				//�ͷŻ����ź���
		vTaskDelay(1000);	//��ʱ1s��Ҳ����1000��ʱ�ӽ���	
	}
}



















