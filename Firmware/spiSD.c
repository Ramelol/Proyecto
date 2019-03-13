#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <generated/csr.h>
#include <generated/mem.h>
#include <hw/flags.h>
#include <system.h>

#include "spiSD.h"
#include "spiLCD.h"

void delaySD (unsigned int time){

	timer0_en_write(0);
	timer0_reload_write(0);
	timer0_load_write(SYSTEM_CLOCK_FREQUENCY/10*time);
	timer0_en_write(1);
	timer0_update_value_write(1);
	while(timer0_value_read()) timer0_update_value_write(1);

}



void spiSDInit(void){

	//Configuración SPI
	unsigned config = 0*OFFLINE;
        config |= 0*CS_POLARITY | 0*CLK_POLARITY | 0*CLK_PHASE;
        config |= 0*LSB_FIRST | 0*HALF_DUPLEX;
        config |= 248*DIV_READ | 248*DIV_WRITE; 
	sdcard_spi_xfer_write( 0b1 | 8*WRITE_LENGTH);	
        sdcard_spi_config_write(config);
	printf("Configuración de SPI finalizada: %x\n", config );
}


void ponerModoSpi(void){


	//Envío de bytes ficticios:

	for(int i=0;i<75;i++){
		escribirSpi(0xFF);
	}

}


void escribirSpi(uint8_t data){


	printf("Escrito: %x \n", data);
	sdcard_spi_mosi_data_write(data<<24);
	sdcard_spi_start_write(1);
	while(sdcard_spi_active_read());
	uint8_t dat = sdcard_spi_miso_data_read();
        //printf("Respuesta: %x \n", dat);
			

}

uint8_t leerSpi(void){
	//uint8_t data = sdcard_spi_miso_data_read();
	//sdcard_spi_start_write(1);
	//while(sdcard_spi_active_read()){}
	return sdcard_spi_miso_data_read();
}


void sdInit(void){
	spiSDInit();

	sdcard_v_out_write(0b10);

		
	ponerModoSpi();
		

	uint8_t resp =0;

	//CMD0

	sdcard_v_out_write(0b00);



	escribirSpi(0xFF);
	escribirSpi(0x40);
	escribirSpi(0x00);
	escribirSpi(0x00);
	escribirSpi(0x00);
	escribirSpi(0x00);
       	escribirSpi(0x95);
	escribirSpi(0xFF);
       	escribirSpi(0xFF);
	//escribirSpi(0xFF);
       	//escribirSpi(0xFF);
	
	resp = leerSpi();
	
	printf("Respuesta CMD0: %x \n", resp);

	

	//CMD8
        
	escribirSpi(0x48);
	escribirSpi(0x00);
	escribirSpi(0x00);
	escribirSpi(0x01);
	escribirSpi(0xAA);
	escribirSpi(0x87);
	escribirSpi(0xFF);
       	escribirSpi(0xFF);

	resp = leerSpi();

	printf("Respuesta CMD8: %x \n", resp);

	if(resp == 0x01){
		printf("SD version 2 \n");
	}else if(resp == 0x05){
		printf("SD version 1 \n");
	}


	//ACMD41

        escribirSpi(0x69);   
        escribirSpi(0x00);    
        escribirSpi(0x00);
        escribirSpi(0x00);
        escribirSpi(0x00);
        escribirSpi(0xFF);
	escribirSpi(0xFF);
 	escribirSpi(0xFF);

	resp = leerSpi();

	printf("Respuesta ACMD41: %x \n", resp);


	//CMD1

	int k = 0;
	
	do{	
        	escribirSpi(0x41); 
        	escribirSpi(0x00);    
        	escribirSpi(0x00);
        	escribirSpi(0x00);
        	escribirSpi(0x00);
        	escribirSpi(0xFF);
		escribirSpi(0xFF);
       		escribirSpi(0xFF);

		resp = leerSpi();
 		k++;
	
		printf("Respuesta CMD1: %x \n", resp);
	}while(resp != 0x00);
	printf("Número de iteraciones: %i \n", k);
	printf("SD card inicializada \n");

	//CMD16
        
	escribirSpi(0x50);
	escribirSpi(0x00);
	escribirSpi(0x00);
	escribirSpi(0x00);
	escribirSpi(0x01);
	escribirSpi(0xFF);
	escribirSpi(0xFF);
       	escribirSpi(0xFF);

	resp = leerSpi();

	printf("Respuesta CMD16: %x \n", resp);
}

uint8_t leerBloque (uint32_t direccion){
	uint8_t resp = 0;
	uint8_t data = 0;
	//CMD17
        
	escribirSpi(0x51);
	escribirSpi(0x00);
	escribirSpi(0x00);
	escribirSpi(0x02);
	escribirSpi(0x00);
	escribirSpi(0xFF);

	do{
		escribirSpi(0xFF);
       		escribirSpi(0xFF);
		resp = leerSpi();
		printf("Respuesta 1  CMD17: %x \n", resp);

	}while(resp != 0x00);
	do{
		escribirSpi(0xFF);
       		escribirSpi(0xFF);
		resp = leerSpi();
		printf("Respuesta 2  CMD17: %x \n", resp);

	}while(resp != 0xfe);

	do{
		escribirSpi(0xFF);
       		escribirSpi(0xFF);
		resp = leerSpi();
		printf("Dato: %x \n", resp);

	}while(resp != 0xff);
	data = leerSpi();
	printf("Dato leido: %x \n", data);
}

void escribirBloque(uint8_t data, uint32_t direccion){

	uint8_t resp = 0;	
	//CMD24    
	escribirSpi(0x58);
	escribirSpi(0x00);
	escribirSpi(0x00);
	escribirSpi(0x02);
	escribirSpi(0x00);
	escribirSpi(0xFF);

	do{
		escribirSpi(0xFF);
       		escribirSpi(0xFF);
		resp = leerSpi();
		printf("Respuesta 1 CMD24: %x \n", resp);

	}while(resp != 0x00);
	escribirSpi(0xFE);
	escribirSpi(0xAA);
	escribirSpi(0xFF);
        escribirSpi(0xFF);
	do{
		escribirSpi(0xFF);
       		escribirSpi(0xFF);
		resp = leerSpi();
		printf("Respuesta 2 CMD24: %x \n", resp);

	}while(resp != 0xfF);



}







