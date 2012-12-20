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
	QString	path;
	QString	db_file;

	if ( argc < 5 ) {
		usage();
		return EXIT_FAILURE;
	}

	if ( strcmp(argv[1], "-s") == 0 and strcmp(argv[3], "-d") == 0 ) {
		path = argv[2];
		db_file = argv[4];
	} else {
		if ( strcmp(argv[1], "-d") == 0 and strcmp(argv[3], "-s") == 0 ) {
			db_file = argv[2];
			path = argv[4];
		} else {
			usage();
			return EXIT_FAILURE;
		}
	}

	zmq::context_t	zmq_context(1);
	Database	database(db_file);
	Parsing_Engine	parser((void*) &zmq_context, path, &database);

	try {
		parser.start();
		parser.wait();
	} catch (std::exception& e) {
		std::cerr << e.what();
		return EXIT_FAILURE;
	}

	std::cout << database.get_row_count("parsed_file") << " files parsed" << std::endl;

	return EXIT_SUCCESS;
}

void	usage() {
	std::cout << "parser -s <source_file> -d <database_file>" << std::endl;
}

