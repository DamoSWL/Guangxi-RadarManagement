#ifndef _ARITHFILESAVE_H_20150724
#define _ARITHFILESAVE_H_20150724
#pragma once

#include "stdio.h"
#include "time.h"
#include <string>
using namespace std;

typedef struct _ARITHFILESAVEPARAM_
{
	char* pBuff;
	size_t BufSize;
	int iLevel;
	int iParmCnt;//只有在多层才使用
	union
	{
		short int ParBuff[12];
		struct{
			float parame0;				//计算产品的参数0
			float parame1;				//计算产品的参数1
			float parame2;				//计算产品的参数2
			float parame3;				//计算产品的参数3
			float parame4;				//计算产品的参数4
			float parame5;				//计算产品的参数5
		};		
	};
}tagArithFileSaveParam;



class CArithFileSave
{
public:
#define  MAXSTRMCODELEN 50
	enum ArithFileSaveMode
	{
		SaveMode_Normal = 0,	//正常保存
		SaveMode_TMPFile		//临时文件,自动保存到另外一个临时文件夹中
	};
public:
	CArithFileSave(IHelper *pHelper,const string & VolFileName,int iPID,const tagArithFileSaveParam& ArithFileSaveParam , ArithFileSaveMode SaveMode = SaveMode_Normal)
	{
		//初始化
		InitData();
		//获取接口
		IZip *pZip = (IZip *)pHelper->QueryInterface( g_I_ZIP );
		IAirthSpecialProductAdmin *pProAdmin = (IAirthSpecialProductAdmin *)pHelper->QueryInterface( g_I_Airth_Special );
		if (pZip==NULL || pProAdmin ==NULL)
		{
			return;
		}
		bool bMultiSite = pProAdmin->AirthIsMultistation();//是否支持多站保存
		//初始化路径
		CProductName nameIn( VolFileName );
		char cFN[300] = {0};
		if (SaveMode == SaveMode_TMPFile)
		{
			if (bMultiSite)
			{
				sprintf(cFN, "%s\\%s\\",
					pHelper->GetDataPath(),
					TEMPDATADIRNAME
				);
				CreateDirectoryA(cFN, NULL);
				sprintf(cFN, "%s\\%s\\%s\\",
					pHelper->GetDataPath(),
					TEMPDATADIRNAME,
					nameIn.GetRadarNum()
				);
				CreateDirectoryA(cFN, NULL);
				sprintf(cFN, "%s\\%s\\%s\\%04d%02d%02d\\",
					pHelper->GetDataPath(),
					TEMPDATADIRNAME,
					nameIn.GetRadarNum(),
					nameIn.GetYear(), nameIn.GetMonth(), nameIn.GetDay());
			}
			else
			{
				sprintf(cFN, "%s\\%s\\",
					pHelper->GetDataPath(),
					TEMPDATADIRNAME
				);
				CreateDirectoryA(cFN, NULL);
				sprintf(cFN, "%s\\%s\\%04d%02d%02d\\",
					pHelper->GetDataPath(),
					TEMPDATADIRNAME,
					nameIn.GetYear(), nameIn.GetMonth(), nameIn.GetDay());
			}
		}
		else
		{
			if (bMultiSite)
			{
				sprintf(cFN, "%s\\%s\\",
					pHelper->GetDataPath(),
					nameIn.GetRadarNum()
				);
				CreateDirectoryA(cFN, NULL);
				sprintf(cFN, "%s\\%s\\%04d%02d%02d\\",
					pHelper->GetDataPath(),
					nameIn.GetRadarNum(),
					nameIn.GetYear(), nameIn.GetMonth(), nameIn.GetDay());
			}
			else
			{
				sprintf(cFN, "%s\\%04d%02d%02d\\",
					pHelper->GetDataPath(),
					nameIn.GetYear(), nameIn.GetMonth(), nameIn.GetDay());
			}
		}
		
		
		string strFN( cFN );
		string szTempName  = "", strName="", retName = "", szOutName="";
		CreateDirectoryA( strFN.c_str(), NULL );
		strName = strFN + pHelper->GetProductInfo( iPID )->EName;
		CreateDirectoryA( strName.c_str(), NULL );

		//解析体扫名称
		string cszIFile = VolFileName;
		size_t pos = cszIFile.rfind('\\');
		string szName( cszIFile.begin() + pos +1,  cszIFile.end());
		string szFileName;
		nameIn.SetIndex( iPID );
		szName = nameIn.GetName();
		szTempName = strName + "\\";
		szTempName += szName;

		//初始化Mcode
		//判断是否支持产品
		string strMcode(StrPidMcode[iPID]);
		if (!strMcode.empty())
		{
			char cAdd[100] = {0};
			if (iPID == PID_MCAPPI || iPID == PID_MPCAPPI || iPID == PID_MVCAPPI || iPID == PID_MWCAPPI)
			{
				//由于此版本之前没有保存窗口宽度，这里也不保存
				// 				sprintf( cAdd, "_%d", (int)ArithFileSaveParam.ParBuff[0]);
				string Tempstr;
				sprintf( cAdd, "_%d", (int)ArithFileSaveParam.ParBuff[2]);

				for (int i=3;i<ArithFileSaveParam.iParmCnt+2;i++)
				{
					char tmp[80] = {0};
					sprintf( tmp, "-%d", (int)ArithFileSaveParam.ParBuff[i] );
					Tempstr += tmp;
				}
				szTempName += (cAdd+Tempstr);
			}
			else if (iPID == PID_USP)
			{
				unsigned int iS = 0;
				unsigned int iT = 0;
				memcpy(&iS,&ArithFileSaveParam.parame0,sizeof(float));
				memcpy(&iT,&ArithFileSaveParam.parame1,sizeof(float));
				sprintf( cAdd, strMcode.c_str(),
					iS,
					iT,
					ArithFileSaveParam.parame2,
					ArithFileSaveParam.parame3,
					ArithFileSaveParam.parame4,
					ArithFileSaveParam.parame5
					);
				szTempName += cAdd;				
			}
			else
			{
				sprintf( cAdd, strMcode.c_str(),
					ArithFileSaveParam.parame0,
					ArithFileSaveParam.parame1,
					ArithFileSaveParam.parame2,
					ArithFileSaveParam.parame3,
					ArithFileSaveParam.parame4,
					ArithFileSaveParam.parame5
					);
				szTempName += cAdd;
			}	
		}

		szName = szTempName;
		szFileName = szName;
		m_szOutFile = szFileName;

		//增加降雨标识

		int RainType = nameIn.GetRainType();
		if (RainType != PrecipEst_NoSave)
		{
			char cRain[5] = {0};
			sprintf( cRain, "_R%d", RainType );
			m_szOutFile += cRain;
		}

		//调用压缩进行保存
		bool bError = false;
		if (ArithFileSaveParam.iLevel == 0)
		{
			//压缩
			switch(pZip->ZipType())
			{
			case ywZIP_FORMAT_BZ2:
				szName = szName + ".bz2";
				m_szOutFile = m_szOutFile + ".bz2";
				break;
			case ywZIP_FORMAT_ZLIB:
				szName = szName + ".zdb";
				m_szOutFile = m_szOutFile + ".zdb";
				break;
			case ywZIP_FORMAT_ZIP:
				szName = szName + ".zip";
				m_szOutFile = m_szOutFile + ".zip";
				break;
			}

			if(ArithFileSaveParam.pBuff!=NULL)
			{
				if( !pZip->CompressM2F(ArithFileSaveParam.pBuff, ArithFileSaveParam.BufSize, (char*)m_szOutFile.c_str()))
					bError = true;
			}
			else bError = true;
		}
		else if (ArithFileSaveParam.iLevel == 999)//文件压缩到文件ZIP
		{
			//RTDPZ需要重命名文件,先重命名文件名再加文件格式
			string strNewFile = szName;
			rename(ArithFileSaveParam.pBuff,szName.c_str());

			szName = szName + ".zip";
			m_szOutFile = m_szOutFile + ".zip";

			if( !pZip->CompressF2F((char*)strNewFile.c_str(), (char*)m_szOutFile.c_str()))
				bError = true;
			remove(strNewFile.c_str());
		}
		else
		{
			// 不压缩产品
			FILE *fp = fopen((char*)m_szOutFile.c_str(),"wb");
			if( fp ) 
			{
				fwrite( ArithFileSaveParam.pBuff, 1, ArithFileSaveParam.BufSize, fp);  // 写数据
				fclose(fp);
			}else{
				m_szOutFile = "";
				bError = true;
			}
		}
		if( !bError )
		{
			if (SaveMode != SaveMode_TMPFile)
			{
				/// 记录产品到数据库
				if( pProAdmin )
					pProAdmin->AirthAddProduct( m_szOutFile.c_str() );
			}

		}
	}

