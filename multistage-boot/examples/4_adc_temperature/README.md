## Note 

This code base works on a single reference board`STM32F407-DISC` so far following the ARMv7-M Architecture. The ADC Pin mapping for the refrence board is highlighted below:


| Port | Pin | ADC Controller | Channel | Note |
|:---:|:---:|:---: |:---:|:---|
| A | 0 | 1,2,3 | 0  |  |
| A | 1 | 1,2,3 | 1 |  |
| A | 2 | 1,2,3 | 2 |  |
| A | 3 | 1,2,3 | 3 |  | 
| A | 4 | 1,2 | 4 |  |
| A | 5 | 1,2 | 5 |  |
| A | 6 | 1,2 | 6 |  |
| A | 7 | 1,2 | 7 |  |
| B | 0 | 1,2 | 8 |  |
| B | 1 | 1,2 | 9 |  |
| C | 0 | 1,2,3 | 10 |  |
| C | 1 | 1,2,3 | 11 |  |
| C | 2 | 1,2,3 | 12 |  |
| C | 3 | 1,2,3 | 13 |  |
| C | 4 | 1,2 | 14 |  |
| C | 5 | 1,2 | 15 |  |
| - | - | 1 | 16 | Temperature|
| - | - | 1 | 17 | VREFINT|
| - | - | 1 | 18 | VBAT/Temperature*|
