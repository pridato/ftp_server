//
// Created by David Arroyo on 15/9/24.
//

#ifndef COMMANDS_H
#define COMMANDS_H
#include <openssl/types.h>

void handle_client(SSL *ssl);
void list_files(SSL *ssl);
void retrieve_file(SSL *ssl, const char *filename);
void store_file(SSL *ssl, const char *filename);

#endif //COMMANDS_H
