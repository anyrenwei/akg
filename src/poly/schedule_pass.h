/**
 * Copyright 2020 Huawei Technologies Co., Ltd
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef POLY_PASS_H_
#define POLY_PASS_H_

#include "poly/isl.h"
#include "poly/scop_info.h"
#include "poly/pass_info.h"

namespace akg {
namespace ir {
namespace poly {
class SchedulePass {
 public:
  virtual ~SchedulePass() {}
  virtual isl::schedule Run(isl::schedule sch) = 0;

  std::string GetPassName() { return pass_name_; }
  std::string pass_name_;
  bool restart_{false};  // triggers restart during runtime
};

isl::schedule_node ReorderFilters(const isl::schedule_node &node,
                                  const std::unordered_map<size_t, size_t> &old_to_new_map);
isl::union_map DependenceAnalysis(const isl::union_map &sources, const isl::union_map &targets,
                                  const isl::union_map &kills, const isl::union_map &sch);
isl::union_map ComputeAllDependences(const isl::schedule &schedule, const isl::union_map &reads_um,
                                     const isl::union_map &writes_um);
isl::schedule_node GetOuterBand(const isl::schedule_node &root);
bool IsSequenceOrSet(const isl::schedule_node &node);

/*
 * Compute copyin for each filter node, by intersecting the domains of
 * reads and writes of the entire scop.
 */
isl::union_map ComputeFilterCopyin(const isl::schedule_node &node, const isl::union_map &ori_reads,
                                   const isl::union_map &ori_writes, const isl::schedule ori_schedule);

bool CompareSchTreeWithString(const std::string &compare_sch, const isl::schedule &sch);

isl::schedule_constraints MakeScheduleConstraints(const isl::schedule &schedule, PassInfo &pass_info);

isl::union_map RemoveReduceOpSelfDependence(ScopInfo &scop_info, PassInfo &pass_info);

isl::union_map RemoveSelfDependence(PassInfo &pass_info);

isl::union_map RemoveInvariantDependence(const isl::schedule &schedule, PassInfo &pass_info);

/*
 * Compute copyin for each filter and return the union of such copyins.
 * In particular, return an empty result when the outermost band node
 * is not a sequence/set node.
 *
 * "result" is the union of "copyin" from each filter node, which in
 * turn is computed by ComputeFilterCopyin.
 */
isl::union_map ComputeFakeCopyin(const isl::schedule &schedule, const isl::union_map &fake_copyin,
                                 const isl::union_map &ori_reads, const isl::union_map &ori_writes);

}  // namespace poly
}  // namespace ir
}  // namespace akg
#endif  // POLY_PASS_H_
