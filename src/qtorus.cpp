#include <QtWidgets>

#include <qdir.h>
#include <qfileinfo.h>

#include "torusview.h"
#include "qtorus.h"
#include "scorefile.h"


#include "ui_mainwindow.h"
#include "ui_preferencedialog.h"



class TreeWidgetItem : public QTreeWidgetItem {
  public:
  TreeWidgetItem(QTreeWidget* parent):QTreeWidgetItem(parent){}
  private:
  bool operator<(const QTreeWidgetItem &other)const {
     int column = treeWidget()->sortColumn();
     if (column != 1)
         return text(column).toInt() < other.text(column).toInt();
  }
};



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

TorusView *torus_view;

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
    ui->treeWidget->setColumnWidth(0, 50);
    ui->treeWidget->setColumnWidth(1, 160);    
    ui->treeWidget->header()->setSectionResizeMode(QHeaderView::Fixed);
    ui->treeWidget->setSortingEnabled(true);
    ui->treeWidget->sortByColumn(2, Qt::DescendingOrder);
    this->readScore();    


    torus_view = new TorusView(this);
    //connect(torus_view, &TorusView::message, this->ui, MainWindow::showMessage );

    torus_view->setFocusPolicy(Qt::StrongFocus);
    this->ui->stackedWidget->insertWidget(1,torus_view);
    //setCentralWidget(torus_view);

    //torus_view->setFocus();    

	resize(TORUS_MAX_WIDTH, 700);

    //readSettings(); 
    //setCurrentFile("");    

    connect(this->ui->action_Preference,    SIGNAL(triggered()),
            this, SLOT(action_preference()));
    connect(this->ui->actionE_xit,          SIGNAL(triggered()),
            this, SLOT(action_exit()));

    connect(this->ui->actionQtorus,       SIGNAL(triggered()),
            this, SLOT(action_qtorus()));
    connect(this->ui->actionAbout_Qt,       SIGNAL(triggered()),
            this, SLOT(action_qt()));


    setWindowTitle(tr("Qtorus - by berise@gmail.com"));

    //
    initialize();

}

void MainWindow::readScore()
{
/*
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
    */

    //	top players 을 기록한 경우, 점수 입력.
    TScoreFile *m_pScoreFile	= new TScoreFile;
    delete m_pScoreFile;

    QFile f(TORUS_SCORE_FILE);
    QDataStream stream(&f);
    ScoreData sd;

    if( f.open(QIODevice::ReadOnly) == true )
    {        
        //while( f.atEnd() != true )
        for(int i = 0; i < 10; i++)
        {
            stream >> sd;

            //qDebug() << sd.theName << sd.theScore;
            TreeWidgetItem* item = new TreeWidgetItem(this->ui->treeWidget);
            item->setText(0, QString::number(i+1));
            item->setText(1, sd.theName);
            item->setText(2, QString::number(sd.theScore));
            this->ui->treeWidget->insertTopLevelItem(i, item);
        }
    }
    f.close();
}


void MainWindow::initialize()
{
    config_file = QApplication::applicationDirPath() + "/torus.ini";


    bool is_exist = QFile::exists( config_file );

    if( is_exist == true )
    {
        //	파일을 읽고 난 후, 리스트 뷰에 출력을 위해서
        //	최대값 정렬이 필요하다.(이게 더 편함)
        //read();
    }
    else
    {
        QSettings * settings = new QSettings(config_file, QSettings::IniFormat);

        settings->beginGroup("preference");
        settings->setValue("extra_torus", 0);
        settings->setValue("difficulty", 3);
        settings->setValue("starting_level", 3);
        settings->endGroup();


        const char *names[] = { "Torus Wizard", "Torus Guru", "Torus Master", "Torus Fanatic", "Torus Expert",
                                "Torus Discipline", "Apprentice", "Intermediate", "Torus Beginner", "What is Torus?" };
        const int scores[] = { 500000, 400000, 300000, 240000, 180000, 140000, 80000, 40000, 20000, 10000 };

        //settings->beginGroup("players");
        settings->beginWriteArray("players");
        for(int i = 0; i < 10; i++)
        {
            QString name = QString("%1").arg(names[i]);
            settings->setValue(name, scores[i]);
        }
        //settings->endGroup();
        settings->endArray();

        delete settings;
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
        this->ui->stackedWidget->currentWidget();
        torus_view->startGame();
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


void MainWindow::action_qtorus()
{
   QMessageBox::about(this, tr("About Qtorus"),
            tr("The <b>QTorus</b> is clearing falling toruses by making same row color.<br>"
               "Use Arrow keys to move pin.<br>Enjoy yourself.<br><br><right>berise@gmail.com</right>"));
}


void MainWindow::action_preference()
{
    QSettings * settings = new QSettings(config_file, QSettings::IniFormat);


    int extra = settings->value("preference/extra_torus").toInt();
    int difficulty = settings->value("preference/difficulty").toInt();
    int level = settings->value("preference/starting_level").toInt();



    Ui::PreferenceDialog ui_pd;

    QDialog *dialog = new QDialog;
    ui_pd.setupUi(dialog);
    ui_pd.horizontalSlider->setValue(level);
    ui_pd.checkBox_enable->setChecked(extra);
    dialog->exec();


    settings->beginGroup("preference");
    settings->setValue("extra_torus", ui_pd.checkBox_enable->isChecked());
    settings->setValue("starting_level", ui_pd.lcdNumber->value());

    if (ui_pd.radioButton->isChecked())
        settings->setValue("difficulty", 1);
    else if(ui_pd.radioButton_2->isChecked())
        settings->setValue("difficulty", 2);
    else
        settings->setValue("difficulty", 3);
    settings->endGroup();
    delete settings;

}


void MainWindow::action_exit()
{
    qApp->exit();
}

void MainWindow::action_qt()
{
    qApp->aboutQt();
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
