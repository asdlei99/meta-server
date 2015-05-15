#ifndef __CELL_POOL_H_
#define __CELL_POOL_H_

#include <stdint.h>

/*�����ʼ������ָ��*/
typedef int32_t(*ob_constructor_t)(void* ob);
/*����������ָ��*/
typedef void(*ob_destructor_t)(void* ob);
/*�Ϸ���У�麯��ָ��*/
typedef int32_t(*ob_check_t)(void* ob);
/*����λ����ָ��*/
typedef void(*ob_reset_t)(void* ob, int32_t flag);

typedef struct cell_pool_s
{
	char*				name;		/*pool name*/
	void**				ptr;		/*object array*/

	size_t				ob_size;	/*����ռ�õĿռ��ֽ���*/
	int					mutex;		/*latch*/
	int32_t				array_size;	/*ptr����ĳ���*/
	int32_t				curr;		/*��ǰ���е�ptrλ��*/

	ob_constructor_t	constructor; /*�����ʼ������ָ��*/
	ob_destructor_t		destructor;	/*���������պ���ָ��*/
	ob_check_t			check;		
	ob_reset_t			reset;
}cell_pool_t;

cell_pool_t*			pool_create(const char* name, size_t ob_size, ob_constructor_t constructor, ob_destructor_t destructor, 
										ob_check_t check, ob_reset_t reset);

void					pool_destroy(cell_pool_t* pool);

void*					pool_alloc(cell_pool_t* pool);

void					pool_free(cell_pool_t* pool, void* ob);

void					pool_print(cell_pool_t* pool);

int32_t					get_pool_info(cell_pool_t* pool, char* buf);

#endif





