#pragma once
//#include "mediasoupclient.hpp"
#include "DebugCpp.h"
#include <Logger.hpp>
using namespace mediasoupclient;

class UnityLogger : public Logger::LogHandlerInterface
{
public:
	void OnLog(Logger::LogLevel level, char* payload, size_t len);
};

