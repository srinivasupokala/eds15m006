// ***** 0. Documentation Section *****
// TableTrafficLight.c for Lab 10
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Daniel Valvano, Jonathan Valvano
// January 15, 2016

// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

// ***** 2. Global Declarations Section *****
void SysTick_Init(void);
void SysTick_Wait(unsigned long delay);
void SysTick_Wait10ms(unsigned long delay);
void PortF_Init(void);
void PortE_Init(void);
void PortB_Init(void);

struct State {
  unsigned long Out1;
  unsigned long Out2;  // 6-bit pattern to output
  unsigned long Time; // delay in 10ms units 
  unsigned long Next[9];}; // next state for inputs 0,1,2,3,4,5,6,7,8,9
typedef const struct State STyp;
#define allred   0
#define goW      1
#define waitW    2
#define goS      3
#define waitS    4
#define goWG     5
#define goDW     6
#define goDWS    7
#define goDWW    8
STyp FSM[9]={
 {0x24,0x02,100,{allred,goW,goS,goS,goWG,goW,goS,goS}}, 
 {0x0C,0x02,100,{goW,goW,waitW,waitW,waitW,waitW,waitW,waitW}},
 {0x14,0x02,100,{allred,goS,goS,goS,goWG,goWG,goS,goWG}},
 {0x21,0x02,100,{goS,waitS,goS,waitS,waitS,waitS,waitS,waitS}},
 {0x22,0x02,100,{allred,goW,goW,goW,goWG,goW,goWG,goW}},
 {0x24,0x08,100,{goWG,goDW,goDW,goDW,goWG,goDW,goDW,goDW}},
 {0x24,0x00,100,{allred,goDWW,goDWS,goDWS,goWG,goDWW,goDWS,goDWS}},
 {0x21,0x02,100,{goS,waitS,goS,waitS,waitS,waitS,waitS,waitS}},
 {0x0C,0x02,100,{goW,goW,waitW,waitW,waitW,waitW,waitW,waitW}}};
unsigned long s;
unsigned long Input;

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

// ***** 3. Subroutines Section 

void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;                   // disable SysTick during setup
  NVIC_ST_RELOAD_R = 0x00FFFFFF;        // maximum reload value
  NVIC_ST_CURRENT_R = 0;                // any write to current clears it             
  NVIC_ST_CTRL_R = 0x00000005;          // enable SysTick with core clock
}


void SysTick_Wait(unsigned long delay)
{ NVIC_ST_RELOAD_R = delay-1;
	NVIC_ST_CURRENT_R = 0;
	while((NVIC_ST_CTRL_R&0x00010000)==0)
	{
	}
}


void SysTick_Wait10ms(unsigned long delay){
  unsigned long i;
  for(i=0; i<delay; i++){
    SysTick_Wait(160000);  // wait 10ms (assumes 50 MHz clock)
  }
}
//port initializations

void PortF_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000020;     // 1) activate clock for Port F
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
	GPIO_PORTF_CR_R = 0x1F;           // allow changes to PF4-0
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog on PF
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PF4-0
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF4,PF0 in, PF3-1 out
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) disable alt funct on 
  GPIO_PORTF_DEN_R = 0x1F;          // 7) enable digital I/O on PF4-0
}

void PortB_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000002;     
  delay = SYSCTL_RCGC2_R;
	GPIO_PORTB_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
	GPIO_PORTB_CR_R = 0xFF; 
  GPIO_PORTB_AMSEL_R = 0x00;        
  GPIO_PORTB_PCTL_R = 0x00000000;   
  GPIO_PORTB_DIR_R = 0xFF;          
  GPIO_PORTB_AFSEL_R = 0x00;        
  GPIO_PORTB_DEN_R = 0xFF;          
}

void PortE_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000010;     // 1) activate clock for Port F
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
                                     // only PF0 needs to be unlocked, other bits can't be locked
	GPIO_PORTE_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
	GPIO_PORTE_CR_R = 0x1F; 
  GPIO_PORTE_AMSEL_R = 0x00;        // 3) disable analog on PF
  GPIO_PORTE_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PF4-0
  GPIO_PORTE_DIR_R = 0x00;          // 5) last 3 bits as input
  GPIO_PORTE_AFSEL_R = 0x00;        // 6) disable alt funct on PF7-0
  GPIO_PORTE_DEN_R = 0x07;          // 7) enable digital I/O on all last 3 bits
}


int main(void){ 
  TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210,ScopeOff); // activate grader and set system clock to 80 MHz
	SysTick_Init();
	PortF_Init();
	PortB_Init();
	PortE_Init();
	s = allred;
 
  
  EnableInterrupts();
  while(1){
		GPIO_PORTB_DATA_R = FSM[s].Out1;
		GPIO_PORTF_DATA_R = FSM[s].Out2;
		SysTick_Wait10ms(FSM[s].Time);
		Input=GPIO_PORTE_DATA_R & 0x07;
		s=FSM[s].Next[Input];
		
		
		
     
  }
}


