#include <stm32f031x6.h>
#include "display.h"

void initClock(void);
void initSysTick(void);
void SysTick_Handler(void);
void delay(volatile uint32_t dly);
void setupIO();
int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py);
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber);
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode);

volatile uint32_t milliseconds;

// Main Menu Function Declared

void drawMainMenu()
{
    uint16_t textColour = RGBToWord(255,255,150);
    uint16_t bgColour = 0x0000;

    clear();

    printTextX2("Welcome to Slime Run", 18, 20, textColour, bgColour);
	printTextX2("---- Main Menu ----", 18, 20, textColour, bgColour);

    printText("Press Right to Start", 12, 70, textColour, bgColour);
    printText("Press Left for Sound On", 6, 95, textColour, bgColour);

    for(int x = 10; x < 118; x += 6)
    {
        fillRectangle(x,120,4,2,textColour);
    }
}

// Images

const uint16_t deco1[] = {0,0,0,0,4,4,4,4,4,0,0,0,0,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,65415,65415,65415,248,65415,0,0,0,0,0,0,0,65415,65415,65415,65415,65415,8068,0,0,0,0,0,0,65415,65415,65415,4096,4096,0,0,0,0,0,0,0,0,65415,65415,65415,0,0,0,0,0,0,0,0,0,7936,7936,7936,0,0,0,0,0,0,0,0,7936,7936,65535,7936,0,0,0,0,0,0,0,0,7936,7936,65535,7936,7936,7936,7936,0,0,0,0,0,7936,7936,65535,65535,65535,65535,7936,0,0,0,0,0,7936,7936,7936,7936,7936,7936,7936,0,0,0,0,0,7936,7936,7936,7936,0,0,0,0,0,0,0,0,0,7936,65535,7936,0,0,0,0,0,0,0,0,0,7936,65535,7936,0,0,0,0,0,0,0,0,0,7936,65535,7936,0,0,0,0,0,0,0,0,0,7940,7940,7940,7940,0,0,0,};
const uint16_t deco2[] = {0,0,0,0,0,4,4,4,4,4,0,0,0,0,4,4,4,4,4,4,4,4,0,0,0,0,0,0,0,65415,65415,65415,248,65415,0,0,0,0,0,0,0,65415,65415,65415,65415,65415,8068,0,0,0,0,0,0,65415,65415,65415,4096,4096,0,0,0,0,0,0,0,0,65415,65415,65415,0,0,0,0,0,0,0,0,7936,7936,7936,0,0,0,0,0,0,0,0,7936,7936,65535,7936,0,0,0,0,0,0,0,0,7936,7936,65535,7936,7936,7936,7936,0,0,0,0,0,7936,7936,65535,65535,65535,65535,7936,0,0,0,0,0,7936,7936,7936,7936,7936,7936,7936,0,0,0,0,0,7936,7936,7936,7936,0,0,0,0,0,0,0,0,0,40224,7936,65535,7936,0,0,0,0,0,0,0,40224,40224,7936,65535,7936,0,0,0,0,0,0,65315,40224,40224,7936,65535,40224,0,0,0,0,0,0,0,65315,0,65315,65315,65315,65315,0,0,};
const uint16_t deco3[] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,4,0,0,0,0,0,0,0,0,0,0,4,4,0,0,0,0,0,0,0,0,0,4,4,4,4,0,0,0,0,0,0,0,4,4,4,4,4,4,0,0,0,0,7936,7936,4,4,4,4,4,4,7936,7936,0,0,65535,65535,4,4,4,4,4,4,65535,65535,0,0,7936,7936,4,4,4,4,4,4,7936,7936,0,0,0,0,0,4,4,4,4,0,0,0,0,0,0,0,0,0,24327,24327,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
const uint16_t dg1[] = {0,0,16142,16142,16142,16142,16142,16142,16142,16142,0,0,0,0,0,16142,16142,16142,16142,16142,16142,0,0,0,0,0,16142,16142,16142,16142,16142,16142,16142,16142,0,0,0,0,16142,16142,16142,1994,1994,16142,16142,16142,0,0,0,0,16142,16142,16142,1994,16142,1994,16142,16142,0,0,0,0,16142,16142,16142,1994,16142,1994,16142,16142,0,0,0,0,16142,16142,16142,1994,16142,1994,16142,16142,0,0,0,0,16142,16142,16142,1994,1994,16142,16142,16142,0,0,0,0,16142,16142,16142,16142,16142,16142,16142,16142,0,0,0,0,16142,16142,16142,1994,1994,1994,16142,16142,0,0,0,0,16142,16142,16142,1994,16142,16142,16142,16142,0,0,0,0,16142,16142,16142,1994,16142,16142,16142,16142,0,0,0,0,16142,16142,16142,1994,16142,1994,16142,16142,0,0,0,0,16142,16142,16142,1994,1994,1994,16142,16142,0,0,0,0,0,16142,16142,16142,16142,16142,16142,0,0,0,0,0,0,16142,16142,16142,16142,16142,16142,0,0,0,};

