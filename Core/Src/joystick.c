/*
 * joystick.c
 *
 *  Created on: Feb 15, 2025
 *      Author: Muhammed A.
 */




#ifndef SRC_JOYSTICK_C_
#define SRC_JOYSTICK_C_

#include "joystick.h"
#include "main.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"
#include "gamelogic.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>

// Function to initialize the I/O expander
void PCA9538A_Init(void) {
    uint8_t config_data = 0xFF;  // Set all pins as inputs (0xFF means all bits are 1)
    HAL_I2C_Mem_Write(&hi2c1, PCA9538A_ADDR_WRITE, 0x03, I2C_MEMADD_SIZE_8BIT, &config_data, 1, HAL_MAX_DELAY);
}

// Function to read the input port register (00h)
uint8_t PCA9538A_ReadInput(void) {
    uint8_t input_data = 0;
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c1, PCA9538A_ADDR, 0x00, I2C_MEMADD_SIZE_8BIT, &input_data, 1, HAL_MAX_DELAY);


    if (status == HAL_OK) {
        return input_data;  // Return the input data (joystick values)
    } else {
        return 0xFF;  // Return an error code if the read fails
    }
}


/**
 * @brief Reads the input from the joystick.
 *
 * This function reads the current state of the joystick and returns the direction.
 * The direction is represented as a Point where x and y can be -1, 0, or 1.
 *
 * @return Point The direction based on the joystick input.
 */

Point read_input(void) {
    Point direction = {0, 0};
    uint8_t joystick_input = PCA9538A_ReadInput();

    // Print raw input
    char debug_msg[50];
    sprintf(debug_msg, "Raw Joystick Input: 0x%02X\r\n", joystick_input);
    HAL_UART_Transmit(&huart2, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);

    // Ignore invalid input 0x1F
    if (joystick_input == 0x1F) {
        HAL_UART_Transmit(&huart2, (uint8_t*)"Ignoring invalid input 0x1F\r\n", 29, HAL_MAX_DELAY);
        return direction;
    }

    HAL_StatusTypeDef i2c_status;

    if (joystick_input == JOY_DOWN) {
        HAL_UART_Transmit(&huart2, (uint8_t*)"Direction: down\r\n", 17, HAL_MAX_DELAY);
        uint8_t data = JOY_DOWN;
        i2c_status = HAL_I2C_Mem_Write(&hi2c1, PCA9538A_ADDR_WRITE, 0x03, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);
        direction.x = 0;
        direction.y = -1;
    }
    else if (joystick_input == JOY_RIGHT) {
        HAL_UART_Transmit(&huart2, (uint8_t*)"Direction: right\r\n", 18, HAL_MAX_DELAY);
        uint8_t data = JOY_RIGHT;
        i2c_status = HAL_I2C_Mem_Write(&hi2c1, PCA9538A_ADDR_WRITE, 0x03, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);
        direction.x = 1;
        direction.y = 0;
    }
    else if (joystick_input == JOY_LEFT) {
        HAL_UART_Transmit(&huart2, (uint8_t*)"Direction: left\r\n", 17, HAL_MAX_DELAY);
        uint8_t data = JOY_LEFT;
        i2c_status = HAL_I2C_Mem_Write(&hi2c1, PCA9538A_ADDR_WRITE, 0x03, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);
        direction.x = -1;
        direction.y = 0;
    }
    else if (joystick_input == JOY_UP) {
        HAL_UART_Transmit(&huart2, (uint8_t*)"Direction: up\r\n", 15, HAL_MAX_DELAY);
        uint8_t data = JOY_UP;
        i2c_status = HAL_I2C_Mem_Write(&hi2c1, PCA9538A_ADDR_WRITE, 0x03, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);
        direction.x = 0;
        direction.y = 1;
    }
    else {
        HAL_UART_Transmit(&huart2, (uint8_t*)"Invalid input\r\n", 15, HAL_MAX_DELAY);
    }

    // Debug: Check I2C write success
    char i2c_debug_msg[50];
    sprintf(i2c_debug_msg, "I2C Write Status: %d\r\n", i2c_status);
    HAL_UART_Transmit(&huart2, (uint8_t*)i2c_debug_msg, strlen(i2c_debug_msg), HAL_MAX_DELAY);

    return direction;
}

#endif /* SRC_JOYSTICK_C_ */
