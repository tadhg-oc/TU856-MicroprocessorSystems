#include <stm32f031x6.h>   // Device-specific definitions for STM32F031 microcontroller
#include "display.h"       // Custom display driver (drawing text, shapes, images)
#include "sound.h"         // Basic sound functions (playNote etc.)
#include "musical_notes.h" // Frequency definitions for musical notes (C4, E4, etc.)
#include <stdlib.h>        // Standard library (used for random numbers)
#include <stdio.h>         // Standard input/output (debugging, printing)


// Functions are declared below: START HERE
// These tell the compiler what functions exist later in the code

void initClock(void);                      // Configure system clock
void initSysTick(void);                   // Setup SysTick timer (for delays/timing)
void SysTick_Handler(void);               // Interrupt handler for SysTick
void delay(volatile uint32_t dly);        // Delay function using SysTick
void setupIO();                           // Configure GPIO pins
int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py); // Check if point is inside rectangle
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber); // Enable pull-up resistor
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode); // Set pin mode


// Global variables are contained below: START HERE

volatile uint32_t milliseconds; 
// Stores elapsed milliseconds (updated in SysTick interrupt)

// volatile - ensures compiler doesn't optimize it away (it changes asynchronously)


// The follwing contaisn all the sound effects and the data associated with them

// Notes for score increase sound (ascending chord)
const uint32_t scoreUpSound[] = {C4, E4, G4, C5};

// Duration of each note (in ms)
const uint32_t scoreUpLen[] = {200, 200, 200, 400};


// Notes for game over sound (descending scale)
const uint32_t gameOverSound[] = {G4, F4, E4, D4, C4};

// Duration of each note
const uint32_t gameOverLen[] = {300, 300, 300, 300, 500};


// Image Sprites Data below: START HERE

// These arrays represent pixel data for a "slime" character
// Each value is a 16-bit color (likely RGB565 format)

// Default slime (facing forward)
const uint16_t slimeDefault[] = {
    // Massive array of pixel color values
    // 0 = transparent/black
    // 9293, 63383 = specific colors used in sprite
};

// Slime facing left
const uint16_t slimeLeft[] = {
    // Slight variation of sprite to show movement left
};

// Slime facing right
const uint16_t slimeRight[] = {
    // Mirrored/adjusted sprite for right movement
};

// Slime facing upward
const uint16_t slimeUp[] = {
    // Sprite variant for upward movement
};


// This is the main menu function that appears at the beginning asking you to press right to start the game.

void drawMainMenu()
{
    // Convert RGB (255,255,255) into 16-bit display format
    uint16_t textColour = RGBToWord(255,255,255);

    // Background color (black)
    uint16_t bgColour = 0x0000;

    clear(); // Clear the screen completely

    // Draw title text at position (x=33, y=20)
    printText("Slime Run", 33, 20, textColour, bgColour);

    // Draw subtitle
    printText("-- Main Menu --", 12, 30, textColour, bgColour);

    // Instruction text
    printText("Right to Start", 15, 60, textColour, bgColour);

    // Draw slime image at (52,110), size 20x20 pixels & slimeDefault contains the pixel data

    putImage(52, 110, 20, 20, slimeDefault, 0, 0);

    // Draw decorative dashed line using small rectangles
    for (int x = 10; x < 118; x += 6)
    {
        // Draw small white rectangles spaced apart
        fillRectangle(x, 90, 4, 2, textColour);
    }
}


// Sound Playback Functions are below: START HERE

void playSound(const uint32_t *notes, const uint32_t *duration, uint32_t length)
{
    // Loop through each note in the sequence
    for (uint32_t i = 0; i < length; i++)
    {
        playNote(notes[i]); // Start playing current note

        delay(duration[i]); // Wait for the specified duration

        // Stop the sound by disabling timer (TIM14 used for PWM sound)
        TIM14->CR1 &= ~(1 << 0);
    }
}



