
#ifndef INCLUDE_AR2_DATA_H_
#define INCLUDE_AR2_DATA_H_

#include <stdint.h>
#include "ar2_modbus_rtu.h"

typedef struct
{
    uint16_t netin_bits;
    uint16_t din_bits;

    uint16_t timer1_value;
    uint16_t counter1_value;
    uint16_t timer_a1_preset;

    uint32_t successful_refresh_count;
    uint32_t failed_refresh_count;

    ar2_modbus_status_t last_comm_status;
    uint8_t communication_ok;
} ar2_data_t;

void AR2_Data_Init(uint8_t slave_id, uint32_t timeout_ms);
ar2_modbus_status_t AR2_Data_Refresh(void);

extern ar2_data_t g_ar2_data;

#endif /* INCLUDE_AR2_DATA_H_ */
