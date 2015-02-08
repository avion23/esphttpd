/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Jeroen Domburg <jeroen@spritesmods.com> wrote this file. As long as you
 * retain
 * this notice you can do whatever you want with this stuff. If we meet some
 * day,
 * and you think this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */
#include "c_types.h"
#include "user_interface.h"
#include "espconn.h"
#include "mem.h"
#include "osapi.h"
#include "gpio.h"
#include "espmissingincludes.h"
#include "dht.h"

#define MAXTIMINGS 40000
#define DHT_MAXCOUNT 32000
#define BREAKTIME 32

const bool debug = false;

enum sensor_type SENSOR;

static float scale_humidity(int *data) {
	if (SENSOR == SENSOR_DHT11) {
		return data[0];
	} else {
		float humidity = (data[0] << 8) + data[1];
		return humidity /= 10;
	}
}

static float scale_temperature(int *data) {
	if (SENSOR == SENSOR_DHT11) {
		return data[2];
	} else {
		float temperature = data[2] & 0x7f;
		temperature *= 256;
		temperature += data[3];
		temperature /= 10;
		if (data[2] & 0x80)
			temperature *= -1;
		return temperature;
	}
}

static inline void delay_ms(int sleep) {
	os_delay_us(1000 * sleep);
}

static inline void delay_us(int sleep){
	os_delay_us(sleep);
}

static struct sensor_reading reading = { .source = "DHT11", .success = 0 };

struct sensor_reading *ICACHE_FLASH_ATTR readDHT(void) {
	return &reading;
}

void inline dhtPinSetLevel(uint8 state) {
	GPIO_OUTPUT_SET(DHT_PIN, state);
}

void inline dhtPinSetOutputMode(uint8 output) {
	if (output) {

	} else {
		GPIO_DIS_OUTPUT(DHT_PIN);
	}
}
uint8 inline dhtPinGetLevel() {
	return GPIO_INPUT_GET(DHT_PIN);
}

void dhtPinInit() {
	// Set GPIO12 to output mode for DHT22
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12);
	// enable pullup
	PIN_PULLUP_EN(PERIPHS_IO_MUX_MTDI_U);
}

/** 
 Originally from: http://harizanov.com/2014/11/esp8266-powered-web-server-led-control-dht22-temperaturehumidity-sensor-reading/
 Adapted from: https://github.com/adafruit/Adafruit_Python_DHT/blob/master/source/Raspberry_Pi/pi_dht_read.c
 LICENSE:
 // Copyright (c) 2014 Adafruit Industries
 // Author: Tony DiCola

 // Permission is hereby granted, free of charge, to any person obtaining a copy
 // of this software and associated documentation files (the "Software"), to deal
 // in the Software without restriction, including without limitation the rights
 // to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 // copies of the Software, and to permit persons to whom the Software is
 // furnished to do so, subject to the following conditions:

 // The above copyright notice and this permission notice shall be included in all
 // copies or substantial portions of the Software.

 // THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 // IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 // FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 // AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 // LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 // OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 // SOFTWARE.
 */
static void ICACHE_FLASH_ATTR pollDHTCb(void * arg) {
	int counter = 0;
	int laststate = 1;
	int i = 0;
	int bits_in = 0;

	int data[100];

	data[0] = data[1] = data[2] = data[3] = data[4] = 0;

	// Wake up device, 500 of high
	dhtPinSetLevel(1);
	delay_ms(500);

	// Hold low for 20ms
	dhtPinSetLevel(0);
	delay_ms(20);

	// High for 40ms
	// GPIO_OUTPUT_SET(2, 1);

	dhtPinSetOutputMode(0);
	delay_us(40);

	// Set pin to input with pullup
	// PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO2_U);

	// os_printf("Waiting for gpio2 to drop \n");

	// wait for pin to drop?
	while (dhtPinGetLevel() && i < DHT_MAXCOUNT) {
		if (i >= DHT_MAXCOUNT) {
			os_printf("[DHT] Failed to get reading, dying\n");
			reading.success = 0;
			return;
		}
		i++;
	}


	// read data!
	for (i = 0; i < MAXTIMINGS; i++) {
		// Count high time (in approx us)
		counter = 0;
		while (dhtPinGetLevel() == laststate) {
			counter++;
			delay_us(1);
			if (counter == 1000)
				break;
		}
		laststate = dhtPinGetLevel();

		if (counter == 1000)
			break;

		// store data after 3 reads
		if ((i > 3) && (i % 2 == 0)) {
			// shove each bit into the storage bytes
			data[bits_in / 8] <<= 1;
			if (counter > BREAKTIME) {
				if (debug) {
					os_printf("1");
				}
				data[bits_in / 8] |= 1;
			} else {
				if (debug) {
					os_printf("0");
				}
			}
			bits_in++;
		}
	}

	if (bits_in < 40) {
		os_printf("[DHT] Got too few bits: %d should be at least 40\n\r",
				bits_in);
		os_printf("[DHT] Failed to get reading, dying\n");
		reading.success = 0;
	}

	int checksum = (data[0] + data[1] + data[2] + data[3]) & 0xFF;
	if(debug){
	  os_printf("[DHT] DHT: %02x %02x %02x %02x [%02x] CS: %02x\n", data[0], data[1],data[2],data[3],data[4],checksum);
	}
	if (data[4] != checksum) {
		os_printf(
				"[DHT] Checksum was incorrect after %d bits. Expected %d but got %d\n",
				bits_in, data[4], checksum);
		os_printf("[DHT] Failed to get reading, dying\n");
		reading.success = 0;
	}

	reading.temperature = scale_temperature(data);
	reading.humidity = scale_humidity(data);
	os_printf("[DHT] Temp =  %d.%d *C, Hum = %d.%d %%\n", (int)(reading.temperature), (int)(reading.temperature * 100)%100,
			(int) (reading.humidity), (int)(reading.humidity * 100)%100);

	reading.success = 1;
	return;
}

void DHTInit(enum sensor_type sensor_type, uint32_t polltime) {
	SENSOR = sensor_type;

	dhtPinInit();

	os_printf("[DHT] DHT Setup for type %d, poll interval of %d\n", sensor_type,
			(int) polltime);

	static ETSTimer dhtTimer;
	os_timer_setfn(&dhtTimer, pollDHTCb, NULL);
	os_timer_arm(&dhtTimer, polltime, 1);
}
