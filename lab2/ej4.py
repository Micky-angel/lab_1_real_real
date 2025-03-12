#ej4
import RPi.GPIO as GPIO
import time

# cambiar los pines de los leds

class pines:
    led1 = 22
    led2 = 23
    led3 = 24
    led4 = 25
    #botones
    bot1 = 17
    bot2 = 27
class raspi_GPIO:
    def _init_(self):
        
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
#configuramos los pines para los leds como salidas
#configuramos los pines como salidas
gpio.setup_in_pulldown(pins.bot1)
gpio.setup_in_pulldown(pins.bot2)

gpio.setup_out(pins.led1)
gpio.setup_out(pins.led2)
gpio.setup_out(pins.led3)
gpio.setup_out(pins.led4)

#configuracion previa para la rasp



flag = -1
tiempo = 1
pulse1 = 0
pulse2 = 0

last = -1
while True:
    if last != flag:
        tiempo=1
    if flag == 0:
        GPIO.output(pins.led1,GPIO.HIGH)
        time.sleep(tiempo)
    if flag == 1:
        GPIO.output(pins.led2,GPIO.HIGH)
        time.sleep(tiempo)
    if flag == 2:
        GPIO.output(pins.led3,GPIO.HIGH)
        time.sleep(tiempo)    
    if flag == 2:
        GPIO.output(pins.led4,GPIO.HIGH)
        time.sleep(tiempo)
    #
    var  = GPIO.input(boton_1) 
    if var == 1 && var!=pulse1:
        flag=flag+1
        pulse1 = var
    if var == 0:
        pulse1 = var
    #receteamos la bandera
    if flag > 3:
        flag=1

    var  = GPIO.input(boton_2) 
    if var == 1 && var!=pulse2:
        tiempo=tiempo+1
        pulse2 = var
    if var == 0:
        pulse2 = var    
    last=flag
    #contador para gpio
    GPIO.output(pins.led1,GPIO.LOW)
    GPIO.output(pins.led2,GPIO.LOW)
    GPIO.output(pins.led3,GPIO.LOW)
    GPIO.output(pins.led4,GPIO.LOW)
    time.sleep(tiempo)
