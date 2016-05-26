#include "includes.h"
BYTE rev_ch;
WORD helm_use=0;
int16_t motor_use=0;
int direction;
BYTE haha;
int supersonic_on_off=1;
int biaoji=0;
int jishu=0;
int jia=0;
int i;
int j;
int LichtZahl=0;
int Light_success=1;
int TestLight=0;
BYTE video_sender;

BYTE waiting_for_response=0;
int  lost_data=0;
BYTE prepared_data_for_ancillary;

void Mode0_DebugCamera(void);
void Lightcontrol(void);
void Light_wifi(BYTE LightCon);
void Mode1_SendVideo(void);
void Mode2_GO(void);
void Mode3_Andriod(void);

void main_wifi_sender (BYTE data_input);
void ancillary_wifi_sender (BYTE data_input2);
void wifi_sender_checker(void);
void wifi_receive_checker (void);
void main(void)


{
	init_all_and_POST();
	LCD_Fill(0x00);
				
	EMIOS_0.CH[3].CCR.B.FEN=1;//开场中断
	LCD_write_english_string(96,0,"T");
	LCD_write_english_string(96,2,"R");
	Lightcontrol();
	

}



void Lightcontrol(void)
{
	StopL=1;
	RightL=0;
	RunL=0;
	for(;;)
	{
//		LCD_PrintoutInt(0, 4, (int)remote_frame_data[0]);
//		LCD_PrintoutInt(10, 4, (int)remote_frame_data[1]);
//		LCD_PrintoutInt(20, 4, (int)remote_frame_data[2]);
		LCD_PrintoutInt(30, 4, (int)remote_frame_data[3]);
//		LCD_PrintoutInt(40, 4, (int)remote_frame_data[4]);
//		LCD_PrintoutInt(50, 4, (int)remote_frame_data[5]);
//		LCD_PrintoutInt(60, 4, (int)remote_frame_data[6]);
//		LCD_PrintoutInt(70, 4, (int)remote_frame_data[7]);
//		LCD_PrintoutInt(80, 4, (int)remote_frame_data[8]);
//		LCD_PrintoutInt(90, 4, (int)remote_frame_data[9]);
		LCD_PrintoutInt(0, 2, (int)lost_data);
		if(LightCC==1)
		{
			LichtZahl++;
			LightCC=0;
		}
		if(LichtZahl==1&&RightL==1)
		{
			LichtZahl=0;
			StopL=1;
			RightL=0;
			main_wifi_sender(0x0A);
		}
		if(LichtZahl==8&&StopL==1)
		{
			LichtZahl=0;
			StopL=0;
			RunL=1;
			main_wifi_sender(0x0B);
		}
		if(LichtZahl==7&&RunL==1)
		{
			LichtZahl=0;
			RunL=0;
			RightL=1;
		}
		if(LightCWifi==1)            // 有一个时间间隔为了 保证在没有收到的时候不会发疯一样发
		{
			LightCWifi=0;
			wifi_sender_checker();//每次检查一次是否收到回复  注意：子函数在被设计为发送完一定时间内不会工作，防止对方还没回答这里不停发
			wifi_receive_checker();    // 收到数据回复我收到了 by xiong
		}
		
			
	}
}



//*********************************************************************************
//  主发送程序                 输入： 发送所需的数据      输出： 1 串口发送      2  waiting位     3 串口发送备份给备发送程序    4 发送丢失数
//*********************************************************************************
void main_wifi_sender (BYTE data_input)
{  
//	***********如果依然在等待回复，放弃上一个发送的等待，并且lostdata数加一***************
	if (waiting_for_response==1)
	{
	   lost_data++;
	   waiting_for_response=0;
	}
//	***********发送函数主体***************	                                    
	if(data_input==0x0A)				
		
		    rfid_ask_road(0xDD, 0x33, 0x04, 0x00DD000A);

	if(data_input==0x0B)
			rfid_ask_road(0xDD, 0x33, 0x04, 0x00DD000B);
//  ***********把发送数据交给辅助发送程序*************** 
	prepared_data_for_ancillary=data_input;
//  ***********等待回复位置1*************** 
	waiting_for_response=1;
	have_responsed=0;  
	sending_waiter=0;
}




