
 #include "stm8l15x_conf.h"

// #include "stm8l15x.h"
 #include "main.h"
// #include "init.h"
 //#include "disp.h" 
// #include "hx1230.h"
 //#include "delay_stm8l.h"
 //#include "menu.h"
 
 //SSD1306
  #include "IIC.h"
 #include "SSD1306.h"
 #include "Fonts.h"
 
 
 
//#include "u8g2.h"
 
//u8g2_t u8g2; // a structure which will contain all the data for one display

 
//#include "LIB_Config.h"

//#define NULL (void*)0
//#define SSD1306_CMD    0
//#define SSD1306_DAT    1
// #define IIC_SCL_PIN         GPIO_Pin_1
// #define IIC_SDA_PIN         GPIO_Pin_0

// #define IIC_SCL_GPIO        GPIOC
// #define IIC_SDA_GPIO        GPIOC

// #define __IIC_SCL_SET()     GPIO_WriteBit(IIC_SCL_GPIO, IIC_SCL_PIN, SET)
// #define __IIC_SCL_CLR()     GPIO_WriteBit(IIC_SCL_GPIO, IIC_SCL_PIN, RESET)

// #define __IIC_SDA_SET()		GPIO_WriteBit(IIC_SDA_GPIO, IIC_SDA_PIN, SET)
// #define __IIC_SDA_CLR()     GPIO_WriteBit(IIC_SDA_GPIO, IIC_SDA_PIN, RESET)

// #define __IIC_SDA_IN()     	GPIO_Init(IIC_SDA_GPIO, IIC_SDA_PIN, GPIO_Mode_In_PU_No_IT);								

// #define __IIC_SDA_OUT() 	GPIO_Init(IIC_SDA_GPIO, IIC_SDA_PIN, GPIO_Mode_Out_PP_High_Fast);

// #define __IIC_SDA_READ()    GPIO_ReadInputDataBit(IIC_SDA_GPIO, IIC_SDA_PIN)

void (*pf)(void); // указактель на функцию   http://www.amse.ru/courses/cpp2/2011_04_11.html и https://metanit.com/cpp/c/5.11.php

// буфер для ДМА чтобы складыввать сюда данные АЦП
/* #define ADC1_DR_ADDRESS        ADC1_BASE+4 	//адрес регистра ADC1 data register high имеет смещение в +4 относительно Peripherals Base Address смотри в stm8l15x.h ((uint16_t)0x5344)
#define BUFFER_SIZE            ((uint8_t) 0x0B)
uint16_t Buffer[BUFFER_SIZE]  =  {0,0,0,0,0,0,0,0,0,0,0};
#define BUFFER_ADDRESS         ((uint16_t)(&Buffer))
 */
 
 //  при делителе 9,1к/1,1к коэфф делителя 8.27
// реально при 25В -> 2,7В ,т.е. реальный коэфф делителя 9,26, т.к. резисторы делителя +-5%
//при делителе 1/1 -> ADC_RATIO = ( 3.3 * 1000 * 1000)/4095
// при делителе 9,26 ->   ADC_RATIO = ( 3.3 * 9,26 * 1000 * 1000)/4095 = ( 30,5 *1000* 1000)/4095 = 7448
//#define ADC_RATIO    ((uint16_t) 806) 
#define ADC_RATIO    ((uint16_t) 7448) 
 	

//  при делителе 91к/9,1к коэфф делителя 10,0, т.е. при 24В будет 2,4В
// реально при 25В -> 2.24В ,т.е. реальный коэфф делителя 11.16, т.к. резисторы делителя +-5%
// при делителе 11,16 ->   ADC_RATIO_ACCUM = ( 3.3 * 11.16 * 1000 * 1000)/4095 = 8993
#define ADC_RATIO_ACCUM    ((uint16_t) 8993)
 
 #define ADC1_DR_ADDRESS        ((uint16_t)0x5344)
#define BUFFER_SIZE            ((uint8_t) 0x08)
#define BUFFER_ADDRESS         ((uint16_t)(&Buffer))
#define ASCII_NUM_0            ((uint8_t)   48)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint16_t Buffer[BUFFER_SIZE]  = {0, 0, 0,0, 0,0,0,0};


uint16_t temp;


//#define ASCII_NUM_0            ((uint8_t)   48)
#define NULL (void*)0




FunctionalState DataADCStatus = DISABLE;


//-------------- переменные для отображения LCD -------------------------
bool LangFont;	// флаг вывода текста на кирилице или на английском
char display_str[5];	// временная переменная для возврата значений из фукнкций IntToStr() и CharToStr()
char page = 0;	//  номер страницы отображения текущего уровня меню (с 0 до...)
char previous_page = 0;	//  номер предыдущей страницы отображения текущего уровня меню (с 0 до...)
unsigned char Count_BL_LCD = 0;	//  счетчик  времени подсветки LCD
unsigned char Count_TA_LCD = 0;	//  счетчик  времени активности LCD
int NumStrLen =0 ;
int NumStrLenCount = 0;	//	счетчик шагов бег строки
int NumStrLenRazn = 0; 
char NumItemMenuVar = 0; // количество пунктов в текущем меню
unsigned char CursorPos = 0; // номер строки на которой стоит курсор
unsigned char CursorPos_pre =0; // предыдущее положение курсора
int InvertStr = 0; // признак инвертирования строки курсора
int InvertVar = 0; // признак инвертирования-редактирования переменной курсора

char NumString = 0;	// номер текущей выводимой на экран строки
 unsigned char canalADC; //  канал АЦП

//---------------переменные АЦП --------------------------------------------------------------
//	0	24В ACDC, 
//	1	24В СТОП1, 
//	2	24В системное, 
//	3	-15V системное,
//	4	+15V системное, 
//	5	5V системное, 
//	6	+24V Аккумул,  
//	7	24В СТОП2, 
//	8	24V ЗУ Аккумулятора, 
//	9	токового шунта 12, 
//	10	токового шунта 11
int VarADC [11] = {0,0,0,0,0,0,0,0,0,0,0};
int PreVarACD [11] = {0,0,0,0,0,0,0,0,0,0,0};

int TimeCountACC;	// переменная-имитатор емкости аккумулятора
int temp_time = 0;		// отладочная переменная - индикация времени


Switcher SW_2kV = 	{off,off,	0, NULL, "Выс.напряж."};	// переключатель вкл-выкл напряжения 2кВ
Switcher SW_start = {off,off,	0, NULL, NULL};	// переключатель ПУСК

