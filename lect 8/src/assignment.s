.section .text

.global InitInterrupts
.extern SysTick_Handler

InitInterrupts: 
###############################################################################
# Assignment 2.1: This function is called from the whackamole game loop. 
#                 Here, you need to: 
#                 1. Initialize the CSR register "mtvec" to point at the 
#                    SysTick_Handler, in the C code. 
#                 2. Enable interrupts from SysTick, by setting the 
#                    corresponding bit in the PFIC_IENR1 register (see Vector 
#                    Table and PFIC in the quickguide).
# Hint: Read lecture 08 to remember that CSR registers are accessed using csrw and
#       csrr instructions (which are in the QuickGuide).
###############################################################################
    la    t0, SysTick_Handler       # t0 = address of SysTick_Handler
    csrw  mtvec, t0                 # mtvec = t0  (direct mode, all exceptions
                                    # jump straight to this address)

    li    t0, PFIC_IENR1            # t0 = 0xE000E100 (register address)
    li    t1, (1 << SYSTICK_IRQ)    # t1 = 0x00001000 (bit 12)
    sw    t1, 0(t0)                 # PFIC_IENR1 |= (1 << 12)

    ret
