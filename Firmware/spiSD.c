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
	timer0_load_write(SYSTEM_CLOCK_FREQUENCY/1000*time);
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


	//printf(" ");
	delaySD(1);	
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

//	do{
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
//	}while(resp!=0x01);
		
	printf("Respuesta CMD0: %x \n", resp);

	//CMD8
        //do{
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

	//}while(!(resp==0x01 || resp==0x05));
	if(resp == 0x01){
		printf("SD version 2 \n");
	}else if(resp == 0x05){
		printf("SD version 1 \n");
	}


	//ACMD41
	
	do{

        	escribirSpi(0x69);   
        	escribirSpi(0x40);    
        	escribirSpi(0x00);
        	escribirSpi(0x00);
        	escribirSpi(0x00);
        	escribirSpi(0xFF);
		escribirSpi(0xFF);
 		escribirSpi(0xFF);
	
		resp = leerSpi();
	
		printf("Respuesta ACMD41: %x \n", resp);
	
 	}while( (resp & 0x1)  != 0x00  );	

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
	escribirSpi(0x02);
	escribirSpi(0x00);
	escribirSpi(0xFF);
	escribirSpi(0xFF);
       	escribirSpi(0xFF);

	resp = leerSpi();

	printf("Respuesta CMD16: %x \n", resp);
}
int* p(void){

	int a[3];
	a[0] = 1;
	a[1] = 2;
	a[2] = 3;
	return a ;

};
uint8_t* leerNave1 (void){
	static uint8_t nave1[32];
	uint8_t resp;
	sdcard_v_out_write(0b10);
	//CMD17
	do{
		escribirSpi(0x51);
		escribirSpi(0x00);
		escribirSpi(0x00);
		escribirSpi(0x00);
		escribirSpi(0x02);
		escribirSpi(0xFF);
		escribirSpi(0xFF);
       		escribirSpi(0xFF);
		resp = leerSpi();
		//printf("Respuesta 1  CMD17: %x \n", resp);
	}while(resp != 0x00);
	do{
		escribirSpi(0xFF);
       		escribirSpi(0xFF);
		resp = leerSpi();
		//printf("Respuesta 2  CMD17: %x \n", resp);

	}while(resp != 0xfe);
	//printf("\n-----------\n");
	int k = 0;
	for(int i=0;i<32;i++){
		escribirSpi(0xFF);
       		escribirSpi(0xFF);
		resp = leerSpi();
		nave1[i] = resp;				
		//printf(" %x ", nave1[i][0]);
	}
	escribirSpi(0xFF);
  	escribirSpi(0xFF);
	return nave1;
}


uint8_t* leerNave1_color (void){
	static uint8_t nave1[32];
	uint8_t resp;
	sdcard_v_out_write(0b10);
	//CMD17
	do{
		escribirSpi(0x51);
		escribirSpi(0x00);
		escribirSpi(0x00);
		escribirSpi(0x00);
		escribirSpi(0x02);
		escribirSpi(0xFF);
		escribirSpi(0xFF);
       		escribirSpi(0xFF);
		resp = leerSpi();
		//printf("Respuesta 1  CMD17: %x \n", resp);
	}while(resp != 0x00);
	do{
		escribirSpi(0xFF);
       		escribirSpi(0xFF);
		resp = leerSpi();
		//printf("Respuesta 2  CMD17: %x \n", resp);

	}while(resp != 0xfe);
	//printf("\n-----------\n");
	int k = 0;
	for(int i=0;i<32;i++){
		escribirSpi(0xFF);
       		escribirSpi(0xFF);
		resp = leerSpi();
	}
	for(int i=0;i<32;i++){
		escribirSpi(0xFF);
       		escribirSpi(0xFF);
		resp = leerSpi();
		nave1[i] = resp;				
		//printf(" %x ", nave1[i][0]);
	}
	escribirSpi(0xFF);
  	escribirSpi(0xFF);
	return nave1;
}







