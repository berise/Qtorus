#include <QImage>
#include <QPainter>
#include <QKeyEvent>
#include <QApplication>
#include <QMessageBox>
#include <QtDebug>
#include <QFontMetrics>
#include <QFont>
#include <QThread>
#include <QMainWindow>
#include <QStatusBar>

#include "qtorus.h"
#include "torusinternal.h"
#include "torusview.h"
#include "scorefile.h"
#include "ui_top10dialog.h"

#include "../res/toruses.h"
#include "../res/sidebar.h"
#include "../res/pin.h"


	
// wxTimer uses TIMER_ID
const int TIMER_ID = 10000; 

//	toruses_xpm ���� ������ �䷯�� ��ġ	x
const int	nFlatTorusPosX	=	TORUS_WIDTH * (MAX_IMAGE_COUNT/2);

//	toruses_xpm ���� �䷯�� ���� ��ġ	y
#define		nFlatTorusPosY( nColor )	(nTorusColor * TORUS_WIDTH + 20)

//	level and trick
const int		TORUS_COUNT_TRICK	=	8;

//	Pipe & Pin trick level adjustment
const int		TORUS_COUNT_LEVEL	=	82;	//	9 * 9

const int		TORUS_MAX_PIPE_BLOCK	=	4;
const int		TORUS_MAX_PIN_BLOCK		=	4;
				



class Sleeper : public QThread
{
	public:
		static void sleep(unsigned long secs)
		{
			QThread::sleep(secs);
		}
		static void msleep(unsigned long msecs) 
		{
			QThread::msleep(msecs);
		}
		static void usleep(unsigned long usecs) 
		{
			QThread::usleep(usecs);
		}
};



TorusView::TorusView( QWidget *parent)
	:QWidget(parent)
{ 
//	resize(TORUS_MAX_WIDTH, TORUS_MAX_HEIGHT);

	//	���� ����� ���� TorusView�� �����͸� �ѱ��.
	m_pTI	=	new TorusInternal( this );

	m_GameState = GAME_OVER;
	setLevel( 3 );
	m_nDifficulty = 3;
	//	
	initGame();
	//updateView();

	//	!! �ѷ��� ũ�⸦ �����ؾ� �Ѵ�.
	m_pTI->setRollerHeight( m_RollerSize.height() );

	m_pTimer	=	new QTimer(this); 
	connect(m_pTimer, SIGNAL(timeout()), this, SLOT(time_tick()));
	//	

	// load images
	bitmap_toruses.loadFromData(toruses_png, sizeof(toruses_png)); 
    bitmap_sidebar.loadFromData(sidebar_png, sizeof(sidebar_png));
    bitmap_pin.loadFromData(pin_png, sizeof(pin_png));

	draw_clear_line = -1;

	//
	//	test code
	/*
	m_pTI->setPipeBlockSize( 2 );

	m_pTI->push_back( 0, 0 );
	m_pTI->push_back( 1, 1 );
	m_pTI->push_back( 2, 2 );
	*/

    main_window = reinterpret_cast<MainWindow*>(parent);
    // find main_window
    /*foreach(QWidget *widget, qApp->topLevelWidgets())
    {
        QMainWindow *mainWindow = reinterpret_cast<QMainWindow*>(widget);
        QString t = mainWindow->objectName();
        if (t == "MainWindow")
        {
            main_window = mainWindow;
            break;
        }
    }
    */
}


void TorusView::initGame()
{
	//	������ ������ �غ�
	m_nScore = 0;
	m_nLine = 0;	

	//	���� ���� ���¸� �ʱ�ȭ
	resetStep();

	//	�⺻������ ������ 3 �� �����Ѵ�.
	
	//	����� ���̾�α׸� ���� ����� ������ �����Ѵ�.
	m_pTI->init( getLevel(), getDifficulty() );
	m_ScoreSize = QSize(TORUS_WIDTH * getLevel(), 20);	 // height : 20

	updateView();
}



TorusView::~TorusView()
{
	if( m_pTimer->isActive() == true )
		m_pTimer->stop();

	zap( m_pTimer );
	zap( m_pTI );


}


