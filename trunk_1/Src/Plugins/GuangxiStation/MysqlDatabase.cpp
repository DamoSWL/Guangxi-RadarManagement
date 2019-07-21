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
bool MySqlDatabase::Connect(char *Host,char *UserName,char *Password,char *Database,unsigned int Port)		//����MySql������
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
	mysql_options(&m_MySqlHandle, MYSQL_OPT_CONNECT_TIMEOUT, (char*)&TimeOut);		//�������ӳ�ʱ

	my_bool reconnect=true;
	mysql_options(&m_MySqlHandle, MYSQL_OPT_RECONNECT, (char *)&reconnect);			//����������

	m_Port = Port;

	if(mysql_real_connect(&m_MySqlHandle,Host,UserName,Password,Database,Port,NULL,CLIENT_MULTI_STATEMENTS)==NULL)				//���ӷ�����
	{
		return false;
	}
	
	if(mysql_set_character_set(&m_MySqlHandle, "gb2312"))							//���������ַ�����Ĭ��Ϊgb2312
	{										
		CloseDatabase();																	//��������ַ������ɹ�����ر�����	
		return false;
	}
	mysql_set_server_option(&m_MySqlHandle, MYSQL_OPTION_MULTI_STATEMENTS_ON);		//������������֧��		
	return true;
}
bool MySqlDatabase::ExecuteNoQuery(wxArrayString SqlArray,int Count,wxString &strErrorSql,wxString &strError)						//ִ�зǲ�ѯ���
{
	if ((INT)SqlArray.GetCount() < Count || Count==0)
	{
		strError = wxT("SQL�������δ��");
		return false;
	}
	if ((mysql_more_results(&m_MySqlHandle) || m_MySqlResult!=NULL) && !FreeResult(strError))				//�з���ֵ����Ҫ���
	{
		return false;
	}
	bool bRet=true;
    mysql_autocommit(&m_MySqlHandle, 0);											//�����Զ��ύģʽ
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
			strErrorSql =  wxT("ִ��������:") + SqlArray[i];
			strError = wxString(mysql_error(&m_MySqlHandle),wxConvLocal);
			bRet = false;
			break;
		}
		//uRet=uRet+(unsigned)mysql_affected_rows(&myData); 						//��ѯ��Ӱ�������
	}	 
	
	if(mysql_commit(&m_MySqlHandle))												//���û�гɹ�,�ع�����
	{
		strError = wxT("���ݿ������ύ����:");
		strError += wxString(mysql_error(&m_MySqlHandle),wxConvLocal);
		mysql_rollback(&m_MySqlHandle);
		bRet=false;
	} 
    mysql_autocommit(&m_MySqlHandle, 1);											//�ָ��Զ��ύģʽ
	if (!bRet)	//�д���
	{
		if (strError.Contains(wxT(" is marked as crashed and should be repaired")))
		{
			GetPlug()->AddLog(LOG_TYPE_WARNING,"��⵽���ݿ��𻵣������Զ��޸�");
	
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
		strError = wxT("SQL���δ��:")+Sql;
		return false;
	}
	if ((mysql_more_results(&m_MySqlHandle) || m_MySqlResult!=NULL) && !FreeResult(strError))				//�з���ֵ����Ҫ���
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
			GetPlug()->AddLog(LOG_TYPE_WARNING,"��⵽���ݿ��𻵣������Զ��޸�");
	
		}
		return false;
	}
	
}
int MySqlDatabase::ExecuteQuery(char *Sql)											//ִ�в�ѯ���
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
	if (mysql_more_results(&m_MySqlHandle) || m_MySqlResult!=NULL)				//�з���ֵ����Ҫ���
	{
		FreeResult(strError);
	}																//�ͷ�ǰһ�εĽ����

    mysql_real_query(&m_MySqlHandle,strSql.mb_str(wxConvLocal),strlen(strSql.mb_str(wxConvLocal)));												//��ѯ

	//wxString errs(mysql_error(&m_MySqlHandle),wxConvLocal);
    m_MySqlResult = mysql_store_result(&m_MySqlHandle);								//�洢�����
    if(m_MySqlResult == NULL)
	{
		
		return 0;																	//��������Ϊ�գ��򷵻�0
	}
    iLine  = (int)mysql_num_rows(m_MySqlResult);									//�������
    iField = (int)mysql_num_fields(m_MySqlResult);									//����ֶ���
   

   for(int i=0;i<iLine;i++)															//�������
   {	   
      pRow=(tagRow*)malloc(sizeof(tagRow));											//Ϊ�洢һ�����ݷ����ڴ�	 
	  if (pRow == NULL)
	  {
		  continue;
	  }
	  pRow->FieldCnt = iField;														//һ���ж��ٸ��ֶ� 
	  MySqlRows      = mysql_fetch_row(m_MySqlResult);								//���һ������(��ȡ֮���Զ�������һ��)
	  for(int j=0;j<iField;j++)
	  {
		  pMySqlFields = mysql_fetch_field_direct(m_MySqlResult,j);					//��ø��е������Ϣ	 
		  strcpy_s(pRow->Fields[j].FieldName,sizeof(char)*DEFAULT_FIELD_SIZE,pMySqlFields->name);						//������	 
		  pRow->Fields[j].FieldValue  = MySqlRows[j];								//��ֵ	 
		  pRow->Fields[j].ValueLength = pMySqlFields->max_length;					//�п��
	  }
	  m_VecResult.push_back(pRow);													//��һ�����ݴ洢��������
   }
   return iLine;																	//��������
}

