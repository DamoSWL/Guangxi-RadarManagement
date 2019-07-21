/*! \file productgenT.h
	本文件定义了算法调用的相关模板和接口
	\see ArithCalcT
	\see ArithCalcStream
	\see IProductArith
	\see IProductStreamArith
*/

#ifndef productgenT_h__20081224
#define productgenT_h__20081224

#include "WRIS/GlobalDef.h"



/*! \addtogroup ProductGenInterface */
/*! @{ */

//! 单次体扫算法辅助模板函数
template< typename T >
void ArithCalcT( tagProGen *pPar, IHelper *pHelper )
{
	T arith;   // 你只需修改这里的类名称， 类必须含有 public 的 calcc(...) 函数
	if( pPar )
	{
		if(pPar->iInProIndex!=pPar->iDependentIndex)
		{
			memset( pPar->cOutFilePath, 0,sizeof(pPar->cOutFilePath) );
			pHelper->AddLogF( LOG_TYPE_WARNING, "算法输入数据[%d]非依赖数据[%d]",pPar->iInProIndex,pPar->iDependentIndex);
			return;//
		}
		bool bCreateBuffer = false; // 是否是压缩文件
		bool bLoad = false; // 是否读取文件成功
		char *pDataBuf = NULL;  // 数据缓冲
		unsigned int ulBufLen = MAX_VOL_SIZE;  // 数据缓冲大小

		if(pPar->pInBuffer!=NULL)//非压缩产品
		{
			pDataBuf = pPar->pInBuffer;
			ulBufLen = pPar->uInBufLen;
			bLoad = true;
		}
		else //压缩产品
		{
			// 使用输入文件计算
			///// 计算函数，检测输入的文件是否是压缩文件，是，就解压缩 
			string szIn( pPar->cInFilePath );
			IZip *pZip = (IZip *)pHelper->QueryInterface( g_I_ZIP );
			tagYWZipHead zipHead;
			int zFormat=pZip->GetZipFormatByFile(  pPar->cInFilePath, &zipHead);
			if(  zFormat>= 0)
			{
				if(zFormat==ywZIP_FORMAT_BZ2)
					zipHead.rSize=MAX_VOL_SIZE;//默认最大算法数据量
			}
			else
			{
				pHelper->AddLogF( LOG_TYPE_WARNING, "非正常压缩文件！%s",pPar->cInFilePath);
				return;
			}
			ulBufLen=zipHead.rSize;
			pDataBuf = new char[ ulBufLen ];
			if(pDataBuf==NULL)
			{
				pHelper->AddLogF( LOG_TYPE_WARNING, "内存不足",pPar->cInFilePath);
				return;
			}
			if( pDataBuf ) 
			{
				bCreateBuffer = true;

				// 检测是否压缩文件
				CProductName name(szIn);
				if( name.IsZip())
				{
					
					if( pZip->DeCompressF2M( pPar->cInFilePath, pDataBuf, ulBufLen ) )
					{
						bLoad = true;
					}
				}
				else
				{
					FILE *fp = fopen( pPar->cInFilePath, "rb" );
					if( fp ) 
					{
						fseek( fp, 0, SEEK_END );
						long lSize = ftell( fp );
						fseek( fp, 0, SEEK_SET );
						fread( pDataBuf, lSize, 1, fp);
						fclose( fp );
						ulBufLen = lSize;
						bLoad = true;
					}
				}
			}
		}
	
		// 确认文件是否加载正确
		if( !bLoad )
		{
			strcpy( pPar->cOutFilePath, "" );
			pHelper->AddLog( LOG_TYPE_MESSAGE, "解压缩文件出现错误\n" );
			if(pDataBuf!=NULL) {delete []pDataBuf;pDataBuf=NULL;}
			return ;
		}

		/////// 调用算法
		string szOutPath = "";
		szOutPath = arith.Calcc( pPar->cInFilePath, pDataBuf, ulBufLen, pPar->iOutProIndex, pHelper, pPar->iLevel );
		strcpy( pPar->cOutFilePath, szOutPath.c_str() );

		/////// 释放内存
		if( pDataBuf && bCreateBuffer ){
			delete []pDataBuf;
		}
	}
}

