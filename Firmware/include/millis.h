#ifndef _millis_H_
#define _millis_H_

#include "aKaReZa.h"

typedef struct
{
  uint32_t Previous;
  uint32_t Delta;
  uint32_t Interval;
}millis_T;

void millis_Init(void);

#endif