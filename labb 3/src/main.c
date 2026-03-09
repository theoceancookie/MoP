#include "afio.h"
#include "exti.h"
#include "gpio.h"
#include "interrupts.h"
#include "keypad.h"
#include "music.h"
#include "pfic.h"
#include "systick.h"
#include "vector_table.h"
#include <stdint.h>
#include <stdio.h>

/* SUMMARY OF TASKS
 *
 * Task 1: Sound Check
 * - in systick_handler(), invert the buzzer pin and acknowledge interrupt
 * - in main(), add systick_periodic_micro(500) to test the buzzer
 * - remove test code before moving on
 *
 * Task 2: External Control
 * - Task 2.1: Reading the Keypad
 * --- in exti_handler(), read keypad and acknowledge interrupt
 * --- use debugger to verify being able to read key press AND release
 * - Task 2.2: Making Noise
 * --- add array with period values, in scope from exti_handler()
 * --- in exti_handler(), start/stop sound on key press/release
 *
 * Task 3: Lights, Please!
 * - in exti_handler() where sound is started, also turn on lights
 * - in exti_handler() where sound is stopped, also turn off lights
 *
 * Task 4: Melody Player
 * - add given global variables to main.c, above systick_handler()
 * - in main(), iterate through list of notes, starting each note and
 *   wait while remaining_duration > 0
 *   (WARNING: do NOT use while(remaining_duration != 0) to wait!)
 * - in systick_handler(), reduce remaining_duration by current_period
 */

// Globals for Task 4
Note notes[] = NOTES;
const int notes_length = sizeof(notes) / sizeof(Note);
int remaining_duration;
int current_period;

// Handle internal SysTick interrupt
__attribute__((interrupt("machine")))
void systick_handler() {
  // TODO: Task 1
  GPIOE->ODR ^= (1 << 0);   // Invert buzzer pin PE0
  //ODR is the Output Data Register — each bit controls one output pin
  //(1 << 0) take 1 and shift it to the left 0 times. creates a bitmask for pin 0: 00000001
  //^= is XOR-assignment, which flips that first bit every time it runs
  //So each time SysTick fires, PE0 toggles between 0 and 1 → square wave → buzzer makes sound
  
  STK->SR &= ~(1 << 0);     // Acknowledge: clear CNTIF flag
  //SR is the Status Register, and bit 0 is CNTIF — the "count interrupt flag"
  //The hardware sets this flag to 1 when the interrupt fires
  //If you don't clear it, the system thinks the interrupt is still pending
  //~(1 << 0) flips the mask to 11111110. ~ is what makes it all flip/ invert
  //&= and operation. with that mask forces bit 0 to 0, clearing the flag
    
  // TODO: Task 4
}

// Handle external keypad interrupt (on press *and* on release)

uint32_t periods[16] = {
  1516, 1431, 1351, 1275,
  1203, 1136, 1072, 1012,
   955,  901,  851,  803,
   758,  715,  675,  637
};

__attribute__((interrupt("machine")))
void exti_handler() {
  // TODO: Task 2
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
  // Acknowledge the EXTI interrupt
  EXTI->INTFR = EXTI->INTFR;


  // TODO: Task 3
}

// Entry point
int main(void) {
  // Initialize
  init_gpio();
  init_vector_table();
  init_interrupts();
  
  // TODO: Task 1

  // TODO: Task 4

  // Wait for interrupts
  while (1) {
    // Could put concurrently running code here!
  }
}
