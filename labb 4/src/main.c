///////////////////////////////////////////////////////////////////////////////
// Lab 4: Write a little game!
// ===========================================================================
// The introduction to this lab will be a few smaller assignments as usual, 
// but after that, it is up to you to use what you have learned to write a 
// simple game.
// 
// The game will play on the 480x320 TFT display, so you need to connect that 
// in the simulator. You will not have to program that directly, instead you 
// will use the tftlib library we have provided. 
// It has the following simple API: 
// - tft_init() to initialize the display (call this once at the start of your 
//   program)
// - tft_ellipse(xc, yc, rx, ry, color, filled) to draw an ellipse (filled or not)
//     - xc, yc are the center coordinates of the ellipse
//     - rx, ry are the radius along the x and y axes, respectively
// - tft_rect(x1, y1, w, h, color, filled) to draw a rectangle (filled or not)
// - tft_line(x1, y1, x2, y2, color) to draw a line
// - tft_pixel(x, y, color) to draw a single pixel
// - tft_sprite(x, y, data, w, h) to draw a sprite (a small bitmap) at the 
//   given location.
// 
///////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "../tftlib/tft.h"
#include "sprites/sprite_17_2.h"
#include "afio.h"
#include "exti.h"
#include "gpio.h"
#include "interrupts.h"
#include "keypad.h"
#include "music.h"
#include "pfic.h"
#include "systick.h"
#include "vector_table.h"

// RGB565 color helpers
#define RGB565(r, g, b) ((((r) & 0x1F) << 11) | (((g) & 0x3F) << 5) | ((b) & 0x1F))
#define COLOR_BLACK  0x0000
#define COLOR_WHITE  0xFFFF
#define COLOR_RED    0xF800
#define COLOR_GREEN  0x07E0
#define COLOR_BLUE   0x001F

#define SOLUTION 1
#define width 479
#define height 319


volatile int update = 0;

typedef struct {
    int x;
    int y;
    int speed_x;
    int speed_y;
    int size;
} Ball;

typedef struct {
    int x;
    int y;
    int speed_y;
    int size;
    int score;
} Paddle;

#define PWidth 10
#define PSpeed 3
#define PUpSpeed -3
Paddle P1 = {width-PWidth, 160, 1, 50, 0};
Paddle P2 = {PWidth, 160, 1, 50, 0};

const int RightCollisionWall = width-PWidth;
const int LeftCollisionWall = PWidth; 

typedef enum {
    UNBINDED,
    P1UP,
    P1DOWN,
    P2UP,
    P2DOWN,
}keybindtype;

Ball ball = {240, 160, 3, 0, 10};  // x, y, speed_x, speed_y, size

uint32_t keybinds[16] = {
    P2UP, 0, 0, P1UP,
    0, 0, 0, 0,
    0, 0, 0, 0,
    P2DOWN, 0, 0, P1DOWN
};


void resetball(){
    ball.x = 240;
    ball.y = 160;
    ball.speed_x = 3;
    ball.speed_y = 1;
    ball.size = 10;  // x, y, speed_x, speed_y, size
}
// volatile int counter = 0;

__attribute__((interrupt("machine")))
void systick_handler() {
    //update = 1;  // just set a flag, very fast
    // counter++;  // increment every time handler fires
    // STK->SR = 0;  // write 0 to clear CNTIF

    //counter++;

    // Erase old ball
    tft_ellipse(ball.x, ball.y, ball.size, ball.size, COLOR_BLUE, 1);

    //Paddle for player 1
    tft_rect(P1.x, P1.y, PWidth, P1.size, COLOR_BLUE, 1 );

    //Paddle for player 2
    tft_rect(P2.x, P2.y, PWidth, P2.size, COLOR_BLUE, 1 );

    

    //Vänster collision
    if (ball.x <= ball.size+LeftCollisionWall ) {
        if(ball.y >= P2.y-(P2.size/2) && ball.y <= P2.y+(P2.size/2)){
            ball.speed_x = -ball.speed_x;
        }else{//lose
            P1.score++;
            resetball();
        }
        
    }
    if(ball.x >= RightCollisionWall -(ball.size+PWidth)){
        if(ball.y >= P1.y-(P1.size/2) && ball.y <= P1.y+(P1.size/2)){
            ball.speed_x = -ball.speed_x;
        }else{//lose
            P2.score++;
            resetball();
        }
    }
    if (ball.y <= ball.size || ball.y >= height - ball.size) {
        ball.speed_y = -ball.speed_y;
    }

    // Move
    ball.x += ball.speed_x;
    ball.y += ball.speed_y;
    P1.y += P1.speed_y;
    P2.y += P2.speed_y;

    //DRAW

    //Paddle for player 1
    tft_rect(P1.x, P1.y, PWidth, P1.size, COLOR_WHITE, 1 );

    //Paddle for player 2
    tft_rect(P2.x, P2.y, PWidth, P2.size, COLOR_WHITE, 1 );

    //ball
    tft_ellipse(ball.x, ball.y, ball.size, ball.size, COLOR_WHITE, 1);
    STK->SR &= 0x1;

}
__attribute__((interrupt("machine")))
void exti_handler() {
    uint8_t key = keypad();

 
    // Key pressed - start tone and show key number (1-16) on bargraph
    if(keybinds[key]==P1DOWN){
        P1.speed_y = PSpeed;
    }else if(keybinds[key]==P1UP){
        P1.speed_y = -PSpeed;
    }else if(keybinds[key]==P2DOWN){
        P2.speed_y = PSpeed;
    }else if(keybinds[key]==P2UP){
        P2.speed_y = -PSpeed;
    }
    //GPIOD->ODR = (GPIOD->ODR & 0x00FF) | ((key + 1) << 8); // & 0x00FF preserves pins D0–D7 (keypad pins), then (key + 1) << 8 writes the key number in binary to pins D8–D15 (bargraph)

    EXTI->INTFR = EXTI->INTFR;
}

int main(void){
    init_gpio();
    init_vector_table();
    init_interrupts();
    tft_init();


    tft_rect(0,0,479,319,COLOR_BLUE, 1);


    systick_periodic_micro(32000);  // 16000 ~60fps

    

    while(1) {
  
    }


    ///////////////////////////////////////////////////////////////////////////
    // Assignment 3: Make it bounce in 2D!
    // =======================================================================
    // Now modify the code to make the ball bounce in both x and y directions. 
    ///////////////////////////////////////////////////////////////////////////

    // <your code here>

    ///////////////////////////////////////////////////////////////////////////
    // Assignment 4: Use a sprite!
    // =======================================================================
    // Just for fun, let's use a sprite instead of an ellipse. A sprite is a 
    // small image, and we have included a bunch of them in the sprites/ 
    // folder that you can include if you want. 
    // 
    // You can find that "sprite_17_2.h" has been included above. Look at that
    // file and you will see that it is a 16x16 pixel image in the right format.
    // See the short README in the sprites/ folder for more info. 
    //
    // Note that this will be very slow on the simulator, but quite fast on 
    // hardware, so if you want to use sprites in your game, you might want to
    // use rectangles as placeholders while developing. 
    ///////////////////////////////////////////////////////////////////////////

    ///////////////////////////////////////////////////////////////////////////
    // Assignment 4: And now...
    // =======================================================================
    // If you add a little paddle (a rectangle) that can be moved with the 
    // keypad, you can easily complete a little pong game. Or you could do 
    // something completely different. 
    // 
    // See the canvas page for instructions and points. 
    ///////////////////////////////////////////////////////////////////////////
    return 0;
}