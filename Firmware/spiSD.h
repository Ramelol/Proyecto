#include <generated/csr.h>

#define OFFLINE      1
#define CS_POLARITY  8
#define CLK_POLARITY 16
#define CLK_PHASE    32
#define LSB_FIRST    64 
#define HALF_DUPLEX  128
#define DIV_READ     65536
#define DIV_WRITE    16777216
#define WRITE_LENGTH 65536
#define READ_LENGTH  16777216


void spiSDInit(void);
void sdInit(void);
void ponerModoSpi(void);
void escribirSpi(uint32_t data);
uint8_t leerSpi(void);
void delaySD (unsigned int time);

