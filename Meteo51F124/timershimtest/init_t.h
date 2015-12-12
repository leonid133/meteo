#ifndef INIT_TH
#define INIT_TH

//-------------------------------------------------------------------------------------
void Timer_Init(void);
void UART0_Init (void); //COM на Timer2
void UART1_Init (void); //GPS на Timer1
void PCA_Init(void);
void ADC_Init(void);
void Port_IO_Init(void);
void Oscillator_Init(void);
void Interrupts_Init(void);

//-------------------------------------------------------------------------------------

#endif