//! 体扫流算法调用辅助模板函数
template< typename callerT >
void ArithCalcStream( void *pT, tagProGen *pPar, IHelper *pHelper )
{
	if( pPar && pT )
	{
		if(pPar->iInProIndex!=pPar->iDependentIndex)
		{
			memset( pPar->cOutFilePath, 0,sizeof(pPar->cOutFilePath) );
			pHelper->AddLogF( LOG_TYPE_WARNING, "算法输入数据[%d]非依赖数据[%d]",pPar->iInProIndex,pPar->iDependentIndex);
			return;//
		}

		callerT caller;

		bool bCreateBuffer = false; // 是否是压缩文件
		bool bLoad = false; // 是否读取文件成功
		char *pDataBuf = NULL;  // 数据缓冲
		unsigned int ulBufLen = MAX_VOL_SIZE;  // 数据缓冲大小

		if(pPar->pInBuffer!=NULL)//非压缩产品
		{
			pDataBuf = pPar->pInBuffer;
			ulBufLen = pPar->uInBufLen;
			bLoad = true;
		}
		else //压缩产品
		{
			// 使用输入文件计算
			///// 计算函数，检测输入的文件是否是压缩文件，是，就解压缩 
			string szIn( pPar->cInFilePath );
			IZip *pZip = (IZip *)pHelper->QueryInterface( g_I_ZIP );
			tagYWZipHead zipHead;
			int zFormat=pZip->GetZipFormatByFile(  pPar->cInFilePath, &zipHead);
			if(  zFormat>= 0)
			{
				if(zFormat==ywZIP_FORMAT_BZ2)
					zipHead.rSize=MAX_VOL_SIZE;//默认最大算法数据量
			}
			else
			{
				pHelper->AddLogF( LOG_TYPE_WARNING, "非正常压缩文件！%s",pPar->cInFilePath);
				return;
			}
			ulBufLen=zipHead.rSize;
			pDataBuf = new char[ ulBufLen ];
			if(pDataBuf==NULL)
			{
				pHelper->AddLogF( LOG_TYPE_WARNING, "内存不足",pPar->cInFilePath);
				return;
			}
			if( pDataBuf ) 
			{
				bCreateBuffer = true;

				// 检测是否压缩文件
				CProductName name(szIn);
				if( name.IsZip())
				{

					if( pZip->DeCompressF2M( pPar->cInFilePath, pDataBuf, ulBufLen ) )
					{
						bLoad = true;
					}
				}
				else
				{
					FILE *fp = fopen( pPar->cInFilePath, "rb" );
					if( fp ) 
					{
						fseek( fp, 0, SEEK_END );
						long lSize = ftell( fp );
						fseek( fp, 0, SEEK_SET );
						fread( pDataBuf, lSize, 1, fp);
						fclose( fp );
						ulBufLen = lSize;
						bLoad = true;
					}
				}
			}
		}

		// 确认文件是否加载正确
		if( !pDataBuf || !bLoad )
		{
			strcpy( pPar->cOutFilePath, "" );
			pHelper->AddLog( LOG_TYPE_MESSAGE, "解压缩文件出现错误\n" );
			return ;
		}

		/////// 调用算法
		string szOutPath = "";
		szOutPath = caller.CalcStream( pT, pPar->cInFilePath, pDataBuf, ulBufLen, pHelper, pPar );
		strcpy( pPar->cOutFilePath, szOutPath.c_str() );

		/////// 释放内存
		if( pDataBuf && bCreateBuffer )
		{
			delete []pDataBuf;pDataBuf=NULL;
		}
	}
}

//@{

/*! \class IProductArith
	\brief 单体扫产品算法接口类
*/
class IProductArith
{
public:

	//! 算法调用函数
	/*! 
		\param szIFile [ IN ]，传入的文件名
		\param pDataBuf [ IN ]，传入的算法源数据缓存指针
		\param uLen [ IN ]，输入缓存的大小
		\param proType [ IN ]，生成的产品类型，此参数对支持多个产品算法的算法模块又用，算法根据此参数输出相应的产品
		\param pHelper [ IN ]，辅助功能接口
		\param iLevel [ IN ]， 算法递归调用层次

		\return 返回产品存放路径。如果生成失败，返回""
	*/
	virtual string Calcc( string szIFile, char *pDataBuf, unsigned long uLen, int proType, IHelper *pHelper, int iLevel ) = 0;
};

/*! \class IProductStreamArith
	\brief 体扫流产品算法接口
*/
class IProductStreamArith
{
public:

	//! 算法调用函数
	/*! 
		\param pT [ IN ]，算法实例，必须实现IProductStreamArith接口
		\param szIFile [ IN ]，传入的文件名
		\param pDataBuf [ IN ]，传入的算法源数据缓存指针
		\param uLen [ IN ]，输入缓存的大小
		\param pHelper [ IN ]，辅助功能接口
		\param pPar [ IN ]， 算法输入信息
		\return 返回产品存放路径。如果生成失败，返回""
	*/
	virtual string CalcStream( void *pT,string szIFile, char *pDataBuf, unsigned long uLen, IHelper *pHelper, tagProGen *pPar ) = 0;
};

//@}

/*! @} */

#endif // productgenT_h__

