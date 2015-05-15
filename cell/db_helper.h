#ifndef __DB_HELPER_H_
#define __DB_HELPER_H_

#include <mysql.h>
#include <stdint.h>
#include "cell_msg.h"

#define HOST_SIZE		64
#define USER_SIZE		64
#define DB_NAME_SIZE	64

#define FREE_NONE		0
#define FREE_KEY		0X01
#define FREE_VALUE		0X02
#define FREE_META		0x04

#define MAX_KEY_SIZE	512
#define MAX_VALUE_SIZE	(1024 * 1024)
#define MAX_SQL_SIZE	((MAX_VALUE_SIZE + MAX_KEY_SIZE) * 2 + 1024)

typedef struct meta_kv_s
{
	char*		key;
	size_t		key_size;

	char*		value;
	size_t		value_size;

	uint32_t	free_flag;
}meta_kv_t;

void			init_meta_kv(meta_kv_t* meta);
void			destroy_meta_kv(meta_kv_t* meta);

typedef struct db_helper_s
{
	/*���ݿ�������Ϣ*/
	char		db_host[HOST_SIZE];
	char		user[USER_SIZE];
	char		passwd[USER_SIZE];
	char		db_name[DB_NAME_SIZE];
	uint32_t	port;

	/*״̬��Ϣ*/
	uint32_t	opened;
	int32_t		busy;

	/*һЩ��ʹ�õı���*/
	char*		sql;
	int32_t		retry_count;
	/*��Ӧpool��slot���*/
	int			seq;

	/*mysql����*/
	MYSQL		mysql;
}db_helper_t;

/*�����Ӳ�������helper��,����ʼ��״̬*/
db_helper_t*	create_mysql(const char* host, uint32_t port, const char* user, const char* pwd, const char* db_name, int seq);
/*����helper*/
void			destroy_mysql(db_helper_t* h);
/*���Ӳ���mysql���ݿ�*/
int32_t			open_mysql(db_helper_t* h);
/*�رմ򿪵����ݿ�*/
int32_t			close_mysql(db_helper_t* h);

/*���ݿ�����ӿ�*/
int32_t			insert_kv(db_helper_t* h, meta_kv_t* kv, const char* space, char* err);
int32_t			replace_kv(db_helper_t* h, meta_kv_t* kv, const char* space, char* err);
int32_t			update_kv(db_helper_t* h, meta_kv_t* kv, const char* space, char* err);
int32_t			delete_kv(db_helper_t* h, const char* key, size_t key_size, const char* space, char* err);

int32_t			get_kv(db_helper_t* h, meta_kv_t* kv, const char* space, char* err);
int32_t			scan_kv(db_helper_t* h, const char* start_key, size_t start_key_size, 
						const char* end_key, size_t end_key_size, meta_kv_t** kvs, 
						const char* space, int limit, int skip, char* err);

/*д��־*/
int32_t			insert_dfs_log(db_helper_t* h, add_log_t* info, char* err);

/*�ϴ��߿ռ����*/
int32_t			get_dfs_user_info(db_helper_t* h, const char* user, int64_t* total_size, int64_t* used_size, 
									int64_t* day_size, int64_t* day_used, int* flag, char* err);

int32_t			update_dfs_user_info(db_helper_t* h, const char* user, uint32_t file_size, char* err);
int32_t			update_dfs_user_flag(db_helper_t* h, const char* user, int flag, char* err);

#endif




