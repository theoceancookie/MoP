///////////////////////////////////////////////////////////////////////////////
// Lab Preparation 2
// ============================================================================
// In Lab 2 you will need to know how the ASCII display works, so in this lab 
// you will write a program that interacts with it, on the simulator. 
//
// YOU ARE ALSO EXPECTED TO HAVE DONE THE EXERCISE FOR LECTURE 05, so you know 
// how the keyboard input works. 
//
// IMPORTANT: Before you do this lab, go through Lecture06, Excercise 02, where
// you learn how to create short delays using systick. This exercise will not
// test that your delay functions work correctly, and if they don't work
// it might still work on the simulator but not on hardware. 
//
// In this assignment there are check_assignment_x() functions  already in place
// where we try to check that you have solved each assignment correctly. Once you 
// have passed an assignment, you can comment these out.
// 
// Please see the quickguide for a summary of the ASCII Display.
// 
// In the simulator IO Setup, connect 
// GPIO E(0-15)/SPI Display -> 18 LC/TFT Display   AND
// Serial Communications Interface 1 -> 06 Console
///////////////////////////////////////////////////////////////////////////////

#include <stdint.h>
#include <stdio.h>

void check_assignment_1(); 
void check_assignment_2_1(uint8_t command_or_data);
void check_assignment_2_2();
void check_assignment_4_1();
void check_assignment_4_2();
void check_assignment_4_3();
void check_assignment_4_4();
void check_assignment_4_5();
void check_assignment_5();

///////////////////////////////////////////////////////////////////////////////
// Register macros. 
///////////////////////////////////////////////////////////////////////////////

/* TODO: Put all your register macros here */
#define GPIOE_BASE  0x40011800

#define GPIOE_CRL   (*(volatile uint32_t *)(GPIOE_BASE + 0x00))
#define GPIOE_CRH   (*(volatile uint32_t *)(GPIOE_BASE + 0x04))
#define GPIOE_IDR   (*(volatile uint32_t *)(GPIOE_BASE + 0x08))
#define GPIOE_ODR   (*(volatile uint32_t *)(GPIOE_BASE + 0x0C))

///////////////////////////////////////////////////////////////////////////////
// Assignment 0: If you have done Lecture06 Exercise02, you already have
//               the code for this. Implement the delay functions below.
//               They are not tested in this assignment, so make sure they
//               work using the Lecture06 Exercise02.
///////////////////////////////////////////////////////////////////////////////
#define STK_BASE   0xE000F000

#define STK_CTLR   (*(volatile uint32_t *)(STK_BASE + 0x0))
#define STK_SR     (*(volatile uint32_t *)(STK_BASE + 0x4))

#define STK_CNT    (*(volatile uint64_t *)(STK_BASE + 0x8))
#define STK_CMP    (*(volatile uint64_t *)(STK_BASE + 0x10))

uint64_t ns_to_ticks(uint64_t ns)
{
	// Your code here
	return (ns * 144000000ULL) / 1000000000ULL;; 
}

void delay(uint64_t ns)
{
    uint64_t ticks = ns_to_ticks(ns);

    // Set compare value
    STK_CMP = ticks;

    // Clear status flag
    STK_SR = 0;

    // Configure SysTick:
    // MODE  = 1 (count down)
    // STRE  = 1 (auto reload)
    // STCLK = 1 (system clock)
    // INIT  = 1 (initialize counter)
    // STE   = 1 (enable)

    STK_CTLR = // building a number to turn on the right bits
        (1 << 4) |  // MODE (down)
        (1 << 3) |  // STRE (reload enable)
        (1 << 2) |  // STCLK (CPU clock)
        (1 << 5) |  // INIT (reset counter)
        (1 << 0);   // STE (enable) this is the moment it starts counting

    // Wait for count flag
    while ((STK_SR & (1 << 0)) == 0) //shifting by 0 is not needed just looks coherent
        ;

    // Disable timer
    STK_CTLR = 0;

    // Clear flag
    STK_SR = 0;
}
void delay_ns(uint64_t ns)
{
    delay(ns);   // from your previous SysTick implementation
}

