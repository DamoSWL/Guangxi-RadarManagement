



#ifndef preparitht_h__20090401
#define preparitht_h__20090401


#include "../../interface/productgen/IPreArith.h"

#include <string>

/*! \class PreparationArithT
	\brief 预处理算法辅助模板类

	\ingroup RadialStreamHandleGroup
*/
template< class T >
class PreparationArithT:public IPreparationArith
{
	char m_szName[50];  //!< 算法名称
	int m_type;  /*! 算法类型
				 - 0 径向数据预处理算法
				 - 1 PPI预处理算法
				 - 2 体扫预处理算法
				 */

	int m_iIdx;
	bool m_bUsing; 
	std::string m_szCfgPath;
public:
	
	PreparationArithT()
	{
		memset( m_szName, 0, 50);
		m_iIdx = 0;
		m_type = ARITH_PREP_PPI;
		m_bUsing = false;
	}

	//! 得到算法的名称
	char *GetPrepName()
	{
		return m_szName;
	}

	//! 得到算法在调用列表中的序号
	int GetPrepIndex()
	{
		return m_iIdx;
	}

	//! 设置算法在调用列表中的序号
	void SetPrepIndex( int index )
	{
		m_iIdx  = index;
		if( !m_szCfgPath.empty() )
			SavePrepConfig( m_szCfgPath.c_str() );
	}

	//! 得到预处理算法处理的数据类型
	int GetPrepType()
	{
		return m_type;
	}

	void SetPrepType( int iPrepType )
	{
		m_type = iPrepType;
	}

	bool IsPrepUsing()
	{
		return m_bUsing;
	}

	//! 开启、关闭算法
	void SetPrepUse( bool bUse )
	{
		m_bUsing = bUse;
		if( !m_szCfgPath.empty() )
			SavePrepConfig( m_szCfgPath.c_str() );
	}

	//////////////////////////////////////////////////////////////////////////
	//! PPI数据接收完毕回调函数
	void OnPrepPPIReceived(IBufferPPI* pPPI, bool &bHandle)
	{

	}

	//! 体扫数据接收完毕回调函数
	void OnPrepVolReceived(const char* szVol, IBufferVol* pVol, bool &bHandle)
	{

	}

	void SetPrepParam()
	{

	}

	//////////////////////////////////////////////////////////////////////////
	void InitPrep( char *szName, int iType, int iIdx, bool bUsing  )
	{
		strcpy( m_szName, szName );
		m_type = iType;
		m_iIdx = iIdx;
		m_bUsing = bUsing;
	}

	//! 加载预处理算法配置文件
	bool LoadPrepConfig( const char *szCfgPath )
	{
		try
		{
			ticpp::Document doc;
			doc.LoadFile( szCfgPath );

			ticpp::Element *root = doc.FirstChildElement("Arith");
			if( root != NULL )
			{
				m_iIdx = atoi( root->GetAttribute("index").c_str());
				if( m_iIdx < 0 )
					m_iIdx = 0;

				m_bUsing = (atoi( root->GetAttribute("use").c_str() )>0)?true:false;
			}

			m_szCfgPath = szCfgPath;

			return true;
		}
		catch (ticpp::Exception& ex)
		{
			wxLogWarning(wxT("读取算法配置信息异常。[%s]\n"),ex.m_details.c_str());
			return false;
		}
	}

	//! 保存预处理算法配置文件
	bool SavePrepConfig( const char *szCfgPath )
	{
		try
		{
			ticpp::Document doc;
			doc.LoadFile( szCfgPath );

			ticpp::Element *root = doc.FirstChildElement("Arith");
			if( root != NULL )
			{
				root->SetAttribute( "index", m_iIdx );
				root->SetAttribute( "use", m_bUsing );
			}

			doc.SaveFile();
			return true;
		}
		catch (ticpp::Exception& ex)
		{
			wxLogWarning(wxT("保存算法配置信息异常。[%s]\n"),ex.m_details.c_str());
			return false;
		}
	}
};

#endif // preparitht_h__20090401

