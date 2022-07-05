# Modbus_RTU
Repozytorium zawiera projekt protokołu przemysłowego Modbus-RTU obsługiwanego na płytce Nucleo, zrealizowany w środowisku STM32-Cube. W ramach zadania udało się zaimplementować bufor cykliczny, odczytywanie i wysyłanie znaków z wykorzystaniem przerwań sprzętowych oraz napisać ogólny schemat maszyny stanu przetwarzającej otrzymywane ramki (wraz z liczeniem CRC-16). W obecnej formie program pozwala na prezentację schematu działania protokołu, w przyszłości planuje się rozszerzyć go tak, aby był w pełni zgodny z oficjalną dokumentacją Modbus-RTU.  

Do obsługi programu wykorzystywano ustalone ramki przesyłane znak po znaku w postaci szesnastkowej, za pomocą terminala Realterm.
