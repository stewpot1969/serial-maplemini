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

static uint16_t bufcnt;
static char buf[BUFLEN+1];

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
  char local_buf[120];

  io_setup();
  uart_setup();
  
  printf("*** Try typing stuff in. ***\n");
  
  while(1)
  {
  printf("Please enter something :");
  fflush(stdout);
  fgets(local_buf, 32, stdin);
  printf("You typed in [%s]\n",local_buf);
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
	int	my_len=0;
  int bufptr=0;
  
	if (fd > 2) {
		return -1;
	}

	get_buffered_line();
	
	while ((bufptr<bufcnt) && (len > 0)) {
		*ptr++ = buf[bufptr++];
		my_len++;
		len--;
	}
	return my_len; /* return the length we got */
}

/*
 * A buffered line editing function.
 */
void get_buffered_line(void) {
  char c;
  
  /* Init vars */
  bufcnt=0;
  
  while (1) {
    /* get char */
    c = usart_recv_blocking(USART3);
    if (c == '\r') {        // CR = end
      buf[bufcnt++]='\n';
      buf[bufcnt++]='\0';
      usart_send_blocking(USART3, '\r');
			usart_send_blocking(USART3, '\n');
			return;
		}
    else if (c == 127) {   // DEL backspace
      if (bufcnt>0) {
        bufcnt--;
        usart_send_blocking(USART3, '\010');
	      usart_send_blocking(USART3, ' ');
	      usart_send_blocking(USART3, '\010');
      }
    }
    else if (bufcnt<BUFLEN) {
      buf[bufcnt++]=c;
      usart_send_blocking(USART3, c);   // echo input
    }
  }
}  
  
  

