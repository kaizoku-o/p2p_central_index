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
        P2Server p2server(9722);
        p2server.create_server();
        cout << "created peer server " << endl;

        string choice;
        while (true) {
            cin >> choice;
            if (choice == ADD) {
                // Change this if you want to use the code. Can cause buffer
                // overflow
                char buff[1024];
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
                // Change this if you want to use the code. Can cause buffer
                // overflow
                char buff[1024];
                string stat;

                string cmd(LOOKUP);

                while (true) {
                    string c_wd;
                    cin.getline(buff, 1024);
                    c_wd = string(buff);
                    if (c_wd == "EOCEOCEOC") {
                        break;
                    }
                    cmd += c_wd + "\n";
                }


                ServerRequestMessage srv_req;
                srv_req.unpack(cmd);

                /*
                ServerRequestMessage srv_req("localhost",
                                             "7793", "SOME RFC",
                                             "128",
                                             "1.0",
                                             ServerRequestMessage::METHOD::LOOKUP);
                */

                cout << "Sending lookup request to server" << endl;
                ServerResponseMessage resp_msg;
                string msg;
                srv_req.pack(msg);
                client.send_msg(msg);
                string recv_msg = client.get_msg();
                cout << "Got a message from server " << endl << endl;
                resp_msg.unpack(recv_msg);
                //resp_msg.format();
                cout << recv_msg << endl;
                if (resp_msg.hostname_.empty())
                    continue;
                string p2ServerIP = resp_msg.hostname_[0];
                string p2ServerPort = "9722"; //resp_msg.port_[0];

                PeerRequestMessage prms(client_ip, "MAC OS",
                        PeerRequestMessage::METHOD::GET,
                        srv_req.rfc_,
                        VERSION);
                string p2rms;
                prms.pack(p2rms);
                cout << "Sending p2s the message: \n";
                cout << p2rms << endl;

                Client client2(p2ServerIP, atoi(p2ServerPort.c_str()));
                client2.create_client();
                client2.send_msg(p2rms);
                string p2resp;
                p2resp = client2.get_msg();
                cout << "Got response from peer server" << endl << endl;
                cout << p2resp;
                cout << endl;
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
                //resp_msg.format();
                cout << recv_msg << endl;
            }
            else
                break;

        }
    }
    }
    return 0;
}
