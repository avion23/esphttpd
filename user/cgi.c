/*
 Some random cgi routines. Used in the LED example and the page that returns the entire
 flash as a binary. Also handles the hit counter on the main page.
 */

/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Jeroen Domburg <jeroen@spritesmods.com> wrote this file. As long as you retain 
 * this notice you can do whatever you want with this stuff. If we meet some day, 
 * and you think this stuff is worth it, you can buy me a beer in return. 
 * ----------------------------------------------------------------------------
 */

#include <string.h>
#include <osapi.h>
#include "user_interface.h"
#include "mem.h"
#include "httpd.h"
#include "cgi.h"
#include "io.h"
#include <ip_addr.h>
#include "espmissingincludes.h"
#include "dht.h"
#include "ds18b20.h"
#include "adc.h"

#define PWM_0_OUT_IO_MUX  PERIPHS_IO_MUX_GPIO2_U
#define PWM_0_OUT_IO_NUM  2
#define PWM_0_OUT_IO_FUNC FUNC_GPIO2
#define PWM_CHANNEL 1

//cause I can't be bothered to write an ioGetLed()
static char currLedState = 0;

//Cgi that turns the LED on or off according to the 'led' param in the POST data
int ICACHE_FLASH_ATTR cgiLed(HttpdConnData *connData) {
	int len;
	char buff[1024];

	if (connData->conn == NULL) {
		//Connection aborted. Clean up.
		return (HTTPD_CGI_DONE);
	}

	len = httpdFindArg(connData->postBuff, "led0", buff, sizeof(buff));
	if (len != 0) {
		currLedState = atoi(buff);
		ioLed(currLedState);
	}

	httpdRedirect(connData, "led.tpl");
	return (HTTPD_CGI_DONE);
}

// cgi to restart the device
int ICACHE_FLASH_ATTR cgiRestart(HttpdConnData *connData) {
	int len;
	char buff[1024];

	if (connData->conn == NULL) {
		//Connection aborted. Clean up.
		return (HTTPD_CGI_DONE);
	}

	len = httpdFindArg(connData->postBuff, "restart_button", buff,
			sizeof(buff));
	if (len != 0) {
		if (os_strcmp(buff, "restart now") == 0) {
			system_restart();
		}
	}

	return ((HTTPD_CGI_DONE));
}

//Template code for the led page.
void ICACHE_FLASH_ATTR tplLed(HttpdConnData *connData, char *token, void **arg) {
	char buff[128];
	if (token == NULL)
		return;

	os_strcpy(buff, "Unknown");
	if (os_strcmp(token, "stateLed0") == 0) {
		if (currLedState) {
			os_strcpy(buff, "on");
		} else {
			os_strcpy(buff, "off");
		}
	}
	if (os_strcmp(token, "led0_pin") == 0) {
		os_sprintf(buff, "%d", LEDGPIO0);
	}

	httpdSend(connData, buff, -1);
}

static long hitCounter = 0;

//Template code for the counter on the index page.
void ICACHE_FLASH_ATTR tplCounter(HttpdConnData *connData, char *token,
		void **arg) {
	char buff[128];
	if (token == NULL)
		return;

	if (os_strcmp(token, "counter") == 0) {
		hitCounter++;
		os_sprintf(buff, "%ld", hitCounter);
	}
	httpdSend(connData, buff, -1);
}

//Cgi that reads the SPI flash. Assumes 512KByte flash.
int ICACHE_FLASH_ATTR cgiReadFlash(HttpdConnData *connData) {
	int *pos = (int *) &connData->cgiData;
	if (connData->conn == NULL) {
		//Connection aborted. Clean up.
		return (HTTPD_CGI_DONE);
	}

	if (*pos == 0) {
		os_printf("Start flash download.\n");
		httpdStartResponse(connData, 200);
		httpdHeader(connData, "Content-Type", "application/bin");
		httpdEndHeaders(connData);
		*pos = 0x40200000;
		return (HTTPD_CGI_MORE);
	}
	//Send 1K of flash per call. We will get called again if we haven't sent 512K yet.
	espconn_sent(connData->conn, (uint8 *) (*pos), 1024);
	*pos += 1024;
	if (*pos >= 0x40200000 + (512 * 1024))
		return (HTTPD_CGI_DONE);
	else
		return (HTTPD_CGI_MORE);
}

void ICACHE_FLASH_ATTR tplDHT(HttpdConnData *connData, char *token, void **arg) {
	char buff[128];
	if (token == NULL)
		return;

	struct sensor_reading* r = readDHT();
	float lastTemp = r->temperature;
	float lastHum = r->humidity;

	os_strcpy(buff, "Unknown");
	if (os_strcmp(token, "temperature") == 0) {
		os_sprintf(buff, "%d.%d", (int) (lastTemp),
				(int) ((lastTemp - (int) lastTemp) * 100));
	}
	if (os_strcmp(token, "humidity") == 0) {
		os_sprintf(buff, "%d.%d", (int) (lastHum),
				(int) ((lastHum - (int) lastHum) * 100));
	}
	if (os_strcmp(token, "sensor_present") == 0) {
		os_sprintf(buff, r->success ? "present" : "not present");
	}
	if (os_strcmp(token, "dht_pin") == 0) {
		os_sprintf(buff, "%d", DHT_PIN);
	}
	r = readDS18B20();
	lastTemp = r->temperature;
	if (os_strcmp(token, "ds18b20_temperature") == 0) {
		if (r->success) {
			os_sprintf(buff, "%d.%d", (int) (lastTemp),
					(int) ((lastTemp - (int) lastTemp) * 100));
		} else {
			os_sprintf(buff, "NA");
		}
	}
	if (os_strcmp(token, "ds18b20_pin") == 0) {
		os_sprintf(buff, "%d", DS1820_PIN);
	}

	httpdSend(connData, buff, -1);
}


// read out the adc
void ICACHE_FLASH_ATTR tplAdc(HttpdConnData *connData, char *token,
		void **arg) {
	char buff[128];
	if (token == NULL)
		return;

	if (os_strcmp(token, "adc") == 0) {
		uint16 adcValue = getAdcValue();
		os_sprintf(buff, "%d", adcValue);
	}
	httpdSend(connData, buff, -1);
}
