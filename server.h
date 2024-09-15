//
// Created by David Arroyo on 15/9/24.
//

#ifndef SERVER_H
#define SERVER_H
#include <openssl/types.h>

void start_server();
int generate_socket();
SSL_CTX *initialize_ssl_context();

#endif //SERVER_H
