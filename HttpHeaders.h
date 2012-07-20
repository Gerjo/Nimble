#ifndef HTTPHEADERS_H
#define	HTTPHEADERS_H

#include <iostream>
#include <string>


struct NimbleString {
    NimbleString() {
        length = 0;
        string = 0;
    }
    
    int length;
    const char* string;

    void log() {
        std::cout << "Header length: " << length << " bytes. Content: '";
        std::cout.write(string, length);
        std::cout << "'." << std::endl;
    }
    
    operator std::string() {
        if(length == 0) {
            return std::string();
        }
        
        std::string asString(string, length);
        
        return asString;
    }
};

template <typename T>
struct NimbleArray {
    NimbleArray() {
        length = 0;
    }

    T data;
    int length;
};

struct HttpHeaders {
    NimbleString method;
    NimbleString file;
    NimbleString extension;
    NimbleString queryString;
    
    NimbleString port; // http://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html mandatory
    NimbleString host;
};

#endif	/* HTTPHEADERS_H */

