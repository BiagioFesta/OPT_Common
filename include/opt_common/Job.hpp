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

#ifndef __OPT_COMMON__JOB__HPP
#define __OPT_COMMON__JOB__HPP
#include <cstdint>
#include <map>
#include <opt_common/Stage.hpp>
#include <opt_common/helper.hpp>

namespace opt_common {

class Job {
 public:
  using JobID = std::uint64_t;

  Job(JobID job_id, TimeInstant submission_time, TimeInstant completion_time);

  const JobID& get_jobID() const noexcept { return m_job_id; }

  const TimeInstant& get_submission_time() const noexcept {
    return m_submission_time;
  }

  const TimeInstant& get_completion_time() const noexcept {
    return m_completion_time;
  }

  void set_id_stages(std::set<Stage::StageID> id_stages) {
    m_id_stages = std::move(id_stages);
  }

 private:
  JobID m_job_id;
  TimeInstant m_submission_time;
  TimeInstant m_completion_time;
  std::set<Stage::StageID> m_id_stages;
};

inline Job::Job(JobID job_id, TimeInstant submission_time, TimeInstant completion_time)
    : m_job_id(std::move(job_id)),
      m_submission_time(std::move(submission_time)),
      m_completion_time(std::move(completion_time)) {}
}  // namespace opt_common

#endif  // __OPT_COMMON__JOB__HPP
