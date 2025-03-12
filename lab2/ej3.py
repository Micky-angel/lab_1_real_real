import RPi.GPIO as GPIO
import adafruit_dht
import board
import time

# Initialize the DHT11 sensor on GPIO 4
dht_device = adafruit_dht.DHT11(board.D20)
GPIO.setmode(GPIO.BCM)

LED_HEATER = 22 # LED rojo (calefacción)
LED_FAN = 23 # LED azul (ventilador)

GPIO.setup(LED_HEATER, GPIO.OUT)
GPIO.setup(LED_FAN, GPIO.OUT)

while True:
    try:
        # Read temperature and humidity
        temperatura = dht_device.temperature
        if temperatura is not None:
            print(f"Temperatura: {temperatura:.1f}°C")

            if temperatura < 12:
                GPIO.output(LED_HEATER, GPIO.HIGH) # Encender calefacción
                GPIO.output(LED_FAN, GPIO.LOW) # Apagar ventilador
            elif temperatura > 20:
                GPIO.output(LED_HEATER, GPIO.LOW) # Apagar calefacción
                GPIO.output(LED_FAN, GPIO.HIGH) # Encender ventilador
            else:
                GPIO.output(LED_HEATER, GPIO.LOW) # Apagar calefacción
                GPIO.output(LED_FAN, GPIO.LOW) # Apagar ventilador
        humidity = dht_device.humidity

        # Print the values
        print(f"Temperature: {temperatura:.1f}°C ")

    except RuntimeError as e:
        # If a reading error occurs, print the error message
        print(f"Sensor error: {e}")

    # Wait 2 seconds before taking another reading
    time.sleep(2)
