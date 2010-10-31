/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * StringUtils..h
 * Random String functions.
 * Copyright (C) 2005-2008 Simon Newton
 */

#ifndef INCLUDE_OLA_STRINGUTILS_H_
#define INCLUDE_OLA_STRINGUTILS_H_

#include <stdint.h>
#include <string>
#include <vector>

namespace ola {

using std::string;

void StringSplit(const string &input,
                 std::vector<string> &tokens,
                 const string &delimiters=" ");
void StringTrim(string *input);
void ShortenString(string *input);
string IntToString(int i);
string IntToString(unsigned int i);
void Escape(string *original);
string EscapeString(const string &original);
bool StringToUInt(const string &value, unsigned int *output);
bool StringToUInt16(const string &value, uint16_t *output);
bool StringToUInt8(const string &value, uint8_t *output);
bool HexStringToUInt(const string &value, unsigned int *output);
bool HexStringToUInt16(const string &value, uint16_t *output);
void ToLower(string *s);
}  // ola

#endif  // INCLUDE_OLA_STRINGUTILS_H_
