
#include "../../../include/WRIS/product/rvw_pro.h"

#include <string>
	using namespace std;

#ifndef rfc_pro_h__
#define rfc_pro_h__


struct RFCTBH_H
{
	unsigned short RangeBinNum;  //!< 距离库数 【2】
	unsigned short RangeBinLen;  //!< 距离库长，单位为m【2】
	unsigned short RecodeNumber; //!< 数据径向个数【2】
	float fHDis;  //!< 横向单位水平距离间隔，单位km【4】
	float fVDis;  //!< 纵向单位水平距离间隔，单位m【4】
	int  HNum;  //!< 横向修正值个数【4】
	int  VNum;  //!< 纵向修正值个数【4】
};

struct RFCTBH_RAD_AZ
{
	unsigned short int sAz;  //!< 各个扇区的其实方位,单位1/100度
	unsigned short int Az;   //!< 各个扇区的跨度,单位1/100度
};

struct RFCTB
{
	unsigned short int *pET;  //!< 顶高数据，单位m，数据量=距离库数x径向个数
	unsigned short int *pEB;  //!< 底高数据，单位m，同上
	unsigned short int *pThick; //!< 厚度数据，单位m，数据量=径向个数
	unsigned short int *pM;   //!< 强度数据，单位0.01M，数据量=径向个数
	unsigned short int *pRef; //!< 修正折射率值(短整型表示，单位为0.01M)
	short int *pPFac; //!< 传播因子值(用短整型表示，单位为0.01dB；)
};

/*! \class RFCPro
	RFC 产品解析类

*/
class RFCPro
{
public:
	RFCPro();
	virtual ~RFCPro();

	string m_szPath;	//!< 文件路径

	tagRealFile m_Header;	//!< 文件头
	char *m_pData;			//!< 数据

	unsigned int m_uiDataLen; //!< 数据区的长度

	RFCTB m_RFC;          //!< 数据	
	RFCTBH_H m_rfcH;		//!< RFC 头
	RFCTBH_RAD_AZ *m_pAzi;  //!< 方位




	int Open(string szPath);	
	int OpenBuff( char *pBuff,int iLen);
	void Close();

	bool HasData();

	//! 获取产品文件名字
	const char *GetDataName() { return m_szPath.c_str(); };

};
#endif 