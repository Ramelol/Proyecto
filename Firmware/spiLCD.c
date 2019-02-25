#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <generated/csr.h>
#include <generated/mem.h>
#include <hw/flags.h>
#include <system.h>

#include "spiLCD.h"
#include "spiLCD.h"


void spiInit(void){

	//pantalla_spi_config_write(0x11110000);  //div_write div_read 00 b 0x11110000
	//pantalla_spi_xfer_write(0x100001);// write_length*24 | 0b1    0x100001
	//printf("Configuración de SPI finalizada: %i\n", pantalla_spi_config_read() );

	//Configuración SPI
	unsigned config = 0*OFFLINE;
        config |= 0*CS_POLARITY | 0*CLK_POLARITY | 0*CLK_PHASE;
        config |= 0*LSB_FIRST | 0*HALF_DUPLEX;
        config |= 16*DIV_READ | 8*DIV_WRITE;
	pantalla_spi_xfer_write( 0b1 | 16*WRITE_LENGTH);	
        pantalla_spi_config_write(config);
	printf("Configuración de SPI finalizada: %x\n", config );
}

void escribirLCD(  unsigned int rs, unsigned int data){
	if(rs==0){
		pantalla_control_out_write(0b101);
	}else{
		pantalla_control_out_write(0b111);
	}
	pantalla_spi_mosi_data_write(data<<16);
	pantalla_spi_start_write(1);
	while(pantalla_spi_active_read()){}
	 /*
	spiInit();
	unsigned int val = data;
	pantalla_spi_xfer_write( 0b1 | 16*WRITE_LENGTH);
	pantalla_spi_mosi_data_write(val<<16);
	printf( "Datos mosi: %x\n", pantalla_spi_mosi_data_read());
	pantalla_spi_start_write(1);
	while( pantalla_spi_active_read()  & 0x1 );*/

	/*if(rs==0){
		pantalla_control_out_write(0b101);
	}else{
		pantalla_control_out_write(0b111);
	}
	spiInit();
	pantalla_spi_xfer_write( 0b1 | 16*WRITE_LENGTH);
	pantalla_spi_mosi_data_write(data<<16);
	pantalla_spi_start_write(1);
	while(pantalla_spi_active_read()){
	};*/
}


void delay (unsigned int time){

	timer0_en_write(0);
	timer0_reload_write(0);
	timer0_load_write(SYSTEM_CLOCK_FREQUENCY/10*time);
	timer0_en_write(1);
	timer0_update_value_write(1);
	while(timer0_value_read()) timer0_update_value_write(1);

}

void dibujarPunto(unsigned int x, unsigned int  y, uint16_t color){

	escribirLCD(0,RAM_ADDR_SET1);
	escribirLCD(1,x);
	escribirLCD(0,RAM_ADDR_SET2);
	escribirLCD(1,y);
	escribirLCD(0,GRAM_DATA_REG);
	escribirLCD(1,color);
}

void dibujarImagen(uint16_t* imagen , int tam){
	int contador = 0;
	for(int i=0; i<176; i++){
		for(int j=0;j<220;j++){
			if(contador < tam){
				dibujarPunto(i,j,imagen[contador]);
				printf("Color: %x\n", imagen[contador]);
				contador++;
			}else{
				dibujarPunto(i,j,BLANCO);
			}
		}	
	}
}

void colorFondo(uint16_t color){
	for(int i=0; i<176; i++){
		for(int j=0;j<220;j++){
			dibujarPunto(i,j,color);
		}	
	}
};

void dibujarTile(uint16_t* imagen, int x, int y){
	int contador = 0;
	for(int i=x; i<x+20; i++){
		for(int j=y;j<y+18;j++){
			dibujarPunto(i,j,imagen[contador]);
			contador++;	
		}	
	}
}

void dibujarTileColor(uint16_t color, int x, int y){
	for(int i=x; i<x+20; i++){
		for(int j=y;j<y+18;j++){
			dibujarPunto(i,j,color);	
		}	
	}
}


