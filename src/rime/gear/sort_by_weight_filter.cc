//2019-09-01
#include <utf8.h>
#include <rime/candidate.h>
#include <rime/translation.h>
#include <rime/gear/sort_by_weight_filter.h>
#include <rime/gear/translator_commons.h>
#include <set>
namespace rime {

// static inline size_t unistrlen(const string& text) {
//   return utf8::unchecked::distance(
//       text.c_str(), text.c_str() + text.length());
// }


bool WeightGrater (const rime::of<Phrase>& a,const rime::of<Phrase>& b) {
  return a.get()->weight() > b.get()->weight();
}

bool WeightLess (const rime::of<Phrase>& a,const rime::of<Phrase>& b) {
  return a.get()->weight() < b.get()->weight();
}

class SortByWeightTranslation : public PrefetchTranslation {
 public:
  SortByWeightTranslation(an<Translation> translation);

 private:
  using WeightSorter = std::set<rime::of<rime::Phrase>,
                                  decltype(WeightGrater)*>;
  bool Rearrange();
};

SortByWeightTranslation::SortByWeightTranslation(
    an<Translation> translation)
    : PrefetchTranslation(translation) {
  Rearrange();
}

bool SortByWeightTranslation::Rearrange() {
  if (exhausted()) {
    return false;
  }
  WeightSorter phraseSet(WeightGrater);
  while (!translation_->exhausted()) {
    auto cand = translation_->Peek();
    auto phrase = As<Phrase>(Candidate::GetGenuineCandidate(cand));
    if(!phrase.get())
    {
      translation_->Next();
      continue;
    }
    phraseSet.insert(phrase);
    translation_->Next();
  }
  for (auto eachPhrase : phraseSet)
  {
    cache_.push_back(As<Candidate>(eachPhrase));
  }
  return !cache_.empty();
}

SortByWeightFilter::SortByWeightFilter(const Ticket& ticket)
    : Filter(ticket) {
}

an<Translation> SortByWeightFilter::Apply(
    an<Translation> translation, CandidateList* candidates) {
  return New<SortByWeightTranslation>(translation);
}

}  // namespace rime
