#include "SettingsManager.h"

SettingsManager::SettingsManager() {
    
}

SettingsManager::~SettingsManager() {

}

void SettingsManager::setHosts(Hosts _hosts) {
    this->_hosts = _hosts;
}

Hosts& SettingsManager::getHosts() {
    return _hosts;
}

void SettingsManager::addHostFromFile(const char* fileName) {

    if(!Helper::FileExists(fileName)) {
        cout << "Cannot parse host file: '" << fileName << "', as it does not exist." << endl;
        return;
    }

    string asString = Helper::GetFileContents(fileName);

    addHost(asString);
}

void SettingsManager::addHost(string& asString) {
    stringstream asStringStream;
    asStringStream << asString;
    addHost(asStringStream);
}

void SettingsManager::addHost(stringstream& asStringStream) {
    json::Object asJson;
    json::Reader::Read(asJson, asStringStream);

    _hosts.push_back(new Host(asJson));
}

Host* SettingsManager::getHostFor(const HttpHeaders& httpHeaders) const {
    return _hosts.at(0);
}