#include <avr/io.h>
#include <clock_peripherals.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "controller.hpp"
// #include "time.hpp"


#define MAX_TEMP 7000 // max temperature in hundredths of a degree Celsius (60.00C)


volatile uint64_t miliseconds;

volatile uint8_t flag = 0;



Heater_controller Crystal_Oven(1, 0, 6000);


ISR(TIMER0_COMPA_vect){
  // Timer0 interrupt service routine
  // This will be called every 1ms
  miliseconds++;
  flag = 1;
  // uart_send_number(miliseconds);
}

int main()
{

  // setup clock to use external 16MHz oscilatior
  CLKPR = (1 << CLKPCE); // enable change of the clock prescaler
  CLKPR = 0; // set prescaler to 1 (16MHz)

  // Disable interrupts while changing WDT
  cli();

  // Clear WDRF in MCUSR
  MCUSR &= ~(1 << WDRF);

  // Start timed sequence to disable WDT
  wdt_reset();                         // Reset WDT
  WDTCSR |= (1 << WDCE) | (1 << WDE);  // Enable change
  WDTCSR = 0x00;                       // Disable WDT


  // setup timer tick
  timer0_init();

  // setup PWM on Timer 1
  PWM_init();
  set_PWM_period(1024); // set period to 601
  set_PWMA_duty_cycle(0); // start with 0% duty cycle
  set_PWMB_duty_cycle(1024); // start with 0% duty cycle
  // setup ADC
  adc_init();

  sei(); // enable global interrupts

  // setup uart
  UBRR0 = 8; // Set baud rate to 112500 for 16MHz clock
  UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // Set frame format: 8 data bits, 1 stop bit, no parity
  UCSR0B = (1 << TXEN0) | (1 << RXEN0); // Enable transmitter and receiver

  
  DDRD |= (1 << LED1_PIN) | (1 << DIO2); // Set LED1_PIN and DIO2 as output

  uart_send_string("hello");
  PORTD |= (1 << DIO2);
  _NOP();
  _NOP();
  _NOP();
  _NOP();
  PORTD &= ~(1 << DIO2);

  
  DDRD |= (1 << SEG_SER_PIN) | (1 << SEG_SCLK_PIN) | (1 << SEG_RCLK_PIN); // Set shift register pins as outputs
  DDRC &= ~((1 << BUTTON_PIN) | (1 << Thermistor_Pin) | (1 << LDR_Pin)); // Set button and sensor pins as inputs
  DDRB |= (1 << Heat_Pin); // Set heating element control pin as output
  DDRD |= (1 << One_PPS_Pin); // Set 1PPS signal input pin as output


  while (1) {
    // runs constantly
    
    if (!flag) {
        // runs if nothing else to do
        continue; // Skip the rest of the loop if no new millisecond tick
    }
    flag = 0;

    // 10Hz loop
    if (miliseconds % 100 == 0) {
      // Crystal_Oven.update(miliseconds);

      if((miliseconds >5000UL) && (miliseconds < 6000UL)){
        Crystal_Oven.set_heater_power((uint16_t)1000); // set heater to 1W for 1s
      }

      // uart_send_number_as_text(ovenTemp);
      // uart_send_char('T');
      // uart_send_number_as_text(actuation);
      // uart_send_char('A');

    }

    // 10Hz alternate loop
    if ((miliseconds+50) % 100 == 0) {
      uint16_t light = get_ADC_result(LDR_Pin);
      // uart_send_number(light);
    }

    if (miliseconds % 1000 == 0) {
      uart_send_number_as_text(miliseconds/1000);
      uart_send_char('S');
    }

    

    uint8_t testPattern[3] = {0x00, 0x00, 0x00};
    send_bytes_to_shift_regs(testPattern);

    
  }

  return 0;
}
