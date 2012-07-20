#include <list>

#include "Nimble.h"
#include "Accepter.h"
#include "Worker.h"

Nimble::Nimble() {
    _epollFd = -1;

    // TODO: scale on demand:
    _pool.reserve(100000);
    for(int i = 0; i < 100000; ++i) {
        _pool[i] = 0;
    }

    pthread_mutex_init(&mutex, NULL);

    // TODO: not so hardcoded:
    _settingsManager.addHostFromFile("/home/gerjo/nimble/hosts.json");

    Hosts& hosts = _settingsManager.getHosts();
    Hosts::iterator itHost;

    for(itHost = hosts.begin(); itHost != hosts.end(); ++itHost) {
        ThreadManager::Register(new Accepter(*this, (*itHost)->getPort()));
    }

    ThreadManager::Register(&_logger);

    for(int i = 0; i < 2; ++i) {
        ThreadManager::Register(new Worker(*this));
    }
    
    startPollLoop();
}

void Nimble::setNonBlocking(int fd) {
    int no    = 0;
    int flags = fcntl(fd, F_GETFL, &no);
    
    if(flags == -1) {
        perror ("Unable to set nonblocking (1)");
        return;
    }

    flags |= O_NONBLOCK;

    int s = fcntl(fd, F_SETFL, flags);

    if(s == -1) {
        perror("Unable to set nonblocking (2)");
        return;
    }
}

void Nimble::newConnection(int newFd) {
    setNonBlocking(newFd);

    bool yes = true;

    // http://en.wikipedia.org/wiki/Nagle's_algorithm
    if(ioctl(newFd, FIONBIO, &yes) == -1) {
        cout << "ioctl setting FIONBIO to true failed :(" << endl;
    }

    Client* client;

    if(_pool[newFd] == 0) {
        _pool[newFd] = new Client(newFd);
        client = _pool[newFd];
    } else {
        client = _pool[newFd];

        if(!client->isClosed) {
            cout << "Warning: recycling open client. Fd [" << client->fd << "] " << endl;
        }

        // Free excessive amounts of data?
        if(client->c_headers != 0) {
            //free(client->c_headers);
            //client->allocated = 0;
            //client->c_headers = 0;
        }

        client->isClosed     = false;
        client->headerLength = 0;
    }


    epoll_event readEvent;
    bzero(&readEvent, sizeof(epoll_event));
    readEvent.data.ptr = client;

    client->event = &readEvent;

    // EPOLLET = edge triggered (ergo: not level triggered)
    readEvent.events  = EPOLLIN | EPOLLET; // EPOLLIN | EPOLLPRI | EPOLLERR | EPOLLHUP

    //readEvent.events |= EPOLLONESHOT;
    
    // Attempt to add the event:
    if(::epoll_ctl(_epollFd, EPOLL_CTL_ADD, newFd, &readEvent) == -1) {
        perror("Unable to nadd ewFD");
    }
}

void Nimble::closeConnection(Client* client) {

    if(client->isClosed) {
        cout << "already closed" << endl;
        return;
    }

    ::epoll_ctl(_epollFd, EPOLL_CTL_DEL, client->fd, client->event);
    ::close(client->fd);

    client->isClosed = true;
}

void Nimble::startPollLoop(void) {
     // Create the epoll FD:
    _epollFd = ::epoll_create1(0);
    if(_epollFd == -1) {
        cout << "Unable to create epoll instance." << endl;
        return;
    }

    const int MAXEVENTS = 64;

    epoll_event* newEvents = new epoll_event[MAXEVENTS];
    int pollGeneration = 0;

    while(1) {
        // Blocking call, will wait until an event occurs:
        int eventCount = ::epoll_wait (_epollFd, newEvents, MAXEVENTS, -1);

        pollGeneration++;

        // Iterate over all events:
        for(int i = 0; i < eventCount; ++i) {
            epoll_event& event = newEvents[i];
            Client* client = (Client*) event.data.ptr;

            // Most scripts include this, I'm actually unsure why.
            if ((event.events & EPOLLERR) || (event.events & EPOLLHUP) || (!(event.events & EPOLLIN))) {
                ::close(client->fd);
                continue;
            }

            _stack.push(client);
        }

    }
}

const Logger& Nimble::getLogger(void) const {
    return _logger;
}

const int& Nimble::getEpoll(void) const {
    return _epollFd;
}

void Nimble::setSettingsManager(SettingsManager _settingsManager) {
    this->_settingsManager = _settingsManager;
}

const SettingsManager& Nimble::getSettingsManager() const {
    return _settingsManager;
}
