## Note
This code base works on a single reference board `STM32F407-DISC` so far following the ARMv7-M Architecture. The PWM Pin mapping made available via the provided function for the refrence board are highlighted below:

| Port | Pin | PWMx | Timer Channel |Note |
|:---:|:---:|:---: |:---: |:---: |
| D | 12 | 1 |  TIM4_CH1  | User LD4 
| D | 13 | 2 |  TIM4_CH2  | User LD3
| D | 14 | 3 |  TIM4_CH3  | User LD5
| D | 15 |4 |  TIM4_CH4  | User LD6

## Description
The example provides a method to simulate a light dimming in the forever loop
