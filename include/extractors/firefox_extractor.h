/**
 * Project: forensics-data-extractor
 * File name: firefox_extractor.h
 * Description: describes the object used to process Mozilla Firefox data
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

#ifndef FIREFOX_EXTRACTOR_H
#define FIREFOX_EXTRACTOR_H

#include "web_browser_extractor.h"

class Firefox_Extractor : public Web_Browser_Extractor
{
	public:
		Firefox_Extractor(void* z_context, const std::string& z_output_uri, const QString& file_path);

		~Firefox_Extractor();

		virtual void	files_filter();
		static regex_list	get_regexes();

	private:
		// TODO: think of scanning RDF files
		void	extract_places();
		void	extract_cookies();
		void	extract_downloads();
		void	extract_forms();
		void	extract_search();
		void	extract_signons();
};

#endif // FIREFOX_EXTRACTOR_H

