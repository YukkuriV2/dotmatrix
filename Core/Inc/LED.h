/*
 * LED.h
 *
 *  Created on: Jan 22, 2025
 *      Author: Aland B.
 */

#ifndef INC_LED_H_
#define INC_LED_H_

void MAX7219_Send(uint8_t address, uint8_t data);
void MAX7219_Init(void);
void clearMatrix(void);
void displaySymbol(uint8_t symbolIndex);

void LED_Clear(void);
void LED_SetPixel(int x, int y, int value);
void LED_Update(void);
void welcome_animation(void);



#include <stdint.h>


#endif /* INC_LED_H_ */