//*********************************************************************************
//  辅助发送程序                 输入： 如果未应答，再发送数据      输出：  串口发送    
//*********************************************************************************
void ancillary_wifi_sender (BYTE data_input2)
{                                      
	if(data_input2==0x0A)				
		
		    rfid_ask_road(0xDD, 0x33, 0x04, 0x00DD000A);

	if(data_input2==0x0B)
			rfid_ask_road(0xDD, 0x33, 0x04, 0x00DD000B);
}



//*********************************************************************************
//  应答检查程序               定时检查发送的数据是否得到了应答，若未，则使用辅助发送程序再次发送。 直到收到应答或有新的程序要发数据。
//*********************************************************************************
void wifi_sender_checker (void)
{ 
	if (sending_waiter<5)
	{
		return;
	}
	else
	{
		if (waiting_for_response==1)
		{
			if (have_responsed==1)
			{
				waiting_for_response=0;
			}
			else if (have_responsed==0)
			{
				ancillary_wifi_sender (prepared_data_for_ancillary);
			}
		}
	}
}


//*********************************************************************************
//  收数据检查程序              如果受到了非应答的程序（即实打实的命令），回复收到
//*********************************************************************************
void wifi_receive_checker (void)
{
	if (order_received == 1)
	{
		order_received=0;
		rfid_ask_road(0xDD, 0x33, 0x04, 0x00000000);
	}
}















void Mode0_DebugCamera(void)
{
//	set_speed_target(20);
	EMIOS_0.CH[3].CCR.B.FEN=1;//开场中断
	LCD_write_english_string(96,0,"T");
	LCD_write_english_string(96,2,"R");
	
	for (;;)
	{
		control_car_action();//ouyang
		if(fieldover)
		{
			fieldover=0;                                              
			set_speed_pwm(800); 
			FindBlackLine();
	    //	Display_Video();
			CenterLineWithVideo();
	     	Video_Show();
	    // 	Send_CCD_Video();
	    // 	supersonic();
#if 0
	     	for(video_sender=0;video_sender<70;video_sender++)
	     	{
	     		serial_port_0_TX(CenterLine[video_sender]);
	     	}
#endif
	     	
			if(target_offset<0)
				LCD_write_english_string(96,1,"-");
			else LCD_write_english_string(96,1,"+");
			LCD_Write_Num(105,1,ABS(target_offset),2);
			LCD_Write_Num(105,2,RoadType,2);
#if 0
			if(flag_Rightangle_l)	
			{ 
				set_speed_pwm(500);
				set_steer_helm_basement(data_steer_helm_basement.left_limit);   
				LCD_Write_Num(105,3,(int)flag_Rightangle_l,4);
			}
			else if(flag_Rightangle_r)		
			{ 
				set_speed_pwm(500);
				set_steer_helm_basement(data_steer_helm_basement.right_limit);   
				LCD_Write_Num(105,4,(int)flag_Rightangle_r,4);
			}
			else
#endif
				SteerControl();

			EMIOS_0.CH[3].CSR.B.FLAG = 1;
			EMIOS_0.CH[3].CCR.B.FEN=1;
		}
	}
}

void Mode1_SendVideo(void)
{
	//SetupCCD();	
	g_f_enable_speed_control = 0;
	EMIOS_0.CH[3].CCR.B.FEN=1;//开场中断

	for (;;)
	{

		if(fieldover)
		{
			fieldover=0;
			FindBlackLine();
		//	Send_CCD_Video();
			
			EMIOS_0.CH[3].CSR.B.FLAG = 1;
			EMIOS_0.CH[3].CCR.B.FEN=1;
		}
		
	}

}

