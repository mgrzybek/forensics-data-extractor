/**
 * Project: forensics-data-extractor
 * File name: nsrl.h
 * Description: describes the NSRL interface
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

#ifndef NSRL_H
#define NSRL_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

#include "databases/generic_database.h"

class NSRL : public Generic_Database
{
	public:
		NSRL(const h_known_db_config& config);
		~NSRL();

		bool	is_known(const struct_file& file);
		bool	update_connection_info(const h_known_db_config& config);

	private:
		QSqlDatabase	db;
};

#endif // NSRL_H
