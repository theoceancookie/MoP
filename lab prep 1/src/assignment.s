###############################################################################
# Lab 1 - Preparation Task
#
#         Like many of the previous exercises, your task here is to write a 
#         function and the function will be tested by external C code. 
#         The test harness will output results on the USART, so, if you are
#         running on a simulator, you need to attach a Console to USART1
#
#         Start with Assignment 1 and work your way through the rest of the 
#         assignments in order.
###############################################################################

# Make the assembly functions visible to main.c (C test harness code)
.global copyvec, hidden, copyelements 

hidden:
    ###########################################################################
    # Assignment 1: 
    # This short assignment will teach you to use the debugger and memory view.
    # The function is called from the C startup file, so place a breakpoint 
    # on the return instruction of this function (`ret`). 
    #
    # The only parameter to this function is the address of an array we have 
    # hidden somewhere in memory. Before calling this function, the main
    # function will have placed that address in register a0 (first parameter
    # register).
    #
    # 1.1:  In the debug view to the left, open "VARIABLES" panel.
    #       When you run the debugger, you will see the list of "Registers"
    #       with their values there. Use the "register" list to find out what
    #       address is passed as the parameter and write it down. You will need
    #       it to pass the Quiz on Canvas.
    #
    # 1.2:  Open the "MEMORY" view below and press the `+` button to create 
    #       a new view. Type in the address you just found, to see what is 
    #       in memory at that address. 
    #     
    #       On the right side of the memory, each byte of memory is decoded as 
    #       an ascii character. What is the message we have hidden? 
    #
    # Once you have written down the address and the message, and feel like
    # you understand what the memory viewer does, move on to the next
    # assignment. 
    ###########################################################################
    ret


###############################################################################
# void copyvec(int src[], short dst[], int size);
# ================================================
# The function should copy `size` elements from the array `src` to the array `dst`
# You may assume that both arrays contain at least `size` elements.
# 
# Input: 
#   a0: The address to the first element of an array called "src" 
#       containing elements of type signed int
#   a1: The address to the first element of an array called "dst"
#       containing elements of type signed short (2 bytes)
#   a2: The number of elements to copy
###############################################################################

copyvec: 
    ###########################################################################
    # Assignment 2: 
    # Write a loop that copies `size` elements from `src` to `dst`
    # 
    # If you are having trouble, use the debugger and memory view (as in the
    # first assignment) to see what is getting copied. 
    ###########################################################################
    
    li t0, 4
    mul t2, a2, t0 

    li t0, 0
    
    loop:
        
        add t1, a0, t0
        lw t1, 0(t1)

        srli t3, t0, 1
        add t3, t3, a1
        sh t1, 0(t3)

        addi t0, t0, 4
        blt t0, t2, loop


    ret

###############################################################################
# int copyelements(int src[], short dst[], int start, int end);
# ===============================================================
# This function shall copy PART of the `src` array to the `dst` array.
# You MUST use the `copyvec` function for this.
# 
# Input: 
#   a0: The address to the first element of an array called "src" 
#       containing elements of type signed int
#   a1: The address to the first element of an array called "dst"
#       containing elements of type signed short (2 bytes)
#   a2: The first element to copy
#   a3: The last element to copy
#
# Output: 
#   a0: The function shall return the number of elements copied
###############################################################################

copyelements: 
    ###########################################################################
    # Assignment 4 (not 3, do 3 first): 
    #
    # Remember to push caller-saved registers with important data to the stack.
    # When we call `copyvec`, these registers may be overwritten.
    ###########################################################################

    ###########################################################################
    # Assignment 3: 
    # To calculate which elements to copy, implement this pseudo code 
    # 
    # src_start = <address to src[start]>
    # dst_start = <address to dst[start]>
    # size = <number of elements to copy>
    # copyvec(src_start, dst_start, size)
    #
    # Then place a breakpoint at the `ret` instruction run the program, 
    # and verify with the memory view that the copying works.
    ###########################################################################

    addi sp, sp, -4
    sw ra, 0(sp)

    sub t0, a3, a2
    addi t0, t0, 1

    addi sp, sp, -4
    sw t0, 0(sp)

    slli t1, a2, 2
    add a0, a0, t1

    slli t1, a2, 1
    add a1, a1, t1

    mv a2, t0

    jal ra, copyvec



    lw a0, 0(sp)
    addi sp, sp, 4

    lw ra, 0(sp)
    addi sp, sp, 4


    ###########################################################################
    # Assignment 5:
    #
    # Remember to restore any values you pushed to the stack.
    ###########################################################################
    ret

###############################################################################
# Assignment 6:
# Remove your breakpoints and let the program run all the tests. (F5 to run.)
# If all tests passed, the secret password will be printed in the console.
# Submit the password along with the hidden message and its address in Canvas.
###############################################################################