#ifndef UI_STATEMACHINE_H_
#define UI_STATEMACHINE_H_

#include "stdint.h"
#include "settings.h"

#define UI_SYMBOLS_NUMBER 11 //+1 for blank
#define UI_SYMBOLS_BLANK 10 //place of blank in LUT

#define UI_MODE_AMOUNT SETTINGS_MODE_AMOUNT
#define UI_MODIFIER_AMOUNT SETTINGS_MODIFIER_AMOUNT
#define UI_PRESET_AMOUNT SETTINGS_PRESET_AMOUNT

#define UI_SLA_AMOUNT SETTINGS_SLA_AMOUNT
#define UI_SLA_OFFSET 0 //for en/disable all SLA
#define UI_SLA_ALL_ENABLE 0

#define UI_MODE_MAX_VALUE (UI_MODE_AMOUNT-1)
#define UI_MODIFIER_MAX_VALUE (UI_MODE_MAX_VALUE+UI_MODIFIER_AMOUNT)
#define UI_PRESET_MAX_VALUE (UI_MODIFIER_MAX_VALUE+UI_PRESET_AMOUNT)

#define UI_MODIFIER_INDEX (uiUIMode-UI_MODE_MAX_VALUE)
#define UI_PRESET_INDEX (uiUIMode-UI_MODIFIER_MAX_VALUE)

#define UI_MODIFIER_UNCHANGED 0x2
#define UI_MODIFIER_ACTIVATED 0x1
#define UI_MODIFIER_DEACTIVATED 0x0

#define UI_STATE_STANDBY 0x0
#define UI_STATE_SLA_SELECTION 0x1
#define UI_STATE_MODE_SELECTION 0x2
#define UI_STATE_PARAMETER_SELECTION 0x3
#define UI_STATE_PARAMETER_SELECTED 0x4
#define UI_STATE_SETTINGS_SELECTION 0x5
#define UI_STATE_SETTINGS_SELECTED 0x6

#define UI_STATE_TYPE_MODE 0x0
#define UI_STATE_TYPE_MODIFIER 0x1
#define UI_STATE_TYPE_PRESET 0x2

#define UI_MODE_NO_SELECTION 255

uint8_t uiUIState;

uint8_t uiUISLA;
uint8_t uiUIMode;
uint8_t uiUISetting;
uint16_t uiUISettingValue;
uint8_t uiUIParameter;
uint16_t uiUIParameterValue;

uint8_t uiUISLAActive[SETTINGS_SLA_AMOUNT+UI_SLA_OFFSET];
uint8_t uiUIModeActive[SETTINGS_MODIFIER_AMOUNT+1];

uint8_t uiUIStandbyPreviousState;


void vUIInit(void);
void vUIReset(void);
void vUIStandby(void);
void vUIWakeUp(void);

void vUIUpdate(void);

uint8_t uiUIModeTypeGet(void);
//void vUIParameterSet(void);
void vUISLASelectionSwitch(void);
void vUIModeParameterStandardSet(void);
void vUIModifierParameterStandardSet(void);
void vUIModifierSelectionSwitch(void);
void vUIModifierActivate(void);
void vUIModifierDeactivate(void);
void vUIPresetLoad(void);
uint16_t uiUIParameterValueGet(void);
void vUIParameterValueSet(void);
uint16_t uiUISettingValueGet(void);
void vUISettingValueSet(void);

#endif /* UI_STATEMACHINE_H_ */
