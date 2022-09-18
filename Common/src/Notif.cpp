#include <Notif.h>

Notif::Notif()
{
	notifType = NotifType::NONE;
	id = 0;
}

Notif::Notif(NotifType _notifType)
{
	notifType = _notifType;
}

Notif::~Notif()
{

}


