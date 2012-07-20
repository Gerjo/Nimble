#ifndef HEADERPARSER_H
#define	HEADERPARSER_H

#include <algorithm>
#include "HttpHeaders.h"

using namespace std;

class HeaderParser {
    public:
        static HttpHeaders parse(const char* headers, const int& headerLength);

    private:
        static int parseRequest(HttpHeaders& httpHeaders, NimbleString& nimbleString);
        static int parseHost(HttpHeaders& httpHeaders, NimbleString& nimbleString);
        
        HeaderParser();
};

#endif	/* HEADERPARSER_H */
