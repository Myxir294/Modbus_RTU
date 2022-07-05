# Modbus_RTU
Repozytorium zawiera projekt protokołu przemysłowego Modbus-RTU obsługiwanego na płytce Nucleo, zrealizowany w środowisku STM32-Cube. W ramach zadania udało się zaimplementować bufor cykliczny, odczytywanie i wysyłanie znaków z wykorzystaniem przerwań sprzętowych oraz napisać ogólny schemat maszyny stanu, przetwarzającej otrzymywane ramki (wraz z liczeniem CRC-16). W obecnej formie program pozwala na prezentację schematu działania protokołu, w przyszłości planuje się go rozszerzyć, tak aby był w pełni zgodny z oficjalną dokumentacją Modbus-RTU, oraz dodać więcej komentarzy do napisanego kodu w celu ułatwienia czytelności.

Do obsługi programu wykorzystywano ustalone ramki przesyłane znak po znaku w postaci szesnastkowej, za pomocą terminala Realterm. Koniec ramki (normalnie sygnalizowany z wykorzystaniem stempla czasowego) zasymulowano poprzez sztuczne zaznaczenie po wysłaniu ustalonej liczby znaków.

The repository contains a project of Modbus-RTU industrial transmission protocol, operating on Nucleo board, written in STM32-Cube IDE. As part of the task, I have managed to implement a cyclic buffer, a characters trasmission/reception system using hardware interrupts as well as the general scheme of state machine, processing the received data frames (including checking the CRC-16 sum). Currently, the program allows to show the basic functionalities of the protocol, in the future it will be improved and expanded, to make it more consistent with the official Modbus-RTU datasheet, as well as better described and easier to read.  

The program is operated with the use of prepared dataframes, transmitted character by character in a hexadecimal format, by the Realterm terminal. The end of dataframe (normally sygnalized by a specified timestamp) is simulated by artificial marking after sending the set number of characters. 

Further english descriptions will be added in the future.
