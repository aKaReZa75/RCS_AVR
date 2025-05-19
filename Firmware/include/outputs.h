#ifndef _outputs_H_
#define _outputs_H_

#include "aKaReZa.h"

#define _PUMP_Config  DDRB
#define _PUMP_Control PORTB
#define _PUMP_Pin     0

#define _Motor_Config  DDRB
#define _Motor_Control PORTB
#define _Motor_Pin     1

#define _Speed_Config  DDRB
#define _Speed_Control PORTB
#define _Speed_Pin     2

#define PUMP_CTR_ON  bitSet(_PUMP_Control, _PUMP_Pin)
#define PUMP_CTR_OFF bitClear(_PUMP_Control, _PUMP_Pin)

#define MOTOR_CTR_ON  bitSet(_Motor_Control, _Motor_Pin)
#define MOTOR_CTR_OFF bitClear(_Motor_Control, _Motor_Pin)

#define SPEED_CTR_LOW  bitClear(_Speed_Control, _Speed_Pin)
#define SPEED_CTR_HIGH bitSet(_Speed_Control, _Speed_Pin)

typedef enum
{
    MOTOR_OFF,
    MOTOR_LOW,
    MOTOR_HIGH
}MOTOR_T;

typedef enum
{
    PUMP_OFF,
    PUMP_ON
}PUMP_T;

void outputs_Init(void);
void Outputs_Control(PUMP_T _pumpStatus, MOTOR_T _motorStatus);

#endif