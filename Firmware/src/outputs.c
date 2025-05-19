#include "outputs.h"

void outputs_Init(void)
{
    GPIO_Config_OUTPUT(_PUMP_Config, _PUMP_Pin);
    GPIO_Config_OUTPUT(_Motor_Config, _Motor_Pin);
    GPIO_Config_OUTPUT(_Speed_Config, _Speed_Pin);
};

void Outputs_Control(PUMP_T _pumpStatus, MOTOR_T _motorStatus)
{
    switch (_pumpStatus)
    {
        case PUMP_OFF:
            PUMP_CTR_OFF;
        break;

        case PUMP_ON:
            PUMP_CTR_ON;
        break;
    };  

    switch (_motorStatus)
    {
        case MOTOR_OFF:
            MOTOR_CTR_OFF;
            SPEED_CTR_LOW;
        break;

        case MOTOR_LOW:
            MOTOR_CTR_ON;
            SPEED_CTR_LOW;
        break;
        
        case MOTOR_HIGH:
            MOTOR_CTR_ON;
            SPEED_CTR_HIGH;
        break;        
    };
};
