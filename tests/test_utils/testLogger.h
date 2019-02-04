#pragma once

#include "core/logger.h"


class TestLoggerHandler : public Grafkit::Logger::ILoggerHandler
{
public:

    TestLoggerHandler() {
    }

    void Write(const Grafkit::Logger::message_t& message) override;
};
