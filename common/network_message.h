#pragma once
#include <string>
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;

class NetworkMessage {
public:
    enum Type {
        HANDSHAKE,
        AUTH_REQUEST,
        AUTH_CHALLENGE,
        AUTH_RESPONSE,
        AUTH_SUCCESS,
        AUTH_FAILED,
        REGISTER_USER,
        USER_EXISTS,
        USER_REGISTERED,
        ERROR
    };

    Type type;
    string data;
    vector<string> params;

    NetworkMessage(Type t = ERROR, const string& d = "") : type(t), data(d) {}
    
    string serialize() const;
    static NetworkMessage deserialize(const string& data);
};