// codeshaunted - gooforge
// include/gooforge/goo_ball.hh
// contains GooBall declarations
// Copyright (C) 2025 codeshaunted
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

#ifndef GOOFORGE_GOO_EVENT_HH
#define GOOFORGE_GOO_EVENT_HH

class GooBall;
class GooStrand;

namespace gooforge {

struct GooEventHandler {
	protected:
		virtual void notifyAddBall(GooBall* ball) {};
		virtual void notifyRemoveBall(GooBall* ball) {};
		virtual void notifyAddStrand(GooStrand* strand) {};
		virtual void notifyRemoveStrand(GooStrand strand) {};
};

} // namespace gooforge

#endif // GOOFORGE_GOO_EVENT_HH