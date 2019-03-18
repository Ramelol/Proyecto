#include <irq.h>
#include <uart.h>
#include <generated/csr.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "spiLCD.h"
#include "spiSD.h"


extern void periodic_isr(void);
void botton_isr(void);
void isr(void);
void dibujarNave1(int pX, int pY, int tam);
void dibujarNave2(int pX, int pY, int tam);
void dibujarCorazon(int pX, int pY, int tam);
int getPosNave1(void);
static void busy_wait(unsigned int ds);


static int pY = 35;

static int posNave1 = 0;
static int juegoIniciado=0;
static uint8_t* nave1;
static uint8_t* nave1_color;
static uint16_t corazon[120] = {
0x0000, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0x0000, 0x0000, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0x0000, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4,   // 0x0010 (16) pixels
0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4,   // 0x0020 (32) pixels
0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4,   // 0x0030 (48) pixels
0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0x0000, 0xE8E4, 0xE8E4, 0xE8E4,   // 0x0040 (64) pixels
0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0x0000, 0x0000, 0x0000, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4,   // 0x0050 (80) pixels
0xE8E4, 0xE8E4, 0x0800, 0x0000, 0x0000, 0x0000, 0x0000, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0xE8E4, 0x0800, 0x0000, 0x0000,   // 0x0060 (96) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0xE8E4, 0xE8C4, 0xE8C4, 0xE8E4, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0070 (112) pixels

};

