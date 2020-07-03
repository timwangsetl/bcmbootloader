#ifndef LCD_COMMON_H
#define LCD_COMMON_H

typedef enum
{
	BPP_1 = 0,
	BPP_2,
	BPP_4,
	BPP_8,
	BPP_16,
	BPP_24,
	BPP_565,
	BPP_12
}bits_per_pixel_t;

typedef struct 
{
	uint32_t PPL;   // pixel per line
	uint32_t LPF;   //line per frame
	uint32_t HSW;   //horizontal pulse width
	uint32_t HBP;   //horziontal back porch
	uint32_t HFP;   //horziontal front porch 
	uint32_t VSW;   //vertical pulse width
	uint32_t VBP;   //vertical back porch
	uint32_t VFP;   //vertical front porch
	uint32_t LED;   //line-end signal delay
    uint8_t  IHS;   //invert horrizontal sync polarity 
    uint8_t  IVS;  //invert vertical sync polarity
    uint8_t  IPC;   //invert panel clock
}lcd_param_t;

#endif // #ifndef LCD_COMMON.h
