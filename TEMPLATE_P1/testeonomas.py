import serial
import RPi.GPIO as GPIO
import time
import os
import re

# Definición de pines como clase separada (solo como referencia inicial)


class Pines:
    MOTOR1_ENA, MOTOR1_IN1, MOTOR1_IN2 = 13, 5, 6
    MOTOR2_ENB, MOTOR2_IN3, MOTOR2_IN4 = 12, 7, 8
    BUTTON_PIN = 18

# Clase principal para controlar la Raspberry Pi


class RaspiControl:
    def __init__(self):
        """Inicializa la configuración básica de GPIO, PWM y UART."""
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
        """Configura un pin como salida para un LED.

        Args:
            led_pin (int): Número del pin GPIO a configurar como salida.
        """
        GPIO.setup(led_pin, GPIO.OUT)

    def setup_button_in(self, button_pin, pull_up_down=None):
        """Configura un pin como entrada para un botón con opción de pull-up/down.

        Args:
            button_pin (int): Número del pin GPIO a configurar como entrada.
            pull_up_down (str, optional): 'up' para pull-up, 'down' para pull-down, None para sin resistencia.
        """
        if pull_up_down == "up":
            GPIO.setup(button_pin, GPIO.IN, pull_up_down=GPIO.PUD_UP)
        elif pull_up_down == "down":
            GPIO.setup(button_pin, GPIO.IN, pull_up_down=GPIO.PUD_DOWN)
        else:
            GPIO.setup(button_pin, GPIO.IN)

    def setup_motor_pwm(self, motor_pwm_pin, frequency=1000):
        """Configura PWM en un pin para controlar un motor.

        Args:
            motor_pwm_pin (int): Pin GPIO donde se aplicará PWM (normalmente ENA).
            frequency (int, optional): Frecuencia del PWM en Hz (por defecto 1000).

        Returns:
            GPIO.PWM: Objeto PWM configurado para el pin.
        """
        if motor_pwm_pin not in self.pwm_objects:
            self.setup_led_out(motor_pwm_pin)  # Configura como salida primero
            self.pwm_objects[motor_pwm_pin] = GPIO.PWM(
                motor_pwm_pin, frequency)
            self.pwm_objects[motor_pwm_pin].start(0)
        return self.pwm_objects[motor_pwm_pin]

    def stop_motors(self, ena_pin, in1_pin, in2_pin):
        """Apaga un motor deteniendo PWM y poniendo los pines de control en bajo.

        Args:
            ena_pin (int): Pin GPIO conectado a ENA (PWM) del motor.
            in1_pin (int): Pin GPIO conectado a IN1 del motor.
            in2_pin (int): Pin GPIO conectado a IN2 del motor.
        """
        if ena_pin in self.pwm_objects:
            self.pwm_objects[ena_pin].ChangeDutyCycle(0)
        GPIO.output([in1_pin, in2_pin], GPIO.LOW)

    def start_motor(self, ena_pin, in1_pin, in2_pin, duty_cycle):
        """Enciende un motor con un ciclo de trabajo específico usando PWM.

        Args:
            ena_pin (int): Pin GPIO conectado a ENA (PWM) del motor.
            in1_pin (int): Pin GPIO conectado a IN1 del motor.
            in2_pin (int): Pin GPIO conectado a IN2 del motor.
            duty_cycle (int): Valor de PWM (0-100) para controlar la velocidad.
        """
        pwm = self.setup_motor_pwm(
            ena_pin)  # Asegura que el pin tenga PWM configurado
        GPIO.output(in1_pin, GPIO.HIGH)
        GPIO.output(in2_pin, GPIO.LOW)
        pwm.ChangeDutyCycle(duty_cycle)
        self.send_uart_message(
            f"Motor en pin {ena_pin} encendido al {duty_cycle}%")

    def send_uart_message(self, message):
        """Envía un mensaje a través de UART.

        Args:
            message (str): Mensaje a enviar por el puerto serial.
        """
        self.ser.write(f"{message}\n".encode('utf-8'))
        print(f"Mensaje enviado: {message}")

    def read_uart_data(self):
        """Lee datos recibidos por UART.

        Returns:
            str: Datos decodificados desde UART o cadena vacía si no hay datos o hay error.
        """
        try:
            data = self.ser.readline().decode('utf-8').strip()
            return data if data else ""
        except UnicodeDecodeError:
            print("Error decodificando datos UART, ignorando...")
            return ""

    def read_file_data(self, file_path):
        """Lee datos optimizados de un archivo (PWM y flag).

        Args:
            file_path (str): Ruta del archivo a leer (ej. 'datos.txt').

        Returns:
            tuple: (PWM, flag) como enteros extraídos del archivo.
        """
        try:
            mod_time = os.path.getmtime(file_path)
            if mod_time != self.ultima_modificacion:
                with open(file_path, 'r') as f:
                    lineas = f.readlines()
                self.ultima_modificacion = mod_time
                pwm_match = re.search(r'\d+', lineas[0]) if lineas else None
                flag_match = re.search(
                    r'\d+', lineas[1]) if len(lineas) > 1 else None
                self.valores_cache["PWM"] = int(
                    pwm_match.group()) if pwm_match else 0
                self.valores_cache["flag"] = int(
                    flag_match.group()) if flag_match else 0
        except (FileNotFoundError, IndexError, ValueError) as e:
            print(f"Error leyendo {file_path}: {e}")
        return self.valores_cache["PWM"], self.valores_cache["flag"]

    def check_button_state(self, button_pin, last_state):
        """Verifica el estado de un botón y detecta flancos ascendentes.

        Args:
            button_pin (int): Pin GPIO conectado al botón.
            last_state (int): Último estado conocido del botón (GPIO.LOW o GPIO.HIGH).

        Returns:
            tuple: (bool, int) - (True si se presionó, nuevo estado del botón).
        """
        current_state = GPIO.input(button_pin)
        if current_state != last_state and current_state == GPIO.HIGH:
            return True, current_state
        return False, current_state

    def cleanup(self):
        """Limpia todos los recursos GPIO, PWM y UART al finalizar."""
        for pwm in self.pwm_objects.values():
            pwm.stop()
        GPIO.cleanup()
        self.ser.close()