void TorusView::paintEvent(QPaintEvent *) 
{
	QPainter painter( this );
	//draw( painter );
	this->drawRoller( painter );
	this->drawScore( painter );
	this->drawPipe( painter );

	//if(draw_clear_line >= 0)		drawClearLine(draw_clear_line);
	
	this->drawPin( painter );
}


void TorusView::keyPressEvent( QKeyEvent *event )
{
	int keycode = event->key();
	
	//QMessageBox information(QMessageBox::Information, "event", "key pressed");
	
	switch( keycode )
	{
		case Qt::Key_Left:
		case Qt::Key_H:
			m_pTI->movePinLeft();
			break;

		case Qt::Key_Right:
		case Qt::Key_L:
			m_pTI->movePinRight();
			break;

		case Qt::Key_Up:
		case Qt::Key_K:
			OnUp();
			break;

		case Qt::Key_Down:
		case Qt::Key_J:
			OnDown();
			break;

		//	Control key strokes
		case Qt::Key_Space: 
		{			
			switch( m_GameState )
				{					
						
					case GAME_RUN:
						pauseGame();
                        //main_window->flipWidget();

						break;

					case GAME_PAUSE:
						resume();
                        //main_window->flipWidget();

						break;

					case GAME_OVER:						
						startGame();
						break;
				}
			}
		 
		
			break;

	case Qt::Key_Escape:
        if( m_GameState == GAME_PAUSE)
		{
			stopGame();			

            main_window->flipWidget();
		}

		else if ( m_GameState == GAME_RUN )
		{
			pauseGame();			
		}

		//Refresh( FALSE );
		break;

	// Following function keys must be disabled in release build
//#ifdef __DEBUG__
	//	debug
	case Qt::Key_F3:
		m_pTI->increaseLevel();
		updateView();
		break;
	case Qt::Key_F4:
		m_pTI->decreaseLevel();
		updateView();
		break;

	case Qt::Key_F5:
		step();		
		updateView();
		break;	

        /*
	case Qt::Key_F6:
	{        
        Ui::Top10Dialog	td;

        QDialog *dialog = new QDialog;
        td.setupUi(dialog);

        if(dialog->exec() == QDialog::Accepted) // YesButton clicked }
		{
            QMessageBox box;
            box.setText( td.player_name->text() );
            box.exec();
		}
		break;
    }*/

	case Qt::Key_F9:
		OnGameOver();
		//Refresh();
		break;
//#endif

	default: 
			//event.Skip(); wx
            //event->ignore();
			break;
	}
	//	Refresh�� ���� ������ Ű�� ��� ������ ��� ���� �ð��� ���� �����.
	//	Ÿ�̸ӿ� ���� �����ϵ��� ������ �θ� �ȴ�. ������ ���� ������ ���ϴ� 
	//	�ð��̴ϱ�.
	//Refresh( FALSE );
}



/*! \brief ȭ��ǥ Ű�� ������ ���
 *   up -> pop_from_pin -> push_frount_pipe
 *   refresh view, if needed.
 *
 *
 */
void TorusView::OnUp()
{
	int aPinPos = m_pTI->getPinPos();

	if( m_pTI->isPinEmpty() != true &&
			m_pTI->isPipeFull(aPinPos) != true )
	{
		//	 �׷���.. ���� �����͸� �ø���.. push_front�� �־�� �Ѵ�.
		int nTorusColor =  m_pTI->topPin();

		//	clearPipe�� gainPoint�� ���������� �����ϱ� ������,
		//	pop�� ���� �ϰ� PipeWindow�� �־�� �Ѵ�.
		m_pTI->popPin();
		m_pTI->push_front_pipe( aPinPos, nTorusColor );

		//draw();

		//	Clear by key action
		m_pTI->clearPipe( TORUS_CLEAR_ROW_BY_PIN ); 
	}
}

