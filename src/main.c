#include <stm32f031x6.h>
#include "display.h"
#include <stdlib.h> // random number generation
#include <stdio.h> 

void initClock(void);
void initSysTick(void);
void SysTick_Handler(void);
void delay(volatile uint32_t dly);
void setupIO();
int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py);
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber);
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode);

volatile uint32_t milliseconds;

const uint16_t slimeDefault[]=
{
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9293,9293,9293,9293,9293,9293,0,0,0,0,0,0,0,0,0,0,0,0,0,9293,9293,63383,63383,63383,63383,9293,9293,0,0,0,0,0,0,0,0,0,0,0,9293,9293,63383,63383,63383,63383,63383,63383,9293,9293,0,0,0,0,0,0,0,0,0,9293,9293,63383,63383,63383,63383,63383,63383,63383,63383,9293,9293,0,0,0,0,0,0,0,9293,9293,63383,63383,0,0,63383,63383,0,0,63383,63383,9293,9293,0,0,0,0,0,0,9293,63383,63383,63383,0,0,63383,63383,0,0,63383,63383,63383,9293,0,0,0,0,0,9293,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,9293,0,0,0,0,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,0,0,0,9293,9293,63383,63383,63383,63383,63383,0,63383,63383,0,63383,63383,63383,63383,63383,9293,9293,0,0,9293,63383,63383,63383,63383,63383,63383,63383,0,0,63383,63383,63383,63383,63383,63383,63383,9293,0,0,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,0,0,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
const uint16_t slimeLeft[]= 
{
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9293,9293,9293,9293,9293,9293,0,0,0,0,0,0,0,0,0,0,0,0,0,9293,9293,63383,63383,63383,63383,9293,9293,0,0,0,0,0,0,0,0,0,0,0,9293,9293,63383,63383,63383,63383,63383,63383,9293,9293,0,0,0,0,0,0,0,0,0,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,9293,0,0,0,0,0,0,0,9293,9293,0,0,63383,63383,63383,63383,63383,63383,63383,63383,9293,9293,0,0,0,0,0,0,9293,63383,0,0,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,0,0,0,0,0,9293,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,9293,0,0,0,0,9293,63383,63383,0,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,0,0,0,9293,9293,0,0,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,9293,0,0,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,0,0,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,0,0,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};                                                     
const uint16_t slimeRight[]= 
{
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9293,9293,9293,9293,9293,9293,0,0,0,0,0,0,0,0,0,0,0,0,0,9293,9293,63383,63383,63383,63383,9293,9293,0,0,0,0,0,0,0,0,0,0,0,9293,9293,63383,63383,63383,63383,63383,63383,9293,9293,0,0,0,0,0,0,0,0,0,9293,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,0,0,0,0,0,0,0,9293,9293,63383,63383,63383,63383,63383,63383,63383,63383,0,0,9293,9293,0,0,0,0,0,0,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,0,0,63383,9293,0,0,0,0,0,9293,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,9293,0,0,0,0,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,0,63383,63383,9293,0,0,0,9293,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,0,0,9293,9293,0,0,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,0,0,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,0,0,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};
const uint16_t slimeUp[]=
{
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9293,9293,9293,9293,9293,9293,0,0,0,0,0,0,0,0,0,0,0,0,0,9293,9293,63383,63383,63383,63383,9293,9293,0,0,0,0,0,0,0,0,0,0,0,9293,9293,63383,63383,63383,63383,63383,63383,9293,9293,0,0,0,0,0,0,0,0,0,9293,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,0,0,0,0,0,0,0,9293,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,9293,0,0,0,0,0,0,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,0,0,0,0,0,9293,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,9293,0,0,0,0,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,0,0,0,9293,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,9293,0,0,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,0,0,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,0,0,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,};

void drawMainMenu()
{
    uint16_t textColour = RGBToWord(255,255,150);
    uint16_t bgColour = 0x0000;

    clear();

    printText("Slime Run", 33, 20, textColour, bgColour);
    printText("-- Main Menu --", 12, 30, textColour, bgColour);

    printText("Right to Start", 15, 50, textColour, bgColour);
    printText("Left for Sound On", 4, 70, textColour, bgColour);

    for (int x = 10; x < 118; x += 6)
    {
        fillRectangle(x, 90, 4, 2, textColour);
    }
}



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
	uint16_t delayTime = 5;
	uint16_t score = 0;


	while(1)
	{
		initClock();
		initSysTick();
		setupIO();

		start:

		drawMainMenu();

		while((GPIOB->IDR & (1 << 4)) != 0)
		{
			// waiting for the Right button to be pressed: doing nothing until it is pressed
		}

		delay(200);

		clear();

		// Game Starts

		// General wall variables:
		uint16_t gap = 30;
		uint16_t wallY = 0;
		uint16_t height = 15;
		uint16_t colour = RGBToWord(255, 0, 0);
		uint16_t w1 = (random() % (128 - gap));
		uint16_t x2 = w1 + gap;
		uint16_t w2 = 128 - x2;

		int gameRunning = 1;

		while(gameRunning == 1)
		{
			fillRectangle(0, 125, 128, 25, RGBToWord(255, 255, 255));

			fillRectangle(35, 135, 50, 10, RGBToWord(255, 255, 255));
			char scoreText[10];
			sprintf(scoreText, "%d", score);
			printText(scoreText, 75, 135, RGBToWord(0, 0, 0), RGBToWord(255, 255, 255));

			printText("Score:", 30, 135, RGBToWord(0, 0, 0), RGBToWord(255, 255, 255));

			fillRectangle(0, wallY, w1, height, 0);
			fillRectangle(x2, wallY, w2, height, 0);

			if (isInside(0, wallY, w1, height, x, y) || isInside(x2, wallY, w2, height, x, y))
			{
				clear();
				printTextX2("Game Over!", 5, 40, RGBToWord(255, 0, 0), RGBToWord(0, 0, 0));
				printText("Final Score:", 15, 90, RGBToWord(255, 255, 255), RGBToWord(0, 0, 0));
				printText(scoreText, 105, 90, RGBToWord(255, 255, 255), RGBToWord(0, 0, 0));
				delay(5000);
				goto start;
			}

			wallY = wallY + 1;

			if (wallY > 110)
			{
				wallY = 0;
				gap = 30; 
				w1 = (random() % (128 - gap)); 
				x2 = w1 + gap; 
				w2 = 128 - x2;
				score = score + 1;

				if (delayTime > 0.1)
				{
					delayTime = delayTime - 0.3;
				}
			}

			fillRectangle(0, wallY, w1, height, colour);
			fillRectangle(x2, wallY, w2, height, colour);

			delay(delayTime);

			hmoved = vmoved = 0;
			hinverted = vinverted = 0;
			if ((GPIOB->IDR & (1 << 4))==0) // right pressed
			{					
				if (x < 128) // determines whether sprite is whitin horizontal boundries
				{
					x = x + 1; // verification for horizontal boundary
					hmoved = 1; // shows horizontal movement
					hinverted=0; // shows which horizontal direction 
				}						
			}
			if ((GPIOB->IDR & (1 << 5))==0) // left pressed
			{			
				
				if (x > 0) // determines whether sprite is whitin horizontal boundries
				{
					x = x - 1; // verification for horizontal boundary
					hmoved = 1; // shows horizontal movement
					hinverted=1; // shows which horizontal direction
				}			
			}
			if ( (GPIOA->IDR & (1 << 11)) == 0) // down pressed
			{
				if (y < 125)
				{
					y = y + 1; // verification for vertical boundary
					vmoved = 1; // shows vertical movement
					vinverted = 0; // shows which vertical direction
				}
			}
			if ( (GPIOA->IDR & (1 << 8)) == 0) // up pressed
			{			
				if (y > 15)
				{
					y = y - 1; // verification for vertical boundary
					vmoved = 1; // shows vertical movement
					vinverted = 1; // shows which vertical direction
				}
			}
			if ((vmoved) || (hmoved))
			{
				// only redraw if there has been some movement (reduces flicker)
				fillRectangle(oldx,oldy,12,16,0);
				oldx = x;
				oldy = y;					
				if (hmoved)
				{
					if (hinverted)
					{
						putImage(x,y,20,20,slimeLeft,0,0);
					}
					else
					{
						putImage(x,y,20,20,slimeRight,0,0);
					}
					toggle = toggle ^ 1;
				}
				else if (vmoved)
				{
					if (vinverted)
					{
						putImage(x,y,20,20,slimeUp,0,0);
					}
					else
					{
						putImage(x, y, 20, 20, slimeDefault, 0, 0);
					}
					
				}
			}
			delay(45);
		}
		return 0;
	}
}
void initSysTick(void)
{
	SysTick->LOAD = 48000;
	SysTick->CTRL = 7;
	SysTick->VAL = 10;
	__asm(" cpsie i "); // enable interrupts
}
void SysTick_Handler(void)
{
	milliseconds++;
}
void initClock(void)
{
// This is potentially a dangerous function as it could
// result in a system with an invalid clock signal - result: a stuck system
        // Set the PLL up
        // First ensure PLL is disabled
        RCC->CR &= ~(1u<<24);
        while( (RCC->CR & (1 <<25))); // wait for PLL ready to be cleared
        
// Warning here: if system clock is greater than 24MHz then wait-state(s) need to be
// inserted into Flash memory interface
				
        FLASH->ACR |= (1 << 0);
        FLASH->ACR &=~((1u << 2) | (1u<<1));
        // Turn on FLASH prefetch buffer
        FLASH->ACR |= (1 << 4);
        // set PLL multiplier to 12 (yielding 48MHz)
        RCC->CFGR &= ~((1u<<21) | (1u<<20) | (1u<<19) | (1u<<18));
        RCC->CFGR |= ((1<<21) | (1<<19) ); 

        // Need to limit ADC clock to below 14MHz so will change ADC prescaler to 4
        RCC->CFGR |= (1<<14);

        // and turn the PLL back on again
        RCC->CR |= (1<<24);        
        // set PLL as system clock source 
        RCC->CFGR |= (1<<1);
}
void delay(volatile uint32_t dly)
{
	uint32_t end_time = dly + milliseconds;
	while(milliseconds != end_time)
		__asm(" wfi "); // sleep
}

void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber)
{
	Port->PUPDR = Port->PUPDR &~(3u << BitNumber*2); // clear pull-up resistor bits
	Port->PUPDR = Port->PUPDR | (1u << BitNumber*2); // set pull-up bit
}
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode)
{
	/*
	*/
	uint32_t mode_value = Port->MODER;
	Mode = Mode << (2 * BitNumber);
	mode_value = mode_value & ~(3u << (BitNumber * 2));
	mode_value = mode_value | Mode;
	Port->MODER = mode_value;
}
int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py)
{
	// checks to see if point px,py is within the rectange defined by x,y,w,h
	uint16_t x2,y2;
	x2 = x1+w;
	y2 = y1+h;
	int rvalue = 0;
	if ( (px >= x1) && (px <= x2))
	{
		// ok, x constraint met
		if ( (py >= y1) && (py <= y2))
			rvalue = 1;
	}
	return rvalue;
}

void setupIO()
{
	RCC->AHBENR |= (1 << 18) + (1 << 17); // enable Ports A and B
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
