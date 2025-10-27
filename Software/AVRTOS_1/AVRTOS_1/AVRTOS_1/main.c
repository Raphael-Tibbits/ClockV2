#include <avr/io.h>
#include "avrtos.h"
#include "clock_peripherals.h"
#include <avr/interrupt.h>

#define NUM_DIGITS 4

const uint8_t num_to_7seg[16] = {
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111, // 9
    0b01110111, // A
    0b01111100, // b    
    0b00111001, // C
    0b01011110, // d
    0b01111001, // E
    0b01110001, // F
};

volatile uint64_t systime = 0;

uint8_t display_raw[NUM_DIGITS];

void blink_task(void){
    while(1){
        PORTB ^= (1 << PORTB5);
        AVRTOS_delay(500);
    }
}

void display_task(void){
    while(1){
        send_bytes_to_serial(display_raw[0], 0x03);
        AVRTOS_delay(1);
        send_bytes_to_serial(display_raw[1], 0x0C);
        AVRTOS_delay(1);
        send_bytes_to_serial(display_raw[2], 0x30);
        AVRTOS_delay(1);
        send_bytes_to_serial(display_raw[3], 0xC0);
        AVRTOS_delay(1);
        
    }
}

// --- Tick handler: call from timer ISR in your main code ---
ISR(TIMER0_COMPA_vect) {
    systime++;

    
    AVRTOS_sequencer_ISR();
}


int main(){
     // Configure timer for 1 ms tick
    TCCR0A = (1 << WGM01);  
    TCCR0B = (1 << CS01) | (1 << CS00); // clk/64
    OCR0A = 125; 
    TIMSK0 = (1 << OCIE0A);

    DDRB |= (1 << DDB5);
    DDRD |= (1 << SEG_RCLK_PIN) | (1 << SEG_SCLK_PIN) | (1 << SEG_SER_PIN); // set shift register pins as outputs

    AVRTOS_create_task(idle_sleep_task, 0);
    AVRTOS_create_task(display_task, 1);
    AVRTOS_create_task(blink_task, 7);

    
    AVRTOS_start();

    return 0;
}