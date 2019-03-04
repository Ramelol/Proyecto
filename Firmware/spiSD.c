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
        config |= 0*CS_POLARITY | 1*CLK_POLARITY | 0*CLK_PHASE;
        config |= 0*LSB_FIRST | 1*HALF_DUPLEX;
        config |= 98*DIV_READ | 98*DIV_WRITE;
	sdcard_spi_xfer_write( 0b1 | 32*WRITE_LENGTH);	
        sdcard_spi_config_write(config);
	printf("Configuración de SPI finalizada: %x\n", config );
}


void ponerModoSpi(void){


	//Envío de bytes ficticios:

	for(int i=0;i<75;i++){
		escribirSpi(0x000000FF);
	}

}


void escribirSpi(uint32_t data){

	sdcard_spi_mosi_data_write(data);
	sdcard_spi_start_write(1);
	while(sdcard_spi_active_read()){}
	printf("Escrito: %x \n", sdcard_spi_mosi_data_read());	

}

uint8_t leerSpi(void){
	return sdcard_spi_miso_data_read();
}


void sdInit(void){
	spiSDInit();
	sdcard_v_out_write(0b01);
		
	ponerModoSpi();
		

	uint8_t resp =0;

	//CMD0


	escribirSpi(0xFF);


	escribirSpi(0x00000040);
	escribirSpi(0x00000000);
	escribirSpi(0x00000000);
	escribirSpi(0x00000000);
	escribirSpi(0x00000000);
	escribirSpi(0x00000095);

	escribirSpi(0x000000FF);

	
	do{
	//Esperar por Respuesta
		printf("Respuesta: %x \n", sdcard_spi_miso_data_read());

	}while( sdcard_spi_miso_data_read() != 0x00000001 );
	printf("Respuesta: %x \n", resp);	


	//CMD8

	escribirSpi(0x00000048);
	escribirSpi(0x000001AA);
	escribirSpi(0x000001AA);
	escribirSpi(0x000001AA);
	escribirSpi(0x000001AA);
	escribirSpi(0x00000087);

	resp = leerSpi();

	if(resp == 0x01){
		printf("SD version 2 \n");
	}else if(resp == 0x05){
		printf("SD version 1 \n");
	}
	printf("Respuesta: %x \n", resp);

}




