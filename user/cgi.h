#ifndef CGI_H

#define CGI_H

#include "httpd.h"

int cgiLed(HttpdConnData *connData);
void tplLed(HttpdConnData *connData, char *token, void **arg);
int cgiReadFlash(HttpdConnData *connData);
void tplCounter(HttpdConnData *connData, char *token, void **arg);
void tplDHT(HttpdConnData *connData, char *token, void **arg);
// cgi to restart the device
int ICACHE_FLASH_ATTR cgiRestart(HttpdConnData *connData);
void ICACHE_FLASH_ATTR tplAdc(HttpdConnData *connData, char *token,
		void **arg);
#endif
