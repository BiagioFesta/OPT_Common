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

#ifndef __OPT_COMMON__HELPER__HPP
#define __OPT_COMMON__HELPER__HPP
#include <cstdint>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#define THROW_RUNTIME_ERROR(message) throw std::runtime_error(message)

namespace opt_common {

using TimeInstant = std::uint64_t;

using CSV_Line = std::vector<std::string>;
using CSV_Data = std::vector<CSV_Line>;

inline std::string get_dagsim_command(const std::string& lua_filename) {
  std::string com;
  std::ifstream ifs{"config.txt"};
  ifs >> com >> com;
  // select the correct number from the output of Dagsim
  com += "dagsim.sh " + lua_filename +
         "_mod.lua 2>&1|sed -n 1,1p|awk '{print $3}' > result.txt";
  return com;
}

inline void read_csv_file(const std::string& csv_namefile,
                          CSV_Data* output_data) {
  using namespace std::string_literals;

  std::ifstream file(csv_namefile);
  if (file.fail()) {
    THROW_RUNTIME_ERROR("In read CSV file: cannot open file '"s + csv_namefile +
                        "'");
  }

  output_data->clear();

  std::string line;
  while (std::getline(file, line)) {
    CSV_Line csv_row;

    // Scan for value in line
    while (line.empty() == false) {
      // Trim whitespace
      line.erase(0, line.find_first_not_of(' '));

      const bool quoted = (line.at(0) == '"');
      std::string::size_type index_sep;

      if (quoted == true) {
        const auto index_close_quote = line.find('"', 1);
        index_sep = line.find(',', index_close_quote);
      } else {
        index_sep = line.find(',');
      }

      const std::string value = line.substr(0, index_sep);
      csv_row.push_back(std::move(value));
      line.erase(0, index_sep);

      // Trim whitespace
      line.erase(0, line.find_first_not_of(" ,"));
    }

    output_data->push_back(std::move(csv_row));
  }
}

inline std::vector<int> parse_string_as_vector_of_numbers(std::string str) {
  // str is in the form: "[1, 2, 3]" or "[]"
  // TODO(biagio): to implement

  // Trim the string
  str.erase(0, str.find_first_not_of(" []\""));
  str.erase(str.find_last_not_of(" []\"") + 1);

  std::vector<int> numbers;

  while (str.empty() == false) {
    // Trim the string
    str.erase(0, str.find_first_not_of(" "));
    const auto finder = str.find(',');
    const std::string val = str.substr(0, finder);
    const int num = std::stoi(val);
    numbers.push_back(num);
    str.erase(0, finder);
    str.erase(0, str.find_first_not_of(", "));
  }

  return numbers;
}

}  // namespace opt_common

#endif  // __OPT_COMMON__HELPER__HPP