SVD SVD_red = 		{low,low,	0, NULL, " ТЕСТ В РАБОТЕ "};	// светодиод красный	
SVD SVD_green = 	{low,low,	0, NULL, " ТЕСТ ЗАВЕРШЕН "};	// светодиод зеленый
SVD SVD_yellow = 	{low,low,	0, NULL, "МПК-24"};	//светодиод желтого

ElRele ElRele_Accum = 	{off,0, 0,0, NULL, " Аккум ", NULL, NULL};	//электронный ключ (электронное реле) аккумулятора
ElRele ElRele_ACDC = 	{off,0, 0,0, NULL, " Сеть ", NULL, NULL};	//электронный ключ (электронное реле) ACDC
ElRele ElRele_ZUAccum = {off,0, 0,0, NULL, NULL, NULL, NULL};	//электронный ключ (электронное реле) ЗУ акуумулятора
ElRele ElRele_2kV = 	{off,0, 0,0, NULL, "Выс.напряж.", NULL, NULL};	//электронный ключ (электронное реле) преобразователя 2кВ

ErrorDevice ErrorMPK = {FALSE, no_error, NULL, NULL};	// глобальная структура хранения и фиксации ошибок работы МПК24

struct StatusDev StatusDevice = {FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE, FALSE};	// глоб структура статуса устроства

//--------------- флаги ---------------------------------------------------------





	

//bool Flag_SW2kV = FALSE;
//bool Flag_SWStart = FALSE;
//bool Flag_RedSvetoDiod = FALSE;
//bool Flag_GreenSvetoDiod = FALSE;
//bool Flag_YellowSvetoDiod = FALSE;



bool StatusPowerLCD;	// флаг статуса питания LCD  TRUE=питание есть, FALSE= питания нет

bool FlagEcoMode= FALSE;	// флаг нахождения в Eco - режиме
//------------------ прочие переменные --------------------------------
uint16_t adc_val = 0;	// результат АЦП - измеренное значение в попугаях
unsigned char Tick250ms = 0;	//  счетчик четвертьсекундых тиков
char previous_o;	// чтобы не было частого стирания экрана(моргания)
char levelNumCount;
char CountTimer4, CountTimer4_1;	//  счетчик и его прошлое, который непрерывно увеличивает таймер 4 через интервалы TIM4_PERIOD_delay100ms	


 int count;
 uint16_t  p;
 char display_str[5];	// временная переменная для возврата значений из фукнкций IntToStr() и CharToStr()

//Директива location указывает компилятору, где разместить переменную, а __no_init запрещает обнулять её при старте.	
#pragma location=FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS //Наш адрес в EEPROM (в данном случае - начало еепромки)
__no_init unsigned char TimeCountACC_EEPROM_L;	// переменная-имитатор емкости аккумулятора -  в EEPROM младший байт

#pragma location=FLASH_DATA_EEPROM_START_PHYSICAL_ADDRESS+1 //Наш адрес в EEPROM (в данном случае - начало еепромки)
__no_init unsigned char TimeCountACC_EEPROM_H;	// переменная-имитатор емкости аккумулятора -  в EEPROM младший байт
 
 
#include "init.c"
#include "disp.c"




 //----------------------------------------------------------------------------
 void delay_100ms(void)
 {
	 
	//TIM4_Cmd(DISABLE);       // stop
	//TIM4_TimeBaseInit(TIMER4_Prescaler, TIM4_PERIOD_delay100ms);
    //TIM4_ClearFlag(TIM4_FLAG_Update);
    //TIM4_ITConfig(TIM4_IT_Update, ENABLE);
	//FlagTimer4Bisy=TRUE;	// поднимем флаг занятости таймера 4
	CountTimer4=0;		// обнулим переменую таймера
	//TIM4_Cmd(ENABLE);       // пуск таймера
	//CountTimer4_1=CountTimer4;
	//GPIO_ToggleBits(PORTC, Debug1);
//	while (CountTimer4 != 3){}// ждем пока таймер отсчитает 300 мс
	//GPIO_ToggleBits(PORTC, Debug1);
	//while (FlagTimer4Bisy){};// ждем пока таймер отсчитает 100 мс
	//TIM4_Cmd(DISABLE);       // остановим таймер
	//FlagTimer4Bisy=FALSE; //опустим флаг занятости таймера 4
	 
 }
  //---------------------------------------------------------------------------------- 
// void delay_us(int us)
// {
        // TIM4_Cmd(DISABLE);       // stop

        // TIM4_TimeBaseInit(TIM4_Prescaler_4, 4);	// 2MHz/4 = 2 us	2us*2=4 us
        // TIM4_ClearFlag(TIM4_FLAG_Update);
        // TIM4_ITConfig(TIM4_IT_Update, ENABLE);

        // count = us>>1;

        // TIM4_Cmd(ENABLE);       // let's go

        // while(count);
// }  
  void delay_us(int us)
{
        // for (int i=0; i<us/2; i++)
		// {
			// nop();
		// }

        
} 
  
  
  //----------------------------------------------------------------------------------
void delay_ms(int ms)
{
        TIM4_Cmd(DISABLE);       // stop

        TIM4_TimeBaseInit(TIM4_Prescaler_16, TIM4_PERIOD);
        TIM4_ClearFlag(TIM4_FLAG_Update);
        TIM4_ITConfig(TIM4_IT_Update, ENABLE);

        count = ms;

        TIM4_Cmd(ENABLE);       // let's go

        while(count);
}
//----------------------------------------------------------------------------------




//----------------------------------------------------------------------------------


void ReadADC1(void)	// чтение всех напряжений изи заполение масива VarADC
{
	// запустим цикл АЦП и считаем напряжение
	ADC_SoftwareStartConv(ADC1);
	VarADC [0] = adc_val;
	
}



