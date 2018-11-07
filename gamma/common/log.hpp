// Copyright (c) 2018 Aleksey Strelnikov
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

#ifndef GAMMA_COMMON_LOG_HPP_
#define GAMMA_COMMON_LOG_HPP_

#include "absl/strings/str_cat.h"
#include "absl/strings/string_view.h"

namespace y {

struct LogSink {
  virtual void WriteLine(absl::string_view line) = 0;
};

// Set the object or callback that receives log messages. Thread-safe.
void SetLogSink(LogSink* sink);

}  // namespace y

// Basic thread-safe macros for logging.
//     YLOG << "message";
//     YLOG_IF(x > 5) << "message";
//     YLOG_IF(y < 0);
//     YERR << "this should never happen";
//     YERR_IF(!file.good()) << "could not open critical file.";
//
// Log a message without automatic file and line number addition.
//
//     YLOG_RAW << "message";
//     YERR_RAW << "message";
//
#define YLOG \
  ::y_internal::LogLine(__FILE__, __LINE__, ::y_internal::LogFatal::kFalse)
#define YLOG_IF(condition)                                                  \
  if (condition)                                                            \
  ::y_internal::LogLine(__FILE__, __LINE__, ::y_internal::LogFatal::kFalse) \
      << "(condition '" #condition "') "

#define YERR \
  ::y_internal::LogLine(__FILE__, __LINE__, ::y_internal::LogFatal::kTrue)
#define YERR_IF(condition)                                                 \
  if (condition)                                                           \
  ::y_internal::LogLine(__FILE__, __LINE__, ::y_internal::LogFatal::kTrue) \
      << "(condition '" #condition "') "

#define YLOG_RAW ::y_internal::LogLine(::y_internal::LogFatal::kFalse)
#define YERR_RAW ::y_internal::LogLine(::y_internal::LogFatal::kTrue)

// -----------------------------------------------------------------------------
//                      Implementation Details Follow

namespace y_internal {

enum class LogFatal { kFalse, kTrue };

class LogLine {
 public:
  LogLine(const LogLine&) = delete;
  LogLine(LogLine&&) = delete;

  explicit LogLine(LogFatal fatal);
  LogLine(const char* file, int line, LogFatal fatal);

  ~LogLine();

  template <typename T>
  LogLine& operator<<(T&& t) {
    absl::StrAppend(&line_, std::forward<T>(t));
    return *this;
  }

 private:
  std::string line_;
  const LogFatal fatal_;
};

}  // namespace y_internal
#endif  // GAMMA_COMMON_LOG_HPP_
