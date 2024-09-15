//
// Created by David Arroyo on 15/9/24.
//
#include "server.h"
#include "commands.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>


#define PORT 21
#define BUFFER_SIZE 1024 // Tamaño del búfer para recibir y enviar datos

/**
 * funcion para crear un contexto de ssl
 * @return puntero con información de las keys del servidor
 */
SSL_CTX *initialize_ssl_context() {
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();

    // Crear un contexto SSL
    const SSL_METHOD *method = TLS_server_method();
    SSL_CTX *ctx = SSL_CTX_new(method);
    if (ctx == NULL) {
        ERR_print_errors_fp(stderr);
        abort();
    }

    // Configurar la clave y el certificado en el ctx
    if (SSL_CTX_use_certificate_file(ctx, "server.crt", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        abort();
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, "server.key", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        abort();
    }

    return ctx;
}

/**
 * Función para la inicialización del servidor
 */
void start_server() {
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len;

    SSL_CTX *ctx = initialize_ssl_context();
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

    SSL *ssl = SSL_new(ctx);
    SSL_set_fd(ssl, client_socket);

    // Realizar el handshake SSL
    if (SSL_accept(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
    } else {
        handle_client(ssl);
    }

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