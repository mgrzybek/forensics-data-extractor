/**
 * Project: forensics-data-extractor
 * File name: internet_explorer_extractor.cpp
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

#include "extractors/internet_explorer_extractor.h"

Internet_Explorer_Extractor::Internet_Explorer_Extractor(
                void*		z_context,
                const std::string& z_output_uri,
                const QString& file_path
                ) : Web_Browser_Extractor(
                            z_context, z_output_uri, file_path
                            )
{
	url_map.clear();
}


Internet_Explorer_Extractor::~Internet_Explorer_Extractor() {
}

void	Internet_Explorer_Extractor::files_filter() {
	QStringList file_path_parts = file.split("/", QString::SkipEmptyParts);
	QString file_name = file_path_parts.at(file_path_parts.size() - 1);

	if ( file_name.compare("index.dat") == 0 ) {
		parse_index_dat();
	}
}

regex_list	Internet_Explorer_Extractor::get_regexes() {
}

void	Internet_Explorer_Extractor::parse_index_dat() {
	int history_file;
	char fourbytes[4];
	int currrecoff;
	int filesize;
	int i;
	char type[5];
	char hashrecflagsstr[4];
	int hashoff;
	int hashsize;
	int nexthashoff;
	int offset;
	int hashrecflags;
	int deleted = 0;
	double filever;

	history_file = open( file.toLatin1().constData(), O_RDONLY, 0 );

	if ( history_file <= 0 ) {
		error.msg = "The index.dat file cannot be opened!";
		throw error;
	}

	pread( history_file, fourbytes, 4, 0x18 );
	filever = atof( fourbytes );

	pread( history_file, fourbytes, 4, 0x1C );
	filesize = bah_to_i( fourbytes, 4 );

	if (deleted == 0) {

		pread( history_file, fourbytes, 4, 0x20 );
		hashoff = bah_to_i( fourbytes, 4 );

		while (hashoff != 0 ) {

			pread( history_file, fourbytes, 4, hashoff+8 );
			nexthashoff = bah_to_i( fourbytes, 4 );

			pread( history_file, fourbytes, 4, hashoff+4 );
			hashsize = bah_to_i( fourbytes, 4 )*BLOCK_SIZE;

			for (offset = hashoff + 16; offset < hashoff+hashsize; offset = offset+8) {
				pread( history_file, hashrecflagsstr, 4, offset );
				hashrecflags = bah_to_i( hashrecflagsstr, 4 );

				pread( history_file, fourbytes, 4, offset+4 );
				currrecoff = bah_to_i( fourbytes, 4 );

				if (hashrecflagsstr[0] != 0x03 && currrecoff != 0xBADF00D ) {
					if (currrecoff != 0) {

						pread( history_file, fourbytes, 4, currrecoff );

						for (i=0;i < 4;i++) {
							type[i] = fourbytes[i];
						}
						type[4] = '\0';

						if (type[0] == 'R' && type[1] == 'E' && type[2] == 'D' && type[3] == 'R' ) {
							parse_redr( history_file, filever, currrecoff, filesize, type );
						} else if ( (type[0] == 'U' && type[1] == 'R' && type[2] == 'L') || (type[0] == 'L' && type[1] == 'E' && type[2] == 'A' && type[3] == 'K') ) {
							parse_url( history_file, filever, currrecoff, filesize, type );
						} else {
							parse_unknown( history_file, filever, currrecoff, filesize, type );
						}
					}
				}
			}
			hashoff = nexthashoff;
		}
	} else if (deleted == 1) {
		currrecoff = 0;

		while (currrecoff < filesize ) {

			pread( history_file, fourbytes, 4, currrecoff );

			for (i=0;i < 4;i++) {
				type[i] = fourbytes[i];
			}
			type[4] = '\0';

			if (type[0] == 'R' && type[1] == 'E' && type[2] == 'D' && type[3] == 'R' ) {
				parse_redr( history_file, filever, currrecoff, filesize, type );
			} else if ( (type[0] == 'U' && type[1] == 'R' && type[2] == 'L') || (type[0] == 'L' && type[1] == 'E' && type[2] == 'A' && type[3] == 'K') ) {
				parse_url( history_file, filever, currrecoff, filesize, type );
			} else {
				parse_unknown( history_file, filever, currrecoff, filesize, type );
			}

			currrecoff = currrecoff + BLOCK_SIZE;
		}

	}
	close (history_file);
}

void	Internet_Explorer_Extractor::extract_places() {
}

void	Internet_Explorer_Extractor::extract_cookies() {
}

void	Internet_Explorer_Extractor::extract_downloads() {
}

void	Internet_Explorer_Extractor::extract_forms() {
}

void	Internet_Explorer_Extractor::extract_search() {
}

void	Internet_Explorer_Extractor::extract_signons() {
}

//
/* This function parses a REDR record. */
//
int Internet_Explorer_Extractor::parse_redr( int history_file, double filever, int currrecoff, int filesize, char *type ) {
	char fourbytes[4];
	char chr;
	int i;
	int reclen;
	char *url;
	char *filename;
	char *httpheaders;
	char ascmodtime[26], ascaccesstime[26];
	char dirname[9];

	pread( history_file, fourbytes, 4, currrecoff+4 );
	reclen = bah_to_i( fourbytes, 4 )*BLOCK_SIZE;

	url = (char *)malloc( reclen+1 );

	i = 0;
	pread( history_file, &chr, 1, currrecoff+0x10 );
	while ( chr != '\0' && currrecoff+0x10+i+1 < filesize ) {
		url[i] = chr;
		pread( history_file, &chr, 1, currrecoff+0x10+i+1 );
		i++;
	}
	url[i] = '\0';

	filename = (char *)malloc( 1 );
	filename[0] = '\0';

	httpheaders = (char *)malloc( 1 );
	httpheaders[0] = '\0';

	dirname[0] = '\0';

	ascmodtime[0] = '\0';
	ascaccesstime[0] = '\0';
	dirname[0] = '\0';

	printablestring( type );
	printablestring( url );
	printablestring( ascmodtime );
	printablestring( ascaccesstime );
	printablestring( filename );
	printablestring( dirname );
	printablestring( httpheaders );
//	printf( "%s%s%s%s%s%s%s%s%s%s%s%s%s\n", type, delim, url, delim, ascmodtime, delim, ascaccesstime, delim, filename, delim, dirname, delim, httpheaders );
	// TODO: update it
	type[0] = '\0';

	free( url );
	free( filename );
	free( httpheaders );
}

