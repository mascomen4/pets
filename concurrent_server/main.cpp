#include "include/Server.h"
#include <csignal>

using namespace std;

const char * PORT = "1234";
const char * IP = "127.0.1.1";

int main() {
    signal(SIGPIPE, SIG_IGN); // ignore the sigpipe, the class with handle the case of disconnection
    ThreadPoolServer server(PORT, IP);
    server.accept_connections();
    return 0;
}