#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//������������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////   	 


//#define KEY0 PEin(4)   	//PE4
//#define KEY1 PEin(3)	//PE3 
//#define KEY2 PEin(2)	//PE2
//#define WK_UP PAin(0)	//PA0  WK_UP

#define SELECT_KEY_PIN  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)//��ȡ����0
#define ENTER_KEY_PIN  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)//��ȡ����1
#define ADJUST_KEY_PIN   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//��ȡ����3(WK_UP) 

#define SELECT_KEY_PRES 	1	//KEY0����
#define ENTER_KEY_PRES	2	//KEY1����
#define ADJUST_KEY_PRES   4	//KEY_UP����(��WK_UP/KEY_UP)

//����������
#define HAVE 0
#define WITHOUT 1
#define SENSOR_0_PIN  PCin(0) 
#define SENSOR_1_PIN  PCin(1) 
#define SENSOR_2_PIN  PCin(2)
#define SENSOR_3_PIN  PCin(3)
#define SENSOR_4_PIN  PCin(4)

void KEY_Init(void);//IO��ʼ��
u8 KEY_Scan(u8);  	//����ɨ�躯��					    
#endif
