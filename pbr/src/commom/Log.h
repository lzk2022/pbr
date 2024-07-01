#pragma once
#ifndef __LOG_H__
#define __LOG_H__
#include <iostream>
#include <string>
#include <stdexcept>
#include <sstream>
#include <ctime>
#include <iomanip>

// ��־����,���ں�ϵͳ�ĺ궨���г�ͻ����e��ͷ
enum class Level
{
    eINFO,
    eWARN,
    eERROR,
    eASSERT,
    eEXCEPTION,
};

// �궨�壬���ڼ���־��¼
#define LOG_INFO(message) Log::WriteLog(Level::eINFO, (message), __FILE__, __LINE__, __func__)
#define LOG_WARN(message) Log::WriteLog(Level::eWARN, (message), __FILE__, __LINE__, __func__)
#define LOG_ERROR(message) Log::WriteLog(Level::eERROR, (message), __FILE__, __LINE__, __func__)
#define LOG_ASSERT(condition, message) Log::AssertLog((condition), (message), __FILE__, __LINE__, __func__)
#define LOG_EXCEPTION(message) Log::WriteLog(Level::eEXCEPTION, (message), __FILE__, __LINE__, __func__)

class Log
{
public:
    static void WriteLog(Level level, const std::string& message,
        const std::string& file, int line, const std::string& func);
    static void AssertLog(bool condition, const std::string& message,
        const std::string& file, int line, const std::string& func);

private:
    static std::string CurrentDateTime();
};

#endif // !__LOG_H__
