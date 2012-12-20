#include "extractors/extractor_select.h"

Extractor_Select::Extractor_Select()
{
	dico.clear();
	dico.insert("chrome", Chrome_Extractor::get_regexes());
	dico.insert("firefox", Firefox_Extractor::get_regexes());
}

Extractor_Select::~Extractor_Select() {
}

QString	Extractor_Select::select(const QString& file_name) {
	Q_FOREACH(QString extractor, dico.keys()) {
		Q_FOREACH(QRegExp regex, dico.value(extractor)) {
			if ( regex.exactMatch(file_name) == true ) {
				return extractor;
			}
		}
	}

	return "";
}
