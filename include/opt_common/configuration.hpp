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

#ifndef __OPT_COMMON__CONFIGURATION__HPP
#define __OPT_COMMON__CONFIGURATION__HPP
#include <fstream>
#include <opt_common/helper.hpp>
#include <string>

namespace opt_common {

class Configuration {
 public:
  Configuration() = default;
  void read_configuration_from_file(const std::string& filename);

  const std::string& get_data_path() const noexcept { return m_data_path; }

  const std::string& get_lua_path() const noexcept { return m_lua_path; }

  const std::string& get_dagsim_path() const noexcept { return m_dagsim_path; }

 private:
  std::string m_data_path;
  std::string m_lua_path;
  std::string m_dagsim_path;
};

inline void Configuration::read_configuration_from_file(
    const std::string& filename) {
  using namespace std::string_literals;

  std::ifstream file(filename);
  if (file.fail()) {
    THROW_RUNTIME_ERROR("In read configuration from file: cannot open file '"s +
                        filename + "'");
  }

  file >> m_data_path >> m_dagsim_path >> m_lua_path;
}

}  // namespace opt_common

#endif  // __OPT_COMMON__CONFIGURATION__HPP
