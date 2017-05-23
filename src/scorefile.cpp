#include <algorithm>
#include <functional>

#include "scorefile.h" 
#include <QFile>

static const char * TORUS_SCORE_FILE = "highscore";

//	Max count of Torus Top Players
const int TORUS_MAX_TOP_PLAYERS	=	10; 


//
//	scorefile functors.
//	helper functors such as Minimum score, 
//	binary_function의 템플릿 인자에는 const 를 쓰지 않는다. see Effective STL p258-259
//template <class T>
struct MinScore: public std::binary_function< ScoreData, ScoreData, bool >
//struct MinScore: public binary_function< T, T, bool >
{ 
	bool	operator()( const ScoreData & x, const ScoreData & y ) const
	//bool	operator()( const T & x, const T& y ) const
	{
		return ( x.getScore() < y.getScore() );
	} 
};


struct MaxScore: public std::binary_function< ScoreData, ScoreData, bool >
{
	bool	operator()( const ScoreData &x, const ScoreData &y ) const
	{
		return ( x.getScore() > y.getScore() );
	} 
};





TScoreFile::TScoreFile( )
:m_bDirty( false )
{
	bool is_exist = QFile::exists( TORUS_SCORE_FILE );

	if( is_exist == true )
	{
		//	파일을 읽고 난 후, 리스트 뷰에 출력을 위해서
		//	최대값 정렬이 필요하다.(이게 더 편함)
		read();		
	}
	else
	{
		initialize();
		write();		
	}
}

TScoreFile::~TScoreFile()
{
//	if( m_bDirty == true )
//	{
//		write();
//	}
	theScoreVector.clear();
}



//	TORUS_SCORE_FILE 파일이 없을 경우
//	기본 점수로 모두 채운다.
void	TScoreFile::initialize()
{
	/*
	 * 	predefined top 10 players
	 */
	theScoreVector.clear();

	push_back( "The Wizard!",		500000 );
	push_back( "Torus Guru",		400000 );
	push_back( "Torus Fanatic",		300000 );
	push_back( "Professional",		240000 );
	push_back( "Torus Master",		180000 );

	push_back( "Discipline",		140000 );
	push_back( "Apprentice",		80000 );
	push_back( "Intermediate",		40000 );
	push_back( "Beginner",			20000 );
	push_back( "Novice",			10000 );
}




void TScoreFile::push_back( const char *pName, int nScore )
{
	
	theScoreVector.push_back( ScoreData( pName, nScore )  );

	//	최소값을 삭제하기 위해
	//	erase 함수와 더불어 사용하면 편함
	sort( theScoreVector.begin(), theScoreVector.end(), MaxScore() );
	m_bDirty = true;
}


int TScoreFile::getScore( int Number )
{
	return theScoreVector[ Number ].getScore();
}

void TScoreFile::getPlayer( int aNumber, char *aPlayerPtr )
{
	strcpy( aPlayerPtr, theScoreVector[ aNumber ].getName() );
}

void TScoreFile::write()
{
	//wxFile f;
	QFile f(TORUS_SCORE_FILE);

	if( f.open(QIODevice::WriteOnly) == true )
	{
		for( int i = 0; i < TORUS_MAX_TOP_PLAYERS; i++ )
		{
			f.write( (const char *) &(theScoreVector[i]), sizeof( ScoreData ) );
		}	

		f.close();

		theScoreVector.clear();
	}
}

void TScoreFile::read()
{
	QFile f(TORUS_SCORE_FILE);
    ScoreData sd;

	if( f.open(QIODevice::ReadOnly) == true )
	{
		while( f.atEnd() != true )
		{		
			f.read( (char *) &sd, sizeof( ScoreData ) );
            push_back( sd.theName, sd.theScore );
		}

        //int bb = theScoreVector.size();
	}
    f.close();
}

int TScoreFile::getMinScore()
{	
	ScoreVector::iterator	iter = min_element( theScoreVector.begin(), 
												theScoreVector.end(), 
												//not2(MinScore<struct ScoreData>()) );
												MinScore() );
	return (*iter).getScore();
}
