/*! \file IPluginHostHelper.h
	\brief 文件存放了一些宿主插件（Host Plugin）辅助类
*/


#ifndef __HEADER_IPluginHostHelper_20080813
#define __HEADER_IPluginHostHelper_20080813

#include "IPluginHelper.h"
#include <map>
#include <string>
	using namespace std;

//! CDYW命名空间
namespace CDYW
{
//! 插件系统命名空间
namespace PluginSystem
{
//! 快捷键结构体
typedef struct _ShortCut
{
	PL_GUID PID; //!< 插件ID
	int id;      //!< 对应的命令ID
	string key;  //!< 快捷键
	string mean; //!< 意思
}tagShortCut;

//! 快捷键配置存储结构体
typedef struct _SCPath
{
	PL_GUID PID; //!< 插件ID
	bool bChange; //!< 快捷键是否改变
	string szPath; //!< 插件配置文件路径
}tagSCPath;

typedef map< string, tagShortCut > MAPShortCut;  //!< 快捷键列表
typedef map< string, tagShortCut >::iterator ItSC;

typedef vector< tagSCPath > VECShortCutPath;  //! 插件的配置文件路径
typedef vector< tagSCPath >::iterator ItSCP;

//////////////////////////////////////////////////////////////////////////

/*! \class IPluginHostHelper
	\brief 宿主插件辅助类
*/
template< class T>
class IPluginHostHelper:
	public IPluginHelper< T >
	,public IHostPlugin
{
public:
	map< string, tagShortCut > m_mapShortCut;  //!< 快捷键列表
	typedef map< string, tagShortCut >::iterator ItSC;

	vector<tagSCPath> m_vecSCPath;  //!< 插件的配置文件路径

	string m_szShortCutPath; //!< 快捷键配置存储文件

	/*! 读取快捷键配置文件
		\param PID 插件的ID
		\param cfgPath 快捷键配置文件
	*/
	bool LoadShortCut( PL_GUID PID, const char * cfgPath )
	{
		try
		{
			ticpp::Document doc;
			doc.LoadFile( cfgPath );

			ticpp::Element *root = doc.FirstChildElement();
			if( root != NULL )
			{
				ticpp::Element *eleSCs = root->FirstChildElement("ShortCuts");
				if( eleSCs ) {
					ticpp::Iterator<ticpp::Element> It( eleSCs->FirstChildElement("ShortCut"), "ShortCut");
					while( It != It.end() )
					{
						tagShortCut sc;
						sc.key = It->GetAttribute( "key" );
						sc.id = atoi( It->GetAttribute("id").c_str() );
						sc.mean = It->GetAttribute("mean");
						sc.PID = PID;
						
						m_mapShortCut[sc.key] = sc;
						////
						It++;
					}
				}
			}

			tagSCPath SCPath;
			SCPath.bChange = false;
			SCPath.PID = PID;
			SCPath.szPath = cfgPath;
			m_vecSCPath.push_back( SCPath );
			return true;
		}
		catch (ticpp::Exception& ex)
		{
			//wxLogWarning(wxT("读取快捷键信息异常，忽略掉信息。[%s]\n"), wxString(ex.m_details.c_str(), wxConvLocal ) );
			return false;
		}
	}

	/*! 保存快捷键配置文件
		\param PID 插件的ID
	*/
	void SaveShortCut( PL_GUID PID )
	{
		ItSCP its = m_vecSCPath.begin(), ite = m_vecSCPath.end();
		while( its != ite )
		{
			if( IsEqualID( its->PID, PID) )
			{
				// 获取地址
				string szCfg = its->szPath;
				
				///// 保存配置文件
				
				/////
				break;
			}
			///
			its++;
		}
	}

	void SaveAllShortCut()
	{
		ItSCP its = m_vecSCPath.begin(), ite = m_vecSCPath.end();
		while( its != ite )
		{
			if( its->bChange )
			{
				string szCfgPath = its->szPath;

				try
				{
					ticpp::Document doc;
					doc.LoadFile( szCfgPath );

					ticpp::Element *root = doc.FirstChildElement();
					if( root != NULL )
					{
						ticpp::Element *eleSCs = root->FirstChildElement("ShortCuts");
						if( eleSCs ) {
							///// 移除所有快捷键
							eleSCs->Clear();

							////// 保存快捷键
							ItSC it0 = m_mapShortCut.begin(), it1 = m_mapShortCut.end();

							while( it0 != it1 )
							{
								if( IsEqualID( it0->second.PID, its->PID ) )
								{
									//<ShortCut key="F1" id="30" mean="测试1" />
									ticpp::Element *elSC = new ticpp::Element("ShortCut");
									if( elSC ){
										elSC->SetAttribute( "key", it0->second.key );
										elSC->SetAttribute( "id", it0->second.id );
										elSC->SetAttribute( "mean", it0->second.mean );

										eleSCs->InsertEndChild( *elSC );
										delete elSC;
									}
								}

								it0++;
							}
						}

						doc.SaveFile();
						its->bChange = false;
					}
				}
				catch (ticpp::Exception& ex)
				{
					wxLogWarning(wxT("保存快捷键信息异常，忽略信息。[%s]\n"),ex.m_details.c_str());
					continue;
				}
			}
			///
			its++;
		}
	}
	
	/*! 卸载指定插件的快捷键信息
		\param PID 插件的 PID
	*/
	void UnLoadShortCut( PL_GUID PID )
	{
		map< string, tagShortCut >::iterator its = m_mapShortCut.begin(), ite = m_mapShortCut.end();
		while( its != ite )
		{
			if( IsEqualID( its->second.PID, PID) )
			{
				// 获取地址
				m_mapShortCut.erase( its );
				its = m_mapShortCut.begin();
				ite = m_mapShortCut.end();

				//////
				continue;
			}
			///
			its++;
		}
	}
};


//////////////////////////////////////////////////////////////////////////
};
};
#endif
