/*****************************************************
This program was produced by the
CodeWizardAVR V2.04.4a Advanced
Automatic Program Generator
© Copyright 1998-2009 Pavel Haiduc, HP InfoTech s.r.l.
http://www.hpinfotech.com

Project : 
Version : 
Date    : 17.03.2011
Author  : NeVaDa
Company : Rusich
Comments: 


Chip type               : ATmega16
Program type            : Application
AVR Core Clock frequency: 8,000000 MHz
Memory model            : Small
External RAM size       : 0
Data Stack size         : 256
*****************************************************/

#include <mega16.h>
#include <stdio.h>
#include <delay.h>
#include <stdlib.h>
#include "sht1x.h"
unsigned char data,checksum;
 char Stat1, Stat2=0x30,Stat3=0x31,rndvect;
unsigned int rndvect1,mvect,Uroven,Uroven1;
unsigned int Povorot_Vector1=0,Povorot_Vector2=0;
int a[13],tik1,tik2,time_GPS,Razn;  
unsigned int i,j,k,p,k1;
unsigned char error,checksum;   
unsigned char inp;
unsigned char Grad1[2],Grad2,buffer1[10], buffer2[90]; 
unsigned char Ur1, Ur2, Ur3; 
float Ugol_Povorota=150;
float fi1, fi2,fires;
float t,rh,t2,rh2,sh;   
float Pd1,Pd2,FlagAntStart; 
float result,DataLirResult,DatL,DataLir1,DataLir2;
float stakveter1=0,stakveter2=0;
float timetik1=0, timetik2=0;
float Skorost_vetra,Napravlenie_vetra, Opornaya_Seriya, Osnovnaya_Seriya,Polojenie_Ant;
float front1=0, front2=0, front3,front4,UgolN,Polojenie_AntStart, Polojenie_AntStop,Ugol; 
unsigned char data1; 
value humi_val,temp_val;  
//----------------------------------------------------------------------------------------    
// calculates temperature [°C] and humidity [%RH]     
//----------------------------------------------------------------------------------------    
  const float C1=-4.0;              // for 12 Bit    
  const float C2=+0.0405;           // for 12 Bit    
  const float C3=-0.0000028;        // for 12 Bit    
  const float T1=+0.01;             // for 14 Bit @ 5V    
  const float T2=+0.00008;           // for 14 Bit @ 5V 
    
  const float FREQ=122.07;     //8000000/65535=122.07      

//----------------------------------------------------------------------------------------
unsigned char USART_Receive( void )
{
    /* Wait for data to be received */
    while ( !(UCSRA & (1<<RXC)) )
        ;
    return UDR;
}

//----------------------------------------------------------------------------------------
void USART_Transmit(unsigned char data )
{
    /* Wait for empty transmit buffer */
    while ( !( UCSRA & (1<<UDRE)) )
        ;
    UDR = data;
}

//I2C Function write---------------------------------------------------------------------
char s_write_byte(unsigned char val)      
{     
   unsigned char i, error = 0;      
   DATA_IO = 1;
   SCK_IO = 1;  
   for (i = 0x80; i > 0; i /= 2)                       
   {      
      if (i & val)       
         DATA_OUT = 1;                 
      else 
         DATA_OUT = 0;       
      delay_us(2);                         
      SCK=1;                              
      delay_us(5);                                                           
      SCK=0;           
   }    
         
  DATA_OUT = 0;                               
  SCK_IO = 1;
  DATA_IO = 0;  
  SCK = 1;                          
  delay_us(2);    
  error = DATA_IN;                   
  delay_us(2);    
  SCK = 0;            
  return error;                             
}
//----------------------------------------------------------------------------------    
// reads a byte I2C     
//----------------------------------------------------------------------------------    
   
char s_read_byte(unsigned char ack)    
{     
  unsigned char i,val=0;    
  SCK_IO=1;  
  for (i=0x80;i>0;i/=2)             
  {
        SCK=1;                              
        delay_us(2);    
        if (DATA_IN) val=(val | i);       
        SCK=0;                                               
        delay_us(2);    
  }  
       
  DATA_IO=1;
  SCK_IO=1;    
  DATA_OUT=!ack;                          
  SCK=1;                            
  delay_us(5);          
  SCK=0;                                                                             
  SCK_IO=1;  
  DATA_IO=0;
  return val;    
}    
//----------------------------------------------------------------------------------    
// generates a transmission start     
//       _____         ________    
// DATA:      |_______|    
//           ___     ___    
// SCK : ___|   |___|   |______    
//----------------------------------------------------------------------------------    
   
