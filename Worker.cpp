#include "Worker.h"
#include "Nimble.h"
#include "Client.h"

Worker::Worker(Nimble& nimble) : 
    _nimble(nimble),
    _settingsManager(nimble.getSettingsManager()) {

}

Worker::~Worker() {

}

const int Worker::readFromClient(Client * client) {
    const int readChunkSize = 2024; // TODO: determine some good average?
    int bytesRead = 0;
    int timeout   = 0;

    do {
        int memoryAvailable = client->allocated - client->headerLength;

        assert(memoryAvailable >= 0);

        if(memoryAvailable == 0) {
            client->allocated += readChunkSize;

            // NB.: realloc is more efficient than using "new" and copying the entire buffer.
            client->c_headers  = (char*)realloc(client->c_headers, sizeof(char*) * client->allocated);

            // Might not be 100% thread safe?
            memset(client->c_headers + (client->allocated - readChunkSize), 0, readChunkSize);

            // So, what do we fall back to here?
            if(client->c_headers == 0) {
                perror("realloc failed.");
                exit(21);
            }

            memoryAvailable = readChunkSize;
        }

        bytesRead = ::recv(client->fd, client->c_headers + client->headerLength, memoryAvailable, MSG_NOSIGNAL);

        if(bytesRead == -1) {
            if(errno == EAGAIN) { // 11
                break;
            }

            perror("Cannot receive from client");
        }

        client->headerLength += bytesRead;

        // We read less than we wanted, so assume there is no more data.
        if(bytesRead < memoryAvailable) {
            break;
        }

    } while (timeout++ < 1000);

    if(timeout > 999) {
        cout << " Cannot read from client, the do...while(1) had a time out. " << endl;
        exit(999);
    }

    // At some point we'll have timeouts:
    client->timeLastRead = time(0);

    return bytesRead;
}

void Worker::sendResponse(Client* client, const char* start, const int& totalLength) {

    int total     = 0;
    int bytesSent = 0;

    do {
        bytesSent = ::send(client->fd, (void*) (start + total), totalLength, MSG_NOSIGNAL);

        if (bytesSent == -1) {
            if (errno == EINTR || errno == EAGAIN) {
                continue;
            }
            
            perror("Cannot send to client because");
            break;
        }

        if ((total += bytesSent) < totalLength) {
            continue;
        }

        // We reached this point, so all data is sent.
        break;
    } while (true);

    _nimble.closeConnection(client);
}

bool Worker::isEndOfRequest(Client* client) {

    const int& strlen = client->headerLength;

    // "\r\n\r\n" 13 10 13 10 (RFC compliant browsers?)
    if (client->headerLength > 4 &&
            *(client->c_headers + (strlen - 1)) == 10 &&
            *(client->c_headers + (strlen - 2)) == 13 &&
            *(client->c_headers + (strlen - 3)) == 10 &&
            *(client->c_headers + (strlen - 4)) == 13) {

        return true;

    // "\r\r" 10 10 (netcat and telnet, just hitting "enter")
    } else if (client->headerLength > 2 &&
            *(client->c_headers + (strlen - 1)) == 10 &&
            *(client->c_headers + (strlen - 2)) == 10) {

        return true;
    }

    return false;
}



void Worker::run(void) {
    while(1) {
        Client* client = _nimble._stack.pop(); // TODO: fix sleepPop

        const int bytesRead = readFromClient(client);

        if(bytesRead > 0) {
            if (isEndOfRequest(client)) {

                HttpHeaders headers = HeaderParser::parse(client->c_headers, client->headerLength);
                Host* host          = _settingsManager.getHostFor(headers);

                // TODO: file sanity check, see if it's in the public html root folder.
                stringstream filePath;
                filePath << host->getRoot();
                filePath.write(headers.file.string, headers.file.length);

                string contentType = host->getMimeType(headers);

                if(host != 0) {
                    stringstream response;

                    // Ok... must account for folders here.
                    if(Helper::FileExists(filePath.str().c_str())) {
                        string data = Helper::GetFileContents(filePath.str().c_str());

                        if(data.empty()) {
                            goto empty_file_hack;
                        }

                        response << "HTTP/1.0 200 OK\r\n";
                        response << "Content-Type:" << contentType << "\r\n\r\n";

                        response << data;


                    } else {
                        empty_file_hack:
                        response << "HTTP/1.0 404 OK\r\n";
                        response << "Content-Type: text/html\r\n\r\n";
                        response << "<h1>Nimble received your request. Error: 404</h1><hr><b>Here are some parsing details:</b><br>";
                        response << "<br>Request method: (" << headers.method.length << " bytes) " << string(headers.method.string, headers.method.length);
                        response << "<br>File name: (" << headers.file.length << " bytes) " << string(headers.file.string, headers.file.length);
                        response << "<br>File extension: (" << headers.extension.length << " bytes) " << string(headers.extension.string, headers.extension.length);
                        response << "<br>Query string: (" << headers.queryString.length << " bytes) " << string(headers.queryString.string, headers.queryString.length);
                        response << "<br>Hostname: (" << headers.host.length << " bytes) " << string(headers.host.string, headers.host.length);
                        response << "<br>Port: (" << headers.port.length << " bytes) " << string(headers.port.string, headers.port.length);

                        response << "<br>File path: " << filePath.str() << "";
                        response << "<br>Content type: " << contentType << "";

                        response << "<hr><b>All received headers: </b><pre>";
                        response << string(client->c_headers, client->headerLength);
                        response << "</pre>";
                    }


                    response << "\r\n";
                    sendResponse(client, response.str().c_str(), response.str().length());
                }
            }
        }
    }
}
