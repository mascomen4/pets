//
// Created by pi on 1/10/23.
//
#include "../include/sockets_io.h"
#include <iostream>

#include "../include/Server.h"

ssize_t robust_read(ioResult_t * pResult, char * usrBuffer, size_t n) {
    int cnt;

    // fill the buffer if it's empty
    while (pResult->cntLeft <= 0) {
        pResult->cntLeft = static_cast<int>( read(pResult->fd, pResult->buffer, sizeof(pResult->buffer)));
        if (pResult->cntLeft < 0) {
            if (errno != EINTR) {
                return -1; // if it's not Unix interruption, return error
            }
        }
        else if (pResult->cntLeft == 0) {
            return 0; // EOF
        }
        else{
            pResult->pNextByte = pResult->buffer; // in case if read is success
        }
    }

    // process the client's request. Return min els between n and pResult->cntLeft
    cnt = static_cast<int>(n);
    if (pResult->cntLeft < n){
        cnt = pResult->cntLeft;
    }
    memcpy(usrBuffer, pResult->pNextByte, cnt);
    pResult->pNextByte += cnt;
    pResult->cntLeft -= cnt;
    return cnt;
}

/**
 * Wrapper function for robust_readline to get a more descriptive output
 * @param fd
 * @return
 */
HandleStatus readline_wrapper(int fd, std::string& line){
    int max_len = 50;
    ioResult_t io_buf(fd);
    int read_res = robust_readline(&io_buf, line, max_len);

    if (read_res == 0) return HandleStatus::disconnected;
    else if (read_res == -1) return HandleStatus::try_again;
    else return HandleStatus::ok;

}

/**
 *
 * @param pResult
 * @param line where to put the result
 * @param maxLen
 * @return
 */
int robust_readline(ioResult_t * pResult, std::string& line, size_t maxLen){
    int n, cntRead; // cntRead - number of already read bytes
    char c;
    for (n = 1; n < maxLen; ++n){
        if ((cntRead = static_cast<int>(robust_read(pResult, &c, 1)))  == 1){
            line += c;
            if ( c == '\n' ){
                ++n;
                break;
            }
        }
        else if (cntRead == 0){
            if (n == 1){
                return 0; // EOF, no data read before.
            }
            else{
                break; // EOF, some data was read.
            }
        }
        else{
            return -1; // In case of any other error, return -1;
        }
    }
    return n-1; // if success return the number of read chars
}

int robust_write(int fd, const std::string& line){
    size_t nleft = line.size();
    ssize_t nwritten;
    const char * bufp = line.data();
    while (nleft > 0){
        if ((nwritten = write(fd, bufp, nleft))<=0){
            if (errno == EINTR || errno == EAGAIN){
                nwritten = 0; // if the process is occupied, let's try again, but set nwritten = 0 before that.
            }
            else{
                std::cout << errno << std::endl;
                return -1;
            }
        }
        nleft -= nwritten;
        bufp += nwritten;
    }
    return static_cast<int>(line.size());
}