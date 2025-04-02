import serial
import RPi.GPIO as GPIO
import time


MOTOR1_ENA = 13
MOTOR1_IN1 = 5
MOTOR1_IN2 = 6

MOTOR2_ENB = 12
MOTOR2_IN3 = 7
MOTOR2_IN4 = 8
TRIG = 17
ECHO = 27


BUTTON_PIN = 18

GPIO.setmode(GPIO.BCM)
GPIO.setup(MOTOR1_ENA, GPIO.OUT)
GPIO.setup(MOTOR1_IN1, GPIO.OUT)
GPIO.setup(MOTOR1_IN2, GPIO.OUT)
GPIO.setup(MOTOR2_ENB, GPIO.OUT)
GPIO.setup(MOTOR2_IN3, GPIO.OUT)
GPIO.setup(MOTOR2_IN4, GPIO.OUT)
GPIO.setup(BUTTON_PIN, GPIO.IN)


pwm1 = GPIO.PWM(MOTOR1_ENA, 1000)
pwm2 = GPIO.PWM(MOTOR2_ENB, 1000)
GPIO.setup(TRIG, GPIO.OUT)
GPIO.setup(ECHO, GPIO.IN)

pwm1.start(0)
pwm2.start(0)


ser = serial.Serial("/dev/serial0", 9600, timeout=1)


ser.write("Raspi conectada\n".encode('utf-8'))
print("Mensaje enviado: Raspi conectada")


def apagar_motores():
    pwm1.ChangeDutyCycle(0)
    pwm2.ChangeDutyCycle(0)
    GPIO.output(MOTOR1_IN1, GPIO.LOW)
    GPIO.output(MOTOR1_IN2, GPIO.LOW)
    GPIO.output(MOTOR2_IN3, GPIO.LOW)
    GPIO.output(MOTOR2_IN4, GPIO.LOW)


def encender_motor1(duty_cycle):
    GPIO.output(MOTOR1_IN1, GPIO.HIGH)
    GPIO.output(MOTOR1_IN2, GPIO.LOW)
    pwm1.ChangeDutyCycle(duty_cycle)
    pwm2.ChangeDutyCycle(0)
    ser.write(f"Motor 1 encendido al {duty_cycle}%\n".encode('utf-8'))


def encender_motor2(duty_cycle):
    GPIO.output(MOTOR2_IN3, GPIO.HIGH)
    GPIO.output(MOTOR2_IN4, GPIO.LOW)
    pwm2.ChangeDutyCycle(duty_cycle)
    pwm1.ChangeDutyCycle(0)
    ser.write(f"Motor 2 encendido al {duty_cycle}%\n".encode('utf-8'))


def leer_distancia():
    # Enviar pulso de T
    GPIO.output(TRIG, True)
    time.sleep(0.00001)
    GPIO.output(TRIG, False)

    tiempo_inicio = time.time()

    # Esperar señal de eco
    while GPIO.input(ECHO) == 0:
        inicio_pulso = time.time()
        if time.time() - tiempo_inicio > 1:
            print("Error: No se recibió señal de ECHO")
            return None

    while GPIO.input(ECHO) == 1:
        fin_pulso = time.time()

    duracion_pulso = fin_pulso - inicio_pulso
    distancia = (duracion_pulso * 34300) / 2
    return distancia


def enviar_mensaje(mensaje):
    ser.write(f"{mensaje}".encode())
    print(f"Mensaje enviado: {mensaje}")


def leer_lineas(archivo):
    with open(archivo, 'r') as f:
        return f.readlines()


print("Esperando datos UART BRO...")


# ultimo_estado_boton = GPIO.LOW

try:
    while True:
        distancia = leer_distancia()
        if distancia is not None:
            mensaje = f"Distancia: {distancia:.2f} cm"
            print(mensaje)

            var1 = 1
            var0 = 0
            if distancia < 7:
                print("Enviando: 1")
                enviar_mensaje("1\n")
            else:
                print("Enviando: 0")
                enviar_mensaje("0\n")

        time.sleep(1)


except KeyboardInterrupt:
    print("Programa terminado")
    apagar_motores()
    pwm1.stop()
    pwm2.stop()
    GPIO.cleanup()
    ser.close()
