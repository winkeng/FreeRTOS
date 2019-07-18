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
void list_task(void *pvParameters);   //任务函数


List_t TestList;
ListItem_t ListItem1;
ListItem_t ListItem2;
ListItem_t ListItem3;



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
    xTaskCreate((TaskFunction_t )list_task,     	
                (const char*    )"list_task",
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

	}
}

//list 任务函数 
void list_task(void* pvParameters)
{
	//初始化列表和列表项
	vListInitialise(&TestList);
	vListInitialiseItem(&ListItem1);
	vListInitialiseItem(&ListItem2);
	vListInitialiseItem(&ListItem3);
	
	ListItem1.xItemValue = 40;
	ListItem2.xItemValue = 60;
	ListItem3.xItemValue = 50;
	
	//打印列表和其他列表项的地址
	printf("*************** 列表和列表项的地址 ***************\r\n");
	printf("项目                            地址		\r\n");
	printf("TestLis                         %#x			\r\n", (int)&TestList);
	printf("TestList->pxIndex               %#x			\r\n", (int)TestList.pxIndex);
	printf("TestList->xListEnd              %#x			\r\n", (int)(&TestList.xListEnd));
	printf("ListItem1                       %#x			\r\n", (int)&ListItem1);
	printf("ListItem2                       %#x			\r\n", (int)&ListItem2);
	printf("ListItem3                       %#x			\r\n", (int)&ListItem3);
	printf("****************** END ************************\r\n");
	printf("按下KEY3键继续！\r\n\r\n");
	while(KEY3() == Bit_SET)
	{
		delay_ms(20);
	}
	delay_ms(1000);
	
	vListInsert(&TestList, &ListItem1);
	printf("********** 添加列表项ListItem1 **************\r\n");
	printf("项目                            地址		\r\n");
	printf("TestList->xListEnd->pxNext      %#x			\r\n", (int)(TestList.xListEnd.pxNext));
	printf("ListItem1->pxNext               %#x			\r\n", (int)(ListItem1.pxNext));
	printf("*********************************************\r\n");
	printf("TestList->xListEnd->pxPrevious  %#x			\r\n", (int)(TestList.xListEnd.pxPrevious));
	printf("ListItem1->pxPrevious           %#x			\r\n", (int)(ListItem1.pxPrevious));
	printf("***************** END *******************\r\n");
	printf("按下KEY3键继续！\r\n\r\n");
	while(KEY3() == Bit_SET)
	{
		delay_ms(20);
	}
	delay_ms(1000);
	
	vListInsert(&TestList, &ListItem2);
	printf("********** 添加列表项ListItem2 **************\r\n");
	printf("项目                            地址		\r\n");
	printf("TestList->xListEnd->pxNext      %#x			\r\n", (int)(TestList.xListEnd.pxNext));
	printf("ListItem1->pxNext               %#x			\r\n", (int)(ListItem1.pxNext));
	printf("ListItem2->pxNext               %#x			\r\n", (int)(ListItem2.pxNext));
	printf("*********************************************\r\n");
	printf("TestList->xListEnd->pxPrevious  %#x			\r\n", (int)(TestList.xListEnd.pxPrevious));
	printf("ListItem1->pxPrevious           %#x			\r\n", (int)(ListItem1.pxPrevious));
	printf("ListItem2->pxPrevious           %#x			\r\n", (int)(ListItem2.pxPrevious));
	printf("***************** END *******************\r\n");
	printf("按下KEY3键继续！\r\n\r\n");
	while(KEY3() == Bit_SET)
	{
		delay_ms(20);
	}
	delay_ms(1000);
	
	vListInsert(&TestList, &ListItem3);
	printf("********** 添加列表项ListItem3 **************\r\n");
	printf("项目                            地址		\r\n");
	printf("TestList->xListEnd->pxNext      %#x			\r\n", (int)(TestList.xListEnd.pxNext));
	printf("ListItem1->pxNext               %#x			\r\n", (int)(ListItem1.pxNext));
	printf("ListItem2->pxNext               %#x			\r\n", (int)(ListItem2.pxNext));
	printf("ListItem3->pxNext               %#x			\r\n", (int)(ListItem3.pxNext));
	printf("*********************************************\r\n");
	printf("TestList->xListEnd->pxPrevious  %#x			\r\n", (int)(TestList.xListEnd.pxPrevious));
	printf("ListItem1->pxPrevious           %#x			\r\n", (int)(ListItem1.pxPrevious));
	printf("ListItem3->pxPrevious           %#x			\r\n", (int)(ListItem3.pxPrevious));
	printf("ListItem2->pxPrevious           %#x			\r\n", (int)(ListItem2.pxPrevious));
	printf("***************** END *******************\r\n");
	printf("按下KEY3键继续！\r\n\r\n");
	while(KEY3() == Bit_SET)
	{
		delay_ms(20);
	}
	delay_ms(1000);
	
	uxListRemove(&ListItem2);
	printf("************** 删除列表项ListItem2 *************\r\n");
	printf("项目                            地址		\r\n");
	printf("TestList->xListEnd->pxNext      %#x			\r\n", (int)(TestList.xListEnd.pxNext));
	printf("ListItem1->pxNext               %#x			\r\n", (int)(ListItem1.pxNext));
	printf("ListItem3->pxNext               %#x			\r\n", (int)(ListItem3.pxNext));
	printf("*********************************************\r\n");
	printf("TestList->xListEnd->pxPrevious  %#x			\r\n", (int)(TestList.xListEnd.pxPrevious));
	printf("ListItem1->pxPrevious           %#x			\r\n", (int)(ListItem1.pxPrevious));
	printf("ListItem3->pxPrevious           %#x			\r\n", (int)(ListItem3.pxPrevious));
	printf("***************** END *******************\r\n");
	printf("按下KEY3键继续！\r\n\r\n");
	while(KEY3() == Bit_SET)
	{
		delay_ms(20);
	}
	delay_ms(1000);
	
	TestList.pxIndex = TestList.pxIndex->pxNext;
	vListInsertEnd(&TestList, &ListItem2);
	printf("********** 在末尾添加列表项ListItem2 *********\r\n");
	printf("项目                            地址		\r\n");
	printf("TestList->pxIndex               %#x			\r\n", (int)(TestList.pxIndex));
	printf("TestList->xListEnd->pxNext      %#x			\r\n", (int)(TestList.xListEnd.pxNext));
	printf("ListItem2->pxNext               %#x			\r\n", (int)(ListItem2.pxNext));
	printf("ListItem1->pxNext               %#x			\r\n", (int)(ListItem1.pxNext));
	printf("ListItem3->pxNext               %#x			\r\n", (int)(ListItem3.pxNext));
	printf("*********************************************\r\n");
	printf("TestList->xListEnd->pxPrevious  %#x			\r\n", (int)(TestList.xListEnd.pxPrevious));
	printf("ListItem2->pxPrevious           %#x			\r\n", (int)(ListItem2.pxPrevious));
	printf("ListItem1->pxPrevious           %#x			\r\n", (int)(ListItem1.pxPrevious));
	printf("ListItem3->pxPrevious           %#x			\r\n", (int)(ListItem3.pxPrevious));
	printf("***************** END *******************\r\n");
	
	
	while(1)
	{
		LED_Toggle();
	}
	
}



















