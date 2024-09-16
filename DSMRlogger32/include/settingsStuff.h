#ifndef SETTINGSSTUFF_H
#define SETTINGSSTUFF_H

//============ Includes ====================
#include "arduinoGlue.h"
#include "Shield32.h"

extern Shield32 myShield;



DECLARE_TIMER_EXTERN(publishMQTTtimer);
DECLARE_TIMER_EXTERN(reconnectMQTTtimer);
DECLARE_TIMER_EXTERN(nextTelegram);
DECLARE_TIMER_EXTERN(oledSleepTimer);

void showDevSettings();   
bool isValidGpio(int newGpio);   

//============ Added by Convertor ==========
#endif // SETTINGSSTUFF_H