void s_transstart(void)    
{              
   DATA_IO=1;
   SCK_IO=1;   
   DATA_OUT=1; SCK=0;                   //Initial state    
   delay_us(2);    
   SCK=1;    
   delay_us(2);    
   DATA_OUT=0;    
   delay_us(2);    
   SCK=0;      
   delay_us(5);    
   SCK=1;    
   delay_us(2);    
   DATA_OUT=1;                       
   delay_us(2);    
   SCK=0;          
   SCK_IO=1;  
}                                           
   
//----------------------------------------------------------------------------------    
// communication reset: DATA-line=1 and at least 9 SCK cycles followed by transstart    
//       _____________________________________________________         ________    
// DATA:                                                      |_______|    
//          _    _    _    _    _    _    _    _    _        ___     ___    
// SCK : __| |__| |__| |__| |__| |__| |__| |__| |__| |______|   |___|   |______    
//----------------------------------------------------------------------------------    
   
void s_connectionreset(void)    
{      
  unsigned char i;     
  DATA_IO=1;
  SCK_IO=1;   
  DATA_OUT=1; SCK=0;                    //Initial state    
  for(i=0;i<9;i++)                  //9 SCK cycles    
  { SCK=1;     
    delay_us(1);    
    SCK=0;    
    delay_us(1);    
  }    
  s_transstart();                   //transmission start    
  SCK_IO=1;  
}    
              
//----------------------------------------------------------------------------------    
// resets the sensor by a softreset     
//----------------------------------------------------------------------------------    
   
char s_softreset(void)    
{     
  unsigned char error=0;      
  s_connectionreset();              //reset communication    
  error+=s_write_byte(RESET);       //send RESET-command to sensor    
  return error;                     //error=1 in case of no response form the sensor    
}    
//----------------------------------------------------------------------------------    
// reads the status register with checksum (8-bit)    
//----------------------------------------------------------------------------------    
   
char s_read_statusreg(unsigned char *p_value, unsigned char *p_checksum)    
{     
  unsigned char error=0;    
  s_transstart();                   //transmission start    
  error=s_write_byte(STATUS_REG_R); //send command to sensor    
  *p_value=s_read_byte(ACK);        //read status register (8-bit)    
  *p_checksum=s_read_byte(noACK);   //read checksum (8-bit)      
  return error;                     //error=1 in case of no response form the sensor    
}                              
   
//----------------------------------------------------------------------------------    
// writes the status register with checksum (8-bit)    
//----------------------------------------------------------------------------------    
   
char s_write_statusreg(unsigned char *p_value)    
{     
  unsigned char error=0;    
  s_transstart();                   //transmission start    
  error+=s_write_byte(STATUS_REG_W);//send command to sensor    
  error+=s_write_byte(*p_value);    //send value of status register    
  return error;                     //error>=1 in case of no response form the sensor    
}    
   
//----------------------------------------------------------------------------------    
// makes a measurement (humidity/temperature) with checksum    
//----------------------------------------------------------------------------------    
   
char s_measure(unsigned char *p_value, unsigned char *p_checksum, unsigned char mode)    
{     
  unsigned error=0;    
  unsigned int cnt=0; 
  s_transstart();                   //transmission start        
   
  switch(mode)
  {                     
         case TEMP        : error+=s_write_byte(MEASURE_TEMP); break;    
         case HUMI        : error+=s_write_byte(MEASURE_HUMI); break;    
         default     : break;             
  }                      
 
  SCK_IO=1;
  cnt=0;         
  while (1)    
  {    
         if(DATA_IN==0) break; 
         cnt++;  
  }    
        
  *(p_value+1)  =s_read_byte(ACK);    //read the first byte (MSB)    
  *(p_value)=s_read_byte(ACK);    //read the second byte (LSB)    
  *p_checksum =s_read_byte(noACK);  //read checksum         
  return error;    
}    

//----------------------------------------------------------------------------------    
//Function Random Rotate Antenna   
//---------------------------------------------------------------------------------- 
void Random_Vector( unsigned char rndvect)             
{

if (rndvect==0x30){PORTB.0=0;PORTB.1=0; }
if (rndvect==0x31)
        { 
          rndvect1=0+rand()%3;
          switch (rndvect1)
          {
          case 0:PORTB.0=0;PORTB.1=0;  break;
          case 1:PORTB.0=0;PORTB.1=1;  break;
          case 2:PORTB.1=0;PORTB.0=1;  break;
          }
        }
     
}

