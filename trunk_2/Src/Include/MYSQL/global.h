#ifndef __GLOBAL_H__
#define __GLOBAL_H__

struct Data_Param			//���ݿ��������
{
	char *db_name;			//���ݿ���
	char *tab_name;			//����
	char *col_name;			//����
	char *select_exp;		//ѡ����ʽ
	char *where_def;		//ѡ���޶�����
	char *insert_val;		//����ֵ
	char *set_exp;			//�������ݿ����ݵı��ʽ
};

struct Database_Param		//���ݿ����
{
	char *host;					//������
	char *user;					//�û���
	char *password;				//����
	char *db;					//���ݿ���
	unsigned int port;			//�˿ڣ�һ��Ϊ0
	const char *unix_socket;	//�׽��֣�һ��ΪNULL
	unsigned int client_flag;	//һ��Ϊ0
};

#endif
