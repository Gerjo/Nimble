#ifndef ACCEPTER_H
#define	ACCEPTER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <iostream>

#include "ThreadManager.h"
#include "Stack.h"

using namespace std;

class Nimble;

class Accepter : public Runnable {
    public:
        Accepter(Nimble& nimble, int port);
        virtual ~Accepter();
        void run(void);

        void handleFdBuffer(void);

    private:
        ThreadStack<int> _fdBuffer;
        int _port;
        int _listenFd;
        Nimble& _nimble;

        void setup();
};

// Command pattern: (Design pattern or just a hack in this case?)
class HandleBufferCommand : public Runnable {
public:
    HandleBufferCommand(Accepter* accepter) {
        _accepter = accepter;
    }

    virtual void run(void) {
        _accepter->handleFdBuffer();

        // Kind of tricky ehh?
        delete this;
    }
private:
    Accepter* _accepter;
};

#endif	/* ACCEPTER_H */

