import socket
import threading
import time
import os
from datetime import datetime

#el servidor escucha en este puerto y a este host
host = "127.0.0.1"
port = 6667

#creo el socket
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
sock.bind((host, port))         #se engancha al puerto 6667
print("Socket bind completado")
sock.listen(5)                  #queda esperando cleintes
print("Socket en modo escucha - pasivo")

#variables importantes
clientes = []       #conexiones activas
clientes_info = {}  #info de cada cliente
apagado = threading.Event()     #bandera para apagar

carpeta_archivos = "archivos_recibidos"
os.makedirs(carpeta_archivos, exist_ok=True)


def mostrar_clientes():
    print("\nClientes conectados:")
    if len(clientes_info) == 0:
        print("No hay clientes conectados")
    else:
        for clave, datos in clientes_info.items():
            print(f"Cliente {clave} - conectado desde {datos['hora'].strftime('%d/%m/%Y %H:%M:%S')}")
    print()


def recibir_linea(conn):
    datos = b""
    while not datos.endswith(b"\n"):
        parte = conn.recv(1)
        if not parte:
            break
        datos += parte
    return datos.decode("utf-8").strip()


def proceso_hijo(conn, addr):
    print("Conexión con {}.".format(addr))
    clientes.append(conn)
    hora_conexion = datetime.now()
    clave_cliente = f"{addr[0]}:{addr[1]}"

    #guarda informacion del cliente
    clientes_info[clave_cliente] = {
        "ip": addr[0],
        "puerto": addr[1],
        "hora": hora_conexion
    }

    mostrar_clientes()  #muestro con la funcion mostrar

    try:
        conn.send("Servidor: Conectado con cliente\n".encode("utf-8"))

        #recibe nombre del archivo
        nombre_archivo = recibir_linea(conn)

        #recibe tamaño del archivo
        tamanio_archivo = int(recibir_linea(conn))

        print(f"Recibiendo archivo de {clave_cliente}")
        print(f"Nombre: {nombre_archivo}")
        print(f"Tamaño: {tamanio_archivo} bytes")

        ruta_destino = os.path.join(carpeta_archivos, "recibido_" + nombre_archivo)

        bytes_recibidos = 0

        with open(ruta_destino, "wb") as archivo:
            while bytes_recibidos < tamanio_archivo:
                datos = conn.recv(1024)

                if not datos:
                    break

                archivo.write(datos)
                bytes_recibidos += len(datos)

        print(f"Archivo recibido desde {clave_cliente}")
        print(f"Guardado como: {ruta_destino}")

        tiempo_conectado=datetime.now() - hora_conexion 

        respuesta = (
            "Archivo recibido correctamente.\n"
            f"Cliente: {clave_cliente}\n"
            f"Día y hora de conexión: {hora_conexion.strftime('%d/%m/%Y %H:%M:%S')}\n"
            f"Tiempo conectado: {tiempo_conectado}\n"
        )
        
        #responde al cliente: archivo+hora+tiempo
        conn.sendall(respuesta.encode("utf-8"))

    except Exception as e:
        print("Error con {}: {}".format(addr, e))

    finally:
        conn.close()
        clientes.remove(conn)

        if clave_cliente in clientes_info:
            del clientes_info[clave_cliente]

        print(f"Cliente desconectado: {clave_cliente}")
        mostrar_clientes()


def control_apagado():
    while not apagado.is_set():
        time.sleep(30)
        respuesta = input("\n¿Desea apagar el servidor? (s/n): ").strip().lower()
        if respuesta == "s":
            apagado.set()
            print("Apagando servidor...")
            break


threading.Thread(target=control_apagado, daemon=True).start()

try:
    while not apagado.is_set():
        sock.settimeout(1.0)
        try:
            #cuando entra el cliente
            conn, addr = sock.accept()      #canal de comunicacion , IP + puerto
            #se crea un hilo, cada cliente tiene su propio hilo y eso permite multiples clientes
            threading.Thread(target=proceso_hijo, args=(conn, addr)).start() 
        except socket.timeout:
            continue

finally:
    print("Cerrando todas las conexiones...")
    for c in clientes:
        c.close()
    sock.close()
    print("Servidor finalizado.")