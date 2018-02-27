/************************************************
Name: BalanceBall
Describe: Contral ball sport on rail(use or change ZhengDianYuanZi Library, Thank you!)
Version: v1.0
Author: Aysi
Date: 2015.10.17
************************************************/

#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "lcd.h"
#include "usart.h"
#include "mpu6050.h"
#include "usmart.h"   
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
#include "timer.h"

#define REPORT 0	//MPU6050���ݷ�������

float pitch,roll,yaw; 		//ŷ����
u8 Locate = 0, Count = 0, Range = 0;	//��ֹͣλ������, ���˶���������, ����
u8 key;	//��ֵ
u8 TimeFlag200ms = 0;	//200ms����־
//ѡ�����: 1: �˶����� 2���� 3 ֹͣλ��
u8 SelectValue = 1, TestTerm = 1;	
u8 WorkFlag = 0, TestEnterFlag = 0, SuccessFlag = 0;	//������־

void Test1(void);
void Test2(void);
void Test3(void);
void Test4(void);
void Test5(void);

//Bee function
const u16 BeeModPeriod[] = {110, 410, 1000}; 
const u16 BeeModCompare[] = {100, 400, 500};
u8 BeeMod = 0, BeeTime = 0;
void BeeFunction(void)
{	
	static u16 BeeTimeCnt = 0;

	if(BeeTime>0)
	{
		if(++BeeTimeCnt>=BeeModPeriod[BeeMod])
		{
			BeeTimeCnt = 0;
			BeeTime--;
		}
		else
		{
			if(BeeTimeCnt>=BeeModCompare[BeeMod])
				SOUND_PIN = 0;
			else
				SOUND_PIN = 1;		
		}	
	}
	else
	{
		SOUND_PIN = 0;
	}
}

