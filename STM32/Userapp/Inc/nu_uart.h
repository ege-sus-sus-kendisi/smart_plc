/**
 ******************************************************************************
 * @file    nu_uart.h
 * @brief   Multi-channel UART driver library for STM32L4
 * @author	NU Technology Inc.
 * @version 1.0
 * @date    Sep 2022 (original), Feb 2026 (energy analyzer adaptation)
 *
 * Provides per-channel TX/RX functions for up to 4 USARTs:
 *   UART1: NRF9151 inter-MCU communication (query/response protocol)
 *   UART2: Modbus RTU to ENTES energy meter (RS-485 via PA4 DE pin)
 *   UART3/4: Legacy helpers from older NU Tech projects.
 *
 * Each channel has:
 *   - Circular receive buffer (UART_BUF, 149+1 bytes)
 *   - Byte-level REC_PROCESS callback (called from ISR)
 *   - Packet send (SEND_PACK_UARTx)
 *   - CRC-16 Modbus RTU calculation (GET_MODBUS_CRC16)
 *
 * © NU Technology Inc. — All rights reserved.
 *
 ******************************************************************************
 */

#ifndef INC_NU_UART_H_
#define INC_NU_UART_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "main.h"
#include "cmsis_os2.h"

#define BUFLEN 				149
#define UART2_IDLE_FLAG 	(1U << 0)

/* Set by USART2 IDLE interrupt when a full Modbus frame is assumed received.
 * Cleared by Modbus_SendAndReceive() after consuming buf_uart2.
 */
extern volatile uint8_t uart2_frame_ready;

void USART_Configuration(void);
void USART1_Send_Enter(void);
void USART1_Put_Inf(char *inf,uint32_t dat);
void USART1_Put_Num(uint32_t dat);
void u32tostr(uint32_t dat,char *str);
void USART1_Send_Str(const char* data);
void USART1_Send_Byte(uint16_t Data);
void USART1_Byte_PACKET(uint8_t size, const uint8_t packet[]);
extern void	CLR_BUFF_UART1(void);
extern void REC_PROCESS_FOR_UART1(uint8_t data);
extern void nextion_examine_received_data(uint8_t dataPack[], uint8_t sizeOfRequestReceived);
extern void nextion_examine_received_command(char *cmd);
extern void WiFi_examine_received_data(uint8_t dataPack[], uint8_t sizeOfDataPack);

void USART2_Send_Enter(void);
void USART2_Put_Inf(char *inf,uint32_t dat);
void USART2_Put_Num(uint32_t dat);
void USART2_Send_Str(const char* data);
void USART2_Send_Byte(uint16_t Data);
void USART2_Byte_PACKET(uint8_t size, const uint8_t packet[]);
extern void CLR_BUFF_UART2(void);
extern void REC_PROCESS_FOR_UART2(uint8_t data);

extern void USART3_Send_Enter(void);
extern void USART3_Put_Inf(char *inf,uint32_t dat);
extern void USART3_Put_Num(uint32_t dat);
extern void USART3_Send_Str(const char* data);
extern void USART3_Send_Byte(uint16_t Data);
extern void USART3_Byte_PACKET(uint8_t size, const uint8_t packet[]);
extern void CLR_BUFF_UART3(void);
extern void REC_PROCESS_FOR_UART3(uint8_t data);

extern void USART4_Send_Enter(void);
extern void USART4_Put_Inf(char *inf,uint32_t dat);
extern void USART4_Put_Num(uint32_t dat);
extern void USART4_Send_Str(const char* data);
extern void USART4_Send_Byte(uint16_t Data);
extern void USART4_Byte_PACKET(uint8_t size, const uint8_t packet[]);
extern void CLR_BUFF_UART4(void);
extern void REC_PROCESS_FOR_UART4(uint8_t data);
extern void SEND_PACK_UART2(uint8_t size, const uint8_t packet[]);
extern void SEND_PACK_UART1(uint8_t size, const uint8_t packet[]);
extern void SEND_PACK_UART3(uint8_t size, const uint8_t packet[]);
extern void SEND_PACK_UART4(uint8_t size, const uint8_t packet[]);

extern uint16_t GET_MODBUS_CRC16(const uint8_t* data, uint16_t length);

typedef struct _UART_BUF {
    uint8_t 		buf[BUFLEN+1];
    unsigned int	index ;
} UART_BUF;

extern uint8_t ChecksumValidation;
extern UART_BUF buf_uart1;
extern UART_BUF buf_uart2;
extern UART_BUF buf_uart3;
extern UART_BUF buf_uart4;

#endif /* INC_BP_UART_H_ */


