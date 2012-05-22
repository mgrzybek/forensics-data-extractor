/**
 * Project: forensics-data-extractor
 * File name: web_browser_extractor.cpp
 * Description: describes the abstract object used to process web browsers' data
 *
 * @author Mathieu Grzybek on 2012-05-20
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

#include "web_browser_extractor.h"

Web_Browser_Extractor::Web_Browser_Extractor(
	QStandardItemModel* cookies,
	QStandardItemModel* downloads,
	QStandardItemModel* forms,
	QStandardItemModel* places,
	QStandardItemModel* search,
	QStandardItemModel* signons
)
{
	model_cookies   = cookies;
	model_downloads = downloads;
	model_forms     = forms;
	model_places    = places;
	model_search    = search;
	model_signons   = signons;
}

Web_Browser_Extractor::~Web_Browser_Extractor() {
	dir_path.clear();

	model_cookies   = NULL;
	model_downloads = NULL;
	model_forms     = NULL;
	model_places    = NULL;
	model_search    = NULL;
	model_signons   = NULL;
}

void Web_Browser_Extractor::run()
{
	zmq::context_t context(1);
	zmq::socket_t socket(context, ZMQ_SUB);

	socket.setsockopt(ZMQ_SUBSCRIBE, 0, 0);
	socket.connect("inproc://forensics-indexer.inproc");

	while (1) {
		QString string_message;
		zmq::message_t  z_message;

		socket.recv(&z_message);
		string_message = static_cast<char*>(z_message.data());
//		qDebug() << "subscriber: " << string_message;

		if ( string_message.compare("END;") != 0 )
			files_filter(string_message);
		else
			return;
	}
}
