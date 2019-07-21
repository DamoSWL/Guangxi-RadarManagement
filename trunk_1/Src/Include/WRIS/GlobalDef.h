#ifndef _GLOBALDEF_
#define _GLOBALDEF_

#include "Format.h"

#define OUTMAXRBINS			10000						//һ����Ʒ���������
#define PPIMAXRAD			600							//һȦ��������
#define ONEPROMAXSIZE		(OUTMAXRBINS*PPIMAXRAD)		//һȦ�����������һ��Z��Ʒ��

#define MAX_VOL_SIZE  (200*1000*1000) // ������ɨ�ļ��������
#define MAX_PRO_SIZE  (10*1000*1000) // ������Ʒ�ļ��������

#define BZ2_VOLMAXBUFF	MAX_VOL_SIZE
#define BZ2_MAXBUFF		MAX_PRO_SIZE

#define MAXPRONUM				8	//һ����ɨ֧�ֵ�����Ʒ��
#define MAXSIZEONEPRO			OUTMAXRBINS	//ÿ�ֻ����ݵ����prt����
#define MAXBASEDATASIZE			(MAXSIZEONEPRO*MAXPRONUM)

enum _WaveForm_{
	WAVEFORM_CS=0,	//��������ģʽ
	WAVEFORM_CD=1,	//���о���ģ��������������ģʽ
	WAVEFORM_HCD=2,	//����������������ģʽ
	WAVEFORM_B=4,	//�˾���ģ����������ģʽ
	WAVEFORM_DPRF=5,//���ٶ�ģ����������ģʽ
	WAVEFORM_TPRF=6,//�˾��롢�ٶ�ģ����������ģʽ
	WAVEFORM_PE=7,	//��λ����ģʽ
	WAVEFORM_ZPKRH	//ZVW ZDR	PDP	KDP	RHV	HCL
};
enum _NewWaveForm_{
	NewWAVEFORM_CS=0,	//R
	NewWAVEFORM_CD=1,	//V W
	NewWAVEFORM_HCD=2,	//RVW
	NewWAVEFORM_B=4,	//RVW
	NewWAVEFORM_DPRF=5,//RVW
	NewWAVEFORM_TPRF=6,//RVW
	NewWAVEFORM_PE=7,	//RVWP
	NewWAVEFORM_ZPKRH	//ZVW ZDR	PDP	KDP	RHV	HCL
};

#endif