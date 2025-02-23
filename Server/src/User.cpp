#include "../include/User.h"

User::User(const std::string& nick, const std::string& pass)
	:username(nick), password(pass) {}


std::string User::getUsername() const
{
	return username;
}

bool User::verifyPassword(const std::string& password) const
{
	return password == this->password;
}




