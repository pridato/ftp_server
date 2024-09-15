//
// Created by David Arroyo on 15/9/24.
//

#include "commands.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <openssl/ssl.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024 // Tamaño del búfer para recibir y enviar datos
#define FTP_DIR "ftp_files" // Directorio donde se almacenarán los archivos

/**
 * función que habilita el listado de comandos del cliente en el repos. actual
 * @param ssl
 */
void handle_client(SSL *ssl) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;
    while ((bytes_received = SSL_read(ssl, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_received] = '\0';
        printf("Mensaje del cliente: %s\n", buffer);

        if (strncmp(buffer, "LIST", 4) == 0) {
            list_files(ssl);
        } else if (strncmp(buffer, "RETR ", 5) == 0) {
            retrieve_file(ssl, buffer + 5);
        } else if (strncmp(buffer, "STOR ", 5) == 0) {
            store_file(ssl, buffer + 5);
        } else {
            const char *response = "Comando no reconocido\n";
            SSL_write(ssl, response, strlen(response));
        }
    }

    if (bytes_received < 0) {
        perror("recv");
    }
}

/**
 * Funcion para listar los archivos del servidor
 * @param ssl
 */
void list_files(SSL *ssl) {
    struct dirent *entry;

    DIR *dir = opendir(FTP_DIR);
    if (dir == NULL) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        // DT_REG significa un archivo regular
        if (entry->d_type == DT_REG) {
            char buffer[BUFFER_SIZE];
            snprintf(buffer, sizeof(buffer), "%s\n", entry->d_name);
            SSL_write(ssl, buffer, strlen(buffer));
        }
    }

    closedir(dir);
    const char *end_msg = "FIN_LISTA\n";
    SSL_write(ssl, end_msg, strlen(end_msg));
}

/**
 * funcion para enviar un archivo almacenado en el servidor al cliente que lo solicita
 * @param ssl
 * @param filename nombre del archivo que el cliente quiere descargar
 */
void retrieve_file(SSL *ssl, const char *filename) {
    char filepath[BUFFER_SIZE]; // buffer para almacenar la dirección entera del archivo
    snprintf(filepath, sizeof(filepath), "%s/%s", FTP_DIR, filename); // almacenamos la ruta entera con el nombre del archivo

    FILE *file = fopen(filepath, "rb"); // abrimos el archivo en modo binario de lectura
    if (file == NULL) {
        perror("fopen");
        const char *error_msg = "Error: archivo no encontrado\n";
        SSL_write(ssl, error_msg, strlen(error_msg));
        return;
    }

    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    // bucle que vamos guardando los bytes en bytes_read en bloques del tamaño del buffer, todos sacados del puntero del archivo
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        SSL_write(ssl, buffer, bytes_read);
    }
    fclose(file);
}


/**
 * funcion para alojar los archivos en el server
 * @param ssl
 * @param filename
 */
void store_file(SSL *ssl, const char *filename) {
    char filepath[BUFFER_SIZE];
    snprintf(filepath, sizeof(filepath), "%s/%s", FTP_DIR, filename); // filepath + filename poco más...

    FILE *file = fopen(filepath, "wb"); // puntero que abre el archivo en modo escritura en binario
    if (file == NULL) {
        perror("fopen");
        const char *error_msg = "Error: no se pudo crear el archivo\n";
        SSL_write(ssl, error_msg, strlen(error_msg));
        return;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;


    while ((bytes_received = SSL_read(ssl, buffer, sizeof(buffer) - 1)) > 0) {
        fwrite(buffer, 1, bytes_received, file);
    }

    if (bytes_received < 0) {
        perror("recv");
    }
    fclose(file);

}
