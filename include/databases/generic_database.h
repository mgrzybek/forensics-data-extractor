/**
 * Project: forensics-data-extractor
 * File name: generic_database.cpp
 * Description: describes the virtual interface to query external files databases
 *
 * @author Mathieu Grzybek on 2012-10-29
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

#ifndef GENERIC_DATABASE_H
#define GENERIC_DATABASE_H

#include <QSettings>
#include <QMap>

#include "../common.h"

/*
 * Generic_Database
 *
 * This virtual class is used to describe the shared public methods that every
 * derived class should provide, ie. answering the question:
 * -> Is it a known hash?
 *
 * According to the answer the client will be able to decide to process the
 * target file or not.
 */
class	Generic_Database {
	public :
//		Generic_Database() = 0;
//		~Generic_Database() = 0;

		/*
		 * is_known
		 *
		 * This method queries the remote database and tells us if the given
		 * hash is known.
		 *
		 * @arg	sha1	: the sha1sum of the file
		 * @return		: true (yes)
		 *
		 * TODO: is QString the best chosen data type?
		 */
		virtual bool	is_known(const struct_file& file) = 0;

		virtual bool	update_connection_info(const h_known_db_config& config) = 0;

		/*
		 *
		 *
		 *
		 */
		const char*	get_name();

	protected:
		/*
		 * error
		 *
		 * This exception type is used to throw pre-defined errors
		 */
		Exception	error;

		/*
		 * name
		 *
		 * This is the name of the database
		 */
		QString		name;
};

typedef QMap<QString, Generic_Database*>	generic_database_list;

#endif // GENERIC_DATABAASE_H
