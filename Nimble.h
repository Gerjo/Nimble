#ifndef NIMBLE_H
#define	NIMBLE_H

#include <cstdlib>
#include <sys/epoll.h>
#include <sys/errno.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <list>


#include "Stack.h"
#include "ThreadManager.h"
#include "Logger.h"
#include "SettingsManager.h"
#include "Client.h"

using namespace std;

class Nimble {
    public:
        Nimble();
        void newConnection(int fd);
        void closeConnection(Client* client);

        ThreadStack<Client*> _stack;

        const Logger& getLogger(void) const;

        pthread_mutex_t mutex;

        const int& getEpoll(void) const;
        void setSettingsManager(SettingsManager _settingsManager);
        const SettingsManager& getSettingsManager() const;

        vector<Client*> _pool;

    private:
        void startPollLoop(void);
        void setNonBlocking(int fd);
        int _epollFd;
        Logger _logger;
        SettingsManager _settingsManager;
};

#endif	/* GPLAT_H */

