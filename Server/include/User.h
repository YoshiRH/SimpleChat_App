#pragma once
#include <string>

class User
{
public:
	User(const std::string& nick, const std::string& pass);

	std::string getUsername() const;
	bool verifyPassword(const std::string& password) const;

private:
	std::string username;
	std::string password;
};

