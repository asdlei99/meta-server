#ifndef __META_RPC_H_
#define __META_RPC_H_

#include "cell_msg.h"

#define RPC_SUCC				0		/*rpc�ɹ�*/
#define RPC_TIMEOUT				1		/*rpc���糬ʱ*/
#define RPC_NO_META				2		/*rpcΪ���ҵ���Ӧ��meta��Ϣ*/
#define RPC_DB_FAILED			3		/*metaserver�������ݿ�ʧ��*/
#define RPC_METASERVER_ERROR	4		/*metaserver�ڲ�����*/
#define RPC_SOCKET_ERROR		5		/*rpc socket����metaserverʱ����*/	
#define RPC_PACKET_ERROR		6		/*metaserver��Ӧ��rpc��������*/
#define RPC_PARAM_ERROR			7		/*rpc����Ĳ�������*/
#define RPC_FILESPACE_LIMITED	8		/*�ϴ��߿ռ��Ѿ��ﵽ���ޣ��޷��ϴ�������ļ�*/
#define RPC_DISABLE_USER		9		/*�ϴ��߱�������������������ϴ��ļ�*/
#define RPC_PWD_ERROR			10		/*��ȡ�����*/
#define RPC_NOT_OWNER			11		/*���ϴ��߽�ֹ����*/



/*meta rpc�ĳ�ʼ�������٣�һ���ڳ���Ŀ�ʼ�ͽ�������*/
void			rpc_init();
void			rpc_destroy();

/*****************************************************************
�����ļ����°汾�Ż�ȡ�ļ�
*ip				Ҫ���ʵ�meta server ip
*port			Ҫ���ʵ�meta server port
*path			�ļ���url���Զ���·���ļ���,����HTTP�����ݻ�ȡ��
*version		�ļ��汾��0��ʾȡ���µģ�������ʾ��ȡ��Ӧ�汾���ļ�
*timeout		rpc ���õĳ�ʱʱ�䣬����Ϊ0��Ĭ��Ϊ3�룬��λ�Ǻ���
*url			���صĶ�Ӧ�ļ���fdfs��URL,��RPC�ڲ�������ڴ棬���rpcʧ�ܣ�������NULL�������ͷ�
				��ʹ������Ҫfree(url);
*����ֵ			0��ʾ�ɹ���1��ʾ��ʱ��2��ʾmeta���ݲ�����
******************************************************************/
int				rpc_get_meta_by_version(const char* ip, int port, const char* path, int version, int timeout, char** url);

typedef struct auth_out_info_t
{
	char		url[MAX_FILE_NAME];				/*fdfs���ļ����ʵ�url*/
	char		encrypt_code[CRPYT_CODE_SIZE];	/*�ӽ���key,���Ϊ""������ʾ����ӽ���*/

}auth_out_info_t;

/*****************************************************************
����Ȩ�޷��ʷ�ʽ��ȡ�ļ���meta��Ϣ
*ip				Ҫ���ʵ�meta server ip
*port			Ҫ���ʵ�meta server port
*path			�ļ���url���Զ���·���ļ���,����HTTP�����ݻ�ȡ��
*auth_level		Ȩ���жϵķ�ʽ��1-���������룬2-ֻ���ϴ����Լ��ܷ��ʣ�
				3-��ȡ����ʣ�4-�ļ���Ҫ���ܷ��ʣ�5-����������֤����
*timeout		rpc ���õĳ�ʱʱ�䣬����Ϊ0��Ĭ��Ϊ3�룬��λ�Ǻ���
*res			���ص��ļ�url�ͽ��ܴ��ṹ�����������ⲿnginxģ���ȡ���ļ�֮�����
*����ֵ			0��ʾ�ɹ���1��ʾ��ʱ��2��ʾmeta���ݲ�����
******************************************************************/
int				rpc_get_meta_by_auth(const char* ip, int port, const char* path, int auth_level, const char* user, 
						const char* pwd, const char* cookie, auth_out_info_t* res, int timeout);

/*****************************************************************
����Ȩ�޷��ʷ�ʽ��ȡ�ļ���meta��Ϣ
*ip				Ҫ���ʵ�meta server ip
*port			Ҫ���ʵ�meta server port
*path			�Զ�����ļ�·��
*url			���صĶ�Ӧ�ļ���fdfs��URL,��RPC�ڲ�������ڴ�,
				���صĶ�Ӧ�ļ���fdfs��URL,��RPC�ڲ�������ڴ棬���rpcʧ�ܣ�������NULL�������ͷ�
*����ֵ			0��ʾ�ɹ���1��ʾ��ʱ��2��ʾmeta���ݲ�����
******************************************************************/
int				rpc_get_meta_by_path(const char* ip, int port, const char* path, char** url, int timeout);
/****************************************************************
����һ���ļ���Ԫ��Ϣ
*info			Ҫ���õ��ļ�Ԫ��Ϣ
*timeout		rpc���ó�ʱʱ��
*����ֵ			0��ʾ�ɹ���������ʾʧ��
****************************************************************/
int				rpc_add_meta(const char* ip, int port, meta_info_t* info, const char* pool, int timeout);