//	Pin�� �� �ڸ��� �ִ� ��쿡��...
/*! \brief down -> pop_from_pipe -> push_front_pin
					 refresh view, if needed.
*/
void TorusView::OnDown()
{
	int aPinPos = m_pTI->getPinPos();

	if( m_pTI->isPinFull() != true &&
		m_pTI->isPipeEmpty( aPinPos ) != true )
	{
	  int pipe_color = m_pTI->front_pipe(aPinPos);
		m_pTI->pop_front_pipe(aPinPos); 

//	  draw();
		//	Clear by key action
		m_pTI->clearPipe( TORUS_CLEAR_ROW_BY_PIN );

		m_pTI->pushPin(pipe_color);
	}
}



void TorusView::drawPin( QPainter &painter )
{	
	QRect		client_rect;
	int i;

	client_rect = rect();

	//	�������� ũ�⵵ �־�� �� ��..
	int nBaseY	=	m_ScoreSize.height() + m_RollerSize.height() + m_PipeSize.height();
	int nBaseX	=	client_rect.width()/2 - m_PinSize.width()/2;

	//painter.save();
	//	make torus hidden
	painter.drawRect(nBaseX, nBaseY, m_PinSize.width(), m_PinSize.height() );
	painter.setClipRect(nBaseX, nBaseY, m_PinSize.width(), m_PinSize.height() );


	int nVOffset = nBaseY + m_PinSize.height();	

	int nPinPos = m_pTI->getPinPos() * TORUS_WIDTH;

#ifdef __DEBUG__
	//	Draw Grid 
	nVOffset = nBaseY + m_PinSize.height();	
	for( i = 0; i < m_PinSize.height()/10; i ++ )
	{ 
		nVOffset -= TORUS_HEIGHT; 
		painter.drawRect( nPinPos, nVOffset, TORUS_WIDTH, TORUS_HEIGHT );
	}
#endif

	painter.drawPixmap(nBaseX + ( TORUS_WIDTH/2 - bitmap_pin.width()/2 ) + nPinPos, nBaseY, bitmap_pin);

	//	���� �׸��� ���� ���� �������� ����
	nVOffset = nBaseY + m_PinSize.height();		
	
	//	���� �׸���
	for( i = m_pTI->getPinBlockSize(); i > 0; i-- )
	{
		nVOffset -= TORUS_HEIGHT;
		//dc.Blit( nBaseX + nPinPos, nVOffset, TORUS_WIDTH, TORUS_HEIGHT, &mdc, 0, 0 );
		painter.drawPixmap( nBaseX + nPinPos, nVOffset, TORUS_WIDTH, TORUS_HEIGHT, bitmap_sidebar );
	}	


	//	�ɿ� ���� �䷯���� �׸���. �׸� ��.. xy ��ǥ�踦 ����ϱ� ������ 
	//	���� ������ �����ϴ� ���� ������ ��´�. ���� nVOffset�� �̸� �����Ѵ�.
	nVOffset = nBaseY + m_PinSize.height() - TORUS_HEIGHT * m_pTI->getPinBlockSize();
	//	�ɿ� ���� �䷯���� ������ �����ͼ�
	int nTorusSize = m_pTI->getPinTorusSize();
	for( i = 0; i < nTorusSize; i ++ )
	{
		//	�Ʒ��ʺ��� �䷯���� ������ �����´�
		int nTorusColor = m_pTI->getPinTorusColor( i );		

		//	�䷯���� ���� �׸��� ���� ���� �������� ����
		nVOffset -= TORUS_HEIGHT; 

	
	  painter.drawPixmap(nBaseX + nPinPos,	//	lefttop
				nVOffset,	//		righttop
				TORUS_WIDTH,
				TORUS_HEIGHT,
				bitmap_toruses,
				nFlatTorusPosX,						//	������ �䷯�� ��ġ	x
				nFlatTorusPosY( nTorusColor ),		//	�䷯�� ���� ��ġ    y
				TORUS_WIDTH,
				TORUS_HEIGHT);
	}

	//painter.restore();
}