int main()
{
    // PLAYER / MOVEMENT STATE VARIABLES
    int hinverted = 0;  // Tracks horizontal direction inversion (e.g. bounce or flip)
    int vinverted = 0;  // Tracks vertical direction inversion
    int toggle = 0;     // Likely used for animation toggling (not shown yet)
    int hmoved = 0;     // Tracks if player moved horizontally this frame
    int vmoved = 0;     // Tracks if player moved vertically this frame

    // The player's position and co-ordiantes
    int x = 50;         // Current X position of player (slime)
    int y = 50;         // Current Y position

    int oldx = x;       // Previous X position (used for clearing/redrawing)
    int oldy = y;       // Previous Y position

    int delayTime = 15; // Game speed (lower = faster game loop)
    int score = 0;      // Player score

    // Infinite loop IS DECLARED MAKING THE GAME RUN FOREVER
    while(1)
    {
        // INITIAL SETUP (RUNS EACH FULL RESET)
        initClock();     // Configure system clock
        initSysTick();   // Setup timing system (used for delay)
        setupIO();       // Configure input/output pins (buttons etc.)
        initSound();     // Initialize sound system (timer/PWM)

        // Label used to restart game after game over
        start:

        score = 0;       // Reset score

        drawMainMenu();  // Show main menu screen
        
        // Wait until button (PB4) is pressed
        // GPIOB->IDR reads input register
        while ((GPIOB->IDR & (1 << 4)) != 0)
        {	
            // Do nothing (busy wait)
        }

        delay(200); // Small delay to debounce button press

        clear();    // Clear screen before starting game

        int gameRunning = 1; // Game loop control flag

        // GAME INITIALISATION

        // Wall (obstacle) settings
        uint16_t gap = 30;         // Gap between walls (player passes through this)
        uint16_t wallY = 0;        // Current vertical position of wall
        uint16_t height = 15;      // Height of wall block
        uint16_t colour = RGBToWord(255, 0, 0); // Wall color (red)

        // Randomize wall gap position
        uint16_t w1 = (random() % (128 - gap)); // Left wall width
        uint16_t x2 = w1 + gap;                 // Start of right wall
        uint16_t w2 = 128 - x2;                 // Right wall width

        // MAIN GAME LOOP
        while(gameRunning == 1)
        {
            // Bottom Bar is drawn

            // Draw white background bar at bottom
            fillRectangle(0, 125, 128, 25, RGBToWord(255, 255, 255));

            // Draw score background box
            fillRectangle(35, 135, 50, 10, RGBToWord(255, 255, 255));

            // Convert score integer to string
            char scoreText[10];
            sprintf(scoreText, "%d", score);

            // Print score value
            printText(scoreText, 75, 135, RGBToWord(0, 0, 0), RGBToWord(255, 255, 255));

            // Print "Score:" label
            printText("Score:", 30, 135, RGBToWord(0, 0, 0), RGBToWord(255, 255, 255));


            // Erase the old wall

            // Draw over previous wall position in black (background)
            fillRectangle(0, wallY, w1, height, 0);
            fillRectangle(x2, wallY, w2, height, 0);


            // Collision detection is below:

            // Check if player (x,y) is inside either wall block
            if (isInside(0, wallY, w1, height, x, y) || 
                isInside(x2, wallY, w2, height, x, y))
            {
                // When the platyer hits the wall - game is over.

                clear();

                // Display Game Over text (double size)
                printTextX2("Game Over!", 5, 40, RGBToWord(255, 0, 0), RGBToWord(0, 0, 0));

                // Show final score
                printText("Final Score:", 15, 90, RGBToWord(255, 255, 255), RGBToWord(0, 0, 0));
                printText(scoreText, 105, 90, RGBToWord(255, 255, 255), RGBToWord(0, 0, 0));

                // Play game over sound
                playSound(gameOverSound, gameOverLen, 
                          sizeof(gameOverSound) / sizeof(gameOverSound[0]));

                delay(5000); // Wait 5 seconds before restarting

                goto start;  // Jump back to main menu
            }


            // Moving the wall down
            wallY = wallY + 1;


            // =When the wall reaches the bottom
            if (wallY > 110)
            {
                // Play score sound
                playSound(scoreUpSound, scoreUpLen, 
                          sizeof(scoreUpSound) / sizeof(scoreUpSound[0]));

                wallY = 0;  // Reset wall to top

                gap = 30;   // Reset gap size

                // Generate new random gap position
                w1 = (random() % (128 - gap)); 
                x2 = w1 + gap; 
                w2 = 128 - x2;

                score = score + 1; // Increase score

                // Increase game speed gradually
                if (delayTime > 0.1)
                {
                    delayTime = delayTime - 0.3;
                }
            }


            // Draw a new wall position below: START HERE

            fillRectangle(0, wallY, w1, height, colour);   // Left wall
            fillRectangle(x2, wallY, w2, height, colour);  // Right wall


            // FRAME DELAY (CONTROLS GAME SPEED)
            delay(delayTime);


            // RESET MOVEMENT FLAGS FOR NEXT FRAME
            hmoved = vmoved = 0;     // No movement yet this frame
            hinverted = vinverted = 0; // Reset direction inversion flags
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
