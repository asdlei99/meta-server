#include <stdio.h>
#include <stdlib.h>
#include <error.h>
#include <pthread.h>
#include <malloc.h>
#include <assert.h>

#include "poll_thread.h"
#include "ae_poll.h"
#include "cell_log.h"
#include "cell.h"

static event_loop_t loop;
static pthread_mutex_t leader;

static thread_count = 0;

void thread_init(int nthreads)
{
	pthread_mutex_init(&leader, NULL);
	memset(&loop, 0, sizeof(event_loop_t));

	if(ae_create(&loop) == AE_ERR){
		log_error("thread init failed, ae_create!");
		exit(-1);
	}

	thread_count = nthreads;
}

void thread_destroy()
{

}


/*��fd��Ϊ���������Բ���ʹ��lock��������*/
int add_event(int fd, int mask, handler_t* h)
{
	if(fd > AE_SETSIZE){
		log_error("fd is too large:%d\n", fd);
		return AE_ERR;
	}
	
	assert(loop.conns[fd] == NULL);

	/*log_debug("add event, fd = %d, event = %d", fd, mask);*/

	loop.conns[fd] = h;

	if(ae_add_event(&loop, fd, mask) == AE_ERR){
		log_error("ae_add_event failed");
		loop.conns[fd] = NULL;
		return AE_ERR;
	}

	return AE_OK;
}

int32_t update_event(int fd, int mask, handler_t* h)
{
	if(fd > AE_SETSIZE){
		log_error("fd is too large:%d\n", fd);
		return AE_ERR;
	}

	h->upated = 0;
	
	/*log_debug("update event, fd = %d, event = %d", fd, mask);*/

	loop.conns[fd] = h;
	if (ae_update_event(&loop, fd, mask) == -1){
		log_error("ae_update_event failed");
		loop.conns[fd] = NULL;
		return AE_ERR;
	}
	return AE_OK;
}

int delete_event(int fd)
{
	if(fd > AE_SETSIZE){
		log_error("fd is too large:%d\n", fd);
		return AE_ERR;
	}

	/*log_debug("delete event, fd = %d", fd);*/

	loop.conns[fd] = NULL;
	if(ae_del_event(&loop, fd) == -1){
		log_error("ae_del_event failed");
		return AE_ERR;
	}

	return AE_OK;
}

static void* thread_worker(void* arg)
{
	struct timeval tv = {0, 500000};

	/*�����߳̿���ȡ��*/
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, 0);

	while (daemon_run != 0) {
		pthread_mutex_lock(&leader); /*��ռepoll leader��*/
AGAIN:
		while(loop.nready <= 0 && daemon_run != 0)
			loop.nready = ae_wait(&loop, &tv);

		if(daemon_run == 0){
			pthread_mutex_unlock(&leader);
			break;
		}

		loop.nready --;
		/*��ö�Ӧ���ӵ���Ϣ,��Ҫ��fd,��д���ͺ�handler����*/
		int fd = loop.fired[loop.nready].fd;
		int mask = loop.fired[loop.nready].mask;

		/*log_debug("fd = %d, event = %d", fd, mask);*/

		handler_t* h = loop.conns[fd];
		if(h == NULL){
			log_warn("conn %d should not be NULL", fd);
			delete_event(fd);
			close(fd);

			goto AGAIN;
		}

		pthread_mutex_unlock(&leader);

		/*�������¼��Ĵ���*/
		if(handle_machine(h, mask) == 0){
			if(update_event(fd, h->ev_flags, h))
				handler_close(h);
		}
	}

	return NULL;
}

void loop_run(check_zookeeper_t fun)
{
	int i, ret;
	pthread_t* ids;
	pthread_attr_t attr;

	log_warn("loop running ...");
	/*����������io �̳߳�*/
	pthread_attr_init(&attr);
	ids = (pthread_t*)malloc(sizeof(pthread_t) * thread_count);
	for(i = 0; i < thread_count; i ++){
		if((ret = pthread_create(ids + i, &attr, thread_worker, NULL))){
			log_error("can't create thread, error = %d", ret);
			exit(-1);
		}
	}

	/*�����߳�Ҳ���뵽����event loop������*/
	fun();

	log_warn("loop exiting ...");
	/*�ȴ������߳��˳�*/
	for(i = 0; i < thread_count - 1; i ++){
		pthread_join(ids[i], NULL); 
		 pthread_detach(ids[i]);
	}

	log_warn("stopped all thread!");
	free(ids);

	/*����handler*/ 
	for(i = AE_SETSIZE - 1; i > 0; i --){ 
		handler_t* h = (handler_t *)(loop.conns[i]);
		if(h != NULL){
			handler_close(h);
		}
	} 
	 
	/*�ͷ�poll�ṹ*/
	ae_free(&loop);

	log_warn("loop exited!!!");
}



