#ifndef SEMPHORE_H
#define	SEMPHORE_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Logic based around: http://www.greenteapress.com/semaphores/
// It's a free book: read it now!

using namespace std;

class Semaphore {
    public:
        Semaphore(int value) {
            _value  = value;
            pthread_mutex_init(&_mutex, NULL);
            pthread_cond_init(&_cond, NULL);
        }

        void wait() {
            pthread_mutex_lock(&_mutex);

            --_value;

            if(_value < 0) {

                // Rather than release all threads, it keeps blocking
                // all but one thread, hence the do... while.
                do {
                    pthread_cond_wait (&_cond, &_mutex);
                } while(_wakeups < 1);

                --_wakeups;
            }

            pthread_mutex_unlock(&_mutex);
        }

        void signal() {
            pthread_mutex_lock(&_mutex);

            ++_value;

            if(_value <= 0) {
                ++_wakeups;
                pthread_cond_signal(&_cond);
            }

            pthread_mutex_unlock(&_mutex);
        }
    private:
        int _value;
        int _wakeups;
        pthread_mutex_t _mutex;
        pthread_cond_t _cond;
};


#endif	/* SEMPHORE_H */

