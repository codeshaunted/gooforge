// codeshaunted - gooforge
// include/gooforge/constants.hh
// contains constant declarations
// Copyright 2024 codeshaunted
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org / licenses / LICENSE - 2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.

#ifndef GOOFORGE_CONSTANTS_HH
#define GOOFORGE_CONSTANTS_HH

namespace gooforge {

// eventually we hopefully entirely get rid of
// or just drastically reduce the number of magic
// numbers declared in this file, for now it's fine
#define GOOFORGE_PIXELS_PER_UNIT 100 // roughly 2160/19.05, calculated manually
#define GOOFORGE_FRAMERATE_LIMIT 144

} // namespace gooforge

#endif // GOOFORGE_CONSTANTS_HH