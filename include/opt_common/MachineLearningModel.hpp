// Copyright 2017 <Danilo Ardagna>
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

#ifndef __OPT_COMMON__MACHINE_LEARNING_MODEL__HPP
#define __OPT_COMMON__MACHINE_LEARNING_MODEL__HPP
#include <opt_common/InfrastructureConfiguration.hpp>
#include <opt_common/helper.hpp>
#include <iostream>

namespace opt_common {
class MachineLearningModel {
  double chi_0;
  double chi_c;

 public:
  MachineLearningModel(double chi_0 = 0.1, double chi_c = 0.1)
      : chi_0(chi_0), chi_c(chi_c) {}

  // returns job execution time prediction for a given number of cores
  double evaluateModel(unsigned n) const;

  // determines the initial number of cores, given the infrastrucutre
  // configuration and deadline
  unsigned initial_core_numbers(const InfrastructureConfiguration& ic,
                                const TimeInstant& deadline) const;

  void print() const;

  double get_chi_0() const { return chi_0; }

  double get_chi_c() const { return chi_c; }

  void set_chi_0(double chi_0) { MachineLearningModel::chi_0 = chi_0; }

  void set_chi_c(double chi_c) { MachineLearningModel::chi_c = chi_c; }

  void print_dump_on_stream(std::ostream* os);
};

inline void MachineLearningModel::print() const {
  std::cout << '\n' << chi_0 << " " << chi_c << std::endl;
}

inline double MachineLearningModel::evaluateModel(unsigned n) const {
  // int n_containers=n/ic.getContainter_cores();
  return chi_0 + chi_c / n;
}

inline unsigned MachineLearningModel::initial_core_numbers(
    const InfrastructureConfiguration& ic, const TimeInstant& deadline) const {
  // double xi=fmin((double)ic.getContainer_memory()/ic.getExecutor_memory(),
  // (double) ic.getContainter_cores()/ic.getExecutor_cores());
  /* Old Version
  double n_containers= fmin(chi_c/(deadline-chi_0),1);

  double n_cores= ic.getExecutor_cores()  * ceil(n_containers);
  */
  double n_cores = fmax(chi_c / (deadline - chi_0), ic.getContainter_cores());

  // double n_cores= ic.getExecutor_cores()  * ceil(n_containers);

  std::cout << "\n"
            << "Initial cores number: " << n_cores << std::endl;

  return n_cores;
}

inline void MachineLearningModel::print_dump_on_stream(std::ostream* os) {
  *os << '\n'
      << chi_0 << " " << chi_c << '\n';
}

}  // namespace opt_common

#endif  // __OPT_COMMON__MACHINE_LEARNING_MODEL__HPP
