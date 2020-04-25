#include <SFML/Network.hpp>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

class chatServer {
public:
    void init() {
        for( int i = 0; i <100; i++) {
            userName[i] = "";
        }
        socket.setBlocking(false);
         if (socket.bind(32000) != sf::Socket::Done) {
            std::cout << "ereur dans le bind des sockets" << std::endl;
        } else {
            std::cout << "socket initialisé" << std::endl;
        }
    }
    void handle() {
        std::string type;
        std::string receivedDatas;
        sf::Packet receivedPacket;
        sf::IpAddress senderIp;
        std::string username;
        unsigned short receivedPort;
        if(socket.receive(receivedPacket, senderIp, receivedPort) != sf::Socket::NotReady) {
            receivedPacket >> receivedDatas >> type >> username;
            std::cout << "Received " << type << " packet: " << receivedDatas << " from " << username << " on port " << receivedPort << std::endl;
            handleAction(receivedDatas, type, senderIp, receivedPort, username);
        }
    }
    void handleAction(std::string receivedDatas, std::string type, sf::IpAddress ip, unsigned short port, std::string username) {
        if(type == "auth") {
            if(receivedDatas == "AdR3dV578HKJHAIY8dajofj") {
                std::cout << ip << " vien de se connecter" << std::endl;
                for(int i = 0; i<100; i++) {
                    if(userName[i] == "") {
                        userName[i] = username;
                        usersIp[i] = ip;
                        userPort[i] = port;
                        std::cout << "utilisateur enregistré au décriminant " << i << std::endl;
                        sendPacket("confirmed", "auth", "server", ip, port);
                        return;
                    }
                }
                std::cout << "trop d'utilisateur" << std::endl;
                    sendPacket("too_much_users", "auth", "server",ip, port);
                    return;
            }
            if(receivedDatas == "disconnect_me") {
                for(int i = 0; i < 100; i++) {
                    if(username == userName[i]) {
                        userName[i] = "";
                        sendPacket("deconnection confirmé", "disconnected", "server", ip, port);
                        std::cout << " utilisateur déconnecté " << std::endl;
                    }
                }
                return;
            }
            std::cout << "auth arg invalide";
            return;
        }
        if(type == "new_message") {
            for(int i = 0; i<100; i++) {
                if(userName[i] != "") {
                    sendPacket(receivedDatas, "new_message", username, usersIp[i], userPort[i]);
                }
            }
            for(int i = 99; i>0; i = i-1) {
                messageHistorique[i] = messageHistorique[i-1];
            }
            messageHistorique[0] = receivedDatas;
            std::cout << "nouveau message de " << username << ": " << receivedDatas << std::endl;
            return;
        }
        std::cout << "unkown packet" << std::endl;
        return;
    }
    void sendPacket(std::string content, std::string type, std::string username,sf::IpAddress ip, unsigned short port) {
        sf::Packet packet;
        packet << content << type << username;
        std::cout << "Sending a " << type << " packet: " << content << " to " << ip << " on port " << port << std::endl;
        socket.send(packet, ip, port);
    } 
private:
    sf::IpAddress usersIp[100];
    std::string userName[100];
    unsigned short userPort[100];
    std::string messageHistorique[100];
    sf::UdpSocket socket;
};

int main()
{
    //declarations et initiation class
    chatServer server;
    server.init();
    //Variables
    bool dontStop = true;
    while(dontStop) {
        server.handle();
    }
    return 0;
}

//g++ -c main.cpp
//g++ main.o -o sfml-app -lsfml-graphics -lsfml-window -lsfml-system -lsfml-network
//g++ main.O -o server-app -lsfml-system -lsfml-network