//----------------------------------------------------------------------------------
// сравнение значений АЦП с Хорошими/плохими и установка соответствующих флагов
//----------------------------------------------------------------------------------
void VerifyVoltage (void)
{
	// если три входных питающих напряжения (Аккум, ACDC и ЗУ) находятя в рамках допусков - установим соответтвующие флаги
	if ((VarADC[0] < VminACDC)||(VarADC[0] > VmaxACDC))	{ StatusDevice.TrueVotageACDC = FALSE;} else { StatusDevice.TrueVotageACDC = TRUE;} 
	if ((VarADC[6] < VminACC)||(VarADC[6] > VmaxACC))	{StatusDevice.TrueVotageACC = FALSE;} else {StatusDevice.TrueVotageACC = TRUE;} 
	if ((VarADC[8] < VminCharge)||(VarADC[8] > VmaxCharge))	{StatusDevice.TrueVotageCharge = FALSE;} else {StatusDevice.TrueVotageCharge = TRUE;} 

	//не забудем про питающие напряжения КРОСС
	if ((VarADC[2] < VminSys24DC)||(VarADC[2] > VmaxSys24DC))	{ StatusDevice.TrueVotageSys24DC = FALSE;} else { StatusDevice.TrueVotageSys24DC = TRUE;}
	if ((VarADC[4] < VminSys15DC)||(VarADC[4] > VmaxSys15DC))	{ StatusDevice.TrueVotageSys15DC = FALSE;} else { StatusDevice.TrueVotageSys15DC = TRUE;}
	if ((VarADC[5] < VminSys5DC)||(VarADC[5] > VmaxSys5DC))	{ StatusDevice.TrueVotageSys5DC = FALSE;} else { StatusDevice.TrueVotageSys5DC = TRUE;}
	
//	0	24В ACDC, 
//	1	24В СТОП1, 
//	2	24В системное, 
//	3	-15V системное,
//	4	+15V системное, 
//	5	5V системное, 
//	6	+24V Аккумул,  
//	7	24В СТОП2, 
//	8	24V ЗУ Аккумулятора, 
//	9	токового шунта 12, 
//	10	токового шунта 11
//int VarADC [11] = {0,0,0,0,0,0,0,0,0,0,0};
}


//----------------------------------------------------------------------------------
// функция включения электронного реле питания модуля 2кВ
//----------------------------------------------------------------------------------
void SwitchPower2kV (void)
{
	// электронное реле включается не только если включен перключатель на передней панели, но и программа Сефелека начала работать - косвенный признак - красный светодиод
	if ((SW_2kV.status) && (SVD_red.status == high))	{StatusDevice.Power2kV = TRUE;} else {StatusDevice.Power2kV = FALSE;}
	if (StatusDevice.Power2kV==TRUE)	{GPIO_SetBits (PORTB, ON_DCDC2kV_H);  } else {GPIO_ResetBits(PORTB, ON_DCDC2kV_H );}	 
}

//----------------------------------------------------------------------------------
//-------------- считываем уровни светодиодов --------------------------------------
//----------------------------------------------------------------------------------
void ScanSvetoDiode(void)
{
	if (GPIO_ReadInputDataBit (PORTB, LED_Red))		{ SVD_red.status = high; } else { SVD_red.status = low; }
	if (GPIO_ReadInputDataBit (PORTB,  LED_Green ))		{ SVD_green.status = high; } else { SVD_green.status = low; }
	if (GPIO_ReadInputDataBit (PORTB, LED_PW))		{ SVD_yellow.status = high; } else { SVD_yellow.status = low; }
	
	//проверка на ошибки светодиодов
	
	//если почему то горят и красный и зеленый
	if ((SVD_green.status == high) && (SVD_red.status == high)) {ErrorMPK.status = TRUE; ErrorMPK.code_error = ErrorRedAndGrSVD;}
	
	//если при горении красного или зеленого не горит жетлый
	if (((SVD_green.status == high) || (SVD_red.status == high)) && (SVD_yellow.status == low)) {ErrorMPK.status = TRUE; ErrorMPK.code_error = ErrorYellowSVD;}
	
	// если уровни светодиодов изменились относительно предыдущего - выставим флаг оновления экрана и 
	//запомним текущий уровень как прдыдущий на будующие разборки
	if (SVD_red.status != SVD_red.pre_status) {StatusDevice.RefreshLCD = TRUE;SVD_red.pre_status = SVD_red.status;}
	if (SVD_green.status != SVD_green.pre_status) {StatusDevice.RefreshLCD = TRUE;SVD_green.pre_status = SVD_green.status;}
	if (SVD_yellow.status != SVD_yellow.pre_status) {StatusDevice.RefreshLCD = TRUE;SVD_yellow.pre_status = SVD_yellow.status;}
}


//----------------------------------------------------------------------------------
//-------------- считываем положение переключателей --------------------------------------
//----------------------------------------------------------------------------------
void ScanSwitch(void)
{
	if (GPIO_ReadInputDataBit (PORTB, Sw_dcdc_2kV))		{  SW_2kV.status = off; } else {  SW_2kV.status = on;}
	if (GPIO_ReadInputDataBit (PORTE, Start_L))		{  SW_start.status = off;} else {  SW_start.status = on;}
	
	// если положение переключателя изменились относительно предыдущего - выставим флаг оновления экрана и 
	//запомним текущее положение как прдыдущее на будующие разборки
	if (SW_2kV.status != SW_2kV.pre_status) {StatusDevice.RefreshLCD = TRUE; SW_2kV.pre_status = SW_2kV.status;}
	if (SW_start.status != SW_start.pre_status) {StatusDevice.RefreshLCD = TRUE; SW_start.pre_status = SW_start.status;}
}

