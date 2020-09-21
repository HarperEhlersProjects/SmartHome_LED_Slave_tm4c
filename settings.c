#include "settings.h"


void vSettingsInit(void)
{
    //initialise length of SLA-stripes
    tsSettings[0].uiSLALength=SETTINGS_SLA0_LENGTH;
    tsSettings[1].uiSLALength=SETTINGS_SLA1_LENGTH;
    tsSettings[2].uiSLALength=SETTINGS_SLA2_LENGTH;
    tsSettings[3].uiSLALength=SETTINGS_SLA3_LENGTH;
    tsSettings[4].uiSLALength=SETTINGS_SLA4_LENGTH;
    tsSettings[5].uiSLALength=SETTINGS_SLA5_LENGTH;
    tsSettings[6].uiSLALength=SETTINGS_SLA6_LENGTH;
    tsSettings[7].uiSLALength=SETTINGS_SLA7_LENGTH;
}


