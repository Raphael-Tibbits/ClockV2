/*
 * controller.hpp
 *
 *  Created on: Nov 9, 2025
 *      Author: raphg
 */

#ifndef CONTROLLER_HPP_
#define CONTROLLER_HPP_

#pragma once

#include <stdint.h>
#include "stm32l4xx_hal.h"

// Forward declaration of LUT
extern const uint16_t heater_circuit_LUT[64];

class Heater_controller {
public:
    Heater_controller(float kp, float ki, float maxTemp);
    ~Heater_controller();

    void update();
    void set_heater_power(float power);
    void set_target(float target) { oven_target = target; }
    float get_temp();

private:
    const float MAX_TEMP;
    float oven_target;
    float heater_kp;
    float heater_ki;
    float integral;
    uint64_t last_seconds;
    float ovenTemp;

};



#endif /* CONTROLLER_HPP_ */
