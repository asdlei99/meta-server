#ifndef __CELL_HANDLER_H_
#define __CELL_HANDLER_H_

#include <stdint.h>

#include "cell_msg.h"
#include "cell_codec.h"

/*hander state*/
#define HANDLER_IDLE			0
#define HANDLER_LISTENING		1
#define HANDLER_CONNECTING		2
#define HANDLER_CONNECTED		3
#define HANDLER_DISCONNECTED	4

typedef struct handler_s
{
	int				fd;					/*socket ���*/
	int32_t			upated;				/*�Ƿ�ı��˼����¼�*/
	int				ev_flags;			/*����ʱ�伯*/

	int32_t			state;				/*��ǰ������״̬*/

	bin_stream_t*	rstrm;				/*���ջ�����*/	
	bin_stream_t*	sstrm;				/*���ͻ�����*/

	uint64_t		magigc;				/*У��ħ����,����cell pool*/
}handler_t;

/*cell pool callback function*/
int32_t				handler_init(void* ptr);
void				handler_destroy(void* ptr);
void				handler_reset(void* ptr, int32_t flag);
int32_t				handler_check(void* ptr);

handler_t*			handler_new(int fd, int32_t state);
void				handler_close(handler_t* handler);

int32_t				handle_machine(handler_t* hander, int mask);

void				hander_send(handler_t* handler, uint16_t id, void* body);

#endif