//---------------------------------------------------------------------------------------------------------------------------------
//-------------- выключаем все электронные ключи в последовательности - 2кВ, ЗУ, АСДС, Аккум --------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
void Off_All_ElRele(void)
{
	GPIO_ResetBits(PORTB, ON_DCDC2kV_H);
	GPIO_ResetBits(PORTB, ON_Charge_H);
	GPIO_ResetBits(PORTB, ON_ACDC_H);
	GPIO_ResetBits(PORTB, ON_ACCUM_H);
}
//---------------------------------------------------------------------------------------------------------------------------------
//-------------- вывод на экран пиктограммы уровня зарадя батарейки соответвующей напряжению  --------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
void DisplayPickBatt  (void)
{
	char SymBat01;
	char SymBat02;
	
	SymBat01 = 0x80;
	SymBat02 = 0x83;
	
	//отобразим символ батарейки в соответствии с напряжением изменеренным АЦП
	//если емкость 30% или менее
	if (TimeCountACC <= CapacityBat30per) {SymBat01 = 0x82;SymBat02 = 0x83;}
	//если емкость от 30 до 60%
	if ((TimeCountACC > CapacityBat30per)&&(TimeCountACC <= CapacityBat60per)) {SymBat01 = 0x80;SymBat02 = 0x83;}
	// если емкость свыше 60 % и до 100%
	if (TimeCountACC > CapacityBat60per) {SymBat01 = 0x80;SymBat02 = 0x81;}

	ssd1306_display_charRUS(127-(8*2), 0, SymBat01, 8,1);  
	ssd1306_display_charRUS(127-(8*1), 0, SymBat02, 8,1);
	
}
//---------------------------------------------------------------------------------------------------------------------------------
//-------------- вывод на экран режимов работы соответвующих "виртуальным" светодиодам  --------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
void DisplayModeWork (void)
{
	//if (Flag_YellowSvetoDiod) {ssd1306_display_stringRUS(40, 17, "МПК-24", 8, 1);}
	if (SVD_yellow.status == high) {ssd1306_display_stringRUS(40, 17, "МПК-24", 8, 1);} else {ssd1306_display_stringRUS(40, 17, "_____", 8, 1);}
	
	//ssd1306_display_string(26,24, " TEST OFF ", 16, 0);
	if (SVD_green.status == high) 	{ssd1306_display_stringRUS(0, 30, " ТЕСТ ЗАВЕРШЕН ", 8, 0);} else {ssd1306_display_stringRUS(0, 30, "_______________", 8, 1);}
	
	if (SVD_red.status == high) { ssd1306_display_stringRUS(0, 30, " ТЕСТ В РАБОТЕ ", 8, 0); }   else {ssd1306_display_stringRUS(0, 30, "_______________", 8, 1);}
	
	
}

//---------------------------------------------------------------------------------------------------------------------------------
//-------------- вывод на экран режимов работы питания  --------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
// 
void DisplayModePower (void)
{
	//ssd1306_display_string(90, 0, "Accum", 12, 0);
	if (StatusDevice.WorkFromACDC)  	{ssd1306_display_stringRUS(0, 0, " Сеть  ", 8, 0);}
	if (StatusDevice.WorkFromACC) 		{ssd1306_display_stringRUS(0, 0, " Аккум ", 8, 0);}
	if ((!StatusDevice.WorkFromACDC) && (!StatusDevice.WorkFromACC)) {ssd1306_display_stringRUS(0, 0, "       ", 8, 0);}
	
	//если почему выставлен и тот флаг и этот - ошибка
	if ((StatusDevice.WorkFromACDC) && (StatusDevice.WorkFromACC)) {ErrorMPK.status = TRUE; ErrorMPK.code_error = ErrorModeWork;}
	
}
//---------------------------------------------------------------------------------------------------------------------------------
//-------------- вывод на экран режима работы проеобразователя 2кВ  --------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
void DisplayMode2kV (void)
{
	//ssd1306_display_string(0, 52, "High voltage", 12, 1);
	ssd1306_display_stringRUS(0, 63-8, "Выс.напряж.", 8, 1);
	
	//ssd1306_display_string(90, 52, " ON ", 12, 0);
	//if (SW_2kV.status)   {ssd1306_display_stringRUS(127-(8*4)-1, 63-8, " ВКЛ.", 8, 0);} else {ssd1306_display_stringRUS(127-(8*4)-2, 63-8, "ВЫКЛ.", 8, 0);}
	if (StatusDevice.Power2kV)   {ssd1306_display_stringRUS(127-(8*4)-1, 63-8, " ВКЛ.", 8, 0);} else {ssd1306_display_stringRUS(127-(8*4)-2, 63-8, "ВЫКЛ.", 8, 0);}
}

//----------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
//--------------  функция старта на аккумуляторе  --------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------

