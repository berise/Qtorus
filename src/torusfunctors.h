#include <algorithm>	//	min_element
#include <functional>

#include "scorefile.h"
#include "roller.h"
#include "pipe.h"

//	객체를 지우는 
template<class T>
struct delete_object : public std::unary_function<const T*, void>
{
	void operator()( const T* ptr ) const
	{
		delete ptr;
	}
};

struct  set_level
{
	set_level( int nLevel ) : theLevel( nLevel ) {}

	void operator()( TPipe * aPipePtr )
	{
		aPipePtr->setLevel( theLevel );
	}


	int theLevel;
};



//	minimum retrieve minimum pipe size(# of torus)
struct minimum : public std::binary_function< TPipe *, TPipe *,  bool >
{
	bool operator() ( TPipe	*aPipePtr1, TPipe	*aPipePtr2  )
	{
		return ( aPipePtr1->getSize() < aPipePtr2->getSize() );
	}
};

struct maximum :  public std::binary_function< TPipe *, TPipe *,  bool >
{
	bool operator() ( TPipe	*aPipePtr1, TPipe	*aPipePtr2  )
	{
		return ( aPipePtr1->getSize() > aPipePtr2->getSize() );
	}
};




/*
//	같은 색상인가?
struct  equal_color: std::unary_function< TPipe *, bool >
{
		equal_color( int Row, int Color ) : theRow( Row ), theColor( Color ) {}

		bool operator()( TPipe * aPipePtr )
		{
			int aTorusColor = aPipePtr->getTorusColor( theRow );

			//	use != in order to avoid int to bool warning
			return	( aTorusColor == theColor );// != 0 ;
		}

		int	theRow, theColor;

};

*/

//	clear_row functor call TPipePtr->clearRow
//	주어진 행을 삭제하는 객체 
struct  clear_row
{
		clear_row( int Row ) : theRow( Row ) {}

		void operator()( TPipe * aPipePtr )
		{
//			assert( aPipePtr != NULL );

			aPipePtr->clearRow( theRow );
		}

		int theRow;
};


struct  is_empty: std::unary_function< TPipe *, bool >
{
	bool operator()( TPipe * aPipePtr )
	{
		return	( aPipePtr->isEmpty() == false );
	}
};


struct  set_block_size
{
	set_block_size( int nBlockSize ) : theBlockSize( nBlockSize ) {}

	void operator()( TPipe * aPipePtr )
	{
		aPipePtr->setBlockSize( theBlockSize );
	}


	int theBlockSize;
};



struct  clear_pipe: public std::unary_function<TPipe *, bool>
{
	bool operator()( TPipe * ptr )
	{
		ptr->clear();

		return true;
	}
};

// end of pipew functors.




struct equal_score: public std::binary_function< const ScoreData &, const ScoreData &, bool >
{	
	bool	operator()( const ScoreData & x, int nScore) const
	{
		return ( x.getScore() == nScore );
	} 
};

/*
struct ScoreDisplay: public std::unary_function< const ScoreData &, bool >
{
	bool	operator()( const ScoreData & x ) const
	{
		cerr << x.getName() << '\t' << x.getScore() << endl;
		return true;
	} 
};
*/


// end of scorefile functors


//	rollw functors
//	각각의 토러스를 움직이는 함수 객체
struct  move_toruses: public std::unary_function< TRoller *, void >
{
	move_toruses( int nLevel ) : m_nLevel( nLevel ) {}
	void operator()( TRoller * aRollerPtr )
	{
		aRollerPtr->roll( m_nLevel );
	}

	int m_nLevel;
};


//	게임을 새로 시작할 때
//	기존의 토러스를 없앰.(다시 감으면 됨)
struct  wrap_torus: public std::unary_function< TRoller *, void >
{
	void operator()( TRoller * aRollerPtr ) 
	{
		aRollerPtr->wrapTorus();
	}
};


//	토러스가 바닥을 쳤는지...에 대한 검사. hit test
/*
struct  hit_bottom: public std::unary_function< TRoller *, void >
{
	hit_bottom( const PipeWindow *aPipeWindowPtr ) : m_PipeWindowPtr( aPipeWindowPtr )
	{
	}


	void operator()( TRoller * aRollerPtr ) const
	{
		if( aRollerPtr->getTorusYPoint() > m_RollerHeight )
		{
			m_PipeWindowPtr->push_back( 
		}
	}
private:
	PipeWindow *m_PipeWindowPtr;
	int			m_RollerHeight;
};
*/


//	TRoller의 높이를 맞추는 함수 객체
//	6.18, 윈도의 크기 혹은 레벨이 변경된 경우 내부의 크기를 다시 설정해야 함.
struct  set_roller_height: public std::unary_function< TRoller *, void >
{
	set_roller_height( int nHeight ): m_nHeight( nHeight )
	{
	}

	void operator()( TRoller * aRollerPtr )
	{
		aRollerPtr->setHeight( m_nHeight );
	}

	int m_nHeight;
};
