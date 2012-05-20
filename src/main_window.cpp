/**
 * Project: forensics-data-extractor
 * File name: main_window.cpp
 * Description: describes the main Qt window
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

#include "main_window.h"
#include "ui_main_window.h"

Main_Window::Main_Window(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::Main_Window)
{
	chrome_engine = NULL;
	firefox_engine = NULL;
	ui->setupUi(this);

	clean_models();
}

Main_Window::~Main_Window()
{
	delete ui;

	if ( chrome_engine != NULL )
		delete chrome_engine;

	if ( firefox_engine != NULL )
		delete firefox_engine;

	if ( search_engine != NULL )
		delete search_engine;
}

void Main_Window::on_browse_button_clicked()
{
	ui->directory_line->setText(QFileDialog::getExistingDirectory(this, tr("Find Files"), QDir::currentPath()));
}

void Main_Window::on_scan_button_clicked()
{
	ui->scan_button->setDisabled(true);
	process_scan();
	ui->scan_button->setEnabled(true);

	ui->cookies_number->setText(QString::number(model_cookies.rowCount()));
	ui->downloads_number->setText(QString::number(model_downloads.rowCount()));
	ui->forms_number->setText(QString::number(model_forms.rowCount()));
	ui->places_number->setText(QString::number(model_places.rowCount()));
	ui->search_number->setText(QString::number(model_search.rowCount()));
	ui->signons_number->setText(QString::number(model_signons.rowCount()));
}

void Main_Window::process_scan() {
	if ( ui->directory_line->text().isEmpty() == true ) {
		return;
	}

	search_engine = new Indexing_Engine(ui->directory_line->text());
//	search_engine = new Indexing_Engine(ui->directory_line->text(), ui->scan_status_text);

	chrome_engine = new Chrome_Extractor(&model_cookies, &model_downloads, &model_forms, &model_places, &model_search, &model_signons);
	firefox_engine = new Firefox_Extractor(&model_cookies, &model_downloads, &model_forms, &model_places, &model_search, &model_signons);

	/*
	 * Clean the models to prevent duplicates
	 */
	clean_models();

	/*
	 * Searching process
	 */
	search_engine->start();
	chrome_engine->start();
	firefox_engine->start();
}

void Main_Window::clean_models() {
	QStringList headers;

	/*
	 * Places
	 */
	headers << "Visites" << "Site";
	model_places.clear();
	model_places.setHorizontalHeaderLabels(headers);
	ui->places_view->setModel(&model_places);
	headers.clear();

	/*
	 * Cookies
	 */
	headers << "Nom" <<  "Valeur" <<  "Hte" << "Chemin" <<  "Expiration" << "Scuris" << "HTTP" << "lastAccessed" << "Dernier accs";
	model_cookies.clear();
	model_cookies.setHorizontalHeaderLabels(headers);
	ui->cookies_view->setModel(&model_cookies);
	headers.clear();

	/*
	 * Downloads
	 */
	headers << "Name" << "Sources" << "Type MIME";
	model_downloads.clear();
	model_downloads.setHorizontalHeaderLabels(headers);
	ui->downloads_view->setModel(&model_downloads);
	headers.clear();

	/*
	 * Forms
	 */
	headers << "Hte" << "Identifiant" << "Mot de passe";
	model_forms.clear();
	model_forms.setHorizontalHeaderLabels(headers);
	ui->forms_view->setModel(&model_forms);
	headers.clear();

	/*
	 * Searches
	 */
	headers << "Noms" << "Valeurs";
	model_search.clear();
	model_search.setHorizontalHeaderLabels(headers);
	ui->search_view->setModel(&model_search);
	headers.clear();

	/*
	 * Signons
	 */
	headers << "Hôte" << "Identifiant" << "Mot de passe";
	model_signons.clear();
	model_signons.setHorizontalHeaderLabels(headers);
	ui->signons_view->setModel(&model_signons);
	headers.clear();
}
