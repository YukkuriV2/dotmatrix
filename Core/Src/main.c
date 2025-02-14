/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"
#include <stdlib.h>
#include <time.h>
#include "LED.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    Point body[8 * 8];
    int length;
    Point direction;
} Snake;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define MATRIX_WIDTH 8
#define MATRIX_HEIGHT 8
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
Point fruit;
Snake snake;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void init_game(void);
void move_snake(void);
void update_direction(void);
void display_game(void);
void check_collision(Point new_head);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/**
 * @brief Initializes the game state.
 *
 * This function sets the initial length and position of the snake,
 * sets the initial direction, and places the fruit at a random position.
 */

void init_game() {
    snake.length = 1;
    snake.body[0].x = MATRIX_WIDTH / 2;
    snake.body[0].y = MATRIX_HEIGHT / 2;
    snake.direction.x = 1;
    snake.direction.y = 0;
    fruit.x = rand() % MATRIX_WIDTH;
    fruit.y = rand() % MATRIX_HEIGHT;
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
    // Check for wall collision
    if (new_head.x < 0 || new_head.x >= MATRIX_WIDTH || new_head.y < 0 || new_head.y >= MATRIX_HEIGHT) {
        LED_Clear();
        const char *msg = "Game Over: Wall Collision\r\n";
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
        init_game();
        return;
    }

    // Check for self-collision
    for (int i = 0; i < snake.length; i++) {
        if (new_head.x == snake.body[i].x && new_head.y == snake.body[i].y) {
            snake.length = i; // Reduce length to the point of collision
            const char *msg = "Snake ate itself\r\n";
            HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
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
}
/**
 * @brief Updates the snake's direction towards the fruit.
 *
 * This function changes the snake's direction to move towards the fruit.
 * The direction is updated based on the current position of the snake's head
 * and the position of the fruit.
 */
void update_direction() {
    if (snake.body[0].x < fruit.x) {
        snake.direction.x = 1;
        snake.direction.y = 0;
    } else if (snake.body[0].x > fruit.x) {
        snake.direction.x = -1;
        snake.direction.y = 0;
    } else if (snake.body[0].y < fruit.y) {
        snake.direction.x = 0;
        snake.direction.y = 1;
    } else if (snake.body[0].y > fruit.y) {
        snake.direction.x = 0;
        snake.direction.y = -1;
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

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
    srand(time(NULL));

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  MAX7219_Init();
  /* USER CODE BEGIN 2 */
  welcome_animation();
  init_game();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      update_direction();
      move_snake();
      display_game();
      HAL_Delay(500);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */

}
/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 16;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
