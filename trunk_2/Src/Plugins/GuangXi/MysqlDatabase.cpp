#include "mysql/MySqlDatabase.h"
#include "WX_Pch.h"
MySqlDatabase::MySqlDatabase()
{
	m_MySqlResult = NULL;
	m_Port        = 0;
	memset(m_Host,0,TEXT_MAX_LEN);
	memset(m_UserName,0,TEXT_MAX_LEN);
	memset(m_Password,0,TEXT_MAX_LEN);
	
	m_VecResult.clear();
}
MySqlDatabase::~MySqlDatabase()
{
	CloseDatabase();
}
bool MySqlDatabase::Connect(char *Host,char *UserName,char *Password,char *Database,unsigned int Port)		//连接MySql服务器
{
	if(strlen(Host)>TEXT_MAX_LEN || strlen(UserName)>TEXT_MAX_LEN || strlen(Password)>TEXT_MAX_LEN)
	{
		return false;
	}
	strcpy_s(m_Host,sizeof(char)*TEXT_MAX_LEN,Host);
	strcpy_s(m_UserName,sizeof(char)*TEXT_MAX_LEN,UserName);
	strcpy_s(m_Password,sizeof(char)*TEXT_MAX_LEN,Password);

	mysql_init(&m_MySqlHandle);

	unsigned int TimeOut = 5*60;
	mysql_options(&m_MySqlHandle, MYSQL_OPT_CONNECT_TIMEOUT, (char*)&TimeOut);		//设置连接超时

	my_bool reconnect=true;
	mysql_options(&m_MySqlHandle, MYSQL_OPT_RECONNECT, (char *)&reconnect);			//启动再连接

	m_Port = Port;

	if(mysql_real_connect(&m_MySqlHandle,Host,UserName,Password,Database,Port,NULL,CLIENT_MULTI_STATEMENTS)==NULL)				//连接服务器
	{
		return false;
	}
	
	if(mysql_set_character_set(&m_MySqlHandle, "gb2312"))							//设置连接字符集，默认为gb2312
	{										
		CloseDatabase();																	//如果设置字符集不成功，则关闭连接	
		return false;
	}
	mysql_set_server_option(&m_MySqlHandle, MYSQL_OPTION_MULTI_STATEMENTS_ON);		//设置允许多语句支持		
	return true;
}
bool MySqlDatabase::ExecuteNoQuery(wxArrayString SqlArray,int Count,wxString &strErrorSql,wxString &strError)						//执行非查询语句
{
	if ((INT)SqlArray.GetCount() < Count || Count==0)
	{
		strError = wxT("SQL批量语句未空");
		return false;
	}
	if ((mysql_more_results(&m_MySqlHandle) || m_MySqlResult!=NULL) && !FreeResult(strError))				//有返回值，需要清空
	{
		return false;
	}
	bool bRet=true;
    mysql_autocommit(&m_MySqlHandle, 0);											//撤消自动提交模式
	for(int i=0;i<Count;i++)
	{
		wxString strTmp = SqlArray[i];
		if (strTmp.IsEmpty())
		{
			continue;
		}
		if (strTmp.Last() == wxT(';'))
		{
			strTmp = strTmp.BeforeLast(wxT(';'));
		}
		if ( mysql_real_query(&m_MySqlHandle,strTmp.mb_str(wxConvLocal),strlen(strTmp.mb_str(wxConvLocal))) != 0)
		{
			strErrorSql =  wxT("执行语句错误:") + SqlArray[i];
			strError = wxString(mysql_error(&m_MySqlHandle),wxConvLocal);
			bRet = false;
			break;
		}
		//uRet=uRet+(unsigned)mysql_affected_rows(&myData); 						//查询受影响的行数
	}	 
	
	if(mysql_commit(&m_MySqlHandle))												//如果没有成功,回滚事务
	{
		strError = wxT("数据库批量提交错误:");
		strError += wxString(mysql_error(&m_MySqlHandle),wxConvLocal);
		mysql_rollback(&m_MySqlHandle);
		bRet=false;
	} 
    mysql_autocommit(&m_MySqlHandle, 1);											//恢复自动提交模式
	if (!bRet)	//有错误
	{
		if (strError.Contains(wxT(" is marked as crashed and should be repaired")))
		{
			GetPlug()->AddLog(LOG_TYPE_WARNING,"检测到数据库损坏，启动自动修复");
	
		}		
	}
	return bRet;
}
bool MySqlDatabase::ExecuteNoQuery(wxString Sql,wxString &strError)
{
	if (Sql.Last() == wxT(';'))
	{
		Sql = Sql.BeforeLast(wxT(';'));
	} 	
	if (Sql==wxEmptyString)
	{
		strError = wxT("SQL语句未空:")+Sql;
		return false;
	}
	if ((mysql_more_results(&m_MySqlHandle) || m_MySqlResult!=NULL) && !FreeResult(strError))				//有返回值，需要清空
	{
		return false;
	}

	if( mysql_real_query(&m_MySqlHandle,Sql.mb_str(wxConvLocal),strlen(Sql.mb_str(wxConvLocal))) == 0)
	{
		return true;
	}
	else
	{
		strError =  wxString(mysql_error(&m_MySqlHandle),wxConvLocal);
		if (strError.Contains(wxT(" is marked as crashed and should be repaired")))
		{
			GetPlug()->AddLog(LOG_TYPE_WARNING,"检测到数据库损坏，启动自动修复");
	
		}
		return false;
	}
	
}
int MySqlDatabase::ExecuteQuery(char *Sql)											//执行查询语句
{
	if (Sql==NULL)
	{
		return 0;
	}
	wxString strSql(Sql,wxConvLocal);
	if (strSql.Last() == wxT(';'))
	{
		strSql = strSql.BeforeLast(wxT(';'));
	}
	int iLine  = 0;
	int iField = 0;
	tagRow		*pRow;
	MYSQL_ROW	MySqlRows;
	MYSQL_FIELD *pMySqlFields;
	wxString strError;
	if (mysql_more_results(&m_MySqlHandle) || m_MySqlResult!=NULL)				//有返回值，需要清空
	{
		FreeResult(strError);
	}																//释放前一次的结果集

    mysql_real_query(&m_MySqlHandle,strSql.mb_str(wxConvLocal),strlen(strSql.mb_str(wxConvLocal)));												//查询

	//wxString errs(mysql_error(&m_MySqlHandle),wxConvLocal);
    m_MySqlResult = mysql_store_result(&m_MySqlHandle);								//存储结果集
    if(m_MySqlResult == NULL)
	{
		
		return 0;																	//如果结果集为空，则返回0
	}
    iLine  = (int)mysql_num_rows(m_MySqlResult);									//获得行数
    iField = (int)mysql_num_fields(m_MySqlResult);									//获得字段数
   

   for(int i=0;i<iLine;i++)															//填充结果集
   {	   
      pRow=(tagRow*)malloc(sizeof(tagRow));											//为存储一行数据分配内存	 
	  if (pRow == NULL)
	  {
		  continue;
	  }
	  pRow->FieldCnt = iField;														//一行有多少个字段 
	  MySqlRows      = mysql_fetch_row(m_MySqlResult);								//获得一行数据(获取之后自动跳到下一行)
	  for(int j=0;j<iField;j++)
	  {
		  pMySqlFields = mysql_fetch_field_direct(m_MySqlResult,j);					//获得该列的相关信息	 
		  strcpy_s(pRow->Fields[j].FieldName,sizeof(char)*DEFAULT_FIELD_SIZE,pMySqlFields->name);						//列名称	 
		  pRow->Fields[j].FieldValue  = MySqlRows[j];								//列值	 
		  pRow->Fields[j].ValueLength = pMySqlFields->max_length;					//列宽度
	  }
	  m_VecResult.push_back(pRow);													//将一行数据存储到容器中
   }
   return iLine;																	//返回行数
}

