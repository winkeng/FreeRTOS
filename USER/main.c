/**
  ******************************************************************************
  * @brief   FreeRTOS 学习
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

#define START_TASK_PRIO  1      		//任务优先级
#define START_STK_SIZE   128    		//任务堆栈大小
TaskHandle_t StartTask_Handler;    		//任务句柄
void start_task(void *pvParameters);  	//任务函数

#define TASK1_TASK_PRIO		    2      //任务优先级
#define TASK1_STK_SIZE		    128    //任务堆栈大小
TaskHandle_t Task1Task_Handler;		   //任务句柄
void task1_task(void *pvParameters);   //任务函数

#define KEYPROCESS_TASK_PRIO	3      //任务优先级
#define KEYPROCESS_STK_SIZE		128    //任务堆栈大小
TaskHandle_t KeyProcess_Handler;		   //任务句柄
void KeyProcess_task(void *pvParameters);   //任务函数


//按键消息队列的数量
#define KEYMSG_Q_NUM	1		//按键消息队列的数量
#define MESSAGE_Q_NUM	4		//发送数据的消息队列的数量


/****
	* @func main program
	*/
int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4	
	delay_init();
	uart_init(115200);					//初始化串口
	KEY_Init();
	LED_Init();
	//创建开始任务函数
	xTaskCreate( (TaskFunction_t )start_task,         //任务函数
							 (const char*    )"start_task",       //任务名称
							 (uint16_t       )START_STK_SIZE,     //任务堆栈大小
							 (void*          )NULL,               //传递给任务函数的参数
							 (UBaseType_t    )START_TASK_PRIO,    //任务优先级
							 (TaskHandle_t*  )&StartTask_Handler  //任务句柄
						 );
	vTaskStartScheduler(); //开启任务调度
}




//开始任务函数
void start_task(void *pvParameters)
{
    taskENTER_CRITICAL();           //进入临界区
	
    //创建TASK1任务
    xTaskCreate((TaskFunction_t )task1_task,     	
                (const char*    )"task1_task",
                (uint16_t       )TASK1_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )TASK1_TASK_PRIO,	
                (TaskHandle_t*  )&Task1Task_Handler);    
    //创建TASK1任务
    xTaskCreate((TaskFunction_t )KeyProcess_task,     	
                (const char*    )"KeyProcess_task",
                (uint16_t       )KEYPROCESS_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )KEYPROCESS_TASK_PRIO,	
                (TaskHandle_t*  )&KeyProcess_Handler);    
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}


//task1 任务函数 
void task1_task(void* pvParameters)
{
	u8 key;
	BaseType_t err;
	
	while(1)
	{
		key = key_scan();

		if((KeyProcess_Handler!=NULL) && (key != 0x00))   
		{
			err=xTaskNotify((TaskHandle_t	)KeyProcess_Handler,		    //接收任务通知的任务句柄
											(uint32_t		)key,						              //任务通知值
											(eNotifyAction	)eSetValueWithOverwrite);	//覆写的方式发送任务通知			
			if(err == pdFAIL)
			{
				printf("任务通知发送失败\r\n");
			}
		}
		
		vTaskDelay(200);
	}
	
}

//KeyProcess_task 任务函数 
void KeyProcess_task(void* pvParameters)
{
	BaseType_t err;
	uint32_t NotifyValue;
	
	while(1)
	{
		err=xTaskNotifyWait((uint32_t	)0x00,	            //进入函数的时候不清除任务bit
							          (uint32_t	)ULONG_MAX,			    //退出函数的时候清除所有的bit
							          (uint32_t*	)&NotifyValue,		//保存任务通知值
							          (TickType_t	)portMAX_DELAY);	//阻塞时间		
		
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



















