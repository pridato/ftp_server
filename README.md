# Servidor FTP en C

Este proyecto es una implementación básica de un servidor FTP con certificados TCP a través de sockets en C. Permite a los clientes conectarse, listar archivos disponibles, descargar archivos y subir archivos al servidor.

## Contenido

- `server.c`: Implementación del servidor FTP.
- `commands.c`: Implementación de las funciones para manejar comandos de FTP.
- `server.h`, `commands.h`, `utils.h`: Archivos de encabezado para el servidor y funciones.

## Requisitos

- Un sistema con un compilador de C (como `gcc`).

## Instalación

Para compilar y ejecutar el servidor FTP, sigue estos pasos:

1. **Clonar el repositorio:**

   ```bash
   git clone https://github.com/pridato/ftp_server.git
   cd ftp_server
   ```
  
2. **Compilar el proyecto:**
  ```bash
  gcc -o ftp_server server.c commands.c utils.c
  ```

3. **Crear el directorio de archivos:**
  ```bash
  mkdir ftp_files
  ```

**Inicializar servidor**
```bash 
./ftp_server
```


## Comandos soportados
  - LIST: Lista todos los archivos en el directorio ftp_files.
  - RETR <filename>: Descarga el archivo especificado al cliente.
  - STOR <filename>: Sube el archivo desde el cliente al servidor