static uint16_t nave2[1782] = { 
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0002, 0x0863, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0010 (16) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0882, 0x4249, 0x52CA, 0x0861, 0x8471, 0xA555, 0x94D3, 0x9D34,   // 0x0020 (32) pixels
0x94D3, 0x94D3, 0x9D14, 0xA555, 0x94D3, 0x94D3, 0x0062, 0x4A6A, 0x4A8A, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0030 (48) pixels
0x0863, 0x0001, 0x0001, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0040 (64) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0041, 0x0041, 0xD6FC, 0xB5D7, 0x9D55, 0x0041, 0x0041, 0x0000, 0x0000, 0x0000, 0x0021,   // 0x0050 (80) pixels
0x0021, 0xD6DB, 0xBE18, 0x9D55, 0x0041, 0x0021, 0x0000, 0x0000, 0x0000, 0x0021, 0x52AB, 0x5B0D, 0x5B4E, 0x0041, 0xAD76, 0xBDF8,   // 0x0060 (96) pixels
0xB5D8, 0xA5D8, 0x9DD8, 0xA5D9, 0xA5B8, 0xAD96, 0xB5B7, 0xB5D7, 0x0021, 0x632D, 0x6B6D, 0x4A8A, 0x0021, 0x0000, 0x0000, 0x0000,   // 0x0070 (112) pixels
0x0042, 0x0041, 0xADB7, 0xAD96, 0xD6DB, 0x0041, 0x0021, 0x0000, 0x0000, 0x0000, 0x0000, 0x0862, 0xAD96, 0xADB5, 0xE75C, 0x0000,   // 0x0080 (128) pixels
0x0001, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x8CB3, 0xADB7, 0xDF1C, 0xA575, 0xBE59, 0x8C92, 0x94B3, 0x0041, 0x0000,   // 0x0090 (144) pixels
0x0862, 0x8C72, 0xB5B7, 0xD6FB, 0xA555, 0xB5F7, 0x94F3, 0x8C92, 0x0021, 0x0000, 0x0001, 0x4A6A, 0x52CB, 0x9D35, 0x532D, 0x0040,   // 0x00A0 (160) pixels
0xE6BA, 0xDEBA, 0xC678, 0xD6DB, 0xF6DC, 0xEEBC, 0xD6BA, 0xE73C, 0xD6DB, 0xCE9A, 0x0882, 0x5AEC, 0x94D3, 0x52CB, 0x4A8A, 0x0002,   // 0x00B0 (176) pixels
0x0000, 0x0021, 0x8CB3, 0x94F3, 0xAD96, 0xB5F7, 0xD6DB, 0xADB7, 0x94D3, 0x0001, 0x0000, 0x0001, 0x9CF4, 0x8CB3, 0xB5F8, 0xADB5,   // 0x00C0 (192) pixels
0xD6FA, 0xB5D7, 0x94D3, 0x0021, 0x0000, 0x0000, 0x0021, 0x0882, 0x0000, 0x9D14, 0xB618, 0xCEBA, 0xA596, 0xA596, 0x94F4, 0xA515,   // 0x00D0 (208) pixels
0x0000, 0x0021, 0x0000, 0xA535, 0xA535, 0xD6FC, 0xB5F7, 0xA575, 0x9514, 0x9D54, 0x0000, 0x0000, 0x0861, 0x630C, 0x9CF4, 0xADB6,   // 0x00E0 (224) pixels
0x8CF3, 0x52EA, 0x0840, 0x1060, 0x0040, 0x0000, 0x0800, 0x1000, 0x0820, 0x0000, 0x0000, 0x0862, 0x4A6A, 0xA534, 0xB5B7, 0x9CF4,   // 0x00F0 (240) pixels
0x5B0C, 0x0001, 0x0001, 0x0041, 0x9D35, 0x8CD3, 0xADB6, 0xB5F7, 0xD6DB, 0xAD96, 0x94D3, 0x0000, 0x0842, 0x0000, 0xA555, 0x8CB3,   // 0x0100 (256) pixels
0xADB6, 0xAD95, 0xD6DB, 0xAD75, 0xA535, 0x0041, 0x0001, 0x0863, 0x94B3, 0x528A, 0x0020, 0x9514, 0xA576, 0xDF1C, 0xB618, 0xA576,   // 0x0110 (272) pixels
0x9D14, 0x94B3, 0x0041, 0x52AB, 0x0000, 0x94B3, 0xB5F8, 0xD6FB, 0xAD96, 0xBE39, 0x94F4, 0x9D13, 0x0020, 0x4A8A, 0x0020, 0xDEDB,   // 0x0120 (288) pixels
0xAD96, 0xADB6, 0xADD6, 0x9555, 0x8CF3, 0x8CB2, 0x9513, 0x8D34, 0x8575, 0x8533, 0x94F2, 0x9CF4, 0x9D34, 0x8C92, 0xA555, 0xAD96,   // 0x0130 (304) pixels
0xA575, 0xB5B7, 0xDF1C, 0x0000, 0x52AB, 0x0021, 0x94D3, 0x9514, 0xB5F8, 0xA555, 0xDEFC, 0xAD96, 0x9D14, 0x0041, 0x4A49, 0x0021,   // 0x0140 (320) pixels
0x94D3, 0x8CB3, 0xB5F7, 0xADB7, 0xD6DB, 0xB5D7, 0x94F3, 0x0000, 0x52AA, 0x9CF4, 0xB5D7, 0x632C, 0x0000, 0x94F3, 0xADB7, 0xD6DB,   // 0x0150 (336) pixels
0xAD96, 0xADB7, 0x9D14, 0xA535, 0x0000, 0x4A6A, 0x0021, 0x9D14, 0xA555, 0xD6DB, 0xB5F7, 0xA576, 0x9D14, 0x9D13, 0x0020, 0x5B0B,   // 0x0160 (352) pixels
0x0000, 0xEF5C, 0xA554, 0xADD6, 0xADD6, 0x9DD8, 0x9514, 0x9533, 0x8513, 0x9555, 0x5ACD, 0x632D, 0x8512, 0x9D14, 0x94F3, 0xA555,   // 0x0170 (368) pixels
0xA555, 0xA575, 0xB5F7, 0x9D35, 0xDF1C, 0x0001, 0x4A6A, 0x0021, 0x9D34, 0x94F4, 0xADB7, 0xB5F8, 0xD6BB, 0xADD7, 0x94F4, 0x0000,   // 0x0180 (384) pixels
0x5ACB, 0x0021, 0x8CB3, 0xA555, 0xA596, 0xADB7, 0xDF1C, 0xAD96, 0x94B3, 0x0061, 0x634D, 0xB5D7, 0x0042, 0x5ACB, 0x0841, 0x9514,   // 0x0190 (400) pixels
0xADB7, 0xD71C, 0xAD96, 0xADD7, 0x8C92, 0x94D3, 0x0000, 0x5AEC, 0x0000, 0x9CF4, 0xADB7, 0xCEBB, 0xA576, 0xA596, 0x9D35, 0x9D34,   // 0x01A0 (416) pixels
0x0000, 0x5AEB, 0x0020, 0xD6BA, 0xBE18, 0xA575, 0xA5B6, 0xADD7, 0x9491, 0x9D52, 0x7CF1, 0x632D, 0x79EC, 0x61A9, 0x5BAC, 0x94F3,   // 0x01B0 (432) pixels
0x94F3, 0x8CB3, 0xBE18, 0xB5F7, 0xA576, 0xB5F7, 0xCEBB, 0x0861, 0x5AEC, 0x0041, 0x8C92, 0xA576, 0x9D14, 0xBE18, 0xDF1C, 0xA596,   // 0x01C0 (448) pixels
0xA555, 0x0041, 0x52CB, 0x0021, 0xA555, 0x8471, 0xADB6, 0xA596, 0xD6DB, 0xB5D7, 0x94F3, 0x0021, 0x632C, 0x0000, 0x0000, 0x0842,   // 0x01D0 (464) pixels
0x0001, 0x9514, 0xAD96, 0xD71C, 0xA596, 0xB5D7, 0xA575, 0x9CD4, 0x0021, 0x5AEC, 0x0021, 0x94D3, 0xB5D7, 0xCEBB, 0xAD96, 0xB5F8,   // 0x01E0 (480) pixels
0x8CB2, 0x94D3, 0x0862, 0x5AEC, 0x0000, 0xD69A, 0xAD96, 0xB5F7, 0xADD7, 0xAD97, 0xA554, 0x94D0, 0x73AD, 0x424B, 0x0004, 0x0084,   // 0x01F0 (496) pixels
0x426A, 0x52EC, 0x9D55, 0x94F3, 0x9D55, 0xB5F7, 0xADD7, 0xA575, 0xDF3D, 0x0000, 0x6B8D, 0x0020, 0x94F3, 0x8CB3, 0xBE38, 0x9D14,   // 0x0200 (512) pixels
0xDF1D, 0xADB6, 0x8492, 0x0021, 0x632D, 0x0021, 0x8C92, 0x9D34, 0xA5B6, 0xADD7, 0xDF3C, 0x9D14, 0xAD96, 0x0021, 0x0001, 0x0000,   // 0x0210 (528) pixels
0x0000, 0x0000, 0x0022, 0x94D3, 0xBE39, 0xD71C, 0xBE38, 0xB5D7, 0x9D14, 0xAD76, 0x18C4, 0xA515, 0x0000, 0x94D3, 0xB5F8, 0xCEBA,   // 0x0220 (544) pixels
0xAD96, 0xAD96, 0x94F3, 0x94F4, 0x0000, 0x9D14, 0x0000, 0xDEFC, 0xAD96, 0xAD96, 0xA596, 0xAD99, 0x8554, 0xB5B6, 0x61EB, 0x0085,   // 0x0230 (560) pixels
0x15F7, 0x2DB6, 0x0024, 0x4ACB, 0xA575, 0x9D34, 0xADD7, 0xA596, 0xADB6, 0xAD96, 0xD6FB, 0x0020, 0x94B2, 0x0020, 0x9D34, 0x94F3,   // 0x0240 (576) pixels
0xA576, 0xB5F8, 0xD6BB, 0xA596, 0x9D55, 0x0021, 0xA515, 0x0021, 0x9D14, 0x8CD3, 0xA596, 0xADB6, 0xC69A, 0xB618, 0x94D4, 0x0021,   // 0x0250 (592) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x18E4, 0xA576, 0xADB6, 0xCE9A, 0xAD96, 0xA555, 0x9D14, 0x94F3, 0x1924, 0x2166, 0x0883, 0x9D35,   // 0x0260 (608) pixels
0xA575, 0xDF1B, 0xADB6, 0xADB7, 0x94D4, 0x9CF4, 0x0000, 0x0000, 0x0841, 0xCE9A, 0xAD96, 0xADD7, 0xADD7, 0xADF8, 0x8CB2, 0xAE38,   // 0x0270 (624) pixels
0x0005, 0x069D, 0x059F, 0x05DF, 0x05DF, 0x0041, 0xA576, 0x9D35, 0xADD7, 0xA596, 0xA555, 0xC639, 0xD6BB, 0x0000, 0x0041, 0x0000,   // 0x0280 (640) pixels
0x9D14, 0x9CF3, 0xB5D7, 0xAD96, 0xDF1C, 0xADB6, 0xA555, 0x0001, 0x0001, 0x0001, 0x9D14, 0x8CB2, 0xB5D7, 0xBDD8, 0xD6BB, 0xB5D7,   // 0x0290 (656) pixels
0x9CF4, 0x0041, 0x0000, 0x0000, 0x0000, 0x0000, 0x2946, 0x94D3, 0xAD75, 0x2165, 0x1904, 0x29A6, 0x94D3, 0x9D14, 0x2986, 0x0000,   // 0x02A0 (672) pixels
0x2166, 0x9CF4, 0xB5F7, 0x0020, 0x0000, 0x0041, 0x9D14, 0x94D3, 0x0841, 0x0000, 0x0000, 0xE75D, 0xADB7, 0xA596, 0xADD6, 0xB577,   // 0x02B0 (688) pixels
0x9CF4, 0xADD7, 0x0003, 0x0598, 0x15BC, 0x1E5F, 0x05DB, 0x0000, 0xBE18, 0x8492, 0xA5B6, 0xADD7, 0xB5F8, 0xA535, 0xDF1C, 0x0862,   // 0x02C0 (704) pixels
0x634D, 0x0020, 0x94B2, 0xA514, 0x0000, 0x0020, 0x0000, 0xB5B6, 0x94B3, 0x0001, 0x634E, 0x0022, 0x94B3, 0xAD96, 0x0000, 0x0000,   // 0x02D0 (720) pixels
0x0862, 0xA534, 0xA534, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x2966, 0x2986, 0x18E3, 0x9D14, 0x0861, 0xCE9A, 0x0841, 0x18E3,   // 0x02E0 (736) pixels
0x18E3, 0x0000, 0x2125, 0x2104, 0x0020, 0xB595, 0x0020, 0xF7BF, 0x0000, 0x0042, 0x0000, 0x0000, 0x0021, 0xCE7A, 0xAD96, 0xB618,   // 0x02F0 (752) pixels
0xA596, 0xB577, 0x9513, 0x94F2, 0x1043, 0x9F7F, 0x8E9F, 0xAF3F, 0x973F, 0x0041, 0x8CD3, 0x9D55, 0xA575, 0xB618, 0xADB6, 0xAD96,   // 0x0300 (768) pixels
0xDEDC, 0x0021, 0x52AC, 0x0862, 0x0000, 0x0000, 0xFFFF, 0x0000, 0xB5B6, 0x0041, 0x0020, 0x0000, 0x6B6E, 0x0000, 0x0021, 0x0020,   // 0x0310 (784) pixels
0xFFFF, 0x0842, 0xB5B6, 0x0000, 0x0020, 0x0021, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xAD55, 0xEF5D, 0x0000, 0x0861, 0x0021,   // 0x0320 (800) pixels
0xFFFF, 0xA514, 0x0000, 0x0000, 0x0000, 0xBDD7, 0xF79D, 0x0020, 0x0840, 0x0000, 0xFFFF, 0xAD97, 0x0000, 0x0000, 0x0000, 0xE71D,   // 0x0330 (816) pixels
0xAD96, 0xA555, 0xADD7, 0xA5B7, 0x8D94, 0x9D53, 0xA4F3, 0x0042, 0xFFBF, 0xFF7F, 0x0000, 0xAD96, 0x94D3, 0x94F3, 0xA596, 0xA5B6,   // 0x0340 (832) pixels
0xADB7, 0xAD76, 0xDF1C, 0x0001, 0x6B4E, 0x0001, 0xBDD8, 0xFFFF, 0x0020, 0x0000, 0x0000, 0xFFFF, 0xAD76, 0x0000, 0x5ACC, 0x0021,   // 0x0350 (848) pixels
0xBDF8, 0xF7BE, 0x0020, 0x0000, 0x0000, 0xFFFF, 0xAD56, 0x0001, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0360 (864) pixels
0x0000, 0xBDD8, 0x0000, 0x0021, 0x0000, 0x0000, 0x0000, 0x2125, 0x18C3, 0xFFDF, 0x0021, 0xB597, 0x0001, 0x0001, 0x0000, 0x0000,   // 0x0370 (880) pixels
0x0000, 0x0000, 0xCE9A, 0xB5F8, 0xA596, 0xA596, 0x8D74, 0x84F1, 0x9532, 0xBE37, 0x0020, 0x0000, 0xADF4, 0x94D3, 0x9D35, 0x8CD3,   // 0x0380 (896) pixels
0xB618, 0xADB6, 0xA576, 0xE73D, 0x0001, 0x632F, 0x0000, 0x0000, 0x0001, 0x0000, 0xB5B7, 0x0841, 0xFFFF, 0x0041, 0x0000, 0x0000,   // 0x0390 (912) pixels
0x0000, 0x0000, 0x0000, 0x0041, 0xAD96, 0x0000, 0xFFDF, 0x0021, 0x0021, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x03A0 (928) pixels
0x0000, 0x0000, 0x0842, 0x0001, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x03B0 (944) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0862, 0xD6FB, 0xAD96, 0xB597, 0x94F3, 0x9512, 0x9533, 0x94D2, 0xADD6, 0xADF6, 0x9593, 0x9CF4,   // 0x03C0 (960) pixels
0x9CF4, 0x9D35, 0xADB6, 0xA576, 0xD6FB, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0001, 0x0022, 0x0000, 0x0000,   // 0x03D0 (976) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0841, 0x0001, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x03E0 (992) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x03F0 (1008) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xCE7A, 0xB5D7, 0xB596, 0x9C93, 0xA535, 0x9CD4, 0x0000, 0x0000, 0x0001,   // 0x0400 (1024) pixels
0x0000, 0x9CD4, 0x94B3, 0x9514, 0xADB7, 0xA576, 0xE75D, 0x0021, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0410 (1040) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0420 (1056) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0430 (1072) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xE73D, 0xAD96, 0xA5D7, 0x9CB3, 0x8D14, 0x0001, 0x0000,   // 0x0440 (1088) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x9D14, 0x9D35, 0xA596, 0xADD7, 0xD6FB, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0450 (1104) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0460 (1120) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0470 (1136) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xD6BA, 0xB5B6, 0xADB7, 0x9D14, 0x9CF3,   // 0x0480 (1152) pixels
0x0021, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x9CF4, 0x8CD4, 0xA576, 0xB5B6, 0xDEFA, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0490 (1168) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x04A0 (1184) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x04B0 (1200) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0041, 0xAD95, 0xADB7,   // 0x04C0 (1216) pixels
0xA555, 0x8C91, 0x0882, 0x0000, 0x0000, 0x0000, 0x0000, 0x0820, 0x94B3, 0x9D56, 0xADB7, 0xB5B6, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x04D0 (1232) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x04E0 (1248) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x04F0 (1264) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xD6DB,   // 0x0500 (1280) pixels
0x0040, 0x0041, 0x0040, 0x10C2, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0021, 0x0042, 0x0020, 0x0020, 0xDEDA, 0x0842,   // 0x0510 (1296) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0520 (1312) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0530 (1328) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0540 (1344) pixels
0x0001, 0xE73C, 0xAD95, 0xADD7, 0x9D34, 0x8C71, 0x1082, 0x0000, 0x0000, 0x0000, 0x0000, 0x0820, 0x9CF4, 0x8CF4, 0xA596, 0xBDF6,   // 0x0550 (1360) pixels
0xDEBA, 0x0021, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0560 (1376) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0570 (1392) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0580 (1408) pixels
0x0000, 0x0000, 0x0022, 0xDEFB, 0xADB6, 0xA597, 0x94F3, 0xA534, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x9CF4, 0x9515,   // 0x0590 (1424) pixels
0xADF7, 0xAD95, 0xDEDA, 0x0001, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x05A0 (1440) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x05B0 (1456) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x05C0 (1472) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0001, 0xDEFC, 0xA575, 0xB5F8, 0x8471, 0x9D13, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x05D0 (1488) pixels
0xA555, 0x8CF4, 0xA596, 0xBDF7, 0xDEFB, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x05E0 (1504) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x05F0 (1520) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0600 (1536) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0001, 0xE75D, 0xAD95, 0xA597, 0x9D14, 0xA554, 0x0021, 0x0000, 0x0000, 0x0000,   // 0x0610 (1552) pixels
0x0000, 0x0841, 0x8472, 0xA597, 0xA5B6, 0xAD74, 0xDEDA, 0x0001, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0620 (1568) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0630 (1584) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0640 (1600) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0022, 0xD6BA, 0xB5D6, 0xB5D8, 0x9D14, 0x8C91, 0x0862, 0x0000,   // 0x0650 (1616) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0xA555, 0x84B3, 0xADD7, 0xB5D6, 0xDEFB, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0660 (1632) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0670 (1648) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0680 (1664) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0863, 0xD6FD, 0xAD54, 0x94B2, 0x9D14,   // 0x0690 (1680) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0040, 0x94B2, 0xA554, 0xB5B7, 0xD6DC, 0x0001, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x06A0 (1696) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x06B0 (1712) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x06C0 (1728) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xE71C,   // 0x06D0 (1744) pixels
0xE73D, 0xCE7A, 0x10A3, 0x0000, 0x0000, 0x0000, 0x0000, 0x0020, 0xD6DB, 0xD6DB, 0xDEFC, 0x0022, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x06E0 (1760) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x06F0 (1776) pixels
};

