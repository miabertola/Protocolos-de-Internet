import sqlite3
import random
import time

OPENWEATHER_API_KEY = "a9419f344848e87145ad886c36cc6497"

# Conexion a SQLite
conexion = sqlite3.connect("sensores.db")
cursor = conexion.cursor()

# Configuracion
cantidad = int(input("Cantidad de capturas: "))
intervalo = int(input("Intervalo entre mediciones (segundos): "))

for i in range(cantidad):

    # Generar valores aleatorios
    co2 = random.randint(400, 1200)
    temperatura = round(random.uniform(18, 32), 2)
    humedad = round(random.uniform(30, 80), 2)

    # Insertar CO2
    cursor.execute("""
    INSERT INTO sensores (sensor, valor, unidad)
    VALUES (?, ?, ?)
    """, ("CO2", co2, "ppm"))

    # Insertar temperatura
    cursor.execute("""
    INSERT INTO sensores (sensor, valor, unidad)
    VALUES (?, ?, ?)
    """, ("Temperatura", temperatura, "°C"))

    # Insertar humedad
    cursor.execute("""
    INSERT INTO sensores (sensor, valor, unidad)
    VALUES (?, ?, ?)
    """, ("Humedad", humedad, "%"))

    conexion.commit()

    print(f"\nCaptura {i+1}")
    print(f"CO2: {co2} ppm")
    print(f"Temperatura: {temperatura} °C")
    print(f"Humedad: {humedad} %")

    time.sleep(intervalo)

conexion.close()

print("\nSimulación finalizada")