//----------------------------------------------------------------------------------    
//Control Uroven Signal Antenna
//---------------------------------------------------------------------------------- 
void Uroven_Vector( float UgolN)                 
{  DataLir1=DataLirResult;
   if(UgolN<0){PORTB.0=0;PORTB.1=1;UgolN=(-1)*(DataLirResult-DataLir1);}
   if(UgolN>0){PORTB.1=0;PORTB.0=1;UgolN=(DataLirResult-DataLir1);} 
}
//----------------------------------------------------------------------------------    
//Manual Control Antenna
//---------------------------------------------------------------------------------- 
void Manual_Vector(float fi2)                  
{  
   if (fi2==1){PORTB.0=0;PORTB.1=1;}
   if (fi2==2){PORTB.1=0;PORTB.0=1;} 
   if (fi2==0){PORTB.1=0;PORTB.0=0;}

 
 /*
   if (rndvect==0x31) goto label2;
   if (fi2<0){fi2=fi2*(-1);}
   if ((fi2<0)&&(fi1<fi2)&&(DataLirResult>(360-fi2+fi1))){PORTB.1=0;PORTB.0=1;}
   if ((fi2<0)&&(fi1>=fi2)&&(DataLirResult>(fi1-fi2))){PORTB.1=0;PORTB.0=1;}
   if ((fi2>0)&&((fi1+fi2)<=360)&&(DataLirResult<(fi1+fi2))){PORTB.0=0;PORTB.1=1;}
   if ((fi2>0)&&((fi1+fi2)>360)&&(DataLirResult<(fi1+fi2-360))){PORTB.0=0;PORTB.1=1;}
   label2:;
 */
 } 
/*  
Shim_ini1(int k)
{

    while(k1<=k)
        {Manual_Vector(1);
        delay_us(1000);
        k1=k1+1; 
        Manual_Vector(0);  
        delay_us(4000);
        } 
k1=0;        


}
Shim_ini2(int k)
{
while(k1<=k)
        {Manual_Vector(2);
        delay_us(1000);
        k1=k1+1; 
        Manual_Vector(0);
        delay_us(4000);
        } 
k1=0;        
}
*/
/*
    DDRD.7 =1;
    PORTD = 0;  
    OCR2=k; 
    TCCR2 |= (1 << WGM20) | ( 1 << WGM21) | (1 << COM21) | (1 << CS22);   
*/  
  //  TIMSK |= (1 << OCIE1A);
   // delay_ms(ch);


//----------------------------------------------------------------------------------    
//Lir Data Request
//---------------------------------------------------------------------------------- 

void LIR(void)
{ //delay_ms(200);
   
    DDRC.3=1; 

   

 
    PORTC.3=1;
    delay_us(1);
    PORTC.3=0;  
    delay_us(1);
   
    PORTC.3=1;
    delay_us(1);
    PORTC.3=0;  
    delay_us(1);
 
        while (i!=0)
     { 
          i--;
          PORTC.3=1;
          a[i]=PINC.2;
          delay_us(1);
          PORTC.3=0;
     }  
      
      PORTC.3=1;  
      delay_us(30);
      i=14;  
      DDRC.3=0;
} 
//        _    _    _    _    _    _    _    _    _    _   _______________        
// Clock : |__| |__| |__| |__| |__| |__| |__| |__| |__| |_|  
//        _____    _____    _____    _____    _____    _____       ____________      
// Data :      |__|     |__|     |__|     |__|     |__|     |_____|     
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------------    
//SHT10(humidity,temperature) Data Request
//---------------------------------------------------------------------------------- 

void SHT10(void)
{
    error=0;
    delay_ms(200);             
    error+=s_measure((unsigned char*) &humi_val.i,&checksum,HUMI);  //measure humidity    
    delay_ms(200);    
    error+=s_measure((unsigned char*) &temp_val.i,&checksum,TEMP);  //measure temperature                      
    error += s_read_statusreg(&inp, &checksum);  
   
    t=temp_val.i; 
    rh=humi_val.i;
    t=temp_val.i*0.01 - 40.07;      
    rh = C3*rh*rh + C2*rh + C1-4;    
   
}

//-----------------------------------------------------------------------------
            
//----------------------------------------------------------------------------------    
//Datchik Vetra Data Request
//---------------------------------------------------------------------------------- 

interrupt [EXT_INT0] void ext_int0_isr(void)       
{
 
 stakveter1=stakveter1+1;
 front2=TCNT1+timetik1*65535; 
 front4=front3-front2; 
 front3=TCNT1+timetik1*65535;
 if (stakveter1==1) {front1=TCNT1+timetik1*65535; }
 if (stakveter1==2)
  {
        Opornaya_Seriya=(TCNT1+timetik1*65535-front1)/8000000;      
         //Period osnovnoi serii impulsov DVM(Skorost vetra)
        if (Opornaya_Seriya<=0){Opornaya_Seriya=Opornaya_Seriya*(-1);}
        stakveter1=0;
  }

}

