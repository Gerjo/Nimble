#include "Accepter.h"
#include "Nimble.h"

//#define USEBUFFER

Accepter::Accepter(Nimble& nimble, int port) : _nimble(nimble) {
    _port       = port;
    _listenFd   = -1;
    setup();

    #ifdef USEBUFFER
        ThreadManager::Worker(new HandleBufferCommand(this), "HandleBufferCommand");
    #endif
}

Accepter::~Accepter() {
    if(_listenFd != -1) {
        ::close(_listenFd);
    }
}

void Accepter::handleFdBuffer(void) {
    while(1) {
        int fd = _fdBuffer.pop();
        _nimble.newConnection(fd);
        /*
        if(fd != 0) {
            _nimble.newConnection(fd);
        } else {
            usleep(1000);
        }*/
    }
}

void Accepter::run(void) {
    if(_listenFd != -1) {

        cout << "Ready to accept new clients on port: #" << _port << endl;

        sockaddr_in cli_addr;
        socklen_t clilen = sizeof(cli_addr);

        while(1) {
            int newsockfd   = ::accept(_listenFd, (sockaddr *) &cli_addr, &clilen);

            #ifdef USEBUFFER
                _fdBuffer.push(newsockfd);
            #else
                _nimble.newConnection(newsockfd);
            #endif
        }
    } else {
        throw string("Cannot run accepter thread as it's not setup yet. Call setup() first.");
    }
}

void Accepter::setup() {
    sockaddr_in serv_addr;

    _listenFd = ::socket(AF_INET, SOCK_STREAM, 0);

    if (_listenFd < 0) {
        cout << "unable to open socket." << endl;
    }

    int yes = 1;
    int no  = 1;

    if((::setsockopt(_listenFd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1 ) || // prevents the "port in use" err.
        (::setsockopt(_listenFd, SOL_SOCKET, SO_KEEPALIVE, &no, sizeof(int)) == -1 )){

        cout << "Error setting option :(\n";
    }

    serv_addr.sin_family        = AF_INET;
    serv_addr.sin_addr.s_addr   = INADDR_ANY; // TODO: variable.
    serv_addr.sin_port          = htons(_port);

    if (::bind(_listenFd, (sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        cout << "ERROR on binding. Perhaps port is taken? Port tried:" << _port << endl;
    }

    if (::listen(_listenFd, 10) < 0) {
        cout << "ERROR unable to listen on port" << _port << endl;
    }
}

