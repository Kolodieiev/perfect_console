#include "touch.h"

void enableTouchCap (void) {
    RCC_PB2PeriphClockCmd (RCC_PB2Periph_ADC1, ENABLE);
    RCC_ADCCLKConfig (RCC_PCLK2_Div8);

    ADC_InitTypeDef ADC_InitStructure = {
        .ADC_Mode = ADC_Mode_Independent,
        .ADC_ScanConvMode = DISABLE,
        .ADC_ContinuousConvMode = DISABLE,
        .ADC_ExternalTrigConv = ADC_ExternalTrigConv_None,
        .ADC_DataAlign = ADC_DataAlign_Right,
        .ADC_NbrOfChannel = 1,
    };

    ADC_Init (ADC1, &ADC_InitStructure);

    ADC_Cmd (ADC1, ENABLE);
    ADC_BufferCmd (ADC1, ENABLE);
    ADC_TKeyCmd (ADC1, ENABLE);
}

void disableTouchCap (void) {
    ADC_Cmd (ADC1, DISABLE);
    ADC_BufferCmd (ADC1, DISABLE);
    ADC_TKeyCmd (ADC1, DISABLE);
}

uint16_t getTouchRawValue (uint8_t chann) {
    if (!isAdc1Enabled())
        return 0;

    ADC_RegularChannelConfig (ADC1, chann, 1, ADC_SampleTime_CyclesMode7);
    TKey1->IDATAR1 = 0x20;  // Charging Time
    TKey1->RDATAR = 0x8;    // Discharging Time
    while (!ADC_GetFlagStatus (ADC1, ADC_FLAG_EOC));
    return (uint16_t)TKey1->RDATAR;
}

uint8_t isAdc1Enabled (void) {
    if ((RCC->PB2PCENR & RCC_PB2Periph_ADC1) > 0)
        return ENABLE;

    return DISABLE;
}
