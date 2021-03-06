//STLINK PA13 PA14
#include "stm32f10x.h"
#include "math.h"
#include "stdio.h"
#include "delay.h"
#include "SteeringEngine.h"
#include "Distance.h"
#include "sys.h"
#include "usart.h"
#include "rfid.h"
#include "Configuration1.h"
#include "Tracking.h"
//ALIENTEK战舰STM32开发板实验4
//串口实验  
//技术支持：www.openedv.com
//广州市星翼电子科技有限公司 

void delay_nus(u32);           //72M时钟下，约延时us
void delay_nms(u32);            //72M时钟下，约延时ms
 void detect(u8 num);              //寻找相应标记
u8 read1(void);               //读出编号为1的RFID数据PA9，PA10，左侧rfid读卡器
u8 read2(void);               //读出编号为2的RFID数据PA2，PA3，走右侧读卡器
void USART3_Configuration(void);
void TIM3_Init(void){
		
	
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;	  //定义格式为TIM_TimeBaseInitTypeDef的结构体的名字为TIM_TimeBaseStructure  

  	TIM_TimeBaseStructure.TIM_Period =9999;		  // 配置计数阈值为9999，超过时，自动清零，并1ms触发中断
  	TIM_TimeBaseStructure.TIM_Prescaler =71;		 //	 时钟预分频值，除以多少
  	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;	// 时钟分频倍数
  	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	// 计数方式为向上计数

    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);		 //	 初始化tim3
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update); //清除TIM3溢出中断标志
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);   //  使能TIM3的溢出更新中断
    TIM_Cmd(TIM3,DISABLE);				 //		   使能TIM3
}
int i=0;
u8 success=0;
u8 duoji_success=0;
u8 RxData;
u8 j=0,k=0,b=0;
u8 mission=0;//0表示红方场地，1表示蓝方场地，测试期间均默认为0
u8 direction=1;
float dis_1=0,dis_11=0,dis_12=0,disA=0;
u8 f1,f2;//红外对管的out
u8 f3,f4,f5,f6,f11,f7,f8,f9,f10,f12;
u8 t1,t2,t3,t4;
u8 a=0x00;//读出的RFID第一块数据
u8 num1;
u8 carpos=0;//记录车所在的位�
u8 flag_count=0;
u8 temp;
u8 high_flag1,high_flag2,temp1=0,temp2=0;
int flag1=0,ok=0,chen=0,ii=0,jj=0,ruo=0,xi=0,wu=0,shuang=0;
void assert_failed(uint8_t* file, uint32_t line)
{
 printf("Wrong parameters value: file %s on line %d\r\n", file, line);
 while(1);
}

