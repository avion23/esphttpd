/*
 * adc.c
 *
 *  Created on: Feb 1, 2015
 *      Author: avion
 */


#include "ets_sys.h"
#include "os_type.h"
#include "mem.h"
#include "osapi.h"
#include "user_interface.h"

#include "espconn.h"

uint16 getAdcValue() {

	uint16 adcValue = system_adc_read();
	return (adcValue);
}