int getPosNave1(void){

	return posNave1;

}
static void busy_wait(unsigned int ds)
{
	timer0_en_write(0);
	timer0_reload_write(0);
	timer0_load_write(SYSTEM_CLOCK_FREQUENCY/10*ds);
	timer0_en_write(1);
	timer0_update_value_write(1);
	while(timer0_value_read()) timer0_update_value_write(1);
}

void dibujarCorazon(int pX, int pY, int tam){
	dibujarImagen(corazon,pX,pY,12,10);
}


void dibujarNave2(int pX, int pY, int tam){
	dibujarImagen(nave2,pX,pY,66,27);
}

void dibujarNave1(int pX, int pY, int tam){
	int p = 0;
	int k = 0;		
	for(int i=0;i<16;i++){	
		p = 128;
		for(int j=0;j<8;j++){
			if( (nave1[k] & p) == p){
	 	     		dibujarTileColor(BLANCO,pX+tam*j,pY-tam*i,tam);
			}
			p = p/2;
		}	
		k=k+2;
	}
	k=1;
	pX=pX+8*tam;
	for(int i=0;i<16;i++){	
		p = 128;
		for(int j=0;j<8;j++){
			if( (nave1[k] & p) == p){
		     		dibujarTileColor(BLANCO,pX+tam*j,pY-tam*i,tam);
			}
			p = p/2;
		}	
		k=k+2;
	}
	k = 0; 
	pX = pX-8*tam;
	for(int i=0;i<16;i++){	
		p = 128;
		for(int j=0;j<8;j++){
			if( (nave1_color[k] & p) == p){
		     		dibujarTileColor(AZUL,pX+tam*j,pY-tam*i,tam);
			}
			p = p/2;
		}	
		k=k+2;
	}
	k=1;
	pX=pX+8*tam;
	for(int i=0;i<16;i++){	
		p = 128;
		for(int j=0;j<8;j++){
			if( (nave1_color[k] & p) == p){
		     		dibujarTileColor(AZUL,pX+tam*j,pY-tam*i,tam);
			}
			p = p/2;
		}	
		k=k+2;
	}			
	
}	



