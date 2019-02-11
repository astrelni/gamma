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

#include "gamma/app/app.hpp"

#include "absl/memory/memory.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"
#include "gamma/common/log.hpp"
#include "gamma/common/watch.hpp"

namespace y {

void App::init(const AppSettings& settings) {
  YERR_IF(window_ != nullptr) << "Application already initialized";

  YERR_IF(!settings.has_window_settings());
  window_ = absl::make_unique<Window>(settings.window_settings());
}

void App::run() {
  YERR_IF(window_ == nullptr) << "Applicaton has not been initialized";

  Watch watch;
  while (!signal_close_) {
    absl::Duration dt = watch.lap();
    function_queue_.update(dt);
    absl::SleepFor(absl::Milliseconds(16));
  }
}

}  // namespace y