void track0(){
	f1=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_1);
		f2=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_2);
		if(f1==1&&f2==1){
			 gobackwards(); 
		 }
		 else if(f1==0&&f2==1){
		   turn_right();
		 }
		 else if(f1==1&&f2==0){
			  turn_left();
		 }
		 else{
		    gobackwards();
		 }
}

	
void track1(){
	  f1=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_1);
		f2=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_2);
		if(f1==1&&f2==1){
			 goforwards(); 
		 }
		 else if(f1==0&&f2==1){
		   turn_left();
		 }
		 else if(f1==1&&f2==0){
			  turn_right();
		 }
		 else{
		    goforwards();
		 }
}
void track2(){ 
		 f3=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_3);
		 f4=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_10);
		 f5=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_11);
		 f6=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_4);
		 if(f3==1&&f4==1&&f5==1&&f6==1){
			 goforwards();
		 }
		 else if((f3==0||f5==0)&&(f4==1||f6==1)){
			 turn_right();
		 }
		 else if((f3==1||f5==1)&&(f4==0||f6==0)){
			 turn_left();
		 }
		 else{
			 goforwards();
		 }
}	
void go_n_step(u8 m){
	  f11=0;
	while(m){
		while(!f11){
			goforwards();
			f11=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_7);
		  f4=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_10);//7
		  f6=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_4);//5
			if(wu==0){
			  if(f4==1&&f6==0){
					if(mission==1){
					setWheel(2,2,40);
		      setWheel(1,2,0);
					}
					else{
					  setWheel(2,2,0);
		        setWheel(1,2,40);
					}
				  delay_nms(10);}
			else if(f4==0&&f6==1){
				if(mission==1){
				   setWheel(2,2,0);
		       setWheel(1,2,40);
				}
				else{
					setWheel(2,2,40);
		      setWheel(1,2,0);
				}
				delay_nms(10);}
			else if(f4==1&&f6==1)
					wu=1;
			else 
			delay_nms(10);
			}
		else 
			delay_nms(10);}
		while(f11){
			goforwards();
			f11=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_7);
			delay_nms(10);
		}
		m--;
		wu=0;
	}
		brake();
}
void back_n_step(u8 m){
	  f11=0;
	while(m){
		while(!f11){
			gobackwards();
			f11=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_8);
			f4=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_10);//7
		  f6=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_4);//5
			if(shuang==0){
			  if(f4==1&&f6==0){
					setWheel(2,1,0);
		      setWheel(1,1,40);
				  delay_nms(10);}
			  else if(f4==0&&f6==1){
					setWheel(2,1,40);
		      setWheel(1,1,0);
				  delay_nms(10);
				}
				else if(f4==1&&f6==1)
					shuang=1;
			  else 
			    delay_nms(10);
			}
		 else 
			  delay_nms(10);
		}
		while(f11){
			gobackwards();
			f11=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_8);
			delay_nms(10);
		}
	m--;
		shuang=0;
	}
		  brake();
}
void go_back_1(){
	  f12=0;
		while(!f12){
			gobackwards();
			f12=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_8);
			delay_nms(10);
		}
		while(f12){
			gobackwards();
			f12=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_8);
			delay_nms(10);
		}
		brake();
}
void turn_0(){
   f11=0;
		while(!f11){
			CircleRight_R();
			f11=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_7);
			delay_nms(10);
		}
		while(f11){
			CircleRight_R();
			f11=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_7);
			delay_nms(10);
		}
		brake();
}
void turn_0_1(){
   f9=0;
		while(!f9){
			CircleLeft_R();
			f9=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_5);
			delay_nms(10);
		}
		while(f9){
			CircleLeft_R();
			f9=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_5);
			delay_nms(10);
		}
		brake();
}
//返回时转弯程序
void turn_1(){
	 f2=0;
	while(!f2){
			CircleLeft_R();
			f2=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_1);
			delay_nms(10);
	}
		while(f2){
			CircleLeft_R();
			f2=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_1);
			delay_nms(10);}
		brake();
}
void turn_1_1(){
	 f2=0;
	while(!f2){
			CircleRight_R();
			f2=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_2);
			delay_nms(10);
	}
		while(f2){
			CircleRight_R();
			f2=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_2);
			delay_nms(10);}
		brake();
}
void turn_2(){
  f1=0;
	while(!f1){
			CircleLeft_R();
			f1=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_1);
			delay_nms(10);
		}
		while(f1){
			CircleLeft_R();
			f1=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_1);
			delay_nms(10);
		}
}
void turn_2_1(){
  f2=0;
	while(!f2){
			CircleRight_R();
			f2=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_2);
			delay_nms(10);
		}
		while(f2){
			CircleRight_R();
			f2=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_2);
			delay_nms(10);
		}
}
//第二次来时去高杆转弯程序
u8 turn_22(void){
		u8 m;
	  u8 n;
		f1=0;
		if(GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_1)==1)
			m=1;
		else
			m=2;
		n=m;
	while(m){
		while(!f1){
			CircleRight_R();
			f1=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_2);
			delay_nms(10);
		}
		while(f1){
			CircleRight_R();
			f1=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_2);
			delay_nms(10);
		}
		m--;
	}
		brake();
	  return n;
}
u8 turn_22_1(void){
	u8 m;
	u8 n;
		f1=0;
		if(GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_2)==1)
			m=1;
		else
			m=2;
		n=m;
	while(m){
		while(!f1){
			CircleLeft_R();
			f1=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_1);
			delay_nms(10);
		}
		while(f1){
			CircleLeft_R();
			f1=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_1);
			delay_nms(10);
		}
		m--;
	}
		brake();
	return n;
}
//去高杆直走程序
void go_1(u8 m){
	m=m+1;
	while(m){
	 while(!f4){
			track1();
			f4=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_10);//7
			delay_nms(10);
	 }
		while(f4){
			track1();
			f4=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_10);//7
			delay_nms(10);
		}
		m--;
	}
  brake();
}
//高杆处的转弯
void turn_3(){
	 while(!f4){
			CarGoLeft_1();
			f4=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_10);//7
			delay_nms(10);
	 }
	 while(f4){
			CarGoLeft_1();
			f4=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_10);//7
			delay_nms(10);
	 }
		  brake();
}
void turn_3_1(){
	 while(!f6){
			CarGoLeft();
				f6=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_4);//5
			delay_nms(10);
	 }
	 while(f6){
			CarGoLeft();
				f6=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_4);//5
			delay_nms(10);
	 }
		  brake();
}
void goforwards_distance(float n_dis){
	float dis,dis1,dis2;
	dis1=GetDistance_A();
	delay_nms(60);
	dis2=GetDistance_A();
	delay_nms(60);
	dis=(dis1+dis2)/2.0;
	if(dis>n_dis)
	  while(dis>n_dis){
		  track1();
		  dis1=GetDistance_A();
		  delay_nms(60);
		  dis2=GetDistance_A();
		  delay_nms(60);
		  dis=(dis1+dis2)/2.0;
	  }
	else if(dis<n_dis)
	  while(dis<n_dis){
		 setWheel(1,1,40);
			setWheel(2,1,40);
		  dis1=GetDistance_A();
		  delay_nms(60);
		  dis2=GetDistance_A();
		  delay_nms(60);
		  dis=(dis1+dis2)/2.0;
	  }
	brake();
}
void action_1(){
   
	 if(a==0x01||a==0x02){
		if(a==0x01)
			USART_SendData(USART3, 'A');
		else
			USART_SendData(USART3, 'B');
		while(duoji_success==0){
			delay_nms(10);
		}
		duoji_success=0;
		flag_count++;
   }
	else if(a==0x03){
		high_flag1=carpos;
	}
	else if(a==0x04){
		high_flag2=carpos;
	}
	else ; 
	 
	 
	 a=0x00;
}
void action_1_1(){
   
	  if(a==0x07||a==0x08){
	
		if(a==0x07)
			USART_SendData(USART3, 'A');
		else
			USART_SendData(USART3, 'B');
		while(duoji_success==0){
			delay_nms(10);
		}
		duoji_success=0;
		flag_count++;
   }
	else if(a==0x05){
		high_flag1=carpos;
	}
	else if(a==0x06){
		high_flag2=carpos;
	}
	else ; 
	 
	 
	 a=0x00;
}
int main(void)
{
	RCC_Configuration();
	GPIO_Configuration();
  DIS_A_INT();
	DIS_A_INT_1();
	GPIOG_Configuration();
	USART3_Configuration();
	TIM3_Init();
	TIM_PWM_Init();
  //while(1){
	//	a=read2();
	//	disA=GetDistance_B();
	//	delay_nms(60);
	//}

	while(success==0){
		USART_SendData(USART3, 'a');//作为上电提示
		delay_nms(10);
	}
	success=0;
	//选择任务场地
	if(GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_0)==0)
	{
		mission = 1;
	}
	else
		{
			mission = 0;
		}
	mission=1;
	//第一阶段循迹，从出发区始发，向前行
		if(mission==1){
  goforwards_distance(80);
	carpos=1;
  while(a==(u8)0x00){
	  a=read2();
	}
	action_1();
	goforwards_distance(120);
	turn_0();
	
	while(carpos<8){
  go_n_step(1);
	while(a==(u8)0x00){
		a=read1();
	}
	if(a==0x01||a==0x02){
		turn_2();
		goforwards_distance(80);
		if(a==0x01)
			USART_SendData(USART3, 'A');
		else
			USART_SendData(USART3, 'B');
		while(duoji_success==0){
			delay_nms(10);
		}
		duoji_success=0;
		flag_count++;
		goforwards_distance(120);
		turn_0();
   }
	else if(a==0x03){
		high_flag1=carpos;
	}
	else if(a==0x04){
		high_flag2=carpos;
	}
	else ;
	carpos++;
	a=0x00;
	}
	//用temp1记录比较靠近旗杆的�
		if(high_flag1>high_flag2)
		{temp1=high_flag1;
			temp2=high_flag2;}
		else{temp2=high_flag1;
			temp1=high_flag2;}

	xi=1;
	TIM_Cmd(TIM3,ENABLE);//定时器用来转弯
	while(ruo!=3){
		delay_nms(10);
	}
		brake();
	while(dis_1>180||dis_1<10){
			track2();
	  	dis_11=GetDistance_B();
	  	delay_nms(60);
	  	dis_12=GetDistance_B();
	  	delay_nms(60);
	 		dis_1=(dis_11+dis_12)/2.0;
	}
	brake();
	
	USART_SendData(USART3, 'P');
	while(duoji_success==0){
		delay_nms(10);
	}
	duoji_success=0;
	go_back_1();
	turn_1();
	goforwards_distance(120);
	delay_nms(1000);
	turn_0();
	delay_nms(1000);
	back_n_step(8-temp1-1);
	turn_2();
	goforwards_distance(80);
	USART_SendData(USART3, 'A');
	while(duoji_success==0){
			delay_nms(10);
	}
	duoji_success=0;
	goforwards_distance(120);
	turn_0();
	back_n_step(temp1-temp2);
	turn_2();
	goforwards_distance(80);
	USART_SendData(USART3,'B');
	while(duoji_success==0){
			delay_nms(10);
	}
	duoji_success=0;
	goforwards_distance(120);
	turn_0();
	go_n_step(8-temp2-1);
	b=turn_22();
	go_1(b);
	turn_3();
	f1=1;
	while(f1){
		f1=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_12);
		if(f1==0){
			delay_nus(20);
			f1=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_12);
			if(f1==0){
				brake();
				break;
			}
		}
		track2();
	}
	USART_SendData(USART3, 'D');
}
else if(mission==0){
	goforwards_distance(80);
	carpos=1;
  while(a==(u8)0x00){
	  a=read2();
	}
	action_1_1();
	goforwards_distance(120);
	turn_0_1();
	
	while(carpos<8){
  go_n_step(1);
	while(a==(u8)0x00){
		a=read1();
	}
	if(a==0x07||a==0x08){
		turn_2_1();
		goforwards_distance(80);
		if(a==0x07)
			USART_SendData(USART3, 'A');
		else
			USART_SendData(USART3, 'B');
		while(duoji_success==0){
			delay_nms(10);
		}
		duoji_success=0;
		flag_count++;
		goforwards_distance(120);
		turn_0_1();
   }
	else if(a==0x05){
		high_flag1=carpos;
	}
	else if(a==0x06){
		high_flag2=carpos;
	}
	else ;
	carpos++;
	a=0x00;
	}
	//用temp1记录比较靠近旗杆的�
		if(high_flag1>high_flag2)
		{temp1=high_flag1;
			temp2=high_flag2;}
		else{temp2=high_flag1;
			temp1=high_flag2;}
	xi=2;
	TIM_Cmd(TIM3,ENABLE);//定时器用来转弯
	while(ruo!=3){
		delay_nms(10);
	}
		brake();
	while(dis_1>180||dis_1<10){
			track2();
	  	dis_11=GetDistance_B();
	  	delay_nms(60);
	  	dis_12=GetDistance_B();
	  	delay_nms(60);
	 		dis_1=(dis_11+dis_12)/2.0;
	}
	brake();
	
	USART_SendData(USART3, 'P');
	while(duoji_success==0){
		delay_nms(10);
	}
	duoji_success=0;
	go_back_1();
	turn_1_1();
  goforwards_distance(120);
	delay_nms(1000);
	turn_0_1();
	delay_nms(1000);
	back_n_step(8-temp1-1);
	turn_2_1();
	goforwards_distance(80);
	USART_SendData(USART3, 'A');
	while(duoji_success==0){
			delay_nms(10);
		}
		duoji_success=0;
	goforwards_distance(120);
	turn_0_1();
	back_n_step(temp1-temp2);
	turn_2_1();
	goforwards_distance(80);
	USART_SendData(USART3,'B');
	while(duoji_success==0){
			delay_nms(10);
		}
	duoji_success=0;
	goforwards_distance(120);
	turn_0_1();
	go_n_step(8-temp2-1);
	b=turn_22_1();
	go_1(b);
	turn_3_1();
	f1=1;
	while(f1){
		f1=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_12);
		if(f1==0){
			delay_nus(20);
			f1=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_12);
			if(f1==0){
				brake();
				break;
			}
		}
		track2();
	}
	USART_SendData(USART3, 'D');
}
else
	;
}
void TIM3_IRQHandler(void){
  TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	i++;
	if(i==10){
		f3=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_3);//8
		f4=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_10);//7
		f5=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_11);//6
		f6=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_4);//5
		f11=GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_7);//第一次转弯的判断
		i=0;
	} 
	
	
   if(xi==1){
	   if(ruo==0){
	      CarGoRight();
	   if(f4==1){
			 brake();
		   ruo=1;
		 }
	  }
	  if(ruo==1){
	     goforwards();
	  if(f5==1){brake();
		ruo=2;
		}
	 }
	  if(ruo==2){
	     CircleLeft_R();
	     if((f3==0)&&(f4==0)&&(f5==0)&&(f6==0)){
				 brake();
		     ruo=3;
		     TIM_Cmd(TIM3,DISABLE);
	     }
	  }}
	  else if(xi==2)
			
		  {
				if(ruo==0){
	      CarGoRight_1();
	   if(f6==1){
			 brake();
		   ruo=1;
		 }
	  }
	  if(ruo==1){
	     goforwards();
	  if(f3==1){brake();
		ruo=2;
		}
	 }
	  if(ruo==2){
	     CircleRight_R();
	     if((f3==0)&&(f4==0)&&(f5==0)&&(f6==0)){
				 brake();
		     ruo=3;
		     TIM_Cmd(TIM3,DISABLE);
	     }
			}}
		else
			;
	}

		



