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

#ifndef __OPT_COMMON__APPLICATION__HPP
#define __OPT_COMMON__APPLICATION__HPP
#include <cassert>
#include <fstream>
#include <map>
#include <opt_common/InfrastructureConfiguration.hpp>
#include <opt_common/Job.hpp>
#include <opt_common/MachineLearningModel.hpp>
#include <opt_common/Stage.hpp>
#include <opt_common/configuration.hpp>
#include <sstream>
#include <string>
#include <vector>

namespace opt_common {
class Application {
 public:
  using ApplicationID = std::string;

  //! Only file name without absolute path
  struct FileResources {
    std::string m_Application_File;
    std::string m_Jobs_File;
    std::string m_Stages_File;
    std::string m_Tasks_File;
    std::string m_Lua_File;
    std::string m_Infrastructure_File;
  };

  Application() = default;

  /*! Compute the approximate execution time instant.
   * This method uses the following formula:
         sum of wave * stage average time
   */
  TimeInstant compute_avg_execution_time() const noexcept {
    return compute_avg_execution_time(m_number_of_cores);
  }

  TimeInstant compute_avg_execution_time(const std::size_t n) const noexcept;

  const TimeInstant& get_deadline() const noexcept { return m_deadline; }
  void set_deadline(const TimeInstant& deadline) noexcept {
    m_deadline = deadline;
  }

  std::size_t compute_max_number_of_task() const noexcept;

  //! \return the absolute lua filename (with absolute path)
  const std::string& get_lua_name() const noexcept { return m_lua_filename; }

  static Application create_application(const std::string& data_input_namefile,
                                        const std::string& config_namefile);

  static Application create_application(FileResources resources_filename,
                                        std::string config_namefile,
                                        std::string deadline_str);

  void set_alpha_beta(unsigned int n1, unsigned int n2);

  double get_alpha() const noexcept { return m_alpha; }
  double get_beta() const noexcept { return m_beta; }

  const auto& get_machine_learning_model() const noexcept { return m_mlm; }

  const auto& get_infrastructure_config() const noexcept {
    return m_infr_config;
  }

  const auto& get_dagsim_path() const noexcept {
    return m_app_configuration.get_dagsim_path();
  }

  unsigned int get_number_of_core() const noexcept { return m_number_of_cores; }
  void set_number_of_core(unsigned int num_cors) noexcept {
    m_number_of_cores = num_cors;
  }

  const std::map<Stage::StageID, Stage>& get_all_stages() const noexcept {
    return m_stages;
  }

  void set_weight(double w) noexcept { m_weight = w; }
  double get_weight() const noexcept { return m_weight; }

  const ApplicationID& get_application_id() const noexcept { return m_app_id; }

  //! \note FileResources have not absolute path
  const FileResources& get_files_resources() const noexcept { return m_files_resources; }

 private:
  ApplicationID m_app_id;
  std::map<Job::JobID, Job> m_jobs;
  std::map<Stage::StageID, Stage> m_stages;

  TimeInstant m_submission_time = 0;
  TimeInstant m_deadline = 0;
  TimeInstant m_real_execution_time = 0;

  std::string m_lua_filename;
  std::string m_infrastructure_filename;

  double m_alpha = 0.0;
  double m_beta = 0.0;

  double m_weight = 0.0;

  unsigned int m_number_of_cores = 0;

  opt_common::InfrastructureConfiguration m_infr_config;
  opt_common::MachineLearningModel m_mlm;

  Configuration m_app_configuration;

