#ifndef __Torus_internal_included__
#define __Torus_internal_included__


#include "global.h"	//	ClearMethod
#include "type.h"

#include <vector>

using namespace std;

class TPin;
class TPipe;
class TRoller; 
class TorusView;



//	Pipe components
//__CORE_DECLARE_CLASS( TPipe );
//DECLARE_VECTOR( TPipePtr, TPipeVector );
typedef std::vector<TPipe *> TPipeVector;


//__CORE_DECLARE_CLASS( TRoller );
//DECLARE_VECTOR( TRollerPtr, TRollerVector );
typedef std::vector<TRoller *> TRollerVector;

class TorusInternal
{
public:
	TorusInternal( TorusView *pView );
	~TorusInternal();


	//	Pipe Function Collections
public:


	void pop_front_pipe( int aCol );
	void pop_back( int aCol );
	void push_front_pipe( int aCol, int aTorusColor );
	void push_back( int aCol, int aTorusColor );



	//	Pipe related function
public:
//	void redraw();
//	void redraw( const VDC &vDCCref );
	void init( int nLevel, int nDifficulty );
	

	//	���� ������ �����´�
	int getLevel();
	void setLevel( int nLevel );

	void	setDifficulty( int nDifficulty );
	int		getDifficulty();

	//	 ������ �ʱ�ȭ 
	void initPipe( int nLevel );
	void destroyPipe();

	//	���� ����� �����ϴ� ���� ����
	void setPipeBlockSize( int nBlockSize );
	int getPipeBlockSize();

	//	setLevel�� Init���� ���������, ���� �� �ִ� ������ �����Ѵ�.
	void setPipeLevel( int nLevel );

	//	�������� ����°�?
	bool isPipeEmpty();		
	bool isPipeEmpty( int aCol );

	//	����ã�°�?
	bool isPipeFull( int aCol );
	



	//	 clear functions.
	void clearRow( int nRow );
	void clearPipe( ClearMethod );
	bool isAllClear();



	//	�䷯���� ���� ���� �������� �䷯���� ����
	int getMaximumPipeSize();	
	int getMinimumPipeSize();

	//	ù��° �䷯�� ������ �����´�.
	int front_pipe( int aCol );
	int back_pipe( int aCol );

	//	���� �������� ����
	int getPipeSize();
	int getSize( int aCol );


	//	���� ���� �Լ�
	void insertPipe();
	void removePipe();


	//	void setTorusWindow( TorusWindowPtr aTorusWindowPtr );	

	//
	// Pin functions
	void initPin( int nLevel );
	void setPinBlockSize( int nBlockSize );
	int		getPinBlockSize();

	void setPinLevel( int nLevel );

	void setPinPos( int nPinPos );
	int getPinPos();
	

	bool isPinFull();
	bool isPinEmpty();
	void popPin();
	void pushPin( int aTorusColor );
	int topPin();
	
	//	virtual functions.	
	void movePinRight();
	void movePinLeft();

	//	�ɿ� �ִ� �䷯���� ����
	int getPinTorusSize();
	int getPinTorusColor( int nTh );



	//
	// Roller functions
	//	see comments in pipew.cpp
	void initRoll( int nLevel );
	void destroyRoll();


	
	//	Pipe �Լ�
	int getTorusColor(int aColumn, int nTh);

	//	Roller �Լ�
	int getTorusColor( int nColumn );
	int getTorusPosY( int nColumn );
	int getTorusImageIndex( int nColumn );


	int moveTorus();

	//	���� ������ Roller �� ������ ����
	void removeRoller();
	//	���ϰ� 
	void insertRoller();

	//	�信�� �ѷ��� ũ�⸦ �����ؾ� �Ѵ�. ������ ������ �� ���� ũ�� ������ �Ӥ������ϴ�
	void	setRollerHeight( int nHeight );
	int		getRollerHeight();

	// ���� �Լ�
	void	increaseLevel();
	void	decreaseLevel();



private:
	//	pref
	int			m_bExtraTorus;
	int			m_nDifficulty;
	int			m_nLevel;



	//	������ ���� �޶����� Roller �� ����
	//	View�� ���õ� �����̱� ������.. Internal�� �ִ�.
	int			m_nRollerHeight;

	TPin		*m_pTPin;
	TPipeVector *m_pPV;
	TRollerVector *	m_pRV;

	//	View Ŭ���� ������
	TorusView	*m_pTorusView;
};

#endif
