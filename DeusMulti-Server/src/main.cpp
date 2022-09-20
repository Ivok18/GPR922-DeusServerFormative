#include <iostream>
#include <SFML/Network.hpp>
#include <Player.h>
#include <Notif.h>
#include <PacketOperatorOverload.h>

void RunPlayerWinGameScenario(sf::TcpSocket* client, std::vector<sf::TcpSocket*> clients)
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

void RunPlayerWinTurnScenario(sf::TcpSocket* client, std::vector<sf::TcpSocket*> clients, std::string msgToPlayer,
	std::string msgToOpponent, float& playerHp, float& opponentHp, int& minLuckPlayer, int& maxLuckPlayer)
{
	for (auto mClient : clients)
	{
		if (mClient != client)
		{
			sf::Packet sendPacket;
			Notif notif(NotifType::TURN_RESULT_NOTIF);
			sendPacket << notif.GetID() << msgToOpponent << opponentHp << playerHp;
			if (mClient->send(sendPacket) != sf::Socket::Done)
			{
				//error
			}
		}
	}

	sf::Packet sendPacket;
	Notif notif(NotifType::MOVE_CHOICE_NOTIF);
	sendPacket << notif.GetID() << msgToPlayer << playerHp << opponentHp;
	if (client->send(sendPacket) != sf::Socket::Done)
	{
		//error
	}
}

void RunPlayerLoseGameScenario(sf::TcpSocket* client, std::vector<sf::TcpSocket*> clients)
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

void RunPlayerLoseTurnScenario(sf::TcpSocket* client, std::vector<sf::TcpSocket*> clients, std::string msgToPlayer,
	std::string msgToOpponent, float& playerHp, float& opponentHp, int& minLuckOpponent, int& maxLuckOpponent)
{
	for (auto mClient : clients)
	{
		if (mClient != client)
		{
			sf::Packet sendPacket;
			Notif notif(NotifType::MOVE_CHOICE_NOTIF);
			sendPacket << notif.GetID() << msgToOpponent << opponentHp << playerHp;
			if (mClient->send(sendPacket) != sf::Socket::Done)
			{
				//error
			}
		}
	}

	sf::Packet sendPacket;
	Notif notif(NotifType::TURN_RESULT_NOTIF);

	sendPacket << notif.GetID() << msgToPlayer << playerHp << opponentHp;
	if (client->send(sendPacket) != sf::Socket::Done)
	{
		//error
	}
}

