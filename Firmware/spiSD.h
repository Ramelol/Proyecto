#include <generated/csr.h>

#define OFFLINE      0b01
#define CS_POLARITY  0b1000
#define CLK_POLARITY 0b10000
#define CLK_PHASE    0b100000
#define LSB_FIRST    0b1000000
#define HALF_DUPLEX  0b10000000
#define DIV_READ     0b10000000000000000 
#define DIV_WRITE    0b1000000000000000000000000
#define WRITE_LENGTH 0b10000000000000000
#define READ_LENGTH  0b1000000000000000000000000


void spiSDInit(void);
void sdInit(void);
void ponerModoSpi(void);
void escribirSpi(uint8_t data);
uint8_t leerSpi(void);
void delaySD (unsigned int time);
uint8_t leerBloque (uint32_t direccion);
void escribirBloque(uint8_t data, uint32_t direccion);
