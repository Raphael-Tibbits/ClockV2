// code file for peripheral functions used in the clock


#include "clock_peripherals.h"


uint16_t heater_circuit_LUT[64] = {
    0,      14,     25,     39,     55,     76,     102,    133,
    172,    220,    278,    349,    436,    542,    671,    826,
    1013,   1237,   1502,   1815,   2182,   2607,   3097,   3656,
    4287,   4992,   5775,   6633,   7569,   8578,   9660,   10812,
    12031,  13314,  14656,  16053,  17503,  19000,  20543,  22128,
    23751,  25412,  27105,  28830,  30585,  32367,  34173,  36004,
    37857,  39730,  41622,  43533,  45461,  47404,  49362,  51335,
    53320,  55318,  57327,  59348,  61379,  63420,  65469,  65535,
};

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

void PWM_init(void){
    // Fast PWM mode 14: WGM13:0 = 1110
    TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11); // Non-inverting PWM
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS10);     // No prescaler
}

void adc_init(void){
  // Set the ADC to use AVcc as reference, left adjust result, and select ADC0 as input channel
  ADMUX = (1 << REFS0); // AVcc reference, right adjust, ADC0

  // Enable the ADC and set prescaler to 128 for 125kHz ADC clock at 16MHz system clock
  ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);


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
void send_bytes_to_shift_regs(uint8_t data[3]){
  // prepare the bytes to be sent to the shift registers
  uint8_t bitsRolledOut[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  for (int i = 0; i < 8; i++){
    bitsRolledOut[i] = (data[0] & (1 << i) ? 1 : 0)<<SEG_SER_PIN;
    bitsRolledOut[i+8] = (data[1] & (1 << i) ? 1 : 0)<<SEG_SER_PIN;
    bitsRolledOut[i+16] = (data[2] & (1 << i) ? 1 : 0)<<SEG_SER_PIN;
  }


  PORTD &= ~(1 << SEG_SER_PIN | 1 << SEG_SCLK_PIN | 1 << SEG_RCLK_PIN); // reset all outputs to 0
  uint8_t clearMask = ~(1 << SEG_SER_PIN);
  // roll out the bits to the shift registers
  for (int i = 0; i < 24; i++){
    PORTD &= clearMask; // clear SER pin
    PORTD |= bitsRolledOut[i]; // set SER pin to next bit to be sent

    // generate clock pulse
    PORTD |= 1 << SEG_SCLK_PIN;
    // _NOP();
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

// Setup Timer0 for 1ms tick
void timer0_init(void) {
  // Set CTC mode: WGM01 = 1, WGM00 = 0
  TCCR0A = (1 << WGM01);

  // Set prescaler = 64 → CS01 = 1, CS00 = 1
  TCCR0B = (1 << CS01) | (1 << CS00);

  // Compare match value for 1kHz
  OCR0A = 249;

  // Enable interrupt on compare match A
  TIMSK0 = (1 << OCIE0A);

  // Initialize counter
  TCNT0 = 0;
}

void uart_send_number(uint16_t number) {
    uart_send_char((number & 0xFF00) >> 8);
    uart_send_char(number & 0x00FF);
    uart_send_char('\n');
    uart_send_char('\r');
}

void uart_send_number_as_text(uint16_t number) {
  uart_send_char((number/100000)%10 + '0');
  uart_send_char((number/10000)%10 + '0');
  uart_send_char((number/1000)%10 + '0');
  uart_send_char((number/100)%10 + '0');
  uart_send_char((number/10)%10 + '0');
  uart_send_char(number%10 + '0'); // Convert number to string
    uart_send_char('\n');
    uart_send_char('\r');
}




/**
 * @brief converts ADC value to temperature in hundredths of a degree Celsius.
 * Uses a quadratic approximation based on calibration data. calibrated with 11k series resistor
 * @param adcVal Raw ADC value from thermistor.
 * @return Temperature in hundredths of a degree Celsius.
 */
uint16_t temp_from_adc(uint16_t adcVal){
  const float a = -0.0000632036; 
  const float b = 0.205524; 
  const float c = -61.9978;

  float temperature = a * adcVal * adcVal + b * adcVal + c;

  return (uint16_t)(temperature * 100);
}