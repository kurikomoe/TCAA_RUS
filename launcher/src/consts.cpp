#include <format>
#include <string>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/local_time/local_time.hpp>

#include "compile_time.h"

#include "consts.h"

std::string convertUnixTimestamp(std::time_t unix_time) {
  using namespace boost::gregorian;
  using namespace boost::posix_time;
  using namespace boost::local_time;

  // Create a ptime object from Unix timestamp
  ptime time = from_time_t(unix_time + 24 * 3600);

  boost::shared_ptr<posix_time_zone> tz(new posix_time_zone("UTC"));

  local_date_time localTime(time, tz);

  // Extract the ptime part from local_date_time
  ptime local_ptime = localTime.local_time();

  // Extract year, month, and day
  auto date = local_ptime.date();
  int year = date.year();
  int month = date.month();
  int day = date.day();

  // Extract hours, minutes, and seconds
  auto tod = local_ptime.time_of_day();
  int hours = tod.hours();
  int minutes = tod.minutes();
  int seconds = tod.seconds();

  // Format the date and time as YYYY-MM-DD HH:MM:SS
  std::ostringstream oss;
  oss << std::setw(4) << std::setfill('0') << year << "-" << std::setw(2)
      << std::setfill('0') << month << "-" << std::setw(2) << std::setfill('0')
      << day << " " << std::setw(2) << std::setfill('0') << hours << ":"
      << std::setw(2) << std::setfill('0') << minutes << ":" << std::setw(2)
      << std::setfill('0') << seconds;

  return oss.str();
}
std::wstring version_string() {
  std::string date = convertUnixTimestamp(__TIME_UNIX__);
  std::wstring wdate(date.begin(), date.end());
  return std::format(version_text, wdate);
}
