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

class RFCIndexRepository {
public:
    typedef pair<string, string> HostPort;

private:
    std::map<string, HostPort> field_value_map_;
public:
    void add(string rfcNo, const HostPort& hostport) {
        // If multithreaded critical section begins
        field_value_map_[rfcNo] = hostport;
        // critical section ends

    }

    string lookup() {
        // If multithreaded critical section begins

        return "";
        // critical section ends
    }

    void list() {
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

                ServerRequestMessage svrReq;
                svrReq.unpack(buffer);
                svrReq.format();

                RFCIndexRepository::HostPort hostPort =
                    make_pair(svrReq.hostname_, svrReq.port_);
                rfcIndex.add(svrReq.rfc_, hostPort);
                rfcIndex.list();

                if (string(buffer) == "-1")
                    break;

                string server_message = "Hi there client";
                send(new_sock, server_message.c_str(), 
                    server_message.length(), 0);
            }
        }
    }
};

class Client {
public:
    Client() : server_ip_("127.0.0.1"), client_sock_fd_(-1) {
        memset(&serv_addr_, '0', sizeof(serv_addr_));
        serv_addr_.sin_family = AF_INET;
        // Convert ip from string to binary
        serv_addr_.sin_addr.s_addr = inet_addr(server_ip_.c_str());
        serv_addr_.sin_port = htons(PORT);
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
