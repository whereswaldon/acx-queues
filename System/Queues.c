#include <avr/io.h>
#include "System.h"
#include "Queues.h"
#include <stdio.h>

QCB queues[QCB_MAX_COUNT];
byte allocated[QCB_MAX_COUNT];

/*
 * Dumps the contents of the queue with the given id
 */
void Q_dump(byte qid) {
    QCB* queue = &queues[qid];
    printf("%d: in: %x out: %x available: %d mask: %x contents: %s\n", qid, queue->in, queue->out, queue->available, queue->smask, queue->pQ);
}

/*
 * Attempts to write 1 byte to specified queue. returns 1 if successful, 0 if queue is full
 */
byte Q_putc(byte qid, char data) {
    QCB* queue = &queues[qid];
	if (queue->flags == Q_FULL) {
		return 0; //queue is full, failed to write
	}
	queue->pQ[queue->in++] = data;
	queue->in &= queue->smask;
	queue->available++;
	if(queue->available > 0 && queue->in == queue->out) {
		queue->flags = Q_FULL;
	}
	Q_dump(qid);
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
	*pdata = queue->pQ[queue->out++];
	queue->out &= queue->smask;
	
	queue->available--;
	
	if (queue->out == queue->in && queue->available == 0) {
		queue->flags = Q_EMPTY;
	}
	
	Q_dump(qid);
	return 1;
}

/*
 * compute the log base two of the integer
 */
int log_2(int n) {
    int result = 0;
    while (n >>= 1) {
        result++;
    }
    return result;
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
    int found = -1;
    for (int i = 0; i < QCB_MAX_COUNT; i ++) {
        allFull &= allocated[i];
        if (found == -1 && allocated[i] == 0) {
            found = i; //save the first index of a non-full queue
        }
    }
    if (allFull) {
        return -1; //all queues full
    }
    
    byte mask = 0xFF >> (8 - log_2(qsize));
    
    allocated[found] = 1;
    queues[found].in = 0;
    queues[found].out = 0;
    queues[found].smask = mask;
    queues[found].flags = Q_EMPTY;
    queues[found].available = 0;
    queues[found].pQ = pbuffer;
    
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
