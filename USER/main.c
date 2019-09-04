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
#include "limits.h"

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
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}

//设置事件位的任务
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
					xTaskNotify((TaskHandle_t	)EventGroupTask_Handler,//接收任务通知的任务句柄
								      (uint32_t		)EVENTBIT_1,			        //要更新的bit
								      (eNotifyAction	)eSetBits);				    //更新指定的bit
					break;
				case 0x03:
					xTaskNotify((TaskHandle_t	)EventGroupTask_Handler,//接收任务通知的任务句柄
								      (uint32_t		)EVENTBIT_2,			        //要更新的bit
								      (eNotifyAction	)eSetBits);				    //更新指定的bit
					break;	
			}
		}
    
		vTaskDelay(200); //延时200ms，也就是10个时钟节拍
	}
}


//事件标志组处理任务
void eventgroup_task(void *pvParameters)
{
	BaseType_t err;
	uint32_t NotifyValue;
	
	while(1)
	{
		//获取任务通知值
		err=xTaskNotifyWait((uint32_t	)0x00,    //进入函数的时候不清除任务bit
							(uint32_t	)ULONG_MAX,			    //退出函数的时候清除所有的bit
							(uint32_t*	)&NotifyValue,		//保存任务通知值
							(TickType_t	)portMAX_DELAY);	//阻塞时间
		
		if(err==pdPASS)	   //任务通知获取成功
		{
			if((NotifyValue&EVENTBIT_1)!=0)	//事件1发生	
			{
				printf("事件1发生\r\n");
			}
			else if((NotifyValue&EVENTBIT_2)!=0)	//事件2发生	
			{
				printf("事件2发生\r\n");
			}
		}
	}
}
















