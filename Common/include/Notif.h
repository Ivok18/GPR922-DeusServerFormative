#pragma once
#include "NotifType.h"

class Notif
{
private:
	NotifType notifType;
	int id;
public:
	Notif();
	Notif(NotifType _notifType);
	~Notif();

	NotifType GetNotifType()
	{
		return notifType;
	}
	int GetID()
	{
		return static_cast<int>(notifType);
	}
};