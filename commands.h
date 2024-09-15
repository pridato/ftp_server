//
// Created by David Arroyo on 15/9/24.
//

#ifndef COMMANDS_H
#define COMMANDS_H

void handle_client(int client_socket);
void list_files(int client_socket);
void retrieve_file(int client_socket, const char *filename);
void store_file(int client_socket, const char *filename);

#endif //COMMANDS_H
