#pragma once
#include <SFML/Network.hpp>
#include <Notif.h>
#include "Player.h"


sf::Packet& operator >>(sf::Packet& packet, Player& player)
{
	return packet >> player.name >> player.hp >> player.classsIDreference >> player.id >> player.move >> player.score >> player.minLuck >> player.maxLuck;
}

sf::Packet& operator <<(sf::Packet& packet, Player& player)
{
	return packet << player.name << player.hp << player.classsIDreference << player.id << player.move << player.score << player.minLuck << player.maxLuck;
}

sf::Packet& operator >>(sf::Packet& packet, char& a)
{
	return packet >> a;
}

sf::Packet& operator <<(sf::Packet& packet, char& a)
{
	return packet << a;
}




