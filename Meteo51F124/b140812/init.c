#include "c8051f120.h"
#include "init.h"

//------------------------------------------------------------------------------------------
void Reset_Sources_Init()
{
    WDTCN     = 0xDE;
    WDTCN     = 0xAD;
}

void Timer_Init()
{

//---------------------------------------
    SFRPAGE   = TIMER01_PAGE;
    TCON      = 0x50;
    TMOD      = 0x22;
    TL0       = 0x4B;
    TH0       = 0xA0;
	TH1 = 0x2B;		
	TL1 = 0x11;

//GPS, скорость 4800 бод, тактируется Timer1 
	SFRPAGE = UART1_PAGE;
	SCON1 = 0x10;       	
	
	SFRPAGE =0x00;
	//CKCON = CKCON | 0x02;	
	/*регистр управления тактированием
									CKCON.3 = T0M = 1:	Timer 0 тактируется системным тактовым сигналом 
									(т.е. биты CKCON.1 и CKCON.0 не влияют)
									CKCON.1 = SCA1 = 1:	Биты выбора делителя частоты тактирования Timer 0, 1 
									CKCON.0 = SCA0 = 0: 	SYSCLK/48*/
	
	//TMOD = TMOD | 0x20;		
	/*Регистр режима Таймеров 0 и 1
									TMOD.7 = GATE1 = 0: Таймер 1 включен, если TR1 = 1, независимо от логического уровня на входе /INT1.
									TMOD.6 = C/T1 = 0:  Т/С1  работает  как  таймер:  Таймер 1  инкрементируется  от  внутреннего  сигнала   
                 										тактирования, который задается битом T1M (CKCON.4).  
									TMOD.5 = T1M1 = 1,  	
									TMOD.4 = T1M0 = 0: 	Выбор режима работы Таймера 1: 8-разрядный таймер/счетчик с автоперезагрузкой*/



//COM, 115200, Timer2
	 SFRPAGE   = TMR2_PAGE;

	 TMR2CF = 0x08; // Timer 2 Configuration
    RCAP2L = 0xF3;  // Timer 2 Reload Register Low Byte   BaudRate = 115200
    RCAP2H = 0xFF;  // Timer 2 Reload Register High Byte

    TMR2L = 0x00;   // Timer 2 Low Byte
    TMR2H = 0x00;   // Timer 2 High Byte
    TMR2CN = 0x04;  // Timer 2 CONTROL
	 TR2 = 1;
	 SFRPAGE = UART0_PAGE;

	 SCON0 = 0x50;
	 SSTA0 = 0x05;    
//SMBus, , Timer3
    SFRPAGE   = TMR3_PAGE;
    TMR3CN    = 0x04;
    RCAP3L    = 0x9F;
    RCAP3H    = 0x38;
    SFRPAGE   = TMR4_PAGE;
    RCAP4L    = 0xC6;
    RCAP4H    = 0x06;
}

void PCA_Init()
{
    SFRPAGE   = PCA0_PAGE;
    PCA0CN    = 0x40;
    PCA0MD    = 0x01;
    PCA0CPM0  = 0x40;
    PCA0CPM1  = 0x40;
    PCA0CPH0  = 0xFF;
    PCA0CPH1  = 0xFF;

}

void SMBus_Init()
{
    SFRPAGE   = SMB0_PAGE;
    SMB0CN    = 0x49;
}

