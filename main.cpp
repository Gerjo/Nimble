#include <cstdlib>
#include <iostream>
#include <sys/types.h>
#include "Nimble.h"
#include "TestSuite.h"
#include "HeaderParser.h"
#include <pthread.h>

using namespace std;

int main(int argc, char** argv) {
    /*
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
    
    cout << "zzz" << endl;
    pthread_cond_signal(&cond);
    pthread_cond_wait(&cond, &mutex);
    cout << "teeheee" << endl;
    */
    
    /*pid_t pid = fork();
    if (pid == 0) {
        while(1) {
            sleep(1);
            cout << "child alive" << endl;
        }
    } else if (pid < 0) {
        cout << "fork error" << endl;
    } else {
        while(1) {
            sleep(1);
            cout << "master alive" << endl;
        }
    } */

    TestSuite testSuite;
    testSuite.run();

    Nimble nimble;

    return 0;
}

