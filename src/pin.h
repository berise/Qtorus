#if !defined( PIN_H )
#define PIN_H

#include <vector> 
using namespace std; 

#include "type.h"



//	2002.3.24
//	TTorus형태의 Pin은 더 이상 사용하지 않는다.
//	대신 최초로 디자인한 int로 되돌린다..
//	
//	젠장.. 코드를 변경한다는 건.. 정말 지랄 같은 일이다.
//	바꾼걸 또 원상태로 바꾼다니, 시간 낭비구만... 쩝.. 지난날의 과오여~~
//DECLARE_VECTOR( TTorus, Pin );
//DECLARE_VECTOR( int, TorusVector ); 
typedef std::vector<int> TorusVector;

//
//	Torus Pin object
//	Takes a part of stackable pin
//DECLARE_CLASS( TPin );


//	"Chu", "Tori"
/*
 *	The TPin class manages stack like object.
 *	In the torus game, it is called "Chu"
 */
class TPin
{
	public:
		TPin();
		virtual ~TPin();

	public:
		
		int getTorusColor( int nRow );
		

		/*
		 *	pin operations.
		 *	
		 *	push Torus object into the TPin.
		 *	pops the Torus on the top of the TPin.
		 *	top the Torus on the top of the TPin.
		 */		

		virtual void push( int aTorusColor );
		virtual void pop();
		int top();

		/*
		 *	Is pin full or empty.
		 */
		bool isFull();
		bool isEmpty();

		/*
		 *	game difficulties 
		 *	This function makes a TPin short. 
		 *	1 <=  PinSize <= 4
		 */ 
		int getSize() const;
		void setSize( const int PinSize );

		//	block size.
		int		getBlockSize();
		void	setBlockSize( int nBlockSize );		//	 trick, 


		/*
		 *	Torus movement.
		 */
		virtual void moveLeft();
		virtual void moveRight();


		/*
		 *	Where the pin positioned?
		 */
		int getPinPos() const;

		/*
		 * clear toruses left in the pin.
		 * usually happen when the game is over. 
		 * Or sometimes by the new forces.
		 */
		void clear();
		void init( int nLevel );		//	initialize에서 init으로 변경 2002.6.xx


		/*
		 *	Level varies pin bottom size
		 *	restrict a total number of torus in the pin.
		 */
		void setLevel( const int Level = 3 );
		int getLevel() const;


		//	핀이 가질 수 있는 토러스의 최대 개수
		void setCapacity( const int Level = 3 );
		int getCapacity() const;

		/*
		 * 	same api as pipeset & rollerset
		 */
		void	append();
		void	remove();



		//	
		//	
		void debug();

		
		//	3.24	commented out to convert to win32
		//void drawTorus( QPainter &aPainterRef, int aTorusColor, int xPos, int yPos );

		/*

	protected:
		void resizeEvent ( QResizeEvent * );
		void paintEvent( QPaintEvent * );
		*/


	protected:
		/*
		 * 	flicker free update
		 */
		//QPixmap	theScreen;		

		TorusVector 	*m_TorusVectorPtr; 


		/*
		 * available pin size
		 * m_Level + [level]
		 */
		unsigned int	m_nCapacity;		
		unsigned int	m_BlockSize;	//	guard size.

		unsigned int 	m_PinPos;
		unsigned int	m_Level;
};

#endif
