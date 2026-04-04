// Headers:
#include <stm32f031x6.h> // STM32F031x6 microcontroller-specific definitions
#include "display.h" // Display functions
#include "sound.h" // For sound effects
#include "musical_notes.h" // For more complex sounds
#include <stdlib.h> // For random number generation

// Function prototypes
void initClock(void); // Initialize the system clock
void initSysTick(void); // Initialize the SysTick timer
void SysTick_Handler(void); // SysTick interrupt handler
void delay(volatile uint32_t dly); // Delay function
void setupIO(); // Setup GPIO pins
int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py); // Check if a point is inside a rectangle
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber); // Enable pull-up resistor for a GPIO pin
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode); // Set GPIO pin mode

// Global variable to track milliseconds (used for delays)
volatile uint32_t milliseconds;

// Turn LEDs ON
void RedOn()    { GPIOA->ODR |= (1 << 12); } // Turn on Red LED (PA12)
void GreenOn()  { GPIOA->ODR |= (1 << 9);  } // Turn on Green LED (PA9)
void YellowOn() { GPIOA->ODR |= (1 << 10); } // Turn on Yellow LED (PA10)

// Turn LEDs OFF
void RedOff()    { GPIOA->ODR &= ~(1 << 12); } // Turn off Red LED (PA12)
void GreenOff()  { GPIOA->ODR &= ~(1 << 9);  } // Turn off Green LED (PA9)
void YellowOff() { GPIOA->ODR &= ~(1 << 10); } // Turn off Yellow LED (PA10)

// Sound data for score-up and game-over events
const uint32_t scoreUpSound[] = {C4, E4, G4, C5}; // Notes for score-up sound
const uint32_t scoreUpLen[] = {200, 200, 200, 400}; // Durations for score-up sound

const uint32_t gameOverSound[] = {G4, F4, E4, D4, C4}; // Notes for game-over sound
const uint32_t gameOverLen[] = {300, 300, 300, 300, 500}; // Durations for game-over sound

