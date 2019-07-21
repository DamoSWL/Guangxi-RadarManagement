/*! \file STI_Pro.h
	本文件定义了 STI 产品解析的相关结构体
*/  

#ifndef __STI_FILE_070915 
#define __STI_FILE_070915 

#include "ss_pro.h"
#include <vector>
#include <string>
	using namespace std;

#pragma pack(4)

//! 风暴位置结构体
typedef struct _StormPosition
{
	float fAScur;    //!< 风暴现在的中心方位，度
	float fRScur;    //!< 风暴现在的中心距离，km
}tagStormPos;

//! 风暴单元结构体
typedef struct _StormCellTrak
{
	unsigned int iIndex; //!<  所属风暴体的序号
	unsigned int iVol;   //!<  所属体扫的序号
	bool bNewStorm;	     //!<  是否新的风暴体

	STORMCELL stormCell; //!<  风暴体信息

	float SS;		 //!< 风暴体的移速,m/s
	float DS;		 //!< 风暴体的移向,度

	float ASPre;    //!< 前一次体扫时风暴体的中心方位，度
	float RSPre;    //!< 前一次体扫时风暴体的中心距离，km

	float XSC;     //!< 
	float YSC;

	int IndPre;          //!<  与前一个体扫匹配的风暴体在前一个体扫风暴中的序号
	tagStormPos PosPre;	 //!<  风暴体位置信息

	int IndCur;	        //!<  当前体扫时匹配的风暴体在当前体扫风暴中的序号
	tagStormPos pos;	//!<  风暴体位置信息

	int cntFort;	    //!<  预测的数据个数
	tm time;            //!< 观测时间
}tagStormCellTrak;

//! 风暴追踪路径预测结构体
typedef struct _StormCellTrakAFort
{
	tagStormCellTrak trak;
	vector<tagStormPos> relStorm;	//!<  相关风暴体信息
	vector<tagStormPos> forts;	    //!<  风暴体移动预测路径
}tagStormCellTrakAFort;

//! 风暴追踪头
typedef struct _StromTrakHead
{
	tm time;	  //!<  时间
	int nStormTrak;	//!<  包含的Storm的个数
}tagStormTrakHeader;

//! 体扫风暴体结构体
typedef struct _VolStormTrak
{	
	tm time;
	int nStormTrak;	 //!< 包含的Storm的个数
	vector<tagStormCellTrakAFort> traks;
}tagVolStormTrak;

//! STI 产品头
typedef struct _STIHeader
{
	float Area; //!<  输入参数，风暴面积
	int nStrom; //!<  风暴体个数
	tm tStart;	//!<  风暴追踪起始时间
	tm tStop;	//!<  风暴追踪终止时间
	int nVol;	//!<  风暴追踪使用的体扫文件个数
}tagStiHeader;

typedef struct
{
#define ALLSTI_DATASIZE 100
	unsigned int Ver;
	unsigned int DataCount;
	unsigned int DataSize[ALLSTI_DATASIZE];
//	char* pData;
}tagALLSTI;

typedef struct _StormCellTrakAFort1
{
	tagStormCellTrak trak;
	int iVol;	 //!< 所属体扫的序号
	vector<tagStormPos> relStorm;	//!< 相关风暴体信息
	vector<tagStormPos> forts;	//!< 风暴体移动预测路径
}tagStormCellTrakAFort1;/*! 用于按照条绘制*/

//! 风暴体结构体
typedef struct _StormTrak
{	
	char idx[5];  //!< 风暴体的序号，这是一个全局的序号，由'A' 开始，到'Z' 后，又从头循环
	tm timeStart;  //!< 风暴起始时间
	tm timeEnd;	 //!< 风暴停止时间
	int nStormTrak;	//!< 包含的Storm的个数
	bool bVisible;  //!< 是否显示此风暴体
	vector<tagStormCellTrakAFort1> traks;  //!< 风暴追踪路径
}tagStormTrak;

//!< STI 参数
typedef struct _STIPara
{
	float CS;		//!< 相关风暴最大移速阀值，m/s	
	float MaxTS;      //!< 相邻两个体扫之间所允许的最大时间间隔，s
	float SPDdef;     //!< 默认的风暴体的移速，m/s
	float DIRdef;    //!< 默认的风暴体的移向，度

}tagSTIPara;

typedef vector<tagStormTrak> STI_Storm;

/*! 风暴体序列化 结构体
	本结构体用于风暴体的序列化
*/
typedef struct _StormSerial
{
	tagRealFile *pHead;
	tagStormTrakHeader *pTrakHead;
	STI_Storm *vStorm;
}tagStormSerial;

#pragma pack()


//! STI 产品解析类
/*!
* \author lava_sdb
* \ingroup group_UT
*/
class STIPro:
		public RVWPro
{
public:
	STIPro(void);
	~STIPro(void);

	tagStiHeader m_StiHeader; //!< STI 头
	unsigned char *m_pDataT;

	char g_cIdx1;
	unsigned int g_cIdx0;

	vector<tagStormTrak>  m_vecStorms;


	int Open( string szPath );	
	int OpenBuff( char *pBuff,int iLen );

	inline float Deg2Float(short data)
	{
		char *pData = (char *)&data;
		return (float)(pData[0] *0.02197265625 + pData[1] * 2.8125);
	}


	//! 获取风暴文件包含体扫个数
	unsigned int GetVolCount( );
	

	//! 获取暴风体预测信息


	/*! 获取暴风体信息
		\param iVolIndex  体扫序号
	*/
	tagStormTrakHeader *GetVolTarkInfo( int iVolIndex );
	/*! 获取风暴体追踪信息
		\param iVolIndex 体扫序号
		\param iTrak 追踪信息
	*/

	tagStormCellTrak *GetTrakInfo( int iVolIndex, int iTrak );
	
	/*! 获取暴风体预测信息
	*/
	tagStormPos *GetFortInfo(int iVolIndex, int iTrak, int &cnt );
	/*! 获取风暴追踪个数
	*/
	unsigned int GetStormCount();

	/*! 获取风暴追踪
	*/
	tagStormTrak *GetStormTrak( unsigned int stormIndex);
	

	void PhraseFile();


	/*! 解析传入的体扫STI数据
	*/
	bool PhraseVol( unsigned char *pData, unsigned int iLen);

	unsigned int m_cntVol; //!< 体扫文件个数
	tm m_timeStart; //!< 数据记录起始时间
	tm m_timeEnd;   //!< 数据记录终止时间

	int m_iVol;     //!< 最新数据的体扫序号
	unsigned int m_iPosVol;		//!< 当前体扫数据序号

	/*! 序列化当前数据
		\param pData 存放指针
		\return >0 成功，返回数据长度
		        <=0 失败
	*/
	int SerialData( unsigned char **pData );

	//int PhraseSerialData( unsigned char *pData );
	int PhraseSerialData( char *pData,int iLen);

private:
	bool m_bInitGetData; //!< 是否是第一次得到数据
	tagStormSerial *m_pSerialData;  //!< 序列化数据的存储区指针
};


#endif