bool MySqlDatabase::FreeResult(wxString &strerror)													//释放查询的结果集
{
	int     len  = 0;
	tagRow* pRow = NULL;

	len = (int)m_VecResult.size();
	for(int i=0;i<len;i++)															//释放结果集的内存
	{
		pRow = m_VecResult[i];
		if(pRow != NULL)
		{
			free(pRow);
			pRow = NULL;
		}
	}
	m_VecResult.clear();	
	vector<tagRow*>().swap(m_VecResult);
	bool bRe = true;
	//由于已经取出结果，需要释放
	if (m_MySqlResult!=NULL)
	{
		mysql_free_result(m_MySqlResult);
	}
	//判断还有释放的没
	int Re = 0;
 	do
 	{
		if (Re > 0)
		{
		/*
		 0
		 成功并有多个结果。
		 -1
		 成功但没有多个结果。
		 >0
		 出错*/
			bRe = false;
			strerror = wxT("mysql_next_result出错,请重启终端");
			break;
		}
 		if ((m_MySqlResult= mysql_store_result( &m_MySqlHandle)))
 		{
 			mysql_free_result(m_MySqlResult);
 		} 
 	} while (!(Re = mysql_next_result( &m_MySqlHandle)));

	return bRe;
}

bool MySqlDatabase::ExecutePrepare(char *Sql)										//执行带参数的SQL语句函数
{
	bool bRet = true;
	m_MySqlStmt=mysql_stmt_init(&m_MySqlHandle);									//初始化句柄
	if (m_MySqlStmt == NULL)
	{
		bRet = false;
		return bRet;
	}	
	if (mysql_stmt_prepare(m_MySqlStmt, Sql, (unsigned long)strlen(Sql)))			//准备SQL
	{
		bRet = false;
		return bRet;
	}
	return bRet;
}
bool MySqlDatabase::BindParam(MYSQL_BIND *Bind)
{
	bool bRet=true;
	if (mysql_stmt_bind_param(m_MySqlStmt,Bind))
	{
       bRet=false;
	}
	return bRet;
}
int MySqlDatabase::Execute()
{
	int iRet=0;
	if (mysql_stmt_execute(m_MySqlStmt))
	{
		iRet=0;
		return iRet;;
	}	
	iRet=(int)mysql_stmt_affected_rows(m_MySqlStmt);								//获得受影响的行数 
    return iRet;
}
bool MySqlDatabase::ClosePrepare()
{
	bool bRet=true;
	if (mysql_stmt_close(m_MySqlStmt))												//关闭句柄
	{
       bRet=false;
	}
	return bRet;
}

