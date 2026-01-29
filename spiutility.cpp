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

#include "stdafx.h"
//#include <stdlib.h>     // srand, rand 
#include <string>

#include "spiutility.h"

DWORD stringtimestamp_prevnowstamp_ms = 0;
int stringtimestamp_nnn = 0;


float RandomFloat(float a, float b)
{
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}

int RandomInt(int lowest, int highest)
{
	int range = (highest - lowest) + 1;
	int random_integer = lowest + int(range*rand() / (RAND_MAX + 1.0));
	return random_integer;
}



// Convert a wide Unicode string to an UTF8 string
std::string utf8_encode(const std::wstring &wstr)
{
	int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
	std::string strTo(size_needed, 0);
	WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
	return strTo;
}

// Convert an UTF8 string to a wide Unicode String
std::wstring utf8_decode(const std::string &str)
{
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}


PCHAR*
CommandLineToArgvA(
PCHAR CmdLine,
int* _argc
)
{
	PCHAR* argv;
	PCHAR  _argv;
	ULONG   len;
	ULONG   argc;
	CHAR   a;
	ULONG   i, j;

	BOOLEAN  in_QM;
	BOOLEAN  in_TEXT;
	BOOLEAN  in_SPACE;

	len = strlen(CmdLine);
	i = ((len + 2) / 2)*sizeof(PVOID)+sizeof(PVOID);

	argv = (PCHAR*)GlobalAlloc(GMEM_FIXED,
		i + (len + 2)*sizeof(CHAR));

	_argv = (PCHAR)(((PUCHAR)argv) + i);

	argc = 0;
	argv[argc] = _argv;
	in_QM = FALSE;
	in_TEXT = FALSE;
	in_SPACE = TRUE;
	i = 0;
	j = 0;

	while (a = CmdLine[i]) {
		if (in_QM) {
			if (a == '\"') {
				in_QM = FALSE;
			}
			else {
				_argv[j] = a;
				j++;
			}
		}
		else {
			switch (a) {
			case '\"':
				in_QM = TRUE;
				in_TEXT = TRUE;
				if (in_SPACE) {
					argv[argc] = _argv + j;
					argc++;
				}
				in_SPACE = FALSE;
				break;
			case ' ':
			case '\t':
			case '\n':
			case '\r':
				if (in_TEXT) {
					_argv[j] = '\0';
					j++;
				}
				in_TEXT = FALSE;
				in_SPACE = TRUE;
				break;
			default:
				in_TEXT = TRUE;
				if (in_SPACE) {
					argv[argc] = _argv + j;
					argc++;
				}
				_argv[j] = a;
				j++;
				in_SPACE = FALSE;
				break;
			}
		}
		i++;
	}
	_argv[j] = '\0';
	argv[argc] = NULL;

	(*_argc) = argc;
	return argv;
}

//i.e. can be used as a filename timestamp prefix
std::string GetStringTimeStamp()
{
	string stringtimestamp = "";

	// Current date/time based on current system
	time_t nowtime = time(0);
	// Convert now to tm struct for local timezone
	tm* localtm = localtime(&nowtime);
	//cout << "The local date and time is: " << asctime(localtm) << endl;
	char charprefix[1000];
	strftime(charprefix,sizeof(charprefix),"%Y%m%d_%H%M%S", localtm);
	stringtimestamp = charprefix;
	stringtimestamp += "_";
	DWORD nowstamp_ms = GetTickCount();
	if ((nowstamp_ms - stringtimestamp_prevnowstamp_ms) < 1000)
	{
		//if sub second, use milliseconds
		if ((nowstamp_ms - stringtimestamp_prevnowstamp_ms) < 1)
		{
			//if sub milliseconds, use an extra integer
			stringtimestamp_nnn++;
		}

	}
	char charbuf[256];
	sprintf_s(charbuf, sizeof(charbuf), "%03d", stringtimestamp_nnn);
	stringtimestamp += charbuf;
	stringtimestamp_prevnowstamp_ms = nowstamp_ms;
	return stringtimestamp;
}


//203mar08, spi, begin
std::wstring ExePath() {
	TCHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
	return std::wstring(buffer).substr(0, pos);
}
//2023mar08, spi, end


