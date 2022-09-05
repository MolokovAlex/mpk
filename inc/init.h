#ifndef   _INIT_H_
#define   _INIT_H_

 #include "main.h"



 extern __no_init unsigned char EngineHoursEEpromH; //// счетчик моточасов -  в EEPROM

 extern __no_init unsigned char EngineHoursEEpromL; //// счетчик моточасов -  в EEPROM

 extern __no_init unsigned char EngineMinuteEEprom; //// счетчик мотоминут -  в EEPROM

extern bool Flag_PowerUp_TA_LCD;	// флаг для включения питания LCD
extern bool Flag_PowerDown_TA_LCD;	// флаг для выключения питания LCD
extern bool StatusPowerLCD;	// флаг статуса питания LCD  TRUE=питание есть, FALSE= питания нет

extern void RestoreStrVarMenuItem (void);

extern int Timer4Init(void);
 
extern int Timer2Init(void);
  
extern int Timer3Init(void);

void PowerUpLCD(void);

void PowerDownLCD(void);
  
extern  void SystemInit(void);



#endif