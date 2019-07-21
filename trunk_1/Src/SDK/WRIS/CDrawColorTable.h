
#ifndef _HEADER_COLORTABLE_20081231
#define _HEADER_COLORTABLE_20081231

#include "../../include/WRIS/colorfile.h"
#include <wris/Product/ProductID.h>
#define MAXCLRNUM	256
namespace CDYW { 
	namespace RadarSystem {
		template<class T>
		class CColorTableT
		{
			pthread_mutex_t _muLoadClr;

			string _szClrFilePath;	//!< 色标文件路径
#define STRHCLOUT {"非气象","小雨","中雨","大雨","干雪","湿雪","冰晶","小冰雹","大冰雹","雨夹雹"};
#define STRHCLOUTCOUNT 10
		public:
			tagColorFile *_pColor;	//!< 颜色列表清单

			COLORREF _rgb[MAXCLRNUM];		//!< 当前域的颜色信息
			int _iActiveField;		//!< 当前活动的颜色色域序号   

			CColorTableT()
			{
				_pColor = NULL;
				_iActiveField = 0;

				memset(_rgb,0,sizeof(COLORREF)*MAXCLRNUM);

				pthread_mutex_init( &_muLoadClr, NULL );
			}

			~CColorTableT()
			{
				if( _pColor != NULL ) {
					if( _pColor->pField != NULL )
						delete []_pColor->pField;

					if( _pColor->pClr != NULL )
						delete []_pColor->pClr;
					
					delete _pColor;
					_pColor = NULL;
				}

				pthread_mutex_destroy( &_muLoadClr );
			}

		//      
		/* function */
		public:
			bool LoadColor(const char *szPath)
			{
				pthread_mutex_lock( &_muLoadClr );
				bool bLoad = false;
				if( _szClrFilePath.empty() )
				{
					_szClrFilePath = szPath;
				}else{
					string szTmpClrPath( szPath );
					if( (_szClrFilePath == szTmpClrPath ) &&
						_pColor
						)
					{
						bLoad = true;
					}else{
						_szClrFilePath = szPath;
					}
				}

				pthread_mutex_unlock( &_muLoadClr );

				if( bLoad )
				{
					return true;
				}


				int i = 0;

				if( _pColor != NULL )
				{
					if( _pColor->pField != NULL ) {
						delete []_pColor->pField;
						_pColor->pField = NULL;
					}

					if( _pColor->pClr != NULL ) {
						delete []_pColor->pClr;
						_pColor->pClr = NULL;
					}

					delete _pColor;
					_pColor = NULL;
				}

				_pColor = new tagColorFile;

				FILE *fp = fopen( szPath, "rb");
				if( fp != NULL )
				{
					fread( &_pColor->head, 1, sizeof(tagColorFileHead), fp);

					_pColor->pField = new tagColorFileField[ _pColor->head.sFieldCnt ];
					fread( _pColor->pField, _pColor->head.sFieldCnt, sizeof(tagColorFileField), fp);

					_pColor->pClr = new tagColorBlock[ _pColor->head.iColorCnt ]; 
					fread( _pColor->pClr, _pColor->head.iColorCnt, sizeof(tagColorBlock), fp);

					fclose(fp);

					FillColorField( 0 );


					return true;
				}

				delete _pColor;
				_pColor = NULL;
				return false;
				//wxLogDebug( "读取色标文件失败，使用缺省色标！\n");
			};

			/*! 根据参数重新排列颜色
			*/
			void RefreshColor()
			{
				FillColorField( 0 );
			}

			void FillColorField(int index)
			{
				if (_pColor->head.sVer == 1)//渐进色
				{
					InitializeGradualColor(index);
				}
				else //if (_pColor->head.sVer ==2)//色块
				{
					InitializeColorLump(index);
				}

			}

