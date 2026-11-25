
import serial
import csv
from datetime import datetime
import math

import os



PORT = "COM3"
BAUD_RATE = 38400
TIMEOUT = 1

# Rozdziel tekst przy dwukropku
try:
    ser = serial.Serial(PORT, baudrate=BAUD_RATE, timeout=TIMEOUT)
except serial.SerialException as e:
    print(f"Error opening serial port: {e}")
    raise SystemExit(1)

try:
    while True:
        raw_data = ser.readline()

        if not raw_data:
            continue

        if not raw_data.endswith(b"\n"):
            print("Incomplete frame")
            continue

        try:
            line = raw_data.decode("utf-8").rstrip()

        except UnicodeDecodeError as e:
            print(f"Invalid encoding: {e}")
            continue



        if line:
            parts = line.split(":")
            if len(parts) != 2:
                print(f"Invalid data format: {line!r}")
            else:
                # Pobierz nazwę pomiaru
                sensor = parts[0]

                # Pobierz wartość pomiaru
                value = parts[1]
                measurement = None

                if sensor == "TEMP":
                    try:
                        temp = float(value)
                        measurement = temp
                        print(f"Temperature: {temp}")
                    except ValueError:
                        print("Invalid numeric value")
                elif sensor == "HUM":
                    try:
                        hum = float(value)
                        measurement = hum
                        print(f"Humidity: {hum}")
                    except ValueError:
                        print("Invalid numeric value")
                elif sensor == "PRESS":
                    try:
                        press = int(value)
                        measurement = press
                        print(f"Pressure: {press}")
                    except ValueError:
                        print("Invalid numeric value")
                else:
                        print(f"Unknown measurement type: {sensor!r}")
                        print(f"Received UART data: {line!r}")

                if measurement is not None and math.isfinite(measurement):
                    filename = "measurements.csv"
                
                    # Sprawdź, czy plik nie istnieje lub jest pusty
                    write_header = (
                        not os.path.exists(filename)
                        or os.path.getsize(filename) == 0
                    )
                
                    # Pobierz aktualną datę i godzinę
                    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
                
                    # Otwórz plik w trybie dopisywania
                    with open(filename, "a", newline="") as file:
                        writer = csv.writer(file)
                
                        # Zapisz nagłówek tylko dla nowego lub pustego pliku
                        if write_header:
                            writer.writerow(["timestamp", "sensor", "measurement"])
                
                        # Zapisz aktualny pomiar
                        writer.writerow([timestamp, sensor, measurement]) 

except KeyboardInterrupt:
    print("\nProgram stopped by user.")

finally:
    ser.close()

