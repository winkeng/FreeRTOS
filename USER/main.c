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

#define TASK2_TASK_PRIO		    3      //任务优先级
#define TASK2_STK_SIZE		    128    //任务堆栈大小
TaskHandle_t Task2Task_Handler;		   //任务句柄
void task2_task(void *pvParameters);   //任务函数


//保存信息的数组
char InfoBuffer[1000];


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
    xTaskCreate((TaskFunction_t )task2_task,     	
                (const char*    )"task2_task",
                (uint16_t       )TASK2_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )TASK2_TASK_PRIO,	
                (TaskHandle_t*  )&Task2Task_Handler);    
    vTaskDelete(StartTask_Handler); //删除开始任务
    taskEXIT_CRITICAL();            //退出临界区
}


//task1 任务函数 
void task1_task(void* pvParameters)
{
	while(1)
	{
		LED1_Toggle();
	}
}

//task2 任务函数 
void task2_task(void* pvParameters)
{
	u32 TotalRunTime;
	UBaseType_t ArraySize, x;
	TaskStatus_t *StatusArray;
	
	printf("******** 第一步：函数uxTaskGetSystemState()的使用 ********\r\n");
	ArraySize = uxTaskGetNumberOfTasks();  //获取系统任务数量
	StatusArray = pvPortMalloc(ArraySize* sizeof(TaskStatus_t));  //申请内存
	
	if(StatusArray != NULL)
	{
		ArraySize = uxTaskGetSystemState((TaskStatus_t*   )StatusArray,
		                                 (UBaseType_t     )ArraySize,
		                                 (uint32_t*       )&TotalRunTime);
		printf("TaskName\t\tPriority\t\tTaskNumber\t\t\r\n");
		
		for(x=0; x<ArraySize; x++)
		{
			printf("%s\t\t%d\t\t\t%d\t\t\t\r\n", StatusArray[x].pcTaskName, 
			                                     (int)StatusArray[x].uxCurrentPriority, 
																					 (int)StatusArray[x].xTaskNumber);
		}
	}
	vPortFree(StatusArray);
	printf("***************** end ***************************\r\n");
	
	printf("按下KEY3键继续！\r\n\r\n");
	while(KEY3() == Bit_SET)
	{
		delay_ms(20);
	}
	delay_ms(1000);
	
	TaskHandle_t TaskHandle;
	TaskStatus_t TaskStatus;
	
	printf("************** 第二步：函数vTaskGetInfo()的使用 *************\r\n");
	TaskHandle = xTaskGetHandle("task1_task");   //根据任务名获取任务句柄
	
	vTaskGetInfo((TaskHandle_t   )TaskHandle,   //任务句柄
	             (TaskStatus_t*  )&TaskStatus,   //任务信息结构体
	             (BaseType_t     )pdTRUE,        //允许统计任务堆栈历史最小剩余大小
	             (eTaskState     )eInvalid);     //函数自己获取任务运行状态   
	
	printf("任务名:                   %s\r\n", TaskStatus.pcTaskName);
	printf("任务编号:                 %d\r\n", (int)TaskStatus.xTaskNumber);
	printf("任务状态:                 %d\r\n", TaskStatus.eCurrentState);
	printf("任务当前优先级:           %d\r\n", (int)TaskStatus.uxCurrentPriority);
	printf("任务基优先级:             %d\r\n", (int)TaskStatus.uxBasePriority);
	printf("任务堆栈基地址:           %x\r\n", (int)TaskStatus.pxStackBase);
	printf("任务堆栈历史剩余最小值:   %d\r\n",  TaskStatus.usStackHighWaterMark);
	printf("********************** end *********************\r\n");
	
	printf("按下KEY3键继续！\r\n\r\n");
	while(KEY3() == Bit_SET)
	{
		delay_ms(20);
	}
	delay_ms(1000);

  eTaskState TaskState;
	char TaskInfo[10];
	
	printf("************* 第三步：函数eTaskGetState ****************\r\n");
	TaskHandle = xTaskGetHandle("task2_task");
	TaskState = eTaskGetState(TaskHandle);
	memset(TaskInfo, 0, sizeof(TaskInfo));
	
	switch((int)TaskState)
	{
		case 0: sprintf(TaskInfo, "Running"); break;
		case 1: sprintf(TaskInfo, "Ready");   break;
		case 2: sprintf(TaskInfo, "Suspend"); break;
		case 3: sprintf(TaskInfo, "Delete");  break;
		case 4: sprintf(TaskInfo, "Invalid"); break;
	}
	printf("任务状态值：%d，对应的状态为：%s\r\n", TaskState, TaskInfo);
	printf("******************** end ******************\r\n");

	printf("按下KEY3键继续！\r\n\r\n");
	while(KEY3() == Bit_SET)
	{
		delay_ms(20);
	}
	delay_ms(1000);
  
  printf("******************** 第四步：函数vTaskList()的使用 *****************\r\n");

	vTaskList(InfoBuffer);  //获取所有任务的信息
	printf("%s\r\n", InfoBuffer);
	
	while(1)
	{
		LED2_Toggle();
	}
}



