			//初始化色标为渐进色
			void InitializeGradualColor(int index)
			{
				/// 生成 刷子
				unsigned int iB, iBase = _pColor->pField[index].iIndex;

				// 生成色表
				unsigned int BlockCount = _pColor->pField[index].sCount;
				UINT startBlock = _pColor->pField[index].iMin;
				UINT endBlock = _pColor->pField[index].iMax;

				int r1=0,r2=0,g1=0,g2=0,b1=0,b2=0;
				float dR,dG,dB;
				memset(_rgb,0,sizeof(_rgb));
				_rgb[0]=_rgb[1]=RGB(0,0,0);//前两块为无效值，强制置为黑色
				r1=g1=b1=0;

				//填充色块
				startBlock = 2;	

				for(iB=0; iB< BlockCount; iB++) 
				{
					endBlock = _pColor->pClr[iB].end; 
					float step = endBlock - startBlock;	
					if(step<1) step=1;

					r2 = GetRValue( _pColor->pClr[iB].rgb );
					g2 = GetGValue( _pColor->pClr[iB].rgb );
					b2 = GetBValue( _pColor->pClr[iB].rgb );

					dR = (r2 - r1) / step;
					dG = (g2 - g1) / step;
					dB = (b2 - b1) / step;

					int k=0;
					for(unsigned int j = startBlock ; j<endBlock;j++ ) 
					{
						_rgb[j] = RGB(r1 + dR*k, g1 + dG*k, b1 + dB*k); 
						k++;
					}
					r1=r2;g1=g2;b1=b2;
					startBlock = _pColor->pClr[iB].end;
				}

				// 填充尾部
				startBlock = _pColor->pClr[iBase + BlockCount-1].end;
				for( iB = startBlock; iB<MAXCLRNUM; iB++ )
					_rgb[iB] = _pColor->pClr[iBase + BlockCount-1].rgb;

				if (_pColor->head.sID==4 || _pColor->head.sID==5)//速度、谱宽第一块为模糊色
					_rgb[1]=RGB(123,0,123);
			}


			//初始化色标为色块
			void InitializeColorLump(int index)
			{
				/// 生成 刷子
				unsigned int iB, iBase = _pColor->pField[index].iIndex;

				// 生成色表
				unsigned int BlockCount = _pColor->pField[index].sCount;
				UINT startBlock = _pColor->pField[index].iMin;
				UINT endBlock = _pColor->pField[index].iMax;

				memset(_rgb,0,sizeof(_rgb));
				_rgb[0]=_rgb[1]=RGB(0,0,0);//前两块为无效值，强制置为黑色

				// 填充中间的
				startBlock = 2 ;
				for(iB=0; iB< BlockCount; iB++) 
				{
					endBlock = _pColor->pClr[iB].end; 
					for(unsigned int j = startBlock ; j<endBlock; j++ ) 
						_rgb[j] = _pColor->pClr[iB].rgb;
					startBlock = _pColor->pClr[iB].end;
				}
				// 填充尾部
				startBlock = _pColor->pClr[BlockCount-1].end;
				for( iB = startBlock; iB<MAXCLRNUM; iB++ )
					_rgb[iB] = _pColor->pClr[BlockCount-1].rgb;

				if (_pColor->head.sID==4 || _pColor->head.sID==5)//速度、谱宽第一块为模糊色
					_rgb[1]=RGB(123,0,123);
			}
			
