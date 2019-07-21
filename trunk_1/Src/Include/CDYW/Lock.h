/*! \file Lock.h
	\brief 本文定义了Lock类
*/


#ifndef Lock_h_20090823
#define Lock_h_20090823


#include <pthread.h>


namespace CDYW{
	
	/*! \class Lock
	*/
	class Lock
	{
		pthread_mutex_t *_pMu;
	public:
		Lock( pthread_mutex_t *pMu )
		{
			_pMu = pMu;
			pthread_mutex_lock( pMu );
		};

		~Lock( )
		{
			pthread_mutex_unlock( _pMu );
		}

	};
}; // end namespace CDYW

#endif // Lock_h_20090823


