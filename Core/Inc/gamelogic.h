/*
 * gamelogic.h
 *
 *  Created on: Feb 15, 2025
 *      Author: Aland B. & Muhammed A.
 */

#ifndef INC_GAMELOGIC_H_
#define INC_GAMELOGIC_H_


#define MATRIX_WIDTH 8
#define MATRIX_HEIGHT 8
#define DEBOUNCE_DELAY 50 // 50 ms debounce delay


typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    Point body[8 * 8];
    int length;
    Point direction;
} Snake;

void init_game(void);
void check_collision(Point new_head);
void move_snake(void);

void update_direction(void);
void display_game();

Point read_input(void);



#endif /* INC_GAMELOGIC_H_ */
