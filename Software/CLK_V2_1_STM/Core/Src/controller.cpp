/*
 * controller.cpp
 *
 *  Created on: Nov 9, 2025
 *      Author: raphg
 */

#include "controller.hpp"
#include "main.h"
#include <algorithm>

extern ADC_HandleTypeDef hadc1;
extern RTC_HandleTypeDef hrtc;
extern TIM_HandleTypeDef htim1;

const float PWMconst[] = {
      0,      1,      2,      4,      5,      7,      9,     12,
     16,     20,     25,     31,     39,     48,     60,     73,
     89,    108,    131,    158,    190,    226,    269,    317,
    372,    433,    502,    577,    660,    749,    845,    948,
   1057,   1173,   1294,   1552,   1689,   1830,   1975,   2124,
   2277,   2433,   2593,   2755,   2920,   3088,   3258,   3430,
   3604,   3780,   3959,   4138,   4320,   4503,   4687,   4873,
   5060,   5248,   5437,   5628,   5819,   6012,   6205,   6399,
   6399
};

const float powerconst[] = {
    0.0f,   0.0016f, 0.0028f, 0.0043f, 0.0061f, 0.0083f, 0.0110f, 0.0143f,
    0.0183f,0.0233f, 0.0293f, 0.0366f, 0.0455f, 0.0563f, 0.0693f, 0.0849f,
    0.1037f,0.1261f, 0.1527f, 0.1840f, 0.2207f, 0.2633f, 0.3125f, 0.3687f,
    0.4323f,0.5038f, 0.5833f, 0.6710f, 0.7669f, 0.8709f, 0.9829f, 1.1026f,
    1.2297f,1.3638f, 1.5047f, 1.6519f, 1.8051f, 1.9639f, 2.1278f, 2.2967f,
    2.4701f,2.6478f, 2.8294f, 3.0147f, 3.2034f, 3.3953f, 3.5902f, 3.7879f,
    3.9882f,4.1909f, 4.3959f, 4.6031f, 4.8122f, 5.0232f, 5.2360f, 5.4504f,
    5.6664f,5.8838f, 6.1026f, 6.3228f, 6.5442f, 6.7667f, 6.9903f, 7.2150f,
    7.4407f,7.4408f
};

extern const uint16_t heater_circuit_LUT[64];


/**
 * @brief converts ADC value to temperature in degrees Celsius.
 * Uses a quadratic approximation based on calibration data. calibrated with 11k series resistor
 * @param adcVal Raw ADC value from thermistor.
 * @return Temperature in degrees Celsius.
 */
float temp_from_adc(uint16_t adcVal) {
    // Scale 12-bit STM32 ADC to equivalent 10-bit value
    float adc_scaled = float(adcVal) * 1023.0f / 4095.0f;

    const float a = -0.0000632036f;
    const float b = 0.205524f;
    const float c = -61.9978f;

    float temperature = a * adc_scaled * adc_scaled + b * adc_scaled + c;

    return (temperature); // °C
}


// --- Utility to get current RTC time in seconds + fraction ---
float get_rtc_seconds() {
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date; // Needed to read TR shadow correctly

    HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN); // Must read date after time

    // Compute subsecond fraction
    float subsec = float(time.SubSeconds)*0.0001f;

    return float(time.Hours * 3600 + time.Minutes * 60 + time.Seconds) + subsec;
}


Heater_controller::Heater_controller(float kp, float ki, float maxTemp)
    : MAX_TEMP(maxTemp),
      oven_target(0),
      heater_kp(kp),
      heater_ki(ki),
      integral(0),
      last_seconds(0.0f)
{
//    // Initial temperature read
//    HAL_ADC_Start(&hadc1);
//    HAL_ADC_PollForConversion(&hadc1, 10);
//    ovenTemp = temp_from_adc(HAL_ADC_GetValue(&hadc1));
//    HAL_ADC_Stop(&hadc1);{
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
}

Heater_controller::~Heater_controller(){
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
}

void Heater_controller::update() {
    // Read new temperature
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 10);
    ovenTemp = temp_from_adc(HAL_ADC_GetValue(&hadc1));
    HAL_ADC_Stop(&hadc1);

    if (ovenTemp > MAX_TEMP) {
        set_heater_power(0.0f);
        last_seconds = get_rtc_seconds(); // reset timing
        return;
    }

    // Compute dt since last update
    float now = get_rtc_seconds();
    float dt = now - last_seconds;
    if (dt < 0) dt += 24*3600; // handle midnight rollover

    last_seconds = now;

    // PI controller
    float error = oven_target - ovenTemp;
    integral += error * dt;
    integral = (integral < -100.0f) ? -100.0f : ((integral > 100.0f) ? 100.0f : integral);

    float actuation = error * heater_kp + integral * heater_ki;
    set_heater_power(actuation);
}

// x and y arrays must be increasing
float inverse_lookup(const float* x, const float* y, size_t len, float y_val) {
    if (!x || !y || len == 0) return 0.0f;

    // Clamp to bounds
    if (y_val <= y[0]) return x[0];
    if (y_val >= y[len-1]) return x[len-1];

    // Find interval
    for (size_t i = 0; i < len-1; i++) {
        if (y_val >= y[i] && y_val <= y[i+1]) {
            float t = (y_val - y[i]) / (y[i+1] - y[i]); // fraction in interval
            return x[i] + t * (x[i+1] - x[i]);           // linear interpolation
        }
    }

    return x[len-1]; // fallback
}

void Heater_controller::set_heater_power(float power) {
    if (ovenTemp > MAX_TEMP) {
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, 0);
        return;
    }

    uint16_t heater_val = inverse_lookup(PWMconst, powerconst, 63, power);

    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, static_cast<uint16_t>(heater_val));
}

float Heater_controller::get_temp(){
	return this->ovenTemp;
}
