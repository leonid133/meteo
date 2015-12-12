#include "c8051f120.h"
#include "init.h"


//------------------------------------------------------------------------------------------
void Port_IO_Init(void)
{
    // P0.0  -  TX0 (UART0), 
    // P0.1  -  RX0 (UART0), 
    // P0.2  -  SDA (SMBus), Open-Drain, Digital
    // P0.3  -  SCL (SMBus), Open-Drain, Digital
    // P0.4  -  TX1 (UART1), Open-Drain, Digital
    // P0.5  -  RX1 (UART1), Open-Drain, Digital
    // P0.6  -  T0 (Timer0), Open-Drain, Digital
    // P0.7  -  скорость ветра INT0 (Tmr0), Push-Pull,  Digital

    // P1.0  -  T1 (Timer1), Push-Pull,  Digital
    // P1.1  -  направление ветра INT1 (Tmr1), Push-Pull,  Digital
    // P1.2  -  T2 (Timer2), Push-Pull,  Digital
    // P1.3  -  T4 (Timer4), Push-Pull,  Digital
    // P1.4  -  SYSCLK,      Push-Pull,  Digital
	 //P1.5 ШИМ 
	 //P1.6 LED
	 //P1.7 ШИМ 
	 //P2.0 ЛИР CLOCK
	 //P2.1 ЛИР DATA

    SFRPAGE   = CONFIG_PAGE;
    P0MDOUT   = 0x83;
    P1MDOUT   = 0xFF;
    P2MDOUT   = 0xFF;
    XBR0      = 0x05;
    XBR1      = 0xBE;
    XBR2      = 0x4C;

}
//------------------------------------------------------------------------------------------
void Oscillator_Init(void)
{
    SFRPAGE   = CONFIG_PAGE;
    OSCICN    = 0x83;
}
//------------------------------------------------------------------------------------------
void Interrupts_Init(void)
{
    IE        = 0xBA;
    EIE1      = 0x02;
    EIE2      = 0x45;
    //IP        = 0x02;//timer0 hight
	 //PX1=1;
	 EIP2      = 0x01;
}
//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Timer_01(void)
{    
	 SFRPAGE   = TIMER01_PAGE;
    TCON      = 0x50;//0x55
    TMOD      = 0x11;
    CKCON     = 0x08;//0x0A;
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

//------------------------------------------------------------------------------------------
void Timer_3(void)
{
    SFRPAGE   = TMR3_PAGE;
    TMR3CN    = 0x04;
    TMR3CF    = 0x02;
   /* RCAP3L    = 0x38;
    RCAP3H    = 0x9f;*/
}
//------------------------------------------------------------------------------------------
void Timer_4(void)
{
    SFRPAGE   = TMR4_PAGE;
    TMR4CN    = 0x04;
    TMR4CF    = 0x02;
  /*  RCAP4L    = 0x06;
    RCAP4H    = 0xc6;*/
}
//------------------------------------------------------------------------------------------

void Init_Device(void)
{
    Port_IO_Init();
    Oscillator_Init();
    Interrupts_Init();
	 Timer_01();
	 UART0_Init();
	 Timer_3();
	 //Timer_4();
}
//------------------------------------------------------------------------------------------

//#endif
