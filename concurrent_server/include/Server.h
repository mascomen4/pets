//
// Created by pi on 1/10/23.
//

#ifndef BAUM_SERVER_H
#define BAUM_SERVER_H

#include <sstream>
#include <iomanip>
#include <algorithm>
#include <list>
#include <unordered_map>

#include "net.h"
#include "sockets_io.h"
#include "concurrency_utils.h"

static const int MAXLINE = 256;

/**
 * ---- The design explanation ----
 * 1. For each Client, Server creates a ClientHandler object and associates it with the client
 * 2. Then a server pushes the pointer to ClientHandler to the threadsafe_queue.
 * 3. A thread_pool object then takes those pointers, calls the doTask() function and pushes/ignores pushing (depending
 * on the output of the doTask() function.
 * 4. ClientHandler frees the allocated resources upon destruction. There's no copy ctors and and copy assignments
 * operations defined for Server and ClientHandler. Copy/move the smart pointers to ClientHandler and Server rather than
 * the objects themselves.
 * 5. Server is able to define an optional new_handler to handle the cases when there's no enough memory for new Clients
 * this behaviour is obtained by the use of NewHandlerSupport class. User can use them to define their own handler functions
 *
 * Note: thread_pool object support arbitrary number of users, which can be much more than the number of available threads.
 *
 * ---- Important ----
 * 1. The maximum number the user can write to the server is 4 digits number as defined in the text of the task: xxxx or yyyy
 * 2. Go to concurrency_utils.cpp and change SEND_WITH_INTERRUPT to true if you want to make the sending process slower
 * 3. If the program cannot instantiate the number of thread defined by the hardware, it exits with throw.
 */

template<class X>
class NewHandlerSupport{
public:
    static std::new_handler set_new_handler(std::new_handler p); // function should exist even if the object don't
    static void* operator new(size_t size); // allocator should exist before the object is constructed
private:
    static std::new_handler current_handler;
};

enum class HandleStatus{
    disconnected = -2,
    fatal_error = -1,
    ok = 0,
    try_again = 1,
    switch_mode = 2
};

class Handler{
public:
    explicit Handler(int fd): fd(fd) {};
    virtual HandleStatus handle() = 0;
    virtual ~Handler() = default;

protected:
    int fd;
};

class ClientHandler: public Handler{
public:
    explicit ClientHandler(int connfd): Handler(connfd) {}

    // close the client upon destruction
    ~ClientHandler() override;

    ClientHandler(const ClientHandler&) = delete;
    ClientHandler& operator=(const ClientHandler&) = delete;

    /**
     * The function used in the loop which either reads clients information, either sends the seqs data
     * @return the status. Return 0 means client disconnections, or other type of error out of our scope.
     */
    HandleStatus handle() override;

private:
    /**
     * The funtion which is used by handle() in the writing mode.
     * @return
     */
    HandleStatus handle_writing();

    /**
     * Helper function to avoid code copy.
     * @param ss stream from where to read
     * @return the integer result which was read from the stream
     */
    static unsigned long long get_number_from_stream(std::stringstream& ss);

    HandleStatus handle_reading();

    /**
     * The function to handle the incoming string.
     * @param input
     * @return -1 means the read unsuccessful, the control thread will try to read again later, item.size() means the
     * read is successful, and the number of processed chars is returned, 1 means command to start sending seqs was send.
     *
     * Note: Command: seq1 1 2 3 4 will be truncated to seq1 1 2 automatically. */
    HandleStatus parse_command(const std::string& input);

    /**
     * Function for updating the current counter of the sequences to output to the client.
     */
    void update();

    enum class ch_mode {
        reading = 0,
        writing = 1,
    };
    ch_mode mode = ch_mode::reading;
    std::array<unsigned long long, 3> seq{0, 0, 0};
    std::array<unsigned long long, 3> step{1, 1, 1};
    std::array<bool, 3> seq_in_use{true, true, true};
    std::array<unsigned long long, 3> inits{0, 0, 0};
};

class Server{
public:
    explicit Server(const char *port): port(port), ip(), listening_fd(), connected_fds() {
        listening_fd = open_listen_fd(port, nullptr);
        if (listening_fd == -1){
            throw std::runtime_error(std::string("Could not connect with the current configuration. Exiting..."));
        }
    };
    Server(const char *port, const char *ip): port(port), ip(ip), listening_fd(), connected_fds() {
        listening_fd = open_listen_fd(port, ip);
        if (listening_fd == -1){
            throw std::runtime_error(std::string("Could not connect with the given IP, PORT. Exiting..."));
        }
    };

    Server& operator=(const Server&) = delete;
    Server(Server&) = delete;
    virtual ~Server(){ // dtors are implicitly inline
        close_fd(listening_fd);
    }

    virtual int listening_file_descriptor(){
        return listening_fd;
    }
    virtual std::list<int>& connected_file_descriptors(){
        return connected_fds;
    }

    virtual void accept_connections() = 0; // require user to redefine this function

protected:
    int listening_fd; // support fast removal in the middle: search + removal total O(N) time, much better than vector.
    std::list<int> connected_fds; // mapping listening_fd -> connected_fds
    std::string port, ip;

    virtual void remove_disconnected_client(int fd){
        auto it = std::find(connected_fds.begin(), connected_fds.end(), fd);
        connected_fds.erase(it);
    }
};

class ThreadPoolServer final: public Server, public NewHandlerSupport<Server> {
public:
    explicit ThreadPoolServer(const char *port): Server(port) {}
    ThreadPoolServer(const char *port, const char *ip): Server(port, ip) {}

    /**
     * Function attaches to the listening sockets opened at the Object construction, and waits for new connections.
     */
    void accept_connections() override;

private:
    thread_pool working_threads;

    /**
     * We could also implement the push/pop of the connected clients for example by passing the pointer to the Server to
     * each ClientHandler, and their functions handle_writing(), handle_reading() could be made friends of ThreadPoolServer
     * and they would call the private function remove_client() each time the ClientHandler is destructed. But it's out of
     * our scope.
     */
    void remove_client();
};

#endif //BAUM_SERVER_H
