#include "ar2_modbus_rtu.h"
#include "ar2_modbus_map.h"
#include "nu_uart.h"

#include <string.h>

/*
 * ============================================================
 *  HARDWARE / DRIVER ADAPTER SECTION
 * ============================================================
 *
 * Replace the bodies of these 3 functions with your actual
 * nu_uart / UART2 / RS-485 implementation.
 *
 * Expected behavior:
 *
 * 1) AR2_MB_PortTx():
 *    - Sends exactly len bytes on the RS-485 line
 *    - Returns 0 on success, nonzero on failure
 *
 * 2) AR2_MB_PortRx():
 *    - Reads a full Modbus response frame into rx_buf
 *    - Waits up to timeout_ms
 *    - Writes received length into *rx_len
 *    - Returns 0 on success, nonzero on timeout/failure
 *
 * 3) AR2_MB_PortFlushRx():
 *    - Clears any stale RX data before a new request
 *
 * Notes:
 * - If your current nu_uart already stores bytes into a buffer,
 *   this is the only place you need to adapt.
 * - Keep the Modbus logic below untouched.
 */

int MODBUS_Tx(const uint8_t *data, uint16_t len)
{
    if ((data == NULL) || (len == 0U) || (len > 255U))
    {
        return -1;
    }
    RS485_TX_ENABLE();
    SEND_PACK_UART2((uint8_t)len, data);
    RS485_RX_ENABLE();
    return 0;
}

int MODBUS_Rx(uint8_t *rx_buf,
              uint16_t rx_buf_size,
			  uint16_t *rx_len,
			  uint32_t timeout_ms)
{
    uint32_t start_tick;

    if ((rx_buf == NULL) || (rx_len == NULL))
    {
        return -1;
    }

    start_tick = osKernelGetTickCount();

    // WE CAN GET RID OF THE WHILE LOOP?
    while (uart2_frame_ready == 0U)
    {
        if ((osKernelGetTickCount() - start_tick) >= timeout_ms)
        {
            return -1;
        }

        osDelay(1);
    }

    if (buf_uart2.index == 0U)
    {
        uart2_frame_ready = 0U;
        return -1;
    }

    if (buf_uart2.index > rx_buf_size)
    {
        uart2_frame_ready = 0U;
        CLR_BUFF_UART2();
        return -1;
    }

    memcpy(rx_buf, buf_uart2.buf, buf_uart2.index);
    *rx_len = (uint16_t)buf_uart2.index;

    uart2_frame_ready = 0U;
    CLR_BUFF_UART2();

    return 0;
}

void MODBUS_CleanRx(void)
{
    uart2_frame_ready = 0U;
    CLR_BUFF_UART2();
}

/* ============================================================ */

static ar2_modbus_status_t MODBUS_Transceive(ar2_modbus_slave_context_t *slave_context,
											 const uint8_t *request,
											 uint16_t request_len,
											 uint8_t *response,
											 uint16_t response_size,
											 uint16_t *response_len)
{
    if ((slave_context == NULL) || (request == NULL) || (response == NULL) || (response_len == NULL))
    {
        return AR2_MB_ERR_PARAM;
    }

    MODBUS_CleanRx();

    if (MODBUS_Tx(request, request_len) != 0)
    {
        return AR2_MB_ERR_TX;
    }

    if (MODBUS_Rx(response, response_size, response_len, slave_context->timeout_ms) != 0)
    {
        return AR2_MB_ERR_RX_TIMEOUT;
    }

    if (*response_len < 5U)
    {
        return AR2_MB_ERR_RX_SHORT;
    }

    return AR2_MB_OK;
}

/*
 * Checks the parts of the response that are common to all read functions.

	It verifies:
	- response length
	- CRC
	- correct slave ID
	- whether the slave returned an exception
	- correct function code

	(*exception_code) parameter is the place you store a possible exception code, you don't feed an
	exception code into the function, you feed a place it can go if one occurs.
 */

/*
 * Response format
 *
 * 	[slave_id][function_code][byte_count][data][crc_lo][crc_hi]
 */


static ar2_modbus_status_t MODBUS_CheckCommonResponse(ar2_modbus_slave_context_t *slave_context,
                                                  	  uint8_t expected_fc,
													  const uint8_t *response,
													  uint16_t response_len,
													  uint8_t *exception_code)
{
    uint16_t crc_calculated;
    uint16_t crc_from_response;

    if ((slave_context == NULL) || (response == NULL) || (exception_code == NULL))
    {
        return AR2_MB_ERR_PARAM;
    }

    if (response_len < 5U)
    {
        return AR2_MB_ERR_RX_SHORT;
    }

    crc_calculated = GET_CRC16(response, (uint16_t)(response_len - 2U));
    crc_from_response = (uint16_t)response[response_len - 2U] |
               ((uint16_t)response[response_len - 1U] << 8U);

    if (crc_calculated != crc_from_response)
    {
        return AR2_MB_ERR_CRC;
    }

    if (response[0] != slave_context->slave_id)
    {
        return AR2_MB_ERR_SLAVE_ID;
    }

    // ex. If request was FC03 exception response becomes 0x83
    if ((response[1] & 0x80U) != 0U)
    {
        *exception_code = response[2];
        return AR2_MB_ERR_EXCEPTION;
    }

    if (response[1] != expected_fc)
    {
        return AR2_MB_ERR_FUNCTION;
    }

    return AR2_MB_OK;
}