			/*! 绘制色表
				\param dc 绘制的设备ID
				\param rect 绘制的区域
				\param szD 显示的单位
				\param dataType 显示刻度的数据类型。0：整型；1：浮点型
				\param iPre 浮点型显示的精度
				\param bDrawRound 绘制矩形框
			*/
			void DrawColorTable( HDC &dc, RECT rect, const char *szD ,int dataType=1, int iPre=1,bool bDrawRound=true)
			{
				if( !_pColor ) return;

				int iF = _iActiveField;

				int iLineWid = 55; // 色标线的宽度
				int ilineMark = 6; // 刻度标记线长度
				int iInvMark = 20; // 刻度线与刻度值之间的间隔
				int iMark = 40; // 刻度值的宽度

				int iInvUnitColor = 25;//色标与单位之间的高度差
				int iInvVal = 8;//刻度值显示高度差
				
				int iVWidth = rect.right - rect.left;
				if( iVWidth <= 100 ) 
				{
					iLineWid = 10;
					ilineMark = 2; // 刻度标记线长度
					iInvMark = 1; // 刻度线与刻度值之间的间隔
					iMark = 35; // 刻度值的宽度
				}

				if (iVWidth > 300)
				{
					iLineWid = iVWidth / 3; // 色标线的宽度
					ilineMark = iLineWid / 10; // 刻度标记线长度
					iInvMark = ilineMark; // 刻度线与刻度值之间的间隔
					iMark = (iVWidth - iLineWid - ilineMark - iInvMark); // 刻度值的宽度
				}
				
				int iWidTxt = rect.right - rect.left;		// 绘制矩形框的宽度
				//// 绘制矩形框	
				char szOut[30] = {0};
				RECT rectT;
				float fValPer = ( _pColor->pField[iF].max - _pColor->pField[iF].min)/ float(_pColor->pField[iF].iMax - _pColor->pField[iF].iMin);

				unsigned int BlockCount = _pColor->pField[0].sCount;

				//////// 新的色标绘制方法
				/// 判断单位
				string szT(_pColor->pField[iF].szUnit);
				/// 绘制色标
				int sX = rect.left, sY = rect.top;
				unsigned int startBlock;
				unsigned int endBlock;
				COLORREF oldTxt = ::SetTextColor( dc, RGB(255,255,255) );
				int mode = ::SetBkMode( dc, TRANSPARENT );

				unsigned int i = 0,j=0;
				/// 绘制色标
				/*
				1. 获取绘制的区域；
				2. 按色块平分绘制区域；
				3. 填充色块；
				计算色块内每单位高度包含几个数据；
				数据 >= 1
				按比例从色表中抽取颜色值；
				数据 < 1
				按比例取值
				*/
				int sHei = rect.bottom - rect.top - 16; // 16 是单位显示的高度位置

				if( sHei <= 0 )
					return;

				unsigned int iHei = 0;
				if (_pColor->head.sVer ==1)//渐进色中暂不考虑速度、谱宽的模糊。
				{
					iHei = sHei / BlockCount;
				}
				else //if (_pColor->head.sVer ==2)
				{
					if (_pColor->head.sID==4 || _pColor->head.sID==5)//在色块中速度、谱宽需要多加一块模糊色。
					{
						iHei = sHei / (BlockCount+1);
					}
					else
					{
						iHei = sHei / BlockCount;
					}
				}
				//根据图片大小设置高度间隔
				if ( iVWidth > 300)//在绘图显示时是固定为182;
				{
					iInvUnitColor = iHei/4.*3.;
					iInvVal = iHei/4.;
				}

				int iInv = 0;
				float fDet;
				int sBlo = sY + iInvUnitColor;
				HPEN penB = ::CreatePen(PS_SOLID, 1, RGB(155,155,155));
				HPEN oldPen = (HPEN)::SelectObject( dc, penB );

				/// 根据高度设置字体大小
				int iPtFont = iHei * 5.5;
				if (iVWidth > 300 && iPtFont > 500) iPtFont = 500;
				else if( iVWidth > 300 && iPtFont > 200 ) iPtFont = 200;
				else if( iPtFont > 100 ) iPtFont = 100;

				HFONT hf = NULL;
				HFONT oldFont = NULL;
				if( iPtFont >= 100 )
				{
					hf = ::CreatePointFont(  iPtFont,  "微软雅黑" , 0 , false );
					oldFont = (HFONT)::SelectObject( dc, hf );  
				}

				// 绘制单位---------在字体大小设置后再进行绘制
				rectT.left = rect.left;
				rectT.top = rect.top;
				rectT.right = sX + rect.right;
				rectT.bottom  = rectT.top + iInvUnitColor;
				sprintf( szOut,"%s", szT.c_str() );
				::DrawTextA( dc, szOut, 
					strlen(szOut), 
					&rectT, 
					DT_LEFT | DT_VCENTER );

				char* szSEIout[10] ={"小雨","中雨","大雨","暴雨","大暴雨","特大暴雨"};
				char* szHCLout[STRHCLOUTCOUNT] =STRHCLOUT;

				for( i=0; i<BlockCount-1; i++)
				{
					startBlock = _pColor->pClr[ i ].end;
					endBlock = _pColor->pClr[ i+1].end;

					iInv = endBlock - startBlock;
					fDet = (float)iInv / (float)iHei;

					
					// 递归每个单位高度，绘制线
					for( j=0;j<iHei; j++ )
					{
						HPEN pen = ::CreatePen(PS_SOLID, 1, _rgb[ int(startBlock + fDet * j) ]);
						::SelectObject( dc, pen );
						sBlo +=1;
						::MoveToEx(dc, sX , sBlo, NULL );
						if( j == 0 ) 
						{
							if (i==0&&_pColor->head.sID==4&&_pColor->head.sID==2)
							{
							}
							else if (_pColor->head.sID == PID_HCL)
							{
								int sBloCen =  sBlo+iHei/2;
								::LineTo( dc, sX+iLineWid,sBloCen);
								::SelectObject( dc, penB );
								::MoveToEx( dc, sX+iLineWid, sBloCen, NULL);
								::LineTo( dc, sX+iLineWid + ilineMark, sBloCen );

								/////// 计算刻度值的位置
								rectT.left = sX + iLineWid+ilineMark + 4;
								rectT.right = sX+ iLineWid + ilineMark + iInvMark + iMark + 30;
								rectT.top = sBloCen-8;
								rectT.bottom = sBloCen+8;

								if (i<STRHCLOUTCOUNT)
								{
									sprintf( szOut, "%s",szHCLout[i] );
								}
								else
								{
									sprintf( szOut, "%s","超出范围" );
								}
								

								::DrawTextA( dc, szOut, 
									(int)strlen(szOut), 
									&rectT, 
									DT_LEFT | DT_VCENTER );

								::DrawTextA( dc, szOut, 
									(int)strlen(szOut), 
									&rectT, 
									DT_CALCRECT|DT_LEFT | DT_VCENTER );
								int wT = rectT.right - rectT.left + 7;
								if( wT > iWidTxt )
									iWidTxt = wT;
							}
							else
							{
								::LineTo( dc, sX+iLineWid, sBlo);
								::SelectObject( dc, penB );
								::MoveToEx( dc, sX+iLineWid, sBlo, NULL);
								::LineTo( dc, sX+iLineWid + ilineMark, sBlo );//色标的上边界线及刻度标记线

								/////// 计算刻度值的位置
								rectT.left = sX + iLineWid+ilineMark + 4;
								rectT.right = sX+ iLineWid + ilineMark + iInvMark + iMark + 30;
								rectT.top = sBlo - iInvVal;
								rectT.bottom = sBlo + iInvVal;

								if( dataType == 0 ) // 整型
								{
									memset( szOut, 0, 30 );
									sprintf( szOut, "%d", 
										int(_pColor->pField[iF].min + fValPer * (startBlock - _pColor->pField[iF].iMin)));
								}
								else if (dataType == 1)
								{
									char szPre[20] = {0};
									sprintf( szPre, "%%.%df ", iPre);
									memset( szOut, 0, 30 );
									sprintf( szOut, szPre, 
										_pColor->pField[iF].min + fValPer * (startBlock - _pColor->pField[iF].iMin));
								}
								else
								{
									rectT.top = sBlo+16;
									rectT.bottom = sBlo + 40;
									memset( szOut, 0, 30 );
									if (_pColor->head.sID == 50)
									{
										sprintf( szOut, "%s",szSEIout[i] );//获取最后一个色块的结束刻度线的值
									}
									
								}

								::DrawTextA( dc, szOut, 
									(int)strlen(szOut), 
									&rectT, 
									DT_LEFT | DT_VCENTER );

								::DrawTextA( dc, szOut, 
									(int)strlen(szOut), 
									&rectT, 
									DT_CALCRECT|DT_LEFT | DT_VCENTER );
								int wT = rectT.right - rectT.left + 7;
								if( wT > iWidTxt )
									iWidTxt = wT;
							}
						}
						else
							::LineTo( dc, sX+iLineWid, sBlo);

						::DeleteObject( pen );
						
					}

					//// 绘制最后一个刻度线
					if( i == (BlockCount-2) )//最后一个色块
					{
						HPEN pen = ::CreatePen(PS_SOLID, 1, _rgb[ int(endBlock + fDet * j) ]);
						::SelectObject( dc, pen );

						::MoveToEx(dc, sX , sBlo, NULL );
						::LineTo( dc, sX+iLineWid, sBlo);

						//对于色标为色块号产品及渐进色需画最后一个刻度线。
						if ((_pColor->head.sVer!=1)&&(_pColor->head.sID==50))
						{
						}
						else if (_pColor->head.sID == PID_HCL)
						{
							//最后一个不需要绘制色块
						}
						else
						{
							::SelectObject( dc, penB );
							::MoveToEx( dc, sX+iLineWid, sBlo, NULL);
							::LineTo( dc, sX+iLineWid + ilineMark, sBlo );
							///// 计算刻度值的位置
							rectT.left = sX + iLineWid+ilineMark + 4;
							rectT.right = sX+ iLineWid + ilineMark + iInvMark + iMark + 30;
							rectT.top = sBlo - iInvVal;
							rectT.bottom = sBlo + iInvVal;

							if( dataType == 0 ) // 整型
							{
								memset( szOut, 0, 30 );
								sprintf( szOut, "%d", 
									int(_pColor->pField[iF].min + fValPer * (endBlock - _pColor->pField[iF].iMin)));//获取最后一个色块的结束刻度线的值
							}else{
								char szPre[20] = {0};
								sprintf( szPre, "%%.%df ", iPre);
								memset( szOut, 0, 30 );
								sprintf( szOut, szPre, 
									_pColor->pField[iF].min + fValPer * (endBlock - _pColor->pField[iF].iMin));//获取最后一个色块的结束刻度线的值
							}

							::DrawTextA( dc, szOut, 
								(int)strlen(szOut), 
								&rectT, 
								DT_LEFT | DT_VCENTER );
						}
						::DeleteObject( pen );
						
					}
					
					
				}
				if ((_pColor->head.sID==4 || _pColor->head.sID==5)
					&&(_pColor->head.sVer !=1))
				{
					COLORREF clr= RGB(123,0,123);//该色为速度、谱宽的模糊色
					for( j=0;j<iHei; j++ )
					{
						HPEN pen = ::CreatePen(PS_SOLID, 1,clr);
						::SelectObject( dc, pen );
						::MoveToEx(dc, sX , sBlo+j, NULL );
						::LineTo( dc, sX+iLineWid-3, sBlo+j);

						///// 计算刻度值的位置
						rectT.left = sX + iLineWid+ilineMark + 4;
						rectT.right = sX+ iLineWid + ilineMark + iInvMark + iMark + 30;
						rectT.top = sBlo+iInvVal;
						rectT.bottom = sBlo+iInvVal*3;
						memset( szOut, 0, 30 );
						sprintf( szOut, "%s","RF" );//获取最后一个色块的结束刻度线的值
						::DrawTextA( dc, szOut, 
							(int)strlen(szOut), 
							&rectT, 
							DT_LEFT | DT_VCENTER );
						::DeleteObject( pen );
					}


				}

				::SelectObject( dc, oldFont);
				::SelectObject( dc, oldPen );
				::SetTextColor( dc, oldTxt );
				if (penB != NULL)
				{
					::DeleteObject( penB );
				}
				if (hf != NULL)
				{
					::DeleteObject( hf );
				}
				

				//// 绘制色表的矩形框
				if (bDrawRound)
				{
					HPEN penCir = ::CreatePen( PS_SOLID, 1, RGB(77,77,77) );
					::SelectObject( dc, ::GetStockObject( NULL_BRUSH ) );
					::SelectObject( dc, penCir );

					::RoundRect( dc,
						rect.left-3,
						rect.top - 5,
						rect.left + iWidTxt,
						rect.bottom - 3,
						6,6
						);
					::SelectObject( dc, oldPen );
					::DeleteObject( penCir );
				}
				

				::SetBkMode( dc, mode );
			}
			
			//************************************
			// Method:    GetHCLStrValue
			// FullName:  CDYW::RadarSystem::CColorTableT<T>::GetHCLStrValue
			// Access:    public 
			// Returns:   char *
			// Qualifier: 更具输入的数值，返回对应的文字
			// Parameter: int ivalue
			//************************************
			char * GetHCLStrValueFromClr(int ivalue)
			{
				if (ivalue>=0 && ivalue<STRHCLOUTCOUNT)
				{
					char* szHCLout[STRHCLOUTCOUNT] =STRHCLOUT;
					return szHCLout[ivalue];
				}
				return NULL;
			};

			//************************************
			// Method:    GetUnitFromClr
			// FullName:  CDYW::RadarSystem::CColorTableT<T>::GetUnitFromClr
			// Access:    public 
			// Returns:   char *
			// Qualifier: 返回单位
			//************************************
			char * GetUnitFromClr()
			{
				return _pColor->pField[_iActiveField].szUnit;
			};
		};

	}; //end namespace RadarSystem
}; // end namespace CDYW

#endif

