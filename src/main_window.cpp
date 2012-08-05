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

Main_Window::Main_Window(void* z_context, QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::Main_Window)
{
	zmq_context = (zmq::context_t*) z_context;

	db = NULL;
	chrome_engine = NULL;
	firefox_engine = NULL;
	search_engine = NULL;
	sorted_model_extracted_files = NULL;

	ui->setupUi(this);

	ui->tab_results->setVisible(false);
	clean_models();
}

Main_Window::~Main_Window()
{
	delete ui;

	if ( db != NULL )
		delete db;

	if ( chrome_engine != NULL )
		delete chrome_engine;

	if ( firefox_engine != NULL )
		delete firefox_engine;

	if ( search_engine != NULL )
		delete search_engine;

	if ( sorted_model_extracted_files != NULL )
		delete sorted_model_extracted_files;
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
}

void Main_Window::update_info() {
	ui->cookies_number->setText(QString::number(web_models.cookies.rowCount()));
	ui->downloads_number->setText(QString::number(web_models.downloads.rowCount()));
	ui->forms_number->setText(QString::number(web_models.forms.rowCount()));
	ui->places_number->setText(QString::number(web_models.places.rowCount()));
	ui->search_number->setText(QString::number(web_models.searches.rowCount()));
	ui->signons_number->setText(QString::number(web_models.signons.rowCount()));
	ui->indexed_files_number->setText(QString::number(model_indexed_files.rowCount()));

	ui->cookies_view->resizeColumnsToContents();
	ui->downloads_view->resizeColumnsToContents();
	ui->forms_view->resizeColumnsToContents();
	ui->places_view->resizeColumnsToContents();
	ui->search_view->resizeColumnsToContents();
	ui->signons_view->resizeColumnsToContents();
}

void Main_Window::launch_extractors() {
//	chrome_engine->start();
//	firefox_engine->start();
}


void Main_Window::process_scan() {
	if ( ui->directory_line->text().isEmpty() == true ) {
		return;
	}

	search_engine = new Indexing_Engine((void*)zmq_context, db, ui->directory_line->text(), &model_indexed_files);

	chrome_engine = new Chrome_Extractor((void*)zmq_context, &web_models);
	firefox_engine = new Firefox_Extractor((void*)zmq_context, &web_models);

	/*
	 * Clean the models to prevent duplicates
	 */
	clean_models();
	connect(search_engine, SIGNAL(finished()), this, SLOT(update_info()));
	connect(search_engine, SIGNAL(ready()), this, SLOT(launch_extractors()));

	/*
	 * Searching process
	 */
	search_engine->start();
}

void Main_Window::clean_models() {
	QStringList headers;

	/*
	 * Places
	 */
	headers << "Visites" << "Site";
	web_models.places.clear();
	web_models.places.setHorizontalHeaderLabels(headers);
	ui->places_view->setModel(&web_models.places);
	headers.clear();

	/*
	 * Cookies
	 */
	headers << "Nom" <<  "Valeur" <<  "Hôte" << "Chemin" <<  "Expiration" << "Scuris" << "HTTP" << "lastAccessed" << "Dernier accès";
	web_models.cookies.clear();
	web_models.cookies.setHorizontalHeaderLabels(headers);
	ui->cookies_view->setModel(&web_models.cookies);
	headers.clear();

	/*
	 * Downloads
	 */
	headers << "Name" << "Sources" << "Type MIME";
	web_models.downloads.clear();
	web_models.downloads.setHorizontalHeaderLabels(headers);
	ui->downloads_view->setModel(&web_models.downloads);
	headers.clear();

	/*
	 * Forms
	 */
	headers << "Hte" << "Identifiant" << "Mot de passe";
	web_models.forms.clear();
	web_models.forms.setHorizontalHeaderLabels(headers);
	ui->forms_view->setModel(&web_models.forms);
	headers.clear();

	/*
	 * Searches
	 */
	headers << "Noms" << "Valeurs";
	web_models.searches.clear();
	web_models.searches.setHorizontalHeaderLabels(headers);
	ui->search_view->setModel(&web_models.searches);
	headers.clear();

	/*
	 * Signons
	 */
	headers << "Hôte" << "Identifiant" << "Mot de passe";
	web_models.signons.clear();
	web_models.signons.setHorizontalHeaderLabels(headers);
	ui->signons_view->setModel(&web_models.signons);
	headers.clear();

	/*
	 * Indexed files list
	 */
	headers << "Fichiers";
	model_indexed_files.clear();
	model_indexed_files.setHorizontalHeaderLabels(headers);
	ui->indexed_list->setModel(&model_indexed_files);
	headers.clear();

	/*
	 * Extracted files list
	 */
	sorted_model_extracted_files = new QSortFilterProxyModel();
	sorted_model_extracted_files->sort(1, Qt::AscendingOrder);
	sorted_model_extracted_files->setDynamicSortFilter(true);
	sorted_model_extracted_files->setSourceModel(&web_models.extracted_files);

	headers << "Fichiers";
	web_models.extracted_files.clear();
	web_models.extracted_files.setHorizontalHeaderLabels(headers);
	ui->extracted_list->setModel(sorted_model_extracted_files);
	headers.clear();
}

void Main_Window::on_action_New_Analysis_triggered()
{
	QString dir = QFileDialog::getSaveFileName(this, "New Analysis", "~/", "");

	if ( dir.isEmpty() == true )
		return;

	dir.replace(":","/");

	QDir	project_directory(dir);

	if ( project_directory.mkdir(project_directory.path()) == false ) {
		QMessageBox::critical(this, tr("Creation failed"), tr("The project cannot be created"));
	} else {
		QString db_file;

		working_directory = project_directory.path();

		db_file += working_directory + "/index.db";
		init_db(db_file);

		ui->action_New_Analysis->setDisabled(true);
		ui->action_Open_Analysis->setDisabled(true);

		ui->tab_results->setVisible(true);
	}
}

void Main_Window::on_action_Quit_triggered()
{
	setAttribute(Qt::WA_DeleteOnClose);
	close();
}

void Main_Window::on_action_Open_Analysis_triggered()
{
	QString buffer_file = QFileDialog::getOpenFileName(this, "Open Analysis", "~/", "");

	if ( buffer_file.isEmpty() == true )
		return;

	QFileInfo	project_file(buffer_file);
	working_directory = project_file.absoluteDir().dirName();

	ui->action_New_Analysis->setDisabled(true);
	ui->action_Open_Analysis->setDisabled(true);

	ui->tab_results->setVisible(true);
}

void Main_Window::on_action_Close_Analysis_triggered()
{
	ui->action_Close_Analysis->setDisabled(true);
	ui->action_New_Analysis->setEnabled(true);
	ui->action_Open_Analysis->setEnabled(true);

	ui->tab_results->setVisible(false);
}

bool Main_Window::init_db(const QString& db_file) {
	if ( db_file.isEmpty() == true )
		return false;

	db = new Sqlite_Backend(db_file);

	if ( db->open_db() == false )
		return false;

	db->exec("CREATE TABLE IF NOT EXIST file ( file_id INTEGER, basename TEXT NOT NULL, complete_suffix TEXT NOT NULL, canonical_path TEXT NOT NULL, size INTEGER NOT NULL, last_modified INTEGER NOT NULL, last_read INTEGER NOT NULL);");

//	db.exec("CREATE TABLE mime_type ();");


	return true;
}