void lcd_init(void){

	
	
	//Inicialización de la Pantalla
	escribirLCD(0,0x0000);
	escribirLCD(0,POWER_CTRL1);
	escribirLCD(1,0x0000);
	escribirLCD(0,POWER_CTRL2);
	escribirLCD(1,0x0000);
	escribirLCD(0,POWER_CTRL3);
	escribirLCD(1,0x0000);
	escribirLCD(0,POWER_CTRL4);
	escribirLCD(1,0x0000);
	escribirLCD(0,POWER_CTRL5);
	escribirLCD(1,0x0000);

	delay(0.4);

	//Power-on sequence
	escribirLCD(0,POWER_CTRL2);
	escribirLCD(1,0x0018);
	escribirLCD(0,POWER_CTRL3);
	escribirLCD(1,0x6121);
	escribirLCD(0,POWER_CTRL4);
	escribirLCD(1,0x006f);
	escribirLCD(0,POWER_CTRL5);
	escribirLCD(1,0x495f);
	escribirLCD(0,POWER_CTRL1);
	escribirLCD(1,0x0800);

	delay(0.1);
	
	escribirLCD(0,POWER_CTRL2);
	escribirLCD(1,0x103b);
	
	delay(0.5);

	escribirLCD(0,DRIVER_OUTPUT_CTRL);
	escribirLCD(1,0x011c);
	escribirLCD(0,LCD_AC_DRIVING_CTRL);
	escribirLCD(1,0x0100);
	escribirLCD(0,ENTRY_MODE);
	escribirLCD(1,0x1038);
	escribirLCD(0,DISP_CTRL1);
	escribirLCD(1,0x0000);
	escribirLCD(0,BLANK_PERIOD_CTRL1);
	escribirLCD(1,0x0808);
	escribirLCD(0,FRAME_CYCLE_CTRL);
	escribirLCD(1,0x1100);
	escribirLCD(0,INTERFACE_CTRL);
	escribirLCD(1,0x0000);
	escribirLCD(0,OSC_CTRL);
	escribirLCD(1,0x0d01);
	escribirLCD(0,VCI_RECYCLING);
	escribirLCD(1,0x0020);
	escribirLCD(0,RAM_ADDR_SET1);
	escribirLCD(1,0x0000);	
	escribirLCD(0,RAM_ADDR_SET2);
	escribirLCD(1,0x0000);

	escribirLCD(0,GATE_SCAN_CTRL);
	escribirLCD(1,0x0000);	
	escribirLCD(0,VERTICAL_SCROLL_CTRL1);
	escribirLCD(1,0x00db);
	escribirLCD(0,VERTICAL_SCROLL_CTRL2);
	escribirLCD(1,0x0000);
	escribirLCD(0,VERTICAL_SCROLL_CTRL3);
	escribirLCD(1,0x0000);
	escribirLCD(0,PARTIAL_DRIVING_POS1);
	escribirLCD(1,0x00db);
	escribirLCD(0,PARTIAL_DRIVING_POS2);
	escribirLCD(1,0x0000);
	escribirLCD(0,HORIZONTAL_WINDOW_ADDR1);
	escribirLCD(1,0x00af);
	escribirLCD(0,HORIZONTAL_WINDOW_ADDR2);
	escribirLCD(1,0x0000);
	escribirLCD(0,VERTICAL_WINDOW_ADDR1);
	escribirLCD(1,0x00db);
	escribirLCD(0,VERTICAL_WINDOW_ADDR2);
	escribirLCD(1,0x0000);

	escribirLCD(0,GAMMA_CTRL1);
	escribirLCD(1,0x0000);
	escribirLCD(0,GAMMA_CTRL2);
	escribirLCD(1,0x0808);
	escribirLCD(0,GAMMA_CTRL3);
	escribirLCD(1,0x080a);
	escribirLCD(0,GAMMA_CTRL4);
	escribirLCD(1,0x000a);
	escribirLCD(0,GAMMA_CTRL5);
	escribirLCD(1,0x0a08);
	escribirLCD(0,GAMMA_CTRL6);
	escribirLCD(1,0x0808);
	escribirLCD(0,GAMMA_CTRL7);
	escribirLCD(1,0x0000);
	escribirLCD(0,GAMMA_CTRL8);
	escribirLCD(1,0x0a00);
	escribirLCD(0,GAMMA_CTRL9);
	escribirLCD(1,0x0710);
	escribirLCD(0,GAMMA_CTRL10);
	escribirLCD(1,0x0710);

	escribirLCD(0,DISP_CTRL1);
	escribirLCD(1,0x0012);

	delay(0.5);

	escribirLCD(0,DISP_CTRL1);
	escribirLCD(1,0x1017);

	escribirLCD(0,ENTRY_MODE); 
	escribirLCD(1,0x1038);


	escribirLCD(0,HORIZONTAL_WINDOW_ADDR1);
	escribirLCD(1,0x00AF);

	escribirLCD(0,HORIZONTAL_WINDOW_ADDR2); 
	escribirLCD(1,0x0000);

	escribirLCD(0,VERTICAL_WINDOW_ADDR1 ); 
	escribirLCD(1,0x00DB);

	escribirLCD(0,VERTICAL_WINDOW_ADDR2);
	escribirLCD(1,0x0000);

	escribirLCD(0,RAM_ADDR_SET1); 
	escribirLCD(1,0x0000);

	escribirLCD(0,RAM_ADDR_SET2); 
	escribirLCD(1,0x0000);


	escribirLCD(0,GRAM_DATA_REG);


	
	
};
