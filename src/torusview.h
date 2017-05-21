#ifndef __TORUSVIEW__INCLUDED__
#define __TORUSVIEW__INCLUDED__


#include <qwidget.h>
#include <qpixmap.h>
#include <qtimer.h>

#include "global.h"

class	TorusInternal;
class	TorusThread;
class	TScoreFile; 
class ScorePanel;
class MainWindow;


//	�䷯���� ���� ���� ��Ȳ�� �˷� �ش�.
//	������ ��Ȳ�� ���� ������ �ٲ� �ָ� �ȴ�.
enum GameState { GAME_RUN, GAME_PAUSE, GAME_OVER };		



class TorusView: public QWidget
{
	Q_OBJECT   // need to use timer
public:
	TorusView(QWidget *parent);
	virtual ~TorusView();

private:
	bool bSwitch;

public:	
	
	void resetStep();
	void step();

	//	���� ����... torusinternal���� ȣ��ȴ�.
	void gainPoint( int nLevel, ClearMethod aClearMethod, bool isAllClear );
	void gainPoint( int nLevel, ClearMethod aClearMethod);
	
	//	������ ������ ���� ������ ũ�� ����
	void updateView();

	//	���� ����  �Լ�
	void	startGame();
	void	stopGame();
	void	pauseGame();
	void	resume();

	void initGame();

	//	pref. dlg. �� ������ ������ ����
	void	setLevel( int nLevel );
	int		getLevel();

	void	setDifficulty( int nDiff );
	int		getDifficulty();

	void	OnLevelUp();		
	void OnGameOver();


	void	onThreadTimer( );

	void drawPipe();	//	torusinternal���� ȣ��(clear line ȿ��)

	// called by torus_internal
	void draw();
	void draw( QPainter & painter);

private:
	void drawPin( QPainter &painter );
	
	void drawPipe( QPainter &painter );
	void drawRoller( QPainter &painter );
	void drawScore( QPainter &painter );

	void drawClearLine( int row );	

protected:

	//	U/D Key Process
	void OnDown();
	void OnUp();

	//	key event
	virtual void paintEvent( QPaintEvent *event );
	virtual void keyPressEvent( QKeyEvent *event );
	//virtual void resizeEvent( QResizeEvent *event ); 
	//	timer event
	virtual void timerEvent( QTimerEvent * event );
	// protected slot: void time_tick();
	
	int draw_clear_line;


signals:
    void message(QString message);

protected slots:
	void time_tick();

	void invalidateClearLine();
private:
	//ScorePanel *score_panel;
    MainWindow *main_window;

	/// bitmap from xml resources
	QPixmap	bitmap_toruses;
	QPixmap bitmap_pin;
	QPixmap bitmap_sidebar;
	QPixmap bitmap_logo;



	//	�÷��̾� ǥ��
	QString m_wxsPlayerEntry[10];

	//	�䷯���� ���� ǥ��
	TorusInternal	*m_pTI;
	
	//	�ð�.. �ȶȶȶ�..	//	Ȥ�� �����带 ������...
	QTimer			*m_pTimer;


	//	game status
	GameState		m_GameState;
	int			m_nLevel;		//	������ ����
	int			m_nDifficulty;
	//	File object
	//	������ ���Ϸ� ����ϱ� ���� Ŭ����
	TScoreFile		*m_pScoreFile;

	
	//	game control variables
	int m_PipeBlockSize;
	int m_PinBlockSize;


	int			m_nScore;
	int			m_nLine;

	QSize		m_PipeSize;
	QSize		m_PinSize;
	QSize		m_RollerSize;
	QSize		m_ScoreSize;
};


#endif
