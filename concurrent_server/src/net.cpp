//
// Created by pi on 1/10/23.
//

#include "../include/net.h"

void print_error_with_code(int code, const std::string& msg){
    std::cerr << msg << code << " Exiting...." << std::endl;
    throw;
}

void print_error(const std::string& msg){
    std::cerr << msg << " Exiting..." << std::endl;
    throw;
}

void close_fd(int fd){
    int res;
    if ((res = close(fd)) < 0){
        print_error("Close error.");
    }
}

void get_name_info(const struct sockaddr *sa, socklen_t salen, char *host,
                   size_t hostlen, char *serv, size_t servlen, int flags){
    int res;
    if ((res = getnameinfo(sa, salen, host, hostlen, serv,
                           servlen, flags)) != 0)
        print_error_with_code(res, "Getnameinfo error");
}

void set_sock_opt(int s, int level, int optname, const void *optval, int optlen){
    int res;
    if ((res = setsockopt(s, level, optname, optval, optlen)) < 0)
        print_error_with_code(res, "Setsockopt error number: ");
}

int accept_connection(int s, struct sockaddr *addr, socklen_t *addrlen){
    int res;
    if ((res = accept4(s, addr, addrlen, SOCK_NONBLOCK)) < 0) // set the socket to be non-blocking so we can handle multiple clients.
        print_error("Accept error");
    return res;
}

/**
 * We make the ip address string an optional parameter, the only required argument is port.
 * @param port required param
 * @param ip has a default parameter
 * @return file descriptor integer if success, -1 otherwise
 */
int open_listen_fd(const char *port, const char * ip) {
    if (!ip) ip = nullptr;
    addrinfo * candidates, * candidate, hints{};
    int listenfd, optVal = 1;

    // getaddrinfo() can return 3 sockets: for connections, for raw sockets and for datagrams, we are asking to
    // return only the socket for connection
    hints.ai_socktype = SOCK_STREAM; // use streams, not diagrams i.e. data is guaranteed to arrive in the order
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG; // the socket will be used for listening | use systems IPv config
    hints.ai_flags |= AI_NUMERICSERV; // represent port as numbers, not string

    int res;
    if ((res = getaddrinfo(ip, port, &hints, &candidates )) != 0){
        print_error_with_code(res, "Getaddrinfo error with number ");
    }
    for (candidate = candidates; candidate; candidate = candidate->ai_next){
        if ((listenfd = socket(candidate->ai_family, candidate->ai_socktype, candidate->ai_protocol)) < 0){
            continue;
        }
        // SO_REUSEADDR - let the program reuse ports (by default the restriction is that we can't use same ports for same clients)
        // SOL_SOCKET - use the protocol configuration of the socket (TCP in our case)
        set_sock_opt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void*)&optVal, sizeof(int));
        if ( bind(listenfd, candidate->ai_addr, candidate->ai_addrlen) == 0 ){
            break;
        }
        else{
            close_fd(listenfd);
        }
    }
    if (!candidate){
        return -1;
    }

    if (listen(listenfd, LISTENQ) < 0){
        close_fd(listenfd);
        return -1;
    }

    return listenfd;
}