/*********************************************************/
/*! \file BaseValue.h
文件名称：BaseView.h
摘    要：本文件定义了几种雷达的基数据转换的公式

原版本：1.0
原作者  ：舒定波
完成日期：2008年05月30日

当前版本：1.0
作    者：舒定波
完成日期：2008年05月30日

修改记录：

版权信息：
	Copyright (c) 2008,ChengDu YuanWang Science & Technology CO., LTD.
	All rights reserved.
*/
/*********************************************************/

#ifndef _HEADER_DEFBASEVALUE_20080530
#define _HEADER_DEFBASEVALUE_20080530

//#if (defined(RADAR_XX2))
//#	define GETREFLVAL(x,y) (x-64)*0.5f
//#	define GETVELVAL(x,y) (char)x*y 	// x 速度，y最大可测速度的1/127/100 单位m/s；速度以最大可测速度的1/127为单位， cm/s ，色标29：-50m/s，229：50m/s
//#	define GETWIDTHVAL(x,y) (char)x*y // x 速度，y最大可测速度的1/512/100 单位m/s；速度以最大可测速度的1/512为单位， cm/s
//#elif (defined(RADAR_XX6))
//#	define GETREFLVAL(x,y) (x-64)*0.5f
//#	define GETVELVAL(x,y) x*y 	// x 速度，y最大可测速度的1/127/100 单位m/s；速度以最大可测速度的1/127为单位， cm/s
//#	define GETWIDTHVAL(x,y) x*y // x 速度，y最大可测速度的1/512/100 单位m/s；速度以最大可测速度的1/512为单位， cm/s
//#else
/*
#	define GETREFLVAL(x) (x-2)*0.5-32.0f
#	define GETVELVAL(x) (x-2)*0.5-63.5f
#	define GETWIDTHVAL(x) (x-129.0f)/2
#	define GETREFL(x) ( 2*x+66 )  //反射率色标映射关系
#	define GETLTA(x) ( 8*x+2 )
#	define GETVEL(x) ( int((x+63.5)*2) + 2 ) //速度色标映射关系
#	define GETWIDTH(x) (int(x*2) + 129 )
#   define GET_ZDR_VAL(x) 0.1*x-5     //差分反射率因子映射关系
#   define GET_PDP_VAL(x) 1.6*x-4.8   //差分相位映射关系
#   define GET_KDP_VAL(x) 0.05*x-3    //差分相移因子映射关系
#   define GET_RHV_VAL(x) 0.01*x-0.05 //零延迟相关系数映射关系
*/

//#endif

#endif
