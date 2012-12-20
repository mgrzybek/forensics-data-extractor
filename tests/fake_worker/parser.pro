# Project: forensics-data-extractor
# File name: forensics-data-extractor.pro
# Description: describes the Qt project and how to build it
#
# @author Mathieu Grzybek on 2012-11-08
# @copyright 2012 Mathieu Grzybek. All rights reserved.
# @version $Id: code-gpl-license.txt,v 1.2 2004/05/04 13:19:30 garry Exp $
#
# @see The GNU Public License (GPL) version 3 or higher
#
#
# forensics-data-extractor is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
# for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA

QT		+= core sql

TARGET		= parser
TEMPLATE	= app

INCLUDEPATH	+= ../../include
LIBS		+= -lssl -lcrypto -lzmq

SOURCES		+= parser.cpp \
		fake_worker.cpp \
		../../src/analysis/parsing_engine.cpp \
		../../src/databases/generic_database.cpp \
		../../src/extractors/extractor_select.cpp \
		../../src/extractors/generic_extractor.cpp \
		../../src/extractors/web_browser_extractor.cpp \
		../../src/extractors/chrome_extractor.cpp \
		../../src/extractors/firefox_extractor.cpp \
		../../src/analysis/checksum.cpp \
		../../src/analysis/database.cpp \
		../../src/exception.cpp

HEADERS		+= parser.h \
		fake_worker.h \
		../../include/analysis/parsing_engine.h \
		../../include/databases/generic_database.h\
		../../include/extractors/extractor_select.h\
		../../include/extractors/generic_extractor.h \
		../../include/extractors/web_browser_extractor.h \
		../../include/extractors/chrome_extractor.h \
		../../include/extractors/firefox_extractor.h \
		../../include/analysis/checksum.h \
		../../include/analysis/database.h \
		../../include/common.h \
		../../include/exception.h

