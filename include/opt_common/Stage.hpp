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

#ifndef __OPT_COMMON__STAGE_HPP
#define __OPT_COMMON__STAGE_HPP
#include <cstdint>
#include <opt_common/helper.hpp>
#include <ostream>
#include <set>
#include <tuple>
#include <vector>

namespace opt_common {
class Stage {
 public:
  using StageID = std::uint64_t;

  //! Constructor
  Stage(StageID stage_id, std::size_t number_of_tasks);

  //! \return the stage identifier
  const StageID& get_stageID() const noexcept { return m_id_stage; }

  //! \retunr the number of tasks in this stage
  unsigned int get_number_of_tasks() const noexcept {
    return m_number_of_tasks;
  }

  const TimeInstant& get_min_time() const noexcept { return m_min_time; }
  const TimeInstant& get_avg_time() const noexcept { return m_avg_time; }
  const TimeInstant& get_max_time() const noexcept { return m_max_time; }

  void set_tasks_times(const std::vector<TimeInstant>& tasks_times) {
    const auto statistical_times = compute_minavgmax_times(tasks_times);
    m_min_time = std::get<0>(statistical_times);
    m_avg_time = std::get<1>(statistical_times);
    m_max_time = std::get<2>(statistical_times);
  }

  void set_dependencies(std::set<StageID> id_dependencies);

  void print_dump_on_stream(std::ostream* os) const;

 private:
  using MinAvgMax_Times = std::tuple<TimeInstant, TimeInstant, TimeInstant>;

  StageID m_id_stage;
  TimeInstant m_min_time;
  TimeInstant m_avg_time;
  TimeInstant m_max_time;
  unsigned int m_number_of_tasks;
  std::set<StageID> m_stages_dependencies;

  MinAvgMax_Times compute_minavgmax_times(
      const std::vector<TimeInstant>& tasks_times) const;
};

inline Stage::Stage(StageID stage_id, std::size_t number_of_tasks)
    : m_id_stage(std::move(stage_id)), m_number_of_tasks(number_of_tasks) {}

inline Stage::MinAvgMax_Times Stage::compute_minavgmax_times(
    const std::vector<TimeInstant>& tasks_times) const {
  if (tasks_times.empty()) {
    THROW_RUNTIME_ERROR("Stage computing timing: the number of tasks is zero");
  }

  TimeInstant min = tasks_times.at(0);
  TimeInstant avg = 0;
  TimeInstant max = tasks_times.at(0);

  for (const auto& task_time : tasks_times) {
    if (task_time < min) {
      min = task_time;
    }
    if (task_time > max) {
      max = task_time;
    }
    avg += task_time;
  }

  avg = static_cast<float>(avg) / static_cast<float>(tasks_times.size());
  return std::make_tuple(min, avg, max);
}

inline void Stage::set_dependencies(std::set<StageID> id_dependencies) {
  m_stages_dependencies = std::move(id_dependencies);
}

inline void Stage::print_dump_on_stream(std::ostream* os) const {
  *os << "ID stage: " << m_id_stage << "\n"
      << "Avg Time: " << m_avg_time << "\n"
      << "Number of tasks: " << m_number_of_tasks << "\n"
      << "Dependencies on stage: \n";
  if (m_stages_dependencies.empty()) {
    *os << "  No dependencies\n";
  } else {
    for (const auto& id_dep : m_stages_dependencies) {
      *os << "  ID stage: " << id_dep << "\n";
    }
  }
}

}  // opt_common

#endif  // __OPT_COMMON__STAGE_HPP