bool MySqlDatabase::ChangeUser(char *UserName,char *Password,char *Database)		//改变用户
{
	bool bRet=true;
	if(mysql_change_user(&m_MySqlHandle,UserName,Password,Database) )				//如果失败，返回FALSE
	{
		bRet=false;
	}
	return bRet;
}

bool MySqlDatabase::SelectDB(char *Database)										//选择数据库
{
	bool bRet=true;
	if(mysql_select_db(&m_MySqlHandle, Database))
	{
		bRet=false;
	}
	return bRet;
}

void MySqlDatabase::CloseDatabase()
{
	wxString strError;
    FreeResult(strError);						  //清空结果集
	mysql_close(&m_MySqlHandle);		  //关闭连接
}

int MySqlDatabase::GetIntValue(int RowIndex, char *FieldName)						//获取查询的值,第二个参数为列名
{
	tagRow *pRow  = NULL;
	int    len    = 0;
	int    iRet   = 0;
	char   *pTemp = NULL;

	len  =  (int)m_VecResult.size();
	if(RowIndex >= len)
	{
		return 0;																	//如果行号超过了范围，则返回0
	}
	pRow = (tagRow*)m_VecResult[RowIndex];
	for(int i=0;i<pRow->FieldCnt;i++)												//查找列
	{
		if(strcmp(pRow->Fields[i].FieldName,FieldName) == 0)
		{		
			pTemp = (char *)pRow->Fields[i].FieldValue;								//获得数据的字符串表示		
			if (pTemp!=NULL)
			{
				iRet  = atoi(pTemp);													//转换成数字
			}
			else
			{
				iRet=0;
			}
			
			break;
		}
	}
	return iRet;
}

UINT64 MySqlDatabase::GetIntValue64(int RowIndex, char *FieldName)
{
	tagRow *pRow  = NULL;
	int    len    = 0;
	UINT64    iRet   = 0;
	char   *pTemp = NULL;

	len  =  (int)m_VecResult.size();
	if(RowIndex >= len)
	{
		return 0;																	//如果行号超过了范围，则返回0
	}
	pRow = (tagRow*)m_VecResult[RowIndex];
	for(int i=0;i<pRow->FieldCnt;i++)												//查找列
	{
		if(strcmp(pRow->Fields[i].FieldName,FieldName) == 0)
		{		
			pTemp = (char *)pRow->Fields[i].FieldValue;								//获得数据的字符串表示		
			if (pTemp!=NULL)
			{
				iRet  = _atoi64(pTemp);													//转换成数字
			}
			else
			{
				iRet=0;
			}

			break;
		}
	}
	return iRet;
}

