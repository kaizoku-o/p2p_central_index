class BaseMessage {
public:
    void format() = 0;
    void pack(char *bytes) = 0;
    void unpack(char *bytes) = 0;
};

class PeerRequestMessage : public BaseMessage {
public:
    typedef enum {GET, POST} METHOD;

    string hostname_;
    string operatingSystem_;
    int method_;
    string rfc_;
    string version_;

    void format() {

    }

    void pack(char* bytes) {

    }

    void unpack(char* bytes) {

    }
};

class PeerResponseMessage : public BaseMessage {
public:
    typedef enum {OK, BAD_REQUEST, NOT_FOUND, VERSION_NOT_SUPPORTED} STATUS_CODE;
    string date_;
    string os_;
    string last_mod;
    string length_;
    string type_;
    int status_code;

    void format() {
    }

    void pack(char* bytes) {
    }

    void unpack(char* bytes) {
    }
};

class ServerRequestMessage : public BaseMessage {
    typedef enum {ADD, LOOKUP, LIST} METHOD;

    string hostname_;
    string port_;
    string title_;
    string rfc_;
    string version_;
    int method_;

    void format() {
    }

    void pack(char* bytes) {
        switch (method_) {
        case METHOD::ADD:
            break;
        case METHOD::LOOKUP:
            break;
        case METHOD::LIST:
            break;
        }
    }

    void unpack(char* bytes) {
    }
};

class ServerResponseMessage : public BaseMessage {
public:
    typedef enum {OK, BAD_REQUEST, NOT_FOUND, VERSION_NOT_SUPPORTED} STATUS_CODE;
    string rfc_;
    string title_;
    string hostname_;
    string port_;

    void format() {
    }

    void pack(char* bytes) {
    }
    void unpack(char* bytes) {
    }
};
