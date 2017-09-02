# ultraPing
Arduino Uno running HC-SR4 ultrasonic sensor with interrupts

This project uses arduino pin 3 (Atmel PD3) for echo and pin 2 (Atmel
PD2) for trigger. Interrupts are enabled on pin 3 to detect the rising
and falling edges and measure the exact amount of time the pin is high
without keeping the CPU busy while waiting for the pulse to complete.
