# Project: forensics-data-extractor
# File name: extraction.pro
# Description: describes the Qt project and how to build it
#
# @author Mathieu Grzybek on 2012-05-20
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

# Version's control
contains(QT_VERSION, ^5\\.[0-9]\\..*) {
	DEFINES	+= QT_5
}

QT		+= core sql

TARGET		= db
TEMPLATE	= app

CONFIG		+= thread

INCLUDEPATH	+= ../../include

include(../../qmake_conf/macx.pro)
include(../../qmake_conf/linux.pro)
include(../../qmake_conf/windows.pro)

SOURCES		+= extraction.cpp \
		../../src/extractors/chrome_extractor.cpp \
		../../src/extractors/extractor_select.cpp \
		../../src/extractors/firefox_extractor.cpp \
		../../src/extractors/internet_explorer_extractor.cpp \
		../../src/extractors/generic_extractor.cpp \
		../../src/extractors/web_browser_extractor.cpp \
		../../src/extractors/worker.cpp \
		../../src/analysis/database.cpp \
		../../src/analysis/receiver.cpp \
		../../src/exception.cpp

HEADERS		+= extraction.h \
		../../include/analysis/database.h \
		../../include/common.h \
		../../include/extractors/chrome_extractor.h \
		../../include/extractors/extractor_select.h \
		../../include/extractors/firefox_extractor.h \
		../../include/extractors/internet_explorer_extractor.h \
		../../include/extractors/generic_extractor.h \
		../../include/extractors/web_browser_extractor.h \
		../../include/extractors/worker.h \
		../../include/analysis/database.h \
		../../include/analysis/receiver.h \
		../../include/exception.h
