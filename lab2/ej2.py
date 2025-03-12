import RPi.GPIO as GPIO

import time

class pines:
    led1 = 22
    led2 = 23
    led3 = 24
    led4 = 25

    bot1 = 17
    bot2 = 27

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

gpio.setup_out(pins.led1)
gpio.setup_out(pins.led2)
gpio.setup_out(pins.led3)
gpio.setup_out(pins.led4)

gpio.setup_in_pulldown(pins.bot1)
gpio.setup_in_pulldown(pins.bot2)


cont = 0
pulse = 0
pulse2 = 0

gpio.write(pins.led1, 0)
gpio.write(pins.led2, 0)
gpio.write(pins.led3, 0)
gpio.write(pins.led4, 0)

def dec2bin(d, n):
    if d == 0:
        return "0000"
    res = ""
    while d > 0:
        res = str(d % n) + res
        d = d //n
    res = (4-len(res))*"0"+res
    return res
def dec2hex(d):
    h = hex(d)
    return h[2:].upper()

def writeleds(res):
    gpio.write(pins.led1, int(res[0]))
    gpio.write(pins.led2, int(res[1]))
    gpio.write(pins.led3, int(res[2]))
    gpio.write(pins.led4, int(res[3]))

try:
    while True:
        var = gpio.state(pins.bot1)
        if var == 1 and var != pulse:
            cont = cont + 1
            pulse = 1
        if var == 0:
            pulse = 0

        var2 = gpio.state(pins.bot2)
        if var2 == 1 and var2 != pulse2:
            cont = cont - 1
            pulse2 = 1
        if var2 == 0:
            pulse2 = 0

        if cont > 15 or cont < 0:
            cont = 0;

        writeleds(dec2bin(cont, 2))

        print("decimal: ",cont)
        print("binary: ",dec2bin(cont, 2))
        print("Hexadecimal: ", dec2hex(cont))


except KeyboardInterrupt:
    GPIO.cleanup()

