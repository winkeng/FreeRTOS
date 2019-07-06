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
void list_task(void *pvParameters);   //������


List_t TestList;
ListItem_t ListItem1;
ListItem_t ListItem2;
ListItem_t ListItem3;



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
    xTaskCreate((TaskFunction_t )list_task,     	
                (const char*    )"list_task",
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

	}
}

//list ������ 
void list_task(void* pvParameters)
{
	//��ʼ���б���б���
	vListInitialise(&TestList);
	vListInitialiseItem(&ListItem1);
	vListInitialiseItem(&ListItem2);
	vListInitialiseItem(&ListItem3);
	
	ListItem1.xItemValue = 40;
	ListItem2.xItemValue = 60;
	ListItem3.xItemValue = 50;
	
	//��ӡ�б�������б���ĵ�ַ
	printf("*************** �б���б���ĵ�ַ ***************\r\n");
	printf("��Ŀ                            ��ַ		\r\n");
	printf("TestLis                         %#x			\r\n", (int)&TestList);
	printf("TestList->pxIndex               %#x			\r\n", (int)TestList.pxIndex);
	printf("TestList->xListEnd              %#x			\r\n", (int)(&TestList.xListEnd));
	printf("ListItem1                       %#x			\r\n", (int)&ListItem1);
	printf("ListItem2                       %#x			\r\n", (int)&ListItem2);
	printf("ListItem3                       %#x			\r\n", (int)&ListItem3);
	printf("****************** END ************************\r\n");
	printf("����KEY3��������\r\n\r\n");
	while(KEY3() == Bit_SET)
	{
		delay_ms(20);
	}
	delay_ms(1000);
	
	vListInsert(&TestList, &ListItem1);
	printf("********** ����б���ListItem1 **************\r\n");
	printf("��Ŀ                            ��ַ		\r\n");
	printf("TestList->xListEnd->pxNext      %#x			\r\n", (int)(TestList.xListEnd.pxNext));
	printf("ListItem1->pxNext               %#x			\r\n", (int)(ListItem1.pxNext));
	printf("*********************************************\r\n");
	printf("TestList->xListEnd->pxPrevious  %#x			\r\n", (int)(TestList.xListEnd.pxPrevious));
	printf("ListItem1->pxPrevious           %#x			\r\n", (int)(ListItem1.pxPrevious));
	printf("***************** END *******************\r\n");
	printf("����KEY3��������\r\n\r\n");
	while(KEY3() == Bit_SET)
	{
		delay_ms(20);
	}
	delay_ms(1000);
	
	vListInsert(&TestList, &ListItem2);
	printf("********** ����б���ListItem2 **************\r\n");
	printf("��Ŀ                            ��ַ		\r\n");
	printf("TestList->xListEnd->pxNext      %#x			\r\n", (int)(TestList.xListEnd.pxNext));
	printf("ListItem1->pxNext               %#x			\r\n", (int)(ListItem1.pxNext));
	printf("ListItem2->pxNext               %#x			\r\n", (int)(ListItem2.pxNext));
	printf("*********************************************\r\n");
	printf("TestList->xListEnd->pxPrevious  %#x			\r\n", (int)(TestList.xListEnd.pxPrevious));
	printf("ListItem1->pxPrevious           %#x			\r\n", (int)(ListItem1.pxPrevious));
	printf("ListItem2->pxPrevious           %#x			\r\n", (int)(ListItem2.pxPrevious));
	printf("***************** END *******************\r\n");
	printf("����KEY3��������\r\n\r\n");
	while(KEY3() == Bit_SET)
	{
		delay_ms(20);
	}
	delay_ms(1000);
	
	vListInsert(&TestList, &ListItem3);
	printf("********** ����б���ListItem3 **************\r\n");
	printf("��Ŀ                            ��ַ		\r\n");
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
	printf("����KEY3��������\r\n\r\n");
	while(KEY3() == Bit_SET)
	{
		delay_ms(20);
	}
	delay_ms(1000);
	
	uxListRemove(&ListItem2);
	printf("************** ɾ���б���ListItem2 *************\r\n");
	printf("��Ŀ                            ��ַ		\r\n");
	printf("TestList->xListEnd->pxNext      %#x			\r\n", (int)(TestList.xListEnd.pxNext));
	printf("ListItem1->pxNext               %#x			\r\n", (int)(ListItem1.pxNext));
	printf("ListItem3->pxNext               %#x			\r\n", (int)(ListItem3.pxNext));
	printf("*********************************************\r\n");
	printf("TestList->xListEnd->pxPrevious  %#x			\r\n", (int)(TestList.xListEnd.pxPrevious));
	printf("ListItem1->pxPrevious           %#x			\r\n", (int)(ListItem1.pxPrevious));
	printf("ListItem3->pxPrevious           %#x			\r\n", (int)(ListItem3.pxPrevious));
	printf("***************** END *******************\r\n");
	printf("����KEY3��������\r\n\r\n");
	while(KEY3() == Bit_SET)
	{
		delay_ms(20);
	}
	delay_ms(1000);
	
	TestList.pxIndex = TestList.pxIndex->pxNext;
	vListInsertEnd(&TestList, &ListItem2);
	printf("********** ��ĩβ����б���ListItem2 *********\r\n");
	printf("��Ŀ                            ��ַ		\r\n");
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



