	string GetOutFile()
	{
		return m_szOutFile;
	}

protected:
	void InitData()
	{
		m_szOutFile = "";
		InitMcode();
	}


	void InitMcode()
	{
		memset(StrPidMcode,0,sizeof(char)*MAXSTRMCODELEN*200);

		strcpy_s(StrPidMcode[PID_ARD],MAXSTRMCODELEN,	"_%.0f-%.2f" );
		strcpy_s(StrPidMcode[PID_CAPPI],MAXSTRMCODELEN,	"_%.0f" );
		strcpy_s(StrPidMcode[PID_CS],MAXSTRMCODELEN,	"_%.0f-%.2f-%.2f" );
		memset(StrPidMcode[PID_DDPD],0,sizeof(char)*MAXSTRMCODELEN);
		memset(StrPidMcode[PID_ETC],0,sizeof(char)*MAXSTRMCODELEN);
		memset(StrPidMcode[PID_EBC],0,sizeof(char)*MAXSTRMCODELEN);
		memset(StrPidMcode[PID_CRC],0,sizeof(char)*MAXSTRMCODELEN);
		strcpy_s(StrPidMcode[PID_HI],MAXSTRMCODELEN,	"_%.2f-%.2f-%.2f" );
		strcpy_s(StrPidMcode[PID_HPF],MAXSTRMCODELEN,	"_%.0f-%.0f-%.0f" );
		strcpy_s(StrPidMcode[PID_IVAP],MAXSTRMCODELEN,	"_%.1f-%.0f-%.0f" );
		strcpy_s(StrPidMcode[PID_LRA],MAXSTRMCODELEN,	"_%.0f-%.0f-%.0f-%.0f" );
		strcpy_s(StrPidMcode[PID_LRM],MAXSTRMCODELEN,	"_%.0f-%.0f-%.0f-%.0f" );
		strcpy_s(StrPidMcode[PID_LTA],MAXSTRMCODELEN,	"_%.0f-%.0f-%.0f-%.0f" );
		strcpy_s(StrPidMcode[PID_LTM],MAXSTRMCODELEN,	"_%.0f-%.0f-%.0f-%.0f" );
		strcpy_s(StrPidMcode[PID_M],MAXSTRMCODELEN,  	"_%.0f-%.0f" );
		strcpy_s(StrPidMcode[PID_MAX],MAXSTRMCODELEN,  	"_%.2f-%.2f-%.0f" );
		//memset(StrPidMcode[PID_MAXREF],0,sizeof(char)*MAXSTRMCODELEN);
		//多层Mcode格式化在赋值部分还会进行格式限定，这只是保证其不为空
		strcpy_s(StrPidMcode[PID_MCAPPI],MAXSTRMCODELEN,	"_%.0f" );
		strcpy_s(StrPidMcode[PID_MVCAPPI],MAXSTRMCODELEN,	"_%.0f" );
		strcpy_s(StrPidMcode[PID_MWCAPPI],MAXSTRMCODELEN,	"_%.0f" );
		strcpy_s(StrPidMcode[PID_MPCAPPI],MAXSTRMCODELEN,	"_%.0f" );
		strcpy_s(StrPidMcode[PID_NHP],MAXSTRMCODELEN,  	"_%.0f-%.2f-%.2f" );
		strcpy_s(StrPidMcode[PID_OHP],MAXSTRMCODELEN,  	"_%.0f-%.2f-%.2f" );
		strcpy_s(StrPidMcode[PID_RCS],MAXSTRMCODELEN,  	"_%.0f-%.2f-%.0f-%.2f-%.2f" );
		strcpy_s(StrPidMcode[PID_VCS],MAXSTRMCODELEN,  	"_%.0f-%.2f-%.0f-%.2f-%.2f" );
		strcpy_s(StrPidMcode[PID_SCS],MAXSTRMCODELEN,  	"_%.0f-%.2f-%.0f-%.2f-%.2f" );
		strcpy_s(StrPidMcode[PID_RVD],MAXSTRMCODELEN,  	"_%.0f-%.2f" );
		strcpy_s(StrPidMcode[PID_RZ],MAXSTRMCODELEN,  	"_%.0f-%.2f-%.2f" );
		strcpy_s(StrPidMcode[PID_SCTI],MAXSTRMCODELEN,  	"_%.2f" );
		strcpy_s(StrPidMcode[PID_SEI],MAXSTRMCODELEN,  	"_%.2f_%.2f_%.2f" );
		strcpy_s(StrPidMcode[PID_SS],MAXSTRMCODELEN,  	"_%.2f" );
		strcpy_s(StrPidMcode[PID_STP],MAXSTRMCODELEN,  	"_%.0f-%.2f-%.2f" );
		strcpy_s(StrPidMcode[PID_CATCH],MAXSTRMCODELEN,  	"_%.0f-%.2f-%.2f" );
		strcpy_s(StrPidMcode[PID_SWP],MAXSTRMCODELEN,	"_%.0f-%.0f-%.0f" );
		memset(StrPidMcode[PID_MAXREF],0,sizeof(char)*MAXSTRMCODELEN);
		memset(StrPidMcode[PID_ET],0,sizeof(char)*MAXSTRMCODELEN);
		memset(StrPidMcode[PID_EB],0,sizeof(char)*MAXSTRMCODELEN);
		strcpy_s(StrPidMcode[PID_THP],MAXSTRMCODELEN,  	"_%.0f-%.2f-%.2f" );
		strcpy_s(StrPidMcode[PID_TVS],MAXSTRMCODELEN,  	"_%.0f" );
		strcpy_s(StrPidMcode[PID_USP],MAXSTRMCODELEN,  	"_%u-%u-%.0f-%.2f-%.2f" );
		strcpy_s(StrPidMcode[PID_VAD],MAXSTRMCODELEN,  	"_%.2f-%.2f" );
		strcpy_s(StrPidMcode[PID_VIL],MAXSTRMCODELEN,  	"_%.0f-%.0f" );
		strcpy_s(StrPidMcode[PID_VWP],MAXSTRMCODELEN,  	"_%.2f-%.2f-%.2f" );
		strcpy_s(StrPidMcode[PID_VSP],MAXSTRMCODELEN,  	"_%.0f-%.2f-%.2f" );

		strcpy_s(StrPidMcode[PID_NWRTD],MAXSTRMCODELEN,  "_%.3f-%.0f" );
		strcpy_s(StrPidMcode[PID_VOLRTD], MAXSTRMCODELEN, "_%.3f-%.0f");
		strcpy_s(StrPidMcode[PID_CAPPIPZ],MAXSTRMCODELEN,  	"_%.3f-%.0f" );
		strcpy_s(StrPidMcode[PID_CRPZ],MAXSTRMCODELEN,  	"_%.3f" );
		strcpy_s(StrPidMcode[PID_RTD],MAXSTRMCODELEN,  	"_%.0f-%.0f" );
		strcpy_s(StrPidMcode[PID_RTDPZ],MAXSTRMCODELEN,  "_%.3f-%.0f" );
		strcpy_s(StrPidMcode[PID_VILPZ],MAXSTRMCODELEN,  "_%.2f-%.0f" );
		strcpy_s(StrPidMcode[PID_RCSPZ],MAXSTRMCODELEN,  "_%.0f-%.0f-%.0f" );
		strcpy_s(StrPidMcode[PID_SSPZ],MAXSTRMCODELEN,  "_%.2f" );
		strcpy_s(StrPidMcode[PID_STIPZ],MAXSTRMCODELEN,  "_%.2f" );

		strcpy_s(StrPidMcode[PID_RZPZ],MAXSTRMCODELEN,  "_%.2f-%.2f-%.2f" );
		strcpy_s(StrPidMcode[PID_OHPPZ],MAXSTRMCODELEN,  "_%.2f-%.2f-%.2f" );
		strcpy_s(StrPidMcode[PID_THPPZ],MAXSTRMCODELEN,  "_%.2f-%.2f-%.2f" );
		strcpy_s(StrPidMcode[PID_STPPZ],MAXSTRMCODELEN,  "_%.2f-%.2f-%.2f" );

		strcpy_s(StrPidMcode[PID_HPFPZ], MAXSTRMCODELEN, "_%.2f-%.0f-%.0f");

		memset(StrPidMcode[PID_ETPZ],0,sizeof(char)*MAXSTRMCODELEN);
		memset(StrPidMcode[PID_EBPZ],0,sizeof(char)*MAXSTRMCODELEN);

		strcpy_s(StrPidMcode[PID_WMPE], MAXSTRMCODELEN, "_%.0f-%.0f-%.0f-%.0f-%.0f-%.0f");

		strcpy_s(StrPidMcode[PID_NWCAPPI],MAXSTRMCODELEN,  	"_%.3f-%.0f-%.0f" );
		strcpy_s(StrPidMcode[PID_NWCR],MAXSTRMCODELEN,  	"_%.3f-%.0f" );
		strcpy_s(StrPidMcode[PID_NWET],MAXSTRMCODELEN,  	"_%.3f" );
		strcpy_s(StrPidMcode[PID_NWEB],MAXSTRMCODELEN,  	"_%.3f" );

		strcpy_s(StrPidMcode[PID_WF],MAXSTRMCODELEN,  	"_%.2f" );
		strcpy_s(StrPidMcode[PID_SDS],MAXSTRMCODELEN,  	"_%.2f" );

		strcpy_s(StrPidMcode[PID_PE], MAXSTRMCODELEN, "_%.0f-%.0f-%.0f-%.0f-%.0f");
		strcpy_s(StrPidMcode[PID_HS], MAXSTRMCODELEN, "_%.0f-%.0f-%.0f-%.0f");

	}
	//////////////////////////////////////////////////////////////////////////
	string m_szOutFile;
	char StrPidMcode[200][MAXSTRMCODELEN];
};

#endif