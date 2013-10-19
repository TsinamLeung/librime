//
// Copyleft RIME Developers
// License: GPLv3
//
// 2013-10-17 GONG Chen <chen.sst@gmail.com>
//
#ifndef RIME_SETUP_H_
#define RIME_SETUP_H_

namespace rime {

extern const char* kDefaultModules[];
extern const char* kDeployerModules[];

void LoadModules(const char* module_names[]);

void SetupLogging(const char* app_name);

}  // namespace rime

#endif  // RIME_SETUP_H_
