
#ifndef RADARFORMAT_SA_SB_CB_HEADER
#define RADARFORMAT_SA_SB_CB_HEADER

//#include "RadialHead.h"
// #include <map>

#define SA_SB_FORMAT
//#define CB_FORMAT
#ifndef CB_FORMAT
     //***  SA/SB      ***
	#define iReflectivityGateSize 1000	//	----	�����ʿⳤ��
	#define iDopplerGateSize	250	//	----	�����տⳤ��
	#define iReflectivityGates	460	//	----	�����ʿ�����
	#define iDopplerGates	920	//	----	�����տ�����
#else
	// ***		�����CBת����������涨���������涨��	***		<---
	#define iReflectivityGateSize 500	//	----	�����ʿⳤ��
	#define iDopplerGateSize	125	//	----	�����տⳤ��
	#define iReflectivityGates	800	//	----	�����ʿ�����
	#define iDopplerGates	1600 	//	----	�����տ�����
#endif

#define MaxCuts  20   //������ǲ���
#define MaxRads  360   //ÿ�������ϵķ�λ��,ÿ�ȱ���һ������
#define CODE_INVALID 0   //����ֵ�е�������,��ʾ����Ч�۲�����
#define CODE_RANFOLD 1   //����ֵ�е�������,��ʾ�о���ģ��

#define VALUE_INVALID -999.  //ʵ��ֵ�е�������,��ʾ����Ч�۲�����
#define VALUE_RANFOLD  999.  //ʵ��ֵ�е�������,��ʾ�о���ģ��

#define RES_POINT_FIVE 2  //�ٶȾ�������,����ľ���Ϊ0.5 M/S
#define RES_ONE_POINT  4  //�ٶȾ�������,����ľ���Ϊ1.0 M/S

#define VOL_BEG 3    //��ɨ��ʼ״̬��־
#define VOL_END 4    //��ɨ����״̬��־
#define ELV_BEG 0    //���ǿ�ʼ״̬��־
#define ELV_END 2    //���ǽ���״̬��־

#define RADIAN  (M_PI/180.)


/*
˵����

1�����ݵĴ洢��ʽ

ÿ����ɨ�洢Ϊһ���������ļ�

2�����ݵ����з�ʽ

���վ������ݵķ�ʽ˳�����У�����CINRAD SA/SB�״��ɨ���������Ե����ǿ�ʼ�������ǽ�����

3���������ݵĳ���

�������ݵĳ��ȹ̶���Ϊ2432�ֽڡ�

4������ⳤ�Ϳ���

�����ʾ���ⳤΪ1000�ף����������Ϊ460��

�ٶȺ��׿����ⳤΪ250�ף����������Ϊ920��
*/
namespace CDYW {
	namespace RadarDataFormat
	{
		namespace SA_SB_CB
		{
		/////////////////////////////////////////////////////////////////////////
			/************************************************************************/
			/*			�����ࣺ�ֱ���FL���ݵ�һ������һ��CUT��һ����ɨ         */
			/************************************************************************/
		#pragma pack ( push, 1)

// 			typedef struct  _RADIAL_BASEDATA
// 			{
// 				BASE_DATA_HEAD RadialHeader;	//	----	��������ͷ��
// 				unsigned char b_RData[iReflectivityGates];	//	----	��ɨ�������������飻
// 				unsigned char b_VData[iDopplerGates];	//	----	��ɨ�ٶ��������飻
// 				unsigned char b_WData[iDopplerGates];	//	----	��ɨ�׿��������飻
// 			}tagOneRadial;//	----	FLһ���������ݸ�ʽ
// 			//////////////////////////////////////////////////////////////////////////
// 
// 			typedef struct  _CUT_BASEDATA
// 			{
// 				tagOneRadial RadialinoneCUT[MaxRads];	//	----	CUT���ݸ�ʽ��
// 			}tagOneCUT;//	----	FLһ��CUT���ݸ�ʽ
// 			////////////////////////////////////////////////////////////////////////// 
// 
// 			typedef struct  _VCP_BASEDATA
// 			{
// 				tagOneCUT CUTinoneVCP[MaxCuts];	//	----	VCP���ݸ�ʽ��
// 			}tagOneVCP;//	----	FLһ����ɨ���ݸ�ʽ