//
/* This function parses a URL and LEAK activity record. */
//
int Internet_Explorer_Extractor::parse_url( int history_file, double filever, int currrecoff, int filesize, char *type ) {
	char fourbytes[4];
	char hashrecflagsstr[4];
	char eightbytes[8];
	char chr;
	int filenameoff;
	int httpheadersoff;
	int urloff;
	int i;
	int reclen;
	int dirnameoff;
	time_t modtime;
	time_t accesstime;
	char ascmodtime[26], ascaccesstime[26];
	char dirname[9];
	char *url;
	char *filename;
	char *httpheaders;
	int year, mon;
	struct tm *accesstm, *modtm;

	QString	host;
	QRegExp	url_cleaner("^PrivacIE\\:(.+?)\\/.*");
	QRegExp slash_cleaner("\\/$");

	url_cleaner.setMinimal(true);

	pread( history_file, fourbytes, 4, currrecoff+4 );
	reclen = bah_to_i( fourbytes, 4 )*BLOCK_SIZE;

	pread( history_file, eightbytes, 8, currrecoff+8 );
	modtime = win_time_to_unix( eightbytes );

	pread( history_file, eightbytes, 8, currrecoff+16 );
	accesstime = win_time_to_unix( eightbytes );

	accesstm = localtime( &accesstime );
	year = accesstm->tm_year + 1900;
	mon = accesstm->tm_mon + 1;
	sprintf( ascaccesstime, "%02d/%02d/%02d %02d:%02d:%02d", mon, accesstm->tm_mday, year, accesstm->tm_hour, accesstm->tm_min, accesstm->tm_sec );

	modtm = localtime( &modtime );
	year = modtm->tm_year + 1900;
	mon = modtm->tm_mon + 1;
	sprintf( ascmodtime, "%02d/%02d/%02d %02d:%02d:%02d", mon, modtm->tm_mday, year, modtm->tm_hour, modtm->tm_min, modtm->tm_sec );

	if (accesstime == 0) {
		ascaccesstime[0] = '\0';
	}

	if (modtime == 0) {
		ascmodtime[0] = '\0';
	}

	url = (char *)malloc( reclen+1 );

	if (filever >= 5) {
		pread( history_file, &chr, 1, currrecoff+0x34 );
	} else {
		pread( history_file, &chr, 1, currrecoff+0x38 );
	}
	urloff = (unsigned char)chr;

	i = 0;
	pread( history_file, &chr, 1, currrecoff+urloff );
	while ( chr != '\0' && currrecoff+urloff+i+1 < filesize ) {
		url[i] = chr;
		pread( history_file, &chr, 1, currrecoff+urloff+i+1 );
		i++;
	}
	url[i] = '\0';

	filename = (char *)malloc( reclen+1 );

	if (filever >= 5) {
		pread( history_file, fourbytes, 4, currrecoff+0x3C );
	} else {
		pread( history_file, fourbytes, 4, currrecoff+0x40 );
	}
	filenameoff = bah_to_i( fourbytes, 4 ) + currrecoff;

	i = 0;
	if (filenameoff > currrecoff+0x3C) {
		pread( history_file, &chr, 1, filenameoff );
		while ( chr != '\0' && filenameoff+i+1 < filesize ) {
			filename[i] = chr;
			pread( history_file, &chr, 1, filenameoff+i+1 );
			i++;
		}
	}
	filename[i] = '\0';

	if (filever >= 5.2) {
		pread( history_file, &chr, 1, currrecoff+0x38 );
	} else if (filever >= 5) {
		pread( history_file, &chr, 1, currrecoff+0x39 );
	} else {
		pread( history_file, &chr, 1, currrecoff+0x3C );
	}
	dirnameoff = (unsigned char)chr;

	if (0x50+(12*dirnameoff)+8 < filesize) {
		pread( history_file, dirname, 8, 0x50+(12*dirnameoff) );
		dirname[8] = '\0';
	} else {
		dirname[0] = '\0';
	}

	httpheaders = (char *)malloc( reclen+1 );

	if (filever >= 5) {
		pread( history_file, fourbytes, 4, currrecoff+0x44 );
	} else {
		pread( history_file, fourbytes, 4, currrecoff+0x48 );
	}
	httpheadersoff = bah_to_i( fourbytes, 4 ) + currrecoff;

	i = 0;
	if (httpheadersoff > currrecoff+0x44) {
		pread( history_file, &chr, 1, httpheadersoff );

		while ( chr != '\0' && httpheadersoff+i+1 < currrecoff+reclen && httpheadersoff+i+1 < filesize ) {
			httpheaders[i] = chr;
			pread( history_file, &chr, 1, httpheadersoff+i+1 );
			i++;
		}
	}
	httpheaders[i] = '\0';

	printablestring( type );
	printablestring( url );
	printablestring( ascmodtime );
	printablestring( ascaccesstime );
	printablestring( filename );
	printablestring( dirname );
	printablestring( httpheaders );

	if (type[3] == ' ') {
		type[3] = '\0';
	}

//	printf( "%s%s%s%s%s%s%s%s%s%s%s%s%s\n", type, delim, url, delim, ascmodtime, delim, ascaccesstime, delim, filename, delim, dirname, delim, httpheaders );

	if ( url_cleaner.indexIn(url) != -1 ) {
		host = url_cleaner.cap(1);
		update_map(url_map, host, 1);
	}

	type[0] = '\0';
	dirname[0] = '\0';
	ascmodtime[0] = '\0';
	ascaccesstime[0] = '\0';

	free( url );
	free( filename );
	free( httpheaders );
}

