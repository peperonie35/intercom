#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <time.h>
#include <windows.h>
#include <TGUI/TGUI.hpp>

using namespace std;
//sf::Style::Fullscreen

class stext {
public:
    sf::Text getText() {
        text.setFont(font);
        text.setString(content);
        text.setCharacterSize(cSize);
        text.setFillColor(couleur);
        text.setStyle(sf::Text::Bold);
        text.setPosition(textPosition);
        return(text);

    }
    void setParameters(string c, int s, sf::Color coul,sf::Vector2f pos) {
        HRSRC rc = FindResource(0, "font", RT_RCDATA);
        HGLOBAL rcData = LoadResource(0, rc);
        void* firstByte = LockResource(rcData);
        size_t rcSize = SizeofResource(0, rc);
        font.loadFromMemory(firstByte, rcSize);
        textPosition = pos;
        content = c;
        cSize = s;
        couleur = coul;
    }
    void setContent(string c) {
        content = c;
    }
private:
    sf::Text text;
    string content = "";
    sf::Font font;
    int cSize = 0;
    sf::Color couleur = sf::Color::Black;
    sf::Vector2f textPosition;
};

class textZone {
public:
    void handle() {

    }
    void init(int s, int x, int y, sf::Vector2f pos,sf::Color coul = sf::Color::Black) {
        HRSRC rc = FindResource(0, "font", RT_RCDATA);
        HGLOBAL rcData = LoadResource(0, rc);
        void* firstByte = LockResource(rcData);
        size_t rcSize = SizeofResource(0, rc);
        font.loadFromMemory(firstByte, rcSize);
        cSize = s;
        couleur = coul;
        position = pos;
        xDim = x;
        yDim = y;
    }
private:
    sf::RenderTexture texture;
    int xDim;
    int yDim;
    sf::Text text;
    string textTyped = "";
    sf::Font font;
    int cSize = 0;
    sf::Color couleur = sf::Color::Black;
    sf::Vector2f position;
};


class chat {
public:
    bool connected = false;
    bool newMessage = false;
    bool init(sf::IpAddress ip, unsigned short portS, std::string username) {
        std::string type = "auth";
        clientUsername = username;
        autPacket << authKey << type << username;
        socket.setBlocking(false);
        serverPort = portS;
        serverIp = ip;
        if (socket.send(autPacket, serverIp, serverPort) != sf::Socket::Done) {
                std::cout << "erreur dans l envoie des donées en udp" << std::endl;
        } else {
            std::cout << "cle de connexion envoyee au server en attente d'une reponse" << std::endl;
        }
        bool dontStop = true;
        sf::Packet receivedPacket;
        sf::IpAddress senderIp;
        unsigned short receivedPort;
        int time = 0;
        sf::Clock clock;
        std::string receivedDatas;
        while(dontStop) {
            if(socket.receive(receivedPacket, senderIp, receivedPort) != sf::Socket::NotReady) {
                receivedPacket >> receivedDatas >> type;
                std::cout << "Received " << type << " packets:" << receivedDatas << " from " << senderIp << " on port " << receivedPort << std::endl;
                break;
            }
            if(clock.getElapsedTime() > sf::seconds(10)) {
                std::cout << "Pas de reponse du server: TIMEOUT..." << endl;
                break;
            }
        }
        if(receivedDatas == "confirmed") {
            std::cout << "connexion etablie avec le server" << std::endl;
            connected = true;
            return(true);
        } else {
            std::cout << "reponse incoherante du serveur, connexion annulé" << std::endl;
            connected = false;
            return(false);
        }
    }
    void sendPacket(std::string content, std::string type, std::string username, sf::IpAddress ip, unsigned short port) {
        sf::Packet packet;
        packet << content << type << username;
        std::cout << "Sending a " << type << " packet: " << content << " to " << ip << " on port " << port << std::endl;
        socket.send(packet, ip, port);
    }
    void disconnect() {
        sendPacket("disconnect_me", "auth", clientUsername, serverIp, serverPort);
    }
    void handle() {
        std::string type;
        std::string receivedDatas;
        std::string username;
        sf::Packet receivedPacket;
        sf::IpAddress senderIp;
        unsigned short receivedPort;
        if(socket.receive(receivedPacket, senderIp, receivedPort) != sf::Socket::NotReady) {
            receivedPacket >> receivedDatas >> type >> username;
            std::cout << "Received: " << receivedDatas << " from " << senderIp << " on port " << receivedPort << std::endl;
            handleAction(receivedDatas, type, senderIp, receivedPort, username);
        }
    }
    void handleAction(std::string receivedDatas, std::string type, sf::IpAddress ip, unsigned short port, std::string username) {
        if(type == "new_message") {
            for(int i = 99; i>0; i = i-1) {
                messageHistorique[i] = messageHistorique[i-1];
            }
            newMessage = true;
            messageHistorique[0] = username + ": " + receivedDatas;
        }
        if(type == "disconnected") {
            connected = false;
            std::cout << "vous avez été déconnecté par l'hote distant, reson: " << receivedDatas << std::endl;
        }
    }
    std::string getMessage(int i) {
        return(messageHistorique[i]);
    }
    void sendMessage(std::string content) {
        sendPacket(content, "new_message", clientUsername, serverIp, serverPort);
    }
private:
    std::string clientUsername;
    std::string messageHistorique[100];
    sf::IpAddress serverIp;
    unsigned short serverPort;
    sf::Packet autPacket;
    string authKey = "AdR3dV578HKJHAIY8dajofj";
    sf::UdpSocket socket;
};


