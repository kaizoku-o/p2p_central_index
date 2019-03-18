#include "sock.h"

int main() {
    int choice;
    cout << "0 for Server 1 for Client" << endl;
    cin >> choice;
    switch (choice) {
    case 0:
        Server server;
        server.create_server();
        break;
    case 1:
        Client client;
        client.create_client();
    }
    return 0;
}
