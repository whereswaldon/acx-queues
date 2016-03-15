#include <avr/io.h>
#include "System.h"
#include "Queues.h"

char* data1[64];
char* data2[64];

QCB queues[QCB_MAX_COUNT];
byte allocated[QCB_MAX_COUNT];

/*
 * Attempts to write 1 byte to specified queue. returns 1 if successful, 0 if queue is full
 */
byte Q_putc(byte qid, char data) {
    QCB* queue = &queues[qid];
	if (queue->available == sizeof(queue->pQ)) {
		return 0; //queue is full, failed to write
	}
	
	queue->available++;
	if(queue->available == sizeof(queue->pQ)) {
		queue->flags = Q_FULL;
	}
	return 1;
}

/*
 * Attempts to read 1 byte from specified queue to *pdata. Returns 1 if successful, 0 of queue empty
 */
byte Q_getc(byte qid, char *pdata ) {
    QCB* queue = &queues[qid];
	if (queue->in == queue->out && queue->flags == Q_EMPTY) {
		return 0; //queue is empty
	}
	byte read = queue->pQ[queue->out--];
	*pdata = read;
	if (queue->out == queue->in) {
		queue->flags = Q_EMPTY;
	}
	queue->available--;
	return 1;
}

/*
 * Create a queue of specified size. Returns id of next available queue, -1 if error (no more queues, invalid size)
 */
byte Q_create(int qsize, char * pbuffer) {
    //check that qsize is valid
    if (qsize > 256 || qsize < 1 || !(qsize == 2
        || qsize == 4 || qsize == 8 || qsize == 16
        || qsize == 32 || qsize == 64 || qsize == 128
        || qsize == 256)) {
            return -1; //invalid size
    }
    byte allFull = 1;
    byte found = -1;
    for (int i = 0; i < QCB_MAX_COUNT; i ++) {
        allFull &= allocated[i];
        if (found == -1 && allocated[i] == 0) {
            found = i; //save the first index of a non-full queue
        }
    }
    if (allFull) {
        return -1; //all queues full
    }
    QCB* queue = &queues[found];
    allocated[found] = 1;
    
    queue->in = 0;
    queue->out = 0;
    queue->smask = 0xFF;
    queue->flags = Q_EMPTY;
    queue->available = qsize;
    queue->pQ = pbuffer;
    
    return found;
}

/*
 * Remove specified queue and reset it to be reused (by Q_create)
 */
void Q_delete(byte qid) {
	QCB* queue = &queues[qid];
    if (allocated[qid]) {
		allocated[qid] = 0;
	}
	Q_create(sizeof(queue->pQ), queue->pQ);
}

/*
 * Returns number of bytes used in the queue (available for reading). Returns -1 if error (invalid queue).
 */
int Q_used(byte qid) {
    QCB* queue = &queues[qid];
	return queue->available;
}

/*
 * Returns number of unused bytes in the queue (available for writing). Returns -1 if error (invalid queue).
 */
int Q_unused(byte qid) {
	QCB* queue = &queues[qid];
    return sizeof(queue->pQ) - queue->available;
}
