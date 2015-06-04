#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "cell_zk.h"
#include "cell_log.h"
#include "cell_config.h"

#define ZK_TIMEOUT 15000

//zk��������ַurl
#define ZK_HOST_SIZE 512

/*zk��ȫ�־��*/
zhandle_t* zk_handle = NULL;

//callback function
void zktest_watcher_g(zhandle_t* zh, int type, int state, const char* path, void* watcherCtx)
{
}

/************************************************************************/
int32_t zk_node_exists(const char* path)
{
	struct Stat stat;
	if(zk_handle == NULL && zk_init(cell_config->zk_host) == NULL){
		return -1;
	}

	return zoo_exists(zk_handle, path, 1, &stat);

}

int32_t zk_create_node(const char* path, const char* value, int tmp)
{
	if(zk_handle == NULL && zk_init(cell_config->zk_host) == NULL){
		return -1;
	}

	if(zk_node_exists(path) != 0){
		return zoo_create(zk_handle, path, value, strlen(value), &ZOO_OPEN_ACL_UNSAFE, (tmp == 1 ? ZOO_EPHEMERAL : 0), 0, 0);
	}

	return 0;
		
}

int32_t zk_delete_node(const char* path)
{
	if(zk_handle == NULL){
		return 0;
	}

	return zoo_delete(zk_handle, path, -1);
}
//���ذ汾��
int32_t zk_get_node(const char* path, char* value, int* len)
{
	struct Stat stat;
	if(zk_handle == NULL && zk_init(cell_config->zk_host) == NULL){
		return -1;
	}

	int rc = zoo_get(zk_handle, path, 1, value, len, &stat);
	if(rc == 0){
		if(*len < ZK_MAX_NODE_SIZE)
			value[*len] = '\0';
		else
			value[ZK_MAX_NODE_SIZE - 1] = '\0';
	}
	else 
		stat.version = -1;

	return stat.version;
}
//ǿ�Ƹ���
int32_t zk_set_node(const char* path, const char* value)
{
	if(zk_handle == NULL){
		return ZCONNECTIONLOSS;
	}

	return zoo_set(zk_handle, path, value, strlen(value), -1);
}

//׷������,һ������־
int32_t zk_append_node(const char* path, const char* value)
{
	char* ctx;
	int len = ZK_MAX_NODE_SIZE;
	int32_t rc = 0;
	struct Stat stat;
	int value_size = strlen(value);

	if(zk_handle == NULL && zk_init(cell_config->zk_host) == NULL){
		return -1;
	}

	ctx = (char *)malloc(ZK_MAX_NODE_SIZE * sizeof(char));
	if(ctx != NULL){
		memset(ctx, 0, sizeof(ZK_MAX_NODE_SIZE));

		//�������ã�ֱ���ɹ�
		do{
			if(zoo_get(zk_handle, path, 1, ctx, &len, &stat) == 0 
				&& len + value_size < ZK_MAX_NODE_SIZE - 1){
					memcpy(ctx + len, value, value_size);
					len += value_size;
					rc = zoo_set(zk_handle, path, ctx, len, stat.version);
			}
			else{
				rc = -1;
				break;
			}
		}while(rc != 0);

		free(ctx);
	}


	return rc;
}

//��ȡ�ڵ�Ŀ¼�����еĺ��ӽڵ�����
int32_t zk_get_all_nodes(const char* path)
{
	char child_path[1024] = {0};
	int len = ZK_MAX_NODE_SIZE;

	int32_t i = 0, rc = -1;
	struct String_vector strings;
	char* ctx;

	if(zk_handle == NULL && zk_init(cell_config->zk_host) == NULL){
		return -1;
	}

	ctx = (char *)malloc(ZK_MAX_NODE_SIZE * sizeof(char));
	if(ctx == NULL){
		return -1;
	}

	rc = zoo_get_children(zk_handle, path, 1, &strings);
	if(rc == 0){
		for(i = 0; i < strings.count; i ++){
			if(strings.data[i] != NULL){
				sprintf(child_path, "%s/%s", path, strings.data[i]);

				if(zk_get_node(child_path, ctx, &len) > 0)
					printf("path = %s, value = %s\n", child_path, ctx);
			}
		}
	}

	//free string
	deallocate_String_vector(&strings);
	free(ctx);

	return rc;
}

zhandle_t* zk_init(const char* zk_host)
{
	if(zk_handle)
		return NULL;

	//const char* host = "storm-supervisor-00:2181,storm-supervisor-01:2181,storm-supervisor-02:2181";	
	zoo_set_debug_level(ZOO_LOG_LEVEL_WARN);
	zk_handle = zookeeper_init(zk_host, zktest_watcher_g, ZK_TIMEOUT, 0, (void *)"metaserver", 0);
	if (zk_handle == NULL) {
		log_fatal("init zookeeper failed, host = %s\n", zk_host);
		zk_handle = NULL;
	}
	else{
		int32_t i = 0;
		//�ȴ�������ɣ��������������ʧ��
		while(zoo_state(zk_handle) != ZOO_CONNECTED_STATE){
			usleep(100);

			//һ��Ҫ��ZOO_CONNECTED_STATE״̬�����ܽ���zk api���ã��ȴ�5��
			i++;
			if(i > 50000){
				zookeeper_close(zk_handle);
				log_fatal("zookeeper connect timeout!");
				zk_handle = NULL;
				break;
			}
		}
	}

	return zk_handle;
}

void zk_destroy()
{
	if(zk_handle != NULL){
		zookeeper_close(zk_handle);
		zk_handle = NULL;
	}
}

zhandle_t* zk_get_handler()
{
	return zk_handle;
}


