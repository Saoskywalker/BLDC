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

#define REPORT 0	//MPU6050数据发送设置

float pitch,roll,yaw; 		//欧拉角
u8 Locate = 0, Count = 0, Range = 0;	//球停止位置设置, 球运动次数设置, 区间
u8 key;	//键值
u8 TimeFlag200ms = 0;	//200ms到标志
//选择变量: 1: 运动次数 2区间 3 停止位置
u8 SelectValue = 1, TestTerm = 1;	
u8 WorkFlag = 0, TestEnterFlag = 0, SuccessFlag = 0;	//工作标志

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

//串口1发送1个字符 
//c:要发送的字符
void usart1_send_char(u8 c)
{   	
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET); //循环发送,直到发送完毕   
	USART_SendData(USART1,c);  
} 
//传送数据给匿名四轴上位机软件(V2.6版本)
//fun:功能字. 0XA0~0XAF
//data:数据缓存区,最多28字节!!
//len:data区有效数据个数
void usart1_niming_report(u8 fun,u8*data,u8 len)
{
	u8 send_buf[32];
	u8 i;
	if(len>28)return;	//最多28字节数据 
	send_buf[len+3]=0;	//校验数置零
	send_buf[0]=0X88;	//帧头
	send_buf[1]=fun;	//功能字
	send_buf[2]=len;	//数据长度
	for(i=0;i<len;i++)send_buf[3+i]=data[i];			//复制数据
	for(i=0;i<len+3;i++)send_buf[len+3]+=send_buf[i];	//计算校验和	
	for(i=0;i<len+4;i++)usart1_send_char(send_buf[i]);	//发送数据到串口1 
}
//发送加速度传感器数据和陀螺仪数据
//aacx,aacy,aacz:x,y,z三个方向上面的加速度值
//gyrox,gyroy,gyroz:x,y,z三个方向上面的陀螺仪值
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
	usart1_niming_report(0XA1,tbuf,12);//自定义帧,0XA1
}	
//通过串口1上报结算后的姿态数据给电脑
//aacx,aacy,aacz:x,y,z三个方向上面的加速度值
//gyrox,gyroy,gyroz:x,y,z三个方向上面的陀螺仪值
//roll:横滚角.单位0.01度。 -18000 -> 18000 对应 -180.00  ->  180.00度
//pitch:俯仰角.单位 0.01度。-9000 - 9000 对应 -90.00 -> 90.00 度
//yaw:航向角.单位为0.1度 0 -> 3600  对应 0 -> 360.0度
void usart1_report_imu(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz,short roll,short pitch,short yaw)
{
	u8 tbuf[28]; 
	u8 i;
	for(i=0;i<28;i++)tbuf[i]=0;//清0
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
	usart1_niming_report(0XAF,tbuf,28);//飞控显示帧,0XAF
}  
 	
 int main(void)
 {	 
	short aacx,aacy,aacz;		//加速度传感器原始数据
	short gyrox,gyroy,gyroz;	//陀螺仪原始数据
	short temp;					//温度
	 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart_init(9600);	 	//串口初始化为9600
	delay_init();	//延时初始化 
	usmart_dev.init(72);		//初始化USMART
	LED_Init();		  			//初始化与LED连接的硬件接口
	KEY_Init();					//初始化按键
	LCD_Init();			   		//初始化LCD  
	MPU_Init();					//初始化MPU6050
	TIM2_Int_Init(999,70);	//初始化定时器2 1ms中断
	TIM4_PWM_Init(7200,0);	//初始化定时器4 10Khz频率
	TIM_SetCompare4(TIM4,0);	
	 
 	POINT_COLOR = RED;			//设置字体为红色 
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
	POINT_COLOR=BLUE;//设置字体为蓝色 
	LCD_ShowString(30,200,200,16,16,"Angle:     Deg");	
 	LCD_ShowString(30,170,200,16,16,"Realy");	
	POINT_COLOR = GRAY;	//设置字体为灰色
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
		
		//数值每200ms显示刷新
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
				LCD_ShowChar(30+48,200,'-',16,0);		//显示负号
				temp=-temp;		//转为正数
			}
			else 
			{
				LCD_ShowChar(30+48,200,' ',16,0);		//去掉负号 
			}			
			LCD_ShowNum(30+48+8,200,temp/10,3,16);		//显示整数部分	    
			LCD_ShowNum(30+48+40,200,temp%10,1,16);		//显示小数部分 
		
			if(SelectValue==3)
				POINT_COLOR = GREEN;
			else
				POINT_COLOR = RED;
			temp=Range*10;	//Locate value display
			LCD_ShowNum(30+48+8,260,temp/10,3,16);		//显示整数部分	    
			LCD_ShowNum(30+48+40,260,temp%10,1,16);		//显示小数部分 
			
			if(SelectValue==2)
				POINT_COLOR = GREEN;
			else
				POINT_COLOR = RED;
			temp=Locate*10;	//Locate value display
			LCD_ShowNum(30+48+8,240,temp/10,3,16);		//显示整数部分	    
			LCD_ShowNum(30+48+40,240,temp%10,1,16);		//显示小数部分 
			
			if(SelectValue==1)
				POINT_COLOR = GREEN;
			else
				POINT_COLOR = RED;
			temp=Count*10;	//Count value display				
			LCD_ShowNum(30+48+8,220,temp/10,3,16);		//显示整数部分	    
			LCD_ShowNum(30+48+40,220,temp%10,1,16);		//显示小数部分 
		}
		
		if(mpu_dmp_get_data(&pitch,&roll,&yaw)==0)
		{ 
			MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//得到加速度传感器数据
			MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//得到陀螺仪数据
			#if REPORT==1
			//用自定义帧发送加速度和陀螺仪原始数据
			mpu6050_send_data(aacx,aacy,aacz,gyrox,gyroy,gyroz);
			#elseif REPORT==2
			//用自定义帧发送加速度和陀螺仪原始数据,和欧拉角
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

/*次数*/
void Test3(void)
{
	
}

/*运动区间与次数*/
void Test4(void)
{
	
}

/*运动区间与次数,定点停止*/
void Test5(void)
{

}

