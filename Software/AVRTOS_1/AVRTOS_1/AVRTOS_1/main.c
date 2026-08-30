/*
 * main.c
 *
 * Created: 9/26/2025 4:52:33 PM
 *  Author: raphg
 */ 

#include <xc.h>

int main(void)
{
    while(1)
    {
		PORTD |= 0x01;
		PORTD &= 0x00;
        //TODO:: Please write your application code 
    }
}