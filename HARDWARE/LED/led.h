#ifndef __LED_H
#define __LED_H	 
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 
#define MOTOR_A_PIN PAout(6)
#define MOTOR_B_PIN PAout(7)
#define SOUND_PIN PAout(5)

#define	INLINE_MOTOR_UP()	{MOTOR_A_PIN = 1; MOTOR_B_PIN = 0;}
#define	INLINE_MOTOR_DOWN()	{MOTOR_A_PIN = 0; MOTOR_B_PIN = 1;}
#define	INLINE_MOTOR_STOP()	{MOTOR_A_PIN = 0; MOTOR_B_PIN = 0;}
									
void LED_Init(void);//初始化

		 				    
#endif
