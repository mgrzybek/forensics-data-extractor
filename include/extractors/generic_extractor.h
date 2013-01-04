/**
 * Project: forensics-data-extractor
 * File name: generic_extractor.h
 * Description: describes the abstract object used to process data
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

#ifndef GENERIC_EXTRACTOR_H
#define GENERIC_EXTRACTOR_H

#include <QRunnable>
#include <QSqlField>
#include <QSqlDriver>

#include <string>
#include <zmq.hpp>

#include "common.h"

class Generic_Extractor : public QRunnable
{
//	Q_OBJECT

	public:
		// TODO: think of using struct_file instead of file_path
		Generic_Extractor(void* z_context, const std::string& z_output_uri, const QString& file_path);

		virtual void		run() = 0;
		static regex_list	get_regexes();

//	signals:
//		void	finished();

	protected:
		zmq::context_t*	zmq_context;
		std::string	zmq_output_uri;

		QString		file;

		Exception	error;

		void	send_zmq(const QString& message);
};

#endif // GENERIC_EXTRACTOR_H
