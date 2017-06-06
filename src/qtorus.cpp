#include <QtWidgets>

#include <qdir.h>
#include <qfileinfo.h>

#include "torusview.h"
#include "qtorus.h"
#include "scorefile.h"


#include "ui_mainwindow.h"
#include "ui_preferencedialog.h"
#include "ui_top10dialog.h"



// Top 10 player items
class TreeWidgetItem : public QTreeWidgetItem {
public:
    TreeWidgetItem(QTreeWidget* parent):QTreeWidgetItem(parent){}
private:
    bool operator<(const QTreeWidgetItem &other)const {
        int column = treeWidget()->sortColumn();
        if (column != 0)
            return text(column).toInt() < other.text(column).toInt();
        else
            return text(column) < other.text(column);
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
    //header->setText(0, "Rank");
    header->setText(0, "Name");
    header->setText(1, "Score");

    ui->treeWidget->setHeaderItem(header);
    ui->treeWidget->setColumnWidth(0, 250);
    //ui->treeWidget->setColumnWidth(1, 300);
    ui->treeWidget->header()->setSectionResizeMode(QHeaderView::Stretch);
    ui->treeWidget->setSortingEnabled(true);
    ui->treeWidget->sortByColumn(1, Qt::DescendingOrder);


    torus_view = new TorusView(this);
    //connect(torus_view, &TorusView::message, this->ui, MainWindow::showMessage );

    torus_view->setFocusPolicy(Qt::StrongFocus);
    this->ui->stackedWidget->insertWidget(1,torus_view);
    //setCentralWidget(torus_view);

    //torus_view->setFocus();    

	resize(TORUS_MAX_WIDTH, 700);

    //readSettings(); 
    //setCurrentFile("");    

    connect(this->ui->action_Preference, SIGNAL(triggered()), this, SLOT(action_preference()));
    connect(this->ui->actionE_xit,       SIGNAL(triggered()), this, SLOT(action_exit()));
    connect(this->ui->actionQtorus,      SIGNAL(triggered()), this, SLOT(action_qtorus()));
    connect(this->ui->actionAbout_Qt,    SIGNAL(triggered()), this, SLOT(action_qt()));


    setWindowTitle(tr("Qtorus - by berise@gmail.com"));

    //
    initialize();

}

void MainWindow::readScore()
{
    this->ui->treeWidget->clear();


    QSettings * settings = new QSettings(config_file, QSettings::IniFormat);
    int size = settings->beginReadArray("players");
    QStringList keys = settings->allKeys();

    for(int i = 0; i < keys.size()-1; i++)
    {
        //settings->setArrayIndex(i);

        int value = settings->value(keys.at(i)).toInt();
        //qDebug() << value <<  keys.at(i);

        TreeWidgetItem* item = new TreeWidgetItem(this->ui->treeWidget);
        //item->setText(0, QString::number(i+1));
        item->setText(0, keys.at(i));
        item->setText(1, QString::number(value));
        this->ui->treeWidget->insertTopLevelItem(i, item);

    }
    settings->endArray();
    delete settings;
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


        const char *names[] = { "Wizard", "Guru", "Master(Near CTS)", "Fanatic", "Expert",
                                "Discipline", "Apprentice", "Intermediate", "Beginner", "What is Torus?" };
        const int scores[] = { 500000, 400000, 300000, 240000, 180000,
                               140000, 80000, 40000, 20000, 10000 };

        //settings->beginGroup("players");
        settings->beginWriteArray("players");
        for(int i = 0; i < 10; i++)
        {
            //settings->setArrayIndex(i);
            QString name = QString("%1").arg(names[i]);
            settings->setValue(name, scores[i]);
        }
        //settings->endGroup();
        settings->endArray();

        delete settings;
    }


    //
    this->readScore();

}


// torusview->on_gameover(score);
// read score and compare with score then get your name if within top 10.
void MainWindow::on_gameover(int score)
{
    QSettings * settings = new QSettings(config_file, QSettings::IniFormat);
    int size = settings->beginReadArray("players");
    QStringList keys = settings->allKeys();

    // sorted map ordered by key(score)
    QMap<int, QString> map;

    int scores[10];

    for(int i = 0; i < keys.size()-1; i++)
    {
        int value = settings->value(keys.at(i)).toInt();
        map.insert(settings->value(keys.at(i)).toInt(), keys.at(i));
    }

    int rrank = 10;
    QString vk; // value_as_a_key
    QMap<int, QString>::iterator i = map.begin();
    for( ; i != map.end(); i++)
    {
        int t = i.key();
        if( score > t)
        {
            vk = i.value();
            rrank--;
        }
        else
        {
            break;
        }

    }


    if (rrank < 10)
    {
        Ui::Top10Dialog	td;
        QDialog *dialog = new QDialog;
        td.setupUi(dialog);

        if(dialog->exec() == QDialog::Accepted) // YesButton clicked
        {
            settings->remove(vk);   // remove previous value
            settings->setValue(td.player_name->text().toUtf8(), score);
        }
    }

    settings->endArray();
    delete settings;

    //
    this->readScore();
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
    QString keyPress;

    if (event->key() == Qt::Key_Space)
    {        
        this->flipWidget();        
        torus_view->startGame();
    }

    keyPress.sprintf("Key code : %d", event->key());
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
