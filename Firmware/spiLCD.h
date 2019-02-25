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

#define POWER_CTRL1 0X0010
#define POWER_CTRL2 0X0011
#define POWER_CTRL3 0X0012
#define POWER_CTRL4 0X0013
#define POWER_CTRL5 0X0014

#define DRIVER_OUTPUT_CTRL 0X0001
#define LCD_AC_DRIVING_CTRL 0X0002
#define ENTRY_MODE 0X0003
#define DISP_CTRL1 0X0007
#define BLANK_PERIOD_CTRL1 0X0008
#define FRAME_CYCLE_CTRL 0X000B
#define INTERFACE_CTRL 0X000C
#define OSC_CTRL   0X000F
#define VCI_RECYCLING 0X0015
#define RAM_ADDR_SET1 0X0020
#define RAM_ADDR_SET2 0X0021

#define GATE_SCAN_CTRL 0X0030
#define VERTICAL_SCROLL_CTRL1 0X0031
#define VERTICAL_SCROLL_CTRL2 0X0032 
#define VERTICAL_SCROLL_CTRL3 0X0033
#define PARTIAL_DRIVING_POS1  0X0034
#define PARTIAL_DRIVING_POS2  0X0035
#define HORIZONTAL_WINDOW_ADDR1 0X0036
#define HORIZONTAL_WINDOW_ADDR2 0X0037
#define VERTICAL_WINDOW_ADDR1   0X0038
#define VERTICAL_WINDOW_ADDR2   0X0039
#define GRAM_DATA_REG 0x0022


#define GAMMA_CTRL1 0x0050
#define GAMMA_CTRL2 0x0051 
#define GAMMA_CTRL3 0x0052
#define GAMMA_CTRL4 0x0053
#define GAMMA_CTRL5 0x0054 
#define GAMMA_CTRL6 0x0055
#define GAMMA_CTRL7 0x0056
#define GAMMA_CTRL8 0x0057
#define GAMMA_CTRL9 0x0058
#define GAMMA_CTRL10 0x0059


#define AZUL   0X001F
#define BLANCO 0XFFFF
#define GRIS 0x8410 
#define AZULOSCURO 0x0011
#define ORO 0xFEA0
#define INDIGO 0x4810
#define ROJO 0XF800
#define PLATA 0xC618
#define BEIGE 0xF7BB 
#define SIENNA 0xA285

void spiInit(void);
void escribirLCD(  unsigned int cs, unsigned int  data);
void delay (unsigned int time);
void lcd_init(void);
void dibujarPunto(unsigned int x, unsigned int  y, uint16_t color);
void dibujarImagen(uint16_t* imagen, int tam);
void dibujarTile(uint16_t* imagen, int x, int y);
void dibujarTileColor(uint16_t color, int x, int y);
void colorFondo(uint16_t color);



