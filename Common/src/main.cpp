#include <iostream>
#include <SFML/Network.hpp>

//CLIENT SIDE
#pragma region code à mettre dans toutes les situations(a mettre avant)
/* std::cout << "client" << std::endl;
    bool runningOld = false;
    bool running = true;


    sf::TcpSocket socket;
    sf::Time timeout = sf::seconds(10);
    sf::Socket::Status status = socket.connect(sf::IpAddress::LocalHost, 53300, timeout);

    if (status != sf::Socket::Done)
    {
        //erreur
    }

    std::cout << "Connected to server" << std::endl;*/
#pragma endregion
#pragma region chat with 2 clients managed by server - client side
/*
bool running = true;

std::string name;
bool tellToWait = true;
bool wait = false;

std::cout << "name: ";
std::getline(std::cin, name);

sf::TcpSocket socket;
sf::Time timeout = sf::seconds(10);
if (socket.connect(sf::IpAddress::LocalHost, 55001, timeout) == sf::Socket::Done)
{
    sf::Packet sendPacket;
    sendPacket << name << tellToWait;
    if (socket.send(sendPacket) != sf::Socket::Done)
    {
        //error
    }
}



while (running)
{
    sf::Packet receivePacket;
    std::string messageToReceive;
    if (socket.receive(receivePacket) != sf::Socket::Done)
    {
        //error
    }



    receivePacket >> messageToReceive >> wait;
    if (wait == false)
    {
        std::cout << messageToReceive;
        std::cout << ">";
        std::string messageToSend;
        std::getline(std::cin, messageToSend);
        tellToWait = false;
        wait = true;

        sf::Packet sendPacket;
        sendPacket << name << messageToSend << tellToWait;
        if (socket.send(sendPacket) != sf::Socket::Done)
        {
            //error
        }

    }

}
return EXIT_SUCCESS;
#pragma endregion
#pragma region TCP - VERSION SANS PAQUET - 1CLIENT
*/
//send to server data
/*
std::string messageToSend;

//receive from server data
size_t received;
char buffer[2000];

while (runningOld)
{
    std::cout << "waiting for response..." << std::endl;
    if (socket.receive(buffer, sizeof(buffer), received) != sf::Socket::Done)
    {
        // error
    }

    std::cout << "server said: " << buffer << std::endl;
    std::cout << "> ";

    std::getline(std::cin, messageToSend);
    if (socket.send(messageToSend.c_str(), messageToSend.length() + 1) != sf::Socket::Done)
    {
        // error
    }

}
*/
#pragma endregion
#pragma region TCP - VERSION AVEC PAQUET - 1CLIENT
/*//send to server data
std::string messageToSend;
sf::Packet sendPacket;

//receive from server data
std::string messageToReceive;
sf::Packet receivePacket;

while (running)
{
    std::cout << "waiting for response..." << std::endl;
    if (socket.receive(receivePacket) != sf::Socket::Done)
    {
        // error
    }

    receivePacket >> messageToReceive;
    std::cout << "server said: " << messageToReceive << std::endl;
    std::cout << "> ";

    std::getline(std::cin, messageToSend);
    sendPacket << messageToSend;
    if (socket.send(sendPacket) != sf::Socket::Done)
    {
        // error
    }

}
*/
#pragma endregion

