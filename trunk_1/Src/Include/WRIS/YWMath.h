
#ifndef _MATH_CDYW_0802010
#define _MATH_CDYW_0802010

namespace CDYW
{
	namespace MATH
	{
		//! ����ƽ�����ĺ������ȿ⺯������1/3;
		float SquareRootFloat( float number );
		
		//! �ڴ�������2�ֽ�
		inline void MemSet_WORD( void *dest, unsigned short int data, int count )
		{
			_asm {
				mov edi,  dest; 
				mov ecx, count;
				mov ax, data; 
				rep stosw;
			}
		}
		
		//! �ڴ�������4�ֽ�
		inline void MemSet_QUAD( void *dest, unsigned int data, int count )
		{
			_asm {
				mov edi,  dest; 
				mov ecx, count;
				mov eax, data; 
				rep stosd;
			}
		}
	}; // end namespace MATH
}; // end namespace CDYW





#endif

