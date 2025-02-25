#pragma once
#include <string>
#include <fstream>
#include <mutex>
#include <chrono>
#include <iomanip>

// Logger class for server
class Log {
public:
	static Log& getInstance();
	void printLog(const std::string& message);

private:
	Log();
	~Log();

	std::ofstream logFile;
	std::mutex logMutex;
};