#include <C8051F120.h>                 // SFR declarations

#define SYSCLK             24500000   // SYSCLK in Hz (24.5 MHz internal
                                       // oscillator )
                                       // the internal oscillator has a
                                       // tolerance of +/- 2%

/*#define TIMER_PRESCALER            48  // Based on Timer CKCON settings

#define LED_TOGGLE_RATE            50  // LED toggle rate in milliseconds
                                       // if LED_TOGGLE_RATE = 1, the LED will
                                       // be on for 1 millisecond and off for
                                       // 1 millisecond

#define TIMER_TICKS_PER_MS  SYSCLK/TIMER_PRESCALER/1000

#define AUX1     TIMER_TICKS_PER_MS*LED_TOGGLE_RATE
#define AUX2     -AUX1
#define AUX3     AUX2&0x00FF
#define AUX4     ((AUX2&0xFF00)>>8)

#define TIMER0_RELOAD_HIGH       AUX4  // Reload value for Timer0 high byte
#define TIMER0_RELOAD_LOW        AUX3  // Reload value for Timer0 low byte		*/

sbit LED0 = P1^0;                          // LED0='1' means ON
sbit LED1 = P1^1;                          // LED1='1' means ON
sbit LED2 = P1^2;                          // LED2='1' means ON

void SYSCLK_Init (void);
void Port_Init (void);                 // Port initialization routine
void Timer0_Init (void);               // Timer0 initialization routine
int count1=0;
int count2=0;
int count3=0;

//-----------------------------------------------------------------------------
void main (void)
{
   WDTCN = 0xDE;                       // Disable watchdog timer
   WDTCN = 0xAD;
   SYSCLK_Init ();                     // Initialize system clock to 24.5MHz
   Timer0_Init ();                     // Initialize the Timer0
   Port_Init ();                       // Init Ports
   EA = 1;                             // Enable global interrupts
	SYSCLK_Init ();
   while (1) ;
		                          
}
void SYSCLK_Init (void)
{
   OSCICN = 0x83;                      // Configure internal oscillator for
                                       // its highest frequency (24.5 MHz)

   RSTSRC = 0x04;                      // Enable missing clock detector
}
//-----------------------------------------------------------------------------
void Port_Init (void)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page
   SFRPAGE = CONFIG_PAGE;              // Set SFR page

   XBR2 = 0xC4;                        // Enable crossbar
   P1MDOUT = 0x40;                     // Set LED to push-pull

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page
}

//-----------------------------------------------------------------------------
void Timer0_Init(void)
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page
   SFRPAGE = TIMER01_PAGE;             // Set SFR page

  // TH0 = 0x63;           // Init Timer0 High register
   //TL0 = 0xE2 ;           // Init Timer0 Low register
   TMOD = 0x11;                        // Timer0 in 16-bit mode
   CKCON = 0x19;                       // Timer0 uses a 1:48 prescaler
   ET0 = 1;                            // Timer0 interrupt enabled
   TCON = 0x10;                        // Timer0 ON

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page
}


//-----------------------------------------------------------------------------
void Timer0_ISR (void) interrupt 1
{
   char SFRPAGE_SAVE = SFRPAGE;        // Save Current SFR page
   SFRPAGE = TIMER01_PAGE; 

   TH0 = 0xFE;           // Reinit Timer0 High register
   TL0 = 0x1B;            // Reinit Timer0 Low register
   	count1=count1+1; 
   if (count1==3) {count1=0;  LED0 = ~LED0; }
 
	count2=count2+1;
   if (count2==5) {count2=2;  LED1 = ~LED1; }	
  
	count3=count3+1; 
   if (count3==7) {count3=4;  LED2 = ~LED2; } 	 

   SFRPAGE = SFRPAGE_SAVE;             // Restore SFR page
}	