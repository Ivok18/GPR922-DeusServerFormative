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
    int i = 0; //counter for player id set

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
    sf::Socket::Status status = socket.connect("176.149.56.128", port);
    if (status == sf::Socket::Done)
    {
        sf::Packet sendPacket;
        sendPacket << player;
        if (socket.send(sendPacket) != sf::Socket::Done)
        {
            //error
        }
    }
    

    
    

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

        //Set ID once when receiving the first packet
        if (i == 0)
        {
            receivePacket >> notifID >> receiveMsg >> hpRemaining >> enemyHpRemaining >> playerID;
            player.SetID(playerID);
            i++;
        }
        else
        {
            receivePacket >> notifID >> receiveMsg >> hpRemaining >> enemyHpRemaining;
        }
    
        NotifType notif = static_cast<NotifType>(notifID);
        
        /*Possible outcome when receiving a packet*/
            //server full, cannot join
            if (notif == NotifType::SERVER_FULL_NOTIF)
            {
                running = false;
            }

            //connection success, wait for another player to join
            else if (notif == NotifType::CONNECTION_NOTIF)
            {
                std::cout << "waiting for opponent.." << std::endl << std::endl;
            }

            //you are
            //the 1st player to make a move when game starts or
            //or you won the turn and are being asked to make the first move of next turn
            else if (notif == NotifType::MOVE_CHOICE_NOTIF)
            {
                //clear console before move
                system("cls");

                std::cout << receiveMsg;
                std::cout << "HP" << hpRemaining << " ("; player.PrintClass(); std::cout << ")";
                std::cout << '\t' << '\t' << '\t' << '\t' << "ENEMY HP" << enemyHpRemaining << std::endl;
                std::cout << "(press a)ATTACK" << std::endl;
                std::cout << "(press d)DEFEND" << std::endl;
                std::cout << "(press s)SUPER MOVE [max luck >= 40]" << std::endl;

                std::string moveChoice;
                std::getline(std::cin, moveChoice);
                if (moveChoice != "a" && moveChoice != "d" && moveChoice != "s")
                {
                    do
                    {
                        std::cout << "retry" << std::endl;
                        std::cin >> moveChoice;
                    } while (moveChoice != "a" && moveChoice != "d" && moveChoice != "s");
                }

                sf::Packet sendPacket;
                sendPacket << moveChoice << player.GetID();
                if (socket.send(sendPacket) != sf::Socket::Done)
                {
                    //error
                }




                //clear console after move
                system("cls");
                
                //wait end of turn message
                std::cout << "wait for turn results.." << std::endl;

            }
       
            //you are
            //the 2nd player to make a move during a turn
            //and you can guess what was the move of your opponent 
            //there is one extra option for guessing
            else if (notif == NotifType::MOVE_CHOICE_GUESS_OPTION_NOTIF)
            {
                //clear console before move
                system("cls");

                std::cout << receiveMsg;
                std::cout << "HP" << hpRemaining << " ("; player.PrintClass(); std::cout << ")";
                std::cout << '\t' << '\t' << '\t' << '\t' << "ENEMY HP" << enemyHpRemaining << std::endl;
                std::cout << "(press a)ATTACK" << std::endl;
                std::cout << "(press d)DEFEND" << std::endl;
                std::cout << "(press s)SUPER MOVE [max luck >= 40]" << std::endl;
                std::cout << "GUESS OPPONENT MOVE" << std::endl;
                std::cout << "(press ga -> attack guess)\n(press gd -> defense guess)\n(press gs -> super move guess)\n";

                std::string moveChoice;
                std::getline(std::cin, moveChoice);
                if (moveChoice != "a" && moveChoice != "d" && moveChoice != "s" && moveChoice != "ga" && moveChoice != "gd" && moveChoice != "gs")
                {
                    do
                    {
                        std::cout << "retry" << std::endl;
                        std::cin >> moveChoice;
                    } while (moveChoice != "a" && moveChoice != "d" && moveChoice != "s" && moveChoice != "ga" && moveChoice != "gd" && moveChoice != "gs");
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
     
            //you lost the turn
            else if (notif == NotifType::TURN_RESULT_NOTIF)
            {
                system("cls");
                std::cout << receiveMsg << std::endl;
                std::cout << "HP REMAINING: " << hpRemaining << '\t' << '\t' << '\t' << '\t' << "ENEMY HP" << enemyHpRemaining << std::endl;
            }   

            //game has ended
            //you either won 
            //or lost
            else if (notif == NotifType::GAME_END_NOTIF)
            {
                system("cls");
                std::cout << receiveMsg << std::endl;
            }
    }
   
    return EXIT_SUCCESS;
}
