#ifndef LOGGER_H
#define	LOGGER_H

#include <pthread.h>
#include <cstdlib>
#include <unistd.h>
#include <string>
#include <sstream>
#include <iostream>

#include "ThreadManager.h"

using namespace std;

// This is a bit of a "meh" moment...
#define LOGBUFFERSIZE 100

// pthread_attr_setstacksize

class Logger : public Runnable {
    public:
        Logger();
        ~Logger();

        void log(string str);

        void run();
    private:
        string _buffer0[LOGBUFFERSIZE];
        string _buffer1[LOGBUFFERSIZE];
        unsigned int _top0;
        unsigned int _top1;
        unsigned int _bufferIndex;

        pthread_mutex_t _logMutex;
        
        void writeBuffer(string (&handleBuffer)[LOGBUFFERSIZE], unsigned int& handleTop);
};

#endif	/* LOGGER_H */

