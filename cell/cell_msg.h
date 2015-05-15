#ifndef __CELL_MSG_H_
#define __CELL_MSG_H_

#include <stdio.h>
#include <stdint.h>
#include "cell_codec.h"

/*file path max size*/
#define MAX_FILE_NAME		1024
#define POOL_NAME_SIZE		64
#define USERNAME_SIZE		32
#define CRPYT_CODE_SIZE		128
#define MAX_INFO_SIZE		1536
//10KB
#define STATE_INFO_SIZE		10240 

/*RPC message*/
#define TEST_MSG			0x0000
#define ADD_META			0x0001
#define ADD_META_ACK		0x0002
#define UPDATE_META			0x0003
#define UPDATE_META_ACK		0x0004
#define ADD_LIFE_CYCLE		0x0005
#define ADD_LIFE_CYCLE_ACK	0x0006
#define GET_META			0x0007
#define GET_META_ACK		0x0008
#define DELETE_META			0x0009
#define DELETE_META_ACK		0x000A
#define UPLAOD_FILE			0x000B
#define UPLOAD_FILE_ACK		0x000C
#define ADD_FILE_LOG		0x000D
#define ADD_FILE_LOG_ACK	0x000E
#define CHECK_UPLOAD		0x000F
#define CHECK_UPLOAD_ACK	0x0010
#define SET_USER_FLAG		0x0011
#define SET_USER_FLAG_ACK	0x0012
#define REPLACE_META_VER	0x0013
#define REPLACE_META_VER_ACK 0x0014

/*�鿴metaserver��״̬��Ϣ*/
#define STATE_INFO			0x1000
#define STATE_INFO_ACK		0x1001

/*RPC ERROR VALUE*/
#define NET_SUCC			0
#define ADD_LIFE_FAILED		1
#define NO_DB_HELPER		2
#define INSERT_FAILED		3
#define KEY_NOT_EXIST		4
#define UPDATE_KV_ERROR		5
#define DELETE_KV_ERROR		6

#define KV_VAULE_ERROR		254
#define SERVER_ERROR		255

typedef struct test_msg_body_s
{
	uint16_t	id;
	uint32_t	key;
	char		value[128];
}test_msg_body_t;

typedef struct add_meta_s
{
	uint32_t	sid;						/*������ţ��ͻ���������*/
	char		path[MAX_FILE_NAME];		/*�û��Զ�����ļ�·��*/
	char		url[MAX_FILE_NAME];			/*DFS���ļ����ʵ�URL*/
	char		pool[POOL_NAME_SIZE];		/*Ӧ�÷�pool��*/
	char		uploader[USERNAME_SIZE];	/*�ϴ���*/
	char		pwd[USERNAME_SIZE];			/*�ļ��������룬Ĭ��Ϊ���ַ�����ʾ������*/
	uint32_t	crpyt_key_size;				/*����key����*/
	char		crpyt_key[CRPYT_CODE_SIZE]; /*�ļ����ܵ�key*/
	uint32_t	opened;						/*�Ƿ�Կ��ţ�Ĭ��Ϊ1����ʾ���⿪��*/
}add_meta_t;

typedef struct add_meta_ack_s
{
	uint32_t	sid;
	uint32_t	result;						/*0��ʾ�ɹ��������ı�ʾʧ��*/
	char		err[MAX_INFO_SIZE];			/*������Ϣ����*/
}add_meta_ack_t;

#define SET_PWD			0X01
#define SET_OPEN		0X02
#define SET_CRPYT		0X04

typedef struct update_meta_s
{
	uint32_t	sid;
	char		path[MAX_FILE_NAME];		/*�û��Զ�����ļ�·��*/
	char		pool[POOL_NAME_SIZE];		/*Ӧ�÷�pool��*/
	uint32_t	op;							/*���ò�����Чλ*/
	char		pwd[USERNAME_SIZE];			/*Ҫ���õķ�������*/
	uint32_t	crpyt_key_size;
	char		crpyt_key[CRPYT_CODE_SIZE]; /*�ļ����ܵ�key*/
	uint32_t	opened;						/*�Ƿ���⿪��*/
}update_meta_t;

typedef add_meta_ack_t update_meta_ack_t;

typedef struct add_life_cycle_s
{
	uint32_t	sid;
	char		url[MAX_FILE_NAME];			/*DFS���ļ����ʵ�URL*/
	uint32_t	days;						/*���ļ�ϵͳ����Ч������*/
	int32_t		cold;						/*���ں��Ƿ�תΪ������, 0��ʾֱ��ɾ���� 1��ʾתΪ������*/
}add_life_cycle_t;

typedef add_meta_ack_t add_life_cycle_ack_t;

typedef struct get_meta_s
{
	uint32_t	sid;
	char		path[MAX_FILE_NAME];
	char		pool[POOL_NAME_SIZE];		/*Ӧ�÷�pool��*/
}get_meta_t;

typedef struct get_meta_ack_s
{
	uint32_t	sid;
	uint32_t	result;
	/*�������ļ�����*/
	char		path[MAX_FILE_NAME];		/*�û��Զ�����ļ�·��*/
	char		url[MAX_FILE_NAME];			/*DFS���ļ����ʵ�URL*/
	char		uploader[USERNAME_SIZE];	/*�ϴ���*/
	char		pwd[USERNAME_SIZE];			/*�ļ��������룬Ĭ��Ϊ���ַ�����ʾ������*/
	uint32_t	crpyt_key_size;
	char		crpyt_key[CRPYT_CODE_SIZE]; /*�ļ����ܵ�key*/
	uint32_t	opened;						/*�Ƿ�Կ��ţ�Ĭ��Ϊ1����ʾ���⿪��*/
}get_meta_ack_t;

