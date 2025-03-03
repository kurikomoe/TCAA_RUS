#pragma once
#include <string>
#include <ctime>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/local_time/local_time.hpp>

// constexpr auto version = L"nightly 2025-01-05";
constexpr const auto* group_text =
    L"「奥秘通译事务所汉化组，版本 {}」";
constexpr const auto* version_text = L"v1.0.0";

std::string convertUnixTimestamp(std::time_t unix_time);

std::wstring version_string();