void ADC_Init()
{
    SFRPAGE   = ADC0_PAGE;
    ADC0CN    = 0x80;
}
//------------------------------------------------------------------------------------------
void Port_IO_Init(void)
{
    // P0.0  -  TX0 (UART0), Open-Drain, Digital
    // P0.1  -  RX0 (UART0), Open-Drain, Digital
    // P0.2  -  SDA (SMBus), Open-Drain, Digital
    // P0.3  -  SCL (SMBus), Open-Drain, Digital
    // P0.4  -  TX1 (UART1), Open-Drain, Digital
    // P0.5  -  RX1 (UART1), Open-Drain, Digital
    // P0.6  -  CEX0 (PCA),  Push-Pull,  Digital ШИМ
    // P0.7  -  CEX1 (PCA),  Push-Pull,  Digital ШИМ

    // P1.0  -  T0 (Timer0), Push-Pull,  Digital
    // P1.1  -  wind Speed INT0 (Tmr0), Push-Pull,  Digital
    // P1.2  -  T1 (Timer1), Push-Pull,  Digital
    // P1.3  -  wind Angle INT1 (Tmr1), Push-Pull,  Digital
    // P1.4  -  T2 (Timer2), Push-Pull,  Digital
	 //P2.0 ЛИР CLOCK
	 //P2.1 ЛИР DATA

    SFRPAGE   = CONFIG_PAGE;
    P0MDOUT   = 0xC0;
    P1MDOUT   = 0xFF;
    P2MDOUT   = 0xFF;
    XBR0      = 0x15;
    XBR1      = 0x3E;
    XBR2      = 0x44;

}
 
void Oscillator_Init(void)
{
    SFRPAGE   = CONFIG_PAGE;
    OSCICN    = 0x83;
}

void Interrupts_Init()
{
    IE        = 0xBF;
    EIE1      = 0x0A;
    EIE2      = 0x43; 

   // IP        = 0x10;
   // EIP2      = 0x40;
}

void Init_Device(void)
{
    Reset_Sources_Init();
    Timer_Init();
    PCA_Init();
    SMBus_Init();
	 ADC_Init();
    Port_IO_Init();
    Oscillator_Init();
    Interrupts_Init();
}
//-----------------------------------------------------------------
//GPS, скорость 4800 бод, тактируется Timer1 
/*
void UART1_Init(void)
{
	SFRPAGE = UART1_PAGE;
	SCON1 = 0x10;       		/* Регистр управления COM1
									SCON1.7 = S1MODE= 0: 8-ми разрядный COM-порт с изменяемой скоростью передачи данных
									SCON1.5 = MCE1  = 0: (при S1MODE = SCON1.7 = 0) логический уровень стопового бита игнорируется
									SCON1.4 = REN1 = 1: Разрешение приема 	*/
/*	
	SFRPAGE =0x00;
	CKCON = CKCON | 0x02;	/*регистр управления тактированием
									CKCON.3 = T0M = 1:	Timer 0 тактируется системным тактовым сигналом 
									(т.е. биты CKCON.1 и CKCON.0 не влияют)
									CKCON.1 = SCA1 = 1:	Биты выбора делителя частоты тактирования Timer 0, 1 
									CKCON.0 = SCA0 = 0: 	SYSCLK/48*/
/*
	TMOD = TMOD | 0x20;		/*Регистр режима Таймеров 0 и 1
									TMOD.7 = GATE1 = 0: Таймер 1 включен, если TR1 = 1, независимо от логического уровня на входе /INT1.
									TMOD.6 = C/T1 = 0:  Т/С1  работает  как  таймер:  Таймер 1  инкрементируется  от  внутреннего  сигнала   
                 										тактирования, который задается битом T1M (CKCON.4).  
									TMOD.5 = T1M1 = 1,  	
									TMOD.4 = T1M0 = 0: 	Выбор режима работы Таймера 1: 8-разрядный таймер/счетчик с автоперезагрузкой*/
/*	TH1 = 148;		
	TL1 = TH1;
	TR1 = 1;  
}*/
//57600.Timer2------------------------------------------------------
/*
void UART0_Init(void)
{
	SFRPAGE = 0x00;

	TMR2CF = 0x08; // Timer 2 Configuration
   RCAP2L = 0xF3;  // Timer 2 Reload Register Low Byte   BaudRate = 115200
   RCAP2H = 0xFF;  // Timer 2 Reload Register High Byte

   TMR2L = 0x00;   // Timer 2 Low Byte
   TMR2H = 0x00;   // Timer 2 High Byte
   TMR2CN = 0x04;  // Timer 2 CONTROL
	TR2 = 1;
	SFRPAGE = UART0_PAGE;

	SCON0 = 0x50;
	SSTA0 = 0x15;
}
*/

//------------------------------------------------------------------------------------------

//#endif