//	���� �������� ũ��� 80�ȼ��̴�.
void TorusView::drawPipe(QPainter &painter)
{
	QRect		client_rect;

	client_rect	= rect();

	int i;
	int nBaseY = m_RollerSize.height() + m_ScoreSize.height() - 1; // 1 �ȼ� ��ŭ ������ݾ�!! �Ʒ��� updateView�� ���ñ�.
	int nBaseX	=	client_rect.width()/2 - m_PipeSize.width()/2;
	
	
	//	�䷯���� �������� �������� ������ ���̴� ���� ����.
	//	�̸� ���ؼ�.. Roller�� ���� ���� �׸���, �� �Ŀ� Score�� �������� �׷��� �Ѵ�.

	//  �䷯���� �׷����� ���� ���� 
    painter.drawRect( nBaseX, nBaseY, m_PipeSize.width(), m_PipeSize.height() );

	/*
			|  |  |   ^
			|  |  |   |
			|  |  |   | getPipeSize()
			|  |  |   / 
		   ---------
	*/
	//	���� �������� ����
	for( int aCol = 0; aCol < m_pTI->getPipeSize(); aCol++ )
	{
		//	���� ������ �׸���		
		for( i = 0; i < m_pTI->getPipeBlockSize(); i++ )
		{						
					painter.drawPixmap(nBaseX + TORUS_WIDTH * aCol,
						nBaseY + TORUS_HEIGHT * i,
						TORUS_WIDTH , 
						TORUS_HEIGHT,
						bitmap_sidebar,
						0,
						0,
						TORUS_WIDTH , 
						TORUS_HEIGHT);

		}
		
		
		//	draw toruses from bottom to top.		

		//	aCol(umn)�� �������� ������ �ִ� �䷯���� ������ �����´�.
		int nVOffset2 = nBaseY + m_PipeSize.height() - TORUS_HEIGHT;
		int nTorus = m_pTI->getSize( aCol );
		for(  i = 0; i < nTorus; i++ )
		{
			int nTorusColor = m_pTI->getTorusColor( aCol, i );
			painter.drawPixmap(nBaseX + TORUS_WIDTH * aCol,	//	lefttop
					nVOffset2,		//		righttop --- nVOffset2���� BaseY�� ���ԵǾ���
					TORUS_WIDTH,
					TORUS_HEIGHT,
					bitmap_toruses,
					nFlatTorusPosX,						//	������ �䷯�� ��ġ	x
					nFlatTorusPosY( nTorusColor ),		//	�䷯�� ���� ��ġ    y
					TORUS_WIDTH,
					TORUS_HEIGHT);



			//	�䷯���� ���� �׸��� ���� ���� �������� ����
			nVOffset2 -= TORUS_HEIGHT; 
		}		
	}


	
	if(draw_clear_line >= 0)
	{
		int row = draw_clear_line;
	
		int ry = m_RollerSize.height() + m_ScoreSize.height() + m_PipeSize.height() - 3; // ���̶簳.. see updateView
		int rx = client_rect.width()/2 - m_PipeSize.width()/2;	
		
		painter.save();
        QPen pen(Qt::red, 2, Qt::DashLine);
		painter.setPen(pen);		
		
		painter.drawLine(	rx, ry - TORUS_HEIGHT * row - 2,
							rx + m_PipeSize.width(),ry - TORUS_HEIGHT * row - 8 );
		painter.restore();
	}
	

}



