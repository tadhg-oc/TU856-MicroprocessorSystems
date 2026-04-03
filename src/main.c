#include <stm32f031x6.h>
#include "display.h"
#include "sound.h" // for sound effects
#include "musical_notes.h" // more complex sounds
#include <stdlib.h> // random number generation
#include <stdio.h>
#include "usbh_core.h"
#include "usbh_hid.h"
#include "usbh_hid_keybd.h"

USBH_HandleTypeDef hUSBHost;
HID_KEYBD_Info_TypeDef *kb_info;

void initClock(void);
void initSysTick(void);
void SysTick_Handler(void);
void delay(volatile uint32_t dly);
void setupIO();
int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py);
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber);
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode);

volatile uint32_t milliseconds;

const uint32_t scoreUpSound[] = {C4, E4, G4, C5};
const uint32_t scoreUpLen[] = {200, 200, 200, 400};

const uint32_t gameOverSound[] = {G4, F4, E4, D4, C4};
const uint32_t gameOverLen[] = {300, 300, 300, 300, 500};

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
    uint16_t textColour = RGBToWord(255,255,255);
    uint16_t bgColour = 0x0000;

    clear();

    printText("Slime Run", 33, 20, textColour, bgColour);
    printText("-- Main Menu --", 12, 30, textColour, bgColour);

    printText("Right to Start", 15, 60, textColour, bgColour);

	putImage(52, 110, 20, 20, slimeDefault, 0, 0);

    for (int x = 10; x < 118; x += 6)
    {
        fillRectangle(x, 90, 4, 2, textColour);
    }
}

void playSound(const uint32_t *notes, const uint32_t *duration, uint32_t length)
{
	for (uint32_t i = 0; i < length; i++)
	{
		playNote(notes[i]);
		delay(duration[i]);
		TIM14->CR1 &= ~(1 << 0);
	}
}

void setupKeyboard(void)
{
    USBH_Init(&hUSBHost, USBH_UserProcess, 0);
    USBH_RegisterClass(&hUSBHost, USBH_HID_CLASS);
    USBH_Start(&hUSBHost);
}

void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id)
{
    switch(id)
    {
        case HOST_USER_SELECT_CONFIGURATION:
        case HOST_USER_DISCONNECTION:
        case HOST_USER_CLASS_ACTIVE:
        case HOST_USER_CONNECTION:
            break;
    }
}

int main()
{
	int hinverted = 0;
	int vinverted = 0;
	int toggle = 0;
	int hmoved = 0;
	int vmoved = 0;
	int x = 50;
	int y = 50;
	int oldx = x;
	int oldy = y;
	int delayTime = 15;
	int score = 0;

	initClock();
	initSysTick();
	setupIO();
	setupKeyboard();
	initSound();

	while(1)
	{
		start:

		score = 0;

		drawMainMenu();
		
		while ((GPIOB->IDR & (1 << 4)) != 0)
		{
			USBH_Process(&hUSBHost);
			kb_info = USBH_HID_GetKeybdInfo(&hUSBHost);
			if (kb_info != NULL && kb_info->keys[0] == HID_KEYBOARD_ARROW_RIGHT)
				break;
		}

		delay(200);

		clear();

		int gameRunning = 1;

		// Game Starts

		// General wall variables:
		uint16_t gap = 30;
		uint16_t wallY = 0;
		uint16_t height = 15;
		uint16_t colour = RGBToWord(255, 0, 0);
		uint16_t w1 = (random() % (128 - gap));
		uint16_t x2 = w1 + gap;
		uint16_t w2 = 128 - x2;

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
				playSound(gameOverSound, gameOverLen, sizeof(gameOverSound) / sizeof(gameOverSound[0])); // Play Game Over sound
				delay(5000);
				goto start;
			}

			wallY = wallY + 1;

			if (wallY > 110)
			{
				playSound(scoreUpSound, scoreUpLen, sizeof(scoreUpSound) / sizeof(scoreUpSound[0]));
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

			// GPIO buttons
			if ((GPIOB->IDR & (1 << 4))==0) // right pressed
			{					
				if (x < 128)
				{
					x = x + 1;
					hmoved = 1;
					hinverted=0;
				}						
			}
			if ((GPIOB->IDR & (1 << 5))==0) // left pressed
			{			
				if (x > 0)
				{
					x = x - 1;
					hmoved = 1;
					hinverted=1;
				}			
			}
			if ( (GPIOA->IDR & (1 << 11)) == 0) // down pressed
			{
				if (y < 125)
				{
					y = y + 1;
					vmoved = 1;
					vinverted = 0;
				}
			}
			if ( (GPIOA->IDR & (1 << 8)) == 0) // up pressed
			{			
				if (y > 15)
				{
					y = y - 1;
					vmoved = 1;
					vinverted = 1;
				}
			}

			// USB keyboard
			USBH_Process(&hUSBHost);
			kb_info = USBH_HID_GetKeybdInfo(&hUSBHost);
			if (kb_info != NULL)
			{
				if (kb_info->keys[0] == HID_KEYBOARD_ARROW_RIGHT && x < 128) { x = x + 1; hmoved = 1; hinverted = 0; }
				if (kb_info->keys[0] == HID_KEYBOARD_ARROW_LEFT  && x > 0)   { x = x - 1; hmoved = 1; hinverted = 1; }
				if (kb_info->keys[0] == HID_KEYBOARD_ARROW_DOWN  && y < 125) { y = y + 1; vmoved = 1; vinverted = 0; }
				if (kb_info->keys[0] == HID_KEYBOARD_ARROW_UP    && y > 15)  { y = y - 1; vmoved = 1; vinverted = 1; }
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
			delay(delayTime);
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
        RCC->CR &= ~(1u<<24);
        while( (RCC->CR & (1 <<25)));
        
        FLASH->ACR |= (1 << 0);
        FLASH->ACR &=~((1u << 2) | (1u<<1));
        FLASH->ACR |= (1 << 4);
        RCC->CFGR &= ~((1u<<21) | (1u<<20) | (1u<<19) | (1u<<18));
        RCC->CFGR |= ((1<<21) | (1<<19) ); 
        RCC->CFGR |= (1<<14);
        RCC->CR |= (1<<24);        
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
	Port->PUPDR = Port->PUPDR &~(3u << BitNumber*2);
	Port->PUPDR = Port->PUPDR | (1u << BitNumber*2);
}
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode)
{
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
