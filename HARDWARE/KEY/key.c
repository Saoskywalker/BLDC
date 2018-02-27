#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//按键驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////  
								    
//按键初始化函数
void KEY_Init(void) //IO初始化
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOE,ENABLE);//使能PORTA,C,E时钟

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3|GPIO_Pin_4;//KEY0: 选择, KEY1: 确认
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
 	GPIO_Init(GPIOE, &GPIO_InitStructure);//初始化GPIOE 3,4

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;	//PA0: 调节
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0设置成下拉输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.0

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;	//PC0~4: 传感器
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //PA0设置成上拉输入
	GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化GPIOC1~4
}
//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，KEY0按下
//2，KEY1按下
//3，KEY2按下 
//4，KEY3按下 WK_UP
//注意此函数有响应优先级,KEY0>KEY1>KEY2>KEY3!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按		  
	if(key_up&&(SELECT_KEY_PIN==0||ENTER_KEY_PIN==0||ADJUST_KEY_PIN==1))
	{
		delay_ms(10);//去抖动 
		key_up=0;
		if(SELECT_KEY_PIN==0)return SELECT_KEY_PRES;
		else if(ENTER_KEY_PIN==0)return ENTER_KEY_PRES;
		else if(ADJUST_KEY_PIN==1)return ADJUST_KEY_PRES;
	}else if(SELECT_KEY_PIN==1&&ENTER_KEY_PIN==1&&ADJUST_KEY_PIN==0)key_up=1; 	    
 	return 0;// 无按键按下
}
