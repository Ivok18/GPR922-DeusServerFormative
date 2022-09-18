#pragma once
#include <iostream>
#include <SFML/Network.hpp>
#include <Notif.h>
#include <PacketOperatorOverload.h>
#include <conio.h>

int main()
{
   
    bool running = true;
    unsigned short int port = 51000;
    

    //Player creation
    Player player;
    std::string name;
    int playerID;
    std::cout << "[Character creation]" << '\n';
    std::cout << "name:";
    std::getline(std::cin, name);
    std::cout << "Classes:" << std::endl;
    std::cout << "press (w) to become a Warrior or press (m) to become a Mage" << std::endl;
    char choice = _getch();
    if (choice != 'w' && choice != 'm')
    {
        do
        {
            std::cout << "this class does not exist" << std::endl;
            std::cout << "press (w) to become a Warrior or press (m) to become a Mage" << std::endl;
            choice = _getch();
        } while (choice != 'w' && choice != 'm');
    }

    system("cls");

    if (choice == 'w')
    {
        player.SetName(name);
        player.SetClass(ClassType::WARRIOR);   
    }
    else if(choice == 'm')
    {
        player.SetName(name);
        player.SetClass(ClassType::MAGE);
    }

    sf::TcpSocket socket;
    sf::Time timeout = sf::seconds(10.f);
    sf::Socket::Status status = socket.connect("localhost", port);
    if (status == sf::Socket::Done)
    {
        sf::Packet sendPacket;
        sendPacket << player;
        if (socket.send(sendPacket) != sf::Socket::Done)
        {
            //error
        }
    }
    

    
    int i = 0;

    while (running)
    {   
        sf::Packet receivePacket;
        std::string receiveMsg;
        int notifID;
        float hpRemaining;
        float enemyHpRemaining;
        
        if (socket.receive(receivePacket) != sf::Socket::Done)
        {
            //error
        }

        
        receivePacket >> notifID >> receiveMsg >> hpRemaining >> enemyHpRemaining >> playerID;

        //Set ID once 
        if (i == 0)
        {
            player.SetID(playerID);
            i++;
        }
        NotifType notif = static_cast<NotifType>(notifID);
        

        #pragma region SERVER FULL
        if (notif == NotifType::SERVER_FULL_NOTIF)
        {
            running = false;
        }
        #pragma endregion
        #pragma region CONNECTION SUCCESS AND WAIT FOR OPPONENT
        else if(notif == NotifType::CONNECTION_NOTIF)
        {
            std::cout << "waiting for opponent.." << std::endl << std::endl;
        }
        #pragma endregion
        #pragma region  BATTLE OR TURN RESULT(win turn scenario)
        else if (notif == NotifType::MOVE_CHOICE_NOTIF)
        {
            //clear console before move
            system("cls");

            std::cout << receiveMsg;
            std::cout << "HP" << hpRemaining << " ("; player.PrintClass(); std::cout << ")";
            std::cout << '\t' << '\t' << '\t' << '\t' << "ENEMY HP" << enemyHpRemaining << std::endl;
            std::cout << "(press a)ATTACK" << std::endl;
            std::cout << "(press d)DEFEND" << std::endl;
            //std::cout << "(press ga -> attack guess)GUESS OPPONENT MOVE"<< std::endl;
               
            std::string moveChoice;  
            std::getline(std::cin, moveChoice);
            if (moveChoice != "a" && moveChoice != "d")
            {
                do
                {
                    std::cout << "retry" << std::endl;
                    std::cin >> moveChoice;
                } while (moveChoice != "a" && moveChoice != "d");
            }

            //convert move choice to int
            //int moveChoiceInt = std::stoi(moveChoice);             
         
            sf::Packet sendPacket;
            sendPacket << moveChoice << player.GetID();
            if (socket.send(sendPacket) != sf::Socket::Done)
            {
                //error
            }

            
            
     
            //clear console after move
            system("cls");
            //std::cout << player.GetID() << std::endl;

            //wait end of turn message
            std::cout << "wait for turn results.." << std::endl;

        }
        #pragma endregion
        #pragma region BATTLE (guess move option)
        else if (notif == NotifType::MOVE_CHOICE_GUESS_OPTION_NOTIF)
        {
            //clear console before move
            system("cls");

            std::cout << receiveMsg;
            std::cout << "HP" << hpRemaining << " ("; player.PrintClass(); std::cout << ")";
            std::cout << '\t' << '\t' << '\t' << '\t' << "ENEMY HP" << enemyHpRemaining << std::endl;
            std::cout << "(press a)ATTACK" << std::endl;
            std::cout << "(press d)DEFEND" << std::endl;
            std::cout << "(press ga -> attack guess | press gd -> defense guess)GUESS OPPONENT MOVE"<< std::endl;

            std::string moveChoice;
            std::getline(std::cin, moveChoice);
            if (moveChoice != "a" && moveChoice != "d" && moveChoice != "ga" && moveChoice != "gd")
            {
                do
                {
                    std::cout << "retry" << std::endl;
                    std::cin >> moveChoice;
                } while (moveChoice != "a" && moveChoice != "d" && moveChoice != "ga" && moveChoice != "gd");
            }

          
            sf::Packet sendPacket;
            sendPacket << moveChoice << player.GetID();
            if (socket.send(sendPacket) != sf::Socket::Done)
            {
                //error
            }




            //clear console after move
            system("cls");
            //std::cout << player.GetID() << std::endl;

            //wait end of turn message
            std::cout << "wait for turn results.." << std::endl;
        }
        #pragma endregion
        #pragma region TURN RESULT (lose turn scenario)
        else if (notif == NotifType::TURN_RESULT_NOTIF)
        {
            system("cls");
            std::cout << receiveMsg << std::endl;
            std::cout << "HP REMAINING: " << hpRemaining << '\t' << '\t' << '\t' << '\t' << "ENEMY HP" << enemyHpRemaining << std::endl;
        }
        #pragma endregion
        #pragma region GAME END
        else if (notif == NotifType::GAME_END_NOTIF)
        {
            system("cls");
            std::cout << receiveMsg << std::endl;
        }
        #pragma endregion
    }
   
    return EXIT_SUCCESS;
}