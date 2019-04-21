// vim: et ts=4 sw=4
#include "sock.h"
#include "message.h"

int main(int argc, char* argv[]) {
    int choice;
    cout << "0 for Server 1 for Client" << endl;
    cin >> choice;
    switch (choice) {
    case 0: {
        Server server;
        server.create_server();
        break;
    }
    case 2: {
        cout << "This is the peer server" << endl;
        P2Server p2Server(5763);
        p2Server.create_server();

        break;
    }
    case 3: {
        cout << "This is the peer client" << endl;
        PeerRequestMessage prms("localhost", "MAC OS",
                                PeerRequestMessage::METHOD::GET,
                                "123",
                                VERSION);
        string msg;
        prms.pack(msg);
        string p2ServerIP = "127.0.0.1";
        int p2ServerPort = 5763;
        Client client(p2ServerIP, p2ServerPort);
        client.create_client();
        // int retryCounter = 3;

        string recv_msg;

        client.send_msg(msg);
        recv_msg = client.get_msg();

        PeerResponseMessage prsp;
        prsp.unpack(recv_msg);

        vector<string> fc = prsp.file_content.back();
        // get fc and write to disk
        prsp.format();
        break;
    }

    case 1: {
        cout << "Enter server ip:" << endl;
        string server_ip;
        cin >> server_ip;

        int server_port;
        cout << "Enter server port:" << endl;
        cin >> server_port; 

        int peer_port;
        cout << "Enter client port (peer server): " << endl;
        cin >> peer_port;

        string client_ip;
        cout << "Enter my ip: " << endl;
        cin >> client_ip;

        Client client(server_ip, server_port);
        client.create_client();

        /* Should spawn a thread here to listen serve fellow peers */
        P2Server p2server(peer_port);
        // P2Server create_server()

        string choice;
        while (true) {
            cin >> choice;
            if (choice == ADD) {
                // Change this if you want to use the code. Can cause buffer
                // overflow
                char buff[1024];
                const static string EOC = "END OF COMMAND";
                string stat;

                string cmd(ADD);

                while (true) {
                    string c_wd;
                    cin.getline(buff, 1024);
                    c_wd = string(buff);
                    if (c_wd == "EOCEOCEOC") {
                        break;
                    }
                    cmd += c_wd + "\n";
                }

                cout << " received cmd:\n ";

                ServerRequestMessage srv_req;
                srv_req.unpack(cmd);
                //req2.format();

                /*
                ServerRequestMessage srv_req(client_ip,
                                             std::to_string(peer_port), 
                                             "SOME RFC",
                                             "128",
                                             VERSION,
                                             ServerRequestMessage::METHOD::ADD);
                */

                string msg;
                srv_req.pack(msg);
                client.send_msg(msg);
                string recv_msg = client.get_msg();

                ServerResponseMessage resp_msg;
                resp_msg.unpack(recv_msg);
                // resp_msg.format();

                cout << endl << endl;
                cout << recv_msg << endl;
            }
            else if (choice == LOOKUP) {

                ServerRequestMessage srv_req("localhost",
                                             "7793", "SOME RFC",
                                             "128",
                                             "1.0",
                                             ServerRequestMessage::METHOD::LOOKUP);

                cout << "Sending lookup request to server" << endl;
                ServerResponseMessage resp_msg;
                string msg;
                srv_req.pack(msg);
                client.send_msg(msg);
                string recv_msg = client.get_msg();
                cout << "Got a message from server " << endl << endl;
                resp_msg.unpack(recv_msg);
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


                cout << "Sending list request to servrer" << endl;
                string msg;
                srv_req.pack(msg);
                client.send_msg(msg);

                ServerResponseMessage resp_msg;
                string recv_msg = client.get_msg();
                cout << "Got a reply from server " << endl << endl;
                resp_msg.unpack(recv_msg);
                resp_msg.format();
            }
            else
                break;

        }
    }
    }
    return 0;
}