uint8_t* leerNombre (void){
	static uint8_t nombre[54];
	uint8_t resp;
	sdcard_v_out_write(0b10);
	//CMD17
	do{
		escribirSpi(0x51);
		escribirSpi(0x00);
		escribirSpi(0x00);
		escribirSpi(0x00);
		escribirSpi(0x01);
		escribirSpi(0xFF);
		escribirSpi(0xFF);
       		escribirSpi(0xFF);
		resp = leerSpi();
		//printf("Respuesta 1  CMD17: %x \n", resp);
	}while(resp != 0x00);
	do{
		escribirSpi(0xFF);
       		escribirSpi(0xFF);
		resp = leerSpi();
		//printf("Respuesta 2  CMD17: %x \n", resp);

	}while(resp != 0xfe);
	printf("\n-------------------------------------------\n");
	for(int i=0;i<54;i++){
		escribirSpi(0xFF);
       		escribirSpi(0xFF);
		resp = leerSpi();
		nombre[i] = resp;
		printf(" %x ", nombre[i]);
	}
	
	escribirSpi(0xFF);
  	escribirSpi(0xFF);
	return nombre;
}
uint8_t leerBloque (void){
	uint8_t resp = 0;
	uint8_t data = 0;
	sdcard_v_out_write(0b10);
	//CMD17
	do{
		escribirSpi(0x51);
		escribirSpi(0x00);
		escribirSpi(0x00);
		escribirSpi(0x00);
		escribirSpi(0x02);
		escribirSpi(0xFF);
		escribirSpi(0xFF);
       		escribirSpi(0xFF);
		resp = leerSpi();
		printf("Respuesta 1  CMD17: %x \n", resp);

	}while(resp != 0x00);
	do{
		escribirSpi(0xFF);
       		escribirSpi(0xFF);
		resp = leerSpi();
		//printf("Respuesta 2  CMD17: %x \n", resp);

	}while(resp != 0xfe);
	//printf("\n-------------------------------------------\n");
	int c = 0;
	do{
		escribirSpi(0xFF);
       		escribirSpi(0xFF);
		resp = leerSpi();
		printf(" %x ", resp);
		c++;

	}while(c != 512);
	data = leerSpi();
	printf("Dato leido: %x \n", data);
	escribirSpi(0xFF);
  	escribirSpi(0xFF);
}

void escribirNombre(void){
	uint8_t resp = 0;	
	//CMD24    
	escribirSpi(0x58);
	escribirSpi(0x00);
	escribirSpi(0x00);
	escribirSpi(0x00);
	escribirSpi(0x01);
	escribirSpi(0xFF);


	do{
		escribirSpi(0xFF);
       		escribirSpi(0xFF);
		resp = leerSpi();
		printf("Respuesta 1 CMD24: %x \n", resp);

	}while(resp != 0x00);
	escribirSpi(0xFE);
	int c = 0;

	//G
	escribirSpi(0xFF);
	escribirSpi(0b00001110);
	escribirSpi(0xFF);
	escribirSpi(0b00010011);
	escribirSpi(0xFF);
	escribirSpi(0b00010001);
	escribirSpi(0xFF);
	escribirSpi(0b00010001);
	escribirSpi(0xFF);
	escribirSpi(0b00010000);
	escribirSpi(0xFF);
	escribirSpi(0b00010111);
	escribirSpi(0xFF);
	escribirSpi(0b00010001);
	escribirSpi(0xFF);
	escribirSpi(0b00010001);
	escribirSpi(0xFF);
	escribirSpi(0b00001110);

	//A
	escribirSpi(0xFF);
	escribirSpi(0b00011111);
	escribirSpi(0xFF);
	escribirSpi(0b00011011);
	escribirSpi(0xFF);	
	escribirSpi(0b00010001);
	escribirSpi(0xFF);
	escribirSpi(0b00010001);
	escribirSpi(0xFF);
	escribirSpi(0b00010001);
	escribirSpi(0xFF);
	escribirSpi(0b00011111);
	escribirSpi(0xFF);
	escribirSpi(0b00010001);
	escribirSpi(0xFF);
	escribirSpi(0b00010001);
	escribirSpi(0xFF);
	escribirSpi(0b00010001);

	//L
	escribirSpi(0xFF);
	escribirSpi(0b00010000);
	escribirSpi(0xFF);
	escribirSpi(0b00010000);
	escribirSpi(0xFF);
	escribirSpi(0b00010000);
	escribirSpi(0xFF);
	escribirSpi(0b00010000);
	escribirSpi(0xFF);
	escribirSpi(0b00010000);
	escribirSpi(0xFF);
	escribirSpi(0b00010000);
	escribirSpi(0xFF);
	escribirSpi(0b00010000);
	escribirSpi(0xFF);
	escribirSpi(0b00010000);
	escribirSpi(0xFF);
	escribirSpi(0b00011111);

	//A
	escribirSpi(0xFF);
	escribirSpi(0b00011111);
	escribirSpi(0xFF);
	escribirSpi(0b00011011);
	escribirSpi(0xFF);	
	escribirSpi(0b00010001);
	escribirSpi(0xFF);
	escribirSpi(0b00010001);
	escribirSpi(0xFF);
	escribirSpi(0b00010001);
	escribirSpi(0xFF);
	escribirSpi(0b00011111);
	escribirSpi(0xFF);
	escribirSpi(0b00010001);
	escribirSpi(0xFF);
	escribirSpi(0b00010001);
	escribirSpi(0xFF);
	escribirSpi(0b00010001);

	//G
	escribirSpi(0xFF);
	escribirSpi(0b00001110);
	escribirSpi(0xFF);
	escribirSpi(0b00010011);
	escribirSpi(0xFF);
	escribirSpi(0b00010001);
	escribirSpi(0xFF);
	escribirSpi(0b00010001);
	escribirSpi(0xFF);
	escribirSpi(0b00010000);
	escribirSpi(0xFF);
	escribirSpi(0b00010111);
	escribirSpi(0xFF);
	escribirSpi(0b00010001);
	escribirSpi(0xFF);
	escribirSpi(0b00010001);
	escribirSpi(0xFF);
	escribirSpi(0b00001110);

	//A
	escribirSpi(0xFF);
	escribirSpi(0b00011111);
	escribirSpi(0xFF);
	escribirSpi(0b00011011);
	escribirSpi(0xFF);	
	escribirSpi(0b00010001);
	escribirSpi(0xFF);
	escribirSpi(0b00010001);
	escribirSpi(0xFF);
	escribirSpi(0b00010001);
	escribirSpi(0xFF);
	escribirSpi(0b00011111);
	escribirSpi(0xFF);
	escribirSpi(0b00010001);
	escribirSpi(0xFF);
	escribirSpi(0b00010001);
	escribirSpi(0xFF);
	escribirSpi(0b00010001);

	do{
       		escribirSpi(0xAA);
		c++;

	}while(c != 506);
	escribirSpi(0xFF);
        escribirSpi(0xFF);
	do{
		escribirSpi(0xFF);
       		escribirSpi(0xFF);
		resp = leerSpi();
		printf("Respuesta 2 CMD24: %x \n", resp);

	}while(resp != 0xFF);

}

void escribirNave1(void){
	uint8_t resp = 0;	
	//CMD24    
	escribirSpi(0x58);
	escribirSpi(0x00);
	escribirSpi(0x00);
	escribirSpi(0x00);
	escribirSpi(0x02);
	escribirSpi(0xFF);


	do{
		escribirSpi(0xFF);
       		escribirSpi(0xFF);
		resp = leerSpi();
		printf("Respuesta 1 CMD24: %x \n", resp);

	}while(resp != 0x00);
	escribirSpi(0xFE);
	int c = 0;
	escribirSpi(0XFF);
	escribirSpi(0b00000000);
	escribirSpi(0XFF);
	escribirSpi(0b10000000);

	escribirSpi(0XFF);
	escribirSpi(0b00000000);
	escribirSpi(0XFF);
	escribirSpi(0b10000000);

	escribirSpi(0XFF);
	escribirSpi(0b00000000);
	escribirSpi(0XFF);
	escribirSpi(0b10000000);

	escribirSpi(0XFF);
	escribirSpi(0b00000001);
	escribirSpi(0XFF);
	escribirSpi(0b11000000);

	escribirSpi(0XFF);
	escribirSpi(0b00000001);
	escribirSpi(0XFF);
	escribirSpi(0b11000000);

	escribirSpi(0XFF);
	escribirSpi(0b00001001);
	escribirSpi(0XFF);
	escribirSpi(0b11001000);

	escribirSpi(0XFF);
	escribirSpi(0b00001001);
	escribirSpi(0XFF);
	escribirSpi(0b11001000);

	escribirSpi(0XFF);
	escribirSpi(0b00001011);
	escribirSpi(0XFF);
	escribirSpi(0b11101000);

	escribirSpi(0XFF);
	escribirSpi(0b01001111);
	escribirSpi(0XFF);
	escribirSpi(0b11111001);

	escribirSpi(0XFF);
	escribirSpi(0b01001111);
	escribirSpi(0XFF);
	escribirSpi(0b11111001);

	escribirSpi(0XFF);
	escribirSpi(0b01001111);
	escribirSpi(0XFF);
	escribirSpi(0b11111001);

	escribirSpi(0XFF);
	escribirSpi(0b01011111);
	escribirSpi(0XFF);
	escribirSpi(0b11111101);

	escribirSpi(0XFF);
	escribirSpi(0b01111111);
	escribirSpi(0XFF);
	escribirSpi(0b11111111);

	escribirSpi(0XFF);
	escribirSpi(0b01110111);
	escribirSpi(0XFF);
	escribirSpi(0b11110111);

	escribirSpi(0XFF);
	escribirSpi(0b01100110);
	escribirSpi(0XFF);
	escribirSpi(0b10110011);

	escribirSpi(0XFF);
	escribirSpi(0b01000000);
	escribirSpi(0XFF);
	escribirSpi(0b10000001);




	escribirSpi(0XFF);
	escribirSpi(0b00000000);
	escribirSpi(0XFF);
	escribirSpi(0b00000000);

	escribirSpi(0XFF);
	escribirSpi(0b00000000);
	escribirSpi(0XFF);
	escribirSpi(0b00000000);

	escribirSpi(0XFF);
	escribirSpi(0b00000000);
	escribirSpi(0XFF);
	escribirSpi(0b00000000);

	escribirSpi(0XFF);
	escribirSpi(0b00000000);
	escribirSpi(0XFF);
	escribirSpi(0b00000000);

	escribirSpi(0XFF);
	escribirSpi(0b00000000);
	escribirSpi(0XFF);
	escribirSpi(0b00000000);

	escribirSpi(0XFF);
	escribirSpi(0b00001000);
	escribirSpi(0XFF);
	escribirSpi(0b00001000);

	escribirSpi(0XFF);
	escribirSpi(0b00001000);
	escribirSpi(0XFF);
	escribirSpi(0b00001000);

	escribirSpi(0XFF);
	escribirSpi(0b00000000);
	escribirSpi(0XFF);
	escribirSpi(0b00000000);

	escribirSpi(0XFF);
	escribirSpi(0b01000000);
	escribirSpi(0XFF);
	escribirSpi(0b10000001);

	escribirSpi(0XFF);
	escribirSpi(0b01000001);
	escribirSpi(0XFF);
	escribirSpi(0b11000001);

	escribirSpi(0XFF);
	escribirSpi(0b00000001);
	escribirSpi(0XFF);
	escribirSpi(0b01000000);

	escribirSpi(0XFF);
	escribirSpi(0b00000000);
	escribirSpi(0XFF);
	escribirSpi(0b00000000);

	escribirSpi(0XFF);
	escribirSpi(0b00000010);
	escribirSpi(0XFF);
	escribirSpi(0b00100000);

	escribirSpi(0XFF);
	escribirSpi(0b00000110);
	escribirSpi(0XFF);
	escribirSpi(0b00110000);

	escribirSpi(0XFF);
	escribirSpi(0b00000110);
	escribirSpi(0XFF);
	escribirSpi(0b00110000);

	escribirSpi(0XFF);
	escribirSpi(0b00000000);
	escribirSpi(0XFF);
	escribirSpi(0b00000000);










	do{
       		escribirSpi(0xAA);
		c++;

	}while(c != 448);
	escribirSpi(0xFF);
        escribirSpi(0xFF);
	do{
		escribirSpi(0xFF);
       		escribirSpi(0xFF);
		resp = leerSpi();
		printf("Respuesta 2 CMD24: %x \n", resp);

	}while(resp != 0xFF);

}