			typedef struct _SAHeader
			{
				char Unused1[14];//1-14 ����
				//unsigned short int MessageType;//15~16 ��¼�������ͣ�����˵��������
				unsigned char MessageType;//15~16 ��¼�������ͣ�����˵��������
				unsigned char Channel;
				char Unused2[12];//17-28 ����

				unsigned int RadialCollectionTime;//29-32 �������ϲɼ���GMTʱ��(����)
				unsigned short RadialCollectionDate;//33-34 �������ϲɼ�������,��1/1/1970������Julian���ڱ�ʾ
				unsigned short int UnambiguousRange;//35-36 ��ģ������,��λ:0.1Km

				unsigned short int AzimuthAngle;//37-38 ��λ��,ʵ��ֵ=(value>>3)*0.043945 (��)
				unsigned short int DataNumber;//39-40 �����������
				unsigned short int DataStatus;//41-42 ��������״̬

				unsigned short int ElevationAngle;//43-44 ����,ʵ��ֵ=(value>>3)*0.043945 (��)
				unsigned short int ElevationNumber;// 45-46 ��ɨ�ڵ����Ǳ��

				//////////////////////////////////////////////////////////////////////////

				short int FirstGateRangeOfRef;//47-48 ��һ��ǿ�ȿ�ľ���(��)
				short int FirstGateRangeOfDoppler;//49-50 ��һ���ٶ�/�׿��ľ���(��)
				unsigned short int ReflectivityGateSize;//51-52 ǿ�ȿⳤ(��)
				unsigned short int DopplerGateSize;//53-54 �ٶ�/�׿�ⳤ(��)
				unsigned short int ReflectivityGates;//55-56 ǿ�ȿ���
				unsigned short int DopplerGates;//57-58 �ٶ�/�׿����

				//////////////////////////////////////////////////////////////////////////

				unsigned short CutSectorNumber;	//	----	CUT�ţ�
				unsigned int CalibrationConst ;	//59-64 ����	�궨������
				unsigned short int RefPointer;//65-66 ���״�����ͷ��ǿ�����ݿ�ʼ���ֽ���
				unsigned short int VelPointer;//67-68 ���״�����ͷ���ٶ����ݿ�ʼ���ֽ���
				unsigned short int SWPointer;//69-70 ���״�����ͷ���׿����ݿ�ʼ���ֽ���
				short int VelResolution;//71-72 �ٶȷֱ���:2=0.5m/s;4=1.0m/s

				unsigned short int VCP;//73-74 ��ɨVCPģʽ (11,21,31,32)
				//11����ˮģʽ��16������
				//21����ˮģʽ��14������
				//31�����ģʽ��8������
				//32�����ģʽ��7������
				char Unused4[8];//75-88 ����
				unsigned short PtrtoRef;
				unsigned short PtrtoVel;
				unsigned short PtrtoWth;
				unsigned short int NyquistVelocity;//89-90 Nyquist����(0.01m/s)��ģ���ٶ�
				unsigned short temp46;	//	----	������
				unsigned short temp47;
				unsigned short temp48;
				unsigned short CircleTotal;	//	----	��������
				unsigned char Unused5[30];//91-128 ����
				//unsigned char Data[ iReflectivityGates+ iDopplerGates + iDopplerGates];//129-2428�ֽ����ݣ���129-4128�ֽڻ����ݣ�
				unsigned char *Data;//129-2428�ֽ����ݣ���129-4128�ֽڻ����ݣ�
				char Unused6[4];//2429-2432 ���� ���� 4129-4132 ������

			}tagSAHeader,tagSBHeader;

		#pragma pack( pop )
			/************************************************************************/
			/*	  ������   �ֱ�����ȡ��SA_SB/CB����ʱ�䡢�ٶȡ����ǡ���λ�ǣ�		*/
			/*							�Լ��õ�FL��Ȩ��λ��				     	*/
			/************************************************************************/

		};//end namespace SA_SB
	};// end namespace RadarDataFormat
}; // end namespace CDYW


#endif