/**
 * Project: forensics-data-extractor
 * File name: main_window.h
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

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QDir>
#include <QtSql>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QFileDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>

#include "common.h"
#include "database.h"
#include "configuration.h"
#include "extractors/firefox_extractor.h"
#include "extractors/chrome_extractor.h"
#include "indexing_engine.h"
#include "parsing_engine.h"
#include "configuration.h"

namespace Ui {
	class Main_Window;
}

class Main_Window : public QMainWindow
{
	Q_OBJECT

	public:
		explicit	Main_Window(void* z_context, QWidget *parent = 0);
		~Main_Window();

		zmq::context_t*	zmq_context;

	private slots:
		void	on_browse_button_clicked();
		void	on_scan_button_clicked();

		void	update_info();
		void	launch_extractors();

		void	on_action_New_Analysis_triggered();
		void	on_action_Quit_triggered();
		void	on_action_Open_Analysis_triggered();
		void	on_action_Close_Analysis_triggered();
		void	on_action_Preferences_triggered();

		void	on_index_button_clicked();

		void	on_action_Save_Analysis_triggered();

	private:
		/*
		 * GUI Stuff
		 */
		Ui::Main_Window*	ui;
		Configuration*		conf_dialog;

		/*
		 * Models
		 */
		Database*			db;
		web_browser_models	web_models;

		// scanned files
		QSqlTableModel*		model_indexed_files;
		// extracted files
		QSqlTableModel*		model_analysed_files;

		/*
		 * Working files
		 */
		QString		working_directory;
		QString		strigi_daemon_path;

		/*
		 * Processing classes
		 */
		Parsing_Engine*		search_engine;
		Firefox_Extractor*	firefox_engine;
		Chrome_Extractor*	chrome_engine;

		Indexing_Engine*	index_engine;

		bool	scan_in_progress;
		bool	index_in_progress;

		void	process_scan();
		void	process_index();
		void	init_models(QSqlDatabase& db);
		void	clean_models();

		bool	init_analysis_db(QSqlQuery& query);
		bool	create_analysis_db(const QString& db_file);
		bool	open_analysis_db(const QString& db_file);
		bool	save_analysis_db(QSqlQuery& query);

		void	load_settings();
		void	save_settings();
};

#endif // MAIN_WINDOW_H
