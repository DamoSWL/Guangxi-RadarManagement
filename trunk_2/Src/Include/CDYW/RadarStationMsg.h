#ifndef _RADARSTATIONMSG_H_
#define _RADARSTATIONMSG_H_

typedef struct _FTP_MSG_
{
	char ftpIP[64];		//IP��ַ
	char ftpPort[12];	//�˿ں�
	char ftpUser[32];	//�û���
	char ftpPass[32];	//����
	char filePath[255];	//�ļ�����ļ���·��
	unsigned int interval;//ʱ����
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
	TransDataFormate_SCD2SA = 9,//ԭ�״�ΪSC��CD���ϴ����Ҿ�ʱת��Ϊһ��������SA��ʽ������
	TransDataFormate_WRX100 = 10,
	TransDataFormate_WSR88D = 11
};

typedef struct _RADARSTATION_MSG_
{
	char radarCd[32]; 		//�״���
	char area[32]; 			//������Ϣ
	char station[32]; 		//����վ��
	char type[32]; 			//�״�����
	long longitude; 		//�״ﾭ��
	long latitude;  		//�״�γ��
	long altitude;			//���θ߶�
	double wav_length; 		//��������
	double wav_width; 		//�������
	unsigned short int TimeFormat[6];	//ʱ�����ƫ�Ƶ�ַ������������ʱ���� ��0,4(��) 4,6(��) ��
	TransDataFormate dataformate;	//��������
	vector<tagFtpMsg> ftpMsgs;		//ftp�����Ϣ
}tagRadarStationMsg;

typedef struct _FILES_MSG_
{
	char station[255];
	char filePath[300];
}tagFileMsg;

#endif