void isr(void){
	unsigned int irqs;

	irqs = irq_pending() & irq_getmask();

	if(irqs & (1 << UART_INTERRUPT))
		uart_isr();
	if(irqs & (1 << 4))
	 botton_isr();


}
void botton_isr(void){
	// borrador por soft la interrupción del periferico
	unsigned int pednig = buttons_ev_pending_read();
	printf("%X\n", pednig);

	buttons_ev_pending_write (0xff);
  	buttons_ev_enable_write(0xff);
	

	if(pednig  & 1 ){
		if(juegoIniciado>0){
			dibujarTileColor(NEGRO,posNave1,pY-32,34);
			posNave1 = posNave1 - 5;
			dibujarNave1(posNave1,pY,2);
		}
	}
	if(pednig  & 1<<2 ){	
	

	}
	if(pednig  & 1<<3 ){
		if(juegoIniciado>0){
			dibujarTileColor(NEGRO,posNave1,pY-31,34);
			posNave1 = posNave1 + 5;
			dibujarNave1(posNave1,pY,2);
		}
	}
	if(pednig  & 1<<4 ){
		led_GB_out_write(~led_GB_out_read());
		
	}
	if(pednig  & 1<<5 ){
		if(juegoIniciado == 0){
			iniciarJuego();
			posNave1 = 88;
			nave1 = leerNave1();
			nave1_color = leerNave1_color();
			juegoIniciado ++;
			colorFondo(NEGRO);
			pY = 35;
			dibujarNave1(posNave1,pY,2);
			dibujarCorazon(5,165,1);
			dibujarCorazon(5,150,1);
			dibujarCorazon(5,135,1);
			dibujarNave2(75,168,1);
		}
	}
	if(pednig  & 1<<6 ){
		if(juegoIniciado>0){
			disparar();
		}
		
	}
	if(pednig  & 1<<7 ){

	}
	if(pednig  & 1<<8 ){

		
	}


}
