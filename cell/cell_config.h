#ifndef __CELL_CONFIG_H_
#define __CELL_CONFIG_H_

#include <stdint.h>

/*�������ݿ�������ĸ���*/
#define DB_SLAVE_N			10

#define PATH_SIZE			1024
#define VALUE_SIZE			64

typedef struct cell_conf_s
{
	int			thread_n;				/*�̸߳���*/
	char		name[VALUE_SIZE];		/*meta server�����ƣ�node1/node2/node3...,���ֲ��ܺ�����meta Server�ظ�*/
	char		listen_ip[VALUE_SIZE];	/*meta server��ip��������*/
	int			listen_port;			/*meta server����ӿ�*/
	int64_t		cache_size;				/*lru cache���ռ�õ��ڴ�ռ�*/
	char		zk_host[PATH_SIZE];		/*zookeeper�ķ��ʵ�ַ*/

	char		user[VALUE_SIZE];		/*���ݿ�����û���*/
	char		passwd[VALUE_SIZE];		/*���ݿ��������*/
	char		db_name[VALUE_SIZE];	/*���������ݿ���*/

	char		db_master[VALUE_SIZE];	/*�����ݿ����IP������*/
	char*		db_slaves[DB_SLAVE_N];	/*�����ݿ����IP�����������*/
	int			db_port;				/*���ݿ���ʶ˿ڣ�*/					

	char		log_path[PATH_SIZE];	/*��־�ļ�·��*/
}cell_conf_t;

extern cell_conf_t*	cell_config;

/*�������ļ��н�cell config���뵽�ڴ���*/
void			load_config(const char* config_file);
/*�ر�cell config�ڴ����*/
void			close_config();

void			print_config();

int				load_zookeeper_host();

#endif




