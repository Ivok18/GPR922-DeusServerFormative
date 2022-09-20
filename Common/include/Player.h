#pragma once
#include "ClassType.h"
#include <string>
#include <iostream>
#include <SFML/Network.hpp>
#include <random>

class Player
{
private:
	std::string name;
	ClassType classs;
	int classsIDreference;
	float hp;
	std::string move;
	int id;
	int score;
	int minLuck;
	int maxLuck;

public:
	Player();
	Player(std::string _name, ClassType _classs, float _hp);
	Player(const Player& other);
	~Player();

	void SetName(std::string name);
	void SetHp(float hp);
	void SetClass(ClassType classs);
	void SetMove(std::string move);
	void SetID(int& id);
	void SetScore(int score);
	void PrintClass();
	void PrintData();

	int GetClassIDRef();
	std::string GetName();
	float GetHp();
	std::string GetMove();
	int GetID();
	int GetScore();
	int GetMinLuck();
	int GetMaxLuck();

	int Roll();
	void RiseLuck();
	void DepleteLuck();

	friend sf::Packet& operator >>(sf::Packet& packet, Player& player);
	friend sf::Packet& operator <<(sf::Packet& packet, Player& player);

};

