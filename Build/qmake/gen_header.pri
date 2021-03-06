#
# gen_header.pri
#
# Copyright (C) 1999-2014 Parallels IP Holdings GmbH
#
# This file is part of Parallels SDK. Parallels SDK is free
# software; you can redistribute it and/or modify it under the
# terms of the GNU Lesser General Public License as published by
# the Free Software Foundation; either version 2.1 of the License,
# or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library.  If not, see
# <http://www.gnu.org/licenses/>.
#
# Our contact details: Parallels IP Holdings GmbH, Vordergasse 59, 8200
# Schaffhausen, Switzerland.
#

gen_header.dependency_type = TYPE_C
gen_header.commands = $$SRC_LEVEL/Build/gen_header.py ${QMAKE_FILE_IN}
gen_header.input = FILES2HEADERS
gen_header.variable_out = HEADERS
gen_header.output = ${QMAKE_FILE_IN}.h
gen_header.depends += $$SRC_LEVEL/Build/gen_header.py
QMAKE_EXTRA_COMPILERS += gen_header
