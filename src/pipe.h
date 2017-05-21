#if !defined( PIPE_H )
#define PIPE_H

/*
 *	some deque implementation got a warning line.
 *	for example, linux 6.x series gets a warning line 
 *	while comparing signed with unsigned.
 */
#if defined(__BORLANDC__)
#pragma hdrstop
#endif


#include "type.h"
#include "global.h"

#include <deque>
typedef std::deque<int> TorusDeque;


/*
 *	A TPipe manages each single pipe line in the TPipeSet
 *
 *	It assumes that top of the pipe is a front and bottom is a back.
 *
 *	Any changes( push, pop ) in the pipe will emit a checkRow signal
 *	which connected to the TPipeSet.
 */
class TPipe
{	

	public:
		TPipe( );
		virtual ~TPipe();

	public:
		int getLevel();
		void setLevel( int nLevel );
		int m_Level;
		/*
		 *	push Torus into the TPipe
		 *
		 *	This function emits checkRow() signal
		 */
		virtual void push_front( int aTorusColor );
		virtual void push_back( int aTorusColor );
	
		/*
		 *	pop torus from the pipe.
		 *
		 *	This function emits checkRow() signal
		 */
		virtual void pop_front();
		virtual void pop_back();


		//	See what is at the edge of the pipe.
		int front() const;
		int back() const;

		/*
		 *	get color in Row_th in the pipe
		 *	For checking routine in pipeset
		 */
		int getTorusColor( const unsigned int Row );
		

		//	clear row[th] torus in the pipe
		void clearRow( const int Row );

		void clearPipe( ClearMethod aClearMethod );

		void isAllClear();

		//	clear toruses left in the pin.
		void clear();
		void initialize();


		bool isEmpty();
		bool isFull();


		//	hardeness.
		void setBlockSize( int nBlockSize );
		int getBlockSize();
		
		//	get deque size
		unsigned int getSize();

		void debug();


		//	2003. 3.24
		//void drawTorus( QPainter &aPainterRef, int aTorusColor, int yPos );
		/*

	signals:
		void checkRow( ClearMethod );


	public slots:
		void slotHitTime() ;

	protected:
		void resizeEvent( QResizeEvent * );
		void paintEvent( QPaintEvent * );

  */

	
	protected:
		TorusDeque	* m_TorusDequePtr; 
		TorusDeque::size_type		m_MaxSize;
	

		int				m_MaxPipe; 
		int				m_BlockSize;	//	 ������ ���ʿ� ��ġ�� �� (difficulty)

		/*
		 * 	HitTimer shows a hit line(matched torus line.
		 * 	It allows use to confirm which line matched. (configurable)
		 */
		//QTimer			m_HitTimer;
};


#endif
