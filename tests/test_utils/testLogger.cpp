#include "testLogger.h"
#include "cout.h"

using testing::internal::GTestColor;
using testing::internal::ColoredPrintf;

namespace
{
    GTestColor colors[] = {
        GTestColor::COLOR_DEFAULT, //LOG_TRACE,
        GTestColor::COLOR_DEFAULT, //LOG_DEBUG,
        GTestColor::COLOR_DEFAULT, //LOG_INFO,
        GTestColor::COLOR_YELLOW, //LOG_WARN,
        GTestColor::COLOR_RED, //LOG_ERROR,
    };

    const char * formatStrings[] = {
        "[          ] ",
        "[          ] ",
        "[     INFO ] ",
        "[     WARN ] ",
        "[    ERROR ] ",

    };
}

void TestLoggerHandler::Write(const Grafkit::Logger::message_t& message) 
{
    do
    {
        ColoredPrintf(colors[message.type], formatStrings[message.type]);
        ColoredPrintf(colors[message.type], message.message);
        ColoredPrintf(colors[message.type], "\r\n");
    }
    while (false);
}