// External Interrupt 1 service routine
interrupt [EXT_INT1] void ext_int1_isr(void)
{
// stakveter2=stakveter2+1;

Osnovnaya_Seriya=(TCNT1+timetik1*65535-front2)/8000000;
if (Osnovnaya_Seriya<=0){Osnovnaya_Seriya=Osnovnaya_Seriya*(-1);} 

}

//----------------------------------------------------------------------------

// Timer1 overflow interrupt service routine

interrupt [TIM1_OVF] void timer1_ovf_isr(void)
{
timetik1=timetik1+1;
timetik2=timetik2+1;
 //Manual_Vector(2);
   // Manual_Vector (0);
if(timetik2>=8000000/(30*65535))                        // Таймер для регулирования угла поворота
{     
          
               LIR(); 
        if (DataLirResult!=0){DatL=DataLirResult;}   
         Polojenie_Ant=(360*DataLirResult)/4096;          
         
        
        /* 
           if ((Ugol_Povorota-Polojenie_Ant)>=50||(Polojenie_Ant-Ugol_Povorota)>=50)  
           {    
            Razn=Ugol_Povorota-Polojenie_Ant;  
            if (Razn<0){Razn=Razn*(-1);}
          // if (Ugol_Povorota-Polojenie_Ant>0){Manual_Vector(2);}
            if (Ugol_Povorota-Polojenie_Ant>0){Manual_Vector(2);delay_ms(100);Manual_Vector(0);}  
            if (Ugol_Povorota-Polojenie_Ant<0){Manual_Vector(1);delay_ms(100);Manual_Vector(0);} 
            
          //  if (Ugol_Povorota-Polojenie_Ant<0){Manual_Vector(1);}  
            } 
          */  
               //Shim_ini((Ugol_Povorota-Polojenie_Ant)*1024/20);
              //  if (Ugol_Povorota==0) Manual_Vector(0);
           //    if (Ugol_Povorota<Polojenie_Ant) {Manual_Vector(1);}
               //delay_ms(100); Manual_Vector(0);  
              
              //  if (Ugol_Povorota==0) Manual_Vector(0);   
           //  if (Ugol_Povorota>Polojenie_Ant) {Manual_Vector(2);} 
             //  delay_ms(100); Manual_Vector(0);  
        

   //   Manual_Vector(0);
   
  timetik2=0; 
}

//-------------------------------------------------------------------------------------------
if (timetik1>=FREQ)                                     //Основной таймер
{   

// Manual_Vector(Stat1);
 //Uroven_Vector(Stat2);  

FlagAntStart=0;
  
        
   

 if (Opornaya_Seriya==0){Opornaya_Seriya=999999;}
                                                                                                                                                                                                                         
 while (time_GPS<50)
      {
            Stat1=USART_Receive();
            USART_Transmit(Stat1);   
            time_GPS=time_GPS+1;
     }  
  FlagAntStart=1;    
    time_GPS=0; 
 
  SHT10();
 
  printf("%.2f %.2f %.2f %.2f %.2f %.2f\r",Polojenie_Ant,1/(0.75*Opornaya_Seriya),360-(Osnovnaya_Seriya/Opornaya_Seriya)*360,t,rh,Ugol_Povorota);   
   
     
 Osnovnaya_Seriya=0;
 Opornaya_Seriya=0;
 front1=0;
 front2=0;
 front4=0;
 front3=0;
 stakveter1=0;
 stakveter2=0;
 timetik1=0;
// Random_Vector(0x31);

}
}



// Declare your global variables here

