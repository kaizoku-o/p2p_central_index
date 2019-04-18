#include <iostream>
#include <sstream>
#include "string"
using namespace std;

#ifndef MESSAGE_H
#define MESSAGE_H

class BaseMessage {
public:
    virtual void format() = 0;
    virtual void pack(string& bytes) = 0;
    virtual void unpack(const string& bytes) = 0;
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

static const string TITLE = "Title";
static const string ADD = "ADD";
static const string LOOKUP = "LOOKUP";
static const string LIST = "LIST";
static const string RFC = "RFC";
static const string HOST = "Host";
static const string PORT_NUM = "Port";
static const string ALL = "ALL";
static const string OK = "200 OK";
static const string BAD = "400 Bad Request";
static const string NOT_FOUND = "404 Not Found";
static const string UNSUPPORTED = "505 P2P-CI Version Not Supported";
static const string VERSION = "P2P-CI/1.0";

class ServerRequestMessage : public BaseMessage {
public:
    ServerRequestMessage() {}
    enum class METHOD {ADD, LOOKUP, LIST};
    enum class LIST_METHOD {ALL};

    string hostname_;
    string port_;
    string title_;
    // rfc_ should be of type integer
    string rfc_;
    string version_;
    METHOD method_;

    ServerRequestMessage(const ServerRequestMessage& sm) {
        hostname_ = sm.hostname_;
        port_ = sm.port_;
        title_ = sm.title_;
        rfc_ = sm.rfc_;
        version_ = sm.version_;
        method_ = sm.method_;
    }

    ServerRequestMessage(string hostname, string port, 
        string title, string rfc, string version,
        METHOD method) : hostname_(hostname), port_(port), title_(title),
                         rfc_(rfc), version_(version), method_(method) {
    }

    void format() {
        cout << "**************************************" << endl;
        cout << "Hostname: " << hostname_ << endl;
        cout << "port: " << port_ << endl;
        cout << "title: " << title_ << endl;
        cout << "rfc: " << rfc_ << endl;
        cout << "version: " << version_ << endl;
        cout << "***************************************" << endl;
    }

    void pack(string& packet) {
        switch (method_) {
        case METHOD::ADD:
            packet += "ADD";
            packet += " RFC ";
            packet += rfc_;
            packet += " ";
            packet += version_ + " \n";

            packet += "Host: ";
            packet += hostname_ + "\n";

            packet += "Port: ";
            packet += port_ + "\n";

            packet += "Title: ";
            packet += title_ + "\n";
            break;

        case METHOD::LOOKUP:
            packet += "LOOKUP";
            packet += " RFC ";
            packet += rfc_;
            packet += " ";
            packet += version_ + "\n";

            packet += "Host: ";
            packet += hostname_ + "\n";

            packet += "Port: ";
            packet += port_ + "\n";

            packet += "Title: ";
            packet += title_ + "\n";
            break;
        case METHOD::LIST:
            packet += "LIST";
            packet += " " + ALL + " ";
            packet += version_ + "\n";

            packet += "Host: ";
            packet += hostname_ + "\n";

            packet += "Port: ";
            packet += port_ + "\n";
            break;
        }
    }

    void unpack(const string& packet) {
        // cout << "**unpacking** " << endl;
        istringstream ss(packet);
        string msg_word;

        ss >> msg_word;

        if (msg_word == ADD) {
            method_ = METHOD::ADD;
        }
        else if (msg_word == LIST) {
            method_ = METHOD::LIST;
        }
        else if (msg_word == LOOKUP) {
            method_ = METHOD::LOOKUP;
        }

        switch (method_) {
        case METHOD::ADD:

            ss >> msg_word;

            if (msg_word == RFC) {
                ss >> msg_word;
                rfc_ = msg_word;
            }

            ss >> msg_word;
            version_ = msg_word;

            ss >> msg_word;
            if (HOST.find_first_of(msg_word) != std::string::npos) {
                ss >> msg_word;
                hostname_ = msg_word;
            }

            ss >> msg_word;
            if (PORT_NUM.find_first_of(msg_word) != std::string::npos) {
                ss >> msg_word;
                port_ = msg_word;
            }

            ss >> msg_word;
            if (TITLE.find_first_of(msg_word) != std::string::npos) {
                while (ss >> msg_word)
                    title_ += msg_word + " ";
            }
        break;
        case METHOD::LOOKUP:
            ss >> msg_word;

            if (msg_word == RFC) {
                ss >> msg_word;
                rfc_ = msg_word;
            }

            ss >> msg_word;
            version_ = msg_word;

            ss >> msg_word;
            if (HOST.find_first_of(msg_word) != std::string::npos) {
                ss >> msg_word;
                hostname_ = msg_word;
            }

            ss >> msg_word;
            if (PORT_NUM.find_first_of(msg_word) != std::string::npos) {
                ss >> msg_word;
                port_ = msg_word;
            }

            ss >> msg_word;
            if (TITLE.find_first_of(msg_word) != std::string::npos) {
                while (ss >> msg_word)
                    title_ += msg_word + " ";
            }
        break;
        case METHOD::LIST:
            ss >> msg_word;

            if (msg_word == ALL) {
                ss >> msg_word;
                version_ = msg_word;

                ss >> msg_word;

                if (HOST.find_first_of(msg_word) != std::string::npos) {
                    ss >> msg_word;
                    hostname_ = msg_word;
                }
                ss >> msg_word;
                if (PORT_NUM.find_first_of(msg_word) != std::string::npos) {
                    ss >> msg_word;
                    port_ = msg_word;
                }

            }
        break;
        }
        // cout << "**unpackign ends**" << endl;
    }
};

class ServerResponseMessage : public BaseMessage {
public:
    enum class STATUS_CODE {OK, BAD_REQUEST, NOT_FOUND, VERSION_NOT_SUPPORTED};
    string rfc_;
    string title_;
    string hostname_;
    string port_;
    STATUS_CODE status_;

    void format() {
    }

    void pack(string& packet) {
        packet += VERSION + " ";

        switch (status_) {
        case STATUS_CODE::OK:
            packet += OK + "\n";
            packet += RFC + " " + rfc_ + " " + TITLE + " " +
                title_ + " " + HOST + " " + hostname_ + " " +
                port_ + "\n";
            break;
        case STATUS_CODE::BAD_REQUEST:
            packet += BAD + "\n";
            break;
        case STATUS_CODE::NOT_FOUND:
            packet += NOT_FOUND + "\n";
            break;
        case STATUS_CODE::VERSION_NOT_SUPPORTED:
            packet += UNSUPPORTED + "\n";
            break;
        }
    }
    void unpack(const string& bytes) {
    }
};

#endif // MESSAGE_H
