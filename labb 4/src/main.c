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
    int speed_x;
    int size;
} Paddle;

Paddle P1 = {width, 160, 0, 20};
Paddle P2 = {0, 160, 0, 20};

typedef enum {
    UNBINDED,
    P1UP,
    P1DOWN,
    P2UP,
    P2DOWN,
}keybindtype;

Ball ball = {240, 160, 1, 0, 10};  // x, y, speed_x, speed_y, size

uint32_t keybinds[16] = {
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0
};

// volatile int counter = 0;

__attribute__((interrupt("machine")))
void systick_handler() {
    //update = 1;  // just set a flag, very fast
    // counter++;  // increment every time handler fires
    // STK->SR = 0;  // write 0 to clear CNTIF

    //counter++;

        // Erase old ball
        tft_ellipse(ball.x, ball.y, ball.size, ball.size, COLOR_BLUE, 1);

        // Bounce
        if (ball.x <= ball.size || ball.x >= width - ball.size) {
            ball.speed_x = -ball.speed_x;
        }
        // if (ball.y <= ball.size || ball.y >= height - ball.size) {
        //     ball.speed_y = -ball.speed_y;
        // }

        // Move
        ball.x += ball.speed_x;
        // ball.y += ball.speed_y;

        // Draw new ball
        tft_ellipse(ball.x, ball.y, ball.size, ball.size, COLOR_WHITE, 1);
    STK->SR &= ~(1 << 0);

}
__attribute__((interrupt("machine")))
void exti_handler() {
    uint8_t key = keypad();

  if (key != 0xFF) {
    // Key pressed - start tone and show key number (1-16) on bargraph
    systick_periodic_micro(periods[key]);
    GPIOD->ODR = (GPIOD->ODR & 0x00FF) | ((key + 1) << 8); // & 0x00FF preserves pins D0–D7 (keypad pins), then (key + 1) << 8 writes the key number in binary to pins D8–D15 (bargraph)
  } else {
    // Key released - stop tone and turn off bargraph
    systick_stop();
    GPIOD->ODR = (GPIOD->ODR & 0x00FF);                      
  }

int main(void)
{
     init_gpio();
     init_vector_table();
     init_interrupts();
     tft_init();
    
    ///////////////////////////////////////////////////////////////////////////
    // Assignment 1: Initialize the display and fill it with a solid color
    // =======================================================================
    // Once tft_init() has completed, the screen should be all yellow. Fill it 
    // with a black background color (use COLOR_BLACK as the color argument) 
    // using tft_rect().
     //tft_rect(x1, y1, w, h, color, filled) to draw a rectangle (filled or not)
    // Note: On the simulator, w has to be given as 479, and h as 319, to fill
    //       the whole screen. 
    ///////////////////////////////////////////////////////////////////////////

    //extern void (*tft_rect)(int x, int y, int w, int h, uint16_t color, int filled);


    tft_rect(0,0,479,319,COLOR_BLUE, 1);


    
    ///////////////////////////////////////////////////////////////////////////
    // Assignment 2: Draw a ball in the middle of the screen
    // =======================================================================
    // Use the tft_ellipse() function to draw a ball in the middle of 
    // the screen. Choose any color for the ball. It should have  
    // The colors are in RGB565 format, which is a common 16-bit color format 
    // used in embedded graphics. It means that the most significant 5 bits are red,    
    // the next 6 bits are green, and the least significant 5 bits are blue.
    // 
    // You can use the RGB565(r, g, b) macro above to convert 8-bit RGB values 
    // to RGB565 format. For example, RGB565(255, 0, 0) will give you the 
    // RGB565 value for pure red.
    ///////////////////////////////////////////////////////////////////////////

    //tft_ellipse(ball.x, ball.y, ball.size, ball.size, RGB565(255, 255, 255), 1);

    ///////////////////////////////////////////////////////////////////////////
    // Assignment 3: Make it move!
    // =======================================================================
    // Now, make the ball move across the screen from left to right. We have 
    // given you a simple loop that will calculate the center and bounce on 
    // the edges, so just fill in the drawing code. 
    // 
    // A few hints: 
    // - Drawing the ball does not erase the previous one. On desktop games
    //   we usually clear the whole screen every frame. You can try that, but
    //   you will find that it is very slow. Instead, just erase the previous
    //   ball by drawing it again, in the background color. 
    // - When you do that, there will be flickering. To reduce the flickering
    //   you can pause for a short time after drawing the ball, before erasing 
    //   it. Use SysTick, or just a simple for-loop delay. 
    // - When you run this on hardware, timings might be very different. 
    ///////////////////////////////////////////////////////////////////////////

    systick_periodic_micro(32000);  // 16000 ~60fps

    

    while(1) {
    // if (update == 1) {
    //     update = 0;
    //     // counter++;

    //     // // Erase old ball
    //     // tft_ellipse(ball.x, ball.y, ball.size, ball.size, COLOR_BLUE, 1);

    //     // // Bounce
    //     // if (ball.x <= ball.size || ball.x >= width - ball.size) {
    //     //     ball.speed_x = -ball.speed_x;
    //     // }
    //     // // if (ball.y <= ball.size || ball.y >= height - ball.size) {
    //     // //     ball.speed_y = -ball.speed_y;
    //     // // }

    //     // // Move
    //     // ball.x += ball.speed_x;
    //     // // ball.y += ball.speed_y;

    //     // // Draw new ball
    //     // tft_ellipse(ball.x, ball.y, ball.size, ball.size, COLOR_WHITE, 1);
    // }
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
}