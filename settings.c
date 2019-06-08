#include "settings.h"


uint16_t uiSettingsCounter;

void vSettingsInit(void)
{

    for(uiSettingsCounter=0;uiSettingsCounter<SETTINGS_SLA_AMOUNT;uiSettingsCounter++)
    {
        tsSettings[uiSettingsCounter].uiModeActive[0]=0;
    }

    //initialise length of SLA-stripes
    tsSettings[0].uiSLALength=SETTINGS_SLA0_LENGTH;
    tsSettings[1].uiSLALength=SETTINGS_SLA1_LENGTH;
    tsSettings[2].uiSLALength=SETTINGS_SLA2_LENGTH;
    tsSettings[3].uiSLALength=SETTINGS_SLA3_LENGTH;
    tsSettings[4].uiSLALength=SETTINGS_SLA4_LENGTH;
    tsSettings[5].uiSLALength=SETTINGS_SLA5_LENGTH;
    tsSettings[6].uiSLALength=SETTINGS_SLA6_LENGTH;
    tsSettings[7].uiSLALength=SETTINGS_SLA7_LENGTH;

    //Standard values for mode 0
    uiSettingsParameterStandard[0][0]=0;
    uiSettingsParameterStandard[0][1]=0;
    uiSettingsParameterStandard[0][2]=0;
    uiSettingsParameterStandard[0][3]=0;
    uiSettingsParameterStandard[0][4]=0;
    uiSettingsParameterStandard[0][5]=0;

    //Standard values for mode 1
    uiSettingsParameterStandard[1][0]=0;
    uiSettingsParameterStandard[1][1]=0;
    uiSettingsParameterStandard[1][2]=0;
    uiSettingsParameterStandard[1][3]=0;
    uiSettingsParameterStandard[1][4]=0;
    uiSettingsParameterStandard[1][5]=0;

    //Standard values for mode 2
    uiSettingsParameterStandard[2][0]=0;
    uiSettingsParameterStandard[2][1]=0;
    uiSettingsParameterStandard[2][2]=0;
    uiSettingsParameterStandard[2][3]=0;
    uiSettingsParameterStandard[2][4]=0;
    uiSettingsParameterStandard[2][5]=0;

    //Standard values for modifier 0
    uiSettingsParameterStandard[SETTINGS_MODE_AMOUNT+0][0]=0;
    uiSettingsParameterStandard[SETTINGS_MODE_AMOUNT+0][1]=0;
    uiSettingsParameterStandard[SETTINGS_MODE_AMOUNT+0][2]=0;
    uiSettingsParameterStandard[SETTINGS_MODE_AMOUNT+0][3]=0;
    uiSettingsParameterStandard[SETTINGS_MODE_AMOUNT+0][4]=0;
    uiSettingsParameterStandard[SETTINGS_MODE_AMOUNT+0][5]=0;
}


