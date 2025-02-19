/*
 * joystick.h
 *
 *  Created on: Feb 15, 2025
 *      Author: Muhammed A.
 */

#ifndef INC_JOYSTICK_H_
#define INC_JOYSTICK_H_

#include <stdint.h>

#define PCA9538A_ADDR  0xE0
#define PCA9538A_ADDR_WRITE  0xE1
//#define JOY_DOWN      0x1E
//#define JOY_RIGHT     0x1B
//#define JOY_LEFT      0x17
//#define JOY_UP        0x0F
//#define JOY_BUTTON    0x1D

#define JOY_DOWN      0x0F
#define JOY_RIGHT     0x17
#define JOY_LEFT      0x1B
#define JOY_UP        0x1E
#define JOY_BUTTON    0x1D

void PCA9538A_Init(void);
uint8_t PCA9538A_ReadInput(void);
uint8_t debounce_joystick_input(uint8_t input_mask);


#endif /* INC_JOYSTICK_H_ */
