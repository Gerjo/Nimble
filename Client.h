#ifndef CLIENT_H
#define	CLIENT_H

#include <sys/epoll.h>
#include <string>
#include <sstream>
#include <pthread.h>


struct Client {

    Client(int newFd) {
        fd           = newFd;
        timeLastRead = 0;
        timeAccepted = time(0);
        c_headers    = 0;
        headerLength = 0;
        allocated    = 0;
        isClosed     = false;
        
    }

    ~Client() {
        if(c_headers != 0) {
            free(c_headers);
            c_headers = 0;
        }
    }
    
    int fd;
    epoll_event* event;
    time_t timeLastRead;
    time_t timeAccepted;

    char* c_headers;
    int headerLength;
    int allocated;

    bool isClosed;
};

#endif	/* CLIENT_H */
