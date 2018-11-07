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

#include "gamma/common/log.hpp"

#include <cstdlib>

#include "absl/strings/str_format.h"
#include "absl/synchronization/mutex.h"

static y::LogSink* log_sink = nullptr;
static absl::Mutex sink_mutex;

void y::SetLogSink(LogSink* sink) {
  absl::MutexLock lock(&sink_mutex);
  log_sink = sink;
}

namespace y_internal {

LogLine::LogLine(LogFatal fatal) : fatal_(fatal) {}

LogLine::LogLine(const char* file, int line, LogFatal fatal) : fatal_(fatal) {
  if (file[0] == '.' && file[1] == '/') file += 2;
  absl::StrAppend(&line_, file, ":", line, ": ");
}

LogLine::~LogLine() {
  absl::MutexLock lock(&sink_mutex);
  if (log_sink != nullptr) {
    log_sink->WriteLine(line_);
  } else {
    absl::PrintF("%s\n", line_);
  }
  if (fatal_ == LogFatal::kTrue) std::abort();
}

}  // namespace y_internal
