//
// Created by David Arroyo on 15/9/24.
//
#include "server.h"
#include "commands.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>

#define PORT 21
#define BUFFER_SIZE 1024 // Tamaño del búfer para recibir y enviar datos

/**
 * Función para la inicialización del servidor
 */
void start_server() {
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;

    const int server_socket = generate_socket();

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // escucha desde cualquier interfaz
    server_addr.sin_port = htons(PORT); // convertir el puerto de HOST, el normal a formato de red, convierte los bits

    // una vez creados el socket y el servidor, los asociamos
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("bind");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) < 0) {
        perror("listen");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Servidor FTP escuchando en el puerto %d...\n", PORT);

    client_len = sizeof(client_addr);
    const int client_socket = accept(server_socket, (struct sockaddr *) &client_addr, &client_len);
    if (client_socket < 0) {
        perror("accept");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Cliente conectado\n");

    handle_client(client_socket);

    printf("Cliente desconectado\n");

    close(client_socket);
    close(server_socket);

}

/**
 * Función para generar un socket
 * @return número generado por el socket
 */
int generate_socket() {
    // AF_INET indica que se trabajará bajo una ipv4 y sock_stream indica que el socket será de tipo "flujo", el que usa TCP
    const int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    return server_socket;
}