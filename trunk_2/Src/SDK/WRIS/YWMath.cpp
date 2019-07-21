#include "../../include/WRIS/ywmath.h"

namespace CDYW
{
	namespace MATH
	{
		float SquareRootFloat( float number )
		{
			long i;
			float x,y;

			const float f = 1.5f;
			x = number * 0.5f;
			y = number;
			i = *(long*)&y;
			//i = 0x5f3759df - ( i >> 1 );
			i = 0x5f375a86 - ( i >> 1 );

			y = *(float*)&i;
			y = y * ( f - ( x * y * y) );
			y = y * ( f - ( x * y * y) );

			return number * y;
		}
		
	}; // end namespace MATH
}; // end namespace CDYW

