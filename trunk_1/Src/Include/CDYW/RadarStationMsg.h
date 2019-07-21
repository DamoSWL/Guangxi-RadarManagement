#ifndef _RADARSTATIONMSG_H_
#define _RADARSTATIONMSG_H_

typedef struct _FTP_MSG_
{
	char ftpIP[64];		//IP地址
	char ftpPort[12];	//端口号
	char ftpUser[32];	//用户名
	char ftpPass[32];	//密码
	char filePath[255];	//文件存放文件夹路径
	unsigned int interval;//时间间隔
}tagFtpMsg;

enum TransDataFormate
{
	TransDataFormate_SA = 1,
	TransDataFormate_SB = 2,
	TransDataFormate_CB = 3,
	TransDataFormate_SC = 4,
	TransDataFormate_CD = 5,
	TransDataFormate_CC1 = 6,
	TransDataFormate_VTB = 7,
	TransDataFormate_CA = 8,
	TransDataFormate_SCD2SA = 9,//原雷达为SC或CD，上传国家局时转换为一个类似与SA格式的数据
	TransDataFormate_WRX100 = 10,
	TransDataFormate_WSR88D = 11
};

typedef struct _RADARSTATION_MSG_
{
	char radarCd[32]; 		//雷达编号
	char area[32]; 			//区域信息
	char station[32]; 		//所属站点
	char type[32]; 			//雷达类型
	long longitude; 		//雷达经度
	long latitude;  		//雷达纬度
	long altitude;			//海拔高度
	double wav_length; 		//波束长度
	double wav_width; 		//波束宽度
	unsigned short int TimeFormat[6];	//时间解析偏移地址，按照年月日时分秒 如0,4(年) 4,6(月) 等
	TransDataFormate dataformate;	//数据类型
	vector<tagFtpMsg> ftpMsgs;		//ftp相关信息
}tagRadarStationMsg;

typedef struct _FILES_MSG_
{
	char station[255];
	char filePath[300];
}tagFileMsg;

#endif