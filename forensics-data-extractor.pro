# Project: forensics-data-extractor
# File name: forensics-data-extractor.pro
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

QT		+= core gui widgets sql

TARGET		= forensics-data-extractor
TEMPLATE	= app

CONFIG		+= qt thread

# C++11 support: ZeroMQ is not ready yet
#QMAKE_CXXFLAGS += -std=c++0x

INCLUDEPATH	+= include

include(qmake_conf/macx.pro)
include(qmake_conf/linux.pro)
include(qmake_conf/windows.pro)

SOURCES		+= src/fde-console.cpp \
		src/gui/main_window.cpp \
		src/gui/configuration.cpp \
		src/analysis/indexing_engine.cpp \
		src/extractors/firefox_extractor.cpp \
		src/extractors/extractor_select.cpp \
		src/extractors/web_browser_extractor.cpp \
		src/extractors/chrome_extractor.cpp \
		src/extractors/internet_explorer_extractor.cpp \
		src/extractors/worker.cpp \
		src/extractors/generic_extractor.cpp \
		src/analysis/parsing_engine.cpp \
		src/analysis/database.cpp \
		src/analysis/checksum.cpp \
		src/exception.cpp \
		src/databases/nsrl.cpp \
		src/databases/generic_database.cpp \
		src/analysis/sleuthkit_wrapper.cpp \
		src/analysis/file_system_wrapper.cpp \
		src/analysis/router.cpp \
		src/analysis/receiver.cpp

HEADERS		+= include/gui/main_window.h \
		include/gui/configuration.h \
		include/common.h \
		include/analysis/indexing_engine.h \
		include/extractors/extractor_select.h \
		include/extractors/firefox_extractor.h \
		include/extractors/web_browser_extractor.h \
		include/extractors/chrome_extractor.h \
		include/extractors/internet_explorer_extractor.h \
		include/analysis/parsing_engine.h \
		include/analysis/database.h \
		include/analysis/checksum.h \
		include/exception.h \
		include/databases/nsrl.h \
		include/databases/generic_database.h \
		include/analysis/sleuthkit_wrapper.h \
		include/analysis/file_system_wrapper.h \
		include/extractors/worker.h \
		include/extractors/generic_extractor.h \
		include/analysis/router.h \
		include/analysis/receiver.h

FORMS	+=	ui/main_window.ui \
		ui/configuration.ui