bool MySqlDatabase::FreeResult(wxString &strerror)													//�ͷŲ�ѯ�Ľ����
{
	int     len  = 0;
	tagRow* pRow = NULL;

	len = (int)m_VecResult.size();
	for(int i=0;i<len;i++)															//�ͷŽ�������ڴ�
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
	//�����Ѿ�ȡ���������Ҫ�ͷ�
	if (m_MySqlResult!=NULL)
	{
		mysql_free_result(m_MySqlResult);
	}
	//�жϻ����ͷŵ�û
	int Re = 0;
 	do
 	{
		if (Re > 0)
		{
		/*
		 0
		 �ɹ����ж�������
		 -1
		 �ɹ���û�ж�������
		 >0
		 ����*/
			bRe = false;
			strerror = wxT("mysql_next_result����,�������ն�");
			break;
		}
 		if ((m_MySqlResult= mysql_store_result( &m_MySqlHandle)))
 		{
 			mysql_free_result(m_MySqlResult);
 		} 
 	} while (!(Re = mysql_next_result( &m_MySqlHandle)));

	return bRe;
}

bool MySqlDatabase::ExecutePrepare(char *Sql)										//ִ�д�������SQL��亯��
{
	bool bRet = true;
	m_MySqlStmt=mysql_stmt_init(&m_MySqlHandle);									//��ʼ�����
	if (m_MySqlStmt == NULL)
	{
		bRet = false;
		return bRet;
	}	
	if (mysql_stmt_prepare(m_MySqlStmt, Sql, (unsigned long)strlen(Sql)))			//׼��SQL
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
	iRet=(int)mysql_stmt_affected_rows(m_MySqlStmt);								//�����Ӱ������� 
    return iRet;
}
bool MySqlDatabase::ClosePrepare()
{
	bool bRet=true;
	if (mysql_stmt_close(m_MySqlStmt))												//�رվ��
	{
       bRet=false;
	}
	return bRet;
}

bool MySqlDatabase::ChangeUser(char *UserName,char *Password,char *Database)		//�ı��û�
{
	bool bRet=true;
	if(mysql_change_user(&m_MySqlHandle,UserName,Password,Database) )				//���ʧ�ܣ�����FALSE
	{
		bRet=false;
	}
	return bRet;
}

bool MySqlDatabase::SelectDB(char *Database)										//ѡ�����ݿ�
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
    FreeResult(strError);						  //��ս����
	mysql_close(&m_MySqlHandle);		  //�ر�����
}

