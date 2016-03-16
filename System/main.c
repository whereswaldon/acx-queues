/*
 * System.c
 *
 * Created: 3/15/2016 2:30:26 PM
 * Author : waldonck
 */ 

#include <avr/io.h>
#include <stdio.h>
#include "Queues.h"


int main(void)
{
        char buffer[64];
        int qid = Q_create(64, buffer);
        printf("%d\n", qid);
        char in = 'a';
        char out;
        for (int i = 0; i < 26; i++) {
            if (!Q_putc(qid, in+i)) {
                printf("Error writing char %c on iteration %d\n", in+i, i);
            }
            if (!Q_getc(qid, &out)) {
                printf("Error reading char %c on iteration %d\n", out, i);
            }
            printf("Wrote %c, Read %c\n", in+i, out);
        }
}

