#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
#define MOTOR_A_PIN PAout(6)
#define MOTOR_B_PIN PAout(7)
#define SOUND_PIN PAout(5)

#define	INLINE_MOTOR_UP()	{MOTOR_A_PIN = 1; MOTOR_B_PIN = 0;}
#define	INLINE_MOTOR_DOWN()	{MOTOR_A_PIN = 0; MOTOR_B_PIN = 1;}
#define	INLINE_MOTOR_STOP()	{MOTOR_A_PIN = 0; MOTOR_B_PIN = 0;}
									
void LED_Init(void);//��ʼ��

		 				    
#endif
