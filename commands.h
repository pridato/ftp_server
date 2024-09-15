//
// Created by David Arroyo on 15/9/24.
//

#ifndef COMMANDS_H
#define COMMANDS_H

void handle_list(int client_socket);
void handle_retr(int client_socket, const char *filename);
void handle_stor(int client_socket, const char *filename);

#endif //COMMANDS_H
