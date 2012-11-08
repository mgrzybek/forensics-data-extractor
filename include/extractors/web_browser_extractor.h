/**
 * Project: forensics-data-extractor
 * File name: web_browser_extractor.h
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

#ifndef WEB_BROWSER_EXTRACTOR_H
#define WEB_BROWSER_EXTRACTOR_H

#include <QFileSystemModel>
#include <QStringBuilder>
#include <QRegExp>
#include <QDebug>
#include <QtSql>
#include <QMap>

#include <zmq.hpp>

#include "common.h"
#include "extractors/generic_extractor.h"

class Web_Browser_Extractor : public Generic_Extractor
{
//	Q_OBJECT

	public:
		/*
		 * Constructor
		 *
		 * @arg	z_context	: ZMQ context to be used
		 * @arg z_output_uri	: the ZMQ URI to send results to
		 * @arg file_path		: the file to process
		 *
		 */
		Web_Browser_Extractor(void* z_context, const std::string& z_output_uri, const QString& file_path);

		/*
		 * Destructor
		 *
		 * Declared as virtual: each derived class has specific objects to destroy
		 *
		 */
		~Web_Browser_Extractor() = 0;

		/*
		 * run
		 *
		 * This method needs to be written to allow QThread to start
		 *
		 */
		void	run();

		virtual void	files_filter() = 0;

		virtual void	extract_places() = 0;
		virtual void	extract_cookies() = 0;
		virtual void	extract_downloads() = 0;
		virtual void	extract_forms() = 0;
		virtual void	extract_search() = 0;
		virtual void	extract_signons() = 0;

	protected:
		QString				dir_path;

		QMap<QString, uint>	url_map;
		QMap<QString, uint>	keyword_map;

		web_browser_models*	models;
		//web_browser_analysed_files	files;
		QStringList			files;

		/*
		 * update_map
		 *
		 * Function used to update an agregation map
		 *
		 * @arg map		: the map to update
		 * @arg key		: the key to insert / update
		 * @arg value	: the value to set / increment
		 */
		void	update_map(QMap<QString, uint>& map, const QString& key, const uint& value);

		/*
		 * update_db_places
		 *
		 * Function used to exec SQL queries using url_map
		 */
		void	update_db_places();

		/*
		 * update_db_search
		 *
		 * Function used to exec SQL queries using keyword_map
		 */
		void	update_db_search();

		/*
		 * set_file_to_analysed()
		 *
		 *
		 */
		void	set_file_to_analysed();
};

#endif // WEB_BROWSER_EXTRACTOR_H