void TorusView::drawRoller(QPainter &painter)
{
	QRect client_rect;

	client_rect = rect();

	int nBaseY	=	m_ScoreSize.height();
	int nBaseX	=	client_rect.width()/2 - m_RollerSize.width()/2;


	/* clip test 
	QRect clip_rect = QRect(nBaseX, nBaseY + m_RollerSize.height()/4, m_RollerSize.width(), m_RollerSize.height()/4);;
	painter.setClipRect(clip_rect);
	*/

	//	erase background with previously set brush.
	// remove below comment if you want to see torus animations
    painter.setPen(Qt::white);
	painter.setBrush(Qt::white);
	painter.drawRect( nBaseX, nBaseY, m_RollerSize.width(), m_RollerSize.height() );
	//painter.setClipRect(nBaseX, nBaseY, m_RollerSize.width(), m_RollerSize.height());
	
	


	//wxString wxsTorusPosition = "�������� �䷯���� ��ġ��...";
	int		nPosBuffer[20];	//	 20 ���� ����� ������,... ���ݱ��� 8�� �̻��� �� ���� ����..

	for( int aCol = 0; aCol < m_pTI->getLevel(); aCol++ )
	{
		//	draw toruses from bottom to top.
		int nTorusColor = m_pTI->getTorusColor( aCol );
		int nTorusYPos	= m_pTI->getTorusPosY( aCol );
		int nImageIndex = m_pTI->getTorusImageIndex( aCol );

		painter.drawPixmap(nBaseX + TORUS_WIDTH * aCol,	//	lefttop
				nBaseY + nTorusYPos,			//		righttop
				TORUS_WIDTH,
				TORUS_WIDTH,
				bitmap_toruses,
				TORUS_WIDTH * nImageIndex,	//	image index
				nTorusColor * TORUS_WIDTH,
				TORUS_WIDTH,
				TORUS_WIDTH
				);

	
		nPosBuffer[ aCol ] = nTorusYPos;	

		//wxsTorusPosition += wxString::Format(" [%d] ", nTorusYPos);
	}

	//((wxFrame*)GetParent())->SetStatusText( wxsTorusPosition, 1);
}


void TorusView::drawScore(QPainter &painter)
{
	QRect		client_rect;
	QString	score_string;

	client_rect = rect();

	int nBaseY	=	0;
	int nBaseX	=	client_rect.width()/2 - m_ScoreSize.width()/2;

    QPen pen(Qt::blue);
	QBrush brush(Qt::white);

    painter.setPen(pen);
    painter.setBrush(brush);

    painter.drawRect( nBaseX, nBaseY, m_ScoreSize.width(), m_ScoreSize.height() );

	QFont f;
	f.setPixelSize(18);
    QFontMetrics fm(f);
	painter.setFont(f);

	score_string = QString("%1").arg(m_nScore, 8, 10,QChar('0'));
	fm.boundingRect(score_string);
//    painter.drawText(nBaseX + m_ScoreSize.width()/2 - fm.width(score_string)/2, fm.height(), score_string);


    painter.drawText( QRect(nBaseX,
                            nBaseY,
                            m_ScoreSize.width(),
                            m_ScoreSize.height()),
                      Qt::AlignCenter,
                      score_string);
    painter.setPen(QPen(Qt::white));
}


void TorusView::draw()
{
	update();
//	QPainter painter( this );	
//	draw( painter );
}


void TorusView::draw( QPainter &painter )
{
	//	������ ��ü�� ���� ȭ���� �׸���	
	//painter.setBrush(Qt::black);
	//painter.setPen(Qt::black);

	drawRoller( painter );
	drawScore( painter );
	drawPipe( painter );
	drawPin( painter );

}



