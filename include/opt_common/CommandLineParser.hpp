// Copyright 2017 <Biagio Festa>

/*
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef __OPT_COMMON__COMMAND_LINE_PARSER__HPP
#define __OPT_COMMON__COMMAND_LINE_PARSER__HPP
#include <opt_common/helper.hpp>
#include <string>
#include <utility>

namespace opt_common {

class CommandLineParser {
 public:
  enum class OptimizeMethod { FAST_OPTIMIZATION, FAST_BISECT_OPTIMIZATION };

  struct CommandLineOptions {
    std::string name_of_file;
    OptimizeMethod optimize_method;
    bool no_ml;
    std::string config_file;
  };

  static CommandLineOptions parse_command_line(int argc, char** argv);
};

inline CommandLineParser::CommandLineOptions
CommandLineParser::parse_command_line(int argc, char** argv) {
  if (argc < 3) {
    THROW_RUNTIME_ERROR("Command line parse error: missing argument");
  }

  CommandLineOptions options;
  options.no_ml = false;

  // Get the name of the input file
  options.name_of_file = argv[1];

  // Parse the optimization method
  std::string method_arg = argv[2];

  // Clean dash before
  auto finder = method_arg.find_first_not_of('-');
  method_arg = method_arg.substr(finder);

  // Check should be one single char
  if (method_arg.size() != 1) {
    THROW_RUNTIME_ERROR(
        "Command line parse error: method expressed with mutiple characters");
  }

  const char method_char = method_arg.at(0);
  switch (method_char) {
    case 'f':
    case 'F':
      options.optimize_method = OptimizeMethod::FAST_OPTIMIZATION;
      break;
    case 'b':
    case 'B':
      options.optimize_method = OptimizeMethod::FAST_BISECT_OPTIMIZATION;
      break;
    default:
      THROW_RUNTIME_ERROR(
          "Command line parse error: Optimize method not recognized");
  }

  // Parse optional arguments (--no-ml and -c)
  const int num_args_to_parse = argc - 3;
  for (int i = 0; i < num_args_to_parse; ++i) {
    std::string arg_str = argv[3 + i];
    if (arg_str == "--no-ml") {
      // Active no-ml
      options.no_ml = true;
    } else if (arg_str == "-c") {
      options.config_file = argv[3 + i + 1];
      ++i;
    } else {
      THROW_RUNTIME_ERROR(std::string("Command line parse error: Option '" +
                                      arg_str + "' not recognized"));
    }
  }

  return options;
}

}  // namespace opt_common

#endif  // __OPT_COMMON__COMMAND_LINE_PARSER__HPP
