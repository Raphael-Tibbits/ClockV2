// code for heater control


#include "controller.hpp"

// lookup table linear interpolation for 64 number table
uint16_t lookup_table_inverse_interp(uint16_t input, const uint16_t* table) {
    if (table == NULL) return 0; // Null table pointer

    // Find the interval in which the input lies
    for (uint8_t i = 0; i < 63; i++){
        if (input >= table[i] && input <= table[i + 1]) {
            // Perform linear interpolation

            uint16_t x0 = i << 10;
            uint16_t x1 = (i + 1) << 10;

            uint16_t y0 = table[i];
            uint16_t y1 = table[i + 1];

            uint16_t x = (input - y0) * (x1 - x0) / (y1 - y0) + x0;

            return x;
        }
    }
    return 0;
}



Heater_controller::Heater_controller(int kp, int ki, uint16_t maxTemp)
    : MAX_TEMP(maxTemp),
      oven_target(0),
      heater_kp(kp),
      heater_ki(ki),
      intergral(0),
      last_milliseconds(0),
      ovenTemp(temp_from_adc(get_ADC_result(Thermistor_Pin))) {}

Heater_controller::~Heater_controller() {
        set_PWMA_duty_cycle(0); // Ensure heater is off on destruction
    }


void Heater_controller::update(uint64_t milliseconds) {
    
    // get the new temp measurment
    ovenTemp = temp_from_adc(get_ADC_result(Thermistor_Pin));
    

    if(ovenTemp > MAX_TEMP){
        last_milliseconds = milliseconds;
        // over temperature, turn off heater
        set_PWMA_duty_cycle(0);
        return;
    }

    // compute error
    int error = oven_target - ovenTemp;

    // int = error * dt
    intergral += error * (milliseconds-last_milliseconds);

    // intergral saturation
    intergral = clamp(intergral, -100, 100);

    // PI controller implementation
    int actuation = error * heater_kp + intergral *heater_ki;

    set_heater_power(actuation);
    
    last_milliseconds = milliseconds;
    
}



void Heater_controller::set_heater_power(uint16_t power){
    if(ovenTemp > MAX_TEMP){
        // over temperature, turn off heater
        set_PWMA_duty_cycle(0);
        return;
    }

    const int scale = 8807; // the constant for power to int conversion

    uint16_t scaled_power = (scale*power) / 1000; 
    uint16_t heater_val = lookup_table_inverse_interp(scaled_power, heater_circuit_LUT);

    // LUT outputs 0->65535, PWM is 0-1024
    set_PWMA_duty_cycle(heater_val>>6);
}


