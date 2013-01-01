# Project: forensics-data-extractor
# File name: windows.pro
# Description: describes the Qt project and how to build it
#
# @author Mathieu Grzybek on 2012-12-30
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

win32 {
	# On Windows 0MQ's inproc is not supported, we use TCP connections instead
	DEFINES	+= WINDOWS_OS

	# Hack to avoid problems with QDateTime
	# FIXME: http://qt-project.org/forums/viewthread/22133
	DEFINES	+= NOMINMAX

	LIBS	+= "E:/Devel/ZeroMQ 3.2.2/lib/libzmq-v110-mt-3_2_2.lib" \
	E:/Devel/libewf-20121209/msvscpp/Release/*.lib \
	E:/Devel/sleuthkit-4.0.1/win32/Release/libtsk.lib

	QMAKE_LFLAGS += /NODEFAULTLIB:library /LTCG

#libbfio.lib
#libcstring.lib
#liberror.lib
#libewf.lib
#libfvalue.lib
#libmfcache.lib
#libmfdata.lib
#libnotify.lib
#libuna.lib
#zlib.lib


	INCLUDEPATH	+= "E:/Devel/ZeroMQ 2.2.0/include" \
	E:/Devel/openssl-1.0.1c/inc32 \
	E:/Devel/openssl-1.0.1c/crypto \
	E:/Devel/sleuthkit-4.0.1

}
