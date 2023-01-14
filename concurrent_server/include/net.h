//
// Created by pi on 1/10/23.
//

#ifndef BAUM_NET_H
#define BAUM_NET_H

#include <netdb.h>
#include <unistd.h>

#include <iostream>

static const int LISTENQ = 8; // max number of clients to wait connection in the queue for Unix listen() function

void print_gai_error(int code, const std::string& msg);

void print_error(const std::string& msg);

void close_fd(int fd);

/**
 * Given the sockaddr structure, the function fills the strings hostname and port (serv param)
 * @param sa - sockaddr structure related to the predefined socket
 * @param salen - sockaddr length defined with socklen_t
 * @param host - hostname to be filled
 * @param hostlen - how much of the hostname to fill
 * @param serv - port to be filled
 * @param servlen - how much of the port to be filled
 * @param flags
 */
void get_name_info(const struct sockaddr *sa, socklen_t salen, char *host,
                 size_t hostlen, char *serv, size_t servlen, int flags);

/**
 * Sets the socket options. In this application it's used to get the server up and running right ahead.
 * @param s - file descriptor
 * @param level
 * @param optname
 * @param optval
 * @param optlen
 */
void set_sock_opt(int s, int level, int optname, const void *optval, int optlen);

/**
 * Wrapper of the Unix function accept()
 * @param s - listening file descriptor
 * @param addr - sockaddr structure
 * @param addrlen - length of the sockaddr structure
 * @return returns the file descriptor of the connected user.
 */
int accept_connection(int s, struct sockaddr *addr, socklen_t *addrlen);


/**
 * Function to open the listening socket for the server. The function gets a list of available addresses from
 * getaddrinfo() and iteratively tries to connect to either of. As soon as the connection is established, the
 * loop is stopped.
 * @param port
 * @return
 */
int open_listen_fd(const char *port, const char *ip = nullptr);

#endif //BAUM_NET_H
