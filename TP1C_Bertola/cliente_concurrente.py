import socket
import os

#puerto donde lo espera el servidor
HOST = "127.0.0.1"
PORT = 6667

try:
    print("Paso 0: Creando socket...")
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(10)

    print(f"Paso 1: Conectando a servidor en {HOST}:{PORT}...")
    sock.connect((HOST, PORT))

    bienvenida = sock.recv(1024)
    print("Paso 2: Mensaje del servidor:", bienvenida.decode("utf-8"))

    print("Paso 3: Ingresar archivo a enviar.")
    ruta_archivo = input("Ingrese el nombre o ruta del archivo: ")

    if not os.path.exists(ruta_archivo):
        print("El archivo no existe.")
    else:
        nombre_archivo = os.path.basename(ruta_archivo)
        tamanio_archivo = os.path.getsize(ruta_archivo)

        print("Paso 4: Enviando nombre del archivo...")
        sock.sendall((nombre_archivo + "\n").encode("utf-8"))         #envia el nombre del archivo

        print("Paso 5: Enviando tamaño del archivo...")
        sock.sendall((str(tamanio_archivo) + "\n").encode("utf-8"))    #envia el tamaño del archivo

        print("Paso 6: Enviando contenido del archivo...")
        #envia el contenido del archivo
        with open(ruta_archivo, "rb") as archivo:
            while True:
                datos = archivo.read(1024)

                if not datos:
                    break

                sock.sendall(datos)

        print("Paso 7: Archivo enviado. Esperando respuesta...")
        
        #espera la respuesta con el servidor
        respuesta = sock.recv(4096)
        print("Respuesta del servidor:")
        print(respuesta.decode("utf-8"))

except ConnectionRefusedError:
    print("No se pudo conectar al servidor. Verifique si está activo.")
except socket.timeout:
    print("Tiempo de espera agotado.")
except Exception as e:
    print(f"Ocurrió un error: {e}")

#opcion de pregunta punto 6
finally:
    input("Presione ENTER para cerrar el cliente y liberar recursos...")
    sock.close()
    print("Paso 8: Socket cerrado. Cliente finalizado.")