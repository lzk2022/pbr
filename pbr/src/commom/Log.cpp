#include "Log.h"
#include <format>

// 定义 ANSI 转义码用于设置控制台颜色
#define RESET       "\033[0m"
#define RED         "\033[31m"
#define GREEN       "\033[32m"
#define YELLOW      "\033[33m"
#define BLUE        "\033[34m"
#define MAGENTA     "\033[35m"
#define CYAN        "\033[36m"
#define WHITE       "\033[37m"

void Log::WriteLog(Level level, const std::string& message, const std::string& file, int line, const std::string& func)
{
    std::string levelStr;
    std::string color;

    std::string newFile = file.substr(file.find_last_of("\\") + 1);
    std::string endMessage = std::format(" > {} {} {}", line, func, newFile);

    switch (level) {
    case Level::eINFO:
        levelStr = "I";
        color = WHITE;
        endMessage = "";
        break;
    case Level::eWARN:
        levelStr = "W";
        color = YELLOW;
        break;
    case Level::eERROR:
        levelStr = "E";
        color = RED;
        break;
    case Level::eASSERT:
        levelStr = "A";
        color = MAGENTA;
        break;
    case Level::eEXCEPTION:
        levelStr = "E";
        color = BLUE;
        break;
    }

    std::string formattedMessage = std::format("{}{} [{}] {} {}", color, CurrentDateTime(), levelStr, message,endMessage);
    std::cout << formattedMessage << RESET << std::endl;  // 结尾添加 ANSI 转义码重置颜色
}

void Log::AssertLog(bool condition, const std::string& message, const std::string& file, int line, const std::string& func)
{
    if (condition)
    {
        std::string info = message + file + std::to_string(line) + func;
        WriteLog(Level::eASSERT, message, file, line, func);
        throw std::runtime_error(message);
    }
}

std::string Log::CurrentDateTime()
{
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(localTime, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}