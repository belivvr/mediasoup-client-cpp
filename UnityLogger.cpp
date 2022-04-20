#include "UnityLogger.h"

void UnityLogger::OnLog(Logger::LogLevel level, char* payload, size_t len)
{
	Debug::Log(payload);
}
