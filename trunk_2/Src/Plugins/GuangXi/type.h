#pragma once

typedef struct
{
	char  STATUS_DATA[9];        //日期
	char  STATUS_TIME[7];        //时间

	int   SEL_PATT;              //体扫模式 11，21，31
	int   LOCALFLG;              //控制标志  LOCAL = 2；REMOTE = 4；EITHER = 8

	float ANT_PEAK_PWR;           //天线峰值功率
	float XMTR_PEAK_PWR;          //发射机峰值功率
	float ANT_AVG_PWR;            //天线平均功率
	float XMTR_AVG_PWR;           //发射机平均功率
	float POWER_METER_RATIO_DB;   
	float ANT_PWR_MTR_ZERO;       //天线功率调零
	float XMTR_PWR_MTR_ZERO;      //发射机功率调零
	int   TX_RECYCLE_CNT;         //no use
	short spare1[2];              
	float RNSPLN;                 //短脉冲噪声电平
	float RNSPLG;                 //no use
	float RNLPLN;                 //长脉冲噪声电平
	float RNLPLG;                 //no use
	float TE;                     //噪声温度 
	short IDU_TEST_DETECTIONS;    
	short SysStatus;  //1表示系统正常，2表示系统可用，3表示系统需要维护，4表示系统故障不能工作，5表示系统关机或者STANDBY。
	float AGC_STEP_AMPL[6];       //no use
	float AGC_STEP_PHASE[6];      //no use
	float AGC_IQ_AMPL_BAL;        //no use
	float AGC_IQ_PHASE_BAL;       //no use
	short spare3[20];           
	float EXPECTED_REFL_AMP[8]; //反射率期望值
	float MEASURED_REFL_AMP[8]; //反射率测量值
	float SYSSPLN;              //短脉冲系统标定常数
	float SYSSPLG;              //no use
	float SYSLPLN;              //长脉冲系统标定常数
	float SYSLPLG;              //no use
	float EXPECTED_VELOCITY[4]; //速度期望值
	float MEASURED_VELOCITY[4]; //速度测量值
	float EXPECTED_WIDTH[4];    //谱宽期望值
	float MEASURED_WIDTH[4];    //谱宽测量值
	float SHELTER_TEMP; 	    //机房温度        
	float XMTR_AIR_TEMP;        //发射机空气温度  
	float RADOME_TEMP;          //天线罩温度      
	short spare4[12];         
	float EXPECTED_RF8_AMP[6];  //KD标定期望值
	float MEASURED_RF8_AMP[6];  //KD标定测量值
	short spare5[16];          
	float UNFILTERED_LIN_CHAN_PWR; //滤波前功率
	float FILTERED_LIN_CHAN_PWR;   //滤波后功率
}RADAR_STATUS_DATA;