// [slave_id][function_code][start_addr_hi][start_addr_lo][qty_hi][qty_lo][crc_lo][crc_hi]

static uint16_t BuildReadRequest(uint8_t slave_id,
								 uint8_t function_code,
								 uint16_t start_addr,
								 uint16_t quantity,
								 uint8_t *request)
{
    uint16_t crc;

    request[0] = slave_id;
    request[1] = function_code;
    request[2] = (uint8_t)(start_addr >> 8U);
    request[3] = (uint8_t)(start_addr & 0xFFU);
    request[4] = (uint8_t)(quantity >> 8U);
    request[5] = (uint8_t)(quantity & 0xFFU);

    crc = GET_CRC16(request, 6U);
    request[6] = (uint8_t)(crc & 0xFFU);
    request[7] = (uint8_t)(crc >> 8U);

    return 8U;
}

void MODBUS_SlaveInit(ar2_modbus_slave_context_t *slave_context, uint8_t slave_id, uint32_t timeout_ms)
{
    if (slave_context == NULL)
    {
        return;
    }

    slave_context->slave_id   = slave_id;
    slave_context->timeout_ms = timeout_ms;
}

uint16_t GET_CRC16(const uint8_t *data, uint16_t len)
{
    uint16_t crc = 0xFFFFU;
    uint16_t i;
    uint8_t j;

    for (i = 0U; i < len; i++)
    {
        crc ^= data[i];

        for (j = 0U; j < 8U; j++)
        {
            if ((crc & 0x0001U) != 0U)
            {
                crc >>= 1U;
                crc ^= 0xA001U;
            }
            else
            {
                crc >>= 1U;
            }
        }
    }

    return crc;
}

ar2_modbus_status_t ReadHoldingRegister(ar2_modbus_slave_context_t *slave_context,
                                        uint16_t reg_addr,
										uint16_t *value)
{
    uint8_t request[8];
    uint8_t response[8];
    uint16_t request_len;
    uint16_t response_len = 0U;
    uint8_t exception_code = 0U;
    ar2_modbus_status_t status;

    if ((slave_context == NULL) || (value == NULL))
    {
        return AR2_MB_ERR_PARAM;
    }

    request_len = BuildReadRequest(slave_context->slave_id,
                                      	  MODBUS_FC_READ_HOLDING_REGISTERS,
										  reg_addr,
										  AR2_MAX_HOLDING_REGS_PER_REQUEST,
										  request);

    status = MODBUS_Transceive(slave_context, request, request_len, response, sizeof(response), &response_len);
    if (status != AR2_MB_OK)
    {
        return status;
    }

    status = MODBUS_CheckCommonResponse(slave_context,
                                        MODBUS_FC_READ_HOLDING_REGISTERS,
                                        response,
                                        response_len,
                                        &exception_code);
    if (status != AR2_MB_OK)
    {
        return status;
    }

    /*
     * Normal FC03 response for 1 register:
     * [slave][0x03][byte_count=2][data_hi][data_lo][crc_lo][crc_hi]
     */
    if ((response_len != 7U) || (response[2] != 2U))
    {
        return AR2_MB_ERR_DATA;
    }

    *value = ((uint16_t)response[3] << 8U) | (uint16_t)response[4];
    return AR2_MB_OK;
}

ar2_modbus_status_t ReadInputRegister(ar2_modbus_slave_context_t *slave_context,
									  uint16_t reg_addr,
									  uint16_t *value)
{
    uint8_t req[8];
    uint8_t resp[8];
    uint16_t req_len;
    uint16_t resp_len = 0U;
    uint8_t exception_code = 0U;
    ar2_modbus_status_t status;

    if ((slave_context == NULL) || (value == NULL))
    {
        return AR2_MB_ERR_PARAM;
    }

    req_len = BuildReadRequest(slave_context->slave_id,
                                      MODBUS_FC_READ_INPUT_REGISTERS,
                                      reg_addr,
                                      AR2_MAX_INPUT_REGS_PER_REQUEST,
                                      req);

    status = MODBUS_Transceive(slave_context, req, req_len, resp, sizeof(resp), &resp_len);
    if (status != AR2_MB_OK)
    {
        return status;
    }

    status = MODBUS_CheckCommonResponse(slave_context,
                                        MODBUS_FC_READ_INPUT_REGISTERS,
                                        resp,
                                        resp_len,
                                        &exception_code);
    if (status != AR2_MB_OK)
    {
        return status;
    }

    /*
     * Normal FC04 response for 1 register:
     * [slave][0x04][byte_count=2][data_hi][data_lo][crc_lo][crc_hi]
     */
    if ((resp_len != 7U) || (resp[2] != 2U))
    {
        return AR2_MB_ERR_DATA;
    }

    *value = ((uint16_t)resp[3] << 8U) | (uint16_t)resp[4];
    return AR2_MB_OK;
}

