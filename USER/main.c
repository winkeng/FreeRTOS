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

#define START_TASK_PRIO  1      		//任务优先级
#define START_STK_SIZE   128    		//任务堆栈大小
TaskHandle_t StartTask_Handler;    		//任务句柄
void start_task(void *pvParameters);  	//任务函数

#define TIMERCONTROL_TASK_PRIO		    2      //任务优先级
#define TIMERCONTROL_STK_SIZE		    128    //任务堆栈大小
TaskHandle_t TimerControlTask_Handler;		   //任务句柄
void timercontrol_task(void *pvParameters);   //任务函数



TimerHandle_t AutoReloadTimer_Handle;		//周期定时器句柄
TimerHandle_t	OneShotTimer_Handle;			//单次定时器句柄

void AutoReloadCallback(TimerHandle_t xTimer); 	//周期定时器回调函数
void OneShotCallback(TimerHandle_t xTimer);		  //单次定时器回调函数


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
	
    //创建软件周期定时器
    AutoReloadTimer_Handle=xTimerCreate((const char*		)"AutoReloadTimer",
									        (TickType_t			)1000,
							            (UBaseType_t		)pdTRUE,
							            (void*				)1,
							            (TimerCallbackFunction_t)AutoReloadCallback); //周期定时器，周期1s(1000个时钟节拍)，周期模式
		//创建单次定时器
		OneShotTimer_Handle=xTimerCreate((const char*			)"OneShotTimer",
												 (TickType_t			)2000,
												 (UBaseType_t			)pdFALSE,
												 (void*					)2,
												 (TimerCallbackFunction_t)OneShotCallback); //单次定时器，周期2s(2000个时钟节拍)，单次模式					  
		//创建定时器控制任务
		xTaskCreate((TaskFunction_t )timercontrol_task,             
								(const char*    )"timercontrol_task",           
								(uint16_t       )TIMERCONTROL_STK_SIZE,        
								(void*          )NULL,                  
								(UBaseType_t    )TIMERCONTROL_TASK_PRIO,        
								(TaskHandle_t*  )&TimerControlTask_Handler);    
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//TimerControl的任务函数
void timercontrol_task(void *pvParameters)
{
	u8 key;
	
	while(1)
	{
		//只有两个定时器都创建成功了才能对其进行操作
		if((AutoReloadTimer_Handle!=NULL)&&(OneShotTimer_Handle!=NULL))
		{
			key = key_scan();
			switch(key)
			{
				case 0x02:     //当key_up按下的话打开周期定时器
					xTimerStart(AutoReloadTimer_Handle,0);	//开启周期定时器
					printf("开启定时器1\r\n");
					break;
				
				case 0x03:		//当key0按下的话打开单次定时器
					xTimerStart(OneShotTimer_Handle,0);		//开启单次定时器
					printf("开启定时器2\r\n");
					break;
				
				case 0x01:		//当key1按下话就关闭定时器
					xTimerStop(AutoReloadTimer_Handle,0); 	//关闭周期定时器
					xTimerStop(OneShotTimer_Handle,0); 		  //关闭单次定时器
					printf("关闭定时器1和2\r\n");
					break;	
			}
		}

    vTaskDelay(200); //延时10ms，也就是10个时钟节拍
	}
}


//周期定时器的回调函数
void AutoReloadCallback(TimerHandle_t xTimer)
{
	printf("周期定时器运行\r\n");
}

//单次定时器的回调函数
void OneShotCallback(TimerHandle_t xTimer)
{
  printf("单次定时器运行\r\n");
}



