//����1����1���ַ� 
//c:Ҫ���͵��ַ�
void usart1_send_char(u8 c)
{   	
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //ѭ������,ֱ���������   
	USART_SendData(USART1,c);  
} 
//�������ݸ�����������λ�����(V2.6�汾)
//fun:������. 0XA0~0XAF
//data:���ݻ�����,���28�ֽ�!!
//len:data����Ч���ݸ���
void usart1_niming_report(u8 fun,u8*data,u8 len)
{
	u8 send_buf[32];
	u8 i;
	if(len>28)return;	//���28�ֽ����� 
	send_buf[len+3]=0;	//У��������
	send_buf[0]=0X88;	//֡ͷ
	send_buf[1]=fun;	//������
	send_buf[2]=len;	//���ݳ���
	for(i=0;i<len;i++)send_buf[3+i]=data[i];			//��������
	for(i=0;i<len+3;i++)send_buf[len+3]+=send_buf[i];	//����У���	
	for(i=0;i<len+4;i++)usart1_send_char(send_buf[i]);	//�������ݵ�����1 
}
//���ͼ��ٶȴ��������ݺ�����������
//aacx,aacy,aacz:x,y,z������������ļ��ٶ�ֵ
//gyrox,gyroy,gyroz:x,y,z�������������������ֵ
void mpu6050_send_data(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz)
{
	u8 tbuf[12]; 
	tbuf[0]=(aacx>>8)&0XFF;
	tbuf[1]=aacx&0XFF;
	tbuf[2]=(aacy>>8)&0XFF;
	tbuf[3]=aacy&0XFF;
	tbuf[4]=(aacz>>8)&0XFF;
	tbuf[5]=aacz&0XFF; 
	tbuf[6]=(gyrox>>8)&0XFF;
	tbuf[7]=gyrox&0XFF;
	tbuf[8]=(gyroy>>8)&0XFF;
	tbuf[9]=gyroy&0XFF;
	tbuf[10]=(gyroz>>8)&0XFF;
	tbuf[11]=gyroz&0XFF;
	usart1_niming_report(0XA1,tbuf,12);//�Զ���֡,0XA1
}	
//ͨ������1�ϱ���������̬���ݸ�����
//aacx,aacy,aacz:x,y,z������������ļ��ٶ�ֵ
//gyrox,gyroy,gyroz:x,y,z�������������������ֵ
//roll:�����.��λ0.01�ȡ� -18000 -> 18000 ��Ӧ -180.00  ->  180.00��
//pitch:������.��λ 0.01�ȡ�-9000 - 9000 ��Ӧ -90.00 -> 90.00 ��
//yaw:�����.��λΪ0.1�� 0 -> 3600  ��Ӧ 0 -> 360.0��
void usart1_report_imu(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz,short roll,short pitch,short yaw)
{
	u8 tbuf[28]; 
	u8 i;
	for(i=0;i<28;i++)tbuf[i]=0;//��0
	tbuf[0]=(aacx>>8)&0XFF;
	tbuf[1]=aacx&0XFF;
	tbuf[2]=(aacy>>8)&0XFF;
	tbuf[3]=aacy&0XFF;
	tbuf[4]=(aacz>>8)&0XFF;
	tbuf[5]=aacz&0XFF; 
	tbuf[6]=(gyrox>>8)&0XFF;
	tbuf[7]=gyrox&0XFF;
	tbuf[8]=(gyroy>>8)&0XFF;
	tbuf[9]=gyroy&0XFF;
	tbuf[10]=(gyroz>>8)&0XFF;
	tbuf[11]=gyroz&0XFF;	
	tbuf[18]=(roll>>8)&0XFF;
	tbuf[19]=roll&0XFF;
	tbuf[20]=(pitch>>8)&0XFF;
	tbuf[21]=pitch&0XFF;
	tbuf[22]=(yaw>>8)&0XFF;
	tbuf[23]=yaw&0XFF;
	usart1_niming_report(0XAF,tbuf,28);//�ɿ���ʾ֡,0XAF
}  
 	
 int main(void)
 {	 
	short aacx,aacy,aacz;		//���ٶȴ�����ԭʼ����
	short gyrox,gyroy,gyroz;	//������ԭʼ����
	short temp;					//�¶�
	 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(9600);	 	//���ڳ�ʼ��Ϊ9600
	delay_init();	//��ʱ��ʼ�� 
	usmart_dev.init(72);		//��ʼ��USMART
	LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
	KEY_Init();					//��ʼ������
	LCD_Init();			   		//��ʼ��LCD  
	MPU_Init();					//��ʼ��MPU6050
	TIM2_Int_Init(999,70);	//��ʼ����ʱ��2 1ms�ж�
	TIM4_PWM_Init(7200,0);	//��ʼ����ʱ��4 10KhzƵ��
	TIM_SetCompare4(TIM4,0);	
	 
 	POINT_COLOR = RED;			//��������Ϊ��ɫ 
	LCD_ShowString(30,50,200,16,16,"BalanceBall");	
	LCD_ShowString(30,90,200,16,16,"By Aysi-E");
	LCD_ShowString(30,110,200,16,16,"2015/10/17"); 
	while(mpu_dmp_init())
 	{
		LCD_ShowString(30,130,200,16,16,"MPU6050 Error");
		delay_ms(200);
		LCD_Fill(30,130,239,130+16,WHITE);
 		delay_ms(200);
	}  
	LCD_ShowString(30,130,200,16,16,"Initialization OK");
	LCD_ShowString(30,150,200,16,16,"Test 1");
	POINT_COLOR=BLUE;//��������Ϊ��ɫ 
	LCD_ShowString(30,200,200,16,16,"Angle:     Deg");	
 	LCD_ShowString(30,170,200,16,16,"Realy");	
	POINT_COLOR = GRAY;	//��������Ϊ��ɫ
 	LCD_ShowString(30,220,200,16,16,"Count:    ");	
 	LCD_ShowString(30,240,200,16,16,"Locate:    ");	 
// 	LCD_ShowString(30,260,200,16,16," Yaw :    . C");	 
 	while(1)
	{
		key = KEY_Scan(0);
		switch(key)
		{
			case SELECT_KEY_PRES:
			{
				if(TestEnterFlag==0)
				{
					if(++TestTerm>=5)
						TestTerm = 1;
					POINT_COLOR = GREEN;
					switch(TestTerm)
					{
						case 1: LCD_ShowString(30,150,200,16,16,"Test 1"); break;
						case 2: LCD_ShowString(30,150,200,16,16,"Test 2"); break;
						case 3: LCD_ShowString(30,150,200,16,16,"Test 3"); break;
						case 4: LCD_ShowString(30,150,200,16,16,"Test 4"); break;
						case 5: LCD_ShowString(30,150,200,16,16,"Test 5"); break;
						default: break;
					}
				}
				else
				{
					if(++SelectValue>=3)
						SelectValue = 1;
				}
				break;
			}			
			case ENTER_KEY_PRES:
			{
				if(TestEnterFlag==0)
				{
					TestEnterFlag = 1;
					POINT_COLOR = RED;
					switch(TestTerm)
					{
						case 1: LCD_ShowString(30,150,200,16,16,"Test 1"); break;
						case 2: LCD_ShowString(30,150,200,16,16,"Test 2"); break;
						case 3: LCD_ShowString(30,150,200,16,16,"Test 3"); break;
						case 4: LCD_ShowString(30,150,200,16,16,"Test 4"); break;
						case 5: LCD_ShowString(30,150,200,16,16,"Test 5"); break;
						default: break;
					}
				}
				else
				{
					if(WorkFlag==1)
					{
						WorkFlag = 0;
						TestEnterFlag = 0;
					}
					else
					{
						WorkFlag = 1;
					}
				}
				break;
			}
			case ADJUST_KEY_PRES:
			{	
				if(SelectValue==1)
				{
					if(++Count>=10)
						Count = 0;
				}
				if(SelectValue==2)
				{
					if(++Range>=10)
						Range = 0;
				}
				if(SelectValue==3)
				{
					if(++Locate>=10)
						Locate = 0;
				}
				break;
			}
			default:{break;}
		}
		
		switch(TestTerm)
		{
			case 1:
			{
				Test1();
				break;
			}
			case 2:
			{
				Test2();
				break;
			}
			case 3:
			{
				Test3();
				break;
			}
			case 4:
			{
				Test4();
				break;
			}
			case 5:
			{
				Test5();
				break;
			}
			default: {break;}
		}
		
		//��ֵÿ200ms��ʾˢ��
		if(TimeFlag200ms==1)
		{
			TimeFlag200ms = 0;
			
			POINT_COLOR = RED;
			if(WorkFlag==1)
			{
				if(SuccessFlag==1)
				{
					LCD_ShowString(30,170,200,16,16,"Success");
				}
				else
				LCD_ShowString(30,170,200,16,16," Go  ");
			}
			else
			{
				LCD_ShowString(30,170,200,16,16,"Realy");
			}
			
			POINT_COLOR = YELLOW;
			temp=roll*10;	//X axis display
			if(temp<0)
			{
				LCD_ShowChar(30+48,200,'-',16,0);		//��ʾ����
				temp=-temp;		//תΪ����
			}
			else 
			{
				LCD_ShowChar(30+48,200,' ',16,0);		//ȥ������ 
			}			
			LCD_ShowNum(30+48+8,200,temp/10,3,16);		//��ʾ��������	    
			LCD_ShowNum(30+48+40,200,temp%10,1,16);		//��ʾС������ 
		
			if(SelectValue==3)
				POINT_COLOR = GREEN;
			else
				POINT_COLOR = RED;
			temp=Range*10;	//Locate value display
			LCD_ShowNum(30+48+8,260,temp/10,3,16);		//��ʾ��������	    
			LCD_ShowNum(30+48+40,260,temp%10,1,16);		//��ʾС������ 
			
			if(SelectValue==2)
				POINT_COLOR = GREEN;
			else
				POINT_COLOR = RED;
			temp=Locate*10;	//Locate value display
			LCD_ShowNum(30+48+8,240,temp/10,3,16);		//��ʾ��������	    
			LCD_ShowNum(30+48+40,240,temp%10,1,16);		//��ʾС������ 
			
			if(SelectValue==1)
				POINT_COLOR = GREEN;
			else
				POINT_COLOR = RED;
			temp=Count*10;	//Count value display				
			LCD_ShowNum(30+48+8,220,temp/10,3,16);		//��ʾ��������	    
			LCD_ShowNum(30+48+40,220,temp%10,1,16);		//��ʾС������ 
		}
		
		if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)
		{ 
			MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//�õ����ٶȴ���������
			MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//�õ�����������
			#if REPORT==1
			//���Զ���֡���ͼ��ٶȺ�������ԭʼ����
			mpu6050_send_data(aacx,aacy,aacz,gyrox,gyroy,gyroz);
			#elseif REPORT==2
			//���Զ���֡���ͼ��ٶȺ�������ԭʼ����,��ŷ����
			usart1_report_imu(aacx,aacy,aacz,gyrox,gyroy,gyroz,(int)(roll*100),(int)(pitch*100),(int)(yaw*10));
			#endif
		}
	} 	
}

