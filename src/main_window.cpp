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
	index_engine = NULL;

	ui->setupUi(this);

	ui->tab_results->setVisible(false);
	ui->action_Close_Analysis->setDisabled(true);

	scan_in_progress = false;
	index_in_progress = false;
}

Main_Window::~Main_Window() {
	/*
	 * The threads should be stopped before deleting to avoid segfault
	 */
	// TODO: print a message to tell that the software is waiting for the threads to end
	if ( chrome_engine != NULL ) {
		chrome_engine->wait();
		delete chrome_engine;
	}

	if ( firefox_engine != NULL ) {
		firefox_engine->wait();
		delete firefox_engine;
	}

	if ( search_engine != NULL ) {
		search_engine->wait();
		delete search_engine;
	}

	if ( index_engine != NULL ) {
		index_engine->wait();
		delete index_engine;
	}

	/*
	 * We should delete the tableview after the models
	 */
	delete ui;

	if ( web_models.places != NULL )
		delete web_models.places;

	if ( model_indexed_files != NULL )
		delete model_indexed_files;

	if ( model_analysed_files != NULL )
		delete model_analysed_files;

//	if ( db != NULL )
//		delete db;
}

void Main_Window::on_browse_button_clicked() {
	QString	selected_directory = QFileDialog::getExistingDirectory(this, tr("Find Files"), "~", QFileDialog::ShowDirsOnly);
	QDir	directory(selected_directory);

	if ( directory.exists(selected_directory) == true )
		ui->directory_line->setText(selected_directory);
}

void Main_Window::on_scan_button_clicked() {
	//	if ( scan_in_progress == true ) {
	//		// TODO: send stop signal to the scanner
	//		// TODO: change button's text
	//		// TODO: set the bool to false
	//		emit scan_stop();
	//		ui->scan_button->setText("Scanner");
	//
	//	} else {
	//		ui->scan_button->setText("Stop scanning");
	process_scan();
	//	}
}

void Main_Window::update_info() {
	scan_in_progress = false;
	ui->scan_button->setText("Scanner");

	/*
	 * We do not use model->rowCount() because the select() method does not read everything at the first time.
	 * We use a custom method using a count(*) sql query against the tables.
	 */
	if ( web_models.cookies != NULL ) {
		web_models.cookies->select();
		ui->cookies_number->setText(QString::number(db->get_row_count("cookie")));
		ui->cookies_view->resizeColumnsToContents();
	}

	if ( web_models.downloads != NULL ) {
		web_models.downloads->select();
		ui->downloads_number->setText(QString::number(db->get_row_count("download")));
		ui->downloads_view->resizeColumnsToContents();
	}

	if ( web_models.forms != NULL ) {
		web_models.forms->select();
		ui->forms_number->setText(QString::number(db->get_row_count("form")));
		ui->forms_view->resizeColumnsToContents();
	}

	if ( web_models.places != NULL ) {
		web_models.places->select();
		ui->places_number->setText(QString::number(db->get_row_count("place")));
		ui->places_view->resizeColumnsToContents();
	}

	if ( web_models.searches != NULL ) {
		web_models.searches->select();
		ui->search_number->setText(QString::number(db->get_row_count("search")));
		ui->search_view->resizeColumnsToContents();
	}

	if ( web_models.signons != NULL ) {
		web_models.signons->select();
		ui->signons_number->setText(QString::number(db->get_row_count("signon")));
		ui->signons_view->resizeColumnsToContents();
	}

	if ( model_indexed_files != NULL ) {
		model_indexed_files->select();
		ui->indexed_files_number->setText(QString::number(db->get_row_count("parsed_file")));
	}

	if ( model_analysed_files != NULL ) {
		model_analysed_files->select();
		ui->extracted_list->resizeColumnsToContents();
	}
}

void Main_Window::launch_extractors() {
	chrome_engine->start();
	firefox_engine->start();
}


void Main_Window::process_scan() {
	if ( ui->directory_line->text().isEmpty() == true ) {
		return;
	}

	search_engine = new Parsing_Engine((void*)zmq_context, ui->directory_line->text(), db);

	chrome_engine = new Chrome_Extractor((void*)zmq_context, db);
	firefox_engine = new Firefox_Extractor((void*)zmq_context, db);

	/*
	 * Clean the models to prevent duplicates
	 */
	clean_models();
	connect(search_engine, SIGNAL(ready()), this, SLOT(launch_extractors()));
	connect(search_engine, SIGNAL(finished()), this, SLOT(update_info()));

	connect(chrome_engine, SIGNAL(finished()), this, SLOT(update_info()));
	connect(firefox_engine, SIGNAL(finished()), this, SLOT(update_info()));

	/*
	 * Searching process
	 */
	search_engine->start();
}