/*蓝牙模块*/
void USART3_RCC_Configuration(void){
	 RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
 //  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);   //打开GPIOA,AFIO,USART1外设时钟
  
}
void nvic(void){
	 NVIC_InitTypeDef NVIC_InitStructure;  //	 //	  命名一优先级变量

 	   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);    //	   将优先级分组方式配置为group1,有2个抢占（打断）优先级，8个响应优先级
     NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn; //该中断为TIM2溢出更新中断
     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//打断优先级为1，在该组中为较低的，0优先级最高
     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; // 响应优先级0，打断优先级一样时，0最高
     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	 	//	设置使能
     NVIC_Init(&NVIC_InitStructure);					   	//	初始化

	 	 NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);    //	   将优先级分组方式配置为group1,有2个抢占（打断）优先级，8个响应优先级
     NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; //该中断为TIM2溢出更新中断
     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//打断优先级为1，在该组中为较低的，0优先级最高
     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; // 响应优先级0，打断优先级一样时，0最高
     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	 	//	设置使能
     NVIC_Init(&NVIC_InitStructure);					   	//	初始化 
	
	   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1); //要用同一个Group
     NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn; //TIM3	溢出更新中断
     NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;//	打断优先级为1，与上一个相同，不希望中断相互打断对方
     NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 	//	响应优先级1，低于上一个，当两个中断同时来时，上一个先执行
     NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
     NVIC_Init(&NVIC_InitStructure);
}
void USART3_Configuration(void){
	  USART_InitTypeDef USART_InitStructure; //串口设置恢复默认参数
  
	  USART3_RCC_Configuration();
	  nvic();
	  
	  USART_InitStructure.USART_BaudRate = 38400; //波特率38400
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; //字长8位
    USART_InitStructure.USART_StopBits = USART_StopBits_1; //1位停止字节
    USART_InitStructure.USART_Parity = USART_Parity_No; //无奇偶校验
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;     //无流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;      //打开Rx接收和Tx发送功能
    USART_Init(USART3, &USART_InitStructure);  //初始化
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);  // 若接收数据寄存器满，则产生中断
    USART_Cmd(USART3, ENABLE);                     //启动串口                   
                                                   
       //-----如下语句解决第1个字节无法正确发送出去的问题-----// 
    USART_ClearFlag(USART3, USART_FLAG_TC);     // 清发送完成标志
}
void USART3_IRQHandler(void){
	if(USART_GetITStatus(USART3,USART_IT_RXNE)!=RESET) //判断是否为接收中断
	{
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);	//清除接受中断标志位
		RxData=USART_ReceiveData(USART3);								//读取接受数据{
			switch(RxData)
			{
				case (u8)0x53:success=1; break;
				case (u8)0x54:duoji_success=1;break;
				default:break;
			}
		
	}
	
	
	if(USART_GetITStatus(USART3,USART_IT_TXE)!=RESET)  //判断是否为发送中断
	{	
		USART_ClearITPendingBit(USART3,USART_IT_TXE);//清除发送中断标志位
		
	}
	if(USART_GetITStatus(USART3,USART_IT_TC)!=RESET)  //判断是否为发送完成中断
	{
		USART_ClearITPendingBit(USART3,USART_IT_TC);	//清除发送完成中断标志位
	}

 }	 
