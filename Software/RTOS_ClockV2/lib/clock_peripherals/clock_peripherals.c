// code file for peripheral functions used in the clock


#include "clock_peripherals.h"

// changes the duty cycle of the timer 1 pwm1 i.e. OCR1B AKA PB2
void set_PWMB_duty_cycle(uint16_t newDC){
  OCR1B = newDC;
}

// changes the duty cycle of the timer 1 pwm2 i.e. OCR1A AKA PB1
void set_PWMA_duty_cycle(uint16_t newDC){
  OCR1A = newDC;
}

void set_PWM_period(uint16_t newPeriod){
  // Update Timer 1 period (top value)
  ICR1 = newPeriod;
}

void start_ADC(uint8_t targetPin){
  // set conversion target
  ADMUX = (ADMUX & 0xf0) | (targetPin & 0x0f);
  
  // Start the conversion
  ADCSRA |= (1 << ADSC); 
}

uint16_t get_ADC_result(uint8_t targetPin){

  start_ADC(targetPin);
  // Wait for conversion to complete
  while (!(ADCSRA & (1 << ADIF)));

  // Clear flag by writing 1
  ADCSRA |= (1 << ADIF);

  return ADC;
}

// sends digit and segment bytes to shift registers
void send_bytes_to_serial(uint8_t segByte, uint8_t digByte){
  // prepare the bytes to be sent to the shift registers
  uint8_t bytes_rolled_out[16];
  uint8_t portD_existing = PORTD;
  portD_existing &= ~( (1 << SEG_SER_PIN) | (1 << SEG_SCLK_PIN) | (1 << SEG_RCLK_PIN) ); // clear the bits we are using for the shift register
  for (int i = 0; i < 8; i++){
    bytes_rolled_out[i] = (segByte & (1 << i) ? 1 : 0)<<SEG_SER_PIN | portD_existing;
    bytes_rolled_out[i+8] = (digByte & (1 << i) ? 1 : 0)<<SEG_SER_PIN | portD_existing;
  }


  PORTD &= ~(1 << SEG_SER_PIN | 1 << SEG_SCLK_PIN | 1 << SEG_RCLK_PIN); // reset all outputs to 0

  // roll out the bits to the shift registers
  for (int i = 0; i < 16; i++){

    PORTD = (bytes_rolled_out[i] & (1 << SEG_SER_PIN));

    // generate clock pulse
    PORTD |= 1 << SEG_SCLK_PIN;
    _NOP();
    PORTD &= ~(1 << SEG_SCLK_PIN);
  }

  // _NOP();
  // generate pulse on Rclk to set all the outputs to the new values
  PORTD |= 1 << SEG_RCLK_PIN;
  _NOP();
  PORTD &= ~(1 << SEG_RCLK_PIN);

}

// Function to transmit a single character via UART
void uart_send_char(char data) {
    // Wait for the transmit buffer to be empty
    while (!(UCSR0A & (1 << UDRE0))) {
        // Do nothing until UDR0 is ready for more data
    }
    
    // Load the data into the transmit buffer
    UDR0 = data;
}

// Function to transmit a string via UART
void uart_send_string(const char* str) {
    while (*str) {
        uart_send_char(*str);
        str++;
    }
}

uint8_t uart_blocking_receive_char() {
    // Wait for data to be received
    while (!(UCSR0A & (1 << RXC0))) {
        // Do nothing until data is received
    }
    
    // Get and return the received data from the buffer
    return UDR0;
}

uint8_t uart_data_available() {
    // Check if data has been received
    return (UCSR0A & (1 << RXC0));
}

void uart_recieve_srting_until_null(uint8_t* buffer, uint8_t max_length) {
    uint8_t index = 0;
    while (index < max_length - 1) { // Leave space for null terminator
        if (uart_data_available()) {
            char received_char = uart_blocking_receive_char();
            if (received_char == '\0') {
                break; // Stop if null character is received
            }
            buffer[index++] = received_char;
        }
    }
    buffer[index] = '\0'; // Null-terminate the string
}

uint64_t uart_recieve_uint64(){
    uint64_t number;
    for(int i=0; i<8; i++){
        ((uint8_t*)&number)[i] = uart_blocking_receive_char();
    }
    return number;
}