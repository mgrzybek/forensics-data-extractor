/**
 * Project: forensics-data-extractor
 * File name: parser.cpp
 * Description: describes the command line tool to parse an image or a directory
 *
 * @author Mathieu Grzybek on 2012-12-20
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

#include "parser.h"

int	main(int argc, char *argv[]) {
	QCoreApplication::setOrganizationName("Forensics-Data-Extractor");
	QCoreApplication::setApplicationName("Parser");

	QThreadPool	thread_pool;

	QStringList	paths;
	QString		db_file;

	if ( argc < 5 ) {
		usage();
		return EXIT_FAILURE;
	}

	if ( strcmp(argv[1], "-d") == 0 and strcmp(argv[3], "-s") == 0 ) {
		db_file = argv[2];
		for (int i = 4 ; i < argc ; i++ )
			paths << argv[i];
	} else {
		usage();
		return EXIT_FAILURE;
	}

	Database	database(db_file);

	try {
		Q_FOREACH (QString path, paths) {
			Parsing_Engine*	parser = new Parsing_Engine(path, &database, NULL);
			thread_pool.start(parser);
		}
	} catch (std::exception& e) {
		std::cerr << e.what();
		return EXIT_FAILURE;
	}

	thread_pool.waitForDone();

	std::cout << database.get_row_count("parsed_file") << " files parsed" << std::endl;
	std::cout << thread_pool.maxThreadCount() << " threads used" << std::endl;

	return EXIT_SUCCESS;
}

void	usage() {
	std::cerr<< "parser -d <database_file> -s <source> [<source>]*" << std::endl;
}

