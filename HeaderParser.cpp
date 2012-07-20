#include "HeaderParser.h"

HeaderParser::HeaderParser() {

}

HttpHeaders HeaderParser::parse(const char* headers, const int& headerLength) {
    HttpHeaders httpHeaders;

    int lineCount        = 0;
    NimbleString* lines  = new NimbleString[40]; // TODO: realloc? NB.: realloc does not call the contructor.
    char endOfRequest    = 0; // We count sequencial empty lines. Two empty lines = end of request.
    int lastOffset       = 0;

    for(int i = 0; i < headerLength; ++i) {
        if(*(headers + i) == '\n') { // '\n' = rfc
            int tentativeEnd = i;
            int trimLength   = 1;

            while(i - trimLength > 1 && *(headers + i - trimLength) == '\r' || *(headers + i - trimLength) == '\n') {
                tentativeEnd -= 1;
                trimLength++;
            }

            if(tentativeEnd <= lastOffset) {
                if(++endOfRequest == 2) {
                    break; // Two empty lines = end of request.
                }

                lastOffset = tentativeEnd + 1;
                continue;
            }

            endOfRequest              = 0; // Restart the empty newline counter
            lines[lineCount].length   = tentativeEnd - lastOffset;
            lines[lineCount].string   = (headers + lastOffset);
            lastOffset                = tentativeEnd + trimLength;
            ++lineCount;
        }

        if(lineCount >= 40) {
            cout << "TODO: realloc in headerparser.cpp";
            exit(45);
        }
    }

    parseRequest(httpHeaders, lines[0]);
    parseHost(httpHeaders, lines[1]);

    delete[] lines;

    return httpHeaders;
}

int HeaderParser::parseHost(HttpHeaders& httpHeaders, NimbleString& nimbleString) {
    const char hostU[]   = "HOST: ";
    const char hostL[]   = "host: ";
    const int hostLength = 6;
    bool isValid         = false;

    if(nimbleString.length < hostLength) {
        return 2; // too short.
    }

    for(int i = 0; i < hostLength; ++i) {
        if(*(nimbleString.string + i) != hostU[i] && *(nimbleString.string + i) != hostL[i]) {
            return 3; // No host prefix.
        }
    }

    httpHeaders.host.string = (nimbleString.string + hostLength);

    bool hasPort = false;

    for(int i = hostLength; i < nimbleString.length; ++i) {
        if(!hasPort && *(nimbleString.string + i) == ':') {
            hasPort = true;

            httpHeaders.host.length = i - hostLength;
            httpHeaders.port.string = (nimbleString.string + i) + 1; // trim the :
        }

        if(hasPort && (i == nimbleString.length - 1)) {
            httpHeaders.port.length = i - (httpHeaders.host.length + hostLength); // account for the :
        }
    }

    return 0;
}

int HeaderParser::parseRequest(HttpHeaders& httpHeaders, NimbleString& nimbleString) {
    //, char* headers, const int& headerLength

    const char* headers     = nimbleString.string;
    const int& headerLength = nimbleString.length;

    const char SPACE = ' ';
    int offset       = 0;

    for(int i = 0; i < headerLength; ++i) {
        if(*(headers + i) == SPACE && httpHeaders.method.length == 0) {
            httpHeaders.method.string = headers + offset;
            httpHeaders.method.length = i;
            offset = i + 1; // Getting rid of the trailing space.

        } else if(*(headers + i) == SPACE && httpHeaders.file.length == 0) {
            httpHeaders.file.string = headers + offset;
            httpHeaders.file.length = i - offset;

            int sensibleFileEnd = httpHeaders.file.length;

            for(int j = 0; j < httpHeaders.file.length; ++j) {
                if(*(httpHeaders.file.string + j) == '?' || *(httpHeaders.file.string + j) == '&') {
                    sensibleFileEnd = j;
                    break;
                }
            }

            for(int k = sensibleFileEnd; k > 0; --k) {
                if(*(httpHeaders.file.string + k) == '.') {
                    ++k; // Trim the leading period of eg.: ".png"
                    httpHeaders.extension.string = httpHeaders.file.string + k;
                    httpHeaders.extension.length = sensibleFileEnd - k;
                    break;
                }
            }

            if(httpHeaders.file.length > sensibleFileEnd) {
                if(httpHeaders.extension.string == 0) {
                     // Trimming trailing slash.
                    httpHeaders.extension.string = httpHeaders.file.string + 1;
                }

                httpHeaders.queryString.string = httpHeaders.extension.string + httpHeaders.extension.length;
                httpHeaders.queryString.length = i - sensibleFileEnd - httpHeaders.method.length;

                // Remove trailing space:
                httpHeaders.queryString.length = max(0, httpHeaders.queryString.length - 1);
            }

            httpHeaders.file.length = sensibleFileEnd; // Getting rid of the query string.
        }

        // Found all! Stop scanning any further.
        if(httpHeaders.method.length != 0 && httpHeaders.file.length != 0) {
            break;
        }
    }

    return 1;
}