typedef get_meta_ack_t	meta_info_t;

typedef struct del_meta_s
{
	uint32_t	sid;
	char		path[MAX_FILE_NAME];
	char		pool[POOL_NAME_SIZE];		/*Ӧ�÷�pool��*/
}del_meta_t;

typedef add_meta_ack_t del_meta_ack_t;

typedef struct upload_file_s
{
	uint32_t	sid;
	char		user[USERNAME_SIZE];
	uint32_t	file_size;
}upload_file_t;

typedef add_meta_ack_t upload_file_ack_t;

typedef struct check_upload_s
{
	uint32_t	sid;
	char		user[USERNAME_SIZE];
	char		exname[USERNAME_SIZE];
	uint32_t	file_size;
}check_upload_t;

typedef add_meta_ack_t check_upload_ack_t;

typedef struct user_flag_s
{
	uint32_t	sid;
	char		user[USERNAME_SIZE];
	int			flag; /*0��ʾ�����ϴ���1��ʾ��ֹ�ϴ�*/
}user_flag_t;

typedef add_meta_ack_t user_flag_ack_t;

typedef struct add_log_s
{
	uint32_t	sid;
	char		user[USERNAME_SIZE];
	char		path[MAX_FILE_NAME];
	char		ip[USERNAME_SIZE];
	char		pool[POOL_NAME_SIZE];
	int32_t		type;
}add_log_t;

typedef add_meta_ack_t add_log_ack_t;

typedef struct replace_meta_ver_s
{
	uint32_t	sid;
	char		path[MAX_FILE_NAME];	/*�ļ����ⲿ����·��*/
	char		url[MAX_FILE_NAME];		/*�滻�����ļ�fdfs���ʵ�url*/
	char		user[USERNAME_SIZE];	/*�������û���*/
	char		pool[POOL_NAME_SIZE];	/*Ӧ����*/
	int32_t		version;				/*���滻�ļ��İ汾�ţ���� <=0����ʾ�����汻�滻�ļ���meta��Ϣ*/
}replace_meta_ver_t;

typedef add_meta_ack_t replace_meta_ack_t;


typedef struct server_state_info_s
{
	uint32_t	sid;
}server_state_info_t;

typedef struct server_state_info_ack_s
{
	uint32_t	sid;
	char		info[STATE_INFO_SIZE];
}server_state_info_ack_t;

/********************************************************************/

void		encode_msg(bin_stream_t* strm, uint16_t id, void* ptr);

void		test_msg_encode(bin_stream_t* strm, test_msg_body_t* body);
void		test_msg_decode(bin_stream_t* strm, test_msg_body_t* body);

void		add_meta_encode(bin_stream_t* strm, add_meta_t* meta);
int			add_meta_decode(bin_stream_t* strm, add_meta_t* meta);

void		ack_encode(bin_stream_t* strm, add_meta_ack_t* ack);
int			ack_decode(bin_stream_t* strm, add_meta_ack_t* ack);

void		update_meta_encode(bin_stream_t* strm, update_meta_t* meta);
int			update_meta_decode(bin_stream_t* strm, update_meta_t* meta);

void		add_life_cycle_encode(bin_stream_t* strm, add_life_cycle_t* data);
int			add_life_cycle_decode(bin_stream_t* strm, add_life_cycle_t* data);

void		get_meta_encode(bin_stream_t* strm, get_meta_t* m);
int			get_meta_decode(bin_stream_t* strm, get_meta_t* m);

void		get_meta_ack_encode(bin_stream_t* strm, get_meta_ack_t* ack);
int			get_meta_ack_decode(bin_stream_t* strm, get_meta_ack_t* ack);

void		del_meta_encode(bin_stream_t* strm, del_meta_t* m);
int			del_meta_decode(bin_stream_t* strm, del_meta_t* m);

void		upload_file_encode(bin_stream_t* strm, upload_file_t* m);
int			upload_file_decode(bin_stream_t* strm, upload_file_t* m);

void		check_upload_encode(bin_stream_t* strm, check_upload_t* m);
int			check_upload_decode(bin_stream_t* strm, check_upload_t* m);

void		user_flag_encode(bin_stream_t* strm, user_flag_t* m);
int			user_flag_decode(bin_stream_t* strm, user_flag_t* m);

void		add_log_encode(bin_stream_t* strm, add_log_t* m);
int			add_log_decode(bin_stream_t* strm, add_log_t* m);

void		replace_meta_encode(bin_stream_t* strm, replace_meta_ver_t* m);
int			replace_meta_decode(bin_stream_t* strm, replace_meta_ver_t* m);

void		state_info_encode(bin_stream_t* strm, server_state_info_t* m);
int			state_info_decode(bin_stream_t* strm, server_state_info_t* m);

void		state_info_ack_encode(bin_stream_t* strm, server_state_info_ack_t* m);
int			state_info_ack_decode(bin_stream_t* strm, server_state_info_ack_t* m);


#endif