//
//	gainpoint
void TorusView::gainPoint( int row, ClearMethod aClearMethod, bool isAllClear )
{
	const int BASE_POINT = 600;
	const int LEVEL_POINT = 300;
	int ALL_CLEAR_POINT = 0;
	int ROW_POINT = 0;
	int METHOD_POINT = 0;
	int current_point = 0;;
	const int nLevelModifier = m_pTI->getLevel();

	
	METHOD_POINT = BASE_POINT + ( (nLevelModifier - 2) * LEVEL_POINT);

	if( aClearMethod == TORUS_CLEAR_ROW_BY_PIN )
	{
		METHOD_POINT /= 2;		
	}

	 // row modifier
	if( row > 0 )
	{
		ROW_POINT = row * 200 * (nLevelModifier-2);
	}

	if( isAllClear == true )
	{
		//static int acPoints[5] = { 3000, 12000, 27000, 48000, 75000, ... };

		//	series of  all clear is..
		//	An = 3n^2
		ALL_CLEAR_POINT = 3000 * ( nLevelModifier  - 2 ) * ( nLevelModifier  - 2 );
		
	}


	current_point = METHOD_POINT + ROW_POINT + ALL_CLEAR_POINT;

	m_nScore += current_point;	
	m_nLine++;

	
	//	status display
	QString wxsMessage;
	QString wxsModifier;
	/*
	wxsMessage.Printf("[%#03d] %s %s = %d", 
							m_nLine, 							
							( aClearMethod == TORUS_CLEAR_ROW_BY_PIN ) ? "Push & Pull" : "Fall",
							(isAllClear == true ) ? "+ ALL CLEAR !" : "",
							current_point		//	score
							);
  */

	
	wxsMessage = QString("Cleared Line %1 : ").arg(m_nLine);
	wxsModifier = QString("%1(%2)").
				arg((aClearMethod == TORUS_CLEAR_ROW_BY_PIN ) ? "Push or Pull" : "Fall"). 
				arg(METHOD_POINT);
	wxsMessage += wxsModifier;
		
	if( row > 0 )
	{
		wxsModifier = QString(" + Row Bonus(%1)").arg(ROW_POINT);
		wxsMessage += wxsModifier;
	}

	if( isAllClear == true )
	{
		wxsModifier = QString(" + ALL CLEAR!(%1)").arg(ALL_CLEAR_POINT);
		wxsMessage += wxsModifier;
	}

	wxsModifier = QString(" = %1").arg(current_point);
	wxsMessage += wxsModifier;

    // MainWindow::CentralWidget::stackedWidget::page[1-N]
    QMainWindow *main_window = reinterpret_cast<QMainWindow*>(parentWidget()->parentWidget()->parentWidget());
    //QString t = main_window->objectName();
    QStatusBar *sb = main_window->statusBar();
    sb->showMessage(wxsMessage);


	//	draw line
	draw_clear_line = row;
	//m_pTimer->stop();
	QTimer::singleShot(100, this, SLOT(invalidateClearLine()));
	
	/*
	// pause timer for about 99 ms.	
	drawClearLine( row );
//	m_pTimer->stop(); 
  Sleeper::msleep(520);	//	sleep for 99 ms to show cleared line	
//	m_pTimer->start();	//	restart with previous timer value 
*/
	
	//	TRICK�� ������ų��...	
	if( ( m_nLine % TORUS_COUNT_TRICK ) == 0 )
	{
		step();		//	���� �Լ�
	} 
}

void TorusView::gainPoint( int row, ClearMethod aClearMethod)
{
	const int BASE_POINT = 600;
	const int LEVEL_POINT = 300;
	int ALL_CLEAR_POINT = 0;
	int ROW_POINT = 0;
	int METHOD_POINT = 0;
	int current_point = 0;;
	const int nLevelModifier = m_pTI->getLevel();


	m_nLine++;
	METHOD_POINT = BASE_POINT + ( (nLevelModifier - 2) * LEVEL_POINT);


	//	status display
	QString wxsMessage;
	QString wxsModifier;


	wxsMessage = QString("Cleared Line %1 : ").arg(m_nLine);

	if( aClearMethod == TORUS_CLEAR_ROW_BY_PIN )
	{
		METHOD_POINT /= 2;		
	}
	wxsModifier = QString("%1(%2)").
				arg((aClearMethod == TORUS_CLEAR_ROW_BY_PIN ) ? "Push or Pull" : "Fall"). 
				arg(METHOD_POINT);
	wxsMessage += wxsModifier;

	 // row modifier
	if( row > 0 )
	{
		ROW_POINT = row * 200 * (nLevelModifier-2);
		wxsModifier = QString(" + Row Bonus(%1)").arg(ROW_POINT);
		wxsMessage += wxsModifier;
	}

	if( m_pTI->isAllClear() == true )
	{
		//static int acPoints[5] = { 3000, 12000, 27000, 48000, 75000, ... };

		//	series of  all clear is..
		//	An = 3n^2
		ALL_CLEAR_POINT = 3000 * ( nLevelModifier  - 2 ) * ( nLevelModifier  - 2 );

		wxsModifier = QString(" + ALL CLEAR!(%1)").arg(ALL_CLEAR_POINT);
		wxsMessage += wxsModifier;
		
	}


	current_point = METHOD_POINT + ROW_POINT + ALL_CLEAR_POINT;
	m_nScore += current_point;

	wxsModifier = QString(" = %1").arg(current_point);
	wxsMessage += wxsModifier;	
	
	/*
	wxsMessage.Printf("[%#03d] %s %s = %d", 
							m_nLine, 							
							( aClearMethod == TORUS_CLEAR_ROW_BY_PIN ) ? "Push & Pull" : "Fall",
							(isAllClear == true ) ? "+ ALL CLEAR !" : "",
							current_point		//	score
							);
  */

	//QApplication::activeWidget();

    emit message(wxsMessage);
    /*
	QMainWindow *main_window = reinterpret_cast<QMainWindow*>(parentWidget());
	main_window->statusBar()->showMessage(wxsMessage);
    */


	//	draw line
	draw_clear_line = row;
	//m_pTimer->stop();
	QTimer::singleShot(100, this, SLOT(invalidateClearLine()));
	
	/*
	// pause timer for about 99 ms.	
	drawClearLine( row );
//	m_pTimer->stop(); 
  Sleeper::msleep(520);	//	sleep for 99 ms to show cleared line	
//	m_pTimer->start();	//	restart with previous timer value 
*/
	
	//	TRICK�� ������ų��...	
	if( ( m_nLine % TORUS_COUNT_TRICK ) == 0 )
	{
		step();		//	���� �Լ�
	} 
}


