#ifndef HTTPHEADERTEST_H
#define	HTTPHEADERTEST_H

#include "Test.h"
#include "HeaderParser.h"
#include "HttpHeaders.h"

class HttpHeaderTest : public Test {
public:
    HttpHeaderTest() : Test("HTTP header parsing") {
        
    }

    virtual void run() {
    
        testResponse("GET / HTTP/1.1 \r\nHost: localhost:80\r\n\r\n",
            "GET",
            "/",
            "",
            "",
            "localhost",
            "80");

        testResponse("GET /file.php?arg=bar&foo=zzz HTTP/1.1 \r\nHost: localhost:8080\r\n\r\n",
            "GET",
            "/file.php",
            "php",
            "?arg=bar&foo=zzz",
            "localhost",
            "8080");

        testResponse("GET /?zzzz HTTP/1.1 \r\nHost: localhost\r\n\r\n",
            "GET",
            "/",
            "",
            "?zzzz",
            "localhost",
            "");

        testResponse("GET /file/in/folder.com.php/more_obscure_data.asp?zzzz/bbbbb/path.html HTTP/1.1 \r\nHost: localhost:2\r\n\r\n",
            "GET",
            "/file/in/folder.com.php/more_obscure_data.asp",
            "asp",
            "?zzzz/bbbbb/path.html",
            "localhost",
            "2");
    }

    private:
        
        void testResponse(string request,
                string httpMethod,
                string fileName,
                string extension,
                string queryString,
                string host,
                string port) {

            HttpHeaders headers = HeaderParser::parse(request.c_str(), request.length());

            if(!assertEquals(headers.file.string, fileName.c_str(), headers.file.length)) {
                trace("Filename mismatch.");
            }

            if(!assertEquals(headers.method.string, httpMethod.c_str(), headers.method.length)) {
                trace("HTTP Method mismatch.");
            }

            if(!assertEquals(headers.extension.string, extension.c_str(), headers.extension.length)) {
                trace("File extention mismatch.");
            }

            if(!assertEquals(headers.queryString.string, queryString.c_str(), headers.queryString.length)) {
                trace("File querystring mismatch.");
            }

            if(!assertEquals(headers.host.string, host.c_str(), headers.host.length)) {
                trace("File hostname mismatch.");
            }

            if(!assertEquals(headers.port.string, port.c_str(), headers.port.length)) {
                trace("File port mismatch.");
            }
        }

};

#endif	/* HTTPHEADERTEST_H */

