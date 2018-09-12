#pragma once

#include "core/logger.h"


class TestLoggerHandler : public Grafkit::Logger::ILoggerHandler
{
public:
    TestLoggerHandler();

protected:
    void Write(Grafkit::Logger::message_t* const& message) override;
};
