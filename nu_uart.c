/**
 *****************************************************************************
 *
 * Designed by NU R&D
 * UART Library - STM32L4 Implementation
 * ENERGY ANALYZER COMMUNICATION - CLEAN VERSION (NO DEBUG OUTPUT)
 *
 * ===========================================================================
 *
 * Modified for NRF9151 ↔ STM32 Energy Data Exchange
 *
 * UART1 (NRF9151 Communication):
 *   RX — REC_PROCESS_FOR_UART1(): Receives query frames
 *         [STX=0x02][LEN][CMD][ADDR][VALUE][CRC16_LO][CRC16_HI][ETX=0x03]
 *   TX — Responds with meter data:
 *         [STX=0x02][LEN][METER_DATA...][CRC16_LO][CRC16_HI][ETX=0x03]
 *
 * UART2 (Modbus RTU):
 *   RX — REC_PROCESS_FOR_UART2(): Byte-level accumulation with IDLE-line
 *         detection (uart2_frame_ready flag set when complete frame received)
 *   TX — SEND_PACK_UART2(): RS-485 packet send with DE pin toggle (PA4)
 *
 *****************************************************************************
 */

#include <string.h>

#include "main.h"
#include "cmsis_os2.h"
#include "stm32l4xx.h"
#include "stm32l4xx_it.h"

#include "nu_uart.h"

UART_BUF buf_uart1;
UART_BUF buf_uart2;
UART_BUF buf_uart3;
UART_BUF buf_uart4;

uint8_t w_EMPTY;
uint8_t WiFi_TxBusyFLAG;

#define SIZEOFDATAPACK  24
uint8_t tempRecievedBuffer[BUFLEN+1];
uint8_t receivedDataPack[8];
uint8_t endOfBuffer;

volatile uint8_t uart2_frame_ready = 0;

void REC_PROCESS_FOR_UART1(uint8_t data);
void REC_PROCESS_FOR_UART2(uint8_t data);
void REC_PROCESS_FOR_UART3(uint8_t data);
void REC_PROCESS_FOR_UART4(uint8_t data);
void CLR_BUFF_UART1(void);
void CLR_BUFF_UART2(void);
void CLR_BUFF_UART3(void);
void CLR_BUFF_UART4(void);
void SEND_PACK_UART1(uint8_t size, const uint8_t packet[]);
void SEND_PACK_UART2(uint8_t size, const uint8_t packet[]);
void SEND_PACK_UART3(uint8_t size, const uint8_t packet[]);
void SEND_PACK_UART4(uint8_t size, const uint8_t packet[]);
void USART3_Send_Byte(uint16_t Data);
void USART3_Send_Enter(void);

uint8_t Find_First_Position(uint8_t array[], uint8_t size, uint8_t member);
uint8_t ChecksumValidation=0;

/**
 * @brief Calculate CRC-16 Modbus RTU (matches NRF9151's modbus_crc16)
 * @param data: Pointer to data buffer
 * @param length: Length of data
 * @return CRC-16 value
 */
