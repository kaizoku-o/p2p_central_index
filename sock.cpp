#include "sock.h"
#include "message.h"

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

        string choice;
        string recv_msg;
        while (true) {
            cin >> choice;
            // cout << choice << endl;

            if (choice == ADD) {
                // cout << "in new " << endl;

                ServerRequestMessage srv_req("localhost",
                                             "7793", "SOME RFC",
                                             "128",
                                             "1.0",
                                             ServerRequestMessage::METHOD::ADD);


                string msg;
                srv_req.pack(msg);
                client.send_msg(msg);
                recv_msg = client.get_msg();
                // cout << "Received message is: " << recv_msg << endl;
            }
            else if (choice == LOOKUP) {
                // cout << "in new " << endl;

                ServerRequestMessage srv_req("localhost",
                                             "7793", "SOME RFC",
                                             "128",
                                             "1.0",
                                             ServerRequestMessage::METHOD::LOOKUP);


                string msg;
                srv_req.pack(msg);
                client.send_msg(msg);
                recv_msg = client.get_msg();
                // cout << "Received message is: " << recv_msg << endl;
            }
            else if (choice == LIST) {
                // cout << "in new " << endl;

                ServerRequestMessage srv_req("localhost",
                                             "7793", "SOME RFC",
                                             "128",
                                             "1.0",
                                             ServerRequestMessage::METHOD::LIST);


                string msg;
                srv_req.pack(msg);
                client.send_msg(msg);
                recv_msg = client.get_msg();
                // cout << "Received message is: " << recv_msg << endl;
            }
            else
                break;

        }
    }
    return 0;
}