void main(void)
{
// Declare your local variables here

// Input/Output Ports initialization
// Port A initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTA=0x00;
DDRA=0x00;

// Port B initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 




// Port C initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTC=0x00;
DDRC=0x00;

// Port D initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In 
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T 
PORTD=0x00;
DDRD=0x00;

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: Timer 0 Stopped
// Mode: Normal top=FFh
// OC0 output: Disconnected
TCCR0=0x00;
TCNT0=0x00;
OCR0=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: Timer1 Stopped
// Mode: Normal top=FFFFh
// OC1A output: Discon.
// OC1B output: Discon.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer1 Overflow Interrupt: On
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
TCCR1A=0x00;
TCCR1B=0x01;
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;

// Timer/Counter 2 initialization
// Clock source: System Clock
// Clock value: Timer2 Stopped
// Mode: Normal top=FFh
// OC2 output: Disconnected
ASSR=0x00;
TCCR2=0x48;
TCNT2=0x00;
OCR2=0x00;

// External Interrupt(s) initialization
// INT0: On
// INT0 Mode: Rising Edge
// I NT1: On
// INT1 Mode: Rising Edge
// INT2: On
// INT2 Mode: Rising Edge
GICR|=0xE0;
MCUCR=0x0F;
MCUCSR=0x40;
GIFR=0xE0;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=0x04;

// USART initialization
// Communication Parameters: 8 Data, 1 Stop, No Parity
// USART Receiver: On
// USART Transmitter: On
// USART Mode: Asynchronous
// USART Baud Rate: 4800
UCSRA=0x00;
UCSRB=0x18;
UCSRC=0x86;
UBRRH=0x00;
UBRRL=0x67;

// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
ACSR=0x80;
SFIOR=0x00;
 

// Global enable interrupts

FlagAntStart=0;
i=14;
j=0;
tik1=0;
tik2=0;
//Shim_ini(254); 
delay_ms(1000);
PORTB.2=1;
DDRB=0xFF; 
Ugol_Povorota=150;

#asm("sei")
// Manual_Vector(0); 
 
 
 
         
/*
PORTB=0x00;
DDRB=0xFF;
delay_ms(1000);
PORTB=0xFF;
DDRB=0xFF;
*/
//Random_Vector(0x30); 

while (1)
   {       
      
      //  Manual_Vector(0);
      //    Shim_ini1(10000);    
                                                                                                                                                                                          
      /*
      data= USART_Receive();
        if(data == '&')
        { 
           // Grad2 = USART_Receive();  
            delay_us(10); 
            FlagAntStart=0;
            Grad1[0] = USART_Receive(); 
            Grad1[1] = USART_Receive(); 
            
            if(Grad1[1]=='%'){Grad1[3]=Grad1[0];Grad1[0]='0';Grad1[1]='0';Grad1[2]='0';  }
            else { Grad1[2] = USART_Receive();}
                       
            if(Grad1[2]=='%'){Grad1[3]=Grad1[1];Grad1[2]=Grad1[0];Grad1[0]='0';Grad1[1]='0';}
            if(Grad1[3]=='%'){Grad1[3]=Grad1[2];Grad1[2]=Grad1[1];Grad1[1]=Grad1[0];Grad1[0]='0';}
           
            Ugol_Povorota=atof(Grad1); 
            Polojenie_AntStart=Polojenie_Ant; 
       }
       */
 //    Manual_Vector(1);
 
      // Random_Vector(0x31); 
      // delay_ms(100); 
 

      
  

  /*
  if ((Polojenie_AntStart-Polojenie_AntStop)<=1)  Manual_Vector(0);
      else 
  {
      if (Polojenie_Ant>Polojenie_AntStop )Manual_Vector(0x31);
      else (Manual_Vector(0x32));  
  }
  delay_ms(100);
 
 */
// Stat1=USART_Receive();
//USART_Transmit(Stat1); 

//USART_Transmit(Stat2);




   

  // TIMSK |= (0 << OCIE1A);

    
   /* while(j<=89)
    {
    j=j+1;
    buffer1[j]=USART_Receive();
        }
    j=0;  
    while(j<=89)
    {
    j=j+1;
    USART_Transmit(buffer1[j]);
    }
     j=0;   
    while(j<=89)
    {
    j=j+1;
    if(buffer1[j]==0x25){Ur3=buffer1[j+1];Ur2=buffer1[j+2];Ur1=buffer1[j+3];}
    }  
    j=0;
    */ 
   //delay_ms(10);
   
   
    //-------------------------------------------------------------------
    //Generate Shim
    //-------------------------------------------------------------------
   // TCCR2 |= (1 << WGM20) | ( 1 << WGM21) | (1 << COM21) | (1 << CS22);
   // OCR2=200; 
  //  TCCR2 |= (0 << WGM20) | ( 0 << WGM21) | (0 << COM21) | (0 << CS22);
    //-------------------------------------------------------------------  
    //-------------------------------------------------------------------
    //Temperature & Vlajnost
    //-------------------------------------------------------------------- 
 
    
    //----------------------------------------------------------------------
    
    //-----------------------------------------------------------------------
    //Generate Request Lir  
    //----------------------------------------------------------------------
    
   // LIR();
  
     //----------------------------------------------------------------------- 
    
}
}
