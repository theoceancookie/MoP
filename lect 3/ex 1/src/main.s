###############################################################################
# Lecture 03 - Example 01
# ============================================================================
# The C standard library (which this program links against) provides the 
# functions: 
# 
# int putchar(int c) - Prints the ASCII character corresponding to c.
#                      Returns -1 on error, otherwise returns the character 
#                      printed.
# 
# int rand(void) - Returns a pseudo-random integer in the range 0 to 32767
#
# ============================================================================
# Write a program that continually renders random, lower case, letters to 
# the console. 
# 
# You need to connect a console to USART1 in the simulator.
###############################################################################
.section .text
.global main

# These lines tell the assembler that these functions are defined elsewhere
.extern putchar
.extern rand

write_random_char:
    // Your code here
    ret

main:
    # Infinite loop
.loop:
    call    write_random_char
    j    .loop   
    ret