  FileResources m_files_resources;
};

inline TimeInstant Application::compute_avg_execution_time(
    const std::size_t n) const noexcept {
  TimeInstant time_execution = 0;
  for (const auto& stage_pair : m_stages) {
    const Stage& stage = stage_pair.second;
    if (stage.get_number_of_tasks() % n != 0) {
      time_execution += stage.get_avg_time();
    }

    const unsigned coeff = stage.get_number_of_tasks() / n;
    time_execution += coeff * stage.get_avg_time();
  }
  return time_execution;
}

inline std::size_t Application::compute_max_number_of_task() const noexcept {
  std::size_t max = 0;
  for (const auto& stage_pair : m_stages) {
    const Stage& stage = stage_pair.second;
    if (stage.get_number_of_tasks() > max) {
      max = stage.get_number_of_tasks();
    }
  }
  return max;
}

inline void Application::set_alpha_beta(unsigned int n1, unsigned int n2) {
  if (n1 == n2) {
    THROW_RUNTIME_ERROR("In setting alpha beta for application: n1 == n2");
  }

  TimeInstant r1 = this->compute_avg_execution_time(n1);
  TimeInstant r2 = this->compute_avg_execution_time(n2);

  if (n1 > n2) {
    m_alpha = static_cast<double>(r2 - r1) * n1 * n2 / (n1 - n2);
    m_beta = static_cast<double>(r1) - m_alpha / n1;
  } else {
    m_alpha = static_cast<double>(r1 - r2) * n1 * n2 / (n2 - n1);
    m_beta = static_cast<double>(r1) - m_alpha / n1;
  }
}

inline Application Application::create_application(
    FileResources resources_filename, std::string config_namefile,
    std::string deadline_str) {
  using namespace std::string_literals;

  if (deadline_str.empty()) {
    THROW_RUNTIME_ERROR("In creation application: some missing information");
  }

  // Create empty application object
  Application app;

  // Set all filenames resouces
  app.m_files_resources = resources_filename;

  // Read the configuration file
  app.m_app_configuration.read_configuration_from_file(config_namefile);

  // Add path to the file names
  resources_filename.m_Application_File =
      app.m_app_configuration.get_data_path() + "/" +
      resources_filename.m_Application_File;
  resources_filename.m_Jobs_File = app.m_app_configuration.get_data_path() +
                                   "/" + resources_filename.m_Jobs_File;
  resources_filename.m_Stages_File = app.m_app_configuration.get_data_path() +
                                     "/" + resources_filename.m_Stages_File;
  resources_filename.m_Tasks_File = app.m_app_configuration.get_data_path() +
                                    "/" + resources_filename.m_Tasks_File;
  resources_filename.m_Infrastructure_File =
      app.m_app_configuration.get_data_path() + "/" +
      resources_filename.m_Infrastructure_File;
  resources_filename.m_Lua_File = app.m_app_configuration.get_lua_path() + "/" +
                                  resources_filename.m_Lua_File;

  // Set some information in application
  app.m_lua_filename = resources_filename.m_Lua_File;
  app.m_infrastructure_filename = resources_filename.m_Infrastructure_File;
  app.m_submission_time = 0;
  app.m_deadline = std::stoul(deadline_str);
  app.m_number_of_cores = 1;

  CSV_Data csv_data;

  // Read the app csv
  read_csv_file(resources_filename.m_Application_File, &csv_data);

  // Set the application id
  app.m_app_id = csv_data.at(1).at(0);

  // Get the duration of application as time difference
  const auto app_time_start = std::stoul(csv_data.at(1).at(1));
  const auto app_time_stop = std::stoul(csv_data.at(2).at(1));
  app.m_real_execution_time = app_time_stop - app_time_start;

  // Read the jobs file
  read_csv_file(resources_filename.m_Jobs_File, &csv_data);

  // For each line in csv parse it
  std::map<Job::JobID, TimeInstant> job2submission_time, job2completion_time;

  // Map each job id with a vector of stages IDs
  std::map<Job::JobID, std::set<Stage::StageID>> id_stages;

  for (std::size_t row_index = 1; row_index < csv_data.size(); ++row_index) {
    // Get the current row
    const auto& row = csv_data.at(row_index);
    const auto row_size = row.size();
    if (row_size != 4) {
      THROW_RUNTIME_ERROR("In creation application: file '"s +
                          resources_filename.m_Jobs_File +
                          "' has different number of cols");
    }

    // Get the job id
    const Job::JobID job_id = std::stoi(row.at(0));

    // Get submission time
    const auto& submission_time_str = row.at(1);
    if (submission_time_str != "NOVAL") {
      const auto submission_time = std::stoul(submission_time_str);
      job2submission_time.insert(std::make_pair(job_id, submission_time));
    }

    // Get the completion time (as last field in row)
    const auto& completion_time_str = row.at(3);
    if (completion_time_str != "NOVAL") {
      const auto completion_time = std::stoul(completion_time_str);
      job2completion_time.insert(std::make_pair(job_id, completion_time));
    }

    // Get the stage dependency of the job
    const auto& set_of_deps = row.at(2);
    if (set_of_deps != "NOVAL") {
      std::set<Stage::StageID> stageIDs;
      const auto numbers = parse_string_as_vector_of_numbers(set_of_deps);
      for (const auto& num : numbers) {
        stageIDs.insert(num);
      }
      id_stages.insert(std::make_pair(job_id, std::move(stageIDs)));
    }
  }  // for all row in jobs file

  // Insert all jobs into the application object
  for (const auto& it : job2submission_time) {
    const auto& job_id = it.first;
    Job job_temp(job_id,                                     // job id
                 it.second,                                  // sub time
                 job2completion_time.find(job_id)->second);  // completion time

    job_temp.set_id_stages(id_stages.find(job_id)->second);

    app.m_jobs.insert(std::make_pair(job_id, std::move(job_temp)));
  }  // for all submission times

  // ---------------
  // Parse the stages file
  read_csv_file(resources_filename.m_Stages_File, &csv_data);

  // Fill the data structures of stages file (for each row in stages file)
  for (unsigned row_index = 1; row_index < csv_data.size(); ++row_index) {
    // Get the current row
    const auto& row = csv_data.at(row_index);
    const auto number_of_cols = row.size();
    if (number_of_cols != 6) {
      THROW_RUNTIME_ERROR("In creation application: file '"s +
                          resources_filename.m_Stages_File +
                          "' has different number of cols");
    }

    // Get the stage id at the current row
    const Stage::StageID stage_id = std::stoi(row.at(0));

    const unsigned number_of_tasks = std::stoi(row.at(3));

    // Create stage object
    Stage stage_temp(stage_id, number_of_tasks);

    // Parse stage dependencies
    std::set<Stage::StageID> parentIDs;
    const auto& parents_str = row.at(2);
    const auto numbers = parse_string_as_vector_of_numbers(parents_str);
    for (const auto& num : numbers) {
      parentIDs.insert(num);
    }
    stage_temp.set_dependencies(std::move(parentIDs));
    app.m_stages.insert(std::make_pair(stage_id, std::move(stage_temp)));
  }  // for each row in stages file

  // Parse the tasks file
  read_csv_file(resources_filename.m_Tasks_File, &csv_data);

  // Map a ID stage with a execution times of stage2tasks
  std::map<Stage::StageID, std::vector<TimeInstant>> stage2tasks;

  // Fill the data structures of task file (for each row in task file)
  for (unsigned row_index = 1; row_index < csv_data.size(); ++row_index) {
    // Get the current row
    const auto& row = csv_data.at(row_index);

    // Get task information
    const unsigned long launch_time = std::stoul(row.at(4));
    const unsigned long finish_time = std::stoul(row.at(5));
    const Stage::StageID id_stage = std::stoi(row.at(16));
    const auto execution_time = finish_time - launch_time;
    stage2tasks[id_stage].push_back(execution_time);
  }

  // Update stages of application with the max min a avg task
  for (auto& stage_pair : app.m_stages) {
    const auto& stage_id = stage_pair.first;
    auto& stage = stage_pair.second;
    stage.set_tasks_times(stage2tasks.at(stage_id));
  }

  // Read the configuration file
  std::ifstream ifs_config(resources_filename.m_Infrastructure_File);
  if (!ifs_config) {
    THROW_RUNTIME_ERROR(
        "In creation application: cannot open the file for infrastructure '"s +
        resources_filename.m_Infrastructure_File + "'");
  }

  // Read the second line (skip the first line -header-)
  std::string configfile_line;
  getline(ifs_config, configfile_line);
  getline(ifs_config, configfile_line);

  // Parse the line saving parameters
  std::istringstream iss_config(configfile_line);
  std::string app_id, chi_0, chi_c, container_memory, executor_memory,
      container_cores, executor_cores;
  iss_config >> app_id >> chi_0 >> chi_c >> container_memory >>
      executor_memory >> container_cores >> executor_cores;

  // Print on the standard output
  std::cout << '\n' << " Optimizing configuration" << std::endl;
  std::cout << app_id << " "
            << " " << chi_0 << " " << chi_c << " " << container_memory << " "
            << executor_memory << " " << container_cores << " "
            << executor_cores;

  InfrastructureConfiguration ic(
      std::stof(container_memory), std::stof(executor_memory),
      std::stoul(container_cores), std::stoul(executor_cores));

  MachineLearningModel mlm(std::stof(chi_0), std::stof(chi_c));

  // Set infrastructure configuraiton and ML into the application object
  app.m_infr_config = ic;
  app.m_mlm = mlm;

  app.m_mlm.print_dump_on_stream(&std::cout);

  return app;
}

inline Application Application::create_application(
    const std::string& data_input_namefile,
    const std::string& config_namefile) {
  using namespace std::string_literals;
  // Read the input file
  std::ifstream ifs(data_input_namefile);
  if (!ifs) {
    THROW_RUNTIME_ERROR("In creation application: cannot open the file '"s +
                        data_input_namefile + "'");
  }

  // Read the line of the input file
  std::string data_line_file;
  std::getline(ifs, data_line_file);

  // Tokenize the line and get app information
  std::istringstream iss(data_line_file);
  FileResources resources_filename;
  std::string deadline_str;
  iss >> resources_filename.m_Application_File;
  iss >> resources_filename.m_Jobs_File;
  iss >> resources_filename.m_Stages_File;
  iss >> resources_filename.m_Tasks_File;
  iss >> resources_filename.m_Lua_File;
  iss >> resources_filename.m_Infrastructure_File;
  iss >> deadline_str;

  return create_application(resources_filename, config_namefile, deadline_str);
}

}  // namespace opt_common
#endif  // __OPT_COMMON__APPLICATION__HPP
