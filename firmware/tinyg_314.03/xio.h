/*
 * xio.h - Xmega IO devices - common header file
 * Part of TinyG project
 *
 * Copyright (c) 2010 - 2011 Alden S. Hart Jr.
 *
 * TinyG is free software: you can redistribute it and/or modify it 
 * under the terms of the GNU General Public License as published by 
 * the Free Software Foundation, either version 3 of the License, 
 * or (at your option) any later version.
 *
 * TinyG is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
 * See the GNU General Public License for details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with TinyG  If not, see <http://www.gnu.org/licenses/>.
 */
/* XIO devices are compatible with avr-gcc stdio, so formatted printing 
 * is supported. To use this sub-system outside of TinyG you may need 
 * some defines in tinyg.h. See notes at end of this file for more details.
 */

#ifndef xio_h
#define xio_h

/*  Note: anything that includes xio.h first needs the following:
 	#include <stdio.h>				// needed for FILE def'n
	#include <avr/pgmspace.h>		// defines prog_char, PSTR
*/

// put all sub-includes here so only xio.h is needed elsewhere
#include "xio_file.h"
#include "xio_usart.h"
#include "xio_signals.h"

/******************************************************************************
 *	Globals
 ******************************************************************************/

/* Note: stdin, stdout and stderr are defined in stdio.h and are used by XIO */

/******************************************************************************
 *	Definitions
 ******************************************************************************/

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

#define PGMFILE (const PROGMEM char *)	// extends pgmspace.h

/* XIO return codes
 * These codes are the "inner nest" for the TG_ return codes. 
 * The first N TG codes correspond directly to these codes.
 * This eases using XIO by itself (without tinyg) and simplifes using
 * tinyg codes with no mapping when used together. This comes at the cost 
 * of making sure these lists are aligned. TG_should be based on this list.
 */

enum xioCodes {
	XIO_OK = 0,				// OK - ALWAYS ZERO
	XIO_ERR,				// generic error return (errors start here)
	XIO_EAGAIN,				// function would block here (must be called again)
	XIO_NOOP,				// function had no-operation	
	XIO_COMPLETE,			// operation complete
	XIO_EOL,				// function returned end-of-line
	XIO_EOF,				// function returned end-of-file 
	XIO_FILE_NOT_OPEN,		// file is not open
	XIO_FILE_SIZE_EXCEEDED, // maximum file size exceeded
	XIO_NO_SUCH_DEVICE,		// illegal or unavailable device
	XIO_BUFFER_EMPTY,		// more of a statement of fact than an error code
	XIO_BUFFER_FULL_FATAL,
	XIO_BUFFER_FULL_NON_FATAL
};
#define XIO_ERRNO_MAX XIO_BUFFER_FULL_NON_FATAL

/* Typedefs  */							// pointers to functions:
typedef void (*fptr_void_void) (void); 	// returns void, void args
typedef int (*fptr_int_void) (void); 	// returns int, void args

/*************************************************************************
 *	Function Prototypes
 *************************************************************************/

void xio_init(void);					// xio system general init
void xio_init_file(const uint8_t dev, const uint8_t offset, const uint32_t control);
void xio_init_stdio(void);				// set std devs & do startup prompt
void xio_init_rs485(void);				// device-specific inits
void xio_init_usb(void);
void xio_init_pgm(void);
void xio_init_eep(void);
//void xio_init_tbl(void);
//void xio_init_ram(void);

int xio_cntl(const uint8_t dev, const uint32_t control);
void xio_set_stdin(const uint8_t dev);
void xio_set_stdout(const uint8_t dev);
void xio_set_stderr(const uint8_t dev);

int xio_getc(const uint8_t dev);
int xio_putc(const uint8_t dev, const char c);
int xio_gets(const uint8_t dev, char *buf, const int size);

void xio_init_dev(uint8_t dev,					// device number
	FILE *(*dev_open)(const prog_char *addr),	// device open routine
	int (*dev_cntl)(const uint32_t control),// set device control flags
	int (*dev_putc)(char, struct __file *),		// write char (stdio compatible)
	int (*dev_getc)(struct __file *),			// read char (stdio compatible)
	int (*dev_gets)(char *buf, int size)	// specialized line reader
	); 

void xio_tests(void);

/*************************************************************************
 *	Device structures
 *************************************************************************/

