/**
 * Project: forensics-data-extractor
 * File name: internet_explorer_extractor.h
 * Description: describes the object used to process MS IE data
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

#ifndef INTERNET_EXPLORER_EXTRACTOR_H
#define INTERNET_EXPLORER_EXTRACTOR_H

#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "extractors/web_browser_extractor.h"

//
/* This is the default block size for an activity record */
//
#define BLOCK_SIZE	0x80

class Internet_Explorer_Extractor : public Web_Browser_Extractor
{
	public:
		Internet_Explorer_Extractor(void* z_context, const std::string& z_output_uri, const QString& file_pat);

		~Internet_Explorer_Extractor();

		void	files_filter();
		static regex_list	get_regexes();

	private:
		void	extract_places();
		void	extract_cookies();
		void	extract_downloads();
		void	extract_forms();
		void	extract_search();
		void	extract_signons();

		void	parse_index_dat();
		int	parse_url( int history_file, double filever, int currrecoff, int filesize, char *type );
		int	parse_redr( int history_file, double filever, int currrecoff, int filesize, char *type );
		int	parse_unknown( int history_file, double filever, int currrecoff, int filesize, char *type );
		unsigned int bah_to_i( char *val, int size );
		time_t	win_time_to_unix( char *val );
		int	printablestring( char *str );
};

#endif // INTERNET_EXPLORER_EXTRACTOR_H