void Main_Window::process_index() {
	QStringList folders;

	if ( ui->directory_line->text().isEmpty() == true ) {
		qCritical() << "No folder to index!";
		return;
	}

	folders << ui->directory_line->text();
	qDebug() << folders;

	index_engine = new Indexing_Engine(strigi_daemon_path, working_directory);
	index_engine->add_indexed_folders(folders);

	index_engine->start();
}

void Main_Window::init_models(QSqlDatabase& db) {
	model_indexed_files = new QSqlTableModel(0, db);
	model_indexed_files->setTable("parsed_file");
	model_indexed_files->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model_indexed_files->setSort(1, Qt::DescendingOrder);
	model_indexed_files->select();
	model_indexed_files->setHeaderData(0, Qt::Horizontal, tr("File"));

	model_analysed_files = new QSqlTableModel(0, db);
	model_analysed_files->setTable("analysed_file");
	model_analysed_files->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model_analysed_files->setSort(1, Qt::DescendingOrder);
	model_analysed_files->select();
	model_analysed_files->setHeaderData(0, Qt::Horizontal, tr("File"));

	web_models.places = new QSqlTableModel(0, db);
	web_models.places->setTable("place");
	web_models.places->setEditStrategy(QSqlTableModel::OnManualSubmit);
	web_models.places->setSort(1, Qt::DescendingOrder);
	web_models.places->select();
	web_models.places->setHeaderData(0, Qt::Horizontal, tr("Site"));
	web_models.places->setHeaderData(1, Qt::Horizontal, tr("Hits"));

	web_models.cookies = new QSqlTableModel(0, db);
	web_models.cookies->setTable("cookie");
	web_models.cookies->setEditStrategy(QSqlTableModel::OnManualSubmit);
	web_models.cookies->setSort(1, Qt::DescendingOrder);
	web_models.cookies->select();
	web_models.cookies->setHeaderData(0, Qt::Horizontal, tr("Name"));
	web_models.cookies->setHeaderData(1, Qt::Horizontal, tr("Value"));
	web_models.cookies->setHeaderData(1, Qt::Horizontal, tr("Hosts"));
	web_models.cookies->setHeaderData(1, Qt::Horizontal, tr("Path"));
	web_models.cookies->setHeaderData(1, Qt::Horizontal, tr("Expiration"));
	web_models.cookies->setHeaderData(1, Qt::Horizontal, tr("Secured"));
	web_models.cookies->setHeaderData(1, Qt::Horizontal, tr("HTTP"));
	web_models.cookies->setHeaderData(1, Qt::Horizontal, tr("Last_Accessed"));

	web_models.downloads = new QSqlTableModel(0, db);
	web_models.downloads->setTable("download");
	web_models.downloads->setEditStrategy(QSqlTableModel::OnManualSubmit);
	web_models.downloads->setSort(1, Qt::DescendingOrder);
	web_models.downloads->select();
	web_models.downloads->setHeaderData(0, Qt::Horizontal, tr("Name"));
	web_models.downloads->setHeaderData(1, Qt::Horizontal, tr("Source"));
	web_models.downloads->setHeaderData(1, Qt::Horizontal, tr("Mime"));

	web_models.forms = new QSqlTableModel(0, db);
	web_models.forms->setTable("form");
	web_models.forms->setEditStrategy(QSqlTableModel::OnManualSubmit);
	web_models.forms->setSort(1, Qt::DescendingOrder);
	web_models.forms->select();
	web_models.forms->setHeaderData(0, Qt::Horizontal, tr("Host"));
	web_models.forms->setHeaderData(0, Qt::Horizontal, tr("ID"));
	web_models.forms->setHeaderData(0, Qt::Horizontal, tr("Password"));

	web_models.searches = new QSqlTableModel(0, db);
	web_models.searches->setTable("search");
	web_models.searches->setEditStrategy(QSqlTableModel::OnManualSubmit);
	web_models.searches->setSort(1, Qt::DescendingOrder);
	web_models.searches->select();
	web_models.searches->setHeaderData(0, Qt::Horizontal, tr("Name"));
	web_models.searches->setHeaderData(0, Qt::Horizontal, tr("Value"));
	web_models.searches->setHeaderData(0, Qt::Horizontal, tr("Hits"));

	web_models.signons = new QSqlTableModel(0, db);
	web_models.signons->setTable("signon");
	web_models.signons->setEditStrategy(QSqlTableModel::OnManualSubmit);
	web_models.signons->setSort(1, Qt::DescendingOrder);
	web_models.signons->select();
	web_models.signons->setHeaderData(0, Qt::Horizontal, tr("Host"));
	web_models.signons->setHeaderData(0, Qt::Horizontal, tr("ID"));
	web_models.signons->setHeaderData(0, Qt::Horizontal, tr("Password"));

	web_models.extracted_files = new QSqlTableModel(0, db);
	web_models.extracted_files->setTable("analysed_file");
	web_models.extracted_files->setEditStrategy(QSqlTableModel::OnManualSubmit);
	web_models.extracted_files->setSort(1, Qt::DescendingOrder);
	web_models.extracted_files->select();
	web_models.extracted_files->setHeaderData(0, Qt::Horizontal, tr("Fichiers"));
}

