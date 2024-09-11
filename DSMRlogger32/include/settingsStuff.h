#ifndef SETTINGSSTUFF_H
#define SETTINGSSTUFF_H

//============ Includes ====================
#include "arduinoGlue.h"

DECLARE_TIMER_EXTERN(publishMQTTtimer);
DECLARE_TIMER_EXTERN(reconnectMQTTtimer);
DECLARE_TIMER_EXTERN(nextTelegram);
DECLARE_TIMER_EXTERN(oledSleepTimer);

void showDevSettings();      

//============ Added by Convertor ==========
#endif // SETTINGSSTUFF_H
