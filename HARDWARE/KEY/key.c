#include "stm32f10x.h"
#include "key.h"
#include "sys.h" 
#include "delay.h"
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
								    
//������ʼ������
void KEY_Init(void) //IO��ʼ��
{ 
 	GPIO_InitTypeDef GPIO_InitStructure;
 
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC|RCC_APB2Periph_GPIOE,ENABLE);//ʹ��PORTA,C,Eʱ��

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_3|GPIO_Pin_4;//KEY0: ѡ��, KEY1: ȷ��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
 	GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��GPIOE 3,4

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0;	//PA0: ����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //PA0���ó���������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.0

	GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;	//PC0~4: ������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //PA0���ó���������
	GPIO_Init(GPIOC, &GPIO_InitStructure);//��ʼ��GPIOC1~4
}
//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//0��û���κΰ�������
//1��KEY0����
//2��KEY1����
//3��KEY2���� 
//4��KEY3���� WK_UP
//ע��˺�������Ӧ���ȼ�,KEY0>KEY1>KEY2>KEY3!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//�������ɿ���־
	if(mode)key_up=1;  //֧������		  
	if(key_up&&(SELECT_KEY_PIN==0||ENTER_KEY_PIN==0||ADJUST_KEY_PIN==1))
	{
		delay_ms(10);//ȥ���� 
		key_up=0;
		if(SELECT_KEY_PIN==0)return SELECT_KEY_PRES;
		else if(ENTER_KEY_PIN==0)return ENTER_KEY_PRES;
		else if(ADJUST_KEY_PIN==1)return ADJUST_KEY_PRES;
	}else if(SELECT_KEY_PIN==1&&ENTER_KEY_PIN==1&&ADJUST_KEY_PIN==0)key_up=1; 	    
 	return 0;// �ް�������
}
