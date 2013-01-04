#ifndef EXTRACTOR_SELECT_H
#define EXTRACTOR_SELECT_H

#include "common.h"
#include "extractors/chrome_extractor.h"
#include "extractors/firefox_extractor.h"
#include "extractors/internet_explorer_extractor.h"

class Extractor_Select
{
	public:
		Extractor_Select();
		~Extractor_Select();

		QString	select(const QString& file_name);

	private:
		/*
		 * Patterns' dictionnary
		 */
		h_dico	dico;
};

#endif // EXTRACTOR_SELECT_H
