/**
 * Project: forensics-data-extractor
 * File name: generic_extractor.cpp
 * Description: implements the abstract object used to process data
 *
 * @author Mathieu Grzybek on 2013-01-03
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

#include "extractors/generic_extractor.h"

Generic_Extractor::Generic_Extractor(void* z_context, const std::string& z_output_uri, const QString& file_path) : QRunnable()
{
	error.calling_method = "Generic_Extractor::Generic_Extractor";

	if ( z_context == NULL ) {
		error.msg = "z_context is NULL";

		throw error;;
	}

	if ( z_output_uri.empty() == true) {
		error.msg = "z_output_uri is empty";
		throw error;
	}

	if ( file_path.isEmpty() == true ) {
		error.msg = "file_path is empty";
		throw error;
	}

	zmq_context	= (zmq::context_t*) z_context;
	zmq_output_uri = z_output_uri;
	file = file_path;

	setAutoDelete(true);

	if ( autoDelete() == false ) {
		error.msg = "Cannot set autoDelete option";
		throw error;
	}
}

void	Generic_Extractor::send_zmq(const QString& message) {
	if ( message.length() < 5 ) {
		qCritical() << error.calling_method << ": message 's length is too short: " << message;
		return;
	}

	zmq::socket_t	socket(*zmq_context, ZMQ_PUSH);
	zmq::message_t	z_message(message.size() + 1);

#ifdef WINDOWS_OS
	_snprintf_s((char*)z_message.data(), message.size() + 1, message.size() + 1, "%s", message.toLatin1().constData());
#else
	snprintf((char*)z_message.data(), message.size() + 1, "%s", message.toLatin1().constData());
#endif

	try {
		socket.connect(zmq_output_uri.c_str());
		socket.send(z_message);
	//} catch (const zmq::error_t& z_error) {
	} catch (const std::exception z_error) {
		qCritical() << error.calling_method << ": Cannot connect against " << zmq_output_uri.c_str() << ": " << z_error.what();
	}

	socket.close();
}
