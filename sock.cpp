#include "sock.h"
#include "message.h"

int main(int argc, char* argv[]) {
    int choice;
    cout << "0 for Server 1 for Client" << endl;
    cin >> choice;
    switch (choice) {
    case 0:
        Server server;
        server.create_server();
        break;

    case 1:
        if (argc < 2) {
            cout << "Incorrect number of parameters" << endl;
            cout << "Usage: ./sock port_num " << endl;
            exit(0);
        }
        Client client;
        client.create_client();

        int port_num = atoi(argv[1]);
        /* Should spawn a thread here to listen serve fellow peers */
        P2Server p2server(port_num);
        // P2Server create_server()

        string choice;
        string recv_msg;
        while (true) {
            cin >> choice;
            cout << choice << endl;

            if (choice == ADD) {

                ServerRequestMessage srv_req("localhost",
                                             "7793", "SOME RFC",
                                             "128",
                                             "1.0",
                                             ServerRequestMessage::METHOD::ADD);

                string msg;
                srv_req.pack(msg);
                client.send_msg(msg);
                recv_msg = client.get_msg();

                ServerResponseMessage resp_msg;
                resp_msg.unpack(recv_msg);
                resp_msg.format();

                cout << endl << endl;
                // cout << recv_msg << endl;
            }
            else if (choice == LOOKUP) {

                ServerRequestMessage srv_req("localhost",
                                             "7793", "SOME RFC",
                                             "128",
                                             "1.0",
                                             ServerRequestMessage::METHOD::LOOKUP);

                int retryCounter = 3;
                ServerResponseMessage resp_msg;
                while (retryCounter--) {
                    string msg;
                    srv_req.pack(msg);
                    client.send_msg(msg);
                    recv_msg = client.get_msg();
                    resp_msg.unpack(recv_msg);
                }
                resp_msg.format();
                // cout << endl;
                // cout << recv_msg << endl;
            }
            else if (choice == LIST) {

                ServerRequestMessage srv_req("localhost",
                                             "7793", "SOME RFC",
                                             "128",
                                             "1.0",
                                             ServerRequestMessage::METHOD::LIST);


                string msg;
                srv_req.pack(msg);
                client.send_msg(msg);
                recv_msg = client.get_msg();
                cout  << recv_msg << endl;
            }
            else
                break;

        }
    }
    return 0;
}
