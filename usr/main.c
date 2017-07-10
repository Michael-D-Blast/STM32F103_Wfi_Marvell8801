#include <stdlib.h>
#include <stdio.h>
#include "stm32f10x.h"
#include "stm32f10x_conf.h"		// includes all drivers' header files


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else	// use this
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* Private functions ---------------------------------------------------------*/

void delay(unsigned int count)
{
	while(count > 0)
		count--;
}

void STM_EVAL_COMInit(USART_InitTypeDef* USART_InitStruct)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable GPIO clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);	 // TX - PA9   RX - PA10

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); 	// DEBUG UART - UART1

  /* Configure USART Tx as alternate function push-pull */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);


  /* Configure USART Rx as input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  /* USART configuration */
  USART_Init(USART1, USART_InitStruct);
    
  /* Enable USART */
  USART_Cmd(USART1, ENABLE);
}

void init_tty_console(void)
{
	USART_InitTypeDef stDebugUart;
  /* USARTx configured as follow:
        - BaudRate = 115200 baud  
        - Word Length = 8 Bits
        - One Stop Bit
        - No parity
        - Hardware flow control disabled (RTS and CTS signals)
        - Receive and transmit enabled
  */
 	stDebugUart.USART_BaudRate = 115200;
	stDebugUart.USART_WordLength = USART_WordLength_8b;
	stDebugUart.USART_StopBits = USART_StopBits_1;
	stDebugUart.USART_Parity = USART_Parity_No;
	stDebugUart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	stDebugUart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	STM_EVAL_COMInit(&stDebugUart);
}

int main(void)
{
	int i;
	GPIO_InitTypeDef stGpio;

//	init_tty_console();
//	delay(1000);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	stGpio.GPIO_Pin = GPIO_Pin_9;
	stGpio.GPIO_Mode = GPIO_Mode_Out_PP;
	stGpio.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOB, &stGpio);
//	printf("\r\n STM32F10x Firmware Library compiled with FULL ASSERT function... \n\r");
//	printf("...Run-time checking enabled  \n\r");

	for (i = 0; i < 100000; i ++)
	{
		if (i & 0x1)
			GPIO_WriteBit(GPIOB, GPIO_Pin_9, Bit_SET);
		else
			GPIO_WriteBit(GPIOB, GPIO_Pin_9, Bit_RESET);

		delay(0x1000000);
	}
		
	return 0;
}

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */

  while(!(USART1->SR & USART_FLAG_TXE));

  USART_SendData(USART1, (uint8_t) ch);

  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
  {}

  return ch;
}
