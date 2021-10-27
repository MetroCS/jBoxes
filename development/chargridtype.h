// interface for chargridtype class

const int chargridrows = 25;
const int chargridcols = 40;

class chargridtype
{
public:

  chargridtype( int numrows, int numcols );
  int map( int k, int j );
  void clear();
  float howWide();
  float howHigh();
  int moveCursor( int r, int c );
	int whatRow();
	int whatCol();
	void hideCursor();
	void showCursor();
  void moveBack();
  void display( float x, float y );
  void print( mystring* incoming );
  float consoleScale( float w, float h );

private:
  int nr;  // number of usable rows
  int nc;  // number of usable columns (have extra for enters)
  int row, col;  // location of cursor
  int showcursor;  // whether or not to show the cursor rectangle
  mystring* grid;   // build grid of nr*nc chars here
  mystring* buffer;  // use for user input buffer
  float width, height;  // effectively constants
};