struct xioDEVICE {				// common device struct (one per dev)
	uint8_t status;				// completion status 
	uint8_t signal;				// signal value
	char c;						// char temp
	uint8_t len;				// chars read so far (buf array index)
	int size;					// text buffer length (dynamic)
	uint32_t flags;				// common control flags
	FILE *(*x_open)(const prog_char *addr);// device open routine
	int (*x_cntl)(const uint32_t control);// device control flags
	int (*x_putc)(char, struct __file *);// write char (stdio compatible)
	int (*x_getc)(struct __file *);	// read char (stdio compatible)
	int (*x_gets)(char *buf, const int size);// specialized line reader

	void *x;					// device-specific struct binding (static)
	FILE *fdev;					// stdio fdev binding (static)
	char *buf;					// text buffer binding (dynamic)
};

/*************************************************************************
 *	Device configurations
 *************************************************************************/

/* Known XIO devices (configured devices) */
// unused devices are commented out

enum xioDevice {				// device enumerations
								// TYPE:	DEVICE:
	XIO_DEV_RS485,				// USART	RS485 device (network port)
	XIO_DEV_USB,				// USART	USB device
	XIO_DEV_TTL,				// USART	TTL device (typically Arduino)
	XIO_DEV_PGM,				// FILE		Program memory file  (read only)
	XIO_DEV_EEP,				// FILE		EEPROM 				 (read/write)
//	XIO_DEV_TBL,				// FILE		Prog mem table space (read/write)
//	XIO_DEV_RAM,				// FILE		RAM 				 (read/write)
//	XIO_DEV_SDC,				// FILE		SD card (not implemented)
//	XIO_DEV_ENC,				// HW		Encoder port
//	XIO_DEV_LIM,				// HW		Limit switch port
	XIO_DEV_COUNT				// total device count (must be last entry)
};

// If your change these ^, check these v

#define XIO_DEV_RS485_OFFSET 	XIO_DEV_RS485	// index into USARTS 
#define XIO_DEV_USB_OFFSET 		XIO_DEV_USB	
#define XIO_DEV_TTL_OFFSET 		XIO_DEV_TTL
#define XIO_DEV_USART_COUNT 	(3) 			// # of USART devices

#define XIO_DEV_PGM_OFFSET 		(XIO_DEV_PGM - XIO_DEV_PGM)// index into FILES
#define XIO_DEV_EEP_OFFSET 		(XIO_DEV_EEP - XIO_DEV_PGM)
//#define XIO_DEV_TBL_OFFSET	(XIO_DEV_TBL - XIO_DEV_PGM)
//#define XIO_DEV_RAM_OFFSET	(XIO_DEV_RAM - XIO_DEV_PGM)
//#define XIO_DEV_SDC_OFFSET	(XIO_DEV_SDC - XIO_DEV_PGM)
#define XIO_DEV_FILE_COUNT 		(2)				// # of FILE devices

// aliases for stdio devices (aka pointers, streams)
#define fdev_rs485	(ds[XIO_DEV_RS485].fdev)// RS485 device for stdio functions
#define fdev_usb	(ds[XIO_DEV_USB].fdev)	// USB device for stdio functions
#define fdev_pgm	(ds[XIO_DEV_PGM].fdev)	// Program memory device
#define fdev_eep	(ds[XIO_DEV_EEP].fdev)	// EEPROM memory device
//#define fdev_tbl	(ds[XIO_DEV_TBL].fdev)	// TABLE space device
//#define fdev_ram	(ds[XIO_DEV_RAM].fdev)	// RAM memory device

// USART devices: See xio_usart.h for USART-based device configs
// FILE devices:  See xio_file/h for FILE-based device configs

/*
 * Struct exports 
 * See xio_usart.h usart device struct definition
 * See xio_file.h file device struct definition
 * See signal.h signal flag struct definition
 */
 
extern struct xioDEVICE ds[XIO_DEV_COUNT];// makes fdev_xxxx descriptors valid
extern struct xioUSART us[XIO_DEV_USART_COUNT];	// USART extended IO structs
extern struct xioFILE fs[XIO_DEV_FILE_COUNT];	// FILE extended IO structs
extern struct xioSIGNALS sig;					// signal flags

/*
 * xio control flag values
 */
