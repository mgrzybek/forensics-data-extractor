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
		void*			z_context,
		web_browser_models*	web_models
		)
{
	zmq_context	= (zmq::context_t*) z_context;
	models		= web_models;
}

Web_Browser_Extractor::~Web_Browser_Extractor() {

}

void Web_Browser_Extractor::run()
{
	zmq::socket_t	socket(*zmq_context, ZMQ_SUB);
	bool		connected = false;

	while ( connected == false ) {
		try {
			socket.setsockopt(ZMQ_SUBSCRIBE, 0, 0);
#ifdef WINDOWS_OS
			socket.connect("tcp://127.0.0.1:5555");
#else
			socket.connect("inproc://forensics-parser.inproc");
#endif
			connected = true;
			while (1) {
				QString string_message;
				zmq::message_t  z_message;

				socket.recv(&z_message);
				string_message = static_cast<char*>(z_message.data());

				if ( string_message.compare("END;") != 0 )
					files_filter(string_message);
				else
					break;
			}
			append_extracted_files_to_model_files();
		} catch (const std::exception& e) {
			qCritical() << "Web_Browser_Extractor: " << e.what();
		}
	}
	connected = false;
	update_model_places();
	emit finished();
	qDebug() << "Web_Browser_Extrator: thread end";
}

void Web_Browser_Extractor::append_extracted_files_to_model_files() {
	// cookies
	append_files_to_model_files(files.cookies);
	// downloads
	append_files_to_model_files(files.downloads);
	// forms
	append_files_to_model_files(files.forms);
	// places
	append_files_to_model_files(files.places);
	// search
	append_files_to_model_files(files.searches);
	// signons
	append_files_to_model_files(files.signons);
}

void Web_Browser_Extractor::append_files_to_model_files(const QStringList& f) {
	QList<QStandardItem*>	row;

	Q_FOREACH (QString file, f) {
		row << new QStandardItem(file);
		models->extracted_files.appendRow(row);
		row.clear();
	}
}

void	Web_Browser_Extractor::update_url_map(const QString& url, const uint& count) {
	if ( url.isEmpty() == false and count > 0 ) {
		QMap<QString, uint>::iterator iter = url_map.find(url);

		if ( iter == url_map.end() ) {
			url_map.insert(url, count);
		} else {
			iter.value() += count;
		}
	}
}

void Web_Browser_Extractor::update_model_places() {
	QList<QStandardItem*>	row;

	dir_path.clear();

	for ( QMap<QString, uint>::const_iterator iter = url_map.begin() ; iter != url_map.end() ; iter++ ) {
		row.clear();

		row << new QStandardItem(QString::number(iter.value()));
		row << new QStandardItem(iter.key());

		models->places.appendRow(row);
	}
	row.clear();
	// TODO: keep clear() ?
	url_map.clear();
}

