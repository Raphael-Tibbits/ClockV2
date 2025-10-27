/**
 * @file clock_peripherals.h
 * @brief Peripheral control functions for Timer1 PWM, ADC, UART, and 7-segment shift registers.
 *
 * This header provides function prototypes for controlling Timer1 PWM outputs, 
 * starting ADC conversions, transmitting data via UART, and driving 
 * 7-segment displays via serial shift registers.
 */

#ifndef CLOCK_PERIPHERALS_H
#define CLOCK_PERIPHERALS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <avr/io.h>
#include <stdint.h>
#include <avr/cpufunc.h> // for _NOP()
#include <string.h>
#include <stdio.h> // for snprintf

/** @name Shift Register Pin Definitions
 *  These external variables represent the bit positions of the 7-segment 
 *  shift register pins on PORTD.
 *  They must be defined in the main program or a hardware configuration file.
 */
///@{
#define SEG_SER_PIN PD5  /**< Serial data pin bit position on PORTD */
#define SEG_SCLK_PIN PD7  /**< Serial clock pin bit position on PORTD */
#define SEG_RCLK_PIN PD6  /**< Register clock (latch) pin bit position on PORTD */
///@}

/** @name IO Pin Definitions
 *  These external variables represent the bit positions of the on-board LEDs.
 *  They must be defined in the main program or a hardware configuration file.
 */
///@{
#define LED1_PIN PD0   /**< On-board LED pin bit position on PORTD */
#define BUTTON_PIN PC0   /**< On-board button pin bit position on PORTC */
#define Thermistor_Pin PC1   /**< Thermistor pin bit position on PORTC */
#define LDR_Pin PC3   /**< LDR pin bit position on PORTC */
#define Heat_Pin PB1   /**< Heating element control pin bit position on PORTB */
#define One_PPS_Pin PD4   /**< 1PPS signal input pin bit position on PORTD */

#define DIO2 PD3
///@}


/**
 * @brief Lookup table for heater circuit linearization.
 */
extern uint16_t heater_circuit_LUT[64];

// const uint8_t hex_2_7seg[32] =
// /*abcdefg.*/ // converted to hex
// {0b11111100, 0b01100000, 0b11011010, 0b11110010, 0b01100110, 0b10110110, 0b10111110, 0b11100000, 0b11111110, 0b11100110, 0b11101110, 0b00111110, 0b10011100, 0b01111010, 0b10011110, 0b10001110,
//  0b11111101, 0b01100001, 0b11011011, 0b11110011, 0b01100111, 0b10110111, 0b10111111, 0b11100001, 0b11111111, 0b11100111, 0b11101111, 0b00111111, 0b10011101, 0b01111011, 0b10011111, 0b10000011
// };
 
/**
 *  @brief initilise PWM on Timer 1
 * Configures Timer1 for Fast PWM mode with non-inverting outputs on OC1A and OC1B.
 * Sets no prescaling for maximum frequency.
 * Call this function once during system initialization.
 * @note The duty cycle and period must be set separately using set_PWMA_duty_cycle(), set_PWMB_duty_cycle(), and set_PWM_period().
 */
void PWM_init(void);

/**
 * @brief Set the duty cycle of Timer1 PWM1 (OCR1B / PB2).
 *
 * Updates the OCR1B register to change the PWM duty cycle.
 *
 * @param newDC New duty cycle value (0 to ICR1 for 100%).
 */
void set_PWMB_duty_cycle(uint16_t newDC);

/**
 * @brief Set the duty cycle of Timer1 PWM2 (OCR1A / PB1).
 *
 * Updates the OCR1A register to change the PWM duty cycle.
 *
 * @param newDC New duty cycle value (0 to ICR1 for 100%).
 */
void set_PWMA_duty_cycle(uint16_t newDC);

/**
 * @brief Set the period of Timer1 PWM.
 *
 * Updates the ICR1 register to change the PWM period.
 * This value defines the TOP of the timer counter in Fast PWM mode.
 *
 * @param newPeriod New top value for ICR1.
 */
void set_PWM_period(uint16_t newPeriod);

/**
 * @brief Initialize the ADC.
 *
 * Configures the ADC for 10-bit resolution, AVcc reference, and prescaler of 128.
 * Call this function once during system initialization.
 */
void adc_init(void);

/**
 * @brief Start an ADC conversion on the specified channel.
 *
 * Updates the ADMUX register to select the target channel and starts a conversion.
 *
 * @param targetPin ADC channel number (0-7 typical for ATmega328).
 */
void start_ADC(uint8_t targetPin);

/**
 * @brief get ADC reading from specified channel
 * 
 * 
 * Start an ADC conversion on the specified channel and blocks until the conversion is complete. returns the conversion result
 * 
 * @param targetPin ADC channel number 
 */
uint16_t get_ADC_result(uint8_t targetPin);

/**
 * @brief Send digit and segment bytes to 7-segment shift registers.
 *
 * This function shifts out a digit selection byte followed by a segment byte
 * using PORTD pins connected to serial shift registers.
 *
 * @param segByte Segment byte to shift out.
 * @param digByte Digit byte to shift out.
 */
void send_bytes_to_shift_regs(uint8_t data[3]);

/**
 * @brief Send a single character via UART0.
 *
 * Waits until the transmit buffer is ready, then writes the character to UDR0.
 *
 * @param data Character to transmit.
 */
void uart_send_char(char data);

/**
 * @brief Send a null-terminated string via UART0.
 *
 * Transmits the string one character at a time using uart_send_char().
 *
 * @param str Pointer to the null-terminated string to transmit.
 */
void uart_send_string(const char* str);

/**
 * @brief send a text number via UART0.
 *
 * sends a 16-bit unsigned integer as text.
 *
 * @param number The 16-bit unsigned integer to send.
 */
void uart_send_number(uint16_t number);

void uart_send_number_as_text(uint16_t number);

/**
 * @brief Setup Timer0 for 1ms tick interrupts.
 *  Configures Timer0 in CTC mode with a compare value for 1ms intervals.
 * Sets prescaler to 64 and enables Output Compare Match A interrupt.
 */
void timer0_init(void);



/**
 * @brief converts ADC value to temperature in hundredths of a degree Celsius.
 * Uses a quadratic approximation based on calibration data. calibrated with 11k series resistor
 * @param adcVal Raw ADC value from thermistor.
 * @return Temperature in hundredths of a degree Celsius.
 */
uint16_t temp_from_adc(uint16_t adcVal);



#ifdef __cplusplus
}
#endif

#endif // MY_PERIPHERALS_H