// must cast 1 to uint32_t for bit evaluations to work correctly
#define XIO_BAUD_gm		0x0000000F			// baud rate enum mask (keep in LSdigit)
#define XIO_RD			((uint32_t)1<<4) 	// read enable bit
#define XIO_WR			((uint32_t)1<<5)	// write enable only
#define XIO_RDWR		(XIO_RD | XIO_WR) 	// read & write
#define XIO_BLOCK		((uint32_t)1<<6)	// enable blocking reads
#define XIO_NOBLOCK		((uint32_t)1<<7)	// disable blocking reads
#define XIO_XOFF 		((uint32_t)1<<8)	// enable XON/OFF flow control
#define XIO_NOXOFF 		((uint32_t)1<<9)	// disable XON/XOFF flow control
#define XIO_ECHO		((uint32_t)1<<10)	// echo reads from device to stdio
#define XIO_NOECHO		((uint32_t)1<<11)	// disable echo
#define XIO_CRLF		((uint32_t)1<<12)	// convert <LF> to <CR><LF> on writes
#define XIO_NOCRLF		((uint32_t)1<<13)	// do not convert <LF> to <CR><LF> on writes
#define XIO_IGNORECR	((uint32_t)1<<14)	// ignore <CR> on reads
#define XIO_NOIGNORECR	((uint32_t)1<<15)	// don't ignore <CR> on reads
#define XIO_IGNORELF	((uint32_t)1<<16)	// ignore <LF> on reads
#define XIO_NOIGNORELF	((uint32_t)1<<17)	// don't ignore <LF> on reads
#define XIO_LINEMODE	((uint32_t)1<<18)	// special <CR><LF> read handling
#define XIO_NOLINEMODE	((uint32_t)1<<19)	// no special <CR><LF> read handling

// internal control flags (which are NOT the similar bits in the control word, above)
// static configuration states
#define XIO_FLAG_RD_bm		((uint32_t)1<<0) // enabled for read
#define XIO_FLAG_WR_bm		((uint32_t)1<<1) // enabled for write
#define XIO_FLAG_BLOCK_bm	((uint32_t)1<<2) // enable blocking RD and WR
#define XIO_FLAG_XOFF_bm 	((uint32_t)1<<3) // XOFF flow control enabled
#define XIO_FLAG_ECHO_bm 	((uint32_t)1<<4) // echo received chars to stderr
#define XIO_FLAG_CRLF_bm 	((uint32_t)1<<5) // convert <LF> to <CR><LF> on writes
#define XIO_FLAG_IGNORECR_bm ((uint32_t)1<<6)// ignore <LF> on reads
#define XIO_FLAG_IGNORELF_bm ((uint32_t)1<<7)// ignore <LF> on reads
#define XIO_FLAG_LINEMODE_bm ((uint32_t)1<<8)// special handling for line-oriented text
// transient states
#define XIO_FLAG_TX_MUTEX_bm ((uint32_t)1<<9)// TX dequeue mutual exclusion flag
#define XIO_FLAG_EOL_bm		((uint32_t)1<<10) // detected EOL (/n, /r, ;)
#define XIO_FLAG_EOF_bm 	((uint32_t)1<<11)// detected EOF (NUL)
#define XIO_FLAG_IN_LINE_bm	((uint32_t)1<<12)// partial line is in buffer
#define XIO_FLAG_DO_XOFF_bm ((uint32_t)1<<13)// flag to transmit an XOFF
#define XIO_FLAG_DO_XON_bm	((uint32_t)1<<14)// flag to transmit an XON
#define XIO_FLAG_IN_XOFF_bm ((uint32_t)1<<15)// device is in XOFF (flow control)

#define XIO_FLAG_RESET_gm	(0x0FFF)	// used to clear the top bits

// Bit evaluations that return actual TRUE and FALSE
// Just using the (a & blahblah) returns FALSE and not_FALSE 
// ...but not actually TRUE (which = 1)
#define READ(a) 		((a & XIO_FLAG_RD_bm) ? TRUE : FALSE)
#define WRITE(a)	 	((a & XIO_FLAG_WR_bm) ? TRUE : FALSE)
#define BLOCKING(a) 	((a & XIO_FLAG_BLOCK_bm) ? TRUE : FALSE)
#define EN_XOFF(a)		((a & XIO_FLAG_XOFF_bm) ? TRUE : FALSE)
#define ECHO(a)		 	((a & XIO_FLAG_ECHO_bm) ? TRUE : FALSE)
#define CRLF(a) 		((a & XIO_FLAG_CRLF_bm) ? TRUE : FALSE)
#define IGNORECR(a) 	((a & XIO_FLAG_IGNORECR_bm) ? TRUE : FALSE)
#define IGNORELF(a) 	((a & XIO_FLAG_IGNORELF_bm) ? TRUE : FALSE)
#define LINEMODE(a)		((a & XIO_FLAG_LINEMODE_bm) ? TRUE : FALSE)
#define TX_MUTEX(a)		((a & XIO_FLAG_TX_MUTEX_bm) ? TRUE : FALSE)
#define IN_LINE(a)		((a & XIO_FLAG_IN_LINE_bm) ? TRUE : FALSE)
#define DO_XOFF(a) 		((a & XIO_FLAG_DO_XOFF_bm) ? TRUE : FALSE)
#define DO_XON(a) 		((a & XIO_FLAG_DO_XON_bm) ? TRUE : FALSE)
#define IN_XOFF(a) 		((a & XIO_FLAG_IN_XOFF_bm) ? TRUE : FALSE)

