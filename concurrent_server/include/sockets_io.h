//
// Created by pi on 1/10/23.
//

#ifndef BAUM_SOCKETS_IO_H
#define BAUM_SOCKETS_IO_H

#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include <cerrno>
#include <cstring>
#include <string>

static const int BUFSIZE = 8192;
enum class HandleStatus;

/**
 * Helper struct to buffer the response from the Unix read() function.
 * It's used by robust_read() which decides whether to read from the buffer of ioResult_t (which was filled before)
 * or use Unix read() and fill the buffer of ioResult_t
 */
struct ioResult_t{
    explicit ioResult_t(int connfd){
        fd = connfd;
        cntLeft = 0;
        pNextByte = buffer;
    }

    int fd; // file descriptor from where the data comes
    int cntLeft; // number of unread bytes
    char * pNextByte; // next byte to read
    char buffer[BUFSIZE]{};
};

HandleStatus readline_wrapper(int fd, std::string&);

/**
 * Handles the low level interaction with Unix read() function, using the structure defined above for buffering.
 * @param pResult Helper structure
 * @param usrBuffer Where to write the result
 * @param n The number of bytes to read
 * @return The number of bytes read
 */
ssize_t robust_read(ioResult_t * pResult, char * usrBuffer, size_t n);

/**
 * Read line char-by-char using robust_read() function.
 * @param pResult Helper structure needed by robust_read()
 * @param line Where to save the output
 * @param maxLen The maximum desired length to read
 * @return
 */
int robust_readline(ioResult_t * pResult, std::string& line, size_t maxLen);

/**
 * Write the string to the connected socket
 * @param fd - file descriptor where to write
 * @param line - string which to write
 * @return - status: if success, returns the number of bytes written
 */
int robust_write(int fd, const std::string& line);

#endif //BAUM_SOCKETS_IO_H
