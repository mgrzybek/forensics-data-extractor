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

#include <QFileDialog>
#include <QMainWindow>
#include <QErrorMessage>
#include <QFileSystemModel>
#include <QSortFilterProxyModel>

#include "common.h"
#include "extractors/firefox_extractor.h"
#include "extractors/chrome_extractor.h"
#include "indexing_engine.h"

namespace Ui {
	class Main_Window;
}

class Main_Window : public QMainWindow
{
	Q_OBJECT

	public:
		explicit	Main_Window(void* z_context, QWidget *parent = 0);
		~Main_Window();

		zmq::context_t*		zmq_context;
	private slots:
		void	on_browse_button_clicked();
		void	on_scan_button_clicked();

		void	update_info();
		void	launch_extractors();

	private:
		/*
		 * GUI Stuff
		 */
		Ui::Main_Window	*ui;

		/*
		 * Models
		 */
		web_browser_models	web_models;

		// scanned files
		QStandardItemModel	model_indexed_files;
		// extracted files
		QSortFilterProxyModel*	sorted_model_extracted_files;

		/*
		 * Processing classes
		 */
		Indexing_Engine*	search_engine;
		Firefox_Extractor*	firefox_engine;
		Chrome_Extractor*	chrome_engine;

		void	process_scan();
		void	clean_models();
};

#endif // MAIN_WINDOW_H