/***************************************************************
�޸�һ���ļ���Ԫ��Ϣ(��ȡ�롢�ӽ���KEY���Ƿ���⿪�ŵ�Ԫ��Ϣ)
*path			�ļ���url���Զ���·���ļ���
*pool			������pool����������д""
*opened			0��ʾ�����ֹ���ʣ�������ʾ�����Ʒ���, -1��ʾ�����ô��
*pwd			���ַ�����ʾ�����룬������ʾ������(һ���ǿ�������ַ���)��
				NULL��ʾ�����ô���
*crpyt_key		�ļ����ܵ�key��NULL��ʾ�����ô���
*timeout		rpc���ó�ʱʱ��
*����ֵ			0��ʾ�ɹ���������ʾʧ��
***************************************************************/
int				rpc_update_meta(const char* ip, int port, const char* path, const char* pool, int opened, const char* pwd,
								unsigned char* crpyt_key, size_t crpyt_size, int timeout);

/*****************************************************************
�滻һ��ӵ��meta��Ϣ���ļ����������ļ��汾����ԭ���ļ���meta��Ϣ,
���û�о��ļ���meta�������һ��meta��Ϣ���൱��Ĭ�ϵ�rpc_add_meta
*path			�ļ���url���Զ���·���ļ���
*pool			������pool����������д""
*url			�µ������滻���ļ���fdfs����url
*user			�������û���
*version		�ļ��汾�ţ����ڱ�����ļ���meta��Ϣ,���version <= 0,��ʾ��������ļ���meta
*timeout		rpc���ó�ʱʱ��
*����ֵ			0��ʾ�ɹ���������ʾʧ��
***************************************************************/
int				rpc_replace_meta(const char* ip, int port, const char* path, const char* pool, const char* url, 
								const char* user, int version, int timeout);

/***************************************************************
ɾ��һ���ļ���Ԫ��Ϣ
*path			�ļ���url���Զ���·���ļ���
*timeout		rpc���ó�ʱʱ��
*����ֵ			0��ʾ�ɹ���������ʾʧ��
***************************************************************/
int				rpc_delete_meta(const char* ip, int port, const char* path, const char* pool, int timeout);

/***************************************************************
����һ���ļ�����������
*url			fdfs��Ӧ���ļ�url
*day			�洢��fdfs�ϵ�����ʱ�䣬����Ϊ��λ
*cold			0��ʾ���ڹ���ɾ����1��ʾ���ڹ���תΪ�����ݴ洢
*timeout		rpc���ó�ʱʱ��
*����ֵ			0��ʾ�ɹ���������ʾʧ��
***************************************************************/
int				rpc_add_life_cycle(const char* ip, int port, const char* url, int day, int cold, int timeout);

/***************************************************************
�ж��û��Ƿ�����ϴ��ļ�
*user			�ϴ��û���
*exname			�ϴ��ļ�����չ��
*file_size		�ϴ��ļ��Ĵ�С			
*timeout		rpc���ó�ʱʱ��
*����ֵ			0��ʾ�����ϴ���2��ʾû�ж���ռ��ϴ���3��ʾuser��
				���������������ʾ�������ϴ�
***************************************************************/
int				rpc_is_upload_file(const char* ip, int port, const char* user, const char* exname, int file_size, int timeout);

/***************************************************************
upload �ϴ��ļ��ɹ��󣬱����metaserver�����û��ռ���Ϣ����
*user			�ϴ��û���
*file_size		�ϴ��ļ��Ĵ�С			
*timeout		rpc���ó�ʱʱ��
*����ֵ			0-�����ϴ��ļ���8-�ռ����ƣ�9-�û��ϴ��ļ�Ȩ�ޱ����ƣ�1 ~ 7����ͨRPC����
***************************************************************/
int				rpc_report_upload_file(const char* ip, int port, const char* user, int file_size, int timeout);

/***************************************************************
�ļ�������ӡ�ɾ�����޸ġ�Ԫ���ݲ����������meta�ύ������־
*user			�������û���
*pool			Ӧ����			
*url			fdfs�ļ���url
*type			�������ͣ�0��ʾ�ϴ�һ�����ļ���1��ʾɾ��һ��fdfs�ļ���2��ʾ�������ļ�Ԫ����
*ip				������ip��ַ
*timeout		rpc���ó�ʱʱ��
*����ֵ			0
***************************************************************/
int				rpc_add_file_log(const char* ip, int port, const char* user, const char* pool, const char* url, int type, const char* op_ip, int timeout);

/***************************************************************
�����û��ϴ�
*user			�ϴ��û���
*timeout		rpc���ó�ʱʱ��
*����ֵ			0-�ɹ�
***************************************************************/
int				rpc_enable_user(const char* ip, int port, const char* user, int timeout);

/***************************************************************
��ֹ�û��ϴ�
*user			�ϴ��û���
*timeout		rpc���ó�ʱʱ��
*����ֵ			0-�ɹ�
***************************************************************/
int				rpc_disable_user(const char* ip, int port, const char* user, int timeout);

#endif





