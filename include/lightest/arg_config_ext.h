/*
This is a Lightest extension, which provides a shortcut to resolve commandline
configuration to Lightest configuration.
*/

#ifndef _ARG_CONFIG_H_
#define _ARG_CONFIG_H_

#include <string>  // Compare string more easily
#include "lightest.h"

namespace lightest {

void MatchArgConfig(std::string arg) {
  if (arg == "--no-color" || arg == "-nc") NO_COLOR();
  if (arg == "--no-output" || arg == "-no") NO_OUTPUT();
  if (arg == "--return-zero" || arg == "--return-0" || arg == "-r0")
    RETURN_ZERO();
}

};  // namespace lightest

#define ARG_CONFIG()                                \
  CONFIG(ArgConfiguration) {                        \
    for (; argn > 0; argn--, argc++) {              \
      lightest::MatchArgConfig(std::string(*argc)); \
    }                                               \
  }

#endif