## Note
This code base works on a single reference board `STM32F407-DISC` so far following the ARMv7-M Architecture. The port mapping for the 3 UART available are:

### **UART 1**
| Port | Pin | UART Pin |
|:---:|:---:|:---: |
| B | 6 | TX | 
| A | 10 | RX | 
| A | 12 | RTS |  
| A | 11 |CTS | 


### **UART 2**
| Port | Pin | UART Pin |
|:---:|:---:|:---: |
| D | 5 | TX | 
| D | 6 | RX | 
| D | 4 | RTS |  
| D | 3 |CTS |

### **UART 3**
| Port | Pin | UART Pin |
|:---:|:---:|:---: |
| D | 8 | TX | 
| D | 9 | RX | 
| D | 12 | RTS |  
| D | 11 |CTS | 

**Note**: UART3_RTS shares the same port/pin with PWM_1. If PWM 1 is in use, UART_3 flow control shouldn't be enable i.e use ***UART_CFG_FLOW_CTRL_NONE***