bool FuncStartDevOnAccum (void)
{
	bool error;
	
	// проверим не включена ли сейчас зарядка аккумулятора от ЗУ
	// если включена - обойдем всю функцию стороной
	
	if (!StatusDevice.WorkCharge)
	{
		// если мы здесь - значит зарядка не включена
		// -проверка напряжения на аккумуляторе (напряжения до ключа аккумулятора) и проверка н евкючена ли сейчас зарядка
		if ((VarADC[6] < VminACC)||(VarADC[6] > VmaxACC))	
		{
			//если напряжение выходит за границы низа и верха
			StatusDevice.TrueVotageACC = FALSE;
			error = TRUE;
			StatusDevice.WorkFromACC = FALSE;
		} 
		else 
		{
			// -если напряжение аккумулятора в поле допуска
			StatusDevice.TrueVotageACC = TRUE;
			// - запуск функции ВКЛючения электроного ключа Аккумулятора
			if (!StatusDevice.WorkFromACC)	{GPIO_ResetBits(PORTB, ON_ACDC_H); GPIO_SetBits (PORTB, ON_ACCUM_H);  }
			// -установка флага работы от акуум
			StatusDevice.WorkFromACC = TRUE;
			StatusDevice.WorkFromACDC = FALSE;
			error = FALSE;
		} 
	}
	return error;
	
}
//----------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
//--------------  функция старта на ACDC  --------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
bool FuncStartDevOnACDC (void)
{
	bool error;
	
	// -проверка напряжения на ACDC (напряжения до ключа ACDC)
	if ((VarADC[0] < VminACDC)||(VarADC[0] > VmaxACDC))	
	{
		//если напряжение выходит за границы низа и верха
		StatusDevice.TrueVotageACDC = FALSE;
		error = TRUE;
		StatusDevice.WorkFromACDC = FALSE;
	} 
	else 
	{
		StatusDevice.TrueVotageACDC = TRUE;
		// -если напряжение ACDC в поле допуска - запуск функции ВКЛючения электроного ключа ACDC
		if (!StatusDevice.WorkFromACDC)	{GPIO_ResetBits(PORTB, ON_ACCUM_H); GPIO_SetBits (PORTB, ON_ACDC_H);  }
		// -установка флага работы от ACDC
		StatusDevice.WorkFromACDC = TRUE;
		StatusDevice.WorkFromACC = FALSE;
		error = FALSE;
	} 
	return error;

}	
//----------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
//--------------  функция одиночных запусков АЦП для всех напряжений  --------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
void ReadVoltage(void)
{
//	0	24В ACDC, 
//	1	24В СТОП1, 
//	2	24В системное, 
//	3	-15V системное,
//	4	+15V системное, 
//	5	5V системное, 
//	6	+24V Аккумул,  
//	7	24В СТОП2, 
//	8	24V ЗУ Аккумулятора, 
//	9	токового шунта 12, 
//	10	токового шунта 1

	canalADC = 0;
	PreVarACD [0] = VarADC [0];
	DataADCStatus = DISABLE;
	ADC_Cmd(ADC1, ENABLE);
	ADC_DMACmd(ADC1, DISABLE); // отключаем DMA
	ADC_SchmittTriggerConfig(ADC1,  ADC_Channel_0, DISABLE);
	ADC_ChannelCmd  (ADC1, ADC_Channel_0, ENABLE);
	//ADC_Cmd(ADC1, ENABLE);
	ADC_SoftwareStartConv(ADC1);
	//while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == 0)  {}
	while(DataADCStatus == DISABLE) {}
	adc_val=ADC_GetConversionValue(ADC1);
	VarADC [0] = (uint32_t)((uint32_t)adc_val * (uint32_t)ADC_RATIO) / 10000;
	if (PreVarACD [0] != VarADC [0]) {StatusDevice.RefreshLCD = TRUE;}
	ADC_Cmd(ADC1, DISABLE);
	
	/*  DataADCStatus = DISABLE;
	//ADC_Cmd(ADC1, DISABLE);
	ADC_ChannelCmd  (ADC1, ADC_Channel_0, DISABLE);
	ADC_ChannelCmd  (ADC1, ADC_Channel_1, ENABLE);
	//ADC_Cmd(ADC1, ENABLE);
	ADC_SoftwareStartConv(ADC1);
	if (DataADCStatus == DISABLE) {}
	PreVarACD [1] = VarADC [1];
	VarADC [1] =	(uint32_t)((uint32_t)adc_val * (uint32_t)ADC_RATIO) / 10000;
	if (PreVarACD [1] != VarADC [1]) {StatusDevice.RefreshLCD = TRUE;}
	
	DataADCStatus = DISABLE;
	//ADC_Cmd(ADC1, DISABLE);
	ADC_ChannelCmd  (ADC1, ADC_Channel_1, DISABLE);
	ADC_ChannelCmd  (ADC1, ADC_Channel_2, ENABLE);
	//ADC_Cmd(ADC1, ENABLE);
	ADC_SoftwareStartConv(ADC1);
	if (DataADCStatus == DISABLE) {}
	PreVarACD [2] = VarADC [2];
	VarADC [2] =	(uint32_t)((uint32_t)adc_val * (uint32_t)ADC_RATIO) / 10000;
	if (PreVarACD [2] != VarADC [2]) {StatusDevice.RefreshLCD = TRUE;}
	
	DataADCStatus = DISABLE;
	//ADC_Cmd(ADC1, DISABLE);
	ADC_ChannelCmd  (ADC1, ADC_Channel_2, DISABLE);
	ADC_ChannelCmd  (ADC1, ADC_Channel_3, ENABLE);
	//ADC_Cmd(ADC1, ENABLE);
	ADC_SoftwareStartConv(ADC1);
	if (DataADCStatus == DISABLE) {}
	PreVarACD [3] = VarADC [3];
	VarADC [3] =	(uint32_t)((uint32_t)adc_val * (uint32_t)ADC_RATIO) / 10000;
	if (PreVarACD [3] != VarADC [3]) {StatusDevice.RefreshLCD = TRUE;}
	
	DataADCStatus = DISABLE;
	//ADC_Cmd(ADC1, DISABLE);
	ADC_ChannelCmd  (ADC1, ADC_Channel_3, DISABLE);
	ADC_ChannelCmd  (ADC1, ADC_Channel_4, ENABLE);
	//ADC_Cmd(ADC1, ENABLE);
	ADC_SoftwareStartConv(ADC1);
	if (DataADCStatus == DISABLE) {}
	PreVarACD [4] = VarADC [4];
	VarADC [4] =	(uint32_t)((uint32_t)adc_val * (uint32_t)ADC_RATIO) / 10000;
	if (PreVarACD [4] != VarADC [4]) {StatusDevice.RefreshLCD = TRUE;}
	
	DataADCStatus = DISABLE;
	//ADC_Cmd(ADC1, DISABLE);
	ADC_ChannelCmd  (ADC1, ADC_Channel_4, DISABLE);
	ADC_ChannelCmd  (ADC1, ADC_Channel_5, ENABLE);
	//ADC_Cmd(ADC1, ENABLE);
	ADC_SoftwareStartConv(ADC1);
	if (DataADCStatus == DISABLE) {}
	PreVarACD [5] = VarADC [5];
	VarADC [5] =	(uint32_t)((uint32_t)adc_val * (uint32_t)ADC_RATIO) / 10000;
	if (PreVarACD [5] != VarADC [5]) {StatusDevice.RefreshLCD = TRUE;}  */
	
	canalADC = 6;
	PreVarACD [6] = VarADC [6];
	DataADCStatus = DISABLE;
	ADC_Cmd(ADC1, ENABLE);
	ADC_DMACmd(ADC1, DISABLE); // отключаем DMA
	ADC_SchmittTriggerConfig(ADC1,  ADC_Channel_6, DISABLE);
	ADC_ChannelCmd  (ADC1, ADC_Channel_0, DISABLE);
	ADC_ChannelCmd  (ADC1, ADC_Channel_6, ENABLE);
	//ADC_Cmd(ADC1, ENABLE);
	ADC_SoftwareStartConv(ADC1);
	//while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == 0)  {}
	while(DataADCStatus == DISABLE) {}
	//if (DataADCStatus == DISABLE) {}	// ждем выхода из прерыания
	adc_val=ADC_GetConversionValue(ADC1);
	VarADC [6] = (uint32_t)((uint32_t)adc_val * (uint32_t)ADC_RATIO_ACCUM) / 10000;
	if (PreVarACD [6] != VarADC [6]) {StatusDevice.RefreshLCD = TRUE;}
	ADC_Cmd(ADC1, DISABLE);
	
	/* DataADCStatus = DISABLE;
	//ADC_Cmd(ADC1, DISABLE);
	ADC_ChannelCmd  (ADC1, ADC_Channel_6, DISABLE);
	ADC_ChannelCmd  (ADC1, ADC_Channel_7, ENABLE);
	//ADC_Cmd(ADC1, ENABLE);
	ADC_SoftwareStartConv(ADC1);
	if (DataADCStatus == DISABLE) {}
	PreVarACD [7] = VarADC [7];
	VarADC [7] =	(uint32_t)((uint32_t)adc_val * (uint32_t)ADC_RATIO) / 10000;
	if (PreVarACD [7] != VarADC [7]) {StatusDevice.RefreshLCD = TRUE;}
	ADC_ChannelCmd  (ADC1, ADC_Channel_7, DISABLE); */
	
	/* canalADC = 8;
	DataADCStatus = DISABLE;
	//ADC_Cmd(ADC1, DISABLE);
	ADC_ChannelCmd  (ADC1, ADC_Channel_6, DISABLE);
	ADC_ChannelCmd  (ADC1, ADC_Channel_8, ENABLE);
	//ADC_Cmd(ADC1, ENABLE);
	ADC_SoftwareStartConv(ADC1);
	while(DataADCStatus == DISABLE) {} */
	// PreVarACD [7] = VarADC [7];
	// VarADC [7] =	(uint32_t)((uint32_t)adc_val * (uint32_t)ADC_RATIO) / 10000;
	// if (PreVarACD [7] != VarADC [7]) {StatusDevice.RefreshLCD = TRUE;}
	// ADC_ChannelCmd  (ADC1, ADC_Channel_7, DISABLE);
	
	ADC_Cmd(ADC1, DISABLE);
	ADC_ChannelCmd  (ADC1, ADC_Channel_0, DISABLE);
	ADC_ChannelCmd  (ADC1, ADC_Channel_1, DISABLE);
	ADC_ChannelCmd  (ADC1, ADC_Channel_6, DISABLE);
	ADC_ChannelCmd  (ADC1, ADC_Channel_8, DISABLE);
}

