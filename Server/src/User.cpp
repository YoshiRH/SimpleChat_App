#include "../include/User.h"
#include <iostream>

User::User(const std::string& nick, const std::string& pass)
	:username(nick), password(pass) {}


std::string User::getUsername() const
{
	return username;
}

bool User::verifyPassword(const std::string& password) const
{
#ifdef _DEBUG
	std::cout << password << std::endl;
	std::cout << this->password << std::endl;
#endif

	return password == this->password;
}




