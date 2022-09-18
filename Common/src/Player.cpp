#include "Player.h"



Player::Player()
{
	name = "Player";
	classs = ClassType::MAGE;
	classsIDreference = 0;
	hp = 100;
	move = "ND";
	opponent = nullptr;
	id = 0;
	score = 0;
	minLuck = 0;
	maxLuck = 100;
	
}

Player::Player(std::string _name, ClassType _classs, float _hp)
{
	name = _name;
	classs = _classs;
	classsIDreference = static_cast<int>(classs);
	hp = _hp;
	move = "ND";
	id = 0;
	score = 0;
	minLuck = 0;
	maxLuck = 100;
}

Player::Player(const Player& other)
{
	name = other.name;
	classs = other.classs;
	hp = other.hp;
}

Player::~Player()
{
	
}

void Player::SetName(std::string name)
{
	this->name = name;
}
void Player::SetHp(float hp)
{
	this->hp = hp;
}
void Player::SetClass(ClassType classs)
{
	this->classs = classs;
	classsIDreference = static_cast<int>(classs);
}
void Player::SetMove(std::string move)
{
	this->move = move;
}
void Player::SetID(int& id)
{
	this->id = id;
}
void Player::SetScore(int score)
{
	this->score = score;
}

void Player::PrintClass()
{
	ClassType currentClass = static_cast<ClassType>(classsIDreference);
	if (currentClass == ClassType::WARRIOR)
	{
		std::cout << "WARRIOR";
	}
	else
	{
		std::cout << "MAGE";
	}
}
void Player::PrintData()
{
	std::cout << "name: " << name << std::endl;
	std::cout << "HP: " << hp << std::endl;
	std::cout << "classs "; PrintClass();
	std::cout << "move " << move << std::endl;
	std::cout << "score " << score << std::endl;
	std::cout << "id " << GetID() << std::endl;
	std::cout << "min luck" << minLuck << std::endl;
	std::cout << "max luck" << maxLuck << std::endl;

	
}

int Player::GetClassIDRef()
{
	return classs;
}
std::string Player::GetName()
{
	return name;
}
float Player::GetHp()
{
	return hp;
}
std::string Player::GetMove()
{
	return move;
}
Player& Player::GetOpponent()
{
	return *opponent.get();
}
int Player::GetID()
{
	return this->id;
}
int Player::GetScore()
{
	return score;
}

int Player::GetMinLuck()
{
	return minLuck;
}

int Player::GetMaxLuck()
{
	return maxLuck;
}

int Player::Roll()
{
	std::random_device rd; // obtain a random number from hardware
	std::mt19937 gen(rd()); // seed the generator
	std::uniform_int_distribution<> distr(minLuck, maxLuck); // define the range

	score = distr(gen);
	return score;
}
void Player::RiseLuck()
{
	minLuck += 60;
	maxLuck += 60;
}

Player& Player::operator=(Player& other)
{
	name = other.name;
	classs = other.classs;
	hp = other.hp;

	return *this;
}
bool Player::operator!=(Player& other)
{
	return (opponent.get() != other.opponent.get());
}


