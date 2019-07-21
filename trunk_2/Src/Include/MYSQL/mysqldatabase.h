#ifndef MY_SQL_DATABASE_20110623
#define MY_SQL_DATABASE_20110623

#include "mysql.h"
#include "vector"
using namespace std;

//**************使用时请根据需要修改以下数据*******************
//#pragma comment(lib,"libmysql.lib")
//#pragma comment(lib,"mysqlclient.lib")
//#pragma comment(lib,"mysys-max.lib")
//#pragma comment(lib,"mysys-nt.lib")
//#pragma comment(lib,"regex.lib")
//#pragma comment(lib,"strings.lib")
//#pragma comment(lib,"zlib.lib")

#define DEFAULT_FIELD        20  //你所要操作的库中，数据表的最大的字段数。假如student表有5个字段，
                                 //分别是name,old,sex,profession,class，而teacher表有10个字段，则
                                 //此值应添10。最好设置为最大字段数，设置太大会占用非常大的内存。

#define DEFAULT_FIELD_SIZE   20  //每个字段名称的最大字节数。如：name为4个字节，profession为10个字节
                                 //此处应该添10。注意：一个汉字等于2个字节，不建议使用汉字作为字段名。
                                 //同样，此值也不应该设置过大，否则也会占用非常大的内存。
#define TEXT_MAX_LEN		255	//文本最大的长度

typedef struct _FIELDS					
{
	char     FieldName[DEFAULT_FIELD_SIZE];		//字段名
	void     *FieldValue;						//字段值的指针
	int      ValueLength;						//字段的长度，只适用于字符串类型
}tagField;										//一个字段结构


typedef struct _ROWS
{
	tagField Fields[DEFAULT_FIELD];		 //字段,根据不同的应用程序可以修改此值
	int		 FieldCnt;					 //多少个字段，指示field中元素的有效个数
}tagRow;								 //一行数据的结构数组

class MySqlDatabase
{
private:
	MYSQL		m_MySqlHandle;						//MySql的连接句柄
	MYSQL_STMT *m_MySqlStmt;						//MySql预处理句柄

    MYSQL_RES		 *m_MySqlResult;				//MySql的结果集
	vector<tagRow*>  m_VecResult;					//存储查询结果集的容器

	char			m_Password[TEXT_MAX_LEN];
	char			m_Host[TEXT_MAX_LEN];
	char			m_UserName[TEXT_MAX_LEN];
	unsigned int	m_Port;
public:
	MySqlDatabase();
	~MySqlDatabase();
	bool Connect(char *Host,char *UserName,char *Password,char *Database,unsigned int Port=3306);		//连接MySql服务器,
	
	bool ExecuteNoQuery(wxArrayString SqlArray,int Count,wxString &strErrorSql,wxString &strError);//执行非查询语句,返回成功或失败
	bool ExecuteNoQuery(wxString Sql,wxString &strError);											//执行一条非查询语句,以分号为结束

	int ExecuteQuery(char * Sql);										//执行查询语句，返回查询到的行数，处理完结果后，要调用FreeResult释放结果
																		//如果不调用FreeResult，则在下次查询时自动释放上次的结果集
	bool FreeResult(wxString &strerror);													//释放查询的结果集	

	bool ChangeUser(char *UserName,char *Password,char *Database);		//改变用户，使用却省Database，如果Database为空，则不使用
	bool SelectDB(char *Database);										//以当前用户的身份，连接其他数据库
	void CloseDatabase();														//关闭连接

	//执行带参数的非SELECT SQL语句,以下四个函数无法在4.1以下的版本中使用
	bool ExecutePrepare(char *Sql);
	bool BindParam(MYSQL_BIND *Bind);
    int  Execute();
	bool ClosePrepare();


	//获得指定行和指定列的整形值
	//行列都从0开始编号
        // row     ：行号
        // fd_name ：列名称
        // fd_num  ：列字段号
        // 注意    ：Get**Value系列函数的参数要确保正确，否则不保证返回值将正确
	int GetIntValue(int RowIndex, char *FieldName);
	UINT64 GetIntValue64(int RowIndex, char *FieldName);
	int GetIntValue(int RowIndex, int FieldNum);

	//获得符点型值
	//从0开始编号
	float GetFloatValue(int RowIndex,char *FieldName);
	float GetFloatValue(int RowIndex,int FieldNum);

	//获得高精度符点型值
	//从0开始编号
	double GetDoubleValue(int RowIndex,char *FieldName);
	double GetDoubleValue(int RowIndex,int FieldNum);

	//获得字符串值，返回字符串长度
	//从0开始编号
	char *GetStringValue(int RowIndex,char *FieldName,int &Length);
    char *GetStringValue(int RowIndex,int  FieldNum,  int &Length);

	//获得日期时间型数据，以字符串形式表示
	//从0开始编号
	//格式如下：0000-00-00 00:00:00
	char *GetDataTimeValue(int RowIndex,char *FieldName);
	char *GetDataTimeValue(int RowIndex,int FieldNum);

	//获得二进制数据的缓冲区
	//从0开始编号，当传递给pBuf==NULL时，获得缓冲区长度
	int GetBinaryValue(int RowIndex,char *FieldName,char * pBuf);
	int GetBinaryValue(int RowIndex,int  FieldNum,  char * pBuf);
	//
	//获得布尔型值
	//从0开始编号
	//bool GetBoolValue(int row,char * fd_name);
	//bool GetBoolValue(int row,int fd_num);
};

#endif