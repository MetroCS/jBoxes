// interface for mymystring class
class mystring 
{
  public:

  mystring();
  mystring( char* p );
  ~mystring();
  void destroy();
  mystring* copy();
  char* charCopy();
  char charAt( int index );
  void replaceCharAt( int index, char x );
  void insertCharAt( int index, char x );
  void deleteCharAt( int index );
  void append( mystring* other );
  void appendChar( char x );
  int length();
  int whereCursor();
  void setCursor( int j );
  void advanceCursor();
  void retreatCursor();
  int equals( mystring* x );
  int equals( char* x );
	int frontEquals( char* x );
  void getInfoAt( int symbol0Choice, int index, int newlines,
                     float& offsetx, float& offsety,
                     int& row, int& col );
  void findNearest( int symbol0Choice,
	                  float offsetx, float offsety, int newlines,
                       int& index, float& actualx, float& actualy,
                       int& row, int& col );
  void upDownCursor( int symbol0Choice,
	                 char dirn, int newlines );
  void homeEndCursor( int symbol0Choice,
	                   char dirn, int newlines );
  void howBig( int newlines,
                float& howwide, float& howhigh );
  void coreHowBig( int newlines, 
	               int symbol0Choice,
                float& howwide, float& howhigh );
  void howBigPortValue( float& howwide, float& howhigh );
  void drawcursorbackground( float x, float y,
						  float w, float h, float restorecolor[] );
  void display(  int x, int y,
                 int newlines,
                int showfocus, float color[] );
  void coreDisplay(  int x, int y,
	              int symbol0Choice,
                 int newlines,
                int showfocus, float color[] );
  void portDisplay(  int x, int y,
                     int showendoffile, float color[] );
  void gridDisplay(  int x, int y,
                 int showcursor, int index );
  void print( mystring* incoming, int index, int& row, int& col );
  int getLineLength();
  void scroll();
  int compare( mystring* other );
  int toInt( long& result );
  int toFloat( double& result );

	int numberUsed();
  int toFile( FILE* handle );
  int toFileArrayAdjusted( FILE* handle );
	int toFileStringForm( FILE* handle );

  // jb22
  int matches( int& pos, char* w );

  // jb25:
  mystring* typeset( float w, float& h );

private:
  char *s;  // holds the current strip of chars
//  char font;  // '0' = mono, '1' = non-mono

  int cursor;  // index of cursor (1 is first spot)
  int numused;  // # of chars currently used
  int numallocated; // # of chars allocated 
};