void Main_Window::clean_models() {
	ui->places_view->setModel(web_models.places);
	ui->cookies_view->setModel(web_models.cookies);
	ui->downloads_view->setModel(web_models.downloads);
	ui->forms_view->setModel(web_models.forms);
	ui->search_view->setModel(web_models.searches);
	ui->signons_view->setModel(web_models.signons);
	ui->indexed_list->setModel(model_indexed_files);
	ui->extracted_list->setModel(model_analysed_files);
}

void Main_Window::on_action_New_Analysis_triggered() {
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
		db_file += working_directory + "/analysis.db";

		try {
			db = new Database(db_file);
		} catch (const std::exception& e) {
			QErrorMessage error_msg;
			error_msg.showMessage("Cannot init the database");
			error_msg.exec();
			return;
		}

		ui->action_New_Analysis->setDisabled(true);
		ui->action_Open_Analysis->setDisabled(true);
		ui->action_Quit->setDisabled(true);

		ui->tab_results->setVisible(true);

		create_analysis_db(db_file);
	}

	ui->action_Close_Analysis->setEnabled(true);

	init_models(*db->get_db());
}

void Main_Window::on_action_Quit_triggered() {
	setAttribute(Qt::WA_DeleteOnClose);
	close();
}

void Main_Window::on_action_Open_Analysis_triggered() {
	QString buffer_file = QFileDialog::getExistingDirectory(this, "Open Analysis", "~/", QFileDialog::ShowDirsOnly);
	QString	db_file;

	if ( buffer_file.isEmpty() == true )
		return;

	QFileInfo	project_file(buffer_file);
	working_directory = project_file.absoluteDir().absolutePath();
	db_file += working_directory + "/analysis.db";

	try {
		db = new Database(db_file);
	} catch (const std::exception& e) {
		QErrorMessage error_msg;
		error_msg.showMessage("Cannot init the database");
		error_msg.exec();
		return;
	}

	qDebug() << working_directory;

	ui->action_New_Analysis->setDisabled(true);
	ui->action_Open_Analysis->setDisabled(true);

	ui->tab_results->setVisible(true);

	init_models(*db->get_db());
}

void Main_Window::on_action_Close_Analysis_triggered() {
	ui->action_Close_Analysis->setDisabled(true);
	ui->action_New_Analysis->setEnabled(true);
	ui->action_Open_Analysis->setEnabled(true);
	ui->action_Quit->setEnabled(true);

	ui->tab_results->setVisible(false);

	// TODO: save the results : analyse's parameters
	// TODO: destroy the objects
	if ( db != NULL ) {
		delete db;
		db = NULL;
	}
}

bool Main_Window::create_analysis_db(const QString& db_file) {
	return true;
}

bool Main_Window::open_analysis_db(const QString& db_file) {
	return true;
}

bool Main_Window::save_analysis_db(QSqlQuery& query) {
	return true;
}

void Main_Window::load_settings() {
	QSettings	settings;
	int		size;
	QList<QStandardItem*>	row;

	settings.beginGroup("extractor_manager");
	size = settings.beginReadArray("indexer");

	row.clear();

	for ( int i = 0 ; i < size ; i++ ) {
		settings.setArrayIndex(i);

		row << new QStandardItem(settings.value("strigi_daemon_path").toString());
		qDebug() << "domain_name :" << settings.value("domain_name").toString();

		//servers_model.insertRow(i, row);
		row.clear();
	}

	settings.endArray();
	settings.endGroup();
}

void Main_Window::save_settings() {
	QSettings settings;

	settings.beginGroup("extractor_manager");
	settings.beginWriteArray("indexer");

	/*
	   for ( int row = 0 ; row < servers_model.rowCount() ; row++ ) {
	   settings.setArrayIndex(row);

	   settings.setValue("domain_name", servers_model.item(row, 0)->text());
	   qDebug() << "domain_name : " << servers_model.item(row, 0)->text();
	   }
	 */

	settings.endArray();
	settings.endGroup();
}

void Main_Window::on_action_Preferences_triggered() {
	conf_dialog = new Configuration();

	if ( conf_dialog->exec() == QDialog::Accepted ) {
		save_settings();
	} else {
		load_settings();
	}

	delete conf_dialog;
}

void Main_Window::on_index_button_clicked() {
	ui->index_button->setDisabled(true);
	process_index();
	ui->index_button->setEnabled(true);
}


void Main_Window::on_action_Save_Analysis_triggered() {
	/*
	QString db_analysis = working_directory + "/analysis.db";
	qDebug() << db_analysis;
	{
		SQLITE_OPEN(db_analysis);

		QSqlQuery query(db);

		init_analysis_db(query);
		query.clear();
	}

	SQLITE_CLOSE(db_analysis);
	*/
}

