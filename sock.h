#include "iostream"
#include "map"
#include "message.h"
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 9721

using std::cout;
using std::endl;
using std::string;
using std::cin;
using std::map;

class ActivePeersRepository {
private:
    std::map<string, string> hostPortMap_;
public:
    typedef std::map<string, string>::iterator APIter;
    void add(string hostname, string port) {
        hostPortMap_[hostname] = port;
    }
     string lookup(string hostname) {
         return hostPortMap_[hostname];
    }
};

class RFCIndexRepository {
public:
    typedef pair<string, string> HostTitle;
    typedef std::multimap<string, HostTitle>::iterator IndexIter;
private:
    std::multimap<string, HostTitle> field_value_map_;
public:
    void add(string rfcNo, const HostTitle& hostport) {
        // If multithreaded critical section begins
        field_value_map_.insert(make_pair(rfcNo, hostport));
        // critical section ends
    }

    IndexIter lookup(string rfcNo) {
        // If multithreaded critical section begins
        return field_value_map_.find(rfcNo);
        // critical section ends
    }

    void list() {
        istringstream ss;
        cout << "listing index" << endl;
        for (auto iter : field_value_map_) {
            cout << "Host " <<  iter.second.first << endl;
            cout << "Port " << iter.second.second << endl;
        }
        cout << "list ends " << endl;
    }
};

class Server {
public:
    void create_server() {
        RFCIndexRepository rfcIndex;
        ActivePeersRepository activeIndex;

        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        int opt = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR |
                   SO_REUSEPORT, &opt,
                   sizeof(opt));

        struct sockaddr_in address;
        int addrlen = sizeof(address);
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = htonl(INADDR_ANY);
        address.sin_port = htons(PORT);

        int bnd = ::bind(sockfd, (struct sockaddr*) &address,
                       sizeof(address));
        if (bnd < 0) {
            cout << "Could not bind" << endl;
            assert(0);
        }

        int ls = listen(sockfd, 128);
        if (ls < 0) {
            cout << "Could not listen" << endl;
            assert(0);
        }

        while (true) {
            int new_sock;
            new_sock = accept(sockfd, (struct sockaddr*) &address,
                              (socklen_t*) &addrlen);

            while (true) {
                char buffer[1024] = {0};
                int vals = read(new_sock, buffer, 1024);

                ServerRequestMessage svReq;
                svReq.unpack(buffer);
                ServerRequestMessage::METHOD method = svReq.method_;

                switch (method) {
                case ServerRequestMessage::METHOD::ADD: {
                    ServerResponseMessage svResponse;
                    RFCIndexRepository::HostTitle hostTitle =
                        make_pair(svReq.hostname_, svReq.title_);
                    rfcIndex.add(svReq.rfc_, hostTitle);
                    activeIndex.add(svReq.hostname_, svReq.port_);

                    svResponse.rfc_ = svReq.rfc_;
                    svResponse.title_ = svReq.title_;
                    svResponse.hostname_ = svReq.hostname_;
                    svResponse.port_ = svReq.port_;
                    svResponse.status_ = ServerResponseMessage::STATUS_CODE::OK;

                    string msg;
                    svResponse.pack(msg);
                    send(new_sock, msg.c_str(), msg.length(), 0);
                    break;
                }
                case ServerRequestMessage::METHOD::LIST: {
                    rfcIndex.list();
                    break;
                }
                case ServerRequestMessage::METHOD::LOOKUP: {
                    ServerResponseMessage svResponse;
                    auto iter = rfcIndex.lookup(svReq.rfc_);
                    svResponse.rfc_ = iter->first;
                    svResponse.hostname_ = iter->second.first;
                    svResponse.title_ = iter->second.second;
                    svResponse.port_ = activeIndex.lookup(iter->second.first);
                    svResponse.status_ = ServerResponseMessage::STATUS_CODE::OK;

                    string msg;
                    svResponse.pack(msg);
                    send(new_sock, msg.c_str(), msg.length(), 0);
                    break;
                }
                }
                // svrReq.format();
                if (string(buffer) == "-1")
                    break;

                string server_message = "Hi there client";
                send(new_sock, server_message.c_str(), 
                    server_message.length(), 0);
            }
        }
    }
};

class P2Server {
private:
    P2Server() { }
    int peerPort_;
public:
    P2Server(int peerPort) : peerPort_(peerPort) { }

    void create_server() {
        int sockfd = socket(AF_INET, SOCK_STREAM, 0);
        int opt = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR |
                   SO_REUSEPORT, &opt,
                   sizeof(opt));

        struct sockaddr_in address;
        int addrlen = sizeof(address);
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = htonl(INADDR_ANY);
        address.sin_port = htons(peerPort_);

        int bnd = ::bind(sockfd, (struct sockaddr*) &address,
                       sizeof(address));
        if (bnd < 0) {
            cout << "Could not bind" << endl;
            assert(0);
        }

        int ls = listen(sockfd, 128);
        if (ls < 0) {
            cout << "Could not listen" << endl;
            assert(0);
        }

        while (true) {
            int new_sock;
            new_sock = accept(sockfd, (struct sockaddr*) &address,
                              (socklen_t*) &addrlen);

            char buffer[1024] = {0};
            int vals = read(new_sock, buffer, 1024);

            PeerRequestMessage prms;
            prms.unpack(buffer);

            prms.format();

            string server_message = "Hi there fellow client! I am a peer!";
            cout << "server is sending the message " << server_message << endl;

            PeerResponseMessage pRespMsg();
            send(new_sock, server_message.c_str(),
                 server_message.length(), 0);
        }
    }
};

class Client {
public:
    Client(string server_ip, int port) :
        server_ip_("127.0.0.1"), client_sock_fd_(-1) {
        serv_addr_.sin_family = AF_INET;
        serv_addr_.sin_addr.s_addr = inet_addr(server_ip_.c_str());
        serv_addr_.sin_port = htons(port);
    }

    Client() : server_ip_("127.0.0.1"), client_sock_fd_(-1) {
        memset(&serv_addr_, '0', sizeof(serv_addr_));
        serv_addr_.sin_family = AF_INET;
        // Convert ip from string to binary
        serv_addr_.sin_addr.s_addr = inet_addr(server_ip_.c_str());
        serv_addr_.sin_port = htons(5760);
    }

    void send_msg(string message) {
        cout << "in send msg " << endl;
        send(client_sock_fd_, message.c_str(), message.length(), 0);
    }

    string get_msg() {
        char buffer[1024] = {0};
        int vals = read(client_sock_fd_, buffer, 1024);
        if (vals == -1) {
            cout << "Error" << endl;
        }
        return string(buffer);
    }

    void create_client() {
        client_sock_fd_ = socket(AF_INET, SOCK_STREAM, 0);
        int conn = connect(client_sock_fd_, (struct sockaddr*)  &serv_addr_,
                sizeof(serv_addr_));
        if (conn < 0) {
            cout << "Connection failed" << endl;
            assert(0);
        }
    }

    int client_sock_fd_;
    int sock_fd_;
    struct sockaddr_in serv_addr_;
    string server_ip_;
};
