/* Periodic interrupt example for the MSP432 development board and the energia development environment */
/* The function OnTimer is called every millisecond and flashes the green LED every second
   The main loop flashes the blue LED every second
   Timer A3 is used to generate the interrupt
*/
  
#include <msp432.h>
#include <driverlib/MSP432P4xx/timer_a.h>
#include <driverlib/MSP432P4xx/interrupt.h>
#define NVIC_ISER0                                         (HWREG32(0xE000E100)) /* Irq 0 to 31 Set Enable Register */

/*
General notes: serial comms uses eusci_a
PWM uses Timer A0 (at least I think so)
The built-in delay function appears not to work if it expires at the same time as a timer interrupt
The delayMicroseconds appears not to be so affected (so far)
*/
volatile uint32_t millisecondCounter=0;
int count = 0;
volatile int state = HIGH;
volatile int flag = HIGH;
  
void setup()
{
  Serial.begin(9600);
  
  pinMode(BLUE_LED,OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  setupTimer(1000); // set timer period to 1000 micro seconds
}
  
void OnTimer()
{
  
  static int Count = 0;
  static int state = 0;
  Count++;
  if (Count > 1000)
  {
    Count = 0;
    digitalWrite(GREEN_LED,state);
    if (state)
      state = 0;
    else
      state = 1;
  }
}
void loop()
{
  
  digitalWrite(BLUE_LED,HIGH);
  delayMicroseconds(500000);
  digitalWrite(BLUE_LED,LOW);
  delayMicroseconds(500000);
  Serial.println(millisecondCounter);
}
  
void setupTimer(unsigned Period)
{
  
  // Configuration word
  // Bits 15-10: Unused
  // Bits 9-8: Clock source select: set to SMCLK (12MHz)
  // Bits 7-6: Input divider: set to 4
  // Bits 5-4: Mode control: Count up to TACCRO and reset
  // Bit 3: Unused
  // Bits 2: TACLR : set to initially clear timer system
  // Bit 1: Enable interrupts from TA0
  // Bit 0: Interrupt (pending) flag : set to zero (initially)
  TA3CTL=0b0000001010010110;
  TA3CCR0=Period*3; // Set TACCR0 = Period (3MHz clock)
  TA3CCTL0=BIT4; // Enable interrupts when TAR = TACCR0
// The following places the address of our interrupt service routine in the RAM based interrupt vector table
// The vector number is 14 + 16  = 30 which is represented by the symbol INT_TA3_0
  Interrupt_registerInterrupt(INT_TA3_0,timerA3ISR); 
  
  // according to the datasheet Table 6-12 timer A3 is on ISR 14
  NVIC_ISER0 = (1<<14); // enable this interrupt in the NVIC
  
}
  
void timerA3ISR(void)
{
  TA3CTL &= ~1;         // Acknowledge the interrupt
  TA3CCTL0 &= ~1;       // Acknowledge the interrupt
  NVIC->ICPR[0] = (1<<14); // clear interrupt pending flag in NVIC
  millisecondCounter++;
  OnTimer();
}