int Internet_Explorer_Extractor::parse_unknown( int history_file, double filever, int currrecoff, int filesize, char *type ) {
	type[0] = '\0';
}


#ifdef CYGWIN
ssize_t Internet_Explorer_Extractor::pread( int d, void *buf, size_t nbytes, off_t offset) {
	lseek( d, offset, SEEK_SET );
	read( d, buf, nbytes );
}
#endif

//
/* Backwards ASCII Hex to Integer */
//
unsigned int Internet_Explorer_Extractor::bah_to_i( char *val, int size ) {
	int total;
	int i;

	total = 0;

	for ( i=0; i < size; i++ ) {
		total += ((unsigned char)val[i] << 8*i);
	}

	return total;
}

//
/* Backwards 8 byte ASCII Hex to time_t */
//
time_t Internet_Explorer_Extractor::win_time_to_unix( char *val ) {
	unsigned long low, high;
	double dbl;
	time_t total;

	char fourbytes[4];

	fourbytes[0] = val[0];
	fourbytes[1] = val[1];
	fourbytes[2] = val[2];
	fourbytes[3] = val[3];

	low = bah_to_i( fourbytes, 4 );

	fourbytes[0] = val[4];
	fourbytes[1] = val[5];
	fourbytes[2] = val[6];
	fourbytes[3] = val[7];

	high = bah_to_i( fourbytes, 4 );

	dbl = ((double)high)*(pow(2,32));
	dbl += (double)(low);

	if ( dbl==0 ) {
		return 0;
	}

	dbl *= 1.0e-7;
	dbl -= 11644473600;

	total = (double)dbl;

	return total;
}

//
/* This function prepares a string for nice output */
//
int Internet_Explorer_Extractor::printablestring( char *str ) {
	int i;

	i = 0;
	while ( str[i] != '\0' ) {
		if ( (unsigned char)str[i] < 32 || (unsigned char)str[i] > 127 ) {
			str[i] = ' ';
		}
		i++;
	}
	return 0;
}
