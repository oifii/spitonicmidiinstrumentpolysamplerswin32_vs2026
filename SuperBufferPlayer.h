/*
 * Copyright (c) 2012-2026 Stephane Poirier
 *
 * stephane.poirier@oifii.org
 *
 * Stephane Poirier
 * 1901 rue Gilford, #53
 * Montreal, QC, H2H 1G8
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef SUPERBUFFERPLAYER_H
#define SUPERBUFFERPLAYER_H

#include "Tonic.h"

using namespace Tonic;


#define SUPERBUFFERPLAYER_NUMBEROFBUFFERS	128

class SuperBufferPlayer : public BufferPlayer{
protected:
	SampleTable _pbuffers[SUPERBUFFERPLAYER_NUMBEROFBUFFERS];
public:

	SuperBufferPlayer& setBuffers(SampleTable** pbuffers)
	{
		for (int i = 0; i < SUPERBUFFERPLAYER_NUMBEROFBUFFERS; i++)
		{
			_pbuffers[i] = *(pbuffers[i]);
		}
		gen()->setBuffer(_pbuffers[0]);
		return *this;
	};

	SuperBufferPlayer& setBuffer(ControlParameter midinotenumber)
	{
		gen()->setBuffer(_pbuffers[(int)(midinotenumber.getValue())]);
		return *this;
	};

	SuperBufferPlayer& setBuffer(int midinotenumber)
	{
		gen()->setBuffer(_pbuffers[midinotenumber]);
		return *this;
	};

};

#endif //SUPERBUFFERPLAYER_H