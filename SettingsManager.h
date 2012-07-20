#ifndef SETTINGSMANAGER_H
#define	SETTINGSMANAGER_H

#include "Helper.h"
#include "Host.h"
#include <vector>
#include <string>
#include <sstream>
#include "HttpHeaders.h"

typedef vector<Host*> Hosts;

using namespace std;

class SettingsManager {
    public:
        SettingsManager();
        ~SettingsManager();
        
        void setHosts(Hosts _hosts);
        Hosts& getHosts();
        void addHostFromFile(const char* fileName);
        void addHost(string& asString);
        void addHost(stringstream& asStringStream);

        Host* getHostFor(const HttpHeaders& httpHeaders) const;
    private:
        SettingsManager(const SettingsManager& orig) { }
        
        Hosts _hosts;
};

#endif	/* SETTINGSMANAGER_H */

