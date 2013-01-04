/**
 * Project: forensics-data-extractor
 * File name: exception.cpp
 * Description: describes the custom exceptions thrown by the program
 *
 * @author Mathieu Grzybek on 2012-10-24
 * @copyright 2012 Mathieu Grzybek. All rights reserved.
 * @version $Id: code-gpl-license.txt,v 1.2 2004/05/04 13:19:30 garry Exp $
 *
 * @see The GNU Public License (GPL) version 3 or higher
 *
 *
 * forensics-data-extractor is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "exception.h"

Exception::Exception() throw() {
}

Exception::Exception(const QString&	method, const QString& message) throw() {
	calling_method = method;
	msg = message;
}

Exception::~Exception() throw() {
}

void	Exception::print() {
	qCritical() << calling_method << ": " << msg;
}

const char* Exception::what() const throw() {
	return msg.toLatin1().constData();
}
