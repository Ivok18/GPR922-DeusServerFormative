#pragma once
#include <iostream>
#include <SFML/Network.hpp>
#include <Player.h>
#include <Notif.h>
#include <PacketOperatorOverload.h>


void SetOpponent(Player& player, Player& opponent)
{
	player.opponent = std::make_unique<Player>(opponent);
}

int main()
{
	bool running = true;
	unsigned short int port = 51000;
	int nbOfSlot = 2;
	int moveCount = 0;


	//Create a socket to listen to new connections
	sf::TcpListener listener;
	
	listener.listen(port);

	//Create a list to store the future clients
	std::vector<sf::TcpSocket*> clients;

	//Create a list of future players
	std::vector<std::unique_ptr<Player>> players;

	//Create selector
	sf::SocketSelector selector;

	//Add the listener to the selector
	selector.add(listener);
	
	

	//Endless loop that wait for new connections
	while (running)
	{
		//Make the selector wait for data on any sockket
		if (selector.wait())
		{
			if (selector.isReady(listener))
			{
				//the listener is ready : there is a pending connection
				sf::TcpSocket* client = new sf::TcpSocket;
				if (listener.accept(*client) == sf::Socket::Done)
				{
				
					if (clients.size() == 0)
					{
						std::cout << "new player!" << std::endl;

						//reserve location for packet
						players.push_back(std::make_unique<Player>(Player::Player()));
						
						
						sf::Packet receivePacket;
						if (client->receive(receivePacket) != sf::Socket::Done)
						{
							//error
						}

						//store player data in reserved location
						receivePacket >> *players[0].get();
						
						//give ID to new player
						int sizeList = players.size();
						players[0]->SetID(sizeList);
					
						
						//send wait message when new client connect				
						sf::Packet sendPacket;
						std::string sendMsg;
						Notif notif(NotifType::CONNECTION_NOTIF);
						sendPacket << notif.GetID() << sendMsg << 0 << 0 << players[0]->GetID();

						if (client->send(sendPacket) != sf::Socket::Done)
						{
							//error
						}

					
						//add new client to the list
						clients.push_back(client);

						//Add the new client to the selector so that we will
						//be notified when he sends something
						selector.add(*client);
					}

					
					else if(clients.size() == 1)
					{
						std::cout << "new player!" << std::endl;

						//reserve location for packet
						players.push_back(std::make_unique<Player>(Player::Player()));

						//server full -> set opponents
						//SetOpponent(player, players[0]);
						//SetOpponent(players[0], player);

						//store player data 
						sf::Packet receivePacket;
						if (client->receive(receivePacket) != sf::Socket::Done)
						{
							//error
						}

						//store player data in reserved location
						receivePacket >> *players[1].get();

						//give ID to new player
						int sizeList = players.size();
						players[1]->SetID(sizeList);
						
	
						//debug
						std::cout << "server full (players data below)" << std::endl;
						players[0]->PrintData();
						players[1]->PrintData();
						
						//there is already someone waiting, start battle right away
						sf::Packet sendPacket;
						std::string sendMsg = "LUCK [ " + std::to_string(players[1]->GetMinLuck()) + ";" + std::to_string(players[1]->GetMaxLuck()) + " ]\n";
						Notif notif(NotifType::MOVE_CHOICE_NOTIF);
						float hpNewPlayer = 100;
						float hpEnemy = 100;
						sendPacket << notif.GetID() << sendMsg << hpNewPlayer << hpEnemy << players[1]->GetID();
						if (client->send(sendPacket) != sf::Socket::Done)
						{
							//error
						}

						//add new client to the list
						clients.push_back(client);

						//Add the new client to the selector so that we will
						//be notified when he sends something
						selector.add(*client);
					}

					//if new client connects and there is already 2 clients on server, do not accept him
					else if (clients.size() == 2)
					{
						sf::Packet sendPacket;
						Notif notif(NotifType::SERVER_FULL_NOTIF);
						sendPacket << notif.GetID();
						if (client->send(sendPacket) != sf::Socket::Done)
						{
							//error
						}
					}
				}
				else
				{
					delete client;
				}
			}
			else
			{
				//The listener is not ready, test all other sockets
				for (auto client : clients)
				{
					//the client has sent some data, we can receive it
					if (selector.isReady(*client))
					{
						sf::Packet receivePacket;
						if (client->receive(receivePacket) != sf::Socket::Done)
						{
							//error
						}

						int moveChoiceInt = 0;
						std::string moveChoice = "";
						int playerID = 0;
						receivePacket >> moveChoice >> playerID;
						
			
						//debug
						std::cout << "move ->" << moveChoice << std::endl;
						std::cout << "playerID ->" << playerID << std::endl;
						
						if (moveCount == 2)
						{
							moveCount = 0;
						}
						moveCount++;						
						for (auto& player : players)
						{
							if (player->GetID() == playerID)
							{
								//debug
								std::cout << "playerID ->" << playerID << std::endl;
								std::cout << "move ->" << moveChoice << std::endl;
								
								int scorePlayer;
								do
								{
									scorePlayer = player->Roll();
								} while (scorePlayer < 0);
								player->SetScore(scorePlayer);
								player->SetMove(moveChoice);
							}
						}

						if (moveCount == 2)
						{
							
							//clear console before printing players datas
							system("cls");
							//std::cout << "just played ID:" << playerID << std::endl;
							std::cout << "PLAYERS THAT HAVE PLAYED" << std::endl;

							//debug
							for (auto& player : players)
							{
								player->PrintData(); std::cout << std::endl << std::endl;
							}


							//traitement des données des moves de chaque joueur
							int scorePlayer = 0;
							std::string movePlayer;
							int scoreOpponent = 0;
							std::string moveOpponent;
							std::string turnResultsMsg;
							int minLuckPlayer = 0;
							int maxLuckPlayer = 0;
							int minLuckOpponent = 0;
							int maxLuckOpponent = 0;
							

							for (auto& player : players)
							{
								if (player->GetID() == playerID)
								{
									scorePlayer = player->GetScore();
									movePlayer = player->GetMove();
									minLuckPlayer = player->GetMinLuck();
									maxLuckPlayer = player->GetMaxLuck();
								}
								else
								{
									scoreOpponent = player->GetScore();
									moveOpponent = player->GetMove();
									minLuckOpponent = player->GetMinLuck();
									maxLuckOpponent = player->GetMaxLuck();
								}
							}

							//debug
							std::cout << scorePlayer << std::endl;
							std::cout << movePlayer << std::endl;
							std::cout << scoreOpponent << std::endl;
							std::cout << moveOpponent << std::endl << std::endl;
							//a a , a d , d a , d d

							//p1 and opponent attack
							if (movePlayer == "a" && moveOpponent == "a")
							{
								//prevents from equal scores
								while (scorePlayer == scoreOpponent)
								{
									do
									{
										scorePlayer = players[0]->Roll();
									} while (scorePlayer < 0);
									do
									{
										scoreOpponent = players[1]->Roll();
									} while (scoreOpponent < 0);
								}

								float opponentHp = 0;
								int opponentID = 0;
								float playerHp = 0;

								//p1 has higher score
								if (scorePlayer > scoreOpponent)
								{

									for (auto& player : players)
									{
										//find opponent in player list
										if (player->GetID() != playerID)
										{
											//opponent loses 10hp
											opponentHp = player->GetHp();
											opponentHp -= 10;
											opponentID = player->GetID();
											player->SetHp(opponentHp);
										}
										else //get player hp
										{
											playerHp = player->GetHp();
										}
									}

									//Player -> win game scenario
									if (opponentHp <= 0)
									{
										for (auto mClient : clients)
										{
											if (mClient != client)
											{
												sf::Packet sendPacket;
												std::string sendMsg = "You lost..";
												Notif notif(NotifType::GAME_END_NOTIF);
												sendPacket << notif.GetID() << sendMsg;
												if (mClient->send(sendPacket) != sf::Socket::Done)
												{
													//error
												}
											}
										}

										sf::Packet sendPacket;
										Notif notif(NotifType::GAME_END_NOTIF);
										std::string sendMsg = "You won!";
										sendPacket << notif.GetID() << sendMsg;
										if (client->send(sendPacket) != sf::Socket::Done)
										{
											//error
										}

									}

									//Player -> win turn scenario
									else
									{
										for (auto mClient : clients)
										{
											if (mClient != client)
											{
												sf::Packet sendPacket;
												std::string sendMsg =
													"Your opponent attack was stronger, you lost 10 hp \n and you will be the last player to make a move next turn";

												Notif notif(NotifType::TURN_RESULT_NOTIF);
												sendPacket << notif.GetID() << sendMsg << opponentHp << playerHp << opponentID;
												if (mClient->send(sendPacket) != sf::Socket::Done)
												{
													//error
												}
											}
										}

										sf::Packet sendPacket;
										Notif notif(NotifType::MOVE_CHOICE_NOTIF);
										std::string sendMsg =
											"Your opponent was hit succesfully ! \n and you will be the first to make a move next turn\n\n\n";
										sendMsg += "LUCK [ " + std::to_string(minLuckPlayer) + ";" + std::to_string(maxLuckPlayer) + " ]\n";
										sendPacket << notif.GetID() << sendMsg << playerHp << opponentHp << playerID;
										if (client->send(sendPacket) != sf::Socket::Done)
										{
											//error
										}

									}
									

								
								}
								//opponent has higher score
								else if (scorePlayer < scoreOpponent)
								{
									for (auto& player : players)
									{
										//find player in player list
										if (player->GetID() == playerID)
										{
											//player loses 10hp
											playerHp = player->GetHp();
											playerHp -= 10;
											player->SetHp(playerHp);
										}
										else //find player
										{
											opponentHp = player->GetHp();
										}
									}

									//Player -> lose game scenario
									if (playerHp <= 0)
									{
										for (auto mClient : clients)
										{
											if (mClient != client)
											{
												sf::Packet sendPacket;
												std::string sendMsg = "You won!";
												Notif notif(NotifType::GAME_END_NOTIF);
												sendPacket << notif.GetID() << sendMsg;
												if (mClient->send(sendPacket) != sf::Socket::Done)
												{
													//error
												}
											}
										}

										sf::Packet sendPacket;
										Notif notif(NotifType::GAME_END_NOTIF);
										std::string sendMsg = "You lost..";
										sendPacket << notif.GetID() << sendMsg;
										if (client->send(sendPacket) != sf::Socket::Done)
										{
											//error
										}

									}

									//Player -> lose turn scenario
									else
									{
										for (auto mClient : clients)
										{
											if (mClient != client)
											{
												sf::Packet sendPacket;
												std::string sendMsg =
													"Your opponent was hit succesfully ! \n and you will be the first to make a move next turn\n\n\n";
												sendMsg += "LUCK [ " + std::to_string(minLuckOpponent) + ";" + std::to_string(maxLuckOpponent) + " ]\n";

												Notif notif(NotifType::MOVE_CHOICE_NOTIF);
												sendPacket << notif.GetID() << sendMsg << opponentHp << playerHp << opponentID;
												if (mClient->send(sendPacket) != sf::Socket::Done)
												{
													//error
												}
											}
										}

										sf::Packet sendPacket;
										Notif notif(NotifType::TURN_RESULT_NOTIF);
										std::string sendMsg =
											"Your opponent attack was stronger, you lost 10 hp \n and you will be the last player to make a move next turn";
										sendPacket << notif.GetID() << sendMsg << playerHp << opponentHp << playerID;
										if (client->send(sendPacket) != sf::Socket::Done)
										{
											//error
										}

									}
								}


							}

							//p1 attack and opponent defends
							else if (movePlayer == "a" && moveOpponent == "d")
							{
								//prevents from equal scores
								while (scorePlayer == scoreOpponent)
								{
									do
									{
										scorePlayer = players[0]->Roll();
									} while (scorePlayer < 0);
									do
									{
										scoreOpponent = players[1]->Roll();
									} while (scoreOpponent < 0);
								}

								float opponentHp = 0;
								int opponentID = 0;
								float playerHp = 0;

								//p1 has higher score
								if (scorePlayer > scoreOpponent)
								{

									for (auto& player : players)
									{
										//find opponent in player list
										if (player->GetID() != playerID)
										{
											//opponent loses 5hp
											opponentHp = player->GetHp();
											opponentHp -= 5;
											opponentID = player->GetID();
											player->SetHp(opponentHp);
										}
										else //get player hp
										{
											playerHp = player->GetHp();
										}
									}

									//Player -> win game scenario
									if (opponentHp <= 0)
									{
										for (auto mClient : clients)
										{
											if (mClient != client)
											{
												sf::Packet sendPacket;
												std::string sendMsg = "You lost..";
												Notif notif(NotifType::GAME_END_NOTIF);
												sendPacket << notif.GetID() << sendMsg;
												if (mClient->send(sendPacket) != sf::Socket::Done)
												{
													//error
												}
											}
										}

										sf::Packet sendPacket;
										Notif notif(NotifType::GAME_END_NOTIF);
										std::string sendMsg = "You won!";
										sendPacket << notif.GetID() << sendMsg;
										if (client->send(sendPacket) != sf::Socket::Done)
										{
											//error
										}

									}

									//Player -> win turn scenario
									else
									{
										for (auto mClient : clients)
										{
											if (mClient != client)
											{
												sf::Packet sendPacket;
												std::string sendMsg =
													"Your opponent attack managed to hit you in spite of your defense, you lost 5 hp \n and you will be the last player to make a move next turn";

												Notif notif(NotifType::TURN_RESULT_NOTIF);
												sendPacket << notif.GetID() << sendMsg << opponentHp << playerHp << opponentID;
												if (mClient->send(sendPacket) != sf::Socket::Done)
												{
													//error
												}
											}
										}

										sf::Packet sendPacket;
										Notif notif(NotifType::MOVE_CHOICE_NOTIF);
										std::string sendMsg =
											"You managed to hit your opponent in spite of his defense ! \n and you will be the first to make a move next turn\n\n\n";
										sendMsg += "LUCK [ " + std::to_string(minLuckPlayer) + ";" + std::to_string(maxLuckPlayer) + " ]\n";
										sendPacket << notif.GetID() << sendMsg << playerHp << opponentHp << playerID;
										if (client->send(sendPacket) != sf::Socket::Done)
										{
											//error
										}

									}



								}
								//opponent has higher score
								else if (scorePlayer < scoreOpponent)
								{
									for (auto& player : players)
									{
										//find opponent in player list
										if (player->GetID() != playerID)
										{
											//opponent loses 10hp
											opponentHp = player->GetHp();
											opponentID = player->GetID();
											player->SetHp(opponentHp);
										}
										else //find player
										{
											playerHp = player->GetHp();
										}
									}


									//Player -> lose turn scenario
									for (auto mClient : clients)
									{
										if (mClient != client)
										{
											sf::Packet sendPacket;
											std::string sendMsg =
												"Your successfully blocked your opponent attack! \n and you will be the first to make a move next turn\n\n\n";
											sendMsg += "LUCK [ " + std::to_string(minLuckOpponent) + ";" + std::to_string(maxLuckOpponent) + " ]\n";

											Notif notif(NotifType::MOVE_CHOICE_NOTIF);
											sendPacket << notif.GetID() << sendMsg << opponentHp << playerHp << opponentID;
											if (mClient->send(sendPacket) != sf::Socket::Done)
											{
												//error
											}
										}
									}

									sf::Packet sendPacket;
									Notif notif(NotifType::TURN_RESULT_NOTIF);
									std::string sendMsg =
										"Your opponent blocked your attack \n and you will be the last player to make a move next turn";
									sendPacket << notif.GetID() << sendMsg << playerHp << opponentHp << playerID;
									if (client->send(sendPacket) != sf::Socket::Done)
									{
										//error
									}

								}
								
							}

							//p1 attacks and opponent guesses attack
							else if (movePlayer == "a" && moveOpponent == "ga")
							{
								float opponentHp = 0;
								int opponentID = 0;
								float playerHp = 0;

								for (auto& player : players)
								{
									//find player in player list
									if (player->GetID() == playerID)
									{
										//get player datas
										playerHp = player->GetHp();
										player->SetHp(playerHp);
									}
									else //get opponent data and rise luck
									{
										opponentHp = player->GetHp();
										opponentID = player->GetID();
										player->RiseLuck();
										minLuckOpponent = player->GetMinLuck();
										maxLuckOpponent = player->GetMaxLuck();
									}
								}

								for (auto mClient : clients)
								{
									if (mClient != client)
									{
										sf::Packet sendPacket;
										Notif notif(NotifType::MOVE_CHOICE_NOTIF);
										std::string sendMsg =
											"Your intuition was good, the Luck Gods decided to rise your luck ! \n and you will be the first to make a move next turn\n\n\n";
										sendMsg += "LUCK [ " + std::to_string(minLuckOpponent) + ";" + std::to_string(maxLuckOpponent) + " ]\n";


										sendPacket << notif.GetID() << sendMsg << playerHp << opponentHp << playerID;
										if (mClient->send(sendPacket) != sf::Socket::Done)
										{
											//error
										}
									}

								}

								sf::Packet sendPacket;
								std::string sendMsg =
									"Your opponent did evade from your attack, hence his intuition sharpened and his luck rose to new heights.. \n and you will be the last to make a move next turn";

								Notif notif(NotifType::TURN_RESULT_NOTIF);
								sendPacket << notif.GetID() << sendMsg << opponentHp << playerHp << opponentID;
								if (client->send(sendPacket) != sf::Socket::Done)
								{
									//error
								}

							}

							//p1 attacks and opponent guesses a defense
							else if (movePlayer == "a" && moveOpponent == "gd")
							{
								float opponentHp = 0;
								int opponentID = 0;
								float playerHp = 0;

								for (auto& player : players)
								{
									//find opponent in player list
									if (player->GetID() != playerID)
									{
										//opponent lose 20hp
										opponentHp = player->GetHp();
										opponentHp -= 20;
										player->SetHp(opponentHp);
										opponentID = player->GetID();

									}
									else //get player data
									{
										playerHp = player->GetHp();
									}
								}

								//Player -> win game scenario
								if (opponentHp <= 0)
								{
									for (auto mClient : clients)
									{
										if (mClient != client)
										{
											sf::Packet sendPacket;
											std::string sendMsg = "You lost..";
											Notif notif(NotifType::GAME_END_NOTIF);
											sendPacket << notif.GetID() << sendMsg;
											if (mClient->send(sendPacket) != sf::Socket::Done)
											{
												//error
											}
										}
									}

									sf::Packet sendPacket;
									Notif notif(NotifType::GAME_END_NOTIF);
									std::string sendMsg = "You won!";
									sendPacket << notif.GetID() << sendMsg;
									if (client->send(sendPacket) != sf::Socket::Done)
									{
										//error
									}

								}

								//Player -> win turn scenario
								else
								{
									for (auto mClient : clients)
									{
										if (mClient != client)
										{
											sf::Packet sendPacket;
											std::string sendMsg =
												"bad intentions leads to bad intuitions, Luck Gods are discontent, you lost 20 hp \n and you will be the last player to make a move next turn";

											Notif notif(NotifType::TURN_RESULT_NOTIF);
											sendPacket << notif.GetID() << sendMsg << opponentHp << playerHp << opponentID;
											if (mClient->send(sendPacket) != sf::Socket::Done)
											{
												//error
											}
										}
									}

									sf::Packet sendPacket;
									Notif notif(NotifType::MOVE_CHOICE_NOTIF);
									std::string sendMsg =
										"Your opponent toyed with Luck Gods and lost! \n and you will be the first to make a move next turn\n\n\n";
									sendMsg += "LUCK [ " + std::to_string(minLuckPlayer) + ";" + std::to_string(maxLuckPlayer) + " ]\n";
									sendPacket << notif.GetID() << sendMsg << playerHp << opponentHp << playerID;
									if (client->send(sendPacket) != sf::Socket::Done)
									{
										//error
									}

								}
							}

							//p1 defends and opponent attacks
							else if (movePlayer == "d" && moveOpponent == "a")
							{
								//prevents from equal scores
								while (scorePlayer == scoreOpponent)
								{
									do
									{
										scorePlayer = players[0]->Roll();
									} while (scorePlayer < 0);
									do
									{
										scoreOpponent = players[1]->Roll();
									} while (scoreOpponent < 0);
								}

								float opponentHp = 0;
								int opponentID = 0;
								float playerHp = 0;

								//p1 has higher score
								if (scorePlayer > scoreOpponent)
								{

									for (auto& player : players)
									{
										//find opponent in player list
										if (player->GetID() != playerID)
										{
											opponentHp = player->GetHp();
											opponentID = player->GetID();
											player->SetHp(opponentHp);
										}
										else //find player
										{
											playerHp = player->GetHp();
										}
									}

									//Player -> win turn scenario
									for (auto mClient : clients)
									{
										if (mClient != client)
										{
											sf::Packet sendPacket;
											std::string sendMsg =
												"Your opponent blocked your attack \n and you will be the last player to make a move next turn";

											Notif notif(NotifType::TURN_RESULT_NOTIF);
											sendPacket << notif.GetID() << sendMsg << opponentHp << playerHp << opponentID;
											if (mClient->send(sendPacket) != sf::Socket::Done)
											{
												//error
											}
										}
									}

									sf::Packet sendPacket;
									Notif notif(NotifType::MOVE_CHOICE_NOTIF);
									std::string sendMsg =
										"Your successfully blocked your opponent attack! \n and you will be the first to make a move next turn\n\n\n";
									sendMsg += "LUCK [ " + std::to_string(minLuckPlayer) + ";" + std::to_string(maxLuckPlayer) + " ]\n";
									sendPacket << notif.GetID() << sendMsg << playerHp << opponentHp << playerID;
									if (client->send(sendPacket) != sf::Socket::Done)
									{
										//error
									}
								}
								//opponent has higher score
								else if (scorePlayer < scoreOpponent)
								{

									for (auto& player : players)
									{
										//find player in player list
										if (player->GetID() == playerID)
										{

											playerHp = player->GetHp();
											playerHp -= 5;
											player->SetHp(playerHp);
										}
										else //find opponent
										{
											opponentID = player->GetID();
											opponentHp = player->GetHp();
											player->SetHp(opponentHp);
										}
									}

									//Player -> lose game scenario
									if (playerHp <= 0)
									{
										for (auto mClient : clients)
										{
											if (mClient != client)
											{
												sf::Packet sendPacket;
												std::string sendMsg = "You won!";
												Notif notif(NotifType::GAME_END_NOTIF);
												sendPacket << notif.GetID() << sendMsg;
												if (mClient->send(sendPacket) != sf::Socket::Done)
												{
													//error
												}
											}
										}

										sf::Packet sendPacket;
										Notif notif(NotifType::GAME_END_NOTIF);
										std::string sendMsg = "You lost..";
										sendPacket << notif.GetID() << sendMsg;
										if (client->send(sendPacket) != sf::Socket::Done)
										{
											//error
										}

									}

									//Player -> lose turn scenario
									else
									{
										for (auto mClient : clients)
										{
											if (mClient != client)
											{
												sf::Packet sendPacket;
												std::string sendMsg =
													"You managed to hit your opponent in spite of his defense ! \n and you will be the first to make a move next turn\n\n\n";
												sendMsg += "LUCK [ " + std::to_string(minLuckOpponent) + ";" + std::to_string(maxLuckOpponent) + " ]\n";

												Notif notif(NotifType::MOVE_CHOICE_NOTIF);
												sendPacket << notif.GetID() << sendMsg << opponentHp << playerHp << opponentID;
												if (mClient->send(sendPacket) != sf::Socket::Done)
												{
													//error
												}
											}
										}

										sf::Packet sendPacket;
										Notif notif(NotifType::TURN_RESULT_NOTIF);
										std::string sendMsg =
											"Your opponent attack managed to hit you in spite of your defense, you lost 5 hp \n and you will be the last player to make a move next turn";
										sendPacket << notif.GetID() << sendMsg << playerHp << opponentHp << playerID;
										if (client->send(sendPacket) != sf::Socket::Done)
										{
											//error
										}
									}
								}
							}

							//p1 and opponent defend
							else if (movePlayer == "d" && moveOpponent == "d")
							{
								//prevents from equal scores
								while (scorePlayer == scoreOpponent)
								{
									do
									{
										scorePlayer = players[0]->Roll();
									} while (scorePlayer < 0);
									do
									{
										scoreOpponent = players[1]->Roll();
									} while (scoreOpponent < 0);
								}

								float opponentHp = 0;
								int opponentID = 0;
								float playerHp = 0;

								//p1 has higher score
								if (scorePlayer > scoreOpponent)
								{

									for (auto& player : players)
									{
										//find opponent in player list
										if (player->GetID() != playerID)
										{
											//opponent loses 3hp
											opponentHp = player->GetHp();
											opponentHp -= 3;
											opponentID = player->GetID();
											player->SetHp(opponentHp);
										}
										else //get player hp
										{
											playerHp = player->GetHp();
										}
									}

									//Player -> win game scenario
									if (opponentHp <= 0)
									{
										for (auto mClient : clients)
										{
											if (mClient != client)
											{
												sf::Packet sendPacket;
												std::string sendMsg = "You lost..";
												Notif notif(NotifType::GAME_END_NOTIF);
												sendPacket << notif.GetID() << sendMsg;
												if (mClient->send(sendPacket) != sf::Socket::Done)
												{
													//error
												}
											}
										}

										sf::Packet sendPacket;
										Notif notif(NotifType::GAME_END_NOTIF);
										std::string sendMsg = "You won!";
										sendPacket << notif.GetID() << sendMsg;
										if (client->send(sendPacket) != sf::Socket::Done)
										{
											//error
										}

									}

									//Player -> win turn scenario
									else
									{
										for (auto mClient : clients)
										{
											if (mClient != client)
											{
												sf::Packet sendPacket;
												std::string sendMsg =
													"Clash of shields ! But your opponent's was stronger, you lost 3 hp \n and you will be the last player to make a move next turn";

												Notif notif(NotifType::TURN_RESULT_NOTIF);
												sendPacket << notif.GetID() << sendMsg << opponentHp << playerHp << opponentID;
												if (mClient->send(sendPacket) != sf::Socket::Done)
												{
													//error
												}
											}
										}

										sf::Packet sendPacket;
										Notif notif(NotifType::MOVE_CHOICE_NOTIF);
										std::string sendMsg =
											"You successfully hit your opponent with your shield ! \n and you will be the first to make a move next turn\n\n\n";
										sendMsg += "LUCK [ " + std::to_string(minLuckPlayer) + ";" + std::to_string(maxLuckPlayer) + " ]\n";
										sendPacket << notif.GetID() << sendMsg << playerHp << opponentHp << playerID;
										if (client->send(sendPacket) != sf::Socket::Done)
										{
											//error
										}

									}



								}
								//opponent has higher score
								else if (scorePlayer < scoreOpponent)
								{
									for (auto& player : players)
									{
										//find player in player list
										if (player->GetID() == playerID)
										{
											//player loses 10hp
											playerHp = player->GetHp();
											playerHp -= 3;
											player->SetHp(playerHp);
										}
										else //find player
										{
											opponentHp = player->GetHp();
										}
									}

									//Player -> lose game scenario
									if (playerHp <= 0)
									{
										for (auto mClient : clients)
										{
											if (mClient != client)
											{
												sf::Packet sendPacket;
												std::string sendMsg = "You won!";
												Notif notif(NotifType::GAME_END_NOTIF);
												sendPacket << notif.GetID() << sendMsg;
												if (mClient->send(sendPacket) != sf::Socket::Done)
												{
													//error
												}
											}
										}

										sf::Packet sendPacket;
										Notif notif(NotifType::GAME_END_NOTIF);
										std::string sendMsg = "You lost..";
										sendPacket << notif.GetID() << sendMsg;
										if (client->send(sendPacket) != sf::Socket::Done)
										{
											//error
										}

									}

									//Player -> lose turn scenario
									else
									{
										for (auto mClient : clients)
										{
											if (mClient != client)
											{
												sf::Packet sendPacket;
												std::string sendMsg =
													"You successfully hit your opponent with your shield ! \n and you will be the first to make a move next turn\n\n\n";
												sendMsg += "LUCK [ " + std::to_string(minLuckOpponent) + ";" + std::to_string(maxLuckOpponent) + " ]\n";

												Notif notif(NotifType::MOVE_CHOICE_NOTIF);
												sendPacket << notif.GetID() << sendMsg << opponentHp << playerHp << opponentID;
												if (mClient->send(sendPacket) != sf::Socket::Done)
												{
													//error
												}
											}
										}

										sf::Packet sendPacket;
										Notif notif(NotifType::TURN_RESULT_NOTIF);
										std::string sendMsg =
											"Clash of shields ! But your opponent's was stronger, you lost 3 hp \n and you will be the last player to make a move next turn";
										sendPacket << notif.GetID() << sendMsg << playerHp << opponentHp << playerID;
										if (client->send(sendPacket) != sf::Socket::Done)
										{
											//error
										}

									}
								}


							}

							//p1 defends and opponent guesses an attack
							else if (movePlayer == "d" && moveOpponent == "ga")
							{
								float opponentHp = 0;
								int opponentID = 0;
								float playerHp = 0;

								for (auto& player : players)
								{
									//find opponent in player list
									if (player->GetID() != playerID)
									{
										//opponent lose 20hp
										opponentHp = player->GetHp();
										opponentHp -= 20;
										player->SetHp(opponentHp);
										opponentID = player->GetID();

									}
									else //get player data
									{
										playerHp = player->GetHp();
									}
								}

								//Player -> win game scenario
								if (opponentHp <= 0)
								{
									for (auto mClient : clients)
									{
										if (mClient != client)
										{
											sf::Packet sendPacket;
											std::string sendMsg = "You lost..";
											Notif notif(NotifType::GAME_END_NOTIF);
											sendPacket << notif.GetID() << sendMsg;
											if (mClient->send(sendPacket) != sf::Socket::Done)
											{
												//error
											}
										}
									}

									sf::Packet sendPacket;
									Notif notif(NotifType::GAME_END_NOTIF);
									std::string sendMsg = "You won!";
									sendPacket << notif.GetID() << sendMsg;
									if (client->send(sendPacket) != sf::Socket::Done)
									{
										//error
									}

								}

								//Player -> win turn scenario
								else
								{
									for (auto mClient : clients)
									{
										if (mClient != client)
										{
											sf::Packet sendPacket;
											std::string sendMsg =
												"bad intentions leads to bad intuitions, Luck Gods are discontent, you lost 20 hp \n and you will be the last player to make a move next turn";

											Notif notif(NotifType::TURN_RESULT_NOTIF);
											sendPacket << notif.GetID() << sendMsg << opponentHp << playerHp << opponentID;
											if (mClient->send(sendPacket) != sf::Socket::Done)
											{
												//error
											}
										}
									}

									sf::Packet sendPacket;
									Notif notif(NotifType::MOVE_CHOICE_NOTIF);
									std::string sendMsg =
										"Your opponent toyed with Luck Gods and lost! \n and you will be the first to make a move next turn\n\n\n";
									sendMsg += "LUCK [ " + std::to_string(minLuckPlayer) + ";" + std::to_string(maxLuckPlayer) + " ]\n";
									sendPacket << notif.GetID() << sendMsg << playerHp << opponentHp << playerID;
									if (client->send(sendPacket) != sf::Socket::Done)
									{
										//error
									}

								}
							}

							//p1 defends and opponent guesses defense
							else if (movePlayer == "d" && moveOpponent == "gd")
							{
								float opponentHp = 0;
								int opponentID = 0;
								float playerHp = 0;

								for (auto& player : players)
								{
									//find player in player list
									if (player->GetID() == playerID)
									{
										//get player datas
										playerHp = player->GetHp();
										player->SetHp(playerHp);
									}
									else //get opponent data and rise luck
									{
										opponentHp = player->GetHp();
										opponentID = player->GetID();
										player->RiseLuck();
										minLuckOpponent = player->GetMinLuck();
										maxLuckOpponent = player->GetMaxLuck();
									}
								}

								for (auto mClient : clients)
								{
									if (mClient != client)
									{
										sf::Packet sendPacket;
										Notif notif(NotifType::MOVE_CHOICE_NOTIF);
										std::string sendMsg =
											"Your intuition was good, the Luck Gods decided to rise your luck ! \n and you will be the first to make a move next turn\n\n\n";
										sendMsg += "LUCK [ " + std::to_string(minLuckOpponent) + ";" + std::to_string(maxLuckOpponent) + " ]\n";


										sendPacket << notif.GetID() << sendMsg << playerHp << opponentHp << playerID;
										if (mClient->send(sendPacket) != sf::Socket::Done)
										{
											//error
										}
									}

								}

								sf::Packet sendPacket;
								std::string sendMsg =
									"Your opponent did not attack. He intuitively knew you were about to raise your defense, hence his intuition sharpened and his luck rose to new heights.. \n and you will be the last to make a move next turn";

								Notif notif(NotifType::TURN_RESULT_NOTIF);
								sendPacket << notif.GetID() << sendMsg << opponentHp << playerHp << opponentID;
								if (client->send(sendPacket) != sf::Socket::Done)
								{
									//error
								}
							}

							//p1 guesses an attack and opponent attacks
							else if (movePlayer == "ga" && moveOpponent == "a")
							{

								float opponentHp = 0;
								int opponentID = 0;
								float playerHp = 0;

								for (auto& player : players)
								{
									//find opponent in player list
									if (player->GetID() != playerID)
									{
										//opponent get opponent datas
										opponentHp = player->GetHp();					
										opponentID = player->GetID();
										player->SetHp(opponentHp);
									}
									else //get player data and rise luck
									{
										playerHp = player->GetHp();
										player->RiseLuck();
										minLuckPlayer = player->GetMinLuck();
										maxLuckPlayer = player->GetMaxLuck();
									}
								}

								for (auto mClient : clients)
								{
									if (mClient != client)
									{
										sf::Packet sendPacket;
										std::string sendMsg =
											"Your opponent did evade from your attack, hence his intuition sharpened and his luck rose to new heights.. \n and you will be the last to make a move next turn";

										Notif notif(NotifType::TURN_RESULT_NOTIF);
										sendPacket << notif.GetID() << sendMsg << opponentHp << playerHp << opponentID;
										if (mClient->send(sendPacket) != sf::Socket::Done)
										{
											//error
										}
									}

								}

								sf::Packet sendPacket;
								Notif notif(NotifType::MOVE_CHOICE_NOTIF);
								std::string sendMsg =
									"Your intuition was good, the Luck Gods decided to rise your luck ! \n and you will be the first to make a move next turn\n\n\n";
								sendMsg += "LUCK [ " + std::to_string(minLuckPlayer) + ";" + std::to_string(maxLuckPlayer) + " ]\n";
								sendPacket << notif.GetID() << sendMsg << playerHp << opponentHp << playerID;
								if (client->send(sendPacket) != sf::Socket::Done)
								{
									//error
								}

							}

							//p1 guesses an attack and opponent defends
							else if (movePlayer == "ga" && moveOpponent == "d")
							{

								float opponentHp = 0;
								int opponentID = 0;
								float playerHp = 0;

								for (auto& player : players)
								{
									//find player in player list
									if (player->GetID() == playerID)
									{
										//player lose 20hp
										playerHp = player->GetHp();
										playerHp -= 20;
										player->SetHp(playerHp);
										
									}
									else //get opponent data
									{
										opponentHp = player->GetHp();
										opponentID = player->GetID();
									}
								}

								//Player -> lose game scenario
								if (playerHp <= 0)
								{
									for (auto mClient : clients)
									{
										if (mClient != client)
										{
											sf::Packet sendPacket;
											std::string sendMsg = "You won!";
											Notif notif(NotifType::GAME_END_NOTIF);
											sendPacket << notif.GetID() << sendMsg;
											if (mClient->send(sendPacket) != sf::Socket::Done)
											{
												//error
											}
										}
									}

									sf::Packet sendPacket;
									Notif notif(NotifType::GAME_END_NOTIF);
									std::string sendMsg = "You lost..";
									sendPacket << notif.GetID() << sendMsg;
									if (client->send(sendPacket) != sf::Socket::Done)
									{
										//error
									}

								}

								else
								{
									//Player -> lose turn scenario
									for (auto mClient : clients)
									{
										if (mClient != client)
										{
											sf::Packet sendPacket;
											std::string sendMsg =
												"Your opponent toyed with Luck Gods and lost! \n and you will be the first to make a move next turn\n\n\n";
											sendMsg += "LUCK [ " + std::to_string(minLuckOpponent) + ";" + std::to_string(maxLuckOpponent) + " ]\n";

											Notif notif(NotifType::MOVE_CHOICE_NOTIF);
											sendPacket << notif.GetID() << sendMsg << opponentHp << playerHp << opponentID;
											if (mClient->send(sendPacket) != sf::Socket::Done)
											{
												//error
											}
										}
									}

									sf::Packet sendPacket;
									Notif notif(NotifType::TURN_RESULT_NOTIF);
									std::string sendMsg =
										"bad intentions leads to bad intuitions, Luck Gods are discontent, you lost 20 hp \n and you will be the last player to make a move next turn";
									sendPacket << notif.GetID() << sendMsg << playerHp << opponentHp << playerID;
									if (client->send(sendPacket) != sf::Socket::Done)
									{
										//error
									}
									
								}
							}

							//p1 guesses a defense and opponent defend
							else if (movePlayer == "gd" && moveOpponent == "d")
							{
								float opponentHp = 0;
								int opponentID = 0;
								float playerHp = 0;

								for (auto& player : players)
								{
									//find opponent in player list
									if (player->GetID() != playerID)
									{
										//opponent get opponent datas
										opponentHp = player->GetHp();
										opponentID = player->GetID();
										player->SetHp(opponentHp);
									}
									else //get player data and rise luck
									{
										playerHp = player->GetHp();
										player->RiseLuck();
										minLuckPlayer = player->GetMinLuck();
										maxLuckPlayer = player->GetMaxLuck();
									}
								}

								for (auto mClient : clients)
								{
									if (mClient != client)
									{
										sf::Packet sendPacket;
										std::string sendMsg =
											"Your opponent did not attack. He intuitively knew you were about to raise your defense, hence his intuition sharpened and his luck rose to new heights.. \n and you will be the last to make a move next turn";

										Notif notif(NotifType::TURN_RESULT_NOTIF);
										sendPacket << notif.GetID() << sendMsg << opponentHp << playerHp << opponentID;
										if (mClient->send(sendPacket) != sf::Socket::Done)
										{
											//error
										}
									}

								}

								sf::Packet sendPacket;
								Notif notif(NotifType::MOVE_CHOICE_NOTIF);
								std::string sendMsg =
									"Your intuition was good, the Luck Gods decided to rise your luck ! \n and you will be the first to make a move next turn\n\n\n";
								sendMsg += "LUCK [ " + std::to_string(minLuckPlayer) + ";" + std::to_string(maxLuckPlayer) + " ]\n";
								sendPacket << notif.GetID() << sendMsg << playerHp << opponentHp << playerID;
								if (client->send(sendPacket) != sf::Socket::Done)
								{
									//error
								}
							}

							//p1 guesses a defense and opponent attack
							else if (movePlayer == "gd" && moveOpponent == "a")
							{
								float opponentHp = 0;
								int opponentID = 0;
								float playerHp = 0;

								for (auto& player : players)
								{
									//find player in player list
									if (player->GetID() == playerID)
									{
										//player lose 20hp
										playerHp = player->GetHp();
										playerHp -= 20;
										player->SetHp(playerHp);

									}
									else //get opponent data
									{
										opponentHp = player->GetHp();
										opponentID = player->GetID();
									}
								}

								//Player -> lose game scenario
								if (playerHp <= 0)
								{
									for (auto mClient : clients)
									{
										if (mClient != client)
										{
											sf::Packet sendPacket;
											std::string sendMsg = "You won!";
											Notif notif(NotifType::GAME_END_NOTIF);
											sendPacket << notif.GetID() << sendMsg;
											if (mClient->send(sendPacket) != sf::Socket::Done)
											{
												//error
											}
										}
									}

									sf::Packet sendPacket;
									Notif notif(NotifType::GAME_END_NOTIF);
									std::string sendMsg = "You lost..";
									sendPacket << notif.GetID() << sendMsg;
									if (client->send(sendPacket) != sf::Socket::Done)
									{
										//error
									}

								}

								else
								{
									//Player -> lose turn scenario
									for (auto mClient : clients)
									{
										if (mClient != client)
										{
											sf::Packet sendPacket;
											std::string sendMsg =
												"Your opponent toyed with Luck Gods and lost! \n and you will be the first to make a move next turn\n\n\n";
											sendMsg += "LUCK [ " + std::to_string(minLuckOpponent) + ";" + std::to_string(maxLuckOpponent) + " ]\n";

											Notif notif(NotifType::MOVE_CHOICE_NOTIF);
											sendPacket << notif.GetID() << sendMsg << opponentHp << playerHp << opponentID;
											if (mClient->send(sendPacket) != sf::Socket::Done)
											{
												//error
											}
										}
									}

									sf::Packet sendPacket;
									Notif notif(NotifType::TURN_RESULT_NOTIF);
									std::string sendMsg =
										"bad intentions leads to bad intuitions, Luck Gods are discontent, you lost 20 hp \n and you will be the last player to make a move next turn";
									sendPacket << notif.GetID() << sendMsg << playerHp << opponentHp << playerID;
									if (client->send(sendPacket) != sf::Socket::Done)
									{
										//error
									}
								}
							}
						}				
						else
						{
							float hpOpponent = 0;
							int minLuckOpponent = 0;
							int maxLuckOpponent = 0;
							float hpPlayerr = 0;
							int scorePlayer = 0;
							std::string movePlayer;
							int idOpponent = 0;
							
							for (auto& player : players)
							{
								if (player->GetID() == playerID)
								{
									hpPlayerr = player->GetHp();
									scorePlayer = player->GetScore();
									movePlayer = player->GetMove();
								}	
								else
								{
									idOpponent = player->GetID();
									hpOpponent = player->GetHp();
									minLuckOpponent = player->GetMinLuck();
									maxLuckOpponent = player->GetMaxLuck();
								}
							}
							
							//envoi					
							for (auto mClient : clients)
							{
								if (mClient != client)
								{
									sf::Packet sendPacket;
									std::string sendMsg =
										"\nOpponent roll score: " + std::to_string(scorePlayer)
										+ "\n\nLUCK [ " + std::to_string(minLuckOpponent) + ";" + std::to_string(maxLuckOpponent) + " ]\n";
										
									Notif notif(NotifType::MOVE_CHOICE_GUESS_OPTION_NOTIF);
									sendPacket << notif.GetID() << sendMsg << hpOpponent << hpPlayerr << idOpponent;
									if (mClient->send(sendPacket) != sf::Socket::Done)
									{
										//error
									}
								}
							}
						}
						
					}
				}
			}
		}
	}	
}

  

