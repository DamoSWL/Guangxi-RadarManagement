
#ifndef _HEADER_COLOR_FILE_080303
#define _HEADER_COLOR_FILE_080303

#define DEF_BLOCK_WID	45
#define DEF_BLOCK_HEI	16

#include < vector >
using namespace std;

#pragma pack(push, 1)
//! ɫ���ļ���Ϣ
//! ɫ���ļ�ͷ
typedef struct _CLRFILEHEAD {
	short int sVer;	// �ļ��汾��
	char szName[50];  //!< ��ɫ�ļ���Ӧ���� 
	short int sID;		  //!< ��Ӧ�Ĳ�Ʒ���
	short int sFieldCnt;  //!< ��ɫ��ĸ���
	int iColorCnt;		//!< �ܹ��е���ɫɫ����Ŀ
}tagColorFileHead;

//! ��ɫ����Ϣ����
typedef struct _CLRFILEFIELD {
	unsigned short sCount;  //!< ��ʾ����ɫ�Ŀ���
	unsigned short spa;  //!< ��ʾ����ɫ�Ŀ���
	unsigned int iIndex; //!< ��ɫ���һ��ɫ�������б��е���ţ�
	char szName[40];//��Ʒ����
	char szUnit[12];//20141127wwm,�޸�Ϊ��λ�ַ���

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

	_CLRFILEFIELD *pField;	//!< ��ɫ��ָ��
	_CLRFILRBLOCK *pClr;
}tagColorFile;

//! ɫ����ʾ����
struct _RectCol		
{
	RECT rect;   //!< ɫ����ʾ������
	COLORREF clr; //!< ɫ�����ɫ
};

#endif


