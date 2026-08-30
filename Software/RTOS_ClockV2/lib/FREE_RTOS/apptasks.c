/*
 * tasks.c
 *
 *  Created on: Feb 4, 2015
 *      Author: jcobb
 */

#include "FreeRTOS.h"
#include "task.h"
#include "tasks.h"
#include "clock_peripherals.hpp"


void vLEDFlashTask(void *pvParms)
{
	vLEDInit();
	portTickType xLastWakeTime;
	const portTickType xFrequency = 1000;
	xLastWakeTime = xTaskGetTickCount();

	for(;;) {
		vLEDToggle();
		vTaskDelayUntil(&xLastWakeTime, xFrequency);
	}
}