ar2_modbus_status_t ReadCoils(ar2_modbus_slave_context_t *slave_context,
                              uint16_t start_addr,
							  uint16_t quantity,
							  uint16_t *bitfield)
{
    uint8_t req[8];
    uint8_t resp[7];
    uint16_t req_len;
    uint16_t resp_len = 0U;
    uint8_t exception_code = 0U;
    ar2_modbus_status_t status;

    if ((slave_context == NULL) || (bitfield == NULL))
    {
        return AR2_MB_ERR_PARAM;
    }

    if ((quantity == 0U) || (quantity > AR2_MAX_COILS_PER_REQUEST))
    {
        return AR2_MB_ERR_PARAM;
    }

    req_len = BuildReadRequest(slave_context->slave_id,
							   MODBUS_FC_READ_COILS,
							   start_addr,
							   quantity,
							   req);

    status = MODBUS_Transceive(slave_context, req, req_len, resp, sizeof(resp), &resp_len);
    if (status != AR2_MB_OK)
    {
        return status;
    }

    status = MODBUS_CheckCommonResponse(slave_context,
                                        MODBUS_FC_READ_COILS,
                                        resp,
                                        resp_len,
                                        &exception_code);
    if (status != AR2_MB_OK)
    {
        return status;
    }

    /*
     * For up to 16 bits:
     * [slave][fc][byte_count][data0][data1?][crc_lo][crc_hi]
     */
    if ((resp[2] == 0U) || (resp[2] > 2U))
    {
        return AR2_MB_ERR_DATA;
    }

    if ((resp_len != (uint16_t)(resp[2] + 5U)))
    {
        return AR2_MB_ERR_DATA;
    }

    *bitfield = resp[3];

    if (resp[2] >= 2U)
    {
        *bitfield |= ((uint16_t)resp[4] << 8U);
    }

    return AR2_MB_OK;
}

ar2_modbus_status_t ReadDiscreteInputs(ar2_modbus_slave_context_t *slave_context,
                                       uint16_t start_addr,
									   uint16_t quantity,
									   uint16_t *bitfield)
{
    uint8_t req[8];
    uint8_t resp[7];
    uint16_t req_len;
    uint16_t resp_len = 0U;
    uint8_t exception_code = 0U;
    ar2_modbus_status_t status;

    if ((slave_context == NULL) || (bitfield == NULL))
    {
        return AR2_MB_ERR_PARAM;
    }

    if ((quantity == 0U) || (quantity > AR2_MAX_DISCRETE_PER_REQUEST))
    {
        return AR2_MB_ERR_PARAM;
    }

    req_len = BuildReadRequest(slave_context->slave_id,
                                      MODBUS_FC_READ_DISCRETE_INPUTS,
                                      start_addr,
                                      quantity,
                                      req);

    status = MODBUS_Transceive(slave_context, req, req_len, resp, sizeof(resp), &resp_len);
    if (status != AR2_MB_OK)
    {
        return status;
    }

    status = MODBUS_CheckCommonResponse(slave_context,
                                        MODBUS_FC_READ_DISCRETE_INPUTS,
                                        resp,
                                        resp_len,
                                        &exception_code);
    if (status != AR2_MB_OK)
    {
        return status;
    }

    if ((resp[2] == 0U) || (resp[2] > 2U))
    {
        return AR2_MB_ERR_DATA;
    }

    if ((resp_len != (uint16_t)(resp[2] + 5U)))
    {
        return AR2_MB_ERR_DATA;
    }

    *bitfield = resp[3];

    if (resp[2] >= 2U)
    {
        *bitfield |= ((uint16_t)resp[4] << 8U);
    }

    return AR2_MB_OK;
}

const char *MODBUS_StatusToString(ar2_modbus_status_t status)
{
    switch (status)
    {
        case AR2_MB_OK:             return "AR2_MB_OK";
        case AR2_MB_ERR_PARAM:      return "AR2_MB_ERR_PARAM";
        case AR2_MB_ERR_TX:         return "AR2_MB_ERR_TX";
        case AR2_MB_ERR_RX_TIMEOUT: return "AR2_MB_ERR_RX_TIMEOUT";
        case AR2_MB_ERR_RX_SHORT:   return "AR2_MB_ERR_RX_SHORT";
        case AR2_MB_ERR_CRC:        return "AR2_MB_ERR_CRC";
        case AR2_MB_ERR_SLAVE_ID:   return "AR2_MB_ERR_SLAVE_ID";
        case AR2_MB_ERR_FUNCTION:   return "AR2_MB_ERR_FUNCTION";
        case AR2_MB_ERR_EXCEPTION:  return "AR2_MB_ERR_EXCEPTION";
        case AR2_MB_ERR_DATA:       return "AR2_MB_ERR_DATA";
        default:                    return "AR2_MB_ERR_UNKNOWN";
    }
}
