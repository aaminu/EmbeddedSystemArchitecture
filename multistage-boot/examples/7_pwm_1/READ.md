## Note
This code base works on a single reference board `STM32F407-DISC` so far following the ARMv7-M Architecture. The PWM Pin mapping made available via the provided function for the refrence board are highlighted below:

| Port | Pin | PWMx | Note |
|:---:|:---:|:---: |:---: |
| D | 12 | 1 | User LD4 
| D | 13 | 2 | User LD3
| D | 14 | 3 | User LD5
| D | 15 |4 | User LD6

## Example
The example provide uses two methods to simulate a light dimming
- One in the forever loop by
- Second one by using the a timer interrupt to dim the light
