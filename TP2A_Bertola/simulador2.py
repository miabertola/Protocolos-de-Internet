import sqlite3
import random
import time
import requests
import geocoder

OPENWEATHER_API_KEY = "a9419f344848e87145ad886c36cc6497"


def geo_latlon():

    g = geocoder.ip("me")

    if not g.latlng:
        raise Exception("No se pudo obtener la ubicación")

    lat, lon = g.latlng

    url = (
        "https://api.openweathermap.org/data/2.5/weather?"
        f"lat={lat}&lon={lon}"
        f"&appid={OPENWEATHER_API_KEY}"
        "&units=metric"
        "&lang=es"
    )

    response = requests.get(url, timeout=10)
    data = response.json()

    main = data["main"]
    weather = data["weather"][0]

    return {
        "temp_ext": main["temp"],
        "presion": main["pressure"],
        "humedad_ext": main["humidity"],
        "descripcion": weather["description"]
    }


conexion = sqlite3.connect("sensores.db")
cursor = conexion.cursor()

cantidad = int(input("Cantidad de capturas: "))
intervalo = int(input("Intervalo entre mediciones: "))

for i in range(cantidad):

    # Sensores internos
    temp_int = round(random.uniform(18, 30), 2)
    humedad_int = round(random.uniform(30, 80), 2)
    co2 = random.randint(400, 1200)

    # Clima externo
    clima = geo_latlon()

    datos = [
        ("CO2", co2, "ppm"),
        ("Temp Interna", temp_int, "°C"),
        ("Humedad Interna", humedad_int, "%"),
        ("Temp Externa", clima["temp_ext"], "°C"),
        ("Presion", clima["presion"], "hPa"),
        ("Humedad Externa", clima["humedad_ext"], "%")
    ]

    for sensor, valor, unidad in datos:

        cursor.execute("""
        INSERT INTO sensores (sensor, valor, unidad)
        VALUES (?, ?, ?)
        """, (sensor, valor, unidad))

    conexion.commit()

    print(f"\nCaptura {i+1}")
    print("CO2:", co2)
    print("Temp interna:", temp_int)
    print("Humedad interna:", humedad_int)
    print("Clima exterior:", clima["descripcion"])

    time.sleep(intervalo)

conexion.close()