/*Test 2 side sensor of rail*/
void Test1(void)
{
	static u8 i = 0, j = 0;
	if(WorkFlag)
	{
		if(SuccessFlag==0)
		{
			if(SENSOR_0_PIN==HAVE)
			{
				BeeMod = 1;
				BeeTime = 1;
				i++;
			}
			else if(SENSOR_4_PIN==HAVE)
			{
				BeeMod = 1;
				BeeTime = 1;
				j++;
			}
			if(i&&j)
			{
				BeeMod = 1;
				BeeTime = 2;
				SuccessFlag = 1;
				i = 0;
				j = 0;
			}
		}
	}
	else
	{
		SuccessFlag = 0;
		i = 0;
		j = 0;
	}
}

/*Test rail becomes balance or not*/
void Test2(void)
{
	if(WorkFlag)
	{
		if(SuccessFlag==0)
		{
			if((roll>=-2)&&(roll<=2))
			{
				INLINE_MOTOR_STOP();
				SuccessFlag = 1;
				BeeMod = 1;
				BeeTime = 2;
			}
			else
			{
				if(roll<0)
				{INLINE_MOTOR_UP();}
				else
				{INLINE_MOTOR_DOWN();}			
			}
		}
	}
	else
	{
		SuccessFlag = 0;
	}
}

/*����*/
void Test3(void)
{
	
}

/*�˶����������*/
void Test4(void)
{
	
}

/*�˶����������,����ֹͣ*/
void Test5(void)
{

}

