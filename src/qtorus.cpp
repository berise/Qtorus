#include <QtWidgets>

#include <qdir.h>
#include <qfileinfo.h>

#include "torusview.h"
#include "qtorus.h"


#include "ui_mainwindow.h"
#include "ui_preferencedialog.h"

/*
 * QString executableDirPath( QApplication & aQApp )
{
	QFileInfo fi( aQApp.argv()[aQApp.argc()-1] );
	QString eDir; 
	if( fi.isSymLink() == true )
	{
		QString aFile;
		// convert symbolic link to abs file path
		if( fi.isRelative() == true )
		{
			// resolve relative symbolic link
			aFile = fi.readLink();
		}
		else
		{
			//	resolve absolute symbolic link
			aFile = fi.dir().absolutePath() + QString("/") +  fi.readLink();
		}

		QFileInfo fi2( aFile );

		eDir = fi2.dir().absolutePath();
//		qDebug( "resolve symbolic link to %s", (const char *)eDir );
	}
	else
	{
		eDir = fi.dir().absolutePath();
//		qDebug( "normal file path %s", (const char *)eDir );
	}


	return eDir;
}
*/

/****************************************************************************
**
** Copyright (C) 2004-2007 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://trolltech.com/products/qt/licenses/licensing/opensource/
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://trolltech.com/products/qt/licenses/licensing/licensingoverview
** or contact the sales department at sales@trolltech.com.
**
** In addition, as a special exception, Trolltech gives you certain
** additional rights. These rights are described in the Trolltech GPL
** Exception version 1.0, which can be found at
** http://www.trolltech.com/products/qt/gplexception/ and in the file
** GPL_EXCEPTION.txt in this package.
**
** In addition, as a special exception, Trolltech, as the sole copyright
** holder for Qt Designer, grants users of the Qt/Eclipse Integration
** plug-in the right for the Qt/Eclipse Integration to link to
** functionality provided by Qt Designer and its related libraries.
**
** Trolltech reserves all rights not expressly granted herein.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/


MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent),
    ui(new Ui_MainWindow)
{

    this->ui->setupUi(this);


    QTreeWidgetItem* header = new QTreeWidgetItem;
    header->setText(0, "Rank");
    header->setText(1, "Name");
    header->setText(2, "Score");
    ui->treeWidget->setHeaderItem(header);
    this->tPopulateTree();

    /*
    17.    QTreeWidgetItem* row1 = new QTreeWidgetItem(treewidget);
    18.    row1->setText(0, "col 1");
    19.    row1->setText(1, "col 2");
    20.    row1->setText(2, "col 3");
    */


    TorusView *torus_view = new TorusView(this);
    //connect(torus_view, &TorusView::message, this->ui, MainWindow::showMessage );

    torus_view->setFocusPolicy(Qt::StrongFocus);
    this->ui->stackedWidget->insertWidget(1,torus_view);
    //setCentralWidget(torus_view);

    //torus_view->setFocus();

    /*
	QWidget *widget = focusWidget();

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    */

	resize(TORUS_MAX_WIDTH, 700);

    //readSettings(); 
    //setCurrentFile("");

    setWindowTitle(tr("Qtorus - by berise"));

    connect(this->ui->action_Preference, SIGNAL(triggered()), this, SLOT(action_preference()));

}

void MainWindow::tPopulateTree()
{
    const char *names[] = { "Torus Wizard", "Torus Guru", "Torus Master", "Torus Fanatic", "Torus Expert",
                          "Torus Discipline", "Torus Apprentice", "Torus Novice", "Torus Beginner", "What is Torus?" };
    const char *scores[] = {"1", "2", "3", "4", "5","6", "7", "8", "9", "10"};



    for(int i = 0; i < 10; i++)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem;
        item->setText(0, scores[i]);
        item->setText(1, names[i]);
        item->setText(2, scores[i]);
        this->ui->treeWidget->insertTopLevelItem(i, item);
    }


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *)
{
}


void MainWindow::keyPressEvent(QKeyEvent *event)
{
    Q_UNUSED(event);

    QString keyPress;
    if (event->key() == Qt::Key_Space)
    {
        keyPress.sprintf("Qt::Key_Space(%d) pressed", event->key());
        this->flipWidget();
        //this->ui->stackedWidget->currentWidget();
        //torus_view->setFocus();
    }

    this->ui->statusbar->showMessage(keyPress);

    QWidget::keyPressEvent(event);
}


void MainWindow::flipWidget()
{
    if (this->ui->stackedWidget->currentIndex() != 0)
    {
        this->ui->stackedWidget->setCurrentIndex(0);
        this->ui->stackedWidget->currentWidget()->setFocus();
    }
    else
    {
        this->ui->stackedWidget->setCurrentIndex(1);
        this->ui->stackedWidget->currentWidget()->setFocus();
    }
}


QStatusBar *MainWindow::statusBar()
{
    return this->ui->statusbar;
}

void MainWindow::newFile()
{    
}

void MainWindow::open()
{    
}

bool MainWindow::save()
{

	return true;
}

bool MainWindow::saveAs()
{

	return true;
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("About QTorus"),
            tr("The <b>QTorus</b> is clearing falling toruses by making same row color."));
}

void MainWindow::documentWasModified()
{
    //setWindowModified(textEdit->document()->isModified());
}

void MainWindow::createActions()
{
    newAct = new QAction(QIcon("./images/new.png"), tr("&New"), this);
    newAct->setShortcut(QKeySequence(tr("Ctrl+N")));

    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    openAct = new QAction(QIcon("./images/open.png"), tr("&Open..."), this);
    openAct->setShortcut(QKeySequence(tr("Ctrl+O")));
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    saveAct = new QAction(QIcon("./images/save.png"), tr("&Save"), this);
    saveAct->setShortcut(QKeySequence(tr("Ctrl+S")));
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(tr("Save &As..."), this);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcut(QKeySequence(tr("Ctrl+Q")));
    exitAct->setStatusTip(tr("Exit the QTorus"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the QTorus's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);    

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Press space to start"));
}


void MainWindow::action_preference()
{
    Ui::PreferenceDialog ui_pd;
    QDialog *dialog = new QDialog;
    ui_pd.setupUi(dialog);
    dialog->show();
}


// main //
int main( int argc, char **argv )
{
    Q_INIT_RESOURCE(application);

    QApplication app( argc, argv ); 

    MainWindow mainWin;	
    mainWin.show();
    return app.exec(); //      run app instance
}                            


// vim:tw=78:fo=tcq2:isk=!-~,^*,^\|,^\":ts=8:ft=help:norl:
