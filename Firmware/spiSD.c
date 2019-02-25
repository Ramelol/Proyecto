#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <generated/csr.h>
#include <generated/mem.h>
#include <hw/flags.h>
#include <system.h>

#include "spiSD.h"


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
        config |= 8*DIV_READ | 8*DIV_WRITE;
	pantalla_spi_xfer_write( 0b1 | 8*WRITE_LENGTH);	
        pantalla_spi_config_write(config);
	printf("Configuración de SPI finalizada: %x\n", config );
}
void escribir(uint8_t data){

	sdcard_spi_mosi_data_write(data);
	pantalla_spi_start_write(1);
	while(pantalla_spi_active_read()){}

}

uint8_t escribirEsperar(uint8_t cmd ,unsigned long arg){
	uint8_t* ptr = (uint8_t*)&arg;
        uint8_t k = 0xFF;
        uint8_t res = 0xFF;
	uint8_t crc = 0xFF; 
	if (cmd==CMD_GO_IDLE_STATE){	
		 crc=0x95; //  CMD00 --> crc needed = 0x95
	}
	if (cmd==CMD_SEND_IF_COND){
	         crc=0x87; //  CMD08 --> crc needed = 0x87
	}
	cmd = (cmd & 0x3F) | 0x40; // Makes sure cmd is 6 bit and OR it with 0x40
	k = 10;
	res=0;
	while ( (res!=0xFF) && (k--) ) {
		res = sdcard_spi_miso_data_read(); 
		delaySD(1);
	}
	escribir(cmd);

	for(k=0;k<4;k++){
		escribir(ptr[3-k]); // Argument bytes: Most Significant Byte first
 		escribir(crc);
	}
	res=0xFF;
	k=10;
	while ( (res&R1_BAD) && (k--) ){
		 res=sdcard_spi_miso_data_read(); 
	}	
	return res;
};

uint8_t sdInit(void){
	sdcard_v_out_write(0b10);
	spiSDInit();
	uint8_t k;
	uint8_t res;
        for(k=0;k<N_trials;k++){  //Try with CMD_0x00 a few times
   		res = escribirEsperar(CMD_GO_IDLE_STATE,0x00); 
    		if (res==R1_IDLE){
			break; // Great, card went into IDLE_STATE
    		}
		delaySD(0.25); //delay
        }
        switch (res){
               case 1:  //SDHC card --> send CMD_01
     		    res=1; 
		    k=0;  // Try with CMD_SEND_OP a few times.
                    while(res & (k<100)){
       			res = escribirEsperar(0x01,0x40000000);
			if (res&R1_BAD){
				continue;
			}
       			k++;
			delaySD(0.1); //delay_ms(20);
      		    }
     		    if (res){
			return 2;
		    }  // Card failed when sending CMD_SEND_OP_COND after CMD08
     		    break;

    	        case 5:  // CMD08 declared illegal command, maybe it is a normal SD card
     		    res=1; 
		    k=0;  // Try with CMD_SEND_OP a few times.
                    while(res & (k<100)){
       			res = escribirEsperar(CMD_APP,0);
			if (res&R1_BAD){
				continue;
			}
       			k++;
			delaySD(0.1); //delay_ms(20);
			res = escribirEsperar(ACMD_SEND_OP_COND,0);
      		    }
     		    if (res){
			return 1;
		    }  // Card failed when sending ACMD_SEND_OP_COND
     		    break;   
     		default:
		    return 3;
                    break;  // Failed CMD08
       }
       res = escribirEsperar(CMD_SET_BLOCKLEN,512L);  // Set block length = 512
       return 0;
}