// Main

int main()
{
	int hinverted = 0;
	int vinverted = 0;
	int toggle = 0;
	int hmoved = 0;
	int vmoved = 0;

	uint16_t x = 50;
	uint16_t y = 50;
	uint16_t oldx = x;
	uint16_t oldy = y;

	initClock();
	initSysTick();
	setupIO();

	// Show Menu

	drawMainMenu();

	while((GPIOB->IDR & (1 << 4)) != 0)
	{
		// waiting for the Right button to be pressed: doing nothing until it is pressed
	}

	delay(200);

	clear();

	// Game Starts

	putImage(20,80,12,16,dg1,0,0);

	while(1)
	{
		hmoved = vmoved = 0;
		hinverted = vinverted = 0;

		if ((GPIOB->IDR & (1 << 4))==0)
		{					
			if (x < 110)
			{
				x = x + 1;
				hmoved = 1;
				hinverted=0;
			}						
		}

		if ((GPIOB->IDR & (1 << 5))==0)
		{			
			if (x > 10)
			{
				x = x - 1;
				hmoved = 1;
				hinverted=1;
			}			
		}

		if ((GPIOA->IDR & (1 << 11)) == 0)
		{
			if (y < 140)
			{
				y = y + 1;			
				vmoved = 1;
				vinverted = 0;
			}
		}

		if ((GPIOA->IDR & (1 << 8)) == 0)
		{			
			if (y > 16)
			{
				y = y - 1;
				vmoved = 1;
				vinverted = 1;
			}
		}

		if ((vmoved) || (hmoved))
		{
			fillRectangle(oldx,oldy,12,16,0);

			oldx = x;
			oldy = y;

			if (hmoved)
			{
				if (toggle)
					putImage(x,y,12,16,deco1,hinverted,0);
				else
					putImage(x,y,12,16,deco2,hinverted,0);

				toggle ^= 1;
			}
			else
			{
				putImage(x,y,12,16,deco3,0,vinverted);
			}

			if (isInside(20,80,12,16,x,y) ||
				isInside(20,80,12,16,x+12,y) ||
				isInside(20,80,12,16,x,y+16) ||
				isInside(20,80,12,16,x+12,y+16))
			{
				printTextX2("GLUG!", 10, 20, RGBToWord(255,255,0), 0);
			}
		}

		delay(50);
	}

	return 0;
}

// Timer

void initSysTick(void)
{
	SysTick->LOAD = 48000;
	SysTick->CTRL = 7;
	SysTick->VAL = 10;
	__asm(" cpsie i ");
}

void SysTick_Handler(void)
{
	milliseconds++;
}

// Clock

void initClock(void)
{
	RCC->CR &= ~(1u<<24);
	while((RCC->CR & (1<<25)));

	FLASH->ACR |= (1<<0);
	FLASH->ACR &=~((1u<<2)|(1u<<1));
	FLASH->ACR |= (1<<4);

	RCC->CFGR &= ~((1u<<21)|(1u<<20)|(1u<<19)|(1u<<18));
	RCC->CFGR |= ((1<<21)|(1<<19));

	RCC->CFGR |= (1<<14);

	RCC->CR |= (1<<24);
	RCC->CFGR |= (1<<1);
}

// Delay

void delay(volatile uint32_t dly)
{
	uint32_t end_time = dly + milliseconds;

	while(milliseconds != end_time)
		__asm(" wfi ");
}

// GPIO

void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber)
{
	Port->PUPDR &= ~(3u << BitNumber*2);
	Port->PUPDR |= (1u << BitNumber*2);
}

void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode)
{
	uint32_t mode_value = Port->MODER;

	Mode <<= (2 * BitNumber);

	mode_value &= ~(3u << (BitNumber * 2));
	mode_value |= Mode;

	Port->MODER = mode_value;
}

void setupIO()
{
	RCC->AHBENR |= (1 << 18) + (1 << 17);

	display_begin();

	pinMode(GPIOB,4,0);
	pinMode(GPIOB,5,0);
	pinMode(GPIOA,8,0);
	pinMode(GPIOA,11,0);

	enablePullUp(GPIOB,4);
	enablePullUp(GPIOB,5);
	enablePullUp(GPIOA,11);
	enablePullUp(GPIOA,8);
}

// When the Colision occurs

int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py)
{
	uint16_t x2 = x1 + w;
	uint16_t y2 = y1 + h;

	if ((px >= x1) && (px <= x2))
	{
		if ((py >= y1) && (py <= y2))
			return 1;
	}

	return 0;
}
