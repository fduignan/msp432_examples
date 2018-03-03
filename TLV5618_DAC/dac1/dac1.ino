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
  SPI.setModule(EUSCI_B0_BASE);
  SPI.setDataMode(SPI_MODE1);
  SPI.setBitOrder(MSBFIRST);   // 
  SPI.setClockDivider(SPI_CLOCK_DIV2); // DIV2 = 8MHz, DIV4 = 4MHz, DIV8 = 2MHz etc. (from measurement)
}
void writeDACA(int Value)
{
  Value=Value & 0xfff;
  Value = Value | 0xc000; // Write DAC A
  digitalWrite(SS_PIN,LOW);
  SPI.transfer( (Value >> 8) & 0xff);
  SPI.transfer( Value  & 0xff);
  digitalWrite(SS_PIN,HIGH);
}
int i=0;
void loop()
{
  // put your main code here, to run repeatedly:
  while(1)
  {
    writeDACA(i);
    i++;
    if (i > 0xfff)
      i = 0;
  }

}

