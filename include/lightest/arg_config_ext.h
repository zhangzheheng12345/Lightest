/*
This is a Lightest extension, which provides a shortcut to resolve commandline
configuration to Lightest configuration.
*/

#ifndef _ARG_CONFIG_H_
#define _ARG_CONFIG_H_

#include "lightest.h"
#include <string> // Compare string more easily

#define ARG_CONFIG()                                                          \
  CONFIG(ArgConfiguration) {                                                  \
    for (; argn > 0; argn--, argc++) {                                        \
      if (std::string(*argc) == "--no-color" || std::string(*argc) == "-nc")  \
        NO_COLOR();                                                           \
      if (std::string(*argc) == "--no-output" || std::string(*argc) == "-no") \
        NO_OUTPUT();                                                          \
      if (std::string(*argc) == "--return-zero" ||                            \
          std::string(*argc) == "--return-0" || std::string(*argc) == "-r0")  \
        RETURN_ZERO();                                                        \
    }                                                                         \
  }

#endif