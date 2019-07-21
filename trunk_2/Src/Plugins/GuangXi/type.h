#pragma once

typedef struct
{
	char  STATUS_DATA[9];        //����
	char  STATUS_TIME[7];        //ʱ��

	int   SEL_PATT;              //��ɨģʽ 11��21��31
	int   LOCALFLG;              //���Ʊ�־  LOCAL = 2��REMOTE = 4��EITHER = 8

	float ANT_PEAK_PWR;           //���߷�ֵ����
	float XMTR_PEAK_PWR;          //�������ֵ����
	float ANT_AVG_PWR;            //����ƽ������
	float XMTR_AVG_PWR;           //�����ƽ������
	float POWER_METER_RATIO_DB;   
	float ANT_PWR_MTR_ZERO;       //���߹��ʵ���
	float XMTR_PWR_MTR_ZERO;      //��������ʵ���
	int   TX_RECYCLE_CNT;         //no use
	short spare1[2];              
	float RNSPLN;                 //������������ƽ
	float RNSPLG;                 //no use
	float RNLPLN;                 //������������ƽ
	float RNLPLG;                 //no use
	float TE;                     //�����¶� 
	short IDU_TEST_DETECTIONS;    
	short SysStatus;  //1��ʾϵͳ������2��ʾϵͳ���ã�3��ʾϵͳ��Ҫά����4��ʾϵͳ���ϲ��ܹ�����5��ʾϵͳ�ػ�����STANDBY��
	float AGC_STEP_AMPL[6];       //no use
	float AGC_STEP_PHASE[6];      //no use
	float AGC_IQ_AMPL_BAL;        //no use
	float AGC_IQ_PHASE_BAL;       //no use
	short spare3[20];           
	float EXPECTED_REFL_AMP[8]; //����������ֵ
	float MEASURED_REFL_AMP[8]; //�����ʲ���ֵ
	float SYSSPLN;              //������ϵͳ�궨����
	float SYSSPLG;              //no use
	float SYSLPLN;              //������ϵͳ�궨����
	float SYSLPLG;              //no use
	float EXPECTED_VELOCITY[4]; //�ٶ�����ֵ
	float MEASURED_VELOCITY[4]; //�ٶȲ���ֵ
	float EXPECTED_WIDTH[4];    //�׿�����ֵ
	float MEASURED_WIDTH[4];    //�׿����ֵ
	float SHELTER_TEMP; 	    //�����¶�        
	float XMTR_AIR_TEMP;        //����������¶�  
	float RADOME_TEMP;          //�������¶�      
	short spare4[12];         
	float EXPECTED_RF8_AMP[6];  //KD�궨����ֵ
	float MEASURED_RF8_AMP[6];  //KD�궨����ֵ
	short spare5[16];          
	float UNFILTERED_LIN_CHAN_PWR; //�˲�ǰ����
	float FILTERED_LIN_CHAN_PWR;   //�˲�����
}RADAR_STATUS_DATA;