void* firstByte(string name) {
        HRSRC rc = FindResource(0, "troll", RT_RCDATA);
        HGLOBAL rcData = LoadResource(0, rc);
        void* firstByte = LockResource(rcData);
        return firstByte;
}

size_t rcSize(string name) {
    HRSRC rc = FindResource(0, "troll", RT_RCDATA);
    HGLOBAL rcData = LoadResource(0, rc);
    size_t rcSize = SizeofResource(0, rc);
    return rcSize;
}

int random(int max) {
    return (rand()%max);
}

chat chat;
string menu = "login";

void sendMessage(tgui::EditBox::Ptr EditBoxChat) {
    std::string message = EditBoxChat->getText();
    chat.sendMessage(message);
    std::cout << EditBoxChat->getText().toAnsiString() << std::endl;
    EditBoxChat->setText("");
}

void connectToServer(tgui::EditBox::Ptr EditBoxUsername,tgui::EditBox::Ptr EditBoxIp,tgui::EditBox::Ptr EditBoxPort) {
    std::cout << EditBoxIp->getText().toAnsiString() << std::endl;
    std::string sip = EditBoxIp->getText();
    sf::IpAddress ip(sip);
    std::string sport = EditBoxPort->getText();
    int iport = atoi(sport.c_str());
    unsigned short port = (unsigned short) iport;
    std::string username = EditBoxUsername->getText();
    std::cout << "debut de la connexion au server" << std::endl;
    if(chat.init(ip, port, username)) {
        menu = "chat";
    } else {
        std::cout << "failed to connect" << std::endl;
    }
}

int main()
{
    bool stop = false;
    while(stop == false) {
        if(menu == "login") {
            sf::RenderWindow window(sf::VideoMode(251, 62), "intercom entre potes");
            tgui::Gui gui(window);
            window.setVerticalSyncEnabled(true);
            gui.loadWidgetsFromFile("loginForm.txt");
            tgui::Button::Ptr ButtonLogin = gui.get<tgui::Button>("ButtonLogin");
            tgui::EditBox::Ptr EditBoxIp = gui.get<tgui::EditBox>("EditBoxIp");
            tgui::EditBox::Ptr EditBoxPort = gui.get<tgui::EditBox>("EditBoxPort");
            tgui::EditBox::Ptr EditBoxUsername = gui.get<tgui::EditBox>("EditBoxUsername");
            ButtonLogin->connect("pressed", connectToServer, EditBoxUsername, EditBoxIp, EditBoxPort);
            while (window.isOpen())
            {
                sf::Event event;
                while (window.pollEvent(event))
                {
                    if (event.type == sf::Event::Closed) {
                        window.close();
                        stop = true;
                        menu = "";
                    }
                    gui.handleEvent(event);
                }
                window.clear();
                gui.draw();
                window.display();
                if(menu != "login") {
                    window.close();
                }
            }
        }
        if(menu == "chat") {
            sf::RenderWindow window(sf::VideoMode(800, 600), "intercom entre potes");
            tgui::Gui gui(window);
            window.setVerticalSyncEnabled(true);
            gui.loadWidgetsFromFile("chatForm.txt");
            tgui::ListBox::Ptr ListBoxChat = gui.get<tgui::ListBox>("ListBoxChat");
            tgui::EditBox::Ptr EditBoxChat = gui.get<tgui::EditBox>("EditBoxChat");
            tgui::Button::Ptr SendButton = gui.get<tgui::Button>("SendButton");
            SendButton->connect("pressed", sendMessage, EditBoxChat);
            bool entAlrd = false;
            while (window.isOpen())
            {
                sf::Event event;
                while (window.pollEvent(event))
                {
                    if (event.type == sf::Event::Closed) {
                        window.close();
                        chat.disconnect();
                        menu = "login";
                    }
                    gui.handleEvent(event);
                }
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::Enter)) {
                    if(entAlrd == false) {
                        sendMessage(EditBoxChat);
                    }
                    entAlrd = true;
                } else {
                    entAlrd = false;
                }
                chat.handle();
                if(chat.newMessage) {
                    chat.newMessage = false;
                    ListBoxChat->addItem(chat.getMessage(0));
                }
                window.clear();
                gui.draw();
                chat.handle();
                window.display();
                if(menu != "chat") {
                    window.close();
                    chat.disconnect();
                }
                if(chat.connected == false) {
                    window.close();
                    menu = "login";
                }
            }
        }
    }
    return 0;
}
