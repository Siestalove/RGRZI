#include "network_message.h"
#include <sstream>

string NetworkMessage::serialize() const {
    stringstream ss;
    ss << type << "|" << data;
    for (const auto& param : params) {
        ss << "|" << param;
    }
    ss << "\n";
    return ss.str();
}

NetworkMessage NetworkMessage::deserialize(const string& data) {
    stringstream ss(data);
    string token;
    
    if (!getline(ss, token, '|')) {
        return NetworkMessage(ERROR, "Invalid format");
    }
    
    Type type = static_cast<Type>(stoi(token));
    
    if (!getline(ss, token, '\n')) {
        return NetworkMessage(ERROR, "Invalid format");
    }
    
    NetworkMessage msg(type, token);
    
    // Дополнительные параметры
    while (getline(ss, token, '|')) {
        msg.params.push_back(token);
    }
    
    return msg;
}