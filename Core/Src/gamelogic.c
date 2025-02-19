/*
 * gamelogic.c
 *
 *  Created on: Feb 15, 2025
 *      Author: Aland B. & Muhammed A.
 */

#include "gamelogic.h"
#include "usart.h"
#include "main.h"
#include "i2c.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"
#include "joystick.h"
#include "string.h"
#include <stdlib.h>
#include <time.h>
#include "LED.h"
#include <stdio.h>





Point fruit;
Snake snake;
int game_over = 0;




//uint8_t debounce_joystick_input(uint8_t input_mask) {
//    uint8_t state = PCA9538A_ReadInput();
//    char debug_msg[50];
//    sprintf(debug_msg, "Initial Joystick State: 0x%02X\r\n", state);
//    HAL_UART_Transmit(&huart2, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
//
//    if (state & input_mask) {
//        HAL_Delay(DEBOUNCE_DELAY);
//        state = PCA9538A_ReadInput();
//        sprintf(debug_msg, "Debounced Joystick State: 0x%02X\r\n", state);
//        HAL_UART_Transmit(&huart2, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
//
//        if (state & input_mask) {
//            return 1;
//        }
//    }
//    return 0;
//}


/**
 * @brief Initializes the game state.
 *
 * This function sets the initial length and position of the snake,
 * sets the initial direction, and places the fruit at a random position.
 */

void init_game() {
	MAX7219_Send(0x0A, 0x02);
    snake.length = 1;
    snake.body[0].x = MATRIX_WIDTH / 2;
    snake.body[0].y = MATRIX_HEIGHT / 2;
    snake.direction.x = 1;
    snake.direction.y = 0;
    fruit.x = rand() % MATRIX_WIDTH;
    fruit.y = rand() % MATRIX_HEIGHT;
    game_over = 0;
}


/**
 * @brief Checks for collisions with walls or the snake itself.
 *
 * @param new_head The new head position of the snake.
 *
 * This function checks if the snake's new head position collides with the walls
 * or with its own body. If a collision with the wall is detected, the game is reset.
 * If a collision with the snake's body is detected, the snake's length is reduced.
 */

void check_collision(Point new_head) {
    if (new_head.x < 0 || new_head.x >= MATRIX_WIDTH || new_head.y < 0 || new_head.y >= MATRIX_HEIGHT) {
        LED_Clear();
        const char *msg = "Game Over: Wall Collision\r\n";
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
        game_over = 1;
        return;
    }

    for (int i = 0; i < snake.length; i++) {
        if (new_head.x == snake.body[i].x && new_head.y == snake.body[i].y) {
            snake.length = i;
            const char *msg = "Snake ate itself\r\n";
            HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
            game_over = 1;
            break;
        }
    }
}

/**
 * @brief Moves the snake in the current direction.
 *
 * This function updates the snake's position based on its current direction.
 * It also checks for collisions and handles the snake eating the fruit.
 */
void move_snake() {


	Point new_head = {snake.body[0].x + snake.direction.x, snake.body[0].y + snake.direction.y};



	    check_collision(new_head);

	    if (game_over) {
	        return;
	    }

	    if (new_head.x == fruit.x && new_head.y == fruit.y) {
	        snake.length++;
	        fruit.x = rand() % MATRIX_WIDTH;
	        fruit.y = rand() % MATRIX_HEIGHT;
	        const char *msg = "Snake ate food\r\n";
	        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
	}

	    for (int i = snake.length - 1; i > 0; i--) {
			snake.body[i] = snake.body[i - 1];
		}




	    snake.body[0] = new_head;
	    // Debugging: Print the new head position
	    char debug_msg[50];
	    sprintf(debug_msg, "Moved Snake to: x=%d, y=%d\r\n", new_head.x, new_head.y);
	    HAL_UART_Transmit(&huart2, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
}
/**
 * @brief Updates the snake's direction towards the fruit.
 *
 * This function changes the snake's direction to move towards the fruit.
 * The direction is updated based on the current position of the snake's head
 * and the position of the fruit.
 */
void update_direction() {
	 Point new_direction = read_input();

	    // Debugging: Print the new direction
	    char debug_msg[50];
	    sprintf(debug_msg, "New Direction: x=%d, y=%d\r\n", new_direction.x, new_direction.y);
	    HAL_UART_Transmit(&huart2, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);

	    // Ensure the snake does not reverse direction
	    if ((new_direction.x != -snake.direction.x) && (new_direction.y != -snake.direction.y) &&
	        (new_direction.x != 0 || new_direction.y != 0)) {
	        snake.direction = new_direction;

	        // Debugging: Print the updated snake direction
	        sprintf(debug_msg, "Updated Snake Direction: x=%d, y=%d\r\n", snake.direction.x, snake.direction.y);
	        HAL_UART_Transmit(&huart2, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
	    } else {
	        // Debugging: Print a message if the direction is not updated
	        sprintf(debug_msg, "Direction not updated: x=%d, y=%d\r\n", snake.direction.x, snake.direction.y);
	        HAL_UART_Transmit(&huart2, (uint8_t*)debug_msg, strlen(debug_msg), HAL_MAX_DELAY);
	    }
}
/**
 * @brief Displays the current game state on the LED matrix.
 *
 * This function clears the LED matrix and sets the pixels for the snake's body
 * and the fruit. It then updates the LED matrix to reflect the current game state.
 */
void display_game() {
    LED_Clear();
    for (int i = 0; i < snake.length; i++) {
        LED_SetPixel(snake.body[i].x, snake.body[i].y, 1);
    }
    LED_SetPixel(fruit.x, fruit.y, 1);
    LED_Update();
}