int MySqlDatabase::GetIntValue(int RowIndex, int FieldNum)
{
    tagRow *pRow  = NULL;
	int    len    = 0;
	int    iRet   = 0;
	char   *pTemp = NULL;
	
	len = (int)m_VecResult.size();
	if(RowIndex >= len)
	{
		return 0;																	//如果行号超过了范围，则返回0
	}
	pRow  = m_VecResult[RowIndex];
	pTemp = (char *)pRow->Fields[FieldNum].FieldValue;								//获得数字的字符串表示
	if (pTemp!=NULL)
	{
		iRet  = atoi(pTemp);															//转换成数字
	}
	return iRet;
}


float MySqlDatabase::GetFloatValue(int RowIndex,char *FieldName)					//获得符点型值,第二个参数为列名
{
	tagRow   *pRow  = NULL;
	int      len    = 0;
	char     *pTemp = NULL;
	float    fRet   = 0.0f;

	len = (int)m_VecResult.size();	
	if(RowIndex >= len)
	{
		return 0;														//如果行号超过了范围，则返回0
	}
	pRow = m_VecResult[RowIndex];

	for(int i=0;i<pRow->FieldCnt;i++)									//查找列
	{
		if( strcmp(pRow->Fields[i].FieldName,FieldName) == 0)
		{
			pTemp = (char*)pRow->Fields[i].FieldValue;
			if (pTemp!=NULL)
			{
				fRet  = (float)atof(pTemp);
			}
			else
			{
				fRet=0;
			}
			
			break;
		}
	}
	return fRet;
}
float MySqlDatabase::GetFloatValue(int RowIndex,int FieldNum)
{
	tagRow *pRow  = NULL;
	int    len    = 0;
	float  iRet   = 0.0f;
	char   *pTemp = NULL;

	len = (int)m_VecResult.size();			
	if(RowIndex >= len)
	{
		return 0;														//如果行号超过了范围，则返回0
	}
	pRow  = m_VecResult[RowIndex];
    pTemp = (char*)pRow->Fields[FieldNum].FieldValue;
	if (pTemp == NULL)
	{
		return 0;
	}
    iRet  = (float)atof(pTemp);
	return iRet;
}
double MySqlDatabase::GetDoubleValue(int RowIndex,char *FieldName)		//获得高精度符点型值
{
	tagRow *pRow  = NULL;
	int    len    = 0;
	char   *pTemp = NULL;
	double iRet   = 0.0;

	len = (int)m_VecResult.size();
	if(RowIndex >= len)
	{
		return 0;														//如果行号超过了范围，则返回0
	}
	pRow = m_VecResult[RowIndex];
	
	for(int i=0;i<pRow->FieldCnt;i++)									//查找列
	{
		if(strcmp(pRow->Fields[i].FieldName,FieldName) == 0)
		{
			pTemp = (char *)pRow->Fields[i].FieldValue;
			if (pTemp!=NULL)
			{
				iRet  = atof(pTemp);
			}
		    else
			{
				iRet=0;
			}
			break;
		}
	}
	return iRet;
}
double MySqlDatabase::GetDoubleValue(int RowIndex,int FieldNum)
{
	tagRow  *pRow  = NULL;
	int     len    = 0;
	char    *pTemp = NULL;
	double  dRet   = 0.0;

	len = (int)m_VecResult.size();
	if(RowIndex >= len)
	{
		return 0;														//如果行号超过了范围，则返回0
	}
	pRow = m_VecResult[RowIndex];										

	pTemp = (char *)pRow->Fields[FieldNum].FieldValue;
	if (pTemp == NULL)
	{
		return 0;
	}
	dRet  = atof(pTemp);
	return dRet;
}
char *MySqlDatabase::GetStringValue(int RowIndex,char *FieldName,int &Length)
{
	tagRow *pRow  = NULL;
	int    len    = 0;
	char   *pTemp = NULL;

	len = (int)m_VecResult.size();
	
	if(RowIndex >= len)
	{
		return NULL;													//如果行号超过了范围，则返回NULL
	}
	pRow = m_VecResult[RowIndex];

	for(int i=0;i<pRow->FieldCnt;i++)									//查找列
	{
		if(strcmp(pRow->Fields[i].FieldName,FieldName) == 0)
		{
			Length = pRow->Fields[i].ValueLength;
			pTemp  = (char *)(pRow->Fields[i].FieldValue);
			break;
		}
	}
	return pTemp;
}
char *MySqlDatabase::GetStringValue(int RowIndex,int FieldNum, int &Length)
{
	tagRow   *pRow  = NULL;
	int      len    = 0;
	char     *pTemp = NULL;

	len = (int)m_VecResult.size();

	if(RowIndex >= len)
	{
		return NULL;													//如果行号超过了范围，则返回NULL
	}
	pRow   = m_VecResult[RowIndex];
	Length = pRow->Fields[FieldNum].ValueLength;						//获得列长度
	pTemp  = (char *)(pRow->Fields[FieldNum].FieldValue);				//直接返回此列的值
	return pTemp;
}

