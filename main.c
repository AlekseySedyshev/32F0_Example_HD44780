//Simple example how to use HD44780 with STM32F0xx
//Wrote by Aleksey Sedyshev, Dawinch.ru 
//For output information used 4  LCD pins D4..D8
//Pin description : Pb4...Pb7 - LCD D4..D7, Pb3 - LCD Rs, Pa10 - LCD E, Pb0 - Blinking Led
//in this Example i'm not use recommended microseconds delays for HD44780 , 
//if you need it  - change TIM17 configuration
//====================================== Display command============================================= 
// Clear display													0x01				Return cursor to home, and un-shift display	0x02
// move cursor right, don’t shift display	0x04    		move cursor right, do shift display (left)	0x05
// move cursor right, don’t shift display	0x06				move cursor right, do shift display (left)	0x07
// ===========Display control: The following control display properties	==============================
// turn display off												0x08…0x0B  	display on, cursor off,											0x0C
//display on, cursor on, steady cursor		0x0E				display on, cursor on, blinking cursor			0x0F
//===========The following commands move the cursor and shift the display	============================
//Shift cursor left												0x10				Shift cursor right													0x14
//Shift display left											0x18				Shift display right													0x1C


#include "stm32f0xx.h"        // Device header

#define RS_Hi()    GPIOB->BSRR |= 1<<3 //Pb3 - RS LCD Pin
#define RS_Lo()    GPIOB->BRR  |= 1<<3

#define E_Hi()    GPIOA->BSRR |= 1<<10 //Pa10 - E LCD Pin
#define E_Lo()   	GPIOA->BRR  |= 1<<10

uint32_t TimingDelay,led_count;

void TimingDelayDec(void) {
 if (TimingDelay			!=0x00) TimingDelay--;
 if (!led_count) {led_count=500; GPIOB->ODR ^=1;} //Blinking Led on Pb0 evey 500mSec
 led_count--;
 }

void TIM17_IRQHandler(void){
		if (TIM17->SR & TIM_SR_UIF) {	
			TimingDelayDec();	TIM17->SR &=(~TIM_SR_UIF);
		  }
}	
  void delay_ms (uint16_t mTime) {
    TimingDelay=mTime;
  while(TimingDelay!= 0x00);
}

void initial (void)
{
	/*timer config*/
  RCC->APB2ENR |= RCC_APB2ENR_TIM17EN;    						//HSI 8 MHz - 1 ms
  TIM17->PSC = 8000-1;				
  TIM17->ARR = 1;
  TIM17->CR1 |= TIM_CR1_ARPE | TIM_CR1_DIR | TIM_CR1_CEN; // 
	TIM17->DIER |=TIM_DIER_UIE;
				
	NVIC_EnableIRQ (TIM17_IRQn);
	NVIC_SetPriority(TIM17_IRQn,0x05);	
	
	RCC->AHBENR  |= RCC_AHBENR_GPIOBEN;
	GPIOB->MODER |= GPIO_MODER_MODER7_0 | GPIO_MODER_MODER6_0 | GPIO_MODER_MODER5_0;  // Pb3..Pb7 - LCD D4..D7,RS - Output mode
	GPIOB->MODER |= GPIO_MODER_MODER0_0 | GPIO_MODER_MODER3_0 | GPIO_MODER_MODER4_0;	// Pb0 - Led Blink
	
	RCC->AHBENR  |= RCC_AHBENR_GPIOAEN; 								
	GPIOA->MODER |= GPIO_MODER_MODER10_0;								//Pa10 - LCD E -Output mode
} 

void SendByte(unsigned char cmd)
{
	uint16_t RdPort = GPIOB->ODR; //for 16 Bit only 

	GPIOB->ODR =((RdPort & 0xFF0F) | ((cmd) & 0xF0));  //For 8 Bit MCU (((RdPort & 0x0F) | ((cmd) & 0xF0))
	E_Hi();
	delay_ms(1);
	E_Lo(); 

	GPIOB->ODR =((RdPort & 0xFF0F) | ((cmd << 4) & 0xF0));
	E_Hi();
	delay_ms(1);
	E_Lo(); 
}

void SendCommand(unsigned char cmd)
{
	RS_Lo(); 
	SendByte(cmd);
	delay_ms(1); //Should be 60uS
}

void SendData(unsigned char cmd)
{
	RS_Hi();
	SendByte(cmd);
	delay_ms(1); //Should be 60uS
}

void ClearLcd(void)
{
	SendCommand(0x1);
}

void Out(char line, char *str)
{
	if (line) {SendCommand(0xC0);}
		else 		{SendCommand(0x80);}
	
	while(*str)
	{
		SendData(*str++);
	}
}

void InitLcd()
{
	E_Lo(); RS_Lo();delay_ms(10);   
	SendCommand(0x33);SendCommand(0x2); 
	SendCommand(0x2C);SendCommand(0x1);
	SendCommand(0x06);
	SendCommand(0x0C); //display on, cursor off
}

int main(void)
{
initial();
InitLcd();
	Out(0, "HD44780 ");Out(1, "+ 32F042");
//-----------------------------initial data----------------------------------

while (1)  /* Main loop */
{

	for (char i=0;i<18;i++)
	{
		delay_ms(100);
		if (i<9) SendCommand(0x18);
			else 		SendCommand(0x1C);
		Out(0, "HD44780 ");Out(1, "+ 32F042");
	}

} // end - main loop 
} // end - Main  
	
#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{ while (1)  {  } }
#endif