uint16_t GET_MODBUS_CRC16(const uint8_t* data, uint16_t length)
{
    uint16_t crc = 0xFFFF;

    for (uint16_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc >>= 1;
                crc ^= 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

/* ============================================================================
   UART1 FUNCTIONS
   ============================================================================ */

/**
 * @brief UART1 - Record NRF9151 Query
 * @param data 
 * @details
 * 
 * Query format: 
 * [STX=0x02][LEN=0x03][CMD][ADDR][VALUE][CRC][ETX=0x03]
 *
 * Simple read_all_energy query:
 * [0x02][0x03][0xC2][0x01][0x00][CRC16-LO][CRC16-HI][0x03]
 * 
 * UART1 RX byte collector:
 * Assembles frames by looking for ETX. Parsing is intentionally light:
 * only recognizes the query CMD and then signals nrfTask via thread flag.
 * CRC validation is performed on the sender/receiver side as needed.
 *
 */

void REC_PROCESS_FOR_UART1(uint8_t data)
{
    /* Clean overflow */
    if (buf_uart1.index >= BUFLEN) {
        buf_uart1.index = 0;
        CLR_BUFF_UART1();
        return;
    }

    buf_uart1.buf[buf_uart1.index++] = data;

    /* Frame complete when we have ETX (0x03) at the end */
    if (buf_uart1.index >= 3 && buf_uart1.buf[buf_uart1.index - 1] == 0x03)
    {
        /* Check for query frame: [0x02][0x03][0xC2][...][0x03] */
        if (buf_uart1.buf[0] == 0x02 &&         // STX = 0x02
            buf_uart1.buf[1] == 0x03 &&         // LEN = 0x03 (CMD + ADDR + VALUE)
			buf_uart1.buf[2] == 0xC2)			// CMD = 0xC2
        {
			/* Avoid calling RTOS primitives before scheduler start:
			 * some early bytes can arrive during boot; ignore flagging until RTOS is live.
			 */
        	if (osKernelGetState() == osKernelRunning)
        	{
//        	    osThreadFlagsSet(nrfTaskHandle, 0x00000001U); // nrfTaskHandle
        	}
            CLR_BUFF_UART1();
            return;
        }

        /* Unrecognized frame - clear and wait for next */
        CLR_BUFF_UART1();
        return;
    }
}

void CLR_BUFF_UART1(void)
{
    unsigned int i = 0 ,c ;
    c = BUFLEN +1;

    for( i = 0 ; i < c ; i ++)
    	{
        	buf_uart1.buf[i] = 0x0 ;
    	}
    buf_uart1.index = 0 ;
}

void SEND_PACK_UART1(uint8_t size, const uint8_t packet[])
{
	uint8_t counter=0;
	for(counter=0;counter<size;counter++)
		{
			USART1_Send_Byte(packet[counter]);
		}
}

void USART1_Send_Byte(uint16_t Data)
{
	while (!(USART1->ISR & USART_ISR_TC));
	USART1->TDR = (Data & (uint16_t)0x01FF);
}

void USART1_Send_Str(const char* data)
{
	uint16_t i;
	uint16_t len = strlen(data)-1;

	for (i=0; i<len; i++)
		{
			USART1_Send_Byte(data[i]);
		}

	if(data[i]=='\n')
		{
			USART1_Send_Enter();
		}
		else
			{
				USART1_Send_Byte(data[i]);
			}
}

void USART1_Send_Enter(void)
{
	USART1_Send_Byte(0x0d);
	USART1_Send_Byte(0x0a);
}

void USART1_Put_Num(uint32_t dat)
{
 	char temp[20];
 	u32tostr(dat,temp);
 	USART1_Send_Str(temp);
}

void USART1_Put_Inf(char *inf,uint32_t dat)
{
 	USART1_Send_Str(inf);
 	USART1_Put_Num(dat);
 	USART1_Send_Str("\n");
}

/* ============================================================================
   UART2 FUNCTIONS
   ============================================================================ */
/** 
 * @brief UART2 RX collector for incoming data from the meter
 * @details
 * 
 * Meter data format:
 * [Slave Addr][Modbus Func Code][LEN][Payload][CRC16-LO][CRC16-HI]
 * 
 * Frame boundary is detected by USART2 IDLE-line interrupt (see stm32l4xx_it.c)
 */
void REC_PROCESS_FOR_UART2(uint8_t data)
{
	if (buf_uart2.index >= BUFLEN)
	{
		CLR_BUFF_UART2();
		return;
	}

	buf_uart2.buf[buf_uart2.index++] = data;
}

void CLR_BUFF_UART2(void)
{
    unsigned int i = 0 ,c ;
    c = BUFLEN +1;

    for( i = 0 ; i < c ; i ++)
    	{
        	buf_uart2.buf[i] = 0x0 ;
    	}
    buf_uart2.index = 0 ;
}

void SEND_PACK_UART2(uint8_t size, const uint8_t packet[])
{
    for (uint8_t i = 0; i < size; i++) {
        USART2_Send_Byte(packet[i]);
    }
    while (!(USART2->ISR & USART_ISR_TC));
}

void USART2_Send_Byte(uint16_t Data)
{
	while (!(USART2->ISR & USART_ISR_TC));
	USART2->TDR = (Data & (uint16_t)0x01FF);
}

void USART2_Send_Str(const char* data)
{
	uint16_t i;
	uint16_t len = strlen(data)-1;

	for (i=0; i<len; i++)
		{
			USART2_Send_Byte(data[i]);
		}

	if(data[i]=='\n')
		{
			USART2_Send_Enter();
		}
		else
			{
				USART2_Send_Byte(data[i]);
			}
}

void USART2_Send_Enter(void)
{
	USART2_Send_Byte(0x0d);
	USART2_Send_Byte(0x0a);
	HAL_Delay(10);
}

void USART2_Put_Num(uint32_t dat)
{
 	char temp[20];
 	u32tostr(dat,temp);
 	USART2_Send_Str(temp);
}

void USART2_Put_Inf(char *inf,uint32_t dat)
{
 	USART2_Send_Str(inf);
 	USART2_Put_Num(dat);
 	USART2_Send_Str("\n");
}

/* ============================================================================
   UART3 FUNCTIONS
   ============================================================================ */

void REC_PROCESS_FOR_UART3(uint8_t data)
{
	int8_t i = 0, j = 0;
	{
		if (buf_uart3.index >= BUFLEN) {
			buf_uart3.index = 0;
			CLR_BUFF_UART3();
		}
			else {
				if ((buf_uart3.index >= 7) && (data == 0x03)) {
					buf_uart3.buf[buf_uart3.index] = data;
					strncpy((char*) tempRecievedBuffer, (char*) buf_uart3.buf, BUFLEN + 1);
					endOfBuffer = buf_uart3.index;
					CLR_BUFF_UART3();

					j = endOfBuffer;
					for (i = SIZEOFDATAPACK - 1; i >= 0; i--) {
						receivedDataPack[i] = tempRecievedBuffer[j--];
					}
				}
					else {
						buf_uart3.buf[buf_uart3.index++] = data;
					}
			}
	}
}

void CLR_BUFF_UART3(void)
{
    unsigned int i = 0 ,c ;
    c = BUFLEN +1;

    for( i = 0 ; i < c ; i ++)
    	{
        	buf_uart3.buf[i] = 0x0 ;
    	}
    buf_uart3.index = 0 ;
}

void SEND_PACK_UART3(uint8_t size, const uint8_t packet[])
{
	uint8_t counter=0;
	for(counter=0;counter<size;counter++)
		{
			USART3_Send_Byte(packet[counter]);
		}
}

void USART3_Send_Byte(uint16_t Data)
{
	while (!(USART3->ISR & USART_ISR_TC));
	USART3->TDR = (Data & (uint16_t)0x01FF);
}

void USART3_Send_Str(const char* data)
{
	uint16_t i;
	uint16_t len = strlen(data)-1;

	for (i=0; i<len; i++)
		{
			USART3_Send_Byte(data[i]);
		}

	if(data[i]=='\n')
		{
			USART3_Send_Enter();
		}
		else
			{
				USART3_Send_Byte(data[i]);
			}
}

void USART3_Send_Enter(void)
{
	USART3_Send_Byte(0x0d);
	USART3_Send_Byte(0x0a);
}

void USART3_Put_Num(uint32_t dat)
{
 	char temp[20];
 	u32tostr(dat,temp);
 	USART3_Send_Str(temp);
}

void USART3_Put_Inf(char *inf,uint32_t dat)
{
 	USART3_Send_Str(inf);
 	USART3_Put_Num(dat);
 	USART3_Send_Str("\n");
}

void u32tostr(uint32_t dat,char *str)
{
 	char temp[20];
 	uint8_t i=0,j=0;
 	i=0;

 	while(dat)
 		{
  			temp[i]=dat%10+0x30;
  			i++;
  			dat/=10;
 		}
 	j=i;

 	for(i=0;i<j;i++)
 		{
  			str[i]=temp[j-i-1];
 		}
 	if(!i) {str[i++]='0';}
 	str[i]=0;
}

/* ============================================================================
   UART4 FUNCTIONS
   ============================================================================ */

void REC_PROCESS_FOR_UART4(uint8_t data)
{
	int8_t i = 0, j = 0;

	if (buf_uart4.index >= BUFLEN) {
		buf_uart4.index = 0;
		CLR_BUFF_UART4();
	}
		else {
			if ((buf_uart4.index >= 7) && (data == 0x03) && (WiFi_TxBusyFLAG == w_EMPTY))
				{
					buf_uart4.buf[buf_uart4.index] = data;
					strncpy((char*) tempRecievedBuffer, (char*) buf_uart4.buf, BUFLEN + 1);
					endOfBuffer = buf_uart4.index;
					CLR_BUFF_UART4();

					j = endOfBuffer;
					for (i = SIZEOFDATAPACK - 1; i >= 0; i--) {
						receivedDataPack[i] = tempRecievedBuffer[j--];
					}
				}
				else {
					buf_uart4.buf[buf_uart4.index++] = data;
				}
		}
}

void CLR_BUFF_UART4(void)
{
    unsigned int i = 0 ,c ;
    c = BUFLEN +1;

    for( i = 0 ; i < c ; i ++)
    	{
        	buf_uart4.buf[i] = 0x0 ;
    	}
    buf_uart4.index = 0 ;
}

void SEND_PACK_UART4(uint8_t size, const uint8_t packet[])
{
	uint8_t counter=0;
	for(counter=0;counter<size;counter++)
		{
			USART4_Send_Byte(packet[counter]);
		}
}

void USART4_Send_Byte(uint16_t Data)
{
	// Placeholder - not implemented
}

void USART4_Send_Str(const char* data)
{
	uint16_t i;
	uint16_t len = strlen(data)-1;

	for (i=0; i<len; i++)
		{
			USART4_Send_Byte(data[i]);
		}

	if(data[i]=='\n')
		{
			USART4_Send_Enter();
		}
		else
			{
				USART4_Send_Byte(data[i]);
			}
}

void USART4_Send_Enter(void)
{
	USART4_Send_Byte(0x0d);
	USART4_Send_Byte(0x0a);
}

void USART4_Put_Num(uint32_t dat)
{
 	char temp[20];
 	u32tostr(dat,temp);
 	USART4_Send_Str(temp);
}

void USART4_Put_Inf(char *inf,uint32_t dat)
{
 	USART4_Send_Str(inf);
 	USART4_Put_Num(dat);
 	USART4_Send_Str("\n");
}

uint8_t Find_First_Position(uint8_t array[], uint8_t size, uint8_t member){
	uint8_t i, location;
	int8_t first = -1;

    for (i = 0; i < size; i++) {
        if (member != array[i])
            continue;
        if (first == -1)
            first = i;
    }
    if (first != -1)
    	location= first;

    return location;
}
