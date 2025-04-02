import serial
import RPi.GPIO as GPIO
import time
import os
import re
import smtplib
from email.mime.text import MIMEText


# ---------------------------- main pines -----------------------------


class Pines:
    MOTOR1_ENA, MOTOR1_IN1, MOTOR1_IN2 = 13, 5, 6
    MOTOR2_ENB, MOTOR2_IN3, MOTOR2_IN4 = 12, 7, 8
    BUTTON_PIN = 18


# ---------------------------- main raspi -----------------------------


class Raspicontrol:
    def __init__(self):
        GPIO.setwarnings(False)
        GPIO.setmode(GPIO.BCM)

        self.pins = Pines()
        # Diccionario para almacenar objetos PWM por pin
        self.pwm_objects = {}

        # Configurar UART
        self.ser = serial.Serial("/dev/serial0", 9600, timeout=1)
        self.send_uart_message("Raspi conectada")

        # Variables para lectura de archivo
        self.ultima_modificacion = 0
        self.valores_cache = {"PWM": 0, "flag": 0}

    def setup_led_out(self, led_pin):

        GPIO.setup(led_pin, GPIO.OUT)

    def setup_button_in(self, button_pin, pull_up_down=None):

        if pull_up_down == "up":
            GPIO.setup(button_pin, GPIO.IN, pull_up_down=GPIO.PUD_UP)
        elif pull_up_down == "down":
            GPIO.setup(button_pin, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
        else:
            GPIO.setup(button_pin, GPIO.IN)

    def setup_motor_pwm(self, motor_pwm_pin, frequency=1000):

        if motor_pwm_pin not in self.pwm_objects:
            self.setup_led_out(motor_pwm_pin)  # Configura como salida primero
            self.pwm_objects[motor_pwm_pin] = GPIO.PWM(
                motor_pwm_pin, frequency)
            self.pwm_objects[motor_pwm_pin].start(0)
        return self.pwm_objects[motor_pwm_pin]

    def stop_motors(self, ena_pin, in1_pin, in2_pin):

        if ena_pin in self.pwm_objects:
            self.pwm_objects[ena_pin].ChangeDutyCycle(0)
        GPIO.output([in1_pin, in2_pin], GPIO.LOW)

    def start_motor(self, ena_pin, in1_pin, in2_pin, duty_cycle):

        pwm = self.setup_motor_pwm(
            ena_pin)  # Asegura que el pin tenga PWM configurado
        GPIO.output(in1_pin, GPIO.HIGH)
        GPIO.output(in2_pin, GPIO.LOW)
        pwm.ChangeDutyCycle(duty_cycle)
        self.send_uart_message(
            f"Motor en pin {ena_pin} encendido al {duty_cycle}%")

    def send_uart_message(self, message):

        self.ser.write(f"{message}\n".encode('utf-8'))
        print(f"Mensaje enviado: {message}")

    def read_uart_data(self):

        try:
            data = self.ser.readline().decode('utf-8').strip()
            return data if data else ""
        except UnicodeDecodeError:
            print("Error decodificando datos UART, ignorando...")
            return ""

    def read_file_data(self, file_path):
        try:
            mod_time = os.path.getmtime(file_path)
            if mod_time != self.ultima_modificacion:
                with open(file_path, 'r') as f:
                    self.valores_cache = [linea.strip()
                                          for linea in f.readlines()]
                self.ultima_modificacion = mod_time
        except (FileNotFoundError, OSError) as e:
            print(f"Error leyendo {file_path}: {e}")
            self.valores_cache = []  # Dlista vacía
        return self.valores_cache

    def check_button_state(self, button_pin, last_state):

        current_state = GPIO.input(button_pin)
        if current_state != last_state and current_state == GPIO.HIGH:
            return True, current_state
        return False, current_state

    def enviar_correo(correo_origen, contraseña, correo_destino, asunto, mensaje):
        try:
            msg = MIMEText(mensaje)
            msg['Subject'] = asunto
            msg['From'] = correo_origen
            msg['To'] = correo_destino

            server = smtplib.SMTP('smtp.gmail.com', 587)
            server.starttls()
            server.login(correo_origen, contraseña)
            server.sendmail(correo_origen, correo_destino, msg.as_string())
            server.quit()

            print("Email enviado correctamente")
        except Exception as e:
            print(f"Error al enviar el correo: {e}")

    def cleanup(self):

        for pwm in self.pwm_objects.values():
            pwm.stop()
        GPIO.cleanup()
        self.ser.close()


# ----------------------------------------------- INICIO -----------------------------------------

TRIG = 17
ECHO = 27

GPIO.setup(TRIG, GPIO.OUT)
GPIO.setup(ECHO, GPIO.IN)

raspi = RaspiControl()
pins = Pines()
archivo = "datos.txt"
comando = 0
ultimo_estado_boton = GPIO.LOW

# Configurar pines iniciales
raspi.setup_led_out(pins.MOTOR1_ENA)
raspi.setup_led_out(pins.MOTOR1_IN1)
raspi.setup_led_out(pins.MOTOR1_IN2)
raspi.setup_led_out(pins.MOTOR2_ENB)
raspi.setup_led_out(pins.MOTOR2_IN3)
raspi.setup_led_out(pins.MOTOR2_IN4)
raspi.setup_button_in(pins.BUTTON_PIN)


ser = serial.Serial(
    port='/dev/serial0',
    baudrate=9600,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    timeout=0.1)


datoraspi = "1"

try:
    print("Iniciando comunicación UART...")
    while True:
        # ---------------------------------------------------UART VARIABLES -------------------------------
        # sirve para enviar datoraspi
        ser.write(datoraspi.encode('utf-8'))
        print(f"Enviado a tiva: {datoraspi}")

        # sirve para recibir datotiva
        datotiva = ser.readline().decode('utf-8').strip()
        if datotiva:
            print(f"Recibido: {datotiva}")
        else:
            print("No se recepcionó nada Mr Tiva ")
# ---------------------------------------------------UART VARIABLES -------------------------------

        time.sleep(0.5)

except KeyboardInterrupt:
    print("\nPrograma terminado")
finally:
    ser.close()  # CIERRA el puerto serial
    print("Puerto serial cerrado")


# enviar_correo(
#    correo_origen='michaelsilver1674@gmail.com',
#    contraseña='slqw icih ingc djha',
#    correo_destino='nayara.lara@ucb.edu.bo',
#    asunto='Prueba',
#    mensaje='Hola'
# )
