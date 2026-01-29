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

#ifndef _SPITONICMIDIINSTRUMENTPOLYSAMPLERSWIN32_H
#define _SPITONICMIDIINSTRUMENTPOLYSAMPLERSWIN32_H

#pragma once

#include "resource.h"

 //2021sept11, spi, begin
 // Forward declarations of functions included in this code module:
 ATOM MyRegisterClass(HINSTANCE hInstance);
 BOOL InitInstance(HINSTANCE, int);
 LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
 INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
 //2021sept11, spi, end


#endif //_SPITONICMIDIINSTRUMENTPOLYSAMPLERSWIN32_H