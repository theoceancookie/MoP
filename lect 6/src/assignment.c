#include <stdint.h>
#include <stdio.h>

///////////////////////////////////////////////////////////////////////////////
// Lecture 06 - Exercise 02
// ============================================================================
// In this exercise, you will learn to use the SysTick timer to implement a 
// delay function. 
//
// Like previous exercises, your code will be tested from an external main.c
// file. Since your code will run on a simulator, meassuring the ACTUAL time
// is not possible (real-time for the simulator depends on the host computer).
//
// Instead, the test code will use another timer (TIMER6) to measure how many
// clock-cycles your delay function takes, and calculate the real time it would
// have taken on hardware. This is not super exact, so don't worry if your 
// results are a bit off.
//
///////////////////////////////////////////////////////////////////////////////

// Ignore this, it is just a declaration of a function you can use to test your
// code later. 
void check_assignment_3();

///////////////////////////////////////////////////////////////////////////////
// Assignment 1: 
// Open the quickguide and create definitions for the SysTick registers.
// Remember that STK_CNT and STK_CMP are 64-bit registers so you will either
// need to define them as uint64_t pointers, or as two uint32_t pointers.
///////////////////////////////////////////////////////////////////////////////

#define STK_BASE   0xE000F000

#define STK_CTLR   (*(volatile uint32_t *)(STK_BASE + 0x0))
#define STK_SR     (*(volatile uint32_t *)(STK_BASE + 0x4))

#define STK_CNT    (*(volatile uint64_t *)(STK_BASE + 0x8))
#define STK_CMP    (*(volatile uint64_t *)(STK_BASE + 0x10))

///////////////////////////////////////////////////////////////////////////////
// Assignment 2: 
// The system runs at 144Mhz. Implement a function that calulates the number 
// of ticks needed for the requested delay in nanoseconds.
///////////////////////////////////////////////////////////////////////////////
uint64_t ns_to_ticks(uint64_t ns)
{
	// Your code here
	return (ns * 144000000ULL) / 1000000000ULL;; 
}


///////////////////////////////////////////////////////////////////////////////
// Assignment 3: 
// Implement a function that uses the SysTick timer to create a delay
// of the requested length in nanoseconds.
///////////////////////////////////////////////////////////////////////////////
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

///////////////////////////////////////////////////////////////////////////////
// Assignment 4:
// Write a delay_us(), and a delay_ms() function that uses your delay(ns) function.
///////////////////////////////////////////////////////////////////////////////
void delay_us(uint32_t us)
{
    delay((uint64_t)us * 1000ULL);
}

void delay_ms(uint32_t ms)
{
    delay((uint64_t)ms * 1000000ULL);
}