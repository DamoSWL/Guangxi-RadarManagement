
#ifndef _MATH_CDYW_0802010
#define _MATH_CDYW_0802010

namespace CDYW
{
	namespace MATH
	{
		//! 计算平方根的函数，比库函数快了1/3;
		float SquareRootFloat( float number );
		
		//! 内存快速填充2字节
		inline void MemSet_WORD( void *dest, unsigned short int data, int count )
		{
			_asm {
				mov edi,  dest; 
				mov ecx, count;
				mov ax, data; 
				rep stosw;
			}
		}
		
		//! 内存快速填充4字节
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

