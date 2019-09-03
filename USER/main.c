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
#include "timers.h"
#include "event_groups.h"

#define START_TASK_PRIO  1      		//任务优先级
#define START_STK_SIZE   128    		//任务堆栈大小
TaskHandle_t StartTask_Handler;    		//任务句柄
void start_task(void *pvParameters);  	//任务函数

#define EVENTSETBIT_TASK_PRIO		    2      //任务优先级
#define EVENTSETBIT_STK_SIZE		    256    //任务堆栈大小
TaskHandle_t EventSetBit_Handler;		   //任务句柄
void eventsetbit_task(void *pvParameters);   //任务函数

#define EVENTGROUP_TASK_PRIO		    3      //任务优先级
#define EVENTGROUP_STK_SIZE		    256    //任务堆栈大小
TaskHandle_t EventGroupTask_Handler;		   //任务句柄
void eventgroup_task(void *pvParameters);   //任务函数

#define EVENTQUERY_TASK_PRIO		    4      //任务优先级
#define EVENTQUERY_STK_SIZE		    256    //任务堆栈大小
TaskHandle_t EventQueryTask_Handler;		   //任务句柄
void eventquery_task(void *pvParameters);   //任务函数

////////////////////////////////////////////////////////
EventGroupHandle_t EventGroupHandler;	//事件标志组句柄


//事件位	
#define EVENTBIT_1	(1<<1)
#define EVENTBIT_2	(1<<2)
#define EVENTBIT_ALL	(EVENTBIT_1|EVENTBIT_2)


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
	
			//创建事件标志组
		EventGroupHandler=xEventGroupCreate();	 //创建事件标志组
	
	  //创建设置事件位的任务
    xTaskCreate((TaskFunction_t )eventsetbit_task,             
                (const char*    )"eventsetbit_task",           
                (uint16_t       )EVENTSETBIT_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )EVENTSETBIT_TASK_PRIO,        
                (TaskHandle_t*  )&EventSetBit_Handler);   	
    //创建事件标志组处理任务
    xTaskCreate((TaskFunction_t )eventgroup_task,             
                (const char*    )"eventgroup_task",           
                (uint16_t       )EVENTGROUP_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )EVENTGROUP_TASK_PRIO,        
                (TaskHandle_t*  )&EventGroupTask_Handler);  
	  //创建事件标志组查询任务
    xTaskCreate((TaskFunction_t )eventquery_task,             
                (const char*    )"eventquery_task",           
                (uint16_t       )EVENTQUERY_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )EVENTQUERY_TASK_PRIO,        
                (TaskHandle_t*  )&EventQueryTask_Handler);    
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//设置事件位的任务
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
    
		vTaskDelay(50); //延时50ms，也就是10个时钟节拍
	}
}


//事件标志组处理任务
void eventgroup_task(void *pvParameters)
{
	EventBits_t EventValue;
	
	while(1)
	{
		vTaskDelay(2000); //延时2s
		
		if(EventGroupHandler!=NULL)
		{
			//等待事件组中的相应事件位
			EventValue=xEventGroupWaitBits((EventGroupHandle_t	)EventGroupHandler,		
										   (EventBits_t			)EVENTBIT_ALL,
										   (BaseType_t			)pdTRUE,				
										   (BaseType_t			)pdTRUE,
								       (TickType_t			)portMAX_DELAY);	
			printf("事件标志组的值:%d\r\n",EventValue);
		}
	}
}


//事件查询任务
void eventquery_task(void *pvParameters)
{	
	EventBits_t NewValue,LastValue;
	
	while(1)
	{
		if(EventGroupHandler!=NULL)
		{
			NewValue=xEventGroupGetBits(EventGroupHandler);	//获取事件组的
			
			if(NewValue != LastValue)
			{
				LastValue=NewValue;
				printf("**事件标志组的值:%d\r\n",NewValue);
			}
		}

		vTaskDelay(50); //延时50ms，也就是50个时钟节拍
	}
}















