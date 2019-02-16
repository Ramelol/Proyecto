#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <irq.h>
#include <uart.h>
#include <console.h>
#include "sdcard.h"
#include <generated/csr.h>



static void busy_wait(unsigned int ds)
{
	timer0_en_write(0);
	timer0_reload_write(0);
	timer0_load_write(SYSTEM_CLOCK_FREQUENCY/10*ds);
	timer0_en_write(1);
	timer0_update_value_write(1);
	while(timer0_value_read()) timer0_update_value_write(1);
}


static char *readstr(void)
{
	char c[2];
	static char s[64];
	static int ptr = 0;

	if(readchar_nonblock()) {
		c[0] = readchar();
		c[1] = 0;
		switch(c[0]) {
			case 0x7f:
			case 0x08:
				if(ptr > 0) {
					ptr--;
					putsnonl("\x08 \x08");
				}
				break;
			case 0x07:
				break;
			case '\r':
			case '\n':
				s[ptr] = 0x00;
				putsnonl("\n");
				ptr = 0;
				return s;
			default:
				if(ptr >= (sizeof(s) - 1))
					break;
				putsnonl(c);
				s[ptr] = c[0];
				ptr++;
				break;
		}
	}

	return NULL;
}

static char *get_token(char **str)
{
	char *c, *d;

	c = (char *)strchr(*str, ' ');
	if(c == NULL) {
		d = *str;
		*str = *str+strlen(*str);
		return d;
	}
	*c = 0;
	d = *str;
	*str = c+1;
	return d;
}

static void prompt(void)
{
	printf("RUNTIME>");
}

static void help(void)
{
	puts("Available commands:");
	puts("help                            - this command");
	puts("reboot                          - reboot CPU");
	puts("display                         - display test");
	puts("led                             - led test");
	puts("switch                          - switch test");
	puts("sdclk <freq>   - SDCard set clk frequency (Mhz)");
	puts("knight                          - knight Rider");
	puts("sdInit       - SDCard initialization");
	puts("sdtest <loops> - SDCard test");
}

static void knight_rider(void)
{
	int i;
	printf("knightRider_test...\n");
	for(i=1; i<=32768; i=i*2) {
		leds_out_write(i);
		busy_wait(1);
	}
	for(i=32768; i>=1; i=i/2) {
		leds_out_write(i);
		busy_wait(1);
	}





}
static void reboot(void)
{
	asm("call r0");
}

static void display_test(void)
{
	int i;
	printf("display_test...\n");
	for(i=0; i<6; i++) {
		display_sel_write(i);
		display_value_write(i);
		display_write_write(1);
	}
}

static void led_test(void)
{
	int i;
	printf("led_test...\n");
	for(i=0; i<32; i++) {
		leds_out_write(i);
		busy_wait(10);
	}
}
static void switch_test(void)
{
	int i;
	printf("switch_test...\n");
	for(i=0;i<30;i++){
		unsigned short int s = switches_in_read();
		leds_out_write(s);
		busy_wait(10);
	}
	
}


static void console_service(void)
{
	char *str;
	char *token;

	str = readstr();
	if(str == NULL) return;
	token = get_token(&str);
	if(strcmp(token, "help") == 0)
		help();
	else if(strcmp(token, "reboot") == 0)
		reboot();
	else if(strcmp(token, "display") == 0)
		display_test();
	else if(strcmp(token, "led") == 0)
		led_test();
	else if(strcmp(token, "switch") == 0)
		switch_test();
	else if(strcmp(token, "knight") == 0)
		 knight_rider();
	else if(strcmp(token, "sdclk") == 0) {
		token = get_token(&str);
		sdclk_set_clk(atoi(token));
	}
	else if(strcmp(token, "sdInit") == 0)
		switch_test();
	else if(strcmp(token, "sdTest") == 0){
		token = get_token(&str);
		sdcard_test(atoi(token));
	}
	prompt();
}
/**
void adxl362_configure(void){
	int  c = 269484032;
	adxl362_config_write(c);
}



static void adxl362_write(unsigned char addr, unsigned char value){

}


static unsigned char adxl362_read(unsigned char addr){

}**/

int main(void)
{
	irq_setmask(0);
	irq_setie(1);
	uart_init();

	puts("\nLab004 - CPU testing software built "__DATE__" "__TIME__"\n");
	help();
	prompt();

	while(1) {
		console_service();
	}

	return 0;
}
