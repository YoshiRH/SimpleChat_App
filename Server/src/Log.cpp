#include "../include/Log.h"
#include <iostream>

Log& Log::getInstance()
{
	static Log instance;
	return instance;
}

void Log::printLog(const std::string& message)
{
	std::lock_guard<std::mutex> lock(logMutex);

	auto now = std::chrono::system_clock::now();
	auto time = std::chrono::system_clock::to_time_t(now);
	struct tm localTime;
	if (localtime_s(&localTime, &time) != 0) {
		throw std::runtime_error("Failed to get a local time");
	}

	logFile << "[" << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S") << "]";
	logFile << message << std::endl;
}

Log::Log()
{
	logFile.open("server.log", std::ios::app);
	if (!logFile.is_open()) {
		throw std::runtime_error("Couldn't open log file");
	}
}

Log::~Log()
{
	if (logFile.is_open())
		logFile.close();
}
