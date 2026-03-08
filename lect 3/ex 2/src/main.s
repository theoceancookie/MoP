###############################################################################
# Lecture 03 - Example 02
# ============================================================================
# The C standard library (which this program links against) provides the 
# functions: 
# 
# int putchar(int c) - Prints the ASCII character corresponding to c.
#                      Returns -1 on error, otherwise returns the character 
#                      printed.
# 
# Use this function to write another function: 
# void print_string(address, length)
# 
# You need to connect a console to USART1 in the simulator.
###############################################################################
.section .text
.global main

# These lines tell the assembler that these functions are defined elsewhere
.extern putchar

print_string: 
    // Your code here
    ret

main:

loop2: 

    la a0, string1      # Load address of string1
    li a1, 4            # Length of string1
    call print_string

    la a0, string2      # Load address of string2
    li a1, 3            # Length of string2
    call print_string

    la a0, string3      # Load address of string3
    li a1, 5            # Length of string3
    call print_string

    j loop2

string1: .ascii "MOP\n"
string2: .ascii "is\n"
string3: .ascii "fun!\n"