char *MySqlDatabase::GetDataTimeValue(int RowIndex,char *FieldName)		//获得日期时间型数据
{
	tagRow* pRow = NULL;
	int     len  = 0;
	char * pTime = NULL;
	
	len = (int)m_VecResult.size();

	if(RowIndex >= len)
	{
		return NULL;													//如果行号超过了范围，则返回NULL
	}
	pRow = m_VecResult[RowIndex];

	for(int i=0;i<pRow->FieldCnt;i++)									//查找列
	{
		if(strcmp(pRow->Fields[i].FieldName,FieldName) ==0 )
		{
			pTime = (char*)(pRow->Fields[i].FieldValue);
			break;
		}
	}
	return pTime;
}
char *MySqlDatabase::GetDataTimeValue(int RowIndex, int FieldNum)
{
   	tagRow *pRow = NULL;
	int    len   = 0;

	len = (int)m_VecResult.size();
	if(RowIndex >= len)
	{
		return NULL;	//如果行号超过了范围，则返回0
	}
	pRow = m_VecResult[RowIndex];
	return (char*)(pRow->Fields[FieldNum].FieldValue);//直接返回此列的值
}

int MySqlDatabase::GetBinaryValue(int RowIndex,char *FieldName,char *pBuf)		//获得二进制数据的缓冲区
{
	tagRow *pRow  = NULL;
	int    len    = 0;
	char   *pData = NULL;

	len = (int)m_VecResult.size();
	
	if(RowIndex >= len)
	{
		return 0;														//如果行号超过了范围，则返回0
	}
	pRow = m_VecResult[RowIndex];

	for(int i=0;i<pRow->FieldCnt;i++)									//查找列
	{
		if(strcmp(pRow->Fields[i].FieldName,FieldName) == 0)
		{
		    len = (int)pRow->Fields[i].ValueLength;
			if(pBuf == NULL)
			{
				break;
			}
			pData = (char*)(pRow->Fields[i].FieldValue);
			memcpy(pBuf,pData,len);
			break;
		}
	}
	return len;
}
int MySqlDatabase::GetBinaryValue(int RowIndex,int FieldNum,char * pBuf)
{
	tagRow *pRow  = NULL;
	int    len    = 0;
	char   *pData = NULL;

	len = (int)m_VecResult.size();
	if(RowIndex >= len)
	{
		return 0;													//如果行号超过了范围，则返回0
	}
	pRow = m_VecResult[RowIndex];
	len  = pRow->Fields[FieldNum].ValueLength;

	if(pBuf==NULL)
	{
		return len;														//直接返回此列的值	 
	}
	pData = (char*)(pRow->Fields[FieldNum].FieldValue);
	memcpy(pBuf,pData,len);
	return len;
}
//bool MySqlDatabase::GetBoolValue(int row,char * fd_name)				//获得布尔型值
//{
//   	tagRow* rows=NULL;
//	int len=0;
//	char * tmp=NULL;
//	bool bRet = false;
//	len = m_VecResult.size();
//	
//	if(row>=len)
//		return false;											//如果行号超过了范围，则返回0
//	rows=m_VecResult[row];
//	
//	for(int i=0;i<rows->FieldCnt;i++)						//查找列
//	{
//		if(!strcmp(rows->Fields[i].FieldName,fd_name))
//		{
//			tmp=(char *)rows->Fields[i].FieldValue;
//			bRet=(bool)atoi(tmp);
//			break;
//		}
//	}
//	return bRet;
//}
//bool MySqlDatabase::GetBoolValue(int row,int fd_num)
//{
//	tagRow* rows=NULL;
//	int len=0;
//	bool bRet=0;
//	char * tmp=NULL;
//	len=m_VecResult.size();
//	if(row>=len)
//		return 0;	//如果行号超过了范围，则返回0
//	rows = m_VecResult[row];
//	tmp=(char *)rows->Fields[fd_num].FieldValue;
//	bRet=(bool)atoi(tmp);
//	return bRet;
//}

