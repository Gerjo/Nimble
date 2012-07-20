#ifndef HOST_H
#define	HOST_H

#include "elements.h"
#include "reader.h"
#include "HttpHeaders.h"
#include <string>
#include <map>

using namespace std;

class Host {
    public:
        Host(const json::Object& asJson) {
            _port = (json::Number) asJson["port"];
            _root = (json::String) asJson["root"];
            _host = (json::String) asJson["host"];
            
            json::Array mimes = (json::Array) asJson["mimetypes"];
            json::Array::iterator itMime;
            
            for(itMime = mimes.Begin(); itMime != mimes.End(); ++itMime) {
                json::Object mime = *itMime;
                
                _mimeTypes.insert(
                        pair<string, string>((json::String)mime["extention"], (json::String)mime["contenttype"])
                );
            }
        }

        ~Host() {
            
        }

        void setRoot(string _root) {
            this->_root = _root;
        }

        const string& getRoot() const {
            return _root;
        }

        void setHost(string _hostname) {
            this->_host = _hostname;
        }

        const string& getHost() const {
            return _host;
        }

        void setPort(int _port) {
            this->_port = _port;
        }

        const int& getPort() const {
            return _port;
        }
        
        const string getMimeType(HttpHeaders& httpHeaders) {
            string meh = httpHeaders.extension;
            
            if(_mimeTypes.find(meh) != _mimeTypes.end()) {
                return _mimeTypes[meh];
            }
            
            //cout << "mime not found" << endl;
            
            // TODO: return something sane.
            return "text/html";
        }

    private:
        Host(const Host& orig) { }

        int _port;
        string _host;
        string _root;
        
        map<string, string> _mimeTypes;
};

#endif	/* HOST_H */

