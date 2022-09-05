#ifndef   _DISP_H_
#define   _DISP_H_

#include "menu.h"
 #include "stm8l15x_conf.h"

extern unsigned int N_RPM;
extern bool LangFont;
extern int InvertStr;
extern char previous_o;	// чтобы не было частого стирания экрана(моргания)
extern unsigned int Impulse_1s;
extern bool VisibleEngineHours [8];
extern int adc_val;	// результат АЦП - измеренное значение в попугаях
extern float Koeff_engine;	// коэффициент для формулы тахометра, зависящий от формулы двигателя (искра/оборот)
extern char display_str[5];

extern void menuChange(menuItem* NewMenu);

void dispMainView(void);





#endif