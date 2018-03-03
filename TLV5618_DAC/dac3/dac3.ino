// This program drives a TLV5618 dual DAC 
// It outputs sawtooth waveforms on both channels
// Output update frequency (both channels) is about 48kHz which 
// is sufficient for decent quality audio.
// A single channel can be driven at more than 100kHz.
#include <SPI.h>
#include <msp432.h>
#define EUSCI_B0_MODULE                                    (0x40002000)
#define NVIC_ISER0                                         (HWREG32(0xE000E100)) /* Irq 0 to 31 Set Enable Register */
#define SS_PIN 17
void setup()
{ 
  // put your setup code here, to run once:
  pinMode(SS_PIN,OUTPUT);
  pinMode(15,OUTPUT); // MOSI
  pinMode(7,OUTPUT); // SCK
  SPI.setModule(EUSCI_B0_MODULE); // Select correct SPI interface
  SPI.setDataMode(SPI_MODE2);
  SPI.setBitOrder(MSBFIRST);   
  SPI.setClockDivider(SPI_CLOCK_DIV2); // DIV2 = 8MHz, DIV4 = 4MHz, DIV8 = 2MHz etc. (from measurement)
  UCB0CTLW0 &= ~(BIT0); // Take SPI out of reset 
}
void writeDACA(int Value)
{
  Value=Value & 0xfff;
  Value = Value | 0xc000; // Write DAC A
  digitalWrite(SS_PIN,LOW); // Drive CS low
  UCB0TXBUF=( (Value >> 8) & 0xff); // write high byte
  while(UCB0STATW & BIT0); // wait while SPI busy
  UCB0TXBUF=  ( Value  & 0xff); // write low byte
  while(UCB0STATW & BIT0); // wait while SPI busy
  digitalWrite(SS_PIN,HIGH); // Drive CS High
}
void writeDACB(int Value)
{
  Value=Value & 0xfff;
  Value = Value | 0x4000; // Write DAC B value to buffer and update
  digitalWrite(SS_PIN,LOW); // Drive CS low
  UCB0TXBUF=( (Value >> 8) & 0xff); // write high byte
  while(UCB0STATW & BIT0); // wait while SPI busy
  UCB0TXBUF=  ( Value  & 0xff); // write low byte
  while(UCB0STATW & BIT0); // wait while SPI busy
  digitalWrite(SS_PIN,HIGH); // Drive CS High
}
void writeDACs(int AValue, int BValue)
{
  
  // Write both DACs and update outputs simultaneously.
  BValue=BValue & 0xfff;
  BValue = BValue | 0x5000; // Write DAC B value to buffer 
  digitalWrite(SS_PIN,LOW); // Drive CS low
  UCB0TXBUF=( (BValue >> 8) & 0xff); // write high byte
  while(UCB0STATW & BIT0); // wait while SPI busy
  UCB0TXBUF=  ( BValue  & 0xff); // write low byte
  while(UCB0STATW & BIT0); // wait while SPI busy
  digitalWrite(SS_PIN,HIGH); // Drive CS High
  digitalWrite(SS_PIN,LOW); // Drive CS low
  AValue=AValue & 0xfff;
  AValue = AValue | 0xc000; // Write DAC A and update B
  digitalWrite(SS_PIN,LOW); // Drive CS low
  UCB0TXBUF=( (AValue >> 8) & 0xff); // write high byte
  while(UCB0STATW & BIT0); // wait while SPI busy
  UCB0TXBUF=  ( AValue  & 0xff); // write low byte
  while(UCB0STATW & BIT0); // wait while SPI busy
  digitalWrite(SS_PIN,HIGH); // Drive CS High

}
int i=0;
void loop()
{
  // put your main code here, to run repeatedly:
  while(1)
  {
    writeDACs(i,0xfff-i);
    i++;
    if (i > 0xfff)
      i = 0;
  }
}

