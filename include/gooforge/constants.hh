// codeshaunted - gooforge
// include/gooforge/constants.hh
// contains constant declarations
// Copyright (C) 2024 codeshaunted
//
// This file is part of gooforge.
// gooforge is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// gooforge is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty
// of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with gooforge. If not, see <https://www.gnu.org/licenses/>.

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