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
        int status = Q_create(64, buffer);
        printf("%d\n", status);
        status = Q_create(64, buffer);
        printf("%d\n", status);
        status = Q_create(64, buffer);
        printf("%d\n", status);
        status = Q_create(64, buffer);
        printf("%d\n", status);
        status = Q_create(64, buffer);
        printf("%d\n", status);
}

