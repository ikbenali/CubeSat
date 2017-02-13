/*!
 *  \file    stream.h
 *  \author  Wim Dolman (<a href="email:w.e.dolman@hva.nl">w.e.dolman@hva.nl</a>)
 *  \date    03-10-2016
 *  \version 1.4
 *
 *  \brief   Stream for the communcation mode of HvA-Xmegaboard
 *
 *  \details Makes a standard stream for the HvA-Xmegaboard. It is build on the wrapper.
 *           With these you can use <code>printf</code> and <code>scanf</code> to write
 *           to and read from the UART F0. The baud rate is 115200 and the protocol is 8N1.
 *
 *           Version 1.3 :  Modified uart_fputc changed. With a full TX-buffer it doesn't
 *           skips characters anymore. It now waits until there is free space in the buffer.
 *
 *           Version 1.4 :  The baud rate is changed toe 115200 
 */

#include <stdio.h>
#define BAUD             115200    //!< baud rate
#define CLK2X            0         //!< no double clock speed

/*!
 * \brief Macro to reset and clear the terminal
 */
#define  clear_screen()     printf("\e[H\e[2J\e[3J");

void init_stream(uint32_t fcpu);