void delay_us(uint32_t us)
{
    delay_ns((uint64_t)us * 1000ULL);
}

void delay_ms(uint32_t ms)
{
    delay_us(ms * 1000ULL);
}


///////////////////////////////////////////////////////////////////////////////
// Assignment 1: Configure GPIO Port E so that all pins [0:15] are set as 
//               output, 2MHz, push-pull.
///////////////////////////////////////////////////////////////////////////////
void init_gpio_port_e()
{
    GPIOE_CRL = 0x22222222;  // Pins 0–7
    GPIOE_CRH = 0x22222222;  // Pins 8–15
}



///////////////////////////////////////////////////////////////////////////////
// Assignment 2: Implement the write cycle. This will be used to write both 
//               commands and data to the display. 
//    
//               Look at the "Writing a command or data to the display" 
//               timing diagram in the quickguide, to see how this works.
// 
// Input: command_or_data - the byte to write to the display. This can be 
//                          either a command or data, depending on the state 
//                          of the control lines.
///////////////////////////////////////////////////////////////////////////////
#define GPIOE_ODR   (*(volatile uint32_t *)(GPIOE_BASE + 0x0C))
#define GPIOE_IDR   (*(volatile uint32_t *)(GPIOE_BASE + 0x08))

#define RS  (1 << 0)
#define RW  (1 << 1)
#define E   (1 << 2)

#define DATA_MASK  0x0000FF00   // PE8–PE15
void ascii_write_controller(uint8_t command_or_data)
{
    // The control lines (RS and RW) are set before calling this function.
    // According to the quickguide, you need to wait 40ns after setting the
    // control lines before initiating a write to the display. 
    //
    // 40ns is the time it takes to execute 8 instructions, so that time 
    // will already have passed by the time the processor has entered this
    // function. 

    ///////////////////////////////////////////////////////////////////////////
    // Assignment 2.1: Set E = 1 to start the write cycle
    //                 Then write the command or data to GPIOE_OUTDR[8..15].
    //                 Hint: Make sure your data write does not overwrite 
    //                 the control bits in GPIOE_OUTDR[0..7]!
    ///////////////////////////////////////////////////////////////////////////
    // Your code here

    // Set E = 1
    GPIOE_ODR |= E;

    // Clear old data (PE8–PE15)
    GPIOE_ODR &= ~DATA_MASK;

    // Write new data to PE8–PE15
    GPIOE_ODR |= ((uint32_t)command_or_data << 8);

    check_assignment_2_1(command_or_data);

   

    ///////////////////////////////////////////////////////////////////////////
    // Assignment 2.2: Wait for at least  max(tsu2, tw) = 230ns, then set 
    //                 E = 0 to end the write cycle.
    ///////////////////////////////////////////////////////////////////////////
    // Your code here

    // Wait ≥ 230 ns
    delay_ns(250);

    // Set E = 0
    GPIOE_ODR &= ~E;

    check_assignment_2_2();

    // 10ns already satisfied automatically


    // Finally, wait for th = 10ns before returning from the function. This 
    //is the time it takes for the display to process the command or data after 
    // the write cycle has ended.
    //
    // 10ns is the time it takes to execute 2 instructions, so that time will
    // already have passed by the this function returns. 
}

///////////////////////////////////////////////////////////////////////////////
// Assignment 3: Implement code for writing a command to the display.
///////////////////////////////////////////////////////////////////////////////
void ascii_write_command(uint8_t command){
    GPIOE_ODR &= ~RS;  // RS = 0
    GPIOE_ODR &= ~RW;  // RW = 0

    ascii_write_controller(command);
}


