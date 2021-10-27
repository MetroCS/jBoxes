#include <stdio.h>
#include "mystring.h"
#include "constants.h"
#include "box.h"
#include "utility.h"
#include "chargridtype.h"

  chargridtype::chargridtype( int numrows, int numcols )
  {
    nr = numrows;  nc = numcols;  

    // build nr by nc grid of spaces, with enterkeys for display

    grid = new mystring();

    int sym = ' ';  // stub---fill grid with all syms in order

    int k, j;
		
		for(k=1;k<=nr;k++)
    {// build row k
      for(j=1;j<=nc;j++)
      {
        grid->insertCharAt( map(k,j), sym );
				/* for putting all printables in the initial console
        sym++;
        if( sym > '~' )
          sym = ' ';
				*/
      }
      grid->insertCharAt( map(k,nc+1), enterkey );
    }

    showcursor = 1;
    row = 1; col = 1;

    buffer = new mystring("");

  }

  // utility that gives the index in string of (k,j)
  // position in simulated 2D array of chars
  int chargridtype::map( int k, int j )
  {
    return (nc+1)*(k-1) + j;
  }

  void chargridtype::clear()
  {
    int k, j;
    for(k=1;k<=nr;k++)
      for(j=1;j<=nc;j++)
        grid->replaceCharAt( map(k,j), ' ' );
    row = 1; col = 1;
  }

  float chargridtype::howWide()
  { return nc * maxcharwidth + 2*hpad; }
  float chargridtype::howHigh()
  { return nr * baseline + 2*vpad; }

  int chargridtype::moveCursor( int r, int c )
  {
    if( 1 <= r && r <= nr &&
        1 <= c && c <= nc )
    {
      row = r;
      col = c;
      return 1;
    }
    else
      return 0;
  }

  void chargridtype::moveBack()
  {
    if( col > 1 )
      col--;
    else
    {// on first column
      if( row > 1 )
      {
        row--;
				// jb18:  this seems silly:
        //   col = 1;
				//   instead:
				col = nc;
      }
      // else in (1,1), do nothing
    }
  
  }

  void chargridtype::display( float x, float y )
  {
      grid->gridDisplay( x+hpad, y-vpad, showcursor, map(row,col) );
  }

  void chargridtype::print( mystring* incoming )
  {
    grid->print( incoming, map(row,col), row, col );
  }

  // return single scaling so that the full console, scaled to
  //  that, fits snugly into w by h view area
  float chargridtype::consoleScale( float w, float h )
  {
    float xscale, yscale;

    // adjust to allow for fact that some of w and h
    // are used for the boundary of the viewer
    float conwidth = (nc+1) * maxcharwidth;
    float conheight = (nr+1) * baseline;

    xscale = w / conwidth;
    yscale = h / conheight;

    return mymin( xscale, yscale );

  }

  int chargridtype::whatRow()
	{  return row; }

	int chargridtype::whatCol()
	{  return col; }

	void chargridtype::hideCursor()
	{  showcursor = 0; }
	void chargridtype::showCursor()
	{  showcursor = 1; }
