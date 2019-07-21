
#ifndef _HEADER_COLOR_FILE_080303
#define _HEADER_COLOR_FILE_080303

#define DEF_BLOCK_WID	45
#define DEF_BLOCK_HEI	16

#include < vector >
using namespace std;

#pragma pack(push, 1)
//! 色标文件信息
//! 色标文件头
typedef struct _CLRFILEHEAD {
	short int sVer;	// 文件版本号
	char szName[50];  //!< 颜色文件对应名称 
	short int sID;		  //!< 对应的产品编号
	short int sFieldCnt;  //!< 颜色域的个数
	int iColorCnt;		//!< 总共有的颜色色块数目
}tagColorFileHead;

//! 颜色域信息描述
typedef struct _CLRFILEFIELD {
	unsigned short sCount;  //!< 显示的颜色的块数
	unsigned short spa;  //!< 显示的颜色的块数
	unsigned int iIndex; //!< 颜色域第一颜色数据在列表中的序号；
	char szName[40];//产品名称
	char szUnit[12];//20141127wwm,修改为单位字符串

	float min;		//!< the normal min value
	float max;		//!< the normal max value
	UINT iMin;		//!< the normalization min value
	UINT iMax;		//!< the normalization max value
	UINT type;	
}tagColorFileField;

typedef struct _CLRFILRBLOCK {
	UINT index;		//!< The index of color block
	UINT end;		//!< The value of color block
	union{
		COLORREF rgb;	//!< the color value of color block
		struct{
			unsigned char R;
			unsigned char G;
			unsigned char B;
			unsigned char spa;
		};
	};
}tagColorBlock;

#pragma pack( pop )


typedef struct _CLRFILE {
	_CLRFILEHEAD head;

	_CLRFILEFIELD *pField;	//!< 颜色域指针
	_CLRFILRBLOCK *pClr;
}tagColorFile;

//! 色标显示区域
struct _RectCol		
{
	RECT rect;   //!< 色标显示的区域
	COLORREF clr; //!< 色标的颜色
};

#endif


