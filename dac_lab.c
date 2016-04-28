// 0.Documentation Section 
// SoftSound.c
// Runs on LM4F120 or TM4C123
// Use the SysTick timer to request interrupts at 500Hz.
// Jonathan Valvano
// November 3, 2013


#include "PLL.h"
#include "tm4c123gh6pm.h"
// Constant declarations to access port registers using 
// symbolic names instead of addresses

void SysTick_Wait10ms(unsigned long delay);
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // low power mode
void Port_Init(void);        // start sound output
void SysFun(void);
void SysLoad(unsigned long period);
unsigned int i=0;
// 3. Subroutines Section
// MAIN: Mandatory for a C Program to be executable
unsigned long sw1, sw2;
unsigned long int ram, ram1 ;

int main(void){
	//unsigned int j=0;
  PLL_Init();   
  	// 80 MHz
	SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOB;
	
	
GPIO_PORTB_DIR_R = 0x9F;
GPIO_PORTB_DEN_R = 0xFF;
	
	
  Port_Init();   
	// initialize output and interrupts
  EnableInterrupts();
	/*Initialize ports and timers*/
	SysFun();
  while(1){   
		sw1 = GPIO_PORTB_DATA_R&(0x20);
		sw2 = GPIO_PORTB_DATA_R & (0x40);
			//				GPIO_PORTA_DATA_R = 0x04|0x10;
			//		for(ram1 =0; ram1<100; ram1++){
		//			SysLoad(40000);            // 1sec
		//			}
		//			GPIO_PORTA_DATA_R = 0x00;
		//			for(ram1 =0; ram1<100; ram1++){
		//			SysLoad(40000);            // 1sec
		//			}
		
		if(sw1&&(!sw2)){
			
			for(ram =0; ram<5; ram++){
			GPIO_PORTA_DATA_R = 0x04;
				for(ram1 =0; ram1<170; ram1++){
			SysLoad(40000);           //153msec
				}
			GPIO_PORTA_DATA_R = 0x00;
			for(ram1 =0; ram1<170; ram1++){
			SysLoad(40000); //153msec
				}	
			}
			
			for(ram =0; ram<7; ram++){
			GPIO_PORTA_DATA_R = 0x10;
			for(ram1 =0; ram1<280; ram1++){
			SysLoad(40000);  // 275msec
				}
			GPIO_PORTA_DATA_R = 0x00;
			for(ram1 =0; ram1<280; ram1++){
			SysLoad(40000);            // 275msec
				}
			}
		}
		else if((!sw1)&&(sw2)){
		
				for(ram =0; ram<5; ram++){
					GPIO_PORTA_DATA_R = 0x08;
					for(ram1 =0; ram1<2000; ram1++){
					SysLoad(40000);            // 2sec
					}
					GPIO_PORTA_DATA_R = 0x00;
					for(ram1 =0; ram1<2000; ram1++){
					SysLoad(40000);            // 2sec
					}
				}
				for(ram =0; ram<3; ram++){
					GPIO_PORTA_DATA_R = 0x04|0x10;
					for(ram1 =0; ram1<1000; ram1++){
					SysLoad(40000);            // 1sec
					}
					GPIO_PORTA_DATA_R = 0x00;
					for(ram1 =0; ram1<1000; ram1++){
					SysLoad(40000);            // 1sec
					}
				}
		}
		
		
		else if(sw1&&sw2){
					for(ram =0; ram<3; ram++){
					GPIO_PORTA_DATA_R = 0x04|0x10|0x08;
					for(ram1 =0; ram1<1000; ram1++){
					SysLoad(40000);            // 1sec
					}
					GPIO_PORTA_DATA_R = 0x00;
					for(ram1 =0; ram1<1000; ram1++){
					SysLoad(40000);            // 1sec
					}
				}
		
		}
		
		else if((!sw1)&&(!sw2)){
					GPIO_PORTA_DATA_R = 0x04|0x10|0x08;
		}
		
		
		/*Your code goes here*/
		//for(j=0;j<50;j++)
		//{
			//GPIO_PORTA_DATA_R |= 0x20;
			//SysTick_Wait10ms(4);
	//		GPIO_PORTA_DATA_R &= ~0x20;
	//		SysTick_Wait10ms(36);
		//}
		//for(j=0;j<50;j++)
		//{
			//GPIO_PORTA_DATA_R |= 0x20;
			//SysTick_Wait10ms(3);
			//GPIO_PORTA_DATA_R &= ~0x20;
			//SysTick_Wait10ms(37);
		//}
		//for(j=0;j<50;j++)
		//{
		//	GPIO_PORTA_DATA_R |= 0x20;
		//	SysTick_Wait10ms(3);
		//	GPIO_PORTA_DATA_R &= ~0x20;
		//	SysTick_Wait10ms(37);
			
		//	GPIO_PORTA_DATA_R |= 0x20;
		//	SysTick_Wait10ms(3);
		//	GPIO_PORTA_DATA_R &= ~0x20;
		//	SysTick_Wait10ms(37);
		//}
		
		
		/*for(j=0;j<50;j++)
		{
			GPIO_PORTA_DATA_R |= 0x20;
			SysTick_Wait10ms(2);
			GPIO_PORTA_DATA_R &= ~0x20;
			SysTick_Wait10ms(38);
		}
		for(j=0;j<50;j++)
		{
			GPIO_PORTA_DATA_R |= 0x20;
			SysTick_Wait10ms(4);
			GPIO_PORTA_DATA_R &= ~0x20;
			SysTick_Wait10ms(36);
		}*/
  }
}
// **************Port_Init*********************
// Initialize SysTick periodic interrupts
// Input: none
// Output: none
void Port_Init(void){ unsigned long volatile delay;
  SYSCTL_RCGC2_R |= 0x00000001; // activate port A
  delay = SYSCTL_RCGC2_R;
  GPIO_PORTA_AMSEL_R &= ~0x1C;      // no analog 
  GPIO_PORTA_PCTL_R &= ~0x00F00000; // regular function
  GPIO_PORTA_DIR_R |= 0x1C;     // make PA5 out
  GPIO_PORTA_DR8R_R |= 0x1C;    // can drive up to 8mA out
  GPIO_PORTA_AFSEL_R &= ~0x1C;  // disable alt funct on PA5
  GPIO_PORTA_DEN_R |= 0x1C;     // enable digital I/O on PA5
}

void SysLoad(unsigned long period){
	
	NVIC_ST_RELOAD_R = period-1;  // number of counts to wait
  NVIC_ST_CURRENT_R = 0;       // any value written to CURRENT clears
  while((NVIC_ST_CTRL_R&0x00010000)==0){ // wait for count flag
  }
	
}

void SysFun(void){
	
	NVIC_ST_CTRL_R = 0;               // disable SysTick during setup
  NVIC_ST_CTRL_R = 0x00000005;
          
} 

void SysTick_Wait10ms(unsigned long delay){
  unsigned long i;
  for(i=0; i<delay; i++){
    SysLoad(40000);  // wait 10ms
  }
}




