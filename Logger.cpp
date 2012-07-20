#include "Logger.h"
#include "Client.h"

Logger::Logger() { 
    _bufferIndex = 0;
    _top0        = 0;
    _top1        = 0;

    pthread_mutex_init(&_logMutex, NULL);
}

Logger::~Logger() {

}

void Logger::writeBuffer(string (&handleBuffer)[LOGBUFFERSIZE], unsigned int& handleTop) {
    if(handleTop > 0) {
        for(int i = 0; i < handleTop; ++i) {
            cout << handleBuffer[i] << endl;
        }
        
        handleTop = 0;
    }
}

// TODO: will fail if logging takes more than 1 second.
void Logger::run() { 
    while(1) {
        pthread_mutex_lock(&_logMutex);

        // Take note of the buffer index, then toggle it.
        unsigned int handleIndex = _bufferIndex;
        _bufferIndex    = 1 - _bufferIndex;

        pthread_mutex_unlock(&_logMutex);

        // TODO: some sort of array index.
        if(handleIndex == 1) {
            writeBuffer(_buffer0, _top0);
        } else {
            writeBuffer(_buffer1, _top1);
        }

        // cout << "handle: " << handleIndex << " new buffer: " << _bufferIndex << " " << endl;
        sleep(2);
    }
}

void Logger::log(string str) {
    pthread_mutex_lock(&_logMutex);

    if(_bufferIndex == 0)
        _buffer0[_top0++] = str;
    else
        _buffer1[_top1++] = str;
    
    pthread_mutex_unlock(&_logMutex);
}
