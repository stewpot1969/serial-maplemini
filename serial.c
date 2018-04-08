#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int _write(int fd, char *ptr, int len);
int _read(int fd, char *ptr, int len);
void get_buffered_line(void);

/*
#ifndef __NO_SYSTEM_INIT
void SystemInit()
{}
#endif
*/

#define BUFLEN 127

static uint16_t start_ndx;
static uint16_t end_ndx;
static char buf[BUFLEN+1];
#define buf_len ((end_ndx - start_ndx) % BUFLEN)
static inline int inc_ndx(int n) { return ((n + 1) % BUFLEN); }
static inline int dec_ndx(int n) { return (((n + BUFLEN) - 1) % BUFLEN); }

static void io_setup(void)
{

  /* configure port B */
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_AFIO);

	
	/* PB1 (LED) alt function push/pull - TIM3_CH4 */
	gpio_set_mode(GPIOB,
	  GPIO_MODE_OUTPUT_2_MHZ,
	  GPIO_CNF_OUTPUT_PUSHPULL,
	  GPIO1);
	
	/* PB8 (button) input */
	gpio_set_mode(GPIOB,
	  GPIO_MODE_INPUT,
	  GPIO_CNF_INPUT_FLOAT,
	  GPIO8);
}

static void uart_setup(void)
{
  /* Enable USART3 */
  rcc_periph_clock_enable(RCC_USART3);
  
  /* Setup GPIO pin GPIO_USART3_TX. */
	gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
		      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART3_TX);

  /* Set USART parameters */
  usart_set_baudrate(USART3, 19200);
  //USART_BRR(USART3)=1666;
	usart_set_databits(USART3, 8);
	usart_set_stopbits(USART3, USART_STOPBITS_1);
	usart_set_mode(USART3, USART_MODE_TX_RX);
	usart_set_parity(USART3, USART_PARITY_NONE);
	usart_set_flow_control(USART3, USART_FLOWCONTROL_NONE);

  /* Enable it */
  usart_enable(USART3);
 
}

int main(void)
{  
  //rcc_clock_setup_in_hse_8mhz_out_72mhz();
  char c;

  io_setup();
  uart_setup();
  
  printf("*** Type shit and I'll send back ASCII values. ***\n");
  
  while(1)
  {
    c = usart_recv_blocking(USART3);
    printf ("%c :: %02X :: %03i\n",c,c,c);
  }
}

int _write(int fd, char *ptr, int len)
{
	int i = 0;

	/*
	 * Write "len" of char from "ptr" to file id "fd"
	 * Return number of char written.
	 *
	 * Only work for STDOUT, STDIN, and STDERR
	 */
	if (fd > 2) {
		return -1;
	}
	while (*ptr && (i < len)) {
		usart_send_blocking(USART3, *ptr);
		if (*ptr == '\n') {
			usart_send_blocking(USART3, '\r');
		}
		i++;
		ptr++;
	}
	return i;
}

/*
 * Called by the libc stdio fread fucntions
 *
 * Implements a buffered read with line editing.
 */
int _read(int fd, char *ptr, int len)
{
	int	my_len;

	if (fd > 2) {
		return -1;
	}

	get_buffered_line();
	my_len = 0;
	while ((buf_len > 0) && (len > 0)) {
		*ptr++ = buf[start_ndx];
		start_ndx = inc_ndx(start_ndx);
		my_len++;
		len--;
	}
	return my_len; /* return the length we got */
}

/* back up the cursor one space */
static inline void back_up(void)
{
	end_ndx = dec_ndx(end_ndx);
	usart_send_blocking(USART3, '\010');
	usart_send_blocking(USART3, ' ');
	usart_send_blocking(USART3, '\010');
}

/*
 * A buffered line editing function.
 */
void get_buffered_line(void) {
	char	c;

	if (start_ndx != end_ndx) {
		return;
	}
	while (1) {
		c = usart_recv_blocking(USART3);
		if (c == '\r') {
			buf[end_ndx] = '\n';
			end_ndx = inc_ndx(end_ndx);
			buf[end_ndx] = '\0';
			usart_send_blocking(USART3, '\r');
			usart_send_blocking(USART3, '\n');
			return;
		}
		/* ^H or DEL erase a character */
		if ((c == '\010') || (c == '\177')) {
			if (buf_len == 0) {
				usart_send_blocking(USART3, '\a');
			} else {
				back_up();
			}
		/* ^W erases a word */
		} else if (c == 0x17) {
			while ((buf_len > 0) &&
					(!(isspace((int) buf[end_ndx])))) {
				back_up();
			}
		/* ^U erases the line */
		} else if (c == 0x15) {
			while (buf_len > 0) {
				back_up();
			}
		/* Non-editing character so insert it */
		} else {
			if (buf_len == (BUFLEN - 1)) {
				usart_send_blocking(USART3, '\a');
			} else {
				buf[end_ndx] = c;
				end_ndx = inc_ndx(end_ndx);
				usart_send_blocking(USART3, c);
			}
		}
	}
}
