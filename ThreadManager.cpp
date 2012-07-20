/* 
 * File:   TheadManager.cpp
 * Author: gerjo
 * 
 * Created on April 16, 2012, 8:27 AM
 */

#include "ThreadManager.h"

ThreadManager::ThreadManager() {
}

void Callback(void* runnableContext) {
    Runnable* runnable = static_cast<Runnable*>(runnableContext);
    runnable->run();
}

pthread_t ThreadManager::Register(Runnable* runnable) {
    pthread_t _threadIndex;

    pthread_create(&_threadIndex, 0, (void *(*) (void *))&Callback, (void*) runnable);

    return _threadIndex;
}

pthread_t ThreadManager::Register(Runnable* runnable, const char* name) {
    pthread_t _threadIndex = ThreadManager::Register(runnable);
    //pthread_setname_np(_threadIndex, name);

    return _threadIndex;
}

