#ifndef SERIAL_H
#define SERIAL_H

#include "main.h"

#define SERIAL_BUFFER_SIZE 1024

typedef struct serial_buffer_struct {
	char buffer[SERIAL_BUFFER_SIZE];
	volatile uint32_t front;
	volatile uint32_t back;
}serial_buffer;

typedef struct Frame{
    uint8_t Address;
    uint8_t Function;
    uint8_t Data[32];
    uint16_t crc;
    int length;
    int expected_length;
}Frame;

extern uint16_t crc_rx_global;
extern uint16_t crc_tx_global;


void serial_init(UART_HandleTypeDef *huart);


uint32_t serial_available(void);
char serial_read(void);
void serial_flush(void);
void serial_push_back(char c);

void put_tx(char c);

uint32_t get_back(void);

void crc16_update(uint16_t* crc, uint8_t a);

#endif /* SERIAL_H */
