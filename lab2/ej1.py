import RPi.GPIO as GPIO

import time

class pines:
    led1 = 22
    led2 = 23

    bot1 = 17

class raspi_GPIO:
    def __init__(self):

        GPIO.setwarnings(False)
        GPIO.setmode(GPIO.BCM)

    def setup_out(self, Pinx):
        GPIO.setup(Pinx, GPIO.OUT)
    def setup_in_pulldown(self, Pinx):
        GPIO.setup(Pinx, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
    def setup_in_pullup(self, Pinx):
        GPIO.setup(Pinx, GPIO.IN, pull_up_down=GPIO.PUD_UP)
    def state(self, Pinx):
        return GPIO.input(Pinx)
    def write(self, Pinx, value):
        return GPIO.output(Pinx, value)




pins = pines()
gpio = raspi_GPIO()

gpio.setup_in_pulldown(pins.bot1)
gpio.setup_out(pins.led1)
gpio.setup_out(pins.led2)
pulse = 0
cont = 0


gpio.write(pins.led1, 0)
gpio.write(pins.led2, 0)
try:
    while True:
        var = gpio.state(pins.bot1)
        if var == 1 and var != pulse:
            cont = cont + 1
            pulse = var

        if var == 0:
            pulse = var

        if cont == 1:
            gpio.write(pins.led1, 1)
            gpio.write(pins.led2, 0)
            time.sleep(1)
            gpio.write(pins.led1, 0)
            gpio.write(pins.led2, 1)
            time.sleep(1)
        if cont == 2:
            gpio.write(pins.led1, 1)
            gpio.write(pins.led2, 1)
            time.sleep(2)
            gpio.write(pins.led1, 0)
            gpio.write(pins.led2, 0)
            time.sleep(2)
        if cont == 3:
            gpio.write(pins.led1, 1)
            gpio.write(pins.led2, 1)
        if cont == 4:
            gpio.write(pins.led1, 0)
            gpio.write(pins.led2, 0)
        if cont == 5:
            cont = 1;
        print(cont)
except KeyboardInterrupt:
    GPIO.cleanup()
