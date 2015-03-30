#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "cell_lru_cache.h"
#include "cell_lock.h"
#include "hash.h"

/*older��younger��̭��ʱ�����ֵ�����younger�б��ж�������10��Ļ��棬��ô����Ҫ�����̭older*/
#define CEEDOUT_THROLD			864000
/*value���������󳤶�1MB*/
#define MAX_ITEM_VALUE_SIZE		1048576

typedef struct lru_cache_t
{
	/*��������*/
	int				lock;			/*���ڱ���lru_cache�ṹ���е����ݻ���*/
	int*			rec_locks;		/*����*/
	cache_item_t**	table;			/*��table����*/
	uint32_t		hashpower;		/*hashͰ�ĸ���,��lru_cache�У�
									����rehash���̣���Ϊ���Ը������Ļ���ռ佨��һ�����������,rehashҲ��������Ƶĸ��Ӷ�,Ҳ�������ڴ����*/
	size_t			items_number;	/*cache�е�item����*/
	size_t			max_size;		/*����ڴ�ռ���ֽ�����*/
	size_t			curr_size;		/*��ǰcacheռ�õĿռ���*/
	
	/*ͳ�Ƽ���*/
	size_t			get_count;
	size_t			hit_count;
	size_t			put_count;

	/*��̭���ж���*/
	cache_item_t	older;			/*older�Ǹ�˫����������ǰ�������µĲ����item,���ĺ��������ϲ����item*/
	cache_item_t	younger;		/*��olderһ��*/
}lru_cache_t;

static lru_cache_t* lru_cache = NULL;

/*����һ����¼��������*/
#define REC_NUM			65536
#define REC_MASK		65535

#define REC_LOCK(h)		LOCK(&(lru_cache->rec_locks[h]))
#define REC_UNLOCK(h)	UNLOCK(&(lru_cache->rec_locks[h]))

/*���ù̶�hash����,��������һ��itemƽ��Ϊ1024���ֽڳ���, max_sizeһ����2��N�η�������10GB*/
void create_cache(size_t max_size)
{
	lru_cache = (lru_cache_t*)calloc(1, sizeof(lru_cache_t));
	if(lru_cache == NULL){
		printf("alloc lru_cache failed, out of memory !\n");
		exit(-1);
	}

	lru_cache->hashpower = max_size >> 10;
	lru_cache->table = (cache_item_t**)calloc(lru_cache->hashpower, sizeof(cache_item_t*));
	if(lru_cache->table == NULL){
		printf("alloc cache table failed, out of memory !\n");
		exit(-1);
	}

	lru_cache->rec_locks = (int *)calloc(REC_NUM, sizeof(int));
	if(lru_cache->rec_locks == NULL){
		printf("alloc cache rec locks, out of memory !\n");
		exit(-1);
	}

	lru_cache->max_size = max_size;
	lru_cache->curr_size = lru_cache->hashpower * (sizeof(cache_item_t*) + sizeof(int)) + REC_NUM * sizeof(int);

	assert(max_size > lru_cache->curr_size);

	lru_cache->older.next = &(lru_cache->older);
	lru_cache->older.prev = &(lru_cache->older);

	lru_cache->younger.next = &(lru_cache->younger);
	lru_cache->younger.prev = &(lru_cache->younger);
}

void destroy_cache()
{
	cache_item_t* it;
	for(it = lru_cache->older.next; it != &(lru_cache->older); ){
		cache_item_t* next = it->next;
		release_cache(it);
		it = next;
	}

	for(it = lru_cache->younger.next; it != &(lru_cache->younger); ){
		cache_item_t* next = it->next;
		release_cache(it);
		it = next;
	}

	if(lru_cache->table != NULL){
		free(lru_cache->table);
		lru_cache->table = NULL;
	}

	free(lru_cache);
	lru_cache = NULL;
}

static void unref(cache_item_t* it)
{
	it->refcount --;
	if(it->refcount <= 0){
		lru_cache->curr_size -= it->size;
		free(it);
	}
}

void release_cache(cache_item_t* item)
{
	/*������������Ϊ���ֻ�����item�����ݺ�lru_cache������*/
	LOCK(&(lru_cache->lock));

	assert(item->refcount > 0);
	unref(item);

	UNLOCK(&(lru_cache->lock));
}

static void remove_list(cache_item_t* it)
{
	it->prev->next = it->next;
	it->next->prev = it->prev;
}

static void append_older(cache_item_t* it)
{
	it->prev = lru_cache->older.prev;
	it->next = &(lru_cache->older);
	it->prev->next = it;
	lru_cache->older.prev = it;
}

static void append_younger(cache_item_t* it)
{
	it->prev = lru_cache->younger.prev;
	it->next = &(lru_cache->younger);
	it->prev->next = it;
	lru_cache->younger.prev = it;
}

cache_item_t* get_cache(const uint8_t* key, size_t ksize)
{
	cache_item_t* it;
	uint32_t r;

	uint32_t hash = murMurHash(key, ksize) % lru_cache->hashpower;
	r = hash & REC_MASK;

	REC_LOCK(r);

	it = lru_cache->table[hash];
	while(it != NULL){
		if(ksize == it->ksize && memcmp(ITEM_KEY(it), key, ksize) == 0)
			break;

		it = it->hash_next;
	}

	REC_UNLOCK(r);

	SYNC_ADD(&(lru_cache->get_count));

	if(it != NULL){
		SYNC_ADD(&(lru_cache->hit_count));

		LOCK(&(lru_cache->lock));
		remove_list(it);
		it->refcount ++;
		it->vcount ++;
		/*�ڻ�����item��������3�λ���3�����ϣ��ͻ����older�б�*/
		if(it->vcount >= 3)
			append_older(it);
		else
			append_younger(it);

		UNLOCK(&(lru_cache->lock));
	}

	return it;
}

