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
  const auto& pa = a.get();
  const auto& pb = b.get();
  return pa->quality() != pb->quality() 
  ? (pa->quality() > pb->quality()) 
  : (pa->weight() > pb->weight());
}

bool WeightLess (const rime::of<Phrase>& a,const rime::of<Phrase>& b) {
  const auto& pa = a.get();
  const auto& pb = b.get();
  return pa->quality() != pb->quality() 
  ? (pa->quality() < pb->quality()) 
  : (pa->weight() < pb->weight());
}

class SortByWeightTranslation : public PrefetchTranslation {
 public:
  SortByWeightTranslation(an<Translation> translation,unsigned int sort_range = 512);

 private:
  using WeightSorter = std::set<rime::of<rime::Phrase>,
                                  decltype(WeightGrater)*>;
  bool Rearrange();
  bool hasType(rime::Candidate* phrase, string type);
  unsigned int sort_range_;
};

SortByWeightTranslation::SortByWeightTranslation(
    an<Translation> translation, unsigned int sort_range)
    : PrefetchTranslation(translation), sort_range_(sort_range) {
  Rearrange();
}

bool SortByWeightTranslation::Rearrange() {
  if (exhausted()) {
    return false;
  }
  WeightSorter sortedPhrase(WeightGrater);
  CandidateQueue unsortedTop;
  CandidateQueue unsortedBottom;
  int current_count = 0;
  while (!translation_->exhausted()) {
    auto cand = translation_->Peek();
    auto phrase = As<Phrase>(Candidate::GetGenuineCandidate(cand));
    bool shouldSkip = phrase.get() == nullptr;
    shouldSkip |= (hasType(cand.get(), "table") || 
                   hasType(cand.get(), "reverse_lookup"));  

    if(shouldSkip)
    { 
      if (sortedPhrase.size() == 0)
      {
        unsortedTop.push_back(cand);
      } else {
        unsortedBottom.push_back(cand);
      }
      translation_->Next();
      continue;
    }
    if(current_count < sort_range_)
    {
      sortedPhrase.insert(phrase);
    } else {
      unsortedBottom.push_back(phrase);
    }
    translation_->Next();
    current_count++;
  }
  cache_.splice(cache_.end(),unsortedTop);
  for (auto eachPhrase : sortedPhrase)
  {
    cache_.push_back(As<Candidate>(eachPhrase));
  }
  cache_.splice(cache_.end(),unsortedBottom);
  return !cache_.empty();
}

bool SortByWeightTranslation::hasType(rime::Candidate* phrase, string type)
{
  if(phrase)
   { return phrase->type().find(type) != std::string::npos; }
  else
  { return false;}
}
SortByWeightFilter::SortByWeightFilter(const Ticket& ticket)
    : Filter(ticket) {
      if(name_space_ == "filter") 
      { name_space_ = "sort_by_weight_filter"; }
      if(Config* config = engine_->schema()->config()) {
        if(!config->GetInt(name_space_ + "/sort_range", &sort_range_)) 
        { sort_range_ = 64; }
      }

}

an<Translation> SortByWeightFilter::Apply(
    an<Translation> translation, CandidateList* candidates) {
  return New<SortByWeightTranslation>(translation, sort_range_);
}

}  // namespace rime