///////////////////////////////////////////////////////////////////////////////
// Assignment 4: Implement a function that reads the status byte from the 
//               display, to see if it is busy or ready for the next command.
//
//               Look at the "Reading status or data from the display" timing 
//               diagram in the quickguide, to see how this works.
//
// Output: 1 if busy, 0 if ready for new command.
///////////////////////////////////////////////////////////////////////////////
uint8_t ascii_read_status(){
    ///////////////////////////////////////////////////////////////////////////
    // Assignment 4.1: Temporarily configure GPIO Port E pins [8:15] as 
    //                 input, floating.
    ///////////////////////////////////////////////////////////////////////////
    // Your code here
    GPIOE_CRH = 0x44444444;

    check_assignment_4_1(); 
    ///////////////////////////////////////////////////////////////////////////
    // Assignment 4.2: Set RS = 0 (command) and RW = 1 (read) to indicate 
    //                 a status read.
    ///////////////////////////////////////////////////////////////////////////
    // Your code here
    GPIOE_ODR &= ~RS;
    GPIOE_ODR |= RW;
    check_assignment_4_2(); 
    ///////////////////////////////////////////////////////////////////////////
    // Assignment 4.3: Set E = 1 to start the read cycle,
    //                 wait for tD = 360ns
    //                 then read the status byte from GPIO Port E INDR register
    //                 pins [8:15]
    ///////////////////////////////////////////////////////////////////////////
    // Your code here
    GPIOE_ODR |= E;
    delay_ns(400);   // ≥360ns

    uint8_t status = (GPIOE_IDR >> 8) & 0xFF;

    check_assignment_4_3(); 
    ///////////////////////////////////////////////////////////////////////////
    // Assignment 4.4: Set E = 0 to end the read cycle.
    ///////////////////////////////////////////////////////////////////////////
    // Your code here
    GPIOE_ODR &= ~E;

    check_assignment_4_4(); 

    ///////////////////////////////////////////////////////////////////////////
    // Assignment 4.5: Configure GPIO Port E pins [8:15] back to output,
    //                 2MHz, push-pull.
    ///////////////////////////////////////////////////////////////////////////
    // Your code here
    GPIOE_CRH = 0x22222222;
    check_assignment_4_5(); 

    ///////////////////////////////////////////////////////////////////////////
    // Assignment 4.6: Return the busy flag (bit 7 of the status byte)
    ///////////////////////////////////////////////////////////////////////////
    // Replace this
    return (status >> 7) & 1;

    return 0;
}


///////////////////////////////////////////////////////////////////////////////
// Assignment 5: Implement code for writing a chareacter (data) to the display.
///////////////////////////////////////////////////////////////////////////////
void ascii_write_data(uint8_t data){
    // TODO: Set RS = 1 (data) and RW = 0 (write) to indicate a command write, 
    //       then call ascii_write_controller to write the character to the 
    //       display.
    GPIOE_ODR |= RS;   // RS = 1
    GPIOE_ODR &= ~RW;  // RW = 0

    ascii_write_controller(data);
}


///////////////////////////////////////////////////////////////////////////////
// Assignment 6: Remove the training wheels!
// ============================================================================
// If you have passed all the previous assignments, and can see the password
// on the display when you run the program, then it is time to write your own
// main function that uses your functions to write to the display. 
// 
// Assignment 6.1: In the main.c file, rename "int main(void)" to 
// "int main_old(void)". 
// 
// Then uncomment the main function below and implement the missing code.
// You will need to consult the list of commands in the quickguide for this. 
///////////////////////////////////////////////////////////////////////////////

/*  
int main()
{
    // Initialize GPIO Port E (just call your function from Assignment 1

    // Read status until display is not busy.
    // Write a "Display Control" command that turns the display off

    // Read status until display is not busy.
    // Write a "Function Set" command that sets the function to 2 lines, 5x8 dots.

    // Read status until display is not busy.
    // Write a "Display Control" command that turns the dusplay on, cursor on, blink

    // Read status until display is not busy.
    // Write a "Entry Mode Set" command that says cursor should move right and 
    // display should not shift.

    // Read status until display is not busy.
    // Write a "Clear Display" command to clear the display.

    // Write the name of your group to the display

}
*/