static cache_item_t** find_item_pointer(uint32_t hash, const uint8_t* key, size_t ksize)
{
	cache_item_t** ptr;

	ptr = &(lru_cache->table[hash]);

	while(*ptr != NULL && (ksize != (*ptr)->ksize || memcmp(key, ITEM_KEY(*ptr), ksize) != 0)){
		ptr = &((*ptr)->hash_next);
	}

	return ptr;
}

static void delete_item(uint32_t hash, const uint8_t* key, size_t ksize)
{
	cache_item_t** ptr;

	ptr = find_item_pointer(hash, key, ksize);
	if((*ptr) != NULL)
		*ptr = (*ptr)->hash_next;
}

/*����������ܻ�ʱ��Ƚϳ�����*/
static void cache_weedout(time_t t)
{
	cache_item_t* it;
	cache_item_t* old;
	uint32_t r, hash;

	while(lru_cache->max_size < lru_cache->curr_size){
		/*�ȿ�����̭younger*/
		it = lru_cache->younger.next;
		
		assert(it != &(lru_cache->younger));

		/*younger�е�item̫���ˣ����Դ�older����̭*/
		if(it->time + CEEDOUT_THROLD < t){ 
			old = lru_cache->older.next;
			/*old���ڵ�ʱ�����*/
			if(old != &(lru_cache->older) && it->time > old->time) 
				it = old;
		}

		/*�ȱ��뽫it��older����younger�������������ֹ�����߳��ظ��ͷ�*/
		remove_list(it);
		hash = it->hash % lru_cache->hashpower;

		/*�ͷŵ�lru_cache lock, �����߳̿��Լ�������*/
		UNLOCK(&(lru_cache->lock));

		r = hash & REC_MASK;
		REC_LOCK(r);
		delete_item(hash, ITEM_KEY(it), it->ksize);
		REC_UNLOCK(r);

		/*���»��lru_cache->lock,��it�����ͷ�*/
		LOCK(&(lru_cache->lock));
		lru_cache->items_number --;
		unref(it);
	}
}

void insert_cache(const uint8_t* key, size_t ksize, const uint8_t* value, size_t vsize)
{
	uint32_t i, r;
	size_t size;
	cache_item_t* old, *it;
	cache_item_t** ptr;

	/*value�������1M,����1M�����ݲ����洢*/
	if(vsize > MAX_ITEM_VALUE_SIZE)
		return ;

	size = sizeof(cache_item_t) + ksize + vsize;

	it = (cache_item_t *)malloc(size);
	memset(it, 0, sizeof(cache_item_t));
	it->size = size;
	it->hash = murMurHash(key, ksize);
	it->ksize = ksize;
	it->vsize = vsize;
	it->vcount = 1;
	it->refcount = 1;
	it->time = time(NULL);

	memcpy(ITEM_KEY(it), key, ksize);
	memcpy(ITEM_VALUE(it), value, vsize);

	i = it->hash % lru_cache->hashpower;
	r = i & REC_MASK;

	REC_LOCK(r);
	ptr = find_item_pointer(i, key, ksize);
	old = *ptr;
	*ptr = it;
		
	if(old != NULL){
		it->hash_next = old->hash_next;
		REC_UNLOCK(r);

		LOCK(&(lru_cache->lock));
		remove_list(old);
		unref(old);

		lru_cache->items_number --;
	}
	else{
		REC_UNLOCK(r);
		LOCK(&(lru_cache->lock));
	}

	lru_cache->curr_size += it->size;
	lru_cache->put_count ++;
	lru_cache->items_number ++;

	append_younger(it);
	/*�ڴ���̭���*/
	cache_weedout(it->time);

	UNLOCK(&(lru_cache->lock));
}

void erase_cache(const uint8_t* key, size_t ksize)
{
	cache_item_t* it;
	cache_item_t** ptr;

	uint32_t hash = murMurHash(key, ksize) % lru_cache->hashpower;
	uint32_t r = hash & REC_MASK;

	REC_LOCK(r);

	ptr = find_item_pointer(hash, key, ksize);
	it = *ptr;
	if(it != NULL){
		*ptr = it->hash_next;
		REC_UNLOCK(r);

		LOCK(&(lru_cache->lock));
		lru_cache->items_number --;
		remove_list(it);
		unref(it);
		UNLOCK(&(lru_cache->lock));
	}
	else{
		REC_UNLOCK(r);
	}
}

void print_cache()
{
	LOCK(&(lru_cache->lock));

	printf("lru cache: \n");
	printf("	table size = %u\n", lru_cache->hashpower);
	printf("	item number = %ld\n", lru_cache->items_number);
	printf("	max_size = %ld\n", lru_cache->max_size);
	printf("	curr size = %ld\n", lru_cache->curr_size);
	printf("	get count = %ld\n", lru_cache->get_count);
	printf("	miss count = %ld\n", lru_cache->get_count - lru_cache->hit_count);
	printf("	put count = %ld\n", lru_cache->put_count);

	UNLOCK(&(lru_cache->lock));
}
