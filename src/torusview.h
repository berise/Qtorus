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


//	토러스의 게임 진행 상황을 알려 준다.
//	게임의 상황에 따라 적절히 바꿔 주면 된다.
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

	//	점수 계산용... torusinternal에서 호출된다.
	void gainPoint( int nLevel, ClearMethod aClearMethod, bool isAllClear );
	void gainPoint( int nLevel, ClearMethod aClearMethod);
	
	//	레벨의 증감에 따른 윈도우 크기 변경
	void updateView();

	//	게임 조절  함수
	void	startGame();
	void	stopGame();
	void	pauseGame();
	void	resume();

	void initGame();

	//	pref. dlg. 에 설정된 레벨을 전달
	void	setLevel( int nLevel );
	int		getLevel();

	void	setDifficulty( int nDiff );
	int		getDifficulty();

	void	OnLevelUp();		
	void OnGameOver();


	void	onThreadTimer( );

	void drawPipe();	//	torusinternal에서 호출(clear line 효과)

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



	//	플레이어 표시
	QString m_wxsPlayerEntry[10];

	//	토러스의 내적 표현
	TorusInternal	*m_pTI;
	
	//	시간.. 똑똑똑똑..	//	혹은 쓰레드를 쓰던지...
	QTimer			*m_pTimer;


	//	game status
	GameState		m_GameState;
	int			m_nLevel;		//	설정된 레벨
	int			m_nDifficulty;
	//	File object
	//	점수로 파일로 기록하기 위한 클래스
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
