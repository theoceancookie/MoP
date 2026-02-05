###############################################################################
# Lab 1, Assignment 2
###############################################################################

.global main
.text 

main: 
    ###########################################################################
    # Nothing to see here. You are on your own (but you have the TA)
    ###########################################################################
    
    loop: 



    la t0, 0x40011800 #GPIOD
    li t1, 0x02000000
    sw t1, 0(t0) #CFGLR

    # Set CFGLR for pin 6
    # Pin 6 needs bits [27:24] = 0011 (push-pull output, 50MHz)
    li t1, 0x03000000      # 0011 in bits [27:24]
    sw t1, 0(t0)  
    # Turn on the LED by setting bit 6 in ODATA
    li t1, 0x00000040      # Bit 6 = 1 (0x40 = 0b01000000)
    sw t1, 12(t0)          # Write to ODATA (offset 0x0C)

    li t3, 0
    li t4, 4000000

    delay:
    addi t3, t3, 1

    blt t3, t4, delay


    
    j loop


