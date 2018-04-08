#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <stdio.h>

/*
#ifndef __NO_SYSTEM_INIT
void SystemInit()
{}
#endif
*/

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
	usart_set_mode(USART3, USART_MODE_TX);
	usart_set_parity(USART3, USART_PARITY_NONE);
	usart_set_flow_control(USART3, USART_FLOWCONTROL_NONE);

  /* Enable it */
  usart_enable(USART3);
 
}

int main(void)
{  
  //rcc_clock_setup_in_hse_8mhz_out_72mhz();


  io_setup();
  uart_setup();
  
  while(1)
  {
    //int i;
    usart_send_blocking(USART3,'T');
    usart_send_blocking(USART3,'h');
    usart_send_blocking(USART3,'e');
    usart_send_blocking(USART3,' ');
    usart_send_blocking(USART3,'q');
    usart_send_blocking(USART3,'u');
    usart_send_blocking(USART3,'i');
    usart_send_blocking(USART3,'c');
    usart_send_blocking(USART3,'k');
    usart_send_blocking(USART3,' ');
    usart_send_blocking(USART3,'b');
    usart_send_blocking(USART3,'r');
    usart_send_blocking(USART3,'o');
    usart_send_blocking(USART3,'w');
    usart_send_blocking(USART3,'n');
    usart_send_blocking(USART3,' ');
    usart_send_blocking(USART3,'f');
    usart_send_blocking(USART3,'o');
    usart_send_blocking(USART3,'x');
    usart_send_blocking(USART3,' ');
    usart_send_blocking(USART3,'j');
    usart_send_blocking(USART3,'u');
    usart_send_blocking(USART3,'m');
    usart_send_blocking(USART3,'p');
    usart_send_blocking(USART3,'e');
    usart_send_blocking(USART3,'d');
    usart_send_blocking(USART3,' ');
    usart_send_blocking(USART3,'o');
    usart_send_blocking(USART3,'v');
    usart_send_blocking(USART3,'e');
    usart_send_blocking(USART3,'r');
    usart_send_blocking(USART3,' ');
    usart_send_blocking(USART3,'t');
    usart_send_blocking(USART3,'h');
    usart_send_blocking(USART3,'e');
    usart_send_blocking(USART3,' ');
    usart_send_blocking(USART3,'l');
    usart_send_blocking(USART3,'a');
    usart_send_blocking(USART3,'z');
    usart_send_blocking(USART3,'y');
    usart_send_blocking(USART3,' ');
    usart_send_blocking(USART3,'d');
    usart_send_blocking(USART3,'o');
    usart_send_blocking(USART3,'g');
    usart_send_blocking(USART3,'s');
    usart_send_blocking(USART3,'.');
    usart_send_blocking(USART3,'\r');
    usart_send_blocking(USART3,'\n');
    //for (i = 0; i < 1000000; i++)	/* Wait a bit. */
		//	__asm__("nop");
  }
}

