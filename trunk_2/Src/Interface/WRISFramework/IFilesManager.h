

#ifndef __INTERFACE_IFilesManager_200931
#define __INTERFACE_IFilesManager_200931

typedef bool (*FunOpenFiles)( int nPaths, char **szPathNames );


class IFilesManager
{
public:

	/*! ע���ļ��򿪺���
		\param pluginID ���ID
		\param szFileType �ļ�����
		\param pFunOpenFiles ����ָ��
		\return true,ע��ɹ���false,ע��ʧ��
	*/
	virtual bool RegOpenFiles( PL_PID pluginID, char *szFileType, FunOpenFiles pFunOpenFiles ) = 0;
	
	/*! ע���ļ��򿪺���
		\param pluginID ���ID
		\return true,ע���ɹ���false,ע��ʧ��
	*/
	virtual bool UnRegOpenFiles( PL_PID pluginID, char *szFileType ) = 0;
};

#endif // __HEADER_IFilesManager_200931