/*! let drawPipe() stop to draw clear line for give milliseconds
 *
 */
void TorusView::invalidateClearLine()
{
	draw_clear_line = -1;
//	m_pTimer->start();
}

void TorusView::drawClearLine( int row )
{
	QPainter painter(this);
	
	QRect client_rect = rect();

//	int i;
	int nBaseY = m_RollerSize.height() + m_ScoreSize.height() + m_PipeSize.height() - 3; // ���̶簳.. see updateView
	int nBaseX	=	client_rect.width()/2 - m_PipeSize.width()/2;
	
	
	//wxPen		pen( *wxWHITE, 2, wxLONG_DASH );
	QPen pen(Qt::red, 2, Qt::DashLine);
	painter.setPen(pen);

	painter.drawLine(	nBaseX, 
										nBaseY - TORUS_HEIGHT * row - 2, 
										nBaseX + m_PipeSize.width(),
										nBaseY - TORUS_HEIGHT * row - 8 );
}


/*! 
    \brief Callback function for timer.
	\return NONE
*/
void TorusView::timerEvent(QTimerEvent *event )
{
//	m_pTI->moveTorus();
	int retcode = m_pTI->moveTorus();
	if( retcode < 0 )
	{
		repaint();
	}
	else
	{
		OnGameOver();		
	}	
} 


//	torus state transition diagram
//	      space           space
//	STOP -------->  RUN  -------> PAUSE
//	STOP <--------  RUN  <------- PAUSE
//			   ESC          space or ESC
/*
void	TorusView::autoGameState()
{
	if( m_GameState == STOP )
}
*/

void	TorusView::startGame()
{	
	initGame(); 

	m_pTimer->start(30); 

	m_GameState	=	GAME_RUN;

/*
	MyFrame *f = (MyFrame*)wxGetApp().GetTopWindow();

	wxString message; 
	message.Printf("Start game!");
	f->SetStatusText( message ); 
	*/
}

void	TorusView::stopGame()
{
	m_pTimer->stop(); 
	m_GameState	=	GAME_OVER;
}

//	implement timer only
void	TorusView::resume()
{
		if( m_GameState == GAME_PAUSE)
		{
				m_pTimer->start();
				m_GameState	=	GAME_RUN;
		}
}

//	implement timer only
void	TorusView::pauseGame()
{
	//if( m_pTimer->IsRunning() == TRUE )
	if( m_GameState == GAME_RUN )
	{
		m_pTimer->stop();
		m_GameState	=	GAME_PAUSE;
	}

}



void TorusView::setLevel( int nLevel )
{
	m_nLevel = nLevel;
}

int TorusView::getLevel()
{
	return m_nLevel;
}


