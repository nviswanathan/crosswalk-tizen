/*
 * Copyright (c) 2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include "common/profiler.h"

#include <math.h>

#include "common/logger.h"
#include "common/string_utils.h"

namespace wrt {

namespace {
void PrintProfileTime(const char* step, const struct timespec& start) {
  struct timespec end;
  clock_gettime(CLOCK_REALTIME, &end);
  int64_t diff_in_milli = (end.tv_sec - start.tv_sec) * 1000
                       + round((end.tv_nsec - start.tv_nsec) * 0.000001);
  std::ostringstream ss;
  ss << "END (" << diff_in_milli << "ms)";
  PrintProfileLog(step, ss.str().c_str());
}

}  //  namespace

void PrintProfileLog(const char* func, const char* tag) {
  LOGGER(DEBUG) << "[PROF] [" << utils::GetCurrentMilliSeconds() << "] "
                << func << ":" << tag;
}

ScopeProfile::ScopeProfile(const char* step) : step_(step), expired_(false) {
  clock_gettime(CLOCK_REALTIME, &start_);
  PrintProfileLog(step, "START");
}

ScopeProfile::~ScopeProfile() {
  if (!expired_)
    PrintProfileTime(step_.c_str(), start_);
}

void ScopeProfile::Reset() {
  clock_gettime(CLOCK_REALTIME, &start_);
  PrintProfileLog(step_.c_str(), "START-updated");
}

void ScopeProfile::End() {
  expired_ = true;
  PrintProfileTime(step_.c_str(), start_);
}

StepProfile* StepProfile::GetInstance() {
  static StepProfile instance;
  return &instance;
}

StepProfile::StepProfile() {
}

StepProfile::~StepProfile() {
}

void StepProfile::Start(const char* step) {
  map_[step].reset(new ScopeProfile(step));
}

void StepProfile::End(const char* step) {
  map_[step].reset();
}

}  // namespace wrt