/*rfid读卡器函数*/
//前置rfid读卡器
u8 read1()
{
	u16 times=0;
	u8 uid[8]={0};
	u8 block[4]={0};
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart1_init(19200);	 //串口1,2初始化为19200
 	t1=RFID_Init1();
	t1=RFID_Init1();
	
 	while(block[0]==0&&(times<10))
	{
		t2=GET_UID1(uid);//获取UID
		t2=GET_UID1(uid);//获取UID
	
		t3=RFID_READ1(uid,block);//读取rfid第一块数据
		times++;
	}	 
	t4=RFID_Disableantenna1();//关闭天线，减少发热 
	return block[0];
}
//侧置rfid读卡器
u8 read2()
{
	u8 t=0;
	u16 times=0;
	u8 uid[8]={0};
	u8 block[4]={0};
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	uart2_init(19200);	 //串口1,2初始化为19200
	
	t=RFID_Init2();
	t=RFID_Init2();
	
	//delay_ms(300);
	
 	while(block[0]==0&&(times<10))
	{
		t=GET_UID2(uid);//获取UID
		t=GET_UID2(uid);//获取UID
	
		t=RFID_READ2(uid,block);//读取rfid第一块数据
		times++;
	}	 
	t=RFID_Disableantenna2();//关闭天线，减少发热 
	return block[0];
}
/*while(ii<8)
		
		
		{go_one_step();
		a=read1();//读取rfid
		if(RxData_t[k]==0x01||RxData_t[k]==0x02){//如果是“红”、“专”
						USART_SendData(USART3, 'A');
					  adjust_sign=1;
			  }
			   else if(RxData_t[k]==0x04){                //如果是“并”
				   USART_SendData(USART3,'A');
			  }
			  else if(RxData_t[k]==0x03){
				  USART_SendData(USART3,'B');     //如果是”进“
			  }
				else 
					;
				ii++;}*/
		
				
		///////////////判断操作结束
	/*	*/
 //第二阶段循迹，转弯并向前走
	
		//goforwards();	
	//setWheel(1,direction,40);
/*	setWheel(2,direction,40);
	while(sign!=1);
  if(sign==1){         //对第一根柱子进行检测
			 USART_SendData(USART3, 'F');//要求读取前方的rfid读卡器
		   while(success!=1);          //等待收到反馈
		   delay_nms(20);
		   success=0;
		   if(mission==0){
			    
	  }
			 else{
				 if(RxData_t[k]==0x05||RxData_t[k]==0x06){//如果是“理”、“实”
						USART_SendData(USART3, 'A');
					  adjust_sign=1;
			  }
			   else if(RxData_t[k]==0x07){                //如果是“交“
				   USART_SendData(USART3,'A');
			 }
			  else if(RxData_t[k]==0x08){
				  USART_SendData(USART3,'B');     //如果是”融”
			 }
		   }
			 while(success!=1);                 //等待收到成功反馈
			 success=0;
	     back_adjust();
			 while(dis<=150);
			 brake();
			 delay_nms(20);
			 if(mission==0)
			    CircleRight_R();
			 else
				  CircleLeft_B();
			 sign=2;                            //第二阶段的循迹
			 TIM_Cmd(TIM3,DISABLE);             //关闭定时器
	     car_pos=1;                         //记录所在位置为1
	}*/