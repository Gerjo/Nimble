#ifndef THEADMANAGER_H
#define	THEADMANAGER_H

#include <pthread.h>
#include "Runnable.h"

class ThreadManager {
    public:
        ThreadManager();
        
        //static void callback(void* runnableContext);
        static pthread_t Register(Runnable* runnable);
        static pthread_t Register(Runnable* runnable, const char* name);
};

#endif	/* THEADMANAGER_H */

