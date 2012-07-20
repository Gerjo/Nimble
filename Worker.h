#ifndef READER_H
#define	READER_H

#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sstream>
#include <cstdio>
#include <assert.h>

#include "Runnable.h"
#include "HttpHeaders.h"
#include "HeaderParser.h"
#include "Helper.h"
#include "SettingsManager.h"

using namespace std;

class Nimble;
class Client;
class HttpHeaderTest;

class Worker : public Runnable {
    public:
        Worker(Nimble& nimble);
        virtual ~Worker();
        virtual void run(void);

    private:
        const SettingsManager& _settingsManager;
        Nimble& _nimble;
        const int readFromClient(Client* client);
        void sendResponse(Client* client, const char* start, const int& totalLength);
        bool isEndOfRequest(Client* client);
};

#endif	/* READER_H */

