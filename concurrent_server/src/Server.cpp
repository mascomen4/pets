//
// Created by pi on 1/10/23.
//

#include "../include/Server.h"

// ---- NewHandlerSupport functions definition ----

template<class X>
std::new_handler NewHandlerSupport<X>::set_new_handler(std::new_handler p){
    std::new_handler old_handler = current_handler;
    current_handler = p;
    return old_handler;
};

template<class X>
std::new_handler NewHandlerSupport<X>::current_handler; // don't forget to initialize the default handler to nullptr


template<class X>
void* NewHandlerSupport<X>::operator new(size_t size) {
    std::new_handler global_handler = std::set_new_handler(current_handler);
    void * memory;
    try{
        memory = ::operator new(size); // calling global operator new
    }
    catch(std::bad_alloc&){ // ::new tries to allocate some memory calling new_handler. if it eventually fails, we throw
        // but getting back to the prev handler.
        std::cout << "Failed to allocate a memory, handler failed to handle the case as well. Aborting..." << std::endl;
        std::set_new_handler(global_handler);
        throw;
    }
    std::set_new_handler(global_handler);
    return memory;
}

// ---- ClientHandler functions definition ----

ClientHandler::~ClientHandler() {
    std::cout << "Client " << fd << " disconnected. Freeing resources..." << std::endl;
    close_fd(fd);
}

HandleStatus ClientHandler::handle(){
    HandleStatus read_res, write_res;

    switch (mode) {
        case ch_mode::reading:
            read_res = handle_reading();
            return read_res;
        case ch_mode::writing:
            write_res = handle_writing();
            return write_res;
    }
}

HandleStatus ClientHandler::handle_reading() {
    std::string line;
    HandleStatus read_res = readline_wrapper(fd, line);
    if (read_res == HandleStatus::try_again || read_res == HandleStatus::disconnected)
        return read_res;
    else{
        HandleStatus parse_res = parse_command(line);
        if (parse_res == HandleStatus::switch_mode){
            std::cout << "Changing mode to writing from listening on client " << fd << "..." << std::endl;
            mode = ch_mode::writing;
        }
        else if (parse_res == HandleStatus::try_again){
            robust_write(fd, "Error occurred parsing command. Please make sure the command is legit and try again...\n");
        }
        return parse_res; // if res is 1, the control thread will push it to the working_threads thread_pool
    }
}

HandleStatus ClientHandler::handle_writing(){
    update();
    std::stringstream ss;
    std::string output;

    if (std::all_of(seq_in_use.begin(), seq_in_use.end(), [](bool el){return !el;})){ // if all are false, don't do anything
        output = "There's nothing to show. Abandoning...\n";
        robust_write(fd, output);
        return HandleStatus::fatal_error; // abandon the client if there's nothing to show;
    }
    else{
        for (int i = 0; i < 3; ++i){ // pick only those which should be used.
            if (!seq_in_use[i]) continue;
            ss << std::setw(25) << seq[i];
        }
        ss << std::endl;
    }
    output = ss.str();
    int write_res = robust_write(fd, output);
    if (write_res == -1){
        return HandleStatus::disconnected; // if write was unsuccessful, probably the client is disconnected, so we return 0 and cause a destruction of an object
    }
    else{
        return HandleStatus::ok;
    }
}

unsigned long long ClientHandler::get_number_from_stream(std::stringstream& ss){
    std::string item;
    std::getline(ss, item, ' ');
    unsigned long long  value;
    if (item.size() > 6) {
        std::cerr << "Parsing client string: Too long number" << std::endl; // мы условились что максимум может быть 4 знака в числе, плюс 2 знака на пробел и перенос строки
        return -1;
    }
    try{
        value = static_cast<unsigned long long>(std::stoll(item));
    }
    catch(std::exception&){
        std::cerr << "Invalid input reading xxxx or yyyy" << std::endl;
        return -1;
    }
    return value;
}

HandleStatus ClientHandler::parse_command(const std::string& input){
    if (input.size() > 17) {
        return HandleStatus::try_again;
    }
    std::stringstream ss(input);
    std::string strcmp = "export seq";
    strcmp += (char)13;
    strcmp += (char)10;
    std::string item;
    int n;

    std::getline(ss, item, ' ');
    if ( item[0] == 's' && item[1] == 'e' && item[2] == 'q' ){
        if ( item[3] == '1' || item[3] == '2' || item[3] == '3' ){
            n = static_cast<int>(item[3] - '1');
            auto init_value = get_number_from_stream(ss);
            if (init_value == -1) return HandleStatus::try_again;

            auto stepn = get_number_from_stream(ss);
            if (stepn == -1) return HandleStatus::try_again;

            if (init_value == 0 || stepn == 0) seq_in_use[n] = false; // if either is zero, don't use the sequence
            seq[n] = init_value;
            step[n] = stepn;
            return HandleStatus::ok; // indicates that the command was read
        }
        else{
            return HandleStatus::try_again;
        }
    }
    else if(ss.str() == strcmp){
        return HandleStatus::switch_mode; // indicates that you need to start sending.
    }
    else{
        return HandleStatus::try_again;
    }
}

void ClientHandler::update(){
    for (int i = 0; i < 3; ++i){
        if (!seq_in_use[i]) continue;
        if ( (std::numeric_limits<unsigned long long>::max() - seq[i]) < step[i] ) { // переполнение счетчика
            seq[i] = inits[i];
            continue;
        }
        seq[i] += step[i];
    }
}

// ---- ThreadPoolServer functions definition ----

void ThreadPoolServer::accept_connections(){
    int connfd;
    socklen_t clientlen{};
    sockaddr_storage clientaddr{};
    char client_hostname[MAXLINE], client_port[MAXLINE];

    while(true) {
        clientlen = sizeof(sockaddr_storage);
        connfd = accept_connection(listening_fd, (sockaddr *) & clientaddr, &clientlen);
        get_name_info((sockaddr *) & clientaddr, clientlen, client_hostname, MAXLINE,
                      client_port, MAXLINE, 0);
        printf("Connected to (%s,%s) \n", client_hostname, client_port);

        // We wrote here the try/catch solution, but we could also use the functionality of NewHandlerSupport
        // to allocate some memory at a program startup, and free it later.
        try{
            std::shared_ptr<Handler> ch = std::make_shared<ClientHandler>(connfd); // spawn new ClientHandler
            working_threads.submit(std::move(ch)); // Add this ClientHandler to the pool
        }
        catch(std::bad_alloc&){
            robust_write(connfd, "Sorry, the server is full now, try again later.");
            close_fd(connfd);
            continue;
        }
    }
}