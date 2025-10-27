#pragma once

#include "clock_peripherals.h"


inline int clamp(int v, int lo, int hi) {
    return (v < lo) ? lo : (v > hi) ? hi : v;
}



/**
 * @brief Class to manage heater control logic
 * 
 * PI controller for the heater element based on temperature feedback.
 */
class Heater_controller{ 
public:
    Heater_controller(int kp, int ki, uint16_t maxTemp);
    ~Heater_controller();


    /**
     * @brief heater controller update
     * 
     * 
     */
    void update(uint64_t milliseconds);
    

    /**
     * @brief sets the heater power level
     * 
     * @param power the desired power level in mW
     */
    void set_heater_power(uint16_t power);
    
    uint16_t MAX_TEMP;
    
    uint16_t oven_target;

private:
    int heater_kp;
    int heater_ki;
    int intergral;
    uint64_t last_milliseconds; // timestamp of previous measurement
    uint16_t ovenTemp; // this is the actual measured temperature
};