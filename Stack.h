#ifndef REQUESTSTACK_H
#define	REQUESTSTACK_H

#include <pthread.h>
#include <vector>
#include <stdlib.h>
#include <assert.h>
#include "Client.h"
#include "Semaphore.h"

using namespace std;

// NB.: All code is contained within this header, I haven't got the slightest
// clue how to span templates from a header file into a .cpp file. Help
// is welcome *sigh* -- Gerjo


template <class T>
struct Link {
    Link() {
        data = 0;
        prev = 0;
        next = 0;
    }
    
    T* data;
    Link* prev;
    Link* next;
};

template <class T>
class ThreadStack {
    public:
        ThreadStack() {
            // TODO: this _still_ isn't very graceful...
            _clients  = new T[10000];
            _size     = 0;
            
            pthread_mutex_init(&_popMutex, NULL);
            pthread_cond_init(&_emptyCondition, NULL);
            
        }
        
        ~ThreadStack() { 
            delete[] _clients;
        }

        void push(T client) {
            int oldSize = _size;

            // Perhaps this needs a mutex, too?
            this->_clients[_size++] = client;
            
            if(oldSize == 0) {
                pthread_cond_signal(&_emptyCondition);
            }
        }

        T pop() {
            pthread_mutex_lock(&_popMutex);

            hack:
            
            if(_size == 0) {
                pthread_cond_wait(&_emptyCondition, &_popMutex);
            }
            
            
            // TODO: producer consumer.
            if(_size <= 0) {
                cout << "A hack saved the day. " << endl;
                goto hack;
            }
            
            T returnClient = 0;

            if(_size > 0) {
                returnClient = this->_clients[--_size];
            }

            pthread_mutex_unlock(&_popMutex);

            return returnClient;
        }
    private:
        pthread_cond_t _emptyCondition;
        pthread_mutex_t _popMutex;
        int _size;
        T* _clients;
        
        Link<T> _head;
        Link<T> _tail;
};

#endif	/* REQUESTSTACK_H */

