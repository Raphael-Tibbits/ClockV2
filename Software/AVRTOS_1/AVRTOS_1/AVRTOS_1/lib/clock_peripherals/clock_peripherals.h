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

#include <avr/io.h>
#include <stdint.h>
#include <avr/cpufunc.h> // for _NOP()
#include "avrtos.h"

/** @name Shift Register Pin Definitions
 *  These external variables represent the bit positions of the 7-segment 
 *  shift register pins on PORTD.
 *  They must be defined in the main program or a hardware configuration file.
 */
///@{
#define SEG_SER_PIN PD2  /**< Serial data pin bit position on PORTD */
#define SEG_SCLK_PIN PD3  /**< Serial clock pin bit position on PORTD */
#define SEG_RCLK_PIN PD4  /**< Register clock (latch) pin bit position on PORTD */
///@}

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
void send_bytes_to_serial(uint8_t segByte, uint8_t digByte);

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

#endif // MY_PERIPHERALS_H