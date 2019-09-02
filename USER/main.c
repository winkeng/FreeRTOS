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

#define LOW_TASK_PRIO		    2      //任务优先级
#define LOW_STK_SIZE		    128    //任务堆栈大小
TaskHandle_t LowTask_Handler;		   //任务句柄
void low_task(void *pvParameters);   //任务函数

#define MIDDLE_TASK_PRIO	3      //任务优先级
#define MIDDLE_STK_SIZE		128    //任务堆栈大小
TaskHandle_t MiddleTask_Handler;		   //任务句柄
void middle_task(void *pvParameters);   //任务函数

#define HIGH_TASK_PRIO	4      //任务优先级
#define HIGH_STK_SIZE		128    //任务堆栈大小
TaskHandle_t HighTask_Handler;		   //任务句柄
void high_task(void *pvParameters);   //任务函数


//二值信号量句柄
SemaphoreHandle_t MutexSemaphore;	//二值信号量



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
	
		//创建互斥信号量
		MutexSemaphore=xSemaphoreCreateMutex();
	
    //创建TASK1任务
    xTaskCreate((TaskFunction_t )high_task,     	
                (const char*    )"high_task",
                (uint16_t       )HIGH_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )HIGH_TASK_PRIO,	
                (TaskHandle_t*  )&HighTask_Handler);    
    //创建TASK1任务
    xTaskCreate((TaskFunction_t )middle_task,     	
                (const char*    )"middle_task",
                (uint16_t       )MIDDLE_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )MIDDLE_TASK_PRIO,	
                (TaskHandle_t*  )&MiddleTask_Handler);    
    //创建TASK1任务
    xTaskCreate((TaskFunction_t )low_task,     	
                (const char*    )"low_task",
                (uint16_t       )LOW_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )LOW_TASK_PRIO,	
                (TaskHandle_t*  )&LowTask_Handler);    
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}


//高优先级任务的任务函数
void high_task(void *pvParameters)
{	
	while(1)
	{	
		vTaskDelay(500); 
		printf("high task Pend Sem\r\n");
		xSemaphoreTake(MutexSemaphore,portMAX_DELAY);	//获取互斥信号量
		printf("high task Running!\r\n");
		xSemaphoreGive(MutexSemaphore);				//释放信号量
		vTaskDelay(500); 
	}
}

//中等优先级任务的任务函数
void middle_task(void *pvParameters)
{
	while(1)
	{
		printf("middle task Running!\r\n");
		vTaskDelay(1000);                               //延时1s，也就是1000个时钟节拍	
	}
}

//低优先级任务的任务函数
void low_task(void *pvParameters)
{
	static u32 times;

	while(1)
	{
		xSemaphoreTake(MutexSemaphore,portMAX_DELAY);	//获取互斥信号量
		printf("low task Running!\r\n");
		for(times=0;times<5000000;times++)				//模拟低优先级任务占用二值信号量
		{
			taskYIELD();								//发起任务调度
		}
		xSemaphoreGive(MutexSemaphore);				//释放互斥信号量
		vTaskDelay(1000);	//延时1s，也就是1000个时钟节拍	
	}
}



















