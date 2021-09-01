// 2021-09-01

#ifndef RIME_SORT_BY_WEIGHT_FILTER_H_
#define RIME_SORT_BY_WEIGHT_FILTER_H_

#include <rime/filter.h>

namespace rime {

class SortByWeightFilter : public Filter {
 public:
  explicit SortByWeightFilter(const Ticket& ticket);

  virtual an<Translation> Apply(an<Translation> translation,
                                        CandidateList* candidates);
};

}  // namespace rime

#endif  // RIME_SORT_BY_WEIGHT_FILTER_H_
