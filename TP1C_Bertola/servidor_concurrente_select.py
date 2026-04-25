import socket
import select
import os
from datetime import datetime

#no hay hilos --> importante

HOST = "127.0.0.1"
PORT = 6668
BUFFER = 1024

carpeta_archivos = "archivos_recibidos_select"
os.makedirs(carpeta_archivos, exist_ok=True)

servidor = socket.socket(socket.AF_INET, socket.SOCK_STREAM)       
servidor.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
servidor.setblocking(False)     #porque el servidor se encarga de avisar cuando se puede leer
servidor.bind((HOST, PORT))
servidor.listen(5)

#Concurrente: usa accept() + timeout
#Select: usa socket no bloqueante + select()


entradas = [servidor]
clientes = {}

print(f"Servidor SELECT escuchando en {HOST}:{PORT}")


def mostrar_clientes():
    print("\nClientes conectados:")
    if len(clientes) == 0:
        print("No hay clientes conectados")
    else:
        for sock_cliente, datos in clientes.items():
            print(
                f"Cliente {datos['ip']}:{datos['puerto']} - "
                f"conectado desde {datos['hora'].strftime('%d/%m/%Y %H:%M:%S')}"
            )
    print()


def cerrar_cliente(sock_cliente):
    if sock_cliente in clientes:
        datos = clientes[sock_cliente]
        print(f"Cliente desconectado: {datos['ip']}:{datos['puerto']}")
        del clientes[sock_cliente]

    if sock_cliente in entradas:
        entradas.remove(sock_cliente)

    sock_cliente.close()
    mostrar_clientes()


try:
    while True:
        #el servidor tiene una lista de sockets y va mirando cual esta listo para ser atendido
        readable, writable, exceptional = select.select(entradas, [], entradas, 1)

        for s in readable:

            if s is servidor:
                
                #lo acepta y agrega a la lista de entrada 
                conn, addr = servidor.accept()
                conn.setblocking(False)

                hora_conexion = datetime.now()

                #la clave es el socket mismo porque select() trabaja con sockets
                clientes[conn] = {
                    "ip": addr[0],
                    "puerto": addr[1],
                    "hora": hora_conexion,
                    "buffer": b"",
                    "estado": "metadata",
                    "nombre_archivo": "",
                    "tamanio": 0,
                    "recibidos": 0,
                    "archivo": None,
                    "ruta": ""
                }

                entradas.append(conn)

                print(f"Conexión con {addr}.")
                conn.sendall("Servidor SELECT: Conectado con cliente\n".encode("utf-8"))
                mostrar_clientes()

            else:
                datos = s.recv(BUFFER)

                if not datos:
                    cerrar_cliente(s)
                    continue

                cliente = clientes[s]
                cliente["buffer"] += datos

                if cliente["estado"] == "metadata":
                    if b"\n" in cliente["buffer"]:
                        nombre, resto = cliente["buffer"].split(b"\n", 1)
                        cliente["nombre_archivo"] = nombre.decode("utf-8").strip()
                        cliente["buffer"] = resto
                        cliente["estado"] = "tamanio"

                if cliente["estado"] == "tamanio":
                    if b"\n" in cliente["buffer"]:
                        tamanio, resto = cliente["buffer"].split(b"\n", 1)
                        cliente["tamanio"] = int(tamanio.decode("utf-8").strip())
                        cliente["buffer"] = resto

                        cliente["ruta"] = os.path.join(
                            carpeta_archivos,
                            "recibido_" + cliente["nombre_archivo"]
                        )

                        cliente["archivo"] = open(cliente["ruta"], "wb")
                        cliente["estado"] = "archivo"

                        print(f"Recibiendo archivo de {cliente['ip']}:{cliente['puerto']}")
                        print(f"Nombre: {cliente['nombre_archivo']}")
                        print(f"Tamaño: {cliente['tamanio']} bytes")

                if cliente["estado"] == "archivo":
                    faltan = cliente["tamanio"] - cliente["recibidos"]
                    escribir = cliente["buffer"][:faltan]

                    if escribir:
                        cliente["archivo"].write(escribir)
                        cliente["recibidos"] += len(escribir)
                        cliente["buffer"] = cliente["buffer"][len(escribir):]

                    if cliente["recibidos"] >= cliente["tamanio"]:
                        cliente["archivo"].close()

                        tiempo_conectado = datetime.now() - cliente["hora"]

                        respuesta = (
                            "Archivo recibido correctamente por servidor SELECT.\n"
                            f"Cliente: {cliente['ip']}:{cliente['puerto']}\n"
                            f"Día y hora de conexión: {cliente['hora'].strftime('%d/%m/%Y %H:%M:%S')}\n"
                            f"Tiempo conectado: {tiempo_conectado}\n"
                        )

                        s.sendall(respuesta.encode("utf-8"))

                        print(f"Archivo recibido desde {cliente['ip']}:{cliente['puerto']}")
                        print(f"Guardado como: {cliente['ruta']}")

                        cerrar_cliente(s)

        #se cierra con una funcion
        for s in exceptional:
            cerrar_cliente(s)
            
        #Concurrente: limpia dentro del hilo
        #Select: limpia sacando el socket de la lista de select

except KeyboardInterrupt:
    print("\nServidor SELECT detenido manualmente.")

finally:
    for s in entradas:
        s.close()
    servidor.close()
    print("Servidor SELECT finalizado.")