/*****************************************************************
*����һ��lru-cache��hash table���ձ��ڴ�������tcmalloc��������
*
*****************************************************************/
#ifndef __CELL_LRU_CACHE_H_
#define __CELL_LRU_CACHE_H_

#include <stdint.h>

#define ITEM_KEY(k)				((uint8_t*)((k)->data))
#define ITEM_VALUE(k)			((uint8_t*)((k)->data + (k)->ksize))

typedef struct cache_item_t
{
	struct cache_item_t*	next;			/*lru list�еĺ�һ����Ԫ��ϵ*/
	struct cache_item_t*	prev;			/*lru list�е�ǰһ����Ԫ��ϵ*/
	struct cache_item_t*	hash_next;		/*hash table�еĺ�һ����ϵ*/
	size_t					ksize;			/*keyռ�õĿռ��С*/
	size_t					vsize;
	size_t					size;			/*�����ṹ��ռ�õĿռ���*/
	time_t					time;		
	uint16_t				refcount;		/*Ӧ�ü���*/
	uint16_t				vcount;			/*�����ʴ���*/
	uint32_t				hash;			/*key��hashֵ*/
	uint8_t					data[1];		/*�����壬key + value*/
}cache_item_t;

/*��ʼ��lru cache*/
void				create_cache(size_t max_size);
/*����lru cache*/
void				destroy_cache();
/*ͨ��key����һ��item*/
cache_item_t*		get_cache(const uint8_t* key, size_t ksize);
/*updateҲ�������������������Ѿ����ڻὫ�ϵ�item���ػ�����������release*/
void				insert_cache(const uint8_t* key, size_t ksize, const uint8_t* value, size_t vsize);
/*�����ͷ�item->refcount*/
void				release_cache(cache_item_t* item);
/*��lru_cache��ɾ��һ��key-value,metaserverһ�㲻����������*/
void				erase_cache(const uint8_t* key, size_t ksize);
/*��lru_cache�Ĵ�ӡ*/
void				print_cache();

#endif



