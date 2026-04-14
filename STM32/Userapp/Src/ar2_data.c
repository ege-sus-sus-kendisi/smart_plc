#include "ar2_data.h"
#include "ar2_modbus_map.h"

static ar2_modbus_slave_context_t g_ar2_slave_ctx;
ar2_data_t g_ar2_data;

/* Currently set to
 * 		slave_id = 1
 * 		timeout_ms = 200
 */
void AR2_Data_Init(uint8_t slave_id, uint32_t timeout_ms)
{
    MODBUS_SlaveInit(&g_ar2_slave_ctx, slave_id, timeout_ms);

    g_ar2_data.netin_bits = 0U;
    g_ar2_data.din_bits = 0U;
    g_ar2_data.timer1_value = 0U;
    g_ar2_data.counter1_value = 0U;
    g_ar2_data.timer_a1_preset = 0U;

    g_ar2_data.successful_refresh_count = 0U;
    g_ar2_data.failed_refresh_count = 0U;

    g_ar2_data.last_comm_status = AR2_MB_OK;
    g_ar2_data.communication_ok = 0U;
}

ar2_modbus_status_t AR2_Data_Refresh(void)
{
    ar2_modbus_status_t status;

    /*
     * Declared bitfield and value as separate variables for semantic clarity, even though they both serve
     * the same purpose as variables to hold temporary data.
     */
    uint16_t bitfield;
    uint16_t value;

    status = ReadCoils(&g_ar2_slave_ctx, AR2_MB_COIL_NETIN(1), 16U, &bitfield);
    if (status != AR2_MB_OK)
    {
        g_ar2_data.last_comm_status = status;
        g_ar2_data.failed_refresh_count++;
        g_ar2_data.communication_ok = 0U;
        return status;
    }
    g_ar2_data.netin_bits = bitfield;

    status = ReadDiscreteInputs(&g_ar2_slave_ctx, AR2_MB_DI_DIN(1), 16U, &bitfield);
    if (status != AR2_MB_OK)
    {
        g_ar2_data.last_comm_status = status;
        g_ar2_data.failed_refresh_count++;
        g_ar2_data.communication_ok = 0U;
        return status;
    }
    g_ar2_data.din_bits = bitfield;

    status = ReadInputRegister(&g_ar2_slave_ctx, AR2_MB_IR_TMR(1), &value);
    if (status != AR2_MB_OK)
    {
        g_ar2_data.last_comm_status = status;
        g_ar2_data.failed_refresh_count++;
        g_ar2_data.communication_ok = 0U;
        return status;
    }
    g_ar2_data.timer1_value = value;

    status = ReadInputRegister(&g_ar2_slave_ctx, AR2_MB_IR_CNT(1), &value);
    if (status != AR2_MB_OK)
    {
        g_ar2_data.last_comm_status = status;
        g_ar2_data.failed_refresh_count++;
        g_ar2_data.communication_ok = 0U;
        return status;
    }
    g_ar2_data.counter1_value = value;

    status = ReadHoldingRegister(&g_ar2_slave_ctx, AR2_MB_HR_TMR_A(1), &value);
    if (status != AR2_MB_OK)
    {
        g_ar2_data.last_comm_status = status;
        g_ar2_data.failed_refresh_count++;
        g_ar2_data.communication_ok = 0U;
        return status;
    }
    g_ar2_data.timer_a1_preset = value;

    g_ar2_data.last_comm_status = AR2_MB_OK;
    g_ar2_data.successful_refresh_count++;
    g_ar2_data.communication_ok = 1U;

    return AR2_MB_OK;
}
