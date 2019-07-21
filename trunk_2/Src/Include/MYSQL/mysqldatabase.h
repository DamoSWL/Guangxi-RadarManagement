#ifndef MY_SQL_DATABASE_20110623
#define MY_SQL_DATABASE_20110623

#include "mysql.h"
#include "vector"
using namespace std;

//**************ʹ��ʱ�������Ҫ�޸���������*******************
//#pragma comment(lib,"libmysql.lib")
//#pragma comment(lib,"mysqlclient.lib")
//#pragma comment(lib,"mysys-max.lib")
//#pragma comment(lib,"mysys-nt.lib")
//#pragma comment(lib,"regex.lib")
//#pragma comment(lib,"strings.lib")
//#pragma comment(lib,"zlib.lib")

#define DEFAULT_FIELD        20  //����Ҫ�����Ŀ��У����ݱ�������ֶ���������student����5���ֶΣ�
                                 //�ֱ���name,old,sex,profession,class����teacher����10���ֶΣ���
                                 //��ֵӦ��10���������Ϊ����ֶ���������̫���ռ�÷ǳ�����ڴ档

#define DEFAULT_FIELD_SIZE   20  //ÿ���ֶ����Ƶ�����ֽ������磺nameΪ4���ֽڣ�professionΪ10���ֽ�
                                 //�˴�Ӧ����10��ע�⣺һ�����ֵ���2���ֽڣ�������ʹ�ú�����Ϊ�ֶ�����
                                 //ͬ������ֵҲ��Ӧ�����ù��󣬷���Ҳ��ռ�÷ǳ�����ڴ档
#define TEXT_MAX_LEN		255	//�ı����ĳ���

typedef struct _FIELDS					
{
	char     FieldName[DEFAULT_FIELD_SIZE];		//�ֶ���
	void     *FieldValue;						//�ֶ�ֵ��ָ��
	int      ValueLength;						//�ֶεĳ��ȣ�ֻ�������ַ�������
}tagField;										//һ���ֶνṹ


typedef struct _ROWS
{
	tagField Fields[DEFAULT_FIELD];		 //�ֶ�,���ݲ�ͬ��Ӧ�ó�������޸Ĵ�ֵ
	int		 FieldCnt;					 //���ٸ��ֶΣ�ָʾfield��Ԫ�ص���Ч����
}tagRow;								 //һ�����ݵĽṹ����

class MySqlDatabase
{
private:
	MYSQL		m_MySqlHandle;						//MySql�����Ӿ��
	MYSQL_STMT *m_MySqlStmt;						//MySqlԤ������

    MYSQL_RES		 *m_MySqlResult;				//MySql�Ľ����
	vector<tagRow*>  m_VecResult;					//�洢��ѯ�����������

	char			m_Password[TEXT_MAX_LEN];
	char			m_Host[TEXT_MAX_LEN];
	char			m_UserName[TEXT_MAX_LEN];
	unsigned int	m_Port;
public:
	MySqlDatabase();
	~MySqlDatabase();
	bool Connect(char *Host,char *UserName,char *Password,char *Database,unsigned int Port=3306);		//����MySql������,
	
	bool ExecuteNoQuery(wxArrayString SqlArray,int Count,wxString &strErrorSql,wxString &strError);//ִ�зǲ�ѯ���,���سɹ���ʧ��
	bool ExecuteNoQuery(wxString Sql,wxString &strError);											//ִ��һ���ǲ�ѯ���,�Էֺ�Ϊ����

	int ExecuteQuery(char * Sql);										//ִ�в�ѯ��䣬���ز�ѯ��������������������Ҫ����FreeResult�ͷŽ��
																		//���������FreeResult�������´β�ѯʱ�Զ��ͷ��ϴεĽ����
	bool FreeResult(wxString &strerror);													//�ͷŲ�ѯ�Ľ����	

	bool ChangeUser(char *UserName,char *Password,char *Database);		//�ı��û���ʹ��ȴʡDatabase�����DatabaseΪ�գ���ʹ��
	bool SelectDB(char *Database);										//�Ե�ǰ�û�����ݣ������������ݿ�
	void CloseDatabase();														//�ر�����

	//ִ�д������ķ�SELECT SQL���,�����ĸ������޷���4.1���µİ汾��ʹ��
	bool ExecutePrepare(char *Sql);
	bool BindParam(MYSQL_BIND *Bind);
    int  Execute();
	bool ClosePrepare();


	//���ָ���к�ָ���е�����ֵ
	//���ж���0��ʼ���
        // row     ���к�
        // fd_name ��������
        // fd_num  �����ֶκ�
        // ע��    ��Get**Valueϵ�к����Ĳ���Ҫȷ����ȷ�����򲻱�֤����ֵ����ȷ
	int GetIntValue(int RowIndex, char *FieldName);
	UINT64 GetIntValue64(int RowIndex, char *FieldName);
	int GetIntValue(int RowIndex, int FieldNum);

	//��÷�����ֵ
	//��0��ʼ���
	float GetFloatValue(int RowIndex,char *FieldName);
	float GetFloatValue(int RowIndex,int FieldNum);

	//��ø߾��ȷ�����ֵ
	//��0��ʼ���
	double GetDoubleValue(int RowIndex,char *FieldName);
	double GetDoubleValue(int RowIndex,int FieldNum);

	//����ַ���ֵ�������ַ�������
	//��0��ʼ���
	char *GetStringValue(int RowIndex,char *FieldName,int &Length);
    char *GetStringValue(int RowIndex,int  FieldNum,  int &Length);

	//�������ʱ�������ݣ����ַ�����ʽ��ʾ
	//��0��ʼ���
	//��ʽ���£�0000-00-00 00:00:00
	char *GetDataTimeValue(int RowIndex,char *FieldName);
	char *GetDataTimeValue(int RowIndex,int FieldNum);

	//��ö��������ݵĻ�����
	//��0��ʼ��ţ������ݸ�pBuf==NULLʱ����û���������
	int GetBinaryValue(int RowIndex,char *FieldName,char * pBuf);
	int GetBinaryValue(int RowIndex,int  FieldNum,  char * pBuf);
	//
	//��ò�����ֵ
	//��0��ʼ���
	//bool GetBoolValue(int row,char * fd_name);
	//bool GetBoolValue(int row,int fd_num);
};

#endif