//SERVER SIDE
#pragma region code à mettre dans toutes les situations
/*
bool running = true;

//Create a socket to listen to new connections
sf::TcpListener listener;
listener.listen(55001);
*/
#pragma endregion
#pragma region chat with 2 clients managed by server - server side
//Create a list to store the future clients
/*
std::list<sf::TcpSocket*> clients;

//Create a selector
sf::SocketSelector selector;

//Add the listener to the selector
selector.add(listener);

//Endless loop that wait for new connections
while (running)
{
    //Make the selector wait for data on any socket
    if (selector.wait())
    {
        //Test the listener
        if (selector.isReady(listener))
        {
            //The listener is ready : there is a pending connection
            sf::TcpSocket* client = new sf::TcpSocket;

            if (listener.accept(*client) == sf::Socket::Done)
            {
                //listener.setBlocking(true);
                //Send welcome message to new user
                //1.Get user name
                sf::Packet receivePacket;
                std::string username;
                bool wait;
                if (client->receive(receivePacket) == sf::Socket::Done)
                    receivePacket >> username >> wait;

                std::cout << username << " joined the server" << std::endl;

                //2.Send welcome message
                sf::Packet sendPacket;
                std::string welcomeMsg =
                    "Welcome to Deus Server, " + username + "!" + '\n' + '\n' +
                    "[chatroom]" + "\n";
                if (clients.size() == 0)
                {
                    bool tellToWait = false;
                    sendPacket << welcomeMsg << tellToWait;
                }
                else
                {
                    bool tellToWait = true;
                    sendPacket << welcomeMsg << tellToWait;
                }

                if (client->send(sendPacket) != sf::Socket::Done)
                {
                    //error
                }

                //Add the new client to the client list
                clients.push_back(client);


                //Add the new client to the selector so that we will
                //be notified when he sends something
                selector.add(*client);

                //listener.setBlocking(false);
            }
            else
            {
                //Error, we won't get a new connection, delete the socket
                delete client;
            }
        }
        else
        {
            //The listener is not ready, test all other sockets
            for (auto client : clients)
            {
                if (selector.isReady(*client))
                {
                    //the client has sent some data, we can receive it
                    sf::Packet receivePacket;
                    if (client->receive(receivePacket) != sf::Socket::Done)
                    {
                        //error
                    }

                    std::string clientUsername;
                    std::string messageToReceive;
                    bool tellToWait;
                    receivePacket >> clientUsername >> messageToReceive >> tellToWait;

                    //std::string notification;
                   // notification = "[" + clientUsername + "]" + messageToReceive + '\n';
                    //std::cout << notification;

                    for (auto mClient : clients)
                    {
                        if (mClient != client)
                        {
                            sf::Packet sendPacket;
                            std::string notification;
                            notification = "[" + clientUsername + "]" + messageToReceive + '\n';
                            sendPacket << notification << tellToWait;

                            mClient->send(sendPacket);
                        }
                    }
                }
            }
        }
    }
}

return EXIT_SUCCESS;
*/
#pragma endregion
#pragma region TCP - VERSION SANS PAQUET - 1CLIENT
/*
  std::cout << "server" << std::endl;
  sf::TcpListener listener;
  bool runningOld = false;
  bool running = true;

  //link listener to port
  if (listener.listen(53300) != sf::Socket::Done)
  {
      //erreur
  }

  //accept new connexion
  sf::TcpSocket client;
  if (listener.accept(client) != sf::Socket::Done)
  {
      //erreur
  }

  std::cout << "Client connected!" << std::endl;


  //std::cout << "> ";*/

  //send to client data
  /*std::string messageToSend;

  //receive from client data
  size_t received;
  char buffer[2000];

  while (runningOld)
  {
      std::getline(std::cin, messageToSend);
      if (client.send(messageToSend.c_str(), messageToSend.length() + 1) != sf::Socket::Done)
      {
          // error
      }

      std::cout << "waiting for response......" << std::endl;
      if (client.receive(buffer, sizeof(buffer), received) != sf::Socket::Done)
      {
          //error
      }

      std::cout << "client said: " << buffer << std::endl;
      std::cout << "> ";

  }*/
#pragma endregion  
#pragma region TCP - VERSION AVEC PAQUET - 1CLIENT
  /*
    std::cout << "server" << std::endl;
    sf::TcpListener listener;
    bool runningOld = false;
    bool running = true;

    //link listener to port
    if (listener.listen(53300) != sf::Socket::Done)
    {
        //erreur
    }

    //accept new connexion
    sf::TcpSocket client;
    if (listener.accept(client) != sf::Socket::Done)
    {
        //erreur
    }

    std::cout << "Client connected!" << std::endl;


    //std::cout << "> ";*/
    /*//send to client data
    std::string messageToSend;
    sf::Packet sendPacket;

    //receive from client data
    std::string messageToReceive;
    sf::Packet receivePacket;

    while (running)
    {
        std::getline(std::cin, messageToSend);
        sendPacket << messageToSend;
        if (client.send(sendPacket) != sf::Socket::Done)
        {
            // error
        }

        std::cout << "waiting for response......" << std::endl;
        if (client.receive(receivePacket) != sf::Socket::Done)
        {
            //error
        }

        receivePacket >> messageToReceive;
        std::cout << "client said: " << messageToReceive << std::endl;
        std::cout << "> ";
    }*/
#pragma endregion