void Mode2_GO(void)
{
	//set_steer_helm_basement(3800);
	set_speed_pwm(240);
	EMIOS_0.CH[3].CCR.B.FEN=1;//开场中断

#if 0
	for (;;)
	{
		trigger_supersonic_0();
        get_supersonic_time_0();
	    trigger_supersonic_2();
		get_supersonic_time_2();
#if 0
		if((ABS((WORD)(tmp_time.R))/100)<100)
		{
			trigger_supersonic_2();
			get_supersonic_time_2();
			LCD_Write_Num(96,6,(ABS((WORD)(tmp_time.R))/100),5);
			set_speed_pwm(0);
		}
#endif
		if((ABS((WORD)(tmp_time2.R))/100)<70 && (ABS((WORD)(tmp_time.R))/100)<100)
			jishu++;
		else if((ABS((WORD)(tmp_time2.R))/100)>70 && (ABS((WORD)(tmp_time.R))/100)>100)
			jishu=0;
		if((ABS((WORD)(tmp_time2.R))/100)<270 && (ABS((WORD)(tmp_time.R))/100)<270)
		{
			biaoji++;
			trigger_supersonic_0();
			get_supersonic_time_0();
			trigger_supersonic_2();
			get_supersonic_time_2();
			LCD_Write_Num(96,6,(ABS((WORD)(tmp_time2.R))/100),5);
			LCD_Write_Num(96,5,(ABS((WORD)(tmp_time.R))/100),5);
		}
		else if((ABS((WORD)(tmp_time2.R))/100)>270 && (ABS((WORD)(tmp_time.R))/100)>270)
		{
			biaoji=0;
			trigger_supersonic_0();
			get_supersonic_time_0();
			trigger_supersonic_2();
			get_supersonic_time_2();
			LCD_Write_Num(96,6,(ABS((WORD)(tmp_time2.R))/100),5);
			LCD_Write_Num(96,5,(ABS((WORD)(tmp_time.R))/100),5);
		}

	//	else if((ABS((WORD)(tmp_time.R))/100)<100)
	//	{
	//		set_speed_pwm(0);
	//		delay_ms(3000);
	//	}
		if(biaoji>=300)
		{
		//	trigger_supersonic_0();
		//	get_supersonic_time_0();
		//	trigger_supersonic_2();
		//	get_supersonic_time_2();
		//	LCD_Write_Num(96,6,(ABS((WORD)(tmp_time.R))/100),5);	
			//for (i = 0; i < 500; i++)
			//	{
				//	for (j = 0; j < 9; j++)
				///	{			
						set_steer_helm_basement(4800);
						set_speed_pwm(210);
						delay_ms(1000);
					//	trigger_supersonic_2();
					//	get_supersonic_time_2();
					//	LCD_Write_Num(96,6,(ABS((WORD)(tmp_time.R))/100),5);
					//}
			//	}
			//for (i = 0; i < 500; i++)
				//{
				//	for (j = 0; j < 9; j++)
				//	{			
						set_steer_helm_basement(2700);
						set_speed_pwm(320);
						delay_ms(1300);
					//	trigger_supersonic_2();
					//	get_supersonic_time_2();
					//	LCD_Write_Num(96,6,(ABS((WORD)(tmp_time.R))/100),5);
				//	}
				//}
			set_steer_helm_basement(STEER_HELM_CENTER);
			set_speed_pwm(240);
		//	delay_ms(2000);//////////////
			biaoji=0;
			jishu=0;
			//jishu++;
			//LCD_Write_Num(0,0,(ABS((WORD)(jishu))/100),5);
			//biaoji=0;
		//	set_speed_pwm(0); ////////////////
			//suicide();////////////
		}
		trigger_supersonic_0();
		get_supersonic_time_0();
		trigger_supersonic_2();
		get_supersonic_time_2();
		if(jishu>=100)
		{
			set_speed_pwm(0);
			suicide();
		}
	}
#endif

	for (;;)
	{
	    trigger_supersonic_2();
		get_supersonic_time_2();
		if((ABS((WORD)(tmp_time2.R))/100)<270)
		{
			biaoji++;
			trigger_supersonic_2();
			get_supersonic_time_2();
			LCD_Write_Num(96,6,(ABS((WORD)(tmp_time2.R))/100),5);
		}
		else if((ABS((WORD)(tmp_time2.R))/100)>270)
		{
			biaoji=0;
			trigger_supersonic_2();
			get_supersonic_time_2();
			LCD_Write_Num(96,6,(ABS((WORD)(tmp_time2.R))/100),5);
		}
		if(biaoji>=300)
		{	
			set_steer_helm_basement(4800);
			set_speed_pwm(210);
			delay_ms(1000);
			set_steer_helm_basement(2700);
			set_speed_pwm(210);
			delay_ms(1300);
			set_steer_helm_basement(STEER_HELM_CENTER);
			set_speed_pwm(240);
			biaoji=0;
		}
		EMIOS_0.CH[3].CSR.B.FLAG = 1;
		EMIOS_0.CH[3].CCR.B.FEN=1;
	}

	
#if 0
		if(biaoji==0)
		{
			while((jishu-jia)>0)
			{
				for (i = 0; i < 10; i++)
				{
					for (j = 0; j < 9; j++)
					{			
						set_steer_helm_basement(2802);
						set_speed_pwm(300);
						trigger_supersonic_2();
						get_supersonic_time_2();
						LCD_Write_Num(96,6,(ABS((WORD)(tmp_time.R))/100),5);
					}
				}
				jishu--;
			}
			biaoji=1;
			
		}
#endif
		LCD_Fill(0x00);
for (;;)
{
		//set_speed_pwm(300);
	//	set_steer_helm_basement(3875);
		/* 执行远程命令 */
		if (REMOTE_FRAME_STATE_OK == g_remote_frame_state)
		{
			g_remote_frame_state = REMOTE_FRAME_STATE_NOK;
			
			execute_remote_cmd(remote_frame_data+5);
		}

		/* 整车动作控制 */
		control_car_action();
		//fieldover=1;//尝试
		if(fieldover)
		{
			fieldover=0;
			
			FindBlackLine();
		//	if(g_f_red==1&&g_f_stopline==1)
		//	{
				//set_speed_target(0);
			//	set_speed_pwm(0);
			//	D6=~D6;
			//}
			//else
				set_speed_pwm(300);
				//set_speed_target(20);
			Display_Video();



						if(target_offset<0)
							LCD_write_english_string(96,1,"-");
						else LCD_write_english_string(96,1,"+");
						LCD_Write_Num(105,1,ABS(target_offset),2);
						LCD_Write_Num(105,2,RoadType,2);
			SteerControl();
			
			EMIOS_0.CH[3].CSR.B.FLAG = 1;
			EMIOS_0.CH[3].CCR.B.FEN=1;
		}
}
}
void Mode3_Andriod(void)
{
	 LCD_Fill(0x00);
	for(;;)
    {
		   // LCD_PrintoutInt(48, 0, (int)haha);
		  // LCD_PrintoutInt(48, 0, (int)shuzi);
		   // LCD_PrintoutInt(48, 0, (int)rev_ch);
		    LCD_PrintoutInt(48, 0, (SWORD)motor_use);
		  
		// ---------------------交给控制--------------------	
		   if(haha==1)
	        {
			   if (direction==1)
			
		    	{
		    		set_steer_helm_basement(helm_use); //舵机调参
		    	}
		        else if (direction==5)
		    	{
		    		set_speed_pwm(motor_use);
		    	}
		        else if (direction==11)
				{
		        	supersonic_on_off=~supersonic_on_off;
				}
		     haha=0;
	        } 
		   if (supersonic_on_off)
		   {
			   trigger_supersonic_0();
			   get_supersonic_time_0();
			   trigger_supersonic_1();
			   get_supersonic_time_1();
			   while(((ABS((WORD)(tmp_time.R))/100)<200)  && (motor_use>0))
			   {
					trigger_supersonic_0();
					get_supersonic_time_0();
					trigger_supersonic_1();
					get_supersonic_time_1();
					LCD_Write_Num(96,6,(ABS((WORD)(tmp_time.R))/100),5);
					set_speed_pwm(0);
				}	
				LCD_Fill(0x00);
				set_speed_pwm(motor_use);
		   }
	}
	
}
