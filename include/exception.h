/**
 * Project: forensics-data-extractor
 * File name: exception.h
 * Description: describes the custom exceptions thrown by the program
 *
 * @author Mathieu Grzybek on 2012-10-24
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

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <QString>
#include <QDebug>

class Exception : public std::exception
{
	public:
		/**
		 * Exception
		 *
		 * This object is used to throw exceptions
		 *
		 * @param	method	the name of the method throwing the exception
		 * @param	message	the error message
		 * @throw	Exception
		 */
		Exception(const QString&	method, const QString& message) throw();

		/**
		 * Exception
		 *
		 * This object is used to throw exceptions
		 * You need to set the calling method and the message manually
		 *
		 * @throw	Exception
		 */
		Exception() throw();

		/**
		 * The destructor
		 */
		~Exception() throw();

		/**
		 * print
		 *
		 * Prints the message to std::err
		 */
		void	print();

		/**
		 * what
		 *
		 * Gives the formatted message
		 *
		 * @return	the message
		 */
		const char* what() const throw();

		/**
		 * calling_method
		 *
		 * The source of the exception
		 */
		QString	calling_method;

		/**
		 * msg
		 *
		 * The explanation of the problem
		 */
		QString	msg;
};

#endif // EXCEPTION_H