/* uint8_t u8g2_gpio_and_delay_stm8(u8x8_t * u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr) 
{
  // Re-use library for delays
  //if (u8x8_avr_delay(u8x8, msg, arg_int, arg_ptr))
   // return 1;

  switch (msg) {
    // called once during init phase of u8g2/u8x8
    // can be used to setup pins
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
      CS_DDR |= _BV(CS_BIT);
      DC_DDR |= _BV(DC_BIT);
      RESET_DDR |= _BV(RESET_BIT);
      break;
    
	// CS (chip select) pin: Output level in arg_int
    case U8X8_MSG_GPIO_CS:
      if (arg_int)
        CS_PORT |= _BV(CS_BIT);
      else
        CS_PORT &= ~_BV(CS_BIT);
      break;
    
	// DC (data/cmd, A0, register select) pin: Output level in arg_int
    case U8X8_MSG_GPIO_DC:
      if (arg_int)
        DC_PORT |= _BV(DC_BIT);
      else
        DC_PORT &= ~_BV(DC_BIT);
      break;
    
	// Reset pin: Output level in arg_int
    case U8X8_MSG_GPIO_RESET:
      if (arg_int)
        RESET_PORT |= _BV(RESET_BIT);
      else
        RESET_PORT &= ~_BV(RESET_BIT);
      break;
    
	default:
      u8x8_SetGPIOResult(u8x8, 1);
      break;
  }
  return 1;
} */


/* uint8_t u8g2_gpio_and_delay_stm8(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  switch(msg)
  {
    case U8X8_MSG_BYTE_SEND:
      Wire.write((uint8_t *)arg_ptr, (int)arg_int);
      break;
    case U8X8_MSG_BYTE_INIT:
      Wire.begin();
      break;
    case U8X8_MSG_BYTE_SET_DC:
      break;
    case U8X8_MSG_BYTE_START_TRANSFER:
      if ( u8x8->display_info->i2c_bus_clock_100kHz >= 4 )
      {
	Wire.setClock(400000L); 
      }
      Wire.beginTransmission(u8x8_GetI2CAddress(u8x8)>>1);
      break;
    case U8X8_MSG_BYTE_END_TRANSFER:
      Wire.endTransmission();
      break;
    default:
      return 0;
  }
  return 1;
} */

/* uint8_t u8g2_gpio_and_delay_stm8(U8X8_UNUSED u8x8_t *u8x8, U8X8_UNUSED uint8_t msg, U8X8_UNUSED uint8_t arg_int, U8X8_UNUSED void *arg_ptr)
{
	switch(msg)
	{
		//Initialize SPI peripheral
		case U8X8_MSG_GPIO_AND_DELAY_INIT:
			// HAL initialization contains all what we need so we can skip this part. 

		break;

		//Function which implements a delay, arg_int contains the amount of ms
		case U8X8_MSG_DELAY_MILLI:
			for (uint16_t n = 0; n < 3200; n++)
			{
				//__NOP();
				temp++;
			}
			break;
		
		//Function which delays 10us
		case U8X8_MSG_DELAY_10MICRO:
			for (uint16_t n = 0; n < 320; n++)
			{
				//__NOP();
				temp++;
			}
			break;
		//Function which delays 100ns
		case U8X8_MSG_DELAY_100NANO:
			// __NOP();
			temp++;
			break;
		
		//Function to define the logic level of the clockline
		case U8X8_MSG_GPIO_SPI_CLOCK:
			// if (arg_int) HAL_GPIO_WritePin(SCK_GPIO_Port, SCK_Pin, RESET);
			// else HAL_GPIO_WritePin(SCK_GPIO_Port, SCK_Pin, SET);
			if (arg_int) {__IIC_SCL_CLR();}
			else {__IIC_SCL_SET();};
			break;

			//Function to define the logic level of the data line to the display
		case U8X8_MSG_GPIO_SPI_DATA:
			// if (arg_int) HAL_GPIO_WritePin(MOSI_GPIO_Port, MOSI_Pin, SET);
			// else HAL_GPIO_WritePin(MOSI_GPIO_Port, MOSI_Pin, RESET);
			if (arg_int) {__IIC_SDA_SET();}
			else {__IIC_SDA_CLR();};
			break;

		// Function to define the logic level of the CS line
		case U8X8_MSG_GPIO_CS:
			// if (arg_int) HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, RESET);
			// else HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, SET);
			break;
		
		//Function to define the logic level of the Data/ Command line
		case U8X8_MSG_GPIO_DC:
//			if (arg_int) HAL_GPIO_WritePin(CD_LCD_PORT, CD_LCD_PIN, SET);
//			else HAL_GPIO_WritePin(CD_LCD_PORT, CD_LCD_PIN, RESET);
			break;
		//Function to define the logic level of the RESET line
		case U8X8_MSG_GPIO_RESET:
			// if (arg_int) HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, SET);
			// else HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, RESET);
			break;

		default:
			return 0; //A message was received which is not implemented, return 0 to indicate an error
	}

	return 1; // command processed successfully.
} */