// Default sprite for the slime character (used for the default orientation)
const uint16_t slimeDefault[] =
{
    // Pixel data for the default slime sprite
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9293,9293,9293,9293,9293,9293,0,0,0,0,0,0,0,0,0,0,0,0,0,9293,9293,63383,63383,63383,63383,9293,9293,0,0,0,0,0,0,0,0,0,0,0,9293,9293,63383,63383,63383,63383,63383,63383,9293,9293,0,0,0,0,0,0,0,0,0,9293,9293,63383,63383,63383,63383,63383,63383,63383,63383,9293,9293,0,0,0,0,0,0,0,9293,9293,63383,63383,0,0,63383,63383,0,0,63383,63383,9293,9293,0,0,0,0,0,0,9293,63383,63383,63383,0,0,63383,63383,0,0,63383,63383,63383,9293,0,0,0,0,0,9293,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,9293,0,0,0,0,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,0,0,0,9293,9293,63383,63383,63383,63383,63383,0,63383,63383,0,63383,63383,63383,63383,63383,9293,9293,0,0,9293,63383,63383,63383,63383,63383,63383,63383,0,0,63383,63383,63383,63383,63383,63383,63383,9293,0,0,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,0,0,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

// Sprite for the slime character facing left
const uint16_t slimeLeft[] =
{
    // Pixel data for the left-facing slime sprite
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9293,9293,9293,9293,9293,9293,0,0,0,0,0,0,0,0,0,0,0,0,0,9293,9293,63383,63383,63383,63383,9293,9293,0,0,0,0,0,0,0,0,0,0,0,9293,9293,63383,63383,63383,63383,63383,63383,9293,9293,0,0,0,0,0,0,0,0,0,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,9293,0,0,0,0,0,0,0,9293,9293,0,0,63383,63383,63383,63383,63383,63383,63383,63383,9293,9293,0,0,0,0,0,0,9293,63383,0,0,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,0,0,0,0,0,9293,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,9293,0,0,0,0,9293,63383,63383,0,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,0,0,0,9293,9293,0,0,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,9293,0,0,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,0,0,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,0,0,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

// Sprite for the slime character facing right
const uint16_t slimeRight[] =
{
    // Pixel data for the right-facing slime sprite
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9293,9293,9293,9293,9293,9293,0,0,0,0,0,0,0,0,0,0,0,0,0,9293,9293,63383,63383,63383,63383,9293,9293,0,0,0,0,0,0,0,0,0,0,0,9293,9293,63383,63383,63383,63383,63383,63383,9293,9293,0,0,0,0,0,0,0,0,0,9293,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,0,0,0,0,0,0,0,9293,9293,63383,63383,63383,63383,63383,63383,63383,63383,0,0,9293,9293,0,0,0,0,0,0,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,0,0,63383,9293,0,0,0,0,0,9293,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,9293,0,0,0,0,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,0,63383,63383,9293,0,0,0,9293,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,0,0,9293,9293,0,0,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,0,0,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,0,0,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

// Sprite for the slime character facing up
const uint16_t slimeUp[] =
{
    // Pixel data for the upward-facing slime sprite
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,9293,9293,9293,9293,9293,9293,0,0,0,0,0,0,0,0,0,0,0,0,0,9293,9293,63383,63383,63383,63383,9293,9293,0,0,0,0,0,0,0,0,0,0,0,9293,9293,63383,63383,63383,63383,63383,63383,9293,9293,0,0,0,0,0,0,0,0,0,9293,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,0,0,0,0,0,0,0,9293,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,9293,0,0,0,0,0,0,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,0,0,0,0,0,9293,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,9293,0,0,0,0,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,0,0,0,9293,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,9293,0,0,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,0,0,9293,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,63383,9293,0,0,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,9293,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};

void drawMainMenu()
{
    // Set text and background colors
    uint16_t textColour = RGBToWord(255,255,255); // White text
    uint16_t bgColour = 0x0000; // Black background

    clear(); // Clear the screen

    // Display the main menu text
    printText("Slime Run", 33, 20, textColour, bgColour);
    printText("-- Main Menu --", 12, 30, textColour, bgColour);
    printText("Right to Start", 15, 60, textColour, bgColour);

    // Display the default slime sprite
    putImage(52, 110, 20, 20, slimeDefault, 0, 0);

    // Draw a decorative line
    for (int x = 10; x < 118; x += 6)
    {
        fillRectangle(x, 90, 4, 2, textColour);
    }
}

void playSound(const uint32_t *notes, const uint32_t *duration, uint32_t length)
{
    // Play a sequence of notes with specified durations
    for (uint32_t i = 0; i < length; i++)
    {
        playNote(notes[i]); // Play the current note
        delay(duration[i]); // Wait for the note duration
        TIM14->CR1 &= ~(1 << 0); // Stop the timer after each note
    }
}

int main()
{
    // Initialize game variables
    int hinverted = 0; // Horizontal inversion flag
    int vinverted = 0; // Vertical inversion flag
    int toggle = 0;    // Toggle state for animation
    int hmoved = 0;    // Horizontal movement flag
    int vmoved = 0;    // Vertical movement flag
    int x = 50;        // Initial x-coordinate of the slime
    int y = 50;        // Initial y-coordinate of the slime
    int oldx = x;      // Previous x-coordinate
    int oldy = y;      // Previous y-coordinate
    int delayTime = 15; // Delay time for frame updates
    int score = 0;     // Player's score

    // Initialize hardware and peripherals
    initClock();
    initSysTick();
    setupIO();
    initLED();
    initSound();

    while(1) // Main game loop
    {
        start: // Restart point for the game

        score = 0; // Reset the score

        drawMainMenu(); // Display the main menu
        RedOff(); // Turn off the red LED
        GreenOn(); // Turn on the green LED

        // Wait for the start button to be pressed
        while ((GPIOB->IDR & (1 << 4)) != 0) {}

        delay(200); // Debounce delay
        GreenOff(); // Turn off the green LED
        clear(); // Clear the screen for the game

        int gameRunning = 1; // Game running flag

        // Initialize wall variables
        uint16_t gap = 30; // Gap between walls
        uint16_t wallY = 0; // Initial wall Y-coordinate
        uint16_t height = 15; // Wall height
        uint16_t colour = RGBToWord(255, 0, 0); // Wall color (red)
        uint16_t w1 = (random() % (128 - gap)); // Width of the first wall
        uint16_t x2 = w1 + gap; // X-coordinate of the second wall
        uint16_t w2 = 128 - x2; // Width of the second wall

        while(gameRunning == 1) // Game loop
        {
            YellowOn(); // Turn on the yellow LED to indicate the game is running

            // Draw the score
            fillRectangle(0, 125, 128, 25, RGBToWord(255, 255, 255)); // Clear the score area
            char scoreText[10];
            sprintf(scoreText, "%d", score); // Convert score to string
            printText(scoreText, 75, 135, RGBToWord(0, 0, 0), RGBToWord(255, 255, 255)); // Display the score
            printText("Score:", 30, 135, RGBToWord(0, 0, 0), RGBToWord(255, 255, 255)); // Display "Score:"

            // Draw the walls
            fillRectangle(0, wallY, w1, height, 0); // Draw the first wall
            fillRectangle(x2, wallY, w2, height, 0); // Draw the second wall

            // Check for collisions with the walls
            if (isInside(0, wallY, w1, height, x, y) || isInside(x2, wallY, w2, height, x, y))
            {
                // Collision detected, end the game
                YellowOff(); // Turn off the yellow LED
                RedOn(); // Turn on the red LED
                clear(); // Clear the screen
                printTextX2("Game Over!", 5, 40, RGBToWord(255, 0, 0), RGBToWord(0, 0, 0)); // Display "Game Over!"
                printText("Final Score:", 15, 90, RGBToWord(255, 255, 255), RGBToWord(0, 0, 0)); // Display "Final Score:"
                printText(scoreText, 105, 90, RGBToWord(255, 255, 255), RGBToWord(0, 0, 0)); // Display the final score
                playSound(gameOverSound, gameOverLen, sizeof(gameOverSound) / sizeof(gameOverSound[0])); // Play the game-over sound
                delay(5000); // Wait for 5 seconds
                goto start; // Restart the game
            }

            wallY = wallY + 1; // Move the walls down

            if (wallY > 110) // If the walls move off the screen
            {
                // Reset the walls and increase the score
                playSound(scoreUpSound, scoreUpLen, sizeof(scoreUpSound) / sizeof(scoreUpSound[0])); // Play the score-up sound
                wallY = 0; // Reset the wall Y-coordinate
                gap = 30; // Reset the gap
                w1 = (random() % (128 - gap)); // Generate a new first wall width
                x2 = w1 + gap; // Calculate the new second wall X-coordinate
                w2 = 128 - x2; // Calculate the new second wall width
                score = score + 1; // Increment the score

                if (delayTime > 0.1) // Decrease the delay time to increase game speed
                {
                    delayTime = delayTime - 0.3;
                }
            }

            // Draw the walls
            fillRectangle(0, wallY, w1, height, colour); // Draw the first wall
            fillRectangle(x2, wallY, w2, height, colour); // Draw the second wall

            hmoved = vmoved = 0; // Reset movement flags
            hinverted = vinverted = 0; // Reset inversion flags

            // Handle player movement based on button presses
            if ((GPIOB->IDR & (1 << 4)) == 0) // Right button pressed
            {					
                if (x < 108) // Ensure the player doesn't move out of bounds
                {
                    x = x + 1; // Move right
                    hmoved = 1; // Set horizontal movement flag
                    hinverted = 0; // Reset horizontal inversion
                }						
            }
            if ((GPIOB->IDR & (1 << 5)) == 0) // Left button pressed
            {			
                if (x > 0) // Ensure the player doesn't move out of bounds
                {
                    x = x - 1; // Move left
                    hmoved = 1; // Set horizontal movement flag
                    hinverted = 1; // Set horizontal inversion
                }			
            }
            if ((GPIOA->IDR & (1 << 11)) == 0) // Down button pressed
            {
                if (y < 110) // Ensure the player doesn't move out of bounds
                {
                    y = y + 1; // Move down
                    vmoved = 1; // Set vertical movement flag
                    vinverted = 0; // Reset vertical inversion
                }
            }
            if ((GPIOA->IDR & (1 << 8)) == 0) // Up button pressed
            {			
                if (y > 15) // Ensure the player doesn't move out of bounds
                {
                    y = y - 1; // Move up
                    vmoved = 1; // Set vertical movement flag
                    vinverted = 1; // Set vertical inversion
                }
            }

            if ((vmoved) || (hmoved)) // If there was movement
            {
                // Redraw the sprite only if there was movement
                fillRectangle(oldx, oldy, 12, 16, 0); // Clear the old sprite
                oldx = x; // Update the old x-coordinate
                oldy = y; // Update the old y-coordinate					
                if (hmoved) // If horizontal movement occurred
                {
                    if (hinverted) // If moving left
                    {
                        putImage(x, y, 20, 20, slimeLeft, 0, 0); // Draw the left-facing sprite
                    }
                    else // If moving right
                    {
                        putImage(x, y, 20, 20, slimeRight, 0, 0); // Draw the right-facing sprite
                    }
                    toggle = toggle ^ 1; // Toggle the animation state
                }
                else if (vmoved) // If vertical movement occurred
                {
                    if (vinverted) // If moving up
                    {
                        putImage(x, y, 20, 20, slimeUp, 0, 0); // Draw the upward-facing sprite
                    }
                    else // If moving down
                    {
                        putImage(x, y, 20, 20, slimeDefault, 0, 0); // Draw the default sprite
                    }
                }
            }
            delay(delayTime); // Add a delay for the frame update
        }
        return 0; // End of the program
    }
}

// Initialize the SysTick timer for generating periodic interrupts
void initSysTick(void)
{
    SysTick->LOAD = 48000; // Set reload value for 1ms interrupts (assuming 48 MHz clock)
    SysTick->CTRL = 7; // Enable SysTick, enable interrupts, and use processor clock
    SysTick->VAL = 10; // Reset the current value register
    __asm(" cpsie i "); // Enable interrupts globally
}

// SysTick interrupt handler
void SysTick_Handler(void)
{
    milliseconds++; // Increment the global milliseconds counter
}

// Initialize the system clock
void initClock(void)
{
    RCC->CR &= ~(1u << 24); // Disable the PLL
    while (RCC->CR & (1 << 25)); // Wait for the PLL to unlock

    FLASH->ACR |= (1 << 0); // Enable the prefetch buffer
    FLASH->ACR |= (1 << 4); // Set 1 wait state for the flash memory

    RCC->CFGR |= ((1 << 21) | (1 << 19)); // Set PLL multiplier to 6 (48 MHz system clock)
    RCC->CR |= (1 << 24); // Enable the PLL
    RCC->CFGR |= (1 << 1); // Set the PLL as the system clock
}

// Initialize the LEDs
void initLED()
{
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN; // Enable the GPIOA clock
    GPIOA->MODER |= ((1 << (12 * 2)) | (1 << (9 * 2)) | (1 << (10 * 2))); // Set PA12, PA9, and PA10 as output
}

// Delay function using the global milliseconds counter
void delay(volatile uint32_t dly)
{
    uint32_t end_time = dly + milliseconds; // Calculate the end time
    while (milliseconds != end_time) // Wait until the delay time has passed
        __asm(" wfi "); // Enter low-power mode until the next interrupt
}

// Enable the pull-up resistor for a specific GPIO pin
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber)
{
    Port->PUPDR &= ~(3u << (BitNumber * 2)); // Clear the pull-up/pull-down bits
    Port->PUPDR |= (1u << (BitNumber * 2)); // Enable the pull-up resistor
}

// Set the mode of a specific GPIO pin
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode)
{
    Port->MODER &= ~(3u << (BitNumber * 2)); // Clear the mode bits
    Port->MODER |= (Mode << (BitNumber * 2)); // Set the mode (input, output, etc.)
}

// Check if a point is inside a rectangle
int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py)
{
    uint16_t x2 = x1 + w; // Calculate the right edge of the rectangle
    uint16_t y2 = y1 + h; // Calculate the bottom edge of the rectangle
    return (px >= x1 && px <= x2 && py >= y1 && py <= y2); // Return true if the point is inside the rectangle
}

// Setup GPIO pins for the game
void setupIO()
{
    RCC->AHBENR |= (1 << 18) + (1 << 17); // Enable GPIOA and GPIOB clocks
    display_begin(); // Initialize the display

    // Configure GPIO pins as input
    pinMode(GPIOB, 4, 0); // Set GPIOB pin 4 as input
    pinMode(GPIOB, 5, 0); // Set GPIOB pin 5 as input
    pinMode(GPIOA, 8, 0); // Set GPIOA pin 8 as input
    pinMode(GPIOA, 11, 0); // Set GPIOA pin 11 as input

    // Enable pull-up resistors for the input pins
    enablePullUp(GPIOB, 4); // Enable pull-up for GPIOB pin 4
    enablePullUp(GPIOB, 5); // Enable pull-up for GPIOB pin 5
    enablePullUp(GPIOA, 8); // Enable pull-up for GPIOA pin 8
    enablePullUp(GPIOA, 11); // Enable pull-up for GPIOA pin 11
}
