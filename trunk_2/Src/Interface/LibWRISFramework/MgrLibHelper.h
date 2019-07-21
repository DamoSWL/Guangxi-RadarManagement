
/*! \file MgrLibHelper.h
	\brief LibGRSFramework的辅助类
	本类用于创建LibGRSFramework的实例
*/

#ifndef __HELPER_MGR_LIB_20080813
#define __HELPER_MGR_LIB_20080813


/*! \class Mgr Manager.h
\brief 辅助类
\ingroup PluginFWGroup
*/
template <class MgrT> 
class Mgr
{
	static MgrT *instance;
	static bool isShutdown;
	explicit Mgr(const Mgr<MgrT>&){};
	//Mgr<MgrT>& operator=(Mgr<MgrT> const&){};

protected:

	Mgr(){assert(Mgr<MgrT>::instance == 0);}
	virtual ~Mgr(){Mgr<MgrT>::instance = 0;}

public:

	static inline bool Valid(){return instance?true:false;}

	static inline MgrT* Get()
	{
		if(instance == 0 && isShutdown == false)
			instance = new MgrT();

		return instance;
	}

	static void Free()
	{
		isShutdown = true;
		delete instance;
		instance = 0;
	}
};

#endif 



