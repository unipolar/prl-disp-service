/**
 * @file wildcards.h
 *
 * @brief Routine for check wildcards matching
 *
 * @author vasilyz
 * @owner alexg
 *
 * Copyright (c) 2005-2015 Parallels IP Holdings GmbH
 *
 * This file is part of Virtuozzo Core Libraries. Virtuozzo Core
 * Libraries is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see
 * <http://www.gnu.org/licenses/> or write to Free Software Foundation,
 * 51 Franklin Street, Fifth Floor Boston, MA 02110, USA.
 *
 * Our contact details: Parallels IP Holdings GmbH, Vordergasse 59, 8200
 * Schaffhausen, Switzerland.
 */

#pragma once

/**
 * Checks whether given string matches to given pattern
 *
 * @param s[in] - string
 * @param p[in] - pattern; '*' and '?' wildcards supported
 * @return non-zero if the string matches to the pattern
 */
int wildcardMatch(const char *s, const char *p);
int wildcardMatchUnicode(const unsigned short *s, const unsigned short *p);