void AnalyseCurrTurnDatas(sf::TcpSocket* client, std::vector<sf::TcpSocket*> clients, std::vector<std::unique_ptr<Player>>& players, int& playerID)
{
	//clear console before printing players datas
	system("cls");
	std::cout << "PLAYERS THAT HAVE PLAYED" << std::endl;

	//debug
	for (auto& player : players)
	{
		player->PrintData(); std::cout << std::endl << std::endl << std::endl;
	}


	//traitement des données du tour de chaque joueur
	int scorePlayer = 0;
	std::string movePlayer;
	int classsIDrefPlayer = 0;
	int scoreOpponent = 0;
	std::string moveOpponent;
	int  classsIDRefOpponent = 0;
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
			classsIDrefPlayer = player->GetClassIDRef();
			minLuckPlayer = player->GetMinLuck();
			maxLuckPlayer = player->GetMaxLuck();
		}
		else
		{
			scoreOpponent = player->GetScore();
			moveOpponent = player->GetMove();
			classsIDRefOpponent = player->GetClassIDRef();
			minLuckOpponent = player->GetMinLuck();
			maxLuckOpponent = player->GetMaxLuck();
		}
	}

	//debug
	std::cout << scorePlayer << std::endl;
	std::cout << movePlayer << std::endl;
	std::cout << scoreOpponent << std::endl;
	std::cout << moveOpponent << std::endl << std::endl;


	//p1 and opponent attack CHECK
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
					player->SetHp(opponentHp);
				}
				else //get player hp
				{
					playerHp = player->GetHp();
				}
			}

			//Player -> win game scenario
			if (opponentHp <= 0)
				RunPlayerWinGameScenario(client, clients);

			//Player -> win turn scenario
			else
			{
				std::string msgToPlayer = "Your opponent was hit succesfully ! \n and you will be the first to make a move next turn\n\n\n";
				msgToPlayer += "LUCK [ " + std::to_string(minLuckPlayer) + ";" + std::to_string(maxLuckPlayer) + " ]\n";

				std::string msgToOpponent = "Your opponent attack was stronger, you lost 10 hp \n and you will be the last player to make a move next turn";

				RunPlayerWinTurnScenario(client, clients, msgToPlayer, msgToOpponent, playerHp, opponentHp, minLuckPlayer,
					maxLuckPlayer);
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
				RunPlayerLoseGameScenario(client, clients);

			//Player -> lose turn scenario
			else
			{
				std::string msgToOpponent =
					"Your opponent was hit succesfully ! \n and you will be the first to make a move next turn\n\n\n";
				msgToOpponent += "LUCK [ " + std::to_string(minLuckOpponent) + ";" + std::to_string(maxLuckOpponent) + " ]\n";

				std::string msgToPlayer =
					"Your opponent attack was stronger, you lost 10 hp \n and you will be the last player to make a move next turn";

				RunPlayerLoseTurnScenario(client, clients, msgToPlayer, msgToOpponent, playerHp, opponentHp, minLuckOpponent,
					maxLuckPlayer);

			}
		}
	}

	//p1 attack and opponent defends CHECK
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
				RunPlayerWinGameScenario(client, clients);

			//Player -> win turn scenario
			else
			{
				std::string msgToPlayer =
					"You managed to hit your opponent in spite of his defense ! \n and you will be the first to make a move next turn\n\n\n";
				msgToPlayer += "LUCK [ " + std::to_string(minLuckPlayer) + ";" + std::to_string(maxLuckPlayer) + " ]\n";

				std::string msgToOpponent =
					"Your opponent attack managed to hit you in spite of your defense, you lost 5 hp \n and you will be the last player to make a move next turn";

				RunPlayerWinTurnScenario(client, clients, msgToPlayer, msgToOpponent, playerHp, opponentHp, minLuckPlayer, maxLuckPlayer);
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
					player->SetHp(opponentHp);
				}
				else //find player
				{
					playerHp = player->GetHp();
				}
			}

			std::string msgToOpponent =
				"Your successfully blocked your opponent attack! \n and you will be the first to make a move next turn\n\n\n";
			msgToOpponent += "LUCK [ " + std::to_string(minLuckOpponent) + ";" + std::to_string(maxLuckOpponent) + " ]\n";

			std::string msgToPlayer =
				"Your opponent blocked your attack \n and you will be the last player to make a move next turn";

			RunPlayerLoseTurnScenario(client, clients, msgToPlayer, msgToOpponent, playerHp, opponentHp, minLuckOpponent, maxLuckOpponent);

		}

	}

	//p1 attacks and opponent guesses attack CHECK
	else if (movePlayer == "a" && moveOpponent == "ga")
	{
		float opponentHp = 0;
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
				player->RiseLuck();
				minLuckOpponent = player->GetMinLuck();
				maxLuckOpponent = player->GetMaxLuck();
			}
		}

		//Player lose turn scenario
		std::string msgToOpponent =
			"Your intuition was good, the Luck Gods decided to rise your luck ! \n and you will be the first to make a move next turn\n\n\n";
		msgToOpponent += "LUCK [ " + std::to_string(minLuckOpponent) + ";" + std::to_string(maxLuckOpponent) + " ]\n";

		std::string msgToPlayer =
			"Your opponent did evade from your attack, hence his intuition sharpened and his luck rose to new heights.. \n and you will be the last to make a move next turn";

		RunPlayerLoseTurnScenario(client, clients, msgToPlayer, msgToOpponent, playerHp, opponentHp, minLuckOpponent, maxLuckOpponent);

	}

	//p1 attacks and opponent guesses a defense CHECK
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
			RunPlayerWinGameScenario(client, clients);

		//Player -> win turn scenario
		else
		{
			std::string msgToOpponent =
				"bad intentions leads to bad intuitions, Luck Gods are discontent, you lost 20 hp \n and you will be the last player to make a move next turn";

			std::string msgToPlayer =
				"Your opponent toyed with Luck Gods and lost! \n and you will be the first to make a move next turn\n\n\n";
			msgToPlayer += "LUCK [ " + std::to_string(minLuckPlayer) + ";" + std::to_string(maxLuckPlayer) + " ]\n";

			RunPlayerWinTurnScenario(client, clients, msgToPlayer, msgToOpponent, playerHp, opponentHp, minLuckPlayer, maxLuckPlayer);

		}
	}

	//p1 defends and opponent attacks CHECK
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
			std::string msgToOpponent =
				"Your opponent blocked your attack \n and you will be the last player to make a move next turn";

			std::string msgToPlayer =
				"Your successfully blocked your opponent attack! \n and you will be the first to make a move next turn\n\n\n";
			msgToPlayer += "LUCK [ " + std::to_string(minLuckPlayer) + ";" + std::to_string(maxLuckPlayer) + " ]\n";

			RunPlayerWinTurnScenario(client, clients, msgToPlayer, msgToOpponent, playerHp, opponentHp, minLuckPlayer, maxLuckPlayer);
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
				RunPlayerLoseGameScenario(client, clients);

			//Player -> lose turn scenario
			else
			{
				std::string msgToOpponent =
					"You managed to hit your opponent in spite of his defense ! \n and you will be the first to make a move next turn\n\n\n";
				msgToOpponent += "LUCK [ " + std::to_string(minLuckOpponent) + ";" + std::to_string(maxLuckOpponent) + " ]\n";

				std::string msgToPlayer =
					"Your opponent attack managed to hit you in spite of your defense, you lost 5 hp \n and you will be the last player to make a move next turn";

				RunPlayerLoseTurnScenario(client, clients, msgToPlayer, msgToOpponent, playerHp, opponentHp, minLuckOpponent, maxLuckOpponent);
			}
		}
	}

	//p1 and opponent defend CHECK
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
					player->SetHp(opponentHp);
				}
				else //get player hp
				{
					playerHp = player->GetHp();
				}
			}

			//Player -> win game scenario
			if (opponentHp <= 0)
				RunPlayerWinGameScenario(client, clients);

			//Player -> win turn scenario
			else
			{
				std::string msgToOpponent =
					"Clash of shields ! But your opponent's was stronger, you lost 3 hp \n and you will be the last player to make a move next turn";

				std::string msgToPlayer =
					"You successfully hit your opponent with your shield ! \n and you will be the first to make a move next turn\n\n\n";
				msgToPlayer += "LUCK [ " + std::to_string(minLuckPlayer) + ";" + std::to_string(maxLuckPlayer) + " ]\n";

				RunPlayerWinTurnScenario(client, clients, msgToPlayer, msgToOpponent, playerHp, opponentHp, minLuckPlayer, maxLuckPlayer);
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
						sendPacket << notif.GetID() << sendMsg << opponentHp << playerHp;
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

	//p1 defends and opponent guesses an attack CHECK
	else if (movePlayer == "d" && moveOpponent == "ga")
	{
		float opponentHp = 0;
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

			}
			else //get player data
			{
				playerHp = player->GetHp();
			}
		}

		//Player -> win game scenario
		if (opponentHp <= 0)
			RunPlayerWinGameScenario(client, clients);

		//Player -> win turn scenario
		else
		{
			std::string msgToOpponent =
				"bad intentions leads to bad intuitions, Luck Gods are discontent, you lost 20 hp \n and you will be the last player to make a move next turn";

			std::string msgToPlayer =
				"Your opponent toyed with Luck Gods and lost! \n and you will be the first to make a move next turn\n\n\n";
			msgToPlayer += "LUCK [ " + std::to_string(minLuckPlayer) + ";" + std::to_string(maxLuckPlayer) + " ]\n";

			RunPlayerWinTurnScenario(client, clients, msgToPlayer, msgToOpponent, playerHp, opponentHp, minLuckPlayer, maxLuckPlayer);
		}
	}

	//p1 defends and opponent guesses defense CHECK
	else if (movePlayer == "d" && moveOpponent == "gd")
	{
		float opponentHp = 0;
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
				player->RiseLuck();
				minLuckOpponent = player->GetMinLuck();
				maxLuckOpponent = player->GetMaxLuck();
			}
		}

		//Player lose turn scenario
		std::string msgToOpponent =
			"Your intuition was good, the Luck Gods decided to rise your luck ! \n and you will be the first to make a move next turn\n\n\n";
		msgToOpponent += "LUCK [ " + std::to_string(minLuckOpponent) + ";" + std::to_string(maxLuckOpponent) + " ]\n";

		std::string msgToPlayer =
			"Your opponent did not attack. He intuitively knew you were about to raise your defense, hence his intuition sharpened and his luck rose to new heights.. \n and you will be the last to make a move next turn";

		RunPlayerLoseTurnScenario(client, clients, msgToPlayer, msgToOpponent, playerHp, opponentHp, minLuckOpponent, maxLuckOpponent);

	}

	//p1 guesses an attack and opponent attacks CHECK
	else if (movePlayer == "ga" && moveOpponent == "a")
	{

		float opponentHp = 0;
		float playerHp = 0;

		for (auto& player : players)
		{
			//find opponent in player list
			if (player->GetID() != playerID)
			{
				//opponent get opponent datas
				opponentHp = player->GetHp();
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

		std::string msgToOpponent =
			"Your opponent did evade from your attack, hence his intuition sharpened and his luck rose to new heights.. \n and you will be the last to make a move next turn";

		std::string msgToPlayer =
			"Your intuition was good, the Luck Gods decided to rise your luck ! \n and you will be the first to make a move next turn\n\n\n";
		msgToPlayer += "LUCK [ " + std::to_string(minLuckPlayer) + ";" + std::to_string(maxLuckPlayer) + " ]\n";


		RunPlayerWinTurnScenario(client, clients, msgToPlayer, msgToOpponent, playerHp, opponentHp, minLuckPlayer, maxLuckPlayer);
	}

	//p1 guesses an attack and opponent defends CHECK
	else if (movePlayer == "ga" && moveOpponent == "d")
	{

		float opponentHp = 0;
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
			}
		}

		//Player -> lose game scenario
		if (playerHp <= 0)
			RunPlayerLoseGameScenario(client, clients);

		else
		{
			std::string msgToOpponent =
				"Your opponent toyed with Luck Gods and lost! \n and you will be the first to make a move next turn\n\n\n";
			msgToOpponent += "LUCK [ " + std::to_string(minLuckOpponent) + ";" + std::to_string(maxLuckOpponent) + " ]\n";

			std::string msgToPlayer =
				"bad intentions leads to bad intuitions, Luck Gods are discontent, you lost 20 hp \n and you will be the last player to make a move next turn";

			RunPlayerLoseTurnScenario(client, clients, msgToPlayer, msgToOpponent, playerHp, opponentHp, minLuckOpponent, maxLuckOpponent);
		}
	}

	//p1 guesses a defense and opponent defend CHECK
	else if (movePlayer == "gd" && moveOpponent == "d")
	{
		float opponentHp = 0;
		float playerHp = 0;

		for (auto& player : players)
		{
			//find opponent in player list
			if (player->GetID() != playerID)
			{
				//opponent get opponent datas
				opponentHp = player->GetHp();
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


		//Player win turn scenario
		std::string msgToOpponent =
			"Your opponent did not attack. He intuitively knew you were about to raise your defense, hence his intuition sharpened and his luck rose to new heights.. \n and you will be the last to make a move next turn";

		std::string msgToPlayer =
			"Your intuition was good, the Luck Gods decided to rise your luck ! \n and you will be the first to make a move next turn\n\n\n";
		msgToPlayer += "LUCK [ " + std::to_string(minLuckPlayer) + ";" + std::to_string(maxLuckPlayer) + " ]\n";

		RunPlayerWinTurnScenario(client, clients, msgToPlayer, msgToOpponent, playerHp, opponentHp, minLuckPlayer, maxLuckPlayer);
	}

	//p1 guesses a defense and opponent attack CHECK
	else if (movePlayer == "gd" && moveOpponent == "a")
	{
		float opponentHp = 0;
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
			}
		}

		//Player -> lose game scenario
		if (playerHp <= 0)
			RunPlayerLoseGameScenario(client, clients);

		else
		{

			std::string msgToOpponent =
				"Your opponent toyed with Luck Gods and lost! \n and you will be the first to make a move next turn\n\n\n";
			msgToOpponent += "LUCK [ " + std::to_string(minLuckOpponent) + ";" + std::to_string(maxLuckOpponent) + " ]\n";

			std::string msgToPlayer =
				"bad intentions leads to bad intuitions, Luck Gods are discontent, you lost 20 hp \n and you will be the last player to make a move next turn";

			RunPlayerLoseTurnScenario(client, clients, msgToPlayer, msgToOpponent, playerHp, opponentHp, minLuckOpponent, maxLuckOpponent);

		}
	}

	//p1 and opponent wanna go super move
	else if (movePlayer == "s" && moveOpponent == "s")
	{
		float opponentHp = 0;
		float playerHp = 0;

		
		//player and opponent wanted to go super move but did not have enough luck power
		if (maxLuckPlayer - 40 < 0 && maxLuckOpponent - 40 < 0)
		{
			for (auto& player : players)
			{
				//find player in player list
				if (player->GetID() == playerID)
				{
					//player lose 40hp
					playerHp = player->GetHp();
					playerHp -= 40;
					player->SetHp(playerHp);

				}
				else //opponent lose 30hp
				{
					opponentHp = player->GetHp();
					opponentHp -= 40;
					player->SetHp(opponentHp);
				}
			}

			//player higher score
			if (scorePlayer > scoreOpponent)
			{
				if (playerHp <= 0 && opponentHp > 0)
					RunPlayerLoseGameScenario(client, clients);
				else if(playerHp <= 0 && opponentHp <= 0)
					RunPlayerWinGameScenario(client, clients);
				else
				{
					std::string msgToPlayer =
						"Be aware of yout limits! You did not have enough power but you tried regardles \n.Luck Gods decided to punish your lack of judgment. You lost 40hp\n\n\n";
					msgToPlayer += "LUCK[" + std::to_string(minLuckPlayer) + "; " + std::to_string(maxLuckPlayer) + "]\n";

					std::string msgToOpponent =
						"Your greed for power was too offensive for Luck Gods \n they decided to punish you, and you lost 40hp";

					RunPlayerWinTurnScenario(client, clients, msgToPlayer, msgToOpponent, playerHp, opponentHp, minLuckPlayer, maxLuckPlayer);
				}
			}
			
			//opponent higher score
			if (scorePlayer < scoreOpponent)
			{
				if (playerHp <= 0 && opponentHp > 0)
					RunPlayerLoseGameScenario(client, clients);
				else if (playerHp <= 0 && opponentHp <= 0)
					RunPlayerLoseGameScenario(client, clients);
				else
				{
					std::string msgToOpponent =
						"Be aware of yout limits! You did not have enough power but you tried regardles \n.Luck Gods decided to punish your lack of judgment. You lost 40hp\n\n\n";
					msgToOpponent += "LUCK[" + std::to_string(minLuckOpponent) + "; " + std::to_string(maxLuckOpponent) + "]\n";

					std::string msgToPlayer =
						"Your greed for power was too offensive for Luck Gods \n they decided to punish you, and you lost 40hp";

					RunPlayerLoseTurnScenario(client, clients, msgToPlayer, msgToOpponent, playerHp, opponentHp, minLuckPlayer, maxLuckPlayer);
				}
			}
		}

		//player and opponent try to land their super move, and they both have enough luck power
		else if (maxLuckPlayer - 40 >= 0 && maxLuckOpponent - 40 >= 0)
		{
			if (scorePlayer > scoreOpponent)
			{
				for (auto& player : players)
				{
					//find player in player list
					if (player->GetID() == playerID)
					{
						playerHp = player->GetHp();
						player->SetHp(playerHp);
						player->DepleteLuck();
						maxLuckPlayer = player->GetMaxLuck();
						minLuckPlayer = player->GetMinLuck();

					}
					else //opponent lose 30hp
					{
						opponentHp = player->GetHp();
						opponentHp -= 40;
						player->DepleteLuck();					
						player->SetHp(opponentHp);
						maxLuckOpponent = player->GetMaxLuck();
						minLuckOpponent = player->GetMinLuck();
					}
				}

				if (opponentHp <= 0)
					RunPlayerWinGameScenario(client, clients);

				else
				{
					if (static_cast<ClassType>(classsIDrefPlayer) == ClassType::MAGE)
					{
						std::string msgToPlayer =
							"Consuming your luck power, you invoke a giant fireball that incinerates your opponent!\n\n\n";
						msgToPlayer+= "LUCK[" + std::to_string(minLuckPlayer) + "; " + std::to_string(maxLuckPlayer) + "]\n";

						std::string msgToOpponent =
							"Giant fireball incinerates you, but your luck power helps you to escape death";

						RunPlayerWinTurnScenario(client, clients, msgToPlayer, msgToOpponent, playerHp, opponentHp, minLuckPlayer, maxLuckPlayer);
					}
					else if(static_cast<ClassType>(classsIDrefPlayer) == ClassType::WARRIOR)
					{
						std::string msgToPlayer =
							"Consuming your luck power, you invoke the greatsword excalibur and you crush your opponent!\n\n\n";
						msgToPlayer += "LUCK[" + std::to_string(minLuckPlayer) + "; " + std::to_string(maxLuckPlayer) + "]\n";

						std::string msgToOpponent =
							"The great sword excalibur crushes you, but your luck power helps you to escape death";

						RunPlayerWinTurnScenario(client, clients, msgToPlayer, msgToOpponent, playerHp, opponentHp, minLuckPlayer, maxLuckPlayer);
					}
				}
			}
			else
			{
				for (auto& player : players)
				{
					//find player in player list
					if (player->GetID() == playerID)
					{
						playerHp = player->GetHp();
						playerHp -= 40;
						player->SetHp(playerHp);
						player->DepleteLuck();
						maxLuckPlayer = player->GetMaxLuck();
						minLuckPlayer = player->GetMinLuck();
						

					}
					else
					{
						opponentHp = player->GetHp();
						player->SetHp(opponentHp);
						player->DepleteLuck();
						maxLuckOpponent = player->GetMaxLuck();
						minLuckOpponent = player->GetMinLuck();
					}
				}

				if (playerHp <= 0)
					RunPlayerLoseGameScenario(client, clients);

				else
				{
					if (static_cast<ClassType>(classsIDRefOpponent) == ClassType::MAGE)
					{
						std::string msgToOpponent =
							"Consuming your luck power, you invoke a giant fireball that incinerates your opponent!\n\n\n";
						msgToOpponent += "LUCK[" + std::to_string(minLuckOpponent) + "; " + std::to_string(maxLuckOpponent) + "]\n";

						std::string msgToPlayer =
							"Giant fireball incinerates you, but your luck power helps you to escape death";

						RunPlayerLoseTurnScenario(client, clients, msgToPlayer, msgToOpponent, playerHp, opponentHp, minLuckOpponent, maxLuckOpponent);
					}
					else if (classsIDRefOpponent == ClassType::WARRIOR)
					{
						std::string msgToOpponent =
							"Consuming your luck power, you invoke the greatsword excalibur and you crush your opponent!\n\n\n";
						msgToOpponent += "LUCK[" + std::to_string(minLuckOpponent) + "; " + std::to_string(maxLuckOpponent) + "]\n";

						std::string msgToPlayer =
							"The great sword excalibur crushes you, but your luck power helps you to escape death";

						RunPlayerLoseTurnScenario(client, clients, msgToPlayer, msgToOpponent, playerHp, opponentHp, minLuckOpponent, maxLuckOpponent);
					}
				}
			}
		}

		//player and opponent try to land their super move, but opponent doesnt have enough luck power
		else if (maxLuckPlayer - 40 >= 0 && maxLuckOpponent - 40 < 0)
		{
			for (auto& player : players)
			{
				//find player in player list
				if (player->GetID() == playerID)
				{
					playerHp = player->GetHp();
					player->SetHp(playerHp);
					player->DepleteLuck();
					maxLuckPlayer = player->GetMaxLuck();
					minLuckPlayer = player->GetMinLuck();

				}
				else //opponent lose 30hp
				{
					opponentHp = player->GetHp();
					opponentHp -= 40;
					player->SetHp(opponentHp);
				}
			}

			if (opponentHp <= 0)
				RunPlayerWinGameScenario(client, clients);

			else
			{
				if (static_cast<ClassType>(classsIDrefPlayer) == ClassType::MAGE)
				{
					std::string msgToPlayer =
						"Consuming your luck power, you invoke a giant fireball that incinerates your opponent!\n\n\n";
					msgToPlayer += "LUCK[" + std::to_string(minLuckPlayer) + "; " + std::to_string(maxLuckPlayer) + "]\n";

					std::string msgToOpponent =
						"Giant fireball incinerates you, but your luck power helps you to escape death";

					RunPlayerWinTurnScenario(client, clients, msgToPlayer, msgToOpponent, playerHp, opponentHp, minLuckPlayer, maxLuckPlayer);
				}
				else if (static_cast<ClassType>(classsIDrefPlayer) == ClassType::WARRIOR)
				{
					std::string msgToPlayer =
						"Consuming your luck power, you invoke the greatsword excalibur and you crush your opponent!\n\n\n";
					msgToPlayer += "LUCK[" + std::to_string(minLuckPlayer) + "; " + std::to_string(maxLuckPlayer) + "]\n";

					std::string msgToOpponent =
						"The great sword excalibur crushes you, but your luck power helps you to escape death";

					RunPlayerWinTurnScenario(client, clients, msgToPlayer, msgToOpponent, playerHp, opponentHp, minLuckPlayer, maxLuckPlayer);
				}
			}
		}

		//player and opponent try to land their super move, but player doesnt have enough luck power
		else if(maxLuckPlayer - 40 < 0 && maxLuckOpponent - 40 >= 0)
		{
			for (auto& player : players)
			{
				//find player in player list
				if (player->GetID() == playerID)
				{
					playerHp = player->GetHp();
					playerHp -= 40;
					player->SetHp(playerHp);


				}
				else
				{
					opponentHp = player->GetHp();
					player->SetHp(opponentHp);
					player->DepleteLuck();
					maxLuckOpponent = player->GetMaxLuck();
					minLuckOpponent = player->GetMinLuck();
				}
			}

			if (playerHp <= 0)
				RunPlayerLoseGameScenario(client, clients);

			else
			{
				if (static_cast<ClassType>(classsIDRefOpponent) == ClassType::MAGE)
				{
					std::string msgToOpponent =
						"Consuming your luck power, you invoke a giant fireball that incinerates your opponent!\n\n\n";
					msgToOpponent += "LUCK[" + std::to_string(minLuckOpponent) + "; " + std::to_string(maxLuckOpponent) + "]\n";

					std::string msgToPlayer =
						"Giant fireball incinerates you, but your luck power helps you to escape death";

					RunPlayerLoseTurnScenario(client, clients, msgToPlayer, msgToOpponent, playerHp, opponentHp, minLuckOpponent, maxLuckOpponent);
				}
				else if (classsIDRefOpponent == ClassType::WARRIOR)
				{
					std::string msgToOpponent =
						"Consuming your luck power, you invoke the greatsword excalibur and you crush your opponent!\n\n\n";
					msgToOpponent += "LUCK[" + std::to_string(minLuckOpponent) + "; " + std::to_string(maxLuckOpponent) + "]\n";

					std::string msgToPlayer =
						"The great sword excalibur crushes you, but your luck power helps you to escape death";

					RunPlayerLoseTurnScenario(client, clients, msgToPlayer, msgToOpponent, playerHp, opponentHp, minLuckOpponent, maxLuckOpponent);
				}
			}
		}


	}

	//p1 wanna go super move and opponent guesses super move
	else if (movePlayer == "s" && moveOpponent == "gs")
	{
		float opponentHp = 0;
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
				player->RiseLuck();
				minLuckOpponent = player->GetMinLuck();
				maxLuckOpponent = player->GetMaxLuck();
			}
		}

		//Player lose turn scenario
		std::string msgToOpponent =
			"Your intuition was good, the Luck Gods decided to rise your luck ! \n and you will be the first to make a move next turn\n\n\n";
		msgToOpponent += "LUCK [ " + std::to_string(minLuckOpponent) + ";" + std::to_string(maxLuckOpponent) + " ]\n";

		std::string msgToPlayer =
			"Your opponent did evade from your super move, hence his intuition sharpened and his luck rose to new heights.. \n and you will be the last to make a move next turn";

		RunPlayerLoseTurnScenario(client, clients, msgToPlayer, msgToOpponent, playerHp, opponentHp, minLuckOpponent, maxLuckOpponent);
	}

	//p1 does something different from super move and opponent guesses super move
	else if (movePlayer != "s" && moveOpponent == "gs")
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
			RunPlayerWinGameScenario(client, clients);

		//Player -> win turn scenario
		else
		{
			std::string msgToOpponent =
				"bad intentions leads to bad intuitions, Luck Gods are discontent, you lost 20 hp \n and you will be the last player to make a move next turn";

			std::string msgToPlayer =
				"Your opponent toyed with Luck Gods and lost! \n and you will be the first to make a move next turn\n\n\n";
			msgToPlayer += "LUCK [ " + std::to_string(minLuckPlayer) + ";" + std::to_string(maxLuckPlayer) + " ]\n";

			RunPlayerWinTurnScenario(client, clients, msgToPlayer, msgToOpponent, playerHp, opponentHp, minLuckPlayer, maxLuckPlayer);

		}
	}

	//p1 guesses super move and opponent does something different from super move
	else if (movePlayer == "gs" && moveOpponent != "s")
	{
		float opponentHp = 0;
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
			}
		}

		//Player -> lose game scenario
		if (playerHp <= 0)
			RunPlayerLoseGameScenario(client, clients);

		else
		{

			std::string msgToOpponent =
				"Your opponent toyed with Luck Gods and lost! \n and you will be the first to make a move next turn\n\n\n";
			msgToOpponent += "LUCK [ " + std::to_string(minLuckOpponent) + ";" + std::to_string(maxLuckOpponent) + " ]\n";

			std::string msgToPlayer =
				"bad intentions leads to bad intuitions, Luck Gods are discontent, you lost 20 hp \n and you will be the last player to make a move next turn";

			RunPlayerLoseTurnScenario(client, clients, msgToPlayer, msgToOpponent, playerHp, opponentHp, minLuckOpponent, maxLuckOpponent);

		}
	}

	//p1 guesses super move and opponent wanna go super move
	else if (movePlayer == "gs" && moveOpponent == "s")
	{
		float opponentHp = 0;
		float playerHp = 0;

		for (auto& player : players)
		{
			//find opponent in player list
			if (player->GetID() != playerID)
			{
				//opponent get opponent datas
				opponentHp = player->GetHp();
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

		std::string msgToOpponent =
			"Your opponent did evade from your super move, hence his intuition sharpened and his luck rose to new heights.. \n and you will be the last to make a move next turn";

		std::string msgToPlayer =
			"Your intuition was good, the Luck Gods decided to rise your luck ! \n and you will be the first to make a move next turn\n\n\n";
		msgToPlayer += "LUCK [ " + std::to_string(minLuckPlayer) + ";" + std::to_string(maxLuckPlayer) + " ]\n";


		RunPlayerWinTurnScenario(client, clients, msgToPlayer, msgToOpponent, playerHp, opponentHp, minLuckPlayer, maxLuckPlayer);
	}

	//p1 wanna go super move and opponent does something else
	else if (movePlayer == "s" && moveOpponent != "s" && moveOpponent != "gs")
	{
		float opponentHp = 0;
		float playerHp = 0;

		//lands super move if enough luck power
		if (maxLuckPlayer - 40 >= 0)
		{
			for (auto& player : players)
			{
				//find player in player list
				if (player->GetID() == playerID)
				{
					playerHp = player->GetHp();
					player->SetHp(playerHp);
					player->DepleteLuck();
					maxLuckPlayer = player->GetMaxLuck();
					minLuckPlayer = player->GetMinLuck();

				}
				else //opponent lose 30hp
				{
					opponentHp = player->GetHp();
					opponentHp -= 40;
					player->SetHp(opponentHp);
				}
			}

			if (opponentHp <= 0)
				RunPlayerWinGameScenario(client, clients);

			else
			{
				if (static_cast<ClassType>(classsIDrefPlayer) == ClassType::MAGE)
				{
					std::string msgToPlayer =
						"Consuming your luck power, you invoke a giant fireball that incinerates your opponent!\n\n\n";
					msgToPlayer += "LUCK[" + std::to_string(minLuckPlayer) + "; " + std::to_string(maxLuckPlayer) + "]\n";

					std::string msgToOpponent =
						"Giant fireball incinerates you, but your luck power helps you to escape death";

					RunPlayerWinTurnScenario(client, clients, msgToPlayer, msgToOpponent, playerHp, opponentHp, minLuckPlayer, maxLuckPlayer);
				}
				else if (static_cast<ClassType>(classsIDrefPlayer) == ClassType::WARRIOR)
				{
					std::string msgToPlayer =
						"Consuming your luck power, you invoke the greatsword excalibur and you crush your opponent!\n\n\n";
					msgToPlayer += "LUCK[" + std::to_string(minLuckPlayer) + "; " + std::to_string(maxLuckPlayer) + "]\n";

					std::string msgToOpponent =
						"The great sword excalibur crushes you, but your luck power helps you to escape death";

					RunPlayerWinTurnScenario(client, clients, msgToPlayer, msgToOpponent, playerHp, opponentHp, minLuckPlayer, maxLuckPlayer);
				}
			}
		}

		//lose life -> not enough luck power
		else if(maxLuckPlayer - 40 < 0)
		{
			for (auto& player : players)
			{
				//find player in player list
				if (player->GetID() == playerID)
				{
					playerHp = player->GetHp();
					playerHp -= 40;
					player->SetHp(playerHp);
				}
				else
				{
					opponentHp = player->GetHp();
					player->SetHp(opponentHp);
				}
			}

			if (playerHp <= 0)
				RunPlayerLoseGameScenario(client, clients);

			else
			{		
				std::string msgToOpponent =
					"Your opponent toyed with Luck Gods and lost! \n and you will be the first to make a move next turn\n\n\n";
				msgToOpponent += "LUCK [ " + std::to_string(minLuckOpponent) + ";" + std::to_string(maxLuckOpponent) + " ]\n";

				std::string msgToPlayer =
					"Your greed for power was too offensive for Luck Gods \n they decided to punish you, and you lost 40hp";

				RunPlayerLoseTurnScenario(client, clients, msgToPlayer, msgToOpponent, playerHp, opponentHp, minLuckOpponent, maxLuckOpponent);
			}
		}
	}

	//p1 does something else and opponent wanna go super move
	else if (movePlayer != "s" && movePlayer != "gs" && moveOpponent == "s")
	{
		float opponentHp = 0;
		float playerHp = 0;

		//lands super move if enough luck power
		if (maxLuckOpponent - 40 >= 0)
		{
			for (auto& player : players)
			{
				//find player in player list
				if (player->GetID() == playerID)
				{
					playerHp = player->GetHp();
					playerHp -= 40;
					player->SetHp(playerHp);


				}
				else
				{
					opponentHp = player->GetHp();
					player->SetHp(opponentHp);
					player->DepleteLuck();
					maxLuckOpponent = player->GetMaxLuck();
					minLuckOpponent = player->GetMinLuck();
				}
			}

			if (playerHp <= 0)
				RunPlayerLoseGameScenario(client, clients);

			else
			{
				if (static_cast<ClassType>(classsIDRefOpponent) == ClassType::MAGE)
				{
					std::string msgToOpponent =
						"Consuming your luck power, you invoke a giant fireball that incinerates your opponent!\n\n\n";
					msgToOpponent += "LUCK[" + std::to_string(minLuckOpponent) + "; " + std::to_string(maxLuckOpponent) + "]\n";

					std::string msgToPlayer =
						"Giant fireball incinerates you, but your luck power helps you to escape death";

					RunPlayerLoseTurnScenario(client, clients, msgToPlayer, msgToOpponent, playerHp, opponentHp, minLuckOpponent, maxLuckOpponent);
				}
				else if (classsIDRefOpponent == ClassType::WARRIOR)
				{
					std::string msgToOpponent =
						"Consuming your luck power, you invoke the greatsword excalibur and you crush your opponent!\n\n\n";
					msgToOpponent += "LUCK[" + std::to_string(minLuckOpponent) + "; " + std::to_string(maxLuckOpponent) + "]\n";

					std::string msgToPlayer =
						"The great sword excalibur crushes you, but your luck power helps you to escape death";

					RunPlayerLoseTurnScenario(client, clients, msgToPlayer, msgToOpponent, playerHp, opponentHp, minLuckOpponent, maxLuckOpponent);
				}
			}
		}

		//opponent lose life -> not enough luck power
		else if (maxLuckOpponent - 40 < 0)
		{
			for (auto& player : players)
			{
				//find player in player list
				if (player->GetID() == playerID)
				{
					//player lose 40hp
					playerHp = player->GetHp();
					player->SetHp(playerHp);

				}
				else //opponent lose 30hp
				{
					opponentHp = player->GetHp();
					opponentHp -= 40;
					player->SetHp(opponentHp);
				}
			}

			if (opponentHp <= 0)
				RunPlayerWinGameScenario(client, clients);

			else
			{
				std::string msgToPlayer =
					"Your opponent toyed with Luck Gods and lost! \n and you will be the first to make a move next turn\n\n\n";
				msgToPlayer += "LUCK [ " + std::to_string(minLuckPlayer) + ";" + std::to_string(maxLuckPlayer) + " ]\n";

				std::string msgToOpponent=
					"Your greed for power was too offensive for Luck Gods \n they decided to punish you, and you lost 40hp";

				RunPlayerWinTurnScenario(client, clients, msgToPlayer, msgToOpponent, playerHp, opponentHp, minLuckOpponent, maxLuckOpponent);
			}
				
		}
	}
}

void ProgressWithTurnAndInformOtherUserOfOpponentMove(sf::TcpSocket* client, std::vector<sf::TcpSocket*> clients, std::vector<std::unique_ptr<Player>>& players,int& playerID)
{
	float hpOpponent = 0;
	int minLuckOpponent = 0;
	int maxLuckOpponent = 0;
	int minLuckPlayer = 0;
	int maxLuckPlayer = 0;
	float hpPlayerr = 0;
	int scorePlayer = 0;
	std::string movePlayer;

	for (auto& player : players)
	{
		if (player->GetID() == playerID)
		{
			hpPlayerr = player->GetHp();
			scorePlayer = player->GetScore();
			movePlayer = player->GetMove();
			minLuckPlayer = player->GetMinLuck();
			maxLuckPlayer = player->GetMaxLuck();
		}
		else
		{
			hpOpponent = player->GetHp();
			minLuckOpponent = player->GetMinLuck();
			maxLuckOpponent = player->GetMaxLuck();
		}
	}
			
	for (auto mClient : clients)
	{
		if (mClient != client)
		{
			sf::Packet sendPacket;
			std::string sendMsg =
				"\nOpponent -LUCK- before turn: [ " + std::to_string(minLuckPlayer) + ";" + std::to_string(maxLuckPlayer) + " ]"
				+"\nOpponent roll score: " + std::to_string(scorePlayer)
				+ "\n\n\nLUCK [ " + std::to_string(minLuckOpponent) + ";" + std::to_string(maxLuckOpponent) + " ]\n";

			Notif notif(NotifType::MOVE_CHOICE_GUESS_OPTION_NOTIF);
			sendPacket << notif.GetID() << sendMsg << hpOpponent << hpPlayerr;
			if (mClient->send(sendPacket) != sf::Socket::Done)
			{
				//error
			}
		}
	}
}

bool HadTheTwoUsersMakeAMove(int& moveCount)
{
	if (moveCount == 2)
		return true;

	return false;
}

void StorePlayerScoreAndMove(std::vector<std::unique_ptr<Player>>& players, int playerID, std::string moveChoice)
{

	for (auto& player : players)
	{
		if (player->GetID() == playerID)
		{
			int scorePlayer;
			do
			{
				scorePlayer = player->Roll();
			} while (scorePlayer < 0);
			player->SetScore(scorePlayer);
			player->SetMove(moveChoice);
		}
	}
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
						players.push_back(std::make_unique<Player>());
						
						
						sf::Packet receivePacket;
						if (client->receive(receivePacket) != sf::Socket::Done)
						{
							//error
						}

						//store player data in reserved location
						receivePacket >> *players[0];
						
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
						players.push_back(std::make_unique<Player>());

						//store player data 
						sf::Packet receivePacket;
						if (client->receive(receivePacket) != sf::Socket::Done)
						{
							//error
						}

						//store player data in reserved location
						receivePacket >> *players[1];

						//give ID to new player
						int sizeList = players.size();
						players[1]->SetID(sizeList);
						
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

						std::string moveChoice = "";
						int playerID = 0;
						receivePacket >> moveChoice >> playerID;
						
			
						//debug
						std::cout << "move ->" << moveChoice << std::endl;
						std::cout << "playerID ->" << playerID << std::endl;
						
						//Reset move count if both users made a move
						if (HadTheTwoUsersMakeAMove(moveCount))
						{
							moveCount = 0;
						}
						
						//Increase move count each time an user make a move
						moveCount++;	

						StorePlayerScoreAndMove(players, playerID, moveChoice);

						if (HadTheTwoUsersMakeAMove(moveCount))
							AnalyseCurrTurnDatas(client, clients, players, playerID);
						else
							ProgressWithTurnAndInformOtherUserOfOpponentMove
							(client, clients, players, playerID);
					}
				}
			}
		}
	}	
}

  

