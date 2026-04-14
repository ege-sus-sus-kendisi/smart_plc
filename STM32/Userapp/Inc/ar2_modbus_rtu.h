#ifndef INC_AR2_MODBUS_RTU_H_
#define INC_AR2_MODBUS_RTU_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MODBUS_FC_READ_COILS               		0x01U
#define MODBUS_FC_READ_DISCRETE_INPUTS     		0x02U
#define MODBUS_FC_READ_HOLDING_REGISTERS   		0x03U
#define MODBUS_FC_READ_INPUT_REGISTERS     		0x04U
#define MODBUS_FC_WRITE_SINGLE_COIL           	0x05U
#define MODBUS_FC_WRITE_SINGLE_REGISTER       	0x06U
#define MODBUS_FC_WRITE_MULTIPLE_COILS      	0x0FU
#define MODBUS_FC_WRITE_MULTIPLE_REGISTERS		0x10U

typedef enum
{
    AR2_MB_OK = 0,
    AR2_MB_ERR_PARAM,
    AR2_MB_ERR_TX,
    AR2_MB_ERR_RX_TIMEOUT,
    AR2_MB_ERR_RX_SHORT,
    AR2_MB_ERR_CRC,
    AR2_MB_ERR_SLAVE_ID,
    AR2_MB_ERR_FUNCTION,
    AR2_MB_ERR_EXCEPTION,
    AR2_MB_ERR_DATA
} ar2_modbus_status_t;

typedef enum
{
    AR2_MB_EXCEPTION_ILLEGAL_FUNCTION      = 0x01U,
    AR2_MB_EXCEPTION_ILLEGAL_DATA_ADDRESS  = 0x02U,
    AR2_MB_EXCEPTION_ILLEGAL_DATA_VALUE    = 0x03U,
    AR2_MB_EXCEPTION_SLAVE_DEVICE_FAILURE  = 0x04U
} ar2_modbus_exception_t;

/*
 * Slave-specific context. Allows different timeout values for different slaves.
 */
typedef struct
{
    uint8_t  slave_id;
    uint32_t timeout_ms;
} ar2_modbus_slave_context_t;

void MODBUS_SlaveInit(ar2_modbus_slave_context_t *ctx, uint8_t slave_id, uint32_t timeout_ms);

uint16_t GET_CRC16(const uint8_t *data, uint16_t len);

ar2_modbus_status_t ReadHoldingRegister(ar2_modbus_slave_context_t *ctx,
                                           uint16_t reg_addr,
                                           uint16_t *value);

ar2_modbus_status_t ReadInputRegister(ar2_modbus_slave_context_t *ctx,
                                         uint16_t reg_addr,
                                         uint16_t *value);

ar2_modbus_status_t ReadCoils(ar2_modbus_slave_context_t *ctx,
                                 uint16_t start_addr,
                                 uint16_t quantity,
                                 uint16_t *bitfield);

ar2_modbus_status_t ReadDiscreteInputs(ar2_modbus_slave_context_t *ctx,
                                          uint16_t start_addr,
                                          uint16_t quantity,
                                          uint16_t *bitfield);

const char *MODBUS_StatusToString(ar2_modbus_status_t status);

#ifdef __cplusplus
}
#endif

#endif /* INC_AR2_MODBUS_RTU_H_ */
