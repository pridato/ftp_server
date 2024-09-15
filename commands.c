//
// Created by David Arroyo on 15/9/24.
//

#include "commands.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/socket.h>

#define BUFFER_SIZE 1024 // Tamaño del búfer para recibir y enviar datos
#define FTP_DIR "ftp_files" // Directorio donde se almacenarán los archivos

/**
 * función que habilita el listado de comandos del cliente en el repos. actual
 * @param client_socket número del cliente conectado por socket
 */
void handle_client(int client_socket) {
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;
    while ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        printf("Mensaje del cliente: %s\n", buffer);

        if (strncmp(buffer, "LIST", 4) == 0) {
            list_files(client_socket);
        } else if (strncmp(buffer, "RETR ", 5) == 0) {
            retrieve_file(client_socket, buffer + 5);
        } else if (strncmp(buffer, "STOR ", 5) == 0) {
            store_file(client_socket, buffer + 5);
        } else {
            const char *response = "Comando no reconocido\n";
            send(client_socket, response, strlen(response), 0);
        }
    }

    if (bytes_received < 0) {
        perror("recv");
    }
}

/**
 * Funcion para listar los archivos del servidor
 * @param client_socket
 */
void list_files(const int client_socket) {
    struct dirent *entry; // representa una entrada a un directorio

    DIR *dir = opendir(FTP_DIR); // creamos un puntero, una dirección de la memoria donde está alojada la variable el directorio
    if (dir == NULL) {
        perror("opendir");
        return;
    }

    // leemos directorios hasta que sean null
    while ((entry = readdir(dir)) != NULL) {
        // DT_REG significa un archivo regular
        if (entry->d_type == DT_REG) {
            char buffer[BUFFER_SIZE]; // buffer por defecto para enviar el nombre
            snprintf(buffer, sizeof(buffer), "%s\n", entry->d_name);
            send(client_socket, buffer, strlen(buffer), 0); // lo enviamos por socket
        }
    }

    closedir(dir);
    const char *end_msg = "FIN_LISTA\n";
    send(client_socket, end_msg, strlen(end_msg), 0);
}

/**
 * funcion para enviar un archivo almacenado en el servidor al cliente que lo solicita
 * @param client_socket descriptor del socket id del cliente
 * @param filename nombre del archivo que el cliente quiere descargar
 */
void retrieve_file(int client_socket, const char *filename) {
    char filepath[BUFFER_SIZE]; // buffer para almacenar la dirección entera del archivo
    snprintf(filepath, sizeof(filepath), "%s/%s", FTP_DIR, filename); // almacenamos la ruta entera con el nombre del archivo

    FILE *file = fopen(filepath, "rb"); // abrimos el archivo en modo binario de lectura
    if (file == NULL) {
        perror("fopen");
        const char *error_msg = "Error: archivo no encontrado\n";
        send(client_socket, error_msg, strlen(error_msg), 0);
        return;
    }

    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    // bucle que vamos guardando los bytes en bytes_read en bloques del tamaño del buffer, todos sacados del puntero del archivo
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        send(client_socket, buffer, bytes_read, 0);
    }
    fclose(file);
}


/**
 * funcion para alojar los archivos en el server
 * @param client_socket
 * @param filename
 */
void store_file(const int client_socket, const char *filename) {
    char filepath[BUFFER_SIZE];
    snprintf(filepath, sizeof(filepath), "%s/%s", FTP_DIR, filename); // filepath + filename poco más...

    FILE *file = fopen(filepath, "wb"); // puntero que abre el archivo en modo escritura en binario
    if (file == NULL) {
        perror("fopen");
        const char *error_msg = "Error: no se pudo crear el archivo\n";
        send(client_socket, error_msg, strlen(error_msg), 0);
        return;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    while ((bytes_received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        fwrite(buffer, 1, bytes_received, file);
    }

    if (bytes_received < 0) {
        perror("recv");
    }
    fclose(file);

}
