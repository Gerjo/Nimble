#ifndef HELPER_H
#define	HELPER_H

#include <fstream>
#include <iostream>
#include <string>

using namespace std;

class Helper {
    public:
        static string GetFileContents(const char* fileName) {
            ifstream file (fileName, ios::in|ios::binary|ios::ate);
            const int size = file.tellg();
            
            if(size <= 0) {
                return string();
            }
            
            char memblock[size];
            
            file.seekg(0, ios::beg);
            file.read(memblock, size);
            file.close();
            
            std::string fileString(memblock, size);
            
            return fileString;
        }

        static bool FileExists(const char* fileName) {
            ifstream file(fileName);
            return file ? true : false;
        }
};

#endif	/* HELPER_H */

