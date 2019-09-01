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

#define TASK1_TASK_PRIO		    2      //任务优先级
#define TASK1_STK_SIZE		    128    //任务堆栈大小
TaskHandle_t Task1Task_Handler;		   //任务句柄
void task1_task(void *pvParameters);   //任务函数

#define DATAPROCESS_TASK_PRIO	3      //任务优先级
#define DATAPROCESS_STK_SIZE		128    //任务堆栈大小
TaskHandle_t DataProcess_Handler;		   //任务句柄
void DataProcess_task(void *pvParameters);   //任务函数


SemaphoreHandle_t BinarySemaphore;	//二值信号量句柄


extern u8 USART_RX_BUF[USART_REC_LEN];

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
	
		//创建二值信号量
		BinarySemaphore=xSemaphoreCreateBinary();		
	
    //创建TASK1任务
    xTaskCreate((TaskFunction_t )task1_task,     	
                (const char*    )"task1_task",
                (uint16_t       )TASK1_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )TASK1_TASK_PRIO,	
                (TaskHandle_t*  )&Task1Task_Handler);    
    //创建TASK1任务
    xTaskCreate((TaskFunction_t )DataProcess_task,     	
                (const char*    )"DataProcess_task",
                (uint16_t       )DATAPROCESS_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )DATAPROCESS_TASK_PRIO,	
                (TaskHandle_t*  )&DataProcess_Handler);    
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}


//task1 任务函数 
void task1_task(void* pvParameters)
{
	while(1)
	{
		LED1_Toggle();
		vTaskDelay(500);
	}
	
}

//DataProcess_task 任务函数 
void DataProcess_task(void* pvParameters)
{
	BaseType_t err=pdFALSE;

	while(1)
	{
		if(BinarySemaphore!=NULL)
		{
			err=xSemaphoreTake(BinarySemaphore,portMAX_DELAY);	//获取信号量
			if(err==pdTRUE)										//获取信号量成功
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
			memset(USART_RX_BUF,0,USART_REC_LEN);			//串口接收缓冲区清零
		}
		else if(err==pdFALSE)
		{
			vTaskDelay(10);      //延时10ms，也就是10个时钟节拍	
		}
	}
}



















