/**
 * Project: forensics-data-extractor
 * File name: indexing_engine.cpp
 * Description: describes the indexing engine that lists and processes the files
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

#include "analysis/indexing_engine.h"

Indexing_Engine::Indexing_Engine(const QString& e_path, const QString& w_directory) {
	engine_path = e_path;
	working_directory = w_directory;
}

void    Indexing_Engine::run() {
	QProcess process;
	QStringList arguments;

	if ( create_conf_file() == false )
		return;

	arguments << "-d" << working_directory;

	process.startDetached(engine_path, arguments);
}

bool    Indexing_Engine::create_conf_file() {
	if ( indexed_folders.size() == 0 )
		return false;

	QString file_path = working_directory + "/" + "daemon.conf";
	QFile   file(file_path);

	if ( ! file.open(QIODevice::WriteOnly | QIODevice::Text) )
		return false;

	QTextStream out(&file);
	out << "<strigiDaemonConfiguration useDBus='1'>"
	    << "      <repository name='localhost' writeable='1' pollingInterval='180' urlbase='' indexdir='./clucene' type='clucene'>";

	Q_FOREACH(QString folder, indexed_folders) {
		out << "              <path path='" << folder << "'></path>";
	}

	out << "      </repository>"
	    << "      <filters>"
	    << "            <filter pattern='.*.directory/' include='1'></filter>"
	    << "            <filter pattern='.*/' include='0'></filter>"
	    << "            <filter pattern='.*' include='0'></filter>"
	    << "      </filters>"
	    << "</strigiDaemonConfiguration>";

	file.close();
	return true;
}

void    Indexing_Engine::add_indexed_folders(const QStringList& folders) {
	indexed_folders.append(folders);
}

