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

#define CMD_GO_IDLE_STATE       0x00        // CMD00: R1
#define CMD_SEND_CSD            0x09        // CMD09: R1
#define CMD_SEND_CID            0x0A        // CMD10: R1
#define CMD_SET_BLOCKLEN        0x10        // CMD16: R1 arg=BLOCKLEN
#define CMD_READ_SECTOR         0x11        // CMD17: R1 arg=data address
#define CMD_READ_MULT           0x12        // CMD18: R1 arg=data address
#define CMD_WRITE_SECTOR        0x18        // CMD24: R1 arg=data address
#define CMD_WRITE_MULT          0x19        // CMD25: R1 arg=data address
#define CMD_APP                 0x37        // CMD55: R1
#define ACMD_SEND_OP_COND       0x29        // ACMD41 R1 (after CMD_APP_CMD)

#define CMD_SEND_OP_COND        0x01
#define CMD_SEND_IF_COND        0x08
 
#define R1_IDLE                 0x01    //  Idle_State
#define R1_BAD                  0x80 

#define N_trials 10

void spiSDInit(void);
uint8_t sdInit(void);
uint8_t escribirEsperar(uint8_t cmd ,unsigned long arg);
void escribir(uint8_t data);
void delay (unsigned int time);