/* uint8_t u8x8_byte_sw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  switch (msg)
  {
  case U8X8_MSG_BYTE_SEND:
    //HAL_SPI_Transmit(&hspi1, (uint8_t *) arg_ptr, arg_int, 10000);
    break;
  case U8X8_MSG_BYTE_INIT:
    break;
  case U8X8_MSG_BYTE_SET_DC:
    //HAL_GPIO_WritePin(OLED_DC_GPIO_Port, OLED_DC_Pin, arg_int);
    break;
  case U8X8_MSG_BYTE_START_TRANSFER:
    break;
  case U8X8_MSG_BYTE_END_TRANSFER:
    break;
  default:
    return 0;
  }
  return 1;
} */

//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------
void main(void)
{
	//char i;

	
	GpioInit();
	ClkInit();
	//ExtiInit ();
	PvdInit ();
	
	//I2C_Init(I2C1, 100000, SLAVE_ADDRESS, I2C_Mode_I2C, I2C_DutyCycle_2, I2C_Ack_Enable, I2C_AcknowledgedAddress_10bit);
	
	AdcInit ();
	//Timer4Init();
	//Timer2Init();
	InitRTC();	
	//InitTIM1();
	// настройка считывания показаний АЦП через DMA. АЦП стартует от пинка Таймера 1
	//http://we.easyelectronics.ru/STM8/acp-v-stm8l-i-vse-chto-s-nim-svyazano.html
	
	//  или https://d1.amobbs.com/bbs_upload782111/files_40/ourdev_642969D6Z1YG.pdf
	
	/* ADC configuration -------------------------------------------*/
	//123ADC_Config();
	/* DMA configuration -------------------------------------------*/
	//123DMA_Config();
    /* Enable ADC1 DMA requests*/
	//123ADC_DMACmd(ADC1, ENABLE);
	/* Start ADC1 Conversion using TIM1 TRGO*/
	//123ADC_ExternalTrigConfig(ADC1, ADC_ExtEventSelection_Trigger2, ADC_ExtTRGSensitivity_Rising);
	//ADC_ExternalTrigConfig(ADC1, ADC_ExtEventSelection_Trigger1, ADC_ExtTRGSensitivity_Rising);
	/* Master Mode selection: Update event */
	//123TIM1_SelectOutputTrigger(TIM1_TRGOSource_Update);
	/* Enable TIM1 */
	//123TIM1_Cmd(ENABLE);

	/* RTC wake-up event every 500 ms (timer_step x (1023 + 1) )*/
	RTC_SetWakeUpCounter(2047);
	RTC_WakeUpCmd(ENABLE);
	
	enableInterrupts();
	
	
	/* //поробуем подключить бибилотеку u8g2
	//u8g2_Setup_sh1106_i2c_128x64_noname_2(u8g2, rotation, u8x8_byte_sw_i2c, uC specific) [page buffer, size = 256 bytes]
	u8g2_Setup_ssd1306_i2c_128x64_noname_2(&u8g2, U8G2_R0, u8x8_byte_sw_i2c, u8g2_gpio_and_delay_stm8);  // init u8g2 structure
	u8g2_InitDisplay(&u8g2); // send init sequence to the display, display is in sleep mode after this,
	u8g2_SetPowerSave(&u8g2, 0); // wake up display  */
	
	
	// операции с OLED 1.3 // дисплей OLED 1.3 128x64 pixel
	iic_init();
    ssd1306_init();
	
	ssd1306_clear_screen(0xFF);
	//delay_ms(100);
	ssd1306_clear_screen(0x00);
	StatusDevice.RefreshLCD = FALSE;
	
	RestoreVarFromEEPROM();
	
	while (1)
	{
		/* u8g2_ClearBuffer(&u8g2);
		u8g2_SetFont(&u8g2, u8g2_font_ncenB14_tr);
		u8g2_DrawStr(&u8g2, 0, 15, "Hello!");
		u8g2_SendBuffer(&u8g2); */

#ifdef WORK
		ScanSvetoDiode();//считываем уровни светодиодов
		ScanSwitch();// считываем положение переключателей
		// если положение переключателя Пуск во ВКЛ то работаем дальше
		if (SW_start.status)
		{
			//GPIO_SetBits(PORTB, ON_ACDC_H);	// принудительно для отладки включим АСДС
			//123if (DataADCStatus != DISABLE)
			//123{
				ReadVoltage();
				
				// сравнение значений АЦП с Хорошими/плохими и установка соответствующих флагов
				VerifyVoltage ();
				
				// -если старт (флаг старта) не произошел, то запускаем функции старта
				if (!StatusDevice.Start)
				{
					//  если признак обновления экрана стоит - обновим полностью экран
					if (StatusDevice.RefreshLCD) 
					{
						//ssd1306_clear_screen(0x00);
						//вывод на экран пиктограммы уровня зарадя батарейки соответвующей напряжению
						DisplayPickBatt ();
						//вывод на экран режимов работы соответвующих "виртуальным" светодиодам - ТЕСТ ЗАВЕРШЕН, ТЕСТ В РАБОТЕ, МПК-24
						DisplayModeWork ();
						// вывод на экран режимов работы питания - СЕТЬ иил АККУМ
						DisplayModePower ();
						// вывод на экран режима работы проеобразователя 2кВ - ВЫСОКОЕ НАПРЯЖЕНИЕ ВКЛ/ВЫКЛ
						DisplayMode2kV ();
						
						//debug
						ssd1306_display_num(0, 12, (uint32_t) VarADC[0] , 5, 12);				//	0	24В ACDC,
						ssd1306_display_num(0, 36, (uint32_t) VarADC[6] , 5, 12);              //	6	+24V Аккумул, 
						//ssd1306_display_num(32, 36, (uint32_t) temp_time , 5, 12);              //	time
						ssd1306_display_num(8*8, 0, (uint32_t) TimeCountACC , 5, 12);              //	time
						
						
						ssd1306_refresh_gram();
						StatusDevice.RefreshLCD = FALSE;// обновили экран - не забыть снять флаг обновления
					}
					
					
					// -если есть напряжение на аккумуляторе в поле допуска - зпускаем функция старта на аккумуляторе:
					if (StatusDevice.TrueVotageACC)  {FuncStartDevOnAccum ();}
					
					// -если есть напряжение на АСДС в поле допуска - запускаем функция старта на АСДС:
					if (StatusDevice.TrueVotageACDC)  {FuncStartDevOnACDC ();}
					
					// если напряжения аккумулятора и АСДС не в полях допуска
					if ((!StatusDevice.TrueVotageACDC)&&(!StatusDevice.TrueVotageACC) )
					{
						// выключаем все электронные ключи в последовательности - 2кВ, ЗУ, АСДС, Аккум
						Off_All_ElRele();
						StatusDevice.WorkFromACC = FALSE; StatusDevice.WorkFromACDC = FALSE; StatusDevice.WorkCharge =FALSE;
					}
					
					// если включена работа от АСДС и напряжение на клеммах ЗУ в допуске - аккумулятор надо 
					//переключить для подзаряда на ЗУ (т.е. режим работы от аккумулятора -выключить
					// а включить электронный ключ ЗУ)
					//if ((StatusDevice.WorkFromACDC) && (StatusDevice.TrueVotageCharge))
					if (StatusDevice.WorkFromACDC) 
					{
						GPIO_ResetBits(PORTB, ON_ACCUM_H); 
						StatusDevice.WorkFromACC = FALSE;
						GPIO_SetBits (PORTB, ON_Charge_H);  
						// -установка флага работы от ЗУ
						StatusDevice.WorkCharge = TRUE;
					}
					
					SwitchPower2kV (); // функция включения электронного реле питания модуля 2кВ
					 	
				}
				
				
				
				
			//123	DataADCStatus = DISABLE;
			//123}
		}
		// если положение переключателя Пуск во ВЫКЛ
		else
		{
			// выключаем все электронные ключи в последовательности - 2кВ, ЗУ, АСДС, Аккум
			Off_All_ElRele();	
			// -сбрасываем флаг старта и коды ошибок, флаги работы от аккум или асдс
			StatusDevice.Start = FALSE; StatusDevice.WorkFromACC = FALSE; StatusDevice.WorkFromACDC = FALSE;
			ErrorMPK.status = FALSE; ErrorMPK.code_error = no_error;
			
			ssd1306_clear_screen(0x00);
		}		
		
#endif
		
		
#ifdef DEBUG
		ScanSvetoDiode();//считываем уровни светодиодов
		ScanSwitch();// считываем положение переключателей
		if (DataADCStatus != DISABLE)
		{
			
			// проверка работоспособности флагов
			//StatusDevice.TrueVotageACC = TRUE;	// флаг плохого/хорошего уровня напряжения аккумулятора
			//StatusDevice.TrueVotageACDC = TRUE;	// флаг плохого/хорошего уровня напряжения ACDC
			//StatusDevice.TrueVotageCharge = TRUE;	// флаг плохого/хорошего уровня напряжения ЗУ Аккумулятора
			//StatusDevice.WorkFromACC = TRUE;	//флаг индикации управляющего сигнала работы устройства от Аккумулятора
			//StatusDevice.WorkFromACDC = TRUE;	//флаг индикации управляющего сигнала работы устройства от ACDC
			//StatusDevice.WorkCharge = TRUE;	//флаг индикации управляющего сигнала работы ЗУ Аккумулятора
			//Flag_Power2kV = TRUE;
			//Flag_TrueVotageSys5DC = TRUE;	
			//Flag_TrueVotageSys15DC = TRUE;	
			//Flag_TrueVotageSys24DC = TRUE;	
			//SW_2kV.status = on;
			//SW_start.status = on;
			//SVD_red.status = high;
			//SVD_green.status = high;
			//SVD_yellow.status = high;
			// функция включения электронного реле питания модуля 2кВ
			SwitchPower2kV ();
			VerifyVoltage ();
			// вывод информаци на экран
			ssd1306_clear_screen(0x00);
			ssd1306_display_string(0, 0, "Technical info", 12, 1);
			ssd1306_display_num(0, 12, (uint32_t) VarADC[0] , 5, 12);				//	0	24В ACDC, 
			ssd1306_display_num(0, 24, (uint32_t) VarADC[1] , 5, 12);               //	1	24В СТОП1, 
			ssd1306_display_num(0, 36, (uint32_t) VarADC[2] , 5, 12);               //	2	24В системное, 
			ssd1306_display_num(0, 48, (uint32_t) VarADC[3] , 5, 12);               //	3	-15V системное,
			ssd1306_display_num(32, 12, (uint32_t) VarADC[4] , 5, 12);              //	4	+15V системное, 
			ssd1306_display_num(32, 24, (uint32_t) VarADC[5] , 5, 12);              //	5	5V системное, 
			ssd1306_display_num(32, 36, (uint32_t) VarADC[6] , 5, 12);              //	6	+24V Аккумул,  
			ssd1306_display_num(32, 48, (uint32_t) VarADC[7] , 5, 12);              //	7	24В СТОП2, 
			ssd1306_display_num(64, 12, (uint32_t) VarADC[8] , 5, 12);              //	8	24V ЗУ Аккумулятора,
			//ssd1306_display_num(64, 12, (uint32_t) VarADC[8] , 5, 12); 			//	9	токового шунта 12,
			//ssd1306_display_num(64, 12, (uint32_t) VarADC[8] , 5, 12);            //	10	токового шунта 11
			if (SVD_yellow.status) {ssd1306_display_string(64, 24, "Y", 12, 1);}     
			if (SVD_green.status) {ssd1306_display_string(64, 36, "G", 12, 1);}     
			if (SVD_red.status) {ssd1306_display_string(64, 36, "R", 12, 1);}
			if (StatusDevice.WorkFromACC) {ssd1306_display_string(64, 48, "ACC", 12, 1);}	//флаг индикации управляющего сигнала работы устройства от Аккумулятора
			if (StatusDevice.TrueVotageACDC)  {ssd1306_display_string(64, 48, "ACDC", 12, 1);}	//флаг индикации управляющего сигнала работы устройства от ACDC
			if (StatusDevice.WorkCharge)  {ssd1306_display_string(96, 12, "ZY", 12, 1);}	//флаг индикации управляющего сигнала работы ЗУ Аккумулятора
			if (SW_2kV.status)   {ssd1306_display_string(96, 24, "SW2kV", 12, 1);}
			if (SW_start.status)  {ssd1306_display_string(96, 36, "SWSTR", 12, 1);}
			ssd1306_refresh_gram();

			
			
			
			DataADCStatus = DISABLE;
		}
		
#endif

		
		
		


	}; // while (1)



}