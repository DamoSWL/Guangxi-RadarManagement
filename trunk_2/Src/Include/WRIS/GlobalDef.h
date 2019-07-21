#ifndef _GLOBALDEF_
#define _GLOBALDEF_

#include "Format.h"

#define OUTMAXRBINS			10000						//一个产品最大距离库数
#define PPIMAXRAD			600							//一圈最大径向个数
#define ONEPROMAXSIZE		(OUTMAXRBINS*PPIMAXRAD)		//一圈最大数据量（一个Z产品）

#define MAX_VOL_SIZE  (200*1000*1000) // 单个体扫文件允许最大
#define MAX_PRO_SIZE  (10*1000*1000) // 单个产品文件允许最大

#define BZ2_VOLMAXBUFF	MAX_VOL_SIZE
#define BZ2_MAXBUFF		MAX_PRO_SIZE

#define MAXPRONUM				8	//一个体扫支持的最大产品数
#define MAXSIZEONEPRO			OUTMAXRBINS	//每种基数据的最大prt个数
#define MAXBASEDATASIZE			(MAXSIZEONEPRO*MAXPRONUM)

enum _WaveForm_{
	WAVEFORM_CS=0,	//连续警戒模式
	WAVEFORM_CD=1,	//具有距离模糊的连续多普勒模式
	WAVEFORM_HCD=2,	//高仰角连续多普勒模式
	WAVEFORM_B=4,	//退距离模糊的批处理模式
	WAVEFORM_DPRF=5,//退速度模糊的批处理模式
	WAVEFORM_TPRF=6,//退距离、速度模糊的批处理模式
	WAVEFORM_PE=7,	//相位编码模式
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