int MySqlDatabase::GetIntValue(int RowIndex, char *FieldName)						//��ȡ��ѯ��ֵ,�ڶ�������Ϊ����
{
	tagRow *pRow  = NULL;
	int    len    = 0;
	int    iRet   = 0;
	char   *pTemp = NULL;

	len  =  (int)m_VecResult.size();
	if(RowIndex >= len)
	{
		return 0;																	//����кų����˷�Χ���򷵻�0
	}
	pRow = (tagRow*)m_VecResult[RowIndex];
	for(int i=0;i<pRow->FieldCnt;i++)												//������
	{
		if(strcmp(pRow->Fields[i].FieldName,FieldName) == 0)
		{		
			pTemp = (char *)pRow->Fields[i].FieldValue;								//������ݵ��ַ�����ʾ		
			if (pTemp!=NULL)
			{
				iRet  = atoi(pTemp);													//ת��������
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
		return 0;																	//����кų����˷�Χ���򷵻�0
	}
	pRow = (tagRow*)m_VecResult[RowIndex];
	for(int i=0;i<pRow->FieldCnt;i++)												//������
	{
		if(strcmp(pRow->Fields[i].FieldName,FieldName) == 0)
		{		
			pTemp = (char *)pRow->Fields[i].FieldValue;								//������ݵ��ַ�����ʾ		
			if (pTemp!=NULL)
			{
				iRet  = _atoi64(pTemp);													//ת��������
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
		return 0;																	//����кų����˷�Χ���򷵻�0
	}
	pRow  = m_VecResult[RowIndex];
	pTemp = (char *)pRow->Fields[FieldNum].FieldValue;								//������ֵ��ַ�����ʾ
	if (pTemp!=NULL)
	{
		iRet  = atoi(pTemp);															//ת��������
	}
	return iRet;
}


float MySqlDatabase::GetFloatValue(int RowIndex,char *FieldName)					//��÷�����ֵ,�ڶ�������Ϊ����
{
	tagRow   *pRow  = NULL;
	int      len    = 0;
	char     *pTemp = NULL;
	float    fRet   = 0.0f;

	len = (int)m_VecResult.size();	
	if(RowIndex >= len)
	{
		return 0;														//����кų����˷�Χ���򷵻�0
	}
	pRow = m_VecResult[RowIndex];

	for(int i=0;i<pRow->FieldCnt;i++)									//������
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
		return 0;														//����кų����˷�Χ���򷵻�0
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
double MySqlDatabase::GetDoubleValue(int RowIndex,char *FieldName)		//��ø߾��ȷ�����ֵ
{
	tagRow *pRow  = NULL;
	int    len    = 0;
	char   *pTemp = NULL;
	double iRet   = 0.0;

	len = (int)m_VecResult.size();
	if(RowIndex >= len)
	{
		return 0;														//����кų����˷�Χ���򷵻�0
	}
	pRow = m_VecResult[RowIndex];
	
	for(int i=0;i<pRow->FieldCnt;i++)									//������
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
		return 0;														//����кų����˷�Χ���򷵻�0
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
		return NULL;													//����кų����˷�Χ���򷵻�NULL
	}
	pRow = m_VecResult[RowIndex];

	for(int i=0;i<pRow->FieldCnt;i++)									//������
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
		return NULL;													//����кų����˷�Χ���򷵻�NULL
	}
	pRow   = m_VecResult[RowIndex];
	Length = pRow->Fields[FieldNum].ValueLength;						//����г���
	pTemp  = (char *)(pRow->Fields[FieldNum].FieldValue);				//ֱ�ӷ��ش��е�ֵ
	return pTemp;
}

char *MySqlDatabase::GetDataTimeValue(int RowIndex,char *FieldName)		//�������ʱ��������
{
	tagRow* pRow = NULL;
	int     len  = 0;
	char * pTime = NULL;
	
	len = (int)m_VecResult.size();

	if(RowIndex >= len)
	{
		return NULL;													//����кų����˷�Χ���򷵻�NULL
	}
	pRow = m_VecResult[RowIndex];

	for(int i=0;i<pRow->FieldCnt;i++)									//������
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
		return NULL;	//����кų����˷�Χ���򷵻�0
	}
	pRow = m_VecResult[RowIndex];
	return (char*)(pRow->Fields[FieldNum].FieldValue);//ֱ�ӷ��ش��е�ֵ
}

int MySqlDatabase::GetBinaryValue(int RowIndex,char *FieldName,char *pBuf)		//��ö��������ݵĻ�����
{
	tagRow *pRow  = NULL;
	int    len    = 0;
	char   *pData = NULL;

	len = (int)m_VecResult.size();
	
	if(RowIndex >= len)
	{
		return 0;														//����кų����˷�Χ���򷵻�0
	}
	pRow = m_VecResult[RowIndex];

	for(int i=0;i<pRow->FieldCnt;i++)									//������
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
		return 0;													//����кų����˷�Χ���򷵻�0
	}
	pRow = m_VecResult[RowIndex];
	len  = pRow->Fields[FieldNum].ValueLength;

	if(pBuf==NULL)
	{
		return len;														//ֱ�ӷ��ش��е�ֵ	 
	}
	pData = (char*)(pRow->Fields[FieldNum].FieldValue);
	memcpy(pBuf,pData,len);
	return len;
}
//bool MySqlDatabase::GetBoolValue(int row,char * fd_name)				//��ò�����ֵ
//{
//   	tagRow* rows=NULL;
//	int len=0;
//	char * tmp=NULL;
//	bool bRet = false;
//	len = m_VecResult.size();
//	
//	if(row>=len)
//		return false;											//����кų����˷�Χ���򷵻�0
//	rows=m_VecResult[row];
//	
//	for(int i=0;i<rows->FieldCnt;i++)						//������
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
//		return 0;	//����кų����˷�Χ���򷵻�0
//	rows = m_VecResult[row];
//	tmp=(char *)rows->Fields[fd_num].FieldValue;
//	bRet=(bool)atoi(tmp);
//	return bRet;
//}

