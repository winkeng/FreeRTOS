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

#define TASK1_TASK_PRIO		    2      //�������ȼ�
#define TASK1_STK_SIZE		    128    //�����ջ��С
TaskHandle_t Task1Task_Handler;		   //������
void task1_task(void *pvParameters);   //������

#define TASK2_TASK_PRIO		    3      //�������ȼ�
#define TASK2_STK_SIZE		    128    //�����ջ��С
TaskHandle_t Task2Task_Handler;		   //������
void task2_task(void *pvParameters);   //������


//������Ϣ������
char InfoBuffer[1000];


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
    //����TASK1����
    xTaskCreate((TaskFunction_t )task1_task,     	
                (const char*    )"task1_task",
                (uint16_t       )TASK1_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )TASK1_TASK_PRIO,	
                (TaskHandle_t*  )&Task1Task_Handler);    
    //����TASK1����
    xTaskCreate((TaskFunction_t )task2_task,     	
                (const char*    )"task2_task",
                (uint16_t       )TASK2_STK_SIZE, 
                (void*          )NULL,				
                (UBaseType_t    )TASK2_TASK_PRIO,	
                (TaskHandle_t*  )&Task2Task_Handler);    
    vTaskDelete(StartTask_Handler); //ɾ����ʼ����
    taskEXIT_CRITICAL();            //�˳��ٽ���
}


//task1 ������ 
void task1_task(void* pvParameters)
{
	while(1)
	{
		LED1_Toggle();
	}
}

//task2 ������ 
void task2_task(void* pvParameters)
{
	u32 TotalRunTime;
	UBaseType_t ArraySize, x;
	TaskStatus_t *StatusArray;
	
	printf("******** ��һ��������uxTaskGetSystemState()��ʹ�� ********\r\n");
	ArraySize = uxTaskGetNumberOfTasks();  //��ȡϵͳ��������
	StatusArray = pvPortMalloc(ArraySize* sizeof(TaskStatus_t));  //�����ڴ�
	
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
	
	printf("����KEY3��������\r\n\r\n");
	while(KEY3() == Bit_SET)
	{
		delay_ms(20);
	}
	delay_ms(1000);
	
	TaskHandle_t TaskHandle;
	TaskStatus_t TaskStatus;
	
	printf("************** �ڶ���������vTaskGetInfo()��ʹ�� *************\r\n");
	TaskHandle = xTaskGetHandle("task1_task");   //������������ȡ������
	
	vTaskGetInfo((TaskHandle_t   )TaskHandle,   //������
	             (TaskStatus_t*  )&TaskStatus,   //������Ϣ�ṹ��
	             (BaseType_t     )pdTRUE,        //����ͳ�������ջ��ʷ��Сʣ���С
	             (eTaskState     )eInvalid);     //�����Լ���ȡ��������״̬   
	
	printf("������:                   %s\r\n", TaskStatus.pcTaskName);
	printf("������:                 %d\r\n", (int)TaskStatus.xTaskNumber);
	printf("����״̬:                 %d\r\n", TaskStatus.eCurrentState);
	printf("����ǰ���ȼ�:           %d\r\n", (int)TaskStatus.uxCurrentPriority);
	printf("��������ȼ�:             %d\r\n", (int)TaskStatus.uxBasePriority);
	printf("�����ջ����ַ:           %x\r\n", (int)TaskStatus.pxStackBase);
	printf("�����ջ��ʷʣ����Сֵ:   %d\r\n",  TaskStatus.usStackHighWaterMark);
	printf("********************** end *********************\r\n");
	
	printf("����KEY3��������\r\n\r\n");
	while(KEY3() == Bit_SET)
	{
		delay_ms(20);
	}
	delay_ms(1000);

  eTaskState TaskState;
	char TaskInfo[10];
	
	printf("************* ������������eTaskGetState ****************\r\n");
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
	printf("����״ֵ̬��%d����Ӧ��״̬Ϊ��%s\r\n", TaskState, TaskInfo);
	printf("******************** end ******************\r\n");

	printf("����KEY3��������\r\n\r\n");
	while(KEY3() == Bit_SET)
	{
		delay_ms(20);
	}
	delay_ms(1000);
  
  printf("******************** ���Ĳ�������vTaskList()��ʹ�� *****************\r\n");

	vTaskList(InfoBuffer);  //��ȡ�����������Ϣ
	printf("%s\r\n", InfoBuffer);
	
	while(1)
	{
		LED2_Toggle();
	}
}



















