#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <irq.h>
#include <uart.h>
#include <console.h>
#include "spiLCD.h"
#include "spiSD.h"
#include <generated/csr.h>


/*static uint16_t prueba[] = { ORO, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, ROJO, AZULOSCURO, ROJO, GRIS, ROJO, ROJO, INDIGO,ORO,ROJO, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, ROJO, AZULOSCURO, ROJO, GRIS, ROJO, ROJO, INDIGO,ORO,ROJO, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, ROJO, AZULOSCURO, ROJO, GRIS, ROJO, ROJO, INDIGO,ORO,ROJO, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, ROJO, AZULOSCURO, ROJO, GRIS, ROJO, ROJO, INDIGO,ORO,ROJO, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, ROJO, AZULOSCURO, ROJO, GRIS, ROJO, ROJO, INDIGO,ORO,ROJO, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, ROJO, AZULOSCURO, ROJO, GRIS, ROJO, ROJO, INDIGO,ORO,ROJO, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, ROJO, AZULOSCURO, ROJO, GRIS, ROJO, ROJO, INDIGO,ORO,ROJO, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, ROJO, AZULOSCURO, ROJO, GRIS, ROJO, ROJO, INDIGO,ORO,ROJO, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, BLANCO, AZULOSCURO, ROJO, GRIS, ROJO, ROJO, INDIGO,ORO,ROJO, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, ROJO, AZULOSCURO, ROJO, GRIS, ROJO, ROJO, INDIGO,ORO,ROJO, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, ROJO, AZULOSCURO, ROJO, GRIS, ROJO, ROJO, INDIGO,ORO,ROJO, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, ROJO, AZULOSCURO, ROJO, GRIS, ROJO, ROJO, INDIGO,ORO,ROJO, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, ROJO, AZULOSCURO, ROJO, GRIS, ROJO, ROJO, INDIGO,ORO,ROJO, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, ROJO, AZULOSCURO, ROJO, GRIS, ROJO, ROJO, INDIGO,ORO,ROJO, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, ROJO, AZULOSCURO, ROJO, GRIS, ROJO, ROJO, INDIGO,ORO,ROJO, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, ROJO, AZULOSCURO, ROJO, GRIS, ROJO, PLATA, INDIGO,ORO,ROJO, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, ROJO, AZULOSCURO, ROJO, GRIS, ROJO, ROJO, INDIGO,ORO,PLATA, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, ROJO, AZULOSCURO, ROJO, GRIS, ROJO, ROJO, INDIGO,ORO,ROJO, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, ROJO, AZULOSCURO, ROJO, GRIS, ROJO, ROJO, INDIGO,ORO,ROJO, ORO, AZULOSCURO, GRIS,INDIGO, ROJO, GRIS, ROJO, AZUL, ORO, ROJO, AZULOSCURO, ROJO, GRIS, ROJO, ROJO, INDIGO,ORO
};*/

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
	puts("sdtest   - SDCard test");
	puts("lcdtest  -  lcdtest");

}

static void reboot(void)
{
	asm("call r0");
}

static void lcd_test(void)
{
	int i;
	printf("lcd_test...\n");
	for(i=0;i<30;i++){
		unsigned short int s = buttons_in_read();
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
			x=0;		
		}
		if(y<0){
			y=0;
		}
		if(e == 1){
			dibujarTileColor(fondo,xAnt,yAnt);
			dibujarTileColor(ORO,x,y);
		}
		busy_wait(2);
		e = 0;
	}
	lcd_test();

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
	else if(strcmp(token, "sdtest") == 0){
		spiSDInit();
		sdInit();
		leerBloque (0x00000000);
		//escribirBloque( 0x00,0x00000000);		
		//leerBloque (0x00000000);
	}else if(strcmp(token, "lcdtest") == 0){
		spiInit();
		lcd_init();
		colorFondo(AZULOSCURO);
		fondo = AZULOSCURO;
		dibujarTileColor(ORO,0,0);
		test_LCD2();

	}
	prompt();
}


int main(void)
{
	irq_setmask(0);
	irq_setie(1);
	uart_init();

	puts("\n- CPU testing software built "__DATE__" "__TIME__"\n");
	help();
	prompt();

	while(1) {
		console_service();
	}

	return 0;
}
