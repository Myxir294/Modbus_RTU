#include "main.h"
#include "serial.h"

UART_HandleTypeDef *serial_huart;
serial_buffer ser_buff;
serial_buffer tx_ser_buff;
uint8_t rx_char;
uint8_t tx_char;

uint8_t TX_BUSY;
uint16_t crc_rx_global = 0xFFFF;
uint16_t crc_tx_global = 0xFFFF;

void serial_init(UART_HandleTypeDef *huart) {
	ser_buff.front = 0;
	ser_buff.back = 0;
	tx_ser_buff.front = 0;
	tx_ser_buff.back = 0;
	TX_BUSY = 0;
	serial_huart = huart;
	HAL_UART_Receive_IT(serial_huart, &rx_char, 1);
}

uint32_t serial_available(void) {
	if(ser_buff.back > ser_buff.front) {
		return ser_buff.back - ser_buff.front;
	}
	else if(ser_buff.back < ser_buff.front) {
		return ser_buff.back + SERIAL_BUFFER_SIZE - ser_buff.front;
	}
	else {
		return 0;
	}
}

uint32_t send_available(void) {
	if(tx_ser_buff.back > tx_ser_buff.front) {
		return tx_ser_buff.back - tx_ser_buff.front;
	}
	else if(tx_ser_buff.back < tx_ser_buff.front) {
		return tx_ser_buff.back + SERIAL_BUFFER_SIZE - tx_ser_buff.front;
	}
	else {
		return 0;
	}
}

uint32_t tx_available(void) {
	if(tx_ser_buff.back > tx_ser_buff.front) {
		return tx_ser_buff.back - tx_ser_buff.front;
	}
	else if(tx_ser_buff.back < tx_ser_buff.front) {
		return tx_ser_buff.back + SERIAL_BUFFER_SIZE - tx_ser_buff.front;
	}
	else {
		return 0;
	}
}

char serial_read(void) {
	char c = '\0';
	if(serial_available()) {
		c = ser_buff.buffer[ser_buff.back++];
		if(SERIAL_BUFFER_SIZE == ser_buff.back)
			ser_buff.back = 0;
	}
	return c;
}

void get_tx(uint8_t* c) {
	if(tx_available()) {
		*c = tx_ser_buff.buffer[tx_ser_buff.back++];
		if(SERIAL_BUFFER_SIZE == tx_ser_buff.back)
			tx_ser_buff.back = 0;
	}
}

void serial_flush() {
	serial_init(serial_huart);
}

void serial_push(char c) {
	ser_buff.buffer[ser_buff.front++] = c;
	if(ser_buff.front == SERIAL_BUFFER_SIZE)
		ser_buff.front = 0;
}


void put_tx(char c) {
	tx_ser_buff.buffer[tx_ser_buff.front++] = c;
	if(tx_ser_buff.front == SERIAL_BUFFER_SIZE){
		tx_ser_buff.front = 0;
	}
	if (TX_BUSY == 0){
		HAL_UART_TxCpltCallback(serial_huart);
	}

}



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	serial_push(rx_char);
	crc16_update(&crc_rx_global, rx_char);
	HAL_UART_Receive_IT(serial_huart, &rx_char, 1);
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	if(tx_available()){
		TX_BUSY = 1;
		get_tx(&tx_char);
		crc16_update(&crc_tx_global, tx_char);
		HAL_UART_Transmit_IT(serial_huart, &tx_char, 1);
	}
	else
	{
		TX_BUSY = 0;
	}
}

uint32_t get_back(void) {
	return ser_buff.back;
}


void crc16_update(uint16_t* crc, uint8_t a) {
	int i;
	//crc_tx_global = crc_tx;
	*crc ^= (uint16_t)a;
	for (i = 0; i < 8; ++i) {
		if (*crc & 1)
		{
			*crc = (*crc >> 1) ^ 0xA001;
		}
		else
		{
			*crc = (*crc >> 1);
		}
	}
}
