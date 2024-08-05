// codeshaunted - gooforge
// include/gooforge/error.hh
// contains Error declarations
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

#ifndef GOOFORGE_ERROR_HH
#define GOOFORGE_ERROR_HH

#include <stdexcept>

namespace gooforge {

enum class Error {
    FAILED_TO_OPEN_FILE,
    RESOURCE_NOT_FOUND,
};

} // namespace gooforge

#endif // GOOFORGE_ERROR_HH