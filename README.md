# Modbus_RTU

The repository contains a project of Modbus-RTU industrial transmission protocol, operating on Nucleo board, written in STM32-Cube IDE. As part of the task, I have managed to implement a cyclic buffer, a characters trasmission/reception system using hardware interrupts as well as the general scheme of state machine, processing the received data frames (including checking the CRC-16 sum). Currently, the program allows to show the basic functionalities of the protocol, in the future it will be improved and expanded, to make it more consistent with the official Modbus-RTU datasheet, as well as better described and easier to read.  

The program is operated with the use of prepared dataframes, transmitted character by character in a hexadecimal format, by the Realterm terminal. The end of dataframe (normally sygnalized by a specified timestamp) is simulated by artificial marking after sending the set number of characters. 

Further english descriptions will be added in the future.

THE PROGRAM IS NOT A LITERAL INSTANCE OF MODBUS RTU PROTOCOL, IT JUST SHOWS ITS WORKING PRINCIPLES FOR EDUCATIONAL PURPOSES

Used dataframes TBA