# Uso del código
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

print("Esperando datos UART BRO...")

try:
    while True:
        PWM, flag = raspi.read_file_data(archivo)

        if comando == 0:
            raspi.stop_motors(
                pins.MOTOR1_ENA, pins.MOTOR1_IN1, pins.MOTOR1_IN2)
            raspi.stop_motors(
                pins.MOTOR2_ENB, pins.MOTOR2_IN3, pins.MOTOR2_IN4)
        elif comando == 1:
            raspi.start_motor(pins.MOTOR1_ENA, pins.MOTOR1_IN1,
                              pins.MOTOR1_IN2, PWM)
            raspi.stop_motors(
                pins.MOTOR2_ENB, pins.MOTOR2_IN3, pins.MOTOR2_IN4)
        elif comando == 2:
            raspi.start_motor(pins.MOTOR2_ENB, pins.MOTOR2_IN3,
                              pins.MOTOR2_IN4, PWM)
            raspi.stop_motors(
                pins.MOTOR1_ENA, pins.MOTOR1_IN1, pins.MOTOR1_IN2)

        data = raspi.read_uart_data()
        if data:
            print(f"Mensaje recibido: {data}")
            if data == "motor1":
                comando = 1
                print(f"Motor 1 encendido al {PWM}%")
            elif data == "motor2":
                comando = 2
                print(f"Motor 2 encendido al {PWM}%")
            elif data == "apagar":
                comando = 0
                raspi.stop_motors(
                    pins.MOTOR1_ENA, pins.MOTOR1_IN1, pins.MOTOR1_IN2)
                raspi.stop_motors(
                    pins.MOTOR2_ENB, pins.MOTOR2_IN3, pins.MOTOR2_IN4)
                raspi.send_uart_message("Motores apagados")
            elif data == "estado":
                estado1 = raspi.pwm_objects.get(pins.MOTOR1_ENA, None)
                estado2 = raspi.pwm_objects.get(pins.MOTOR2_ENB, None)
                estado1_val = estado1.DutyCycle() if estado1 else 0
                estado2_val = estado2.DutyCycle() if estado2 else 0
                raspi.send_uart_message(
                    f"Estado: Motor 1 al {estado1_val}%, Motor 2 al {estado2_val}%")

        disparar_buzzer, ultimo_estado_boton = raspi.check_button_state(
            pins.BUTTON_PIN, ultimo_estado_boton)
        if disparar_buzzer:
            raspi.send_uart_message("buzzer")

        time.sleep(0.1)

except KeyboardInterrupt:
    print("Programa terminado")
finally:
    raspi.cleanup()
