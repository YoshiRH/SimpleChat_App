#include "../include/Log.h"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

Log& Log::getInstance()
{
	static Log instance;
	return instance;
}

// Print out a custom message to file
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

// Open up file at the beggining
Log::Log()
{
	fs::path filePath = fs::current_path() / "server.log";

	logFile.open(filePath, std::ios::app);
	if (!logFile.is_open()) {
		throw std::runtime_error("Couldn't open log file");
	}
}

Log::~Log()
{
	logFile.close();
}
