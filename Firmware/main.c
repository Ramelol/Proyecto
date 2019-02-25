#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <irq.h>
#include <uart.h>
#include <console.h>
#include "sdcard.h"
#include "spiLCD.h"
#include "spiSD.h"
#include <generated/csr.h>


static uint16_t prueba[] = { ORO, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, ROJO, AZULOSCURO, ROJO, GRIS, ROJO, ROJO, INDIGO,ORO,ROJO, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, ROJO, AZULOSCURO, ROJO, GRIS, ROJO, ROJO, INDIGO,ORO,ROJO, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, ROJO, AZULOSCURO, ROJO, GRIS, ROJO, ROJO, INDIGO,ORO,ROJO, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, ROJO, AZULOSCURO, ROJO, GRIS, ROJO, ROJO, INDIGO,ORO,ROJO, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, ROJO, AZULOSCURO, ROJO, GRIS, ROJO, ROJO, INDIGO,ORO,ROJO, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, ROJO, AZULOSCURO, ROJO, GRIS, ROJO, ROJO, INDIGO,ORO,ROJO, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, ROJO, AZULOSCURO, ROJO, GRIS, ROJO, ROJO, INDIGO,ORO,ROJO, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, ROJO, AZULOSCURO, ROJO, GRIS, ROJO, ROJO, INDIGO,ORO,ROJO, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, BLANCO, AZULOSCURO, ROJO, GRIS, ROJO, ROJO, INDIGO,ORO,ROJO, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, ROJO, AZULOSCURO, ROJO, GRIS, ROJO, ROJO, INDIGO,ORO,ROJO, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, ROJO, AZULOSCURO, ROJO, GRIS, ROJO, ROJO, INDIGO,ORO,ROJO, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, ROJO, AZULOSCURO, ROJO, GRIS, ROJO, ROJO, INDIGO,ORO,ROJO, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, ROJO, AZULOSCURO, ROJO, GRIS, ROJO, ROJO, INDIGO,ORO,ROJO, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, ROJO, AZULOSCURO, ROJO, GRIS, ROJO, ROJO, INDIGO,ORO,ROJO, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, ROJO, AZULOSCURO, ROJO, GRIS, ROJO, ROJO, INDIGO,ORO,ROJO, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, ROJO, AZULOSCURO, ROJO, GRIS, ROJO, PLATA, INDIGO,ORO,ROJO, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, ROJO, AZULOSCURO, ROJO, GRIS, ROJO, ROJO, INDIGO,ORO,PLATA, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, ROJO, AZULOSCURO, ROJO, GRIS, ROJO, ROJO, INDIGO,ORO,ROJO, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, ROJO, AZULOSCURO, ROJO, GRIS, ROJO, ROJO, INDIGO,ORO,ROJO, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, ROJO, AZULOSCURO, ROJO, GRIS, ROJO, ROJO, INDIGO,ORO
};

static uint16_t fondo = BLANCO;

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
	puts("knight                          - knight Rider");
	puts("sdclk <freq>   - SDCard set clk frequency (Mhz)");	
	puts("sdinit       - SDCard initialization");
	puts("sdtest <loops> - SDCard test");
	puts("lcdtest  -  lcdtest");
}

/*static void knight_rider(void)
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
}*/
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
/*
static void led_test(void)
{
	int i;
	printf("led_test...\n");
	for(i=0; i<32; i++) {
		leds_out_write(i);
		busy_wait(10);
	}
}*/
static void switch_test(void)
{
	int i;
	printf("switch_test...\n");
	for(i=0;i<30;i++){
		unsigned short int s = switches_in_read();
		//leds_out_write(s);

	}
	
}

static void lcd_test(void)
{
	int i;
	printf("lcd_test...\n");
	for(i=0;i<30;i++){
		unsigned short int s = switches_in_read();
		printf("valor: %i\n", s);
		if(s==0){
			colorFondo(AZUL);
		}else if(s==1){
			colorFondo(BLANCO);
		}else if(s==2){
			colorFondo(GRIS);
		}else if(s==3){
			colorFondo(AZULOSCURO);
		}else if(s==4){
			colorFondo(ORO);
		}else{
			colorFondo(BEIGE);
		}
		busy_wait(10);
	}
	
}

static void test_LCD2(void){
	printf("lcd_test...\n");
	int e = 0;
	int x = 0;
	int y = 0;
	for(int i=0;i<1000;i++){
		unsigned short int s = buttons_in_read();
		int xAnt = x;
		int yAnt = y;
		if (s==4){
			x=x+20;		
			e = 1;
		}else if(s==16){
			y=y+18;
			e = 1;
		}else if(s==8){
			x=x-20;
			e = 1;
		}else if(s==2){
			y=y-18;
			e = 1;
		}else if(s==1){
			if(fondo == AZULOSCURO){
				colorFondo(PLATA);
				fondo = PLATA;
			}else{
			        colorFondo(AZULOSCURO);
			        fondo = AZULOSCURO;
			}
			dibujarTileColor(ORO,x,y);
		}	
		if(x<0){
			x==0;		
		}
		if(y<0){
			y==0;
		}
		if(e == 1){
			dibujarTileColor(fondo,xAnt,yAnt);
			dibujarTileColor(ORO,x,y);
		}
		busy_wait(2);
		e = 0;
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
	else if(strcmp(token, "led") == 0){
		//led_test();
	}	
	else if(strcmp(token, "switch") == 0)
		switch_test();
	else if(strcmp(token, "knight") == 0){
		 //knight_rider();
	}
	else if(strcmp(token, "sdclk") == 0) {
		token = get_token(&str);
		//sdclk_set_clk(atoi(token));
	}
	else if(strcmp(token, "sdinit") == 0){
		//sdcard_init();
	}
	else if(strcmp(token, "sdtest") == 0){
		token = get_token(&str);
		int res = sdInit();
		printf("Respuesta: %i\n", res);
		//sdcard_test(atoi(token));
	}
	else if(strcmp(token, "lcdtest") == 0){
		spiInit();
		lcd_init();
		colorFondo(AZULOSCURO);
		fondo = AZULOSCURO;
		dibujarTileColor(ORO,0,0);
		test_LCD2();

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
