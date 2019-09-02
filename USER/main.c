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


#define START_TASK_PRIO  1      		//任务优先级
#define START_STK_SIZE   128    		//任务堆栈大小
TaskHandle_t StartTask_Handler;    		//任务句柄
void start_task(void *pvParameters);  	//任务函数

#define SEMAPGIVE_TASK_PRIO		    2      //任务优先级
#define SEMAPGIVE_STK_SIZE		    128    //任务堆栈大小
TaskHandle_t SemapGiveTask_Handler;		   //任务句柄
void SemapGive_task(void *pvParameters);   //任务函数

#define SEMAPTAKE_TASK_PRIO	3      //任务优先级
#define SEMAPTAKE_STK_SIZE		128    //任务堆栈大小
TaskHandle_t SemapTakeTask_Handler;		   //任务句柄
void SemapTake_task(void *pvParameters);   //任务函数


SemaphoreHandle_t CountSemaphore;//计数型信号量



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
	
	printf("create start task!!!\r\n");
	
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
	
		//创建计数型信号量
		CountSemaphore=xSemaphoreCreateCounting(255, 0);		
	
    //创建TASK1任务
    xTaskCreate((TaskFunction_t )SemapGive_task,     	
                (const char*    )"SemapGive_task",
                (uint16_t       )SEMAPGIVE_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )SEMAPGIVE_TASK_PRIO,	
                (TaskHandle_t*  )&SemapGiveTask_Handler);    
    //创建TASK1任务
    xTaskCreate((TaskFunction_t )SemapTake_task,     	
                (const char*    )"SemapTake_task",
                (uint16_t       )SEMAPTAKE_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )SEMAPTAKE_TASK_PRIO,	
                (TaskHandle_t*  )&SemapTakeTask_Handler);    
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}


//释放计数型信号量任务函数
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
				err = xSemaphoreGive(CountSemaphore);//释放计数型信号量
				if(err == pdFALSE)
				{
					printf("信号量释放失败!!!\r\n");
				}				
			}
			
			vTaskDelay(200); 
		}
	}
	
}

//获取计数型信号量任务函数
void SemapTake_task(void* pvParameters)
{
	uint8_t num;
	uint8_t semavalue;

	while(1)
	{
		xSemaphoreTake(CountSemaphore,portMAX_DELAY); 	//等待数值信号量
		num++;
		semavalue=uxSemaphoreGetCount(CountSemaphore); 	//获取数值信号量值		
		printf("semavalue=%d\r\n", semavalue);
		vTaskDelay(1000);                               //延时1s，也就是1000个时钟节拍	
	}
}



















