/*
 * LED.c
 *
 *  Created on: Jan 21, 2025
 *      Author: Aland B.
 */

#include "main.h"
#include "LED.h"
#include "spi.h"
#include "gpio.h"


static uint8_t ledMatrix[8][8] = {0}; // 2D array to store the state of the LED matrix

/**
 * @brief Sends a command to the MAX7219 LED module.
 *
 * @param address The address of the register to configure.
 * @param data The data value to write to the register.
 */
void MAX7219_Send(uint8_t address, uint8_t data)
{
    uint8_t txData[2] = {address, data};
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, txData, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, GPIO_PIN_SET);

}

/**
 * @brief Initialize the MAX7219 LED module.
 *        Configures the settings for normal operation. These settings include:
 *        Scan-Limit, Decode-Mode, Shutdown-Mode, Display-Test-Mode,
 *        Intensity, and Display-Size.
 */

void MAX7219_Init(void)
{
    MAX7219_Send(0x0C, 0x01); // Shutdown-Mode close
    MAX7219_Send(0x0F, 0x00); // Display-Test-Mode close
    MAX7219_Send(0x0B, 0x07); // Scan-Limit for all digits
    MAX7219_Send(0x0A, 0x01); // Intensity as 10%
    MAX7219_Send(0x09, 0x00); // Decode-Mode close
    clearMatrix();
}

/**
 * @brief  Clears all pixels on the LED matrix by setting all registers to 0.
 */
void clearMatrix(void)
{
    for (uint8_t i = 1; i <= 8; i++)
    {
        MAX7219_Send(i, 0x00);
    }
}


/**
 * @brief Clears the LED matrix by setting all pixels to 0.
 */
void LED_Clear(void) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            ledMatrix[i][j] = 0;
        }
    }
    clearMatrix();
}

/**
 * @brief Sets a specific pixel on the LED matrix.
 *
 * @param x The x-coordinate of the pixel.
 * @param y The y-coordinate of the pixel.
 * @param value The value to set (1 to turn on, 0 to turn off).
 */
void LED_SetPixel(int x, int y, int value) {
    if (x < 0 || x >= 8 || y < 0 || y >= 8) {
        return; // Out of bounds
    }

    ledMatrix[y][x] = value; // Update the state of the LED matrix

    uint8_t row = y + 1; // MAX7219 rows are 1-indexed
    uint8_t currentRowData = 0;

    for (int i = 0; i < 8; i++) {
        if (ledMatrix[y][i]) {
            currentRowData |= (1 << i);
        }
    }

    MAX7219_Send(row, currentRowData); // Send updated row data
}

void LED_Update(void) {
	for (int y = 0; y < 8; y++) {
	        uint8_t row = y + 1; // MAX7219 rows are 1-indexed
	        uint8_t currentRowData = 0;

	        for (int x = 0; x < 8; x++) {
	            if (ledMatrix[y][x]) {
	                currentRowData |= (1 << x);
	            }
	        }

	        MAX7219_Send(row, currentRowData); // Send updated row data
	    }
}



/**
 * @brief Displays a welcome animation on the LED matrix.
 *
 * This function shows a simple animation on the LED matrix when the STM32 is turned on.
 */
void welcome_animation(void) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            LED_SetPixel(i, j, 1);
            LED_Update();
            HAL_Delay(50);
        }
    }
    HAL_Delay(500);
    LED_Clear();
    LED_Update();
}