/*
 * Generic XIO signals and error conditions. 
 * See signals.h for application specific signal defs and routines.
 */

enum xioSignals {
	XIO_SIG_OK,				// OK
	XIO_SIG_EAGAIN,			// would block
	XIO_SIG_EOL,			// end-of-line encountered (string has data)
	XIO_SIG_EOF,			// end-of-file encountered (string has no data)
	XIO_SIG_KILL,			// cancel operation immediately (^c, ETX, 0x04)
	XIO_SIG_TERM,			// cancel operation nicely (^x, CAN, 0x24)
	XIO_SIG_PAUSE,			// pause operation (^s, XOFF, DC3, 0x13)
	XIO_SIG_RESUME,			// resume operation (^q, XON, DC1, 0x11)
	XIO_SIG_ESCAPE,			// ESC. Typically mapped to ^c or ^x functions
	XIO_SIG_DELETE,			// backspace or delete character (BS, DEL)
	XIO_SIG_BELL			// BELL character (BEL, ^g)
};

/* Some useful ASCII definitions */

#define NUL (char)0x00		// ASCII NUL character (0) (not "NULL" which is a pointer)
#define ETX (char)0x03		// ^c - aka ETX, KILL, END
#define BEL (char)0x07		// ^g - aka BEL
#define BS  (char)0x08		// ^h - aka backspace 
#define LF	(char)0x0A		//  line feed
#define CR	(char)0x0D		//  carriage return
#define SHIFTOUT (char)0x0E	// ^n - aka shift out 
#define SHIFTIN (char)0x0F	// ^o - aka shift in
#define XON (char)0x11		// ^q - aka DC1, XON, resume
#define XOFF (char)0x13		// ^s - aka DC3, XOFF, pause
#define ESC (char)0x1B		// ESC(ape)
#define DEL (char)0x7F		// DEL(ete)

#define KILL ETX
#define CTRL_C ETX
#define CTRL_G BEL
#define CTRL_H BS	
#define CTRL_N SHIFTOUT	
#define CTRL_O SHIFTIN	
#define CTRL_Q XON
#define CTRL_S XOFF
#define CTRL_X (char)0x18				// ^x - aka CAN(cel), TERM(inate)

/* Signal character mappings */

#define SIG_KILL_CHAR (char)'!'
#define SIG_TERM_CHAR (char)'!'
#define SIG_PAUSE_CHAR (char)'@'		// pause and resume
#define SIG_RESUME_CHAR (char)'*'


/**** NOTES ON XIO ****/

