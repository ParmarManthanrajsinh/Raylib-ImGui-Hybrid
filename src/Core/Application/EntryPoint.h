#pragma once
#include <memory>

namespace Core
{
    class FApplication;
}

std::unique_ptr<Core::FApplication> CreateApplication();