#ifndef IWorkFlow_h__20090902
#define IWorkFlow_h__20090902

/*! \class ������������

*/
class IWorkFlowControl
{
public:
	virtual bool StartWorkFlow() = 0;				//����������
	virtual bool StopWorkFlow() = 0;				//ֹͣ������
	virtual bool IsCanStop()=0;						//�Ƿ����˳������ڹرճ���ʱ���ã�
};


/*! \class IWorkFlow
	�������ӿ�
*/
class IWorkFlow
{
public:
	virtual bool RegWorkFlowControl( IWorkFlowControl *IWFC ) = 0;			//! ע�Ṥ����������
};


/*! \brief ���������ܽӿ�ID  {079FFFB9-01E0-4e07-B7EA-3FBBC42A719E}
*/
static const PL_GUID guid_I_WorkFlow =
{ 0x79fffb9, 0x1e0, 0x4e07, { 0xb7, 0xea, 0x3f, 0xbb, 0xc4, 0x2a, 0x71, 0x9e } };


#endif // IWorkFlow_h__20090902