/*---- Notes on the circular buffers ----

  An attempt has beeen made to make the circular buffers used by low-level 
  putc/getc as efficient as possible. This enables high-speed serial IO 
  operating between 100K and 1Mbaud.

  The circular buffers are unsigned char arrays that fill down from the 
  top element and wrap back to the top when index zero is reached. This 
  allows pre-decrement operations, zero tests, and eliminates modulus, 
  masks, substractions and other less efficient array bounds checking. 
  Buffer indexes are all uint_fast8_t which limits these buffers to 254 
  usable locations. (one location is lost to head/tail collision detection 
  and one is lost to the zero position) All this enables the compiler to 
  do better optimization.

  Chars are written to the *head* and read from the *tail*. 

  The head is left "pointing to" the character that was previously 
  written - meaning that on write the head is pre-decremented (and 
  wrapped, if necessary), then the new character is written.

  The tail is left "pointing to" the character that was previouly read - 
  meaning that on read the tail is pre-decremented (and wrapped, if 
  necessary), then the new character is read.

  The head is only allowed to equal the tail if there are no characters 
  to read.

  On read: If the head = the tail there is nothing to read, so the 
  function either exits with TG_EAGAIN or blocks (depending on the 
  blocking mode selected).

  On write: If the head pre-decrement causes the head to equal the tail 
  the buffer is full. The head is left at its original value and the 
  device should go into flow control (and the byte in the USART device is 
  not read, and therefore remains in the USART (VERIFY THAT I DIDN'T BREAK 
  THIS BEHAVIOR!)). Reading a character from a buffer that is in flow 
  control should clear flow control.

  (Note: More sophisticated flow control would detect the full condition 
   earlier, say at a high water mark of 95% full, and may go out of flow 
   control at some low water mark like 33% full).
*/
/*---- Notes on control characters and signals ----

  The underlying USART RX ISRs (used by getc() and gets()) trap control 
  characters and treat them as signals. 
  
  On receipt of a signal the signal value (see enum xioSignals) is written 
  to xioDEVICE.sig and a signal handler specific to that signal is invoked 
  (see signals.c). The signal character is not written into the RX buffer.
  
  The signal handler runs at the ISR level, so it might be run, set some 
  flag somewhere, or just return, relying on the application to detect the 
  sig value being set. It's up to the app to reset sig. If a new signal 
  arrives before the previous sig is handled or cleared the new sig will 
  overwrite the previous sig value.

  For now, the control chars and their mapping to signals are hard-coded 
  into the ISR for simplicity and speed. A more flexible system of 
  bindings and callbacks could be written at some sacrifice to execution 
  speed. 

  IMPORTANT--> Since signals are trapped at the ISR level it is not 
  necessaryto be actively reading a device for it to receive signals. 
  Any configured IO device will process signals. This allows input lines 
  to come from one source (e.g. a file device), while pause, resume and 
  kill are still active from another device (e.g. the USB port being used 
  as a console).
*/

/*---- Notes on signal callbacks ----
  An earlier version of the code had signals implemented as callbacks. 
  Here's how it worked.

The struct had sig_func and line_func callback addresses:

struct xioUSART {
	// PUBLIC VARIABLES - must be the same in every device type
	uint16_t flags;				 // control flags
	uint8_t sig;				 // signal or error value
	char c;						 // line buffer character temp
	uint8_t i;					 // line buffer pointer
	uint8_t len;				 // line buffer maximum length (zero based)
	char *buf;					 // pointer to input line buffer
	int (*sig_func)(uint8_t sig);// pointer to signal handler function
	int (*line_func)(char * buf);// pointer to line handler function

	// PRIVATE VARIABLES - in this case for USART. Can be different by device type
	volatile uint8_t rx_buf_tail; // RX buffer read index
	volatile uint8_t rx_buf_head; // RX buffer write index (written by ISR)
	volatile uint8_t tx_buf_tail; // TX buffer read index (written by ISR)
	volatile uint=8_t tx_buf_head; // TX buffer write index
	uint8_t next_tx_buf_head;	  // next TX buffer write index
	volatile unsigned char rx_buf[RX_BUFFER_SIZE];  // (written by ISR)
	volatile unsigned char tx_buf[TX_BUFFER_SIZE];

	// hardware bindings
	struct USART_struct *usart;	  // USART structure
	struct PORT_struct *port;	  // corresponding port
};

Bindings occurred during init in xio_usb_init():

	// bind signal and line handlers to struct
	f.sig_func = &xio_null_signal;		// bind null signal handler
	f.line_func = &xio_null_line;		// bind null line handler

...and as controls in xio_usb_control():

	// commands with args - only do one flag if there's an arg
	if (control & XIO_SIG_FUNC) {
		f.sig_func = (fptr_int_uint8)arg;
		return (0);	
	}
	if (control & XIO_LINE_FUNC) {
		f.line_func = (fptr_int_char_p)arg;
		return (0);
	}

  Using these defines:

#define XIO_SIG_FUNC	(1<<4)			// signal handler function 
#define XIO_LINE_FUNC	(1<<5)			// line handler function 

Applications may call the control functions to bind signal handlers:

	xio_control(XIO_DEV_USB, XIO_SIG_FUNC, (int)&tg_signal); // bind sig handler
	xio_control(XIO_DEV_RS485, XIO_SIG_FUNC, (int)&tg_signal);
	xio_control(XIO_DEV_AUX, XIO_SIG_FUNC, (int)&tg_signal);

*/

#endif