void TorusView::OnGameOver()
{
	stopGame();

	//	���� ������ ���ÿ� ������ ������ ����.
	//m_Level = getLevel();

	
	//	top players �� ����� ���, ���� �Է�.	
	m_pScoreFile	= new TScoreFile;

	//	TScoreFile�� sort predicate�� MinScore�� �����ؾ� �Ѵ�.
	if( m_nScore > m_pScoreFile->getMinScore() )
	{	
        Ui::Top10Dialog	td;

        QDialog *dialog = new QDialog;
        td.setupUi(dialog);

        if(dialog->exec() == QDialog::Accepted) // YesButton clicked }
		{
			//wxMessageBox( td.m_wxsName ); 
            m_pScoreFile->push_back( td.player_name->text().toUtf8(), m_nScore );
		}
		else
		{
			m_pScoreFile->push_back( "Noname", m_nScore );
		}
		m_pScoreFile->write();
	} 
	delete m_pScoreFile;
	

	// 
	/*
	MyFrame *f = (MyFrame*)wxGetApp().GetTopWindow();
	f->SwitchPanel();

	wxString message; 
	message.Printf("Thank you for playing wxTorus. Copyright (c) JaeSung Lee");
	f->SetStatusText( message );
	*/
}



void TorusView::step()
{
		m_PipeBlockSize++;

		//	���� �ְ� ������ �������� ��.
		if( m_PipeBlockSize % TORUS_MAX_PIPE_BLOCK == 0 )
		{
			m_PinBlockSize++;
			m_PipeBlockSize = 1;

			//	���� �ְ� ������ �������� ��.
			if( m_PinBlockSize % TORUS_MAX_PIN_BLOCK == 0 )
			{
					m_pTI->increaseLevel();

					//	�並 �����ؾ�.. �ϴµ�.. 
					//	 �� �Լ��� step�ȿ� ���� �ִ� ���� ���� ������ ���ϱ�?
					//	�ϴ� �־� �α�� �ϰ�.. ���߿� ������.
					updateView();
					//	OnLevelUp();
					resetStep();

					return ;

			}
		}


		//	BlockSize�� ��ȭ�� ���� ���� ȣ��.
		m_pTI->setPinBlockSize( m_PinBlockSize );
		m_pTI->setPipeBlockSize( m_PipeBlockSize ); 
}

void TorusView::resetStep()
{
	m_PinBlockSize		= 1;
	m_PipeBlockSize		= 1;

	m_pTI->setPinBlockSize( m_PinBlockSize );
	m_pTI->setPipeBlockSize( m_PipeBlockSize ); 

}


void	TorusView::setDifficulty( int nDifficulty )
{
	m_nDifficulty = nDifficulty;
}

int		TorusView::getDifficulty()
{
	return	m_nDifficulty;
}



//	������ ���� �� ���� ȣ���ؼ� ������ ũ�⸦ �����Ѵ�..
void TorusView::updateView()
{
	//	���� ������ ���� �����Ѵ�.
	int nLevel	=	m_pTI->getLevel();

	//	score
	m_ScoreSize = QSize(	TORUS_WIDTH * nLevel, 20 );	 // height : 20

	//	width :  , height : nLevel * 10 + 50 = 80
	m_PipeSize = QSize(		TORUS_WIDTH * nLevel, 
						nLevel*TORUS_HEIGHT + 50 );	//	3, 7 torus & 1 block guide = 80

	//	���� �⺻ ũ��	
	//	+1�� pipe�� �ɰ��� �Ÿ��� ���ؼ��̴�.
	m_PinSize = QSize(		TORUS_WIDTH * nLevel, 
						nLevel*TORUS_HEIGHT + 20 + 1 );	//	���� 5 ���� �䷯�� + ���������� �⺻ �Ÿ� 1 �ȼ� = 51

	m_RollerSize = QSize(	TORUS_WIDTH * nLevel,						
						TORUS_MAX_HEIGHT - ( m_PinSize.height() + m_PipeSize.height() + m_ScoreSize.height() ) );

	//	�ѷ��� ũ�⸦ �����ؾ�...
	m_pTI->setRollerHeight( m_RollerSize.height() );

    // let Qt update view
    update();
}


void TorusView::time_tick()
{	
	int retcode = m_pTI->moveTorus();
	if( retcode < 0 )
	{
		repaint();
	}
	else
	{
		OnGameOver();		
	}	
}

// vim:ts=2:sw=2
