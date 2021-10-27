#include <iostream>

// implementation of mystring class

// note that resource allocation checking must be done by
// manager, string just assumes that there is room on the heap
// to do its things
// go ahead and make string capable of multi-line---simply
// make some branches---will slow it
// down a little, but should be fine, and will save worrying
// about string/page distinction, maybe
//  Certainly will save code discrepancy ---

// manager will need to disallow enter key (and space, too, for 
// that matter) in single line strings

// string don't really care how it displays, so any display
// related things must pass in info telling what character
//  choices to make, then string properly accounts for them
//   --- makes display match howBig, for example

// choices are:  single line string --- always non-mono font
//               multi-line non-mono --- for documentation part
//               multi-line mono --- for console grid part

// note that every box has a "value" part, which is a string,
//  in which we can encode its actual string value, as in
//   a data box, or nothing, as for a sequence box, or
//   as the console display grid, as for the unique console i/o box

// only one font now!

#include <glut/glut.h> // Mac OS X
//#include <gl/glut.h> 
//#include "glut.h"

#include <string.h>
#include <stdio.h>

#include "mystring.h"
#include "constants.h"

#include "box.h"
#include "utility.h"

long charsused = 0; // global for this class

// internal constants
const int chunksize = 16; // for efficiency, grab more than one
                         // when need one more --- 
const int maxstringsize = 10000;

// notes: 
//   cursor is not changed by insert, delete guys
//   from outside, first char in a string is in position 1, not 0 
//
//   string must be able to display itself, figure out how wide
//   and high it is.  

  // default constructor tidies up at start:
  mystring::mystring()
  {
    numused = 0;
    numallocated = 0;
    cursor = 0;  // header, not a real char  
//    font = '1';  // non-mono is preferred font 
  }

  // purely for convenience, build string that is a copy of a
  //  given "string"
  mystring::mystring( char* p )
  {
    int k;

    numused = strlen(p);

    int numchunks = numused/chunksize + 1;
    s = new char[ numchunks * chunksize ];
    numallocated = numchunks * chunksize;
      charsused += numallocated;

    for(k=0;k<numused;k++)
    {
      s[k] = p[k];
    }

//    font = '1';

    cursor = 0;
  }

  // destructor
  mystring::~mystring()
  {
    if(numallocated > 0)
    {
      errorexit("destructor is deleting mystring???");
    }
  }

  // should manually, carefully remove resources used by
  //  a string that is being deleted
  void mystring::destroy()
  {
    if (numallocated > 0)
    {
      delete s;
      charsused -= numallocated;
      numallocated = 0;
      numused = 0;
      cursor = 0;
      s = NULL;
    }
  }

  // make a copy of me and return it
  mystring* mystring::copy()
  {
    mystring* newstr = new mystring();

    int k;

//    newstr->font = font;

    newstr->cursor = cursor;
    newstr->numused = numused;
    newstr->numallocated = numallocated;

    newstr->s = new char[ numallocated ];
      charsused += numallocated;

    for(k=0;k<numused;k++)
    {
      newstr->s[k] = s[k];
    }

    return newstr;

  }// mystring::copy

  // allocate an array of char large enough to hold my
  // data, copy it over, return pointer to the array
  char* mystring::charCopy()
  {
    char* p = new char[ numused+1 ];
    int k;
    for( k=1; k<=numused; k++ )
      p[k-1] = s[k-1];
    p[numused] = '\0';
    return p;
  }

  char mystring::charAt( int index )
  {
    return s[index-1];
  }

  void mystring::replaceCharAt( int index, char x )
  {
    index--;
    if(index<0 || index>numused+1)
      errorexit("huh? overwriting outside feasible bounds???");

    s[index] = x;

  }

  void mystring::insertCharAt( int index, char x )
  {
    index--;  // to switch from outside view to inside

    char *temp;
    int k;

    if(numallocated >= maxstringsize+chunksize)
    {
      errorexit("about to add when already have huge mystring???");
    }

    // note:  8/13 changed numused+1 below to numused---
    //           numused+1 was clearly wrong, forgot to
    //           adjust it when did the index-- bit
    if(index<0 || index>numused)
      errorexit("huh? adding outside feasible bounds???");

    if(numallocated==numused)
    {// must add one spot, so add a chunk
        temp = new char[numallocated + chunksize];
               charsused += chunksize;  // keep global count

      // shift 'em over, leaving index spot ready for x
      for(k=numused; k>index; k-- )
        temp[k] = s[k-1];
      for(k=index-1; k>=0; k-- )
        temp[k] = s[k];

      if (numallocated > 0) // if not first chunk created
        delete s;
      s = temp;
      numallocated += chunksize;
    }
    else
    {// have space to shift in place
      for(k=numused; k>index; k-- )
        s[k] = s[k-1];
    }

    s[index] = x;
    numused++;

  }

  void mystring::deleteCharAt( int index )
  {
    index--;  // to switch to inside view of indexing

    char *temp;
    int k;

    if(index<0 || index>numused)
      errorexit("huh?  deleted non-existent char?");

    if(numallocated-numused>=chunksize-1)
    {// build new char strip, copy into it, leaving guy behind
      temp = new char[numallocated-chunksize];
            charsused -= chunksize;
      
      for(k=0; k<index; k++ )
        temp[k] = s[k];
      for(k=index; k<numused-1; k++ )
        temp[k] = s[k+1];

      if( numallocated > 0)
        delete s;
      s = temp;
      numallocated -= chunksize;
    }
    else
    {// shift in current strip
      for(k=index+1; k<numused; k++ )
        s[k-1] = s[k];
    }

    numused--;

  }

  // tack other onto end of me (high-level---use more basic
	//   methods rather than doing from scratch)
  void mystring::append( mystring* other )
	{
    int k, j;
		for( k=1, j=length()+1; k<=other->length(); k++, j++ )
		  insertCharAt( j, other->charAt( k ) );
	}

  // convenience guy to stick a single char on end
  void mystring::appendChar( char x )
  {
    insertCharAt( length()+1, x );
  }

  int mystring::length()
  {
    return numused;
  }

  int mystring::whereCursor()
  {
    return cursor;
  }

  void mystring::setCursor( int j )
  {
    if(j<0 || j>numused)
      errorexit("tried to setcursor out of bounds???");
    cursor = j;
  }
  
  void mystring::advanceCursor()
  {   
    if(cursor>=numused)
      errorexit("oops, moved cursor off far end???");
    cursor++;
  }

  void mystring::retreatCursor()
  {
    if(cursor<=0)
      errorexit("oops, moved cursor off front end???");
    cursor--;
  }

  // return 1 if s is the same as me, as a sequence of
  // symbols, 0 otherwise
  int mystring::equals( mystring* x )
  {
    if( numused != x->numused )
      return 0;  // different lengths!
    else
    {// same length, try for it
      int k=0;
      while( k < numused && s[k] == x->s[k] )
        k++;

      if( k==numused )
        return 1;
      else
        return 0;
    }
  }

  // compare me to a "string"
  int mystring::equals( char* x )
  {
    if( numused != strlen(x) )
      return 0;  // different lengths!
    else
    {// same length, try for it
      int k=0;
      while( k < numused && s[k] == x[k] )
        k++;

      if( k==numused )
        return 1;
      else
        return 0;
    }
  }

  // compare initial part of me to a standard string
  int mystring::frontEquals( char* x )
	{
	  if( numused < strlen(x) )
		  return 0;  // I'm too short to match
		else
		{// I'm at least as long, try it
      int k=0;
			for( k=0; k < strlen(x); k++ )
			  if( s[k] != x[k] )
				  return 0;
			return 1;
		}
	}

  // scan through s, keeping track of geometric info,
  // until hit index, and return the info:
  //       offset to ulc of the char at s[index-1],
  //       line number and number of characters over in the line
  //       for that character
  void mystring::getInfoAt( int symbol0Choice,
		                  int index, int newlines,
                     float& offsetx, float& offsety,
                     int& row, int& col )
  {
    if(index<0 || numused<index)
      errorexit("illegal index in getInfoAt");
    else
    {
      int k, linecount, charcount;
      float leftmargin;
      float chx, chy;  // current character offsets
      float prevchx, prevchy;
      int prevcharcount, prevlinecount;

      // initial width is symbol0 width   
			if( symbol0Choice == 1 )
        leftmargin = widthsym[ littlepagecode ];
			else if( symbol0Choice == 2 )
				leftmargin = widthsym[ absimagecode ];
			else if( symbol0Choice == 3 )
			  leftmargin = widthsym[ boxedjavacode ];
			else
				leftmargin = widthsym[ trianglecode ];
 
      chy = 0;  // on first line

      linecount = 1;  

      chx = leftmargin;

        // jb25:  have bug with index==0
        // jb26:  seems wrong:        prevchx = chx;
        //       (was not framing symbol0 in a mystring correctly)
             prevchx = 0;

      charcount = 0;

      // scan until hit index, maintaining info
      for(k=1; k<=index; k++)
      {// size char #k = s[k-1] and update 
    
        // move to right across current character
            // but first, note value before move across current guy
            prevchx = chx;

        chx += widthsym[ unsignchar(s[k-1]) ];
        charcount++;
      
        // note old info in case need it
        prevchy = chy;
        prevcharcount = charcount;
        prevlinecount = linecount;

        // handle enterkey, if is one
        if( s[k-1]==enterkey && newlines)
        {// start new line
        // update width info

          chx = leftmargin; // move to start of next line
          linecount++; // note that we are on next line
          charcount = 0;  // haven't scanned any on this line

          chy -= baseline;  // move down standard distance

        } // handled enterkey

      } // loop to scan chars

      // back up to get chx at ulc of target char
      chx = prevchx;
      // restore others only if off end
            // was sloppy before, doing s[index-1] with index==0
      if(index > 0 && s[index-1]==enterkey && newlines)
      {
        charcount = prevcharcount;
        linecount = prevlinecount;
        chy = prevchy;
      }

      // send out the info
      offsetx = chx;
      offsety = chy;
      row = linecount;
      col = charcount;

    }// legal index

  }// end of getInfoAt

  // scan all chars in the mystring and return geo info
  // for the one that is closest to the given offset
  void mystring::findNearest( int symbol0Choice,
		                   float offsetx, float offsety, int newlines,
                       int& index, float& actualx, float& actualy,
                       int& row, int& col )
  {

      int k, linecount, charcount;
      float leftmargin;
      float chx, chy;  // current character offsets
      float prevchx, prevchy;
      int prevcharcount, prevlinecount;

      // guys to hold current best values as scan
      int bestindex, bestrow, bestcol;
      float bestx, besty, bestcenterx, bestcentery;
      float centerx, centery;

      // initial width is symbol0 width   
			if( symbol0Choice == 1 )
        leftmargin = widthsym[ littlepagecode ];
			else if( symbol0Choice == 2 )
				leftmargin = widthsym[ absimagecode ];
			else if( symbol0Choice == 3 )
			  leftmargin = widthsym[ boxedjavacode ];
			else
				leftmargin = widthsym[ trianglecode ];
 
      chy = 0;  // on first line
      linecount = 1;  

      chx = leftmargin;
      charcount = 0;

      bestindex = 0;  // maybe the triangle is actually closest!
      bestrow = 1; bestcol = 0;
      bestx = 0; besty = 0; // ulc of current best
      bestcenterx = leftmargin / 2;  // center point of current best
      bestcentery = - baseline / 2; 

      centerx = bestcenterx; 
      centery = bestcentery;

      // scan until hit index, maintaining info
      for(k=1; k<=numused; k++)
      {// size char #k = s[k-1] and update 
    
        // move to right across current character
            // but first, note value before move across current guy
            prevchx = chx;

          chx += widthsym[ unsignchar(s[k-1]) ];
          charcount++;
      
        // note old info in case need it
        prevchy = chy;
        prevcharcount = charcount;
        prevlinecount = linecount;

        // decide right here, using the "previous" info,
        // if current beats the best:
            // must compare center points
            centerx = prevchx + widthsym[ unsignchar(s[k-1]) ]/2;
            centery = prevchy - baseline/2;

        if( 
            ( myabs( offsety - bestcentery ) >
              myabs( offsety - centery )        
            )
             ||
            (  
              (myabs( offsety - bestcentery ) ==
               myabs( offsety - centery) 
              )       
               &&
              (myabs( offsetx - bestcenterx )   >
               myabs( offsetx - centerx)      
              )
            )
          )
            
        {
          bestindex = k;
          bestx = prevchx;
          besty = prevchy;
          bestrow = prevlinecount;
          bestcol = prevcharcount;
          bestcenterx = centerx;
          bestcentery = centery;
        }

        // handle enterkey, if is one
        if( s[k-1]==enterkey && newlines)
        {// start new line
        // update width info

          chx = leftmargin; // move to start of next line
          linecount++; // note that we are on next line
          charcount = 0;  // haven't scanned any on this line

          chy -= baseline;  // move down standard distance

        } // handled enterkey

      } // loop to scan chars

      // send out the info, adjusted for scaling

      index = bestindex;
      actualx = bestx; 
      actualy = besty;
      row = bestrow;
      col = bestcol;

  }// findNearest

  // find actual offset of the cursor,
  //  move up/down a level
  void mystring::upDownCursor( int symbol0Choice,
		                           char dirn, int newlines )
  {
    float offsetx, offsety, newx, newy;
    int row, col, index;

    // find offset of cursor right now
    getInfoAt( symbol0Choice, cursor, newlines,
                 offsetx, offsety, row, col );
    // change offsety to one line above center of cursor char:
    if(dirn=='u')
      offsety += baseline/2;
    else
      offsety -= baseline*1.5;

    // put offsetx smack on center of cursor symbol
    if(cursor > 0 )
    {
      offsetx += widthsym[ unsignchar(s[cursor-1]) ] * 0.5;
    }
    else
      offsetx = 0;  // doesn't need to be in middle to find left end

    // find best corresponding index
    findNearest( symbol0Choice, 
			             offsetx, offsety, newlines,
                   index, newx, newy, row, col );
    cursor = index;    
  }

 // move cursor to beginning/end of current line
  // by geometrical approach
 void mystring::homeEndCursor( int symbol0Choice,
	                             char dirn, int newlines )
  {
    float offsetx, offsety, newx, newy;
    int row, col, index;
    float howwide, howhigh;

    // find offset of cursor right now
    getInfoAt( symbol0Choice, 
			           cursor, newlines,
                 offsetx, offsety, row, col );

    // change offsetx to front or end of current line,
       // first, need to find out how wide the box is
       coreHowBig( newlines, symbol0Choice, howwide, howhigh );

    if(dirn=='h')
      offsetx = 0;
    else
      offsetx = howwide;

    // put offsety in center of current line
    offsety -= baseline*0.5;

    // find best corresponding index
    findNearest( symbol0Choice, 
			             offsetx, offsety, newlines,
                   index, newx, newy, row, col );
    cursor = index;    
  }

  // 9/28/2000:  small change to allow doc, abs to use non-triangle
  void mystring::howBig( int newlines,
                float& howwide, float& howhigh )
  {
		coreHowBig( newlines, 0, howwide, howhigh );
	}

  void mystring::coreHowBig( int newlines,
	                int symbol0Choice,
                float& howwide, float& howhigh )
  {
    int k, linecount;
    float overallwidth, currentlinewidth, leftmargin;

    // initial width is symbol0 width   
    if( symbol0Choice == 1 )
      overallwidth = widthsym[ littlepagecode ];
		else if( symbol0Choice == 2 )
			overallwidth = widthsym[ absimagecode ];
		else if( symbol0Choice == 3 )
		  overallwidth = widthsym[ boxedjavacode ];
		else // all non-specials (like 0) use triangle
      overallwidth = widthsym[ trianglecode ];

    leftmargin = overallwidth;

    currentlinewidth = overallwidth;
    linecount = 1;  

    for(k=1; k<=numused; k++)
    {// size char #k = s[k-1] and update 

      currentlinewidth += widthsym[ unsignchar(s[k-1]) ];
      
      // handle enterkey, if is one
      if( s[k-1]==enterkey && newlines)
      {// start new line
        // update width info
        if(currentlinewidth > overallwidth)
          overallwidth = currentlinewidth;

        currentlinewidth = leftmargin;

        linecount++; // note that we have another line
      }

    }

    // let last line impact the overall width, since might not
    //  end with an enterkey
    if(currentlinewidth > overallwidth)
      overallwidth = currentlinewidth;

    howwide = overallwidth;
    howhigh = linecount * baseline;
    
  } // end of coreHowBig

  // figure width and height of portbox value mystring ---
	//  uses monospacing 
  void mystring::howBigPortValue( float& howwide, float& howhigh )
  {
    howwide = widthsym[ trianglecode ] + numused * maxcharwidth;
    howhigh = baseline;   
  } // end of howBigPortValue

// little utility --- called in two places---just draws background
//  rectangle to highlight the cursor
void mystring::drawcursorbackground( float x, float y,
						  float w, float h, float restorecolor[] )
{
  glColor3fv( focuscolor );
  // draw rectangle behind the symbol
  glBegin(GL_POLYGON);
    glVertex3f( x, y, textfocusz );
  	glVertex3f( x+w, y, textfocusz );
	  glVertex3f( x+w, y+h, textfocusz );
	  glVertex3f( x, y+h, textfocusz );
  glEnd();
  glColor3fv( restorecolor );
}

  // display self with reference point at (x,y),
  // which is where the ulc should be,
  //  with details depending on the other parameters

  // new version:  uses new version of drawsymbol that takes
  //                 lower left corner (x,y) parameters
  //  This is the one place where we pass that rather than ulc,
  //  is worth it to easily stay on the baseline of the string

  // 9/28/2000:  old display becomes coreDisplay, modified to
  //             take symbol0Choice parameter, make new
  //             display that simply calls coreDisplay:
  void mystring::display( int x, int y,
                int newlines,
                int showfocus, float color[] )
  {
		coreDisplay( x, y, 0, newlines, showfocus, color );
	}
  
	// symbol0Choice can be 0 for the basic triangle,
	//                      1 for the little page for doc
	//                      2 for the abstract image
  void mystring::coreDisplay( int x, int y,
	              int symbol0Choice, 
                 int newlines,
                int showfocus, float color[] )
  {
    int k, linecount;
    float leftmargin;

    if( !texdraw )
    {// only bother to set the color if not drawing to texfile
	    glColor3fv( color );
    }

    float lineheight;  // distance from string's ulc y value down
                         // to the baseline
    lineheight = baseline;  // maybe this will do it

    float llx, lly;  // keeps track of the lower left corner
                       // coordinates of the next symbol

    llx = x;  lly = y - lineheight;
 
		if( symbol0Choice == 1 )
		{
   	  if( !texdraw && cursor==0 && showfocus )
	         	drawcursorbackground( llx, lly+symdepth, 
		           widthsym[ littlepagecode ], lineheight, color );
      drawsymbol( llx, lly + symdepth, 
                  littlepagecode,
                  1 ); 
      leftmargin = llx + widthsym[ littlepagecode ]; 
		}
		else if( symbol0Choice == 2 )
		{
   	  if( !texdraw && cursor==0 && showfocus )
	         	drawcursorbackground( llx, lly+symdepth, 
		           widthsym[ absimagecode ], lineheight, color );
      drawsymbol( llx, lly + symdepth, 
                  absimagecode,
                  1 ); 
      leftmargin = llx + widthsym[ absimagecode ]; 
		}
		else if( symbol0Choice == 3 )
		{
   	  if( !texdraw && cursor==0 && showfocus )
	         	drawcursorbackground( llx, lly+symdepth, 
		           widthsym[ boxedjavacode ], lineheight, color );
      drawsymbol( llx, lly + symdepth, 
                  boxedjavacode,
                  1 ); 
      leftmargin = llx + widthsym[ boxedjavacode ]; 
    }
    else if( symbol0Choice == 4 )
    {
      leftmargin = llx;      
    }
		else
		{
   	  if( !texdraw && cursor==0 && showfocus )
	         	drawcursorbackground( llx, lly+symdepth, 
		           widthsym[ trianglecode ], lineheight, color );
      drawsymbol( llx, lly + symdepth, 
                  trianglecode,
                  1 ); 
      leftmargin = llx + widthsym[trianglecode ]; 
		} 

    linecount = 1;  

    llx = leftmargin; // lly stays the same

    for(k=1; k<=numused; k++)
    {// display char #k = s[k-1] and update 

      if( !texdraw && cursor==k && showfocus )
    		drawcursorbackground( llx+padsym[unsignchar(s[k-1])], 
				                      lly+symdepth, 
		        widthsym[ unsignchar(s[k-1]) ], lineheight, color );

      // draw the char, properly positioned
      drawsymbol( llx + padsym[unsignchar(s[k-1])], lly + symdepth,
                     unsignchar(s[k-1]),
                     (cursor==k && showfocus) || !newlines );
      
      llx += widthsym[ unsignchar(s[k-1]) ]; 
			                           // move marker over full width

      // handle enterkey, if is one
      if(s[k-1]==enterkey && newlines)
      {// start new line
        linecount++; // note that we have another line

        // set up for next line:
        llx = leftmargin;
        lly -= lineheight;
      
      }// special --- just drew an enterkey

    }// loop to draw symbol k in the string 

  } // end of display method

  // display the value string of a portbox specially
	//   show all details, do monospacing, and depending
	//   on showendoffile add on special endoffile symbol
  void mystring::portDisplay(  int x, int y,
                     int showendoffile, float color[] )
  {
    int k, linecount;
    float leftmargin;

    if( !texdraw )
    {// only bother to set the color if not drawing to texfile
  	  glColor3fv( color );
    }

    float lineheight;  // distance from string's ulc y value down
                         // to the baseline
    lineheight = baseline;  // maybe this will do it

    float llx, lly;  // keeps track of the lower left corner
                       // coordinates of the next symbol

    llx = x;  lly = y - lineheight;
    
    drawsymbol( llx, lly + symdepth, 
                  trianglecode,
                  1 );
      
    leftmargin = llx + widthsym[trianglecode ]; 

    linecount = 1;  

    llx = leftmargin; // lly stays the same

    float monopad;

    for(k=1; k<=numused; k++)
    {// display char #k = s[k-1] and update 

        monopad = (maxcharwidth - 
				                physwidthsym[ unsignchar(s[k-1]) ])/2;

        drawsymbol( llx + monopad, lly + symdepth,
                     unsignchar(s[k-1]),
                     1 );
      
        llx += maxcharwidth;  

    }// loop to draw symbol k in the string 

		// maybe draw the endoffile symbol
		if( showendoffile )
		{
        monopad = (maxcharwidth - 
				                physwidthsym[ endoffilecode ])/2;

        drawsymbol( llx + monopad, lly + symdepth,
                     endoffilecode,
                     1 );		 
		}

  } // end of portDisplay method

  // display myself in a 2D arrangement, by honoring
  // enterkeys, with ulc at x,y, showcursor, index of cursor char,
  //  let me indicate which spot is the console cursor
	// jb16:  draw tabsymbol in single grid position, using 
	//        special call to draw it narrowly
  void mystring::gridDisplay(  int x, int y,
                 int showcursor, int index )
  {
    int k;

    if( !texdraw )
    {// all drawn in normal color, except when draw cursor
      // char border, use special color, then pop back to normal
      glColor3fv( fulltextcolor );
    }

    float lineheight;  // distance from string's ulc y value down
                         // to the baseline
    float monopad;

    lineheight = baseline;  // maybe this will do it

    float llx, lly;  // keeps track of the lower left corner
                       // coordinates of the next symbol

    llx = x;  lly = y - lineheight;

    for(k=1; k<=numused; k++)
    {// display char #k = s[k-1] and update 

      // enterkey doesn't display, just marks end of row
      if(s[k-1]==enterkey)
      {// start new line
        llx = x;
        lly -= lineheight;     
      }
      else
      {// char to draw
        // wow --- I had k here ----------------vvvvv  !
        monopad = (maxcharwidth - 
				                physwidthsym[ unsignchar(s[k-1]) ])/2;

        // if showing the cursor and k is the cursor guy, draw
        // the border box in special color
        if( showcursor && k==index )
        {// show the cursor
 
          if( !texdraw )
          {// to screen, so need to fool with color
            glColor3fv( consolecursorcolor );
            // draw rectangle around the symbol
            glBegin(GL_POLYGON);
              glVertex3f( llx, lly, textfocusz );
              glVertex3f( llx, lly+lineheight, textfocusz );
              glVertex3f( llx+maxcharwidth, lly+lineheight, 
                             textfocusz );
              glVertex3f( llx+maxcharwidth, lly, textfocusz );
            glEnd();
            glColor3fv( fulltextcolor );
          }
          else
          {// to texfile
          } 

        }

        // jb16:  draw tab only one spot wide to avoid
				//        messing up the grid
				if( unsignchar(s[k-1]) == tabsymbolcode )
          drawspecialsymbol( llx, lly + symdepth,
                     unsignchar(s[k-1]),
                     0 );
        else
				  drawsymbol( llx + monopad, lly + symdepth,
                     unsignchar(s[k-1]),
                     0 );
      
        llx += maxcharwidth;  

      }// not an enterkey

    }// loop to draw symbol k in the string 

  } // end of gridDisplay method

  // I must be the console, simulate incoming being written
  // on my string --- only problem is handling enters in both
  // places, and when need to scroll off end
  void mystring::print( mystring* incoming, int index, int& row, int& col )
  {
    char ch;

    int k, j, linelength;

    linelength = getLineLength();

    j = index; // position in me where we can put the next guy from
           // incoming (guaranteed at start---but might be on enter)

	// 10/4/2000:  added this chunk, crashed when got called with
	//  row 26, col 1, j=1026
	if( j > numused )
	{// starting out in oblivion!
      scroll();
      j -= linelength;
      row--;
	}
  
    for( k=1; k<=incoming->numused; k++ )
    {// process ch = symbol number k in incoming
      ch = incoming->s[k-1];

      if( ch != enterkey )
      {// ordinary symbol to be smacked in 
        s[j-1] = ch;
        // now update j, row, col
        j++;
        col++;
        if( s[j-1] == enterkey )
        {// oops, on the end of line marker, move ahead
          j++;  // just moved to start of next line, or into oblivion
          row++;
          col = 1;
          if( j > numused )
          {// oops --- moved to start of non-existent line,
            // must scroll
            scroll();
            j -= linelength;
            row--;
          }
        }
      }
      else
      {// incoming enterkey---move to start of next line, may need to
        // scroll
        while( s[j-1] != enterkey )
          j++;
        col = 1;
        // now sitting on the enterkey
        if( j < numused )
        {// weren't on last line
          j++;
          row++;
        }
        else
        {// j is on the enterkey of the last line
          scroll();
          // 11/5/2000:  had this:  j -= linelength;
					// but seems wrong---want to move j to first guy in last row,
					// but this is the enterkey before that point!
					// How could this have ever appeared to be okay?
					// So, do this instead:

					j -= linelength;  // sitting on enterkey on next to last
					j++;  // now on first of last row

          // leave row alone

        }
      }

    }// process incoming symbol k

  }// print

  int mystring::getLineLength()
  {
    int k;
    k = 1;
    while( s[k-1] != enterkey )
      k++;
    // now s[k-1] is an enterkey
    return  k;
  }

  // I must be a chargrid, count linelength (including enterkey at end)
  // and go through my entire string, putting in each guy either the
  // guy linelength ahead in index, or ' ' if that index is off the end
  void mystring::scroll()
  {
    int k, linelength;
    linelength = getLineLength();

    for( k=1; k<=numused; k++ )
    {// transfer into me from linelength ahead
      if( k+linelength <= numused )
        s[k-1] = s[k-1 + linelength ];
      else
      {// off end, create ' ' or leave as enter
        if( s[k-1] != enterkey )
          s[k-1] = ' ';
      }
    }
  }

  // return -1 if I'm < other, 0 if I'm == other,
  //   and 1 if I'm > other --- is easy, just use
  //  strcmp
  int mystring::compare( mystring* other )
  {
    int ans;
    // need to stick on null terminators to make compare easy
    insertCharAt( numused+1, '\0' );
    other->insertCharAt( other->length()+1, '\0' );

    ans = strcmp( s, other->s );

    deleteCharAt( length() );
    other->deleteCharAt( other->length() );

    return ans;
  }

  // convert my sequence of symbols to a long integer
  // value and return it, or something, and return
  // function value meaning "okay" or not
  //   allow up to 9 digits --- fits within long, 10 wouldn't
  int mystring::toInt( long& result )
  {
    char ch;
    long val;  // accumulate the value
    long sign;  // note the sign  
    int numdigits = 0;
    int state = 0;  // which state am I in
    int okay = 1;
    
    int k = 0;  // index into my array s

    while( okay && k < numused )
    {
      ch = s[k];  // for typing convenience

      if(state==0)
      {
        if(ch==' ')      state=0;
        else if(ch=='+') { state=1; sign=1; val=0;}
        else if(ch=='-') { state=1; sign=-1; val=0;}
        else if('0'<=ch && ch<='9')
        { 
          state=2; 
          sign=1; 
          val = ch - '0'; 
          numdigits++;
        }
        else
          okay = 0;
      }

      else if(state==1)
      {
        if('0'<=ch && ch<='9')
        {
          state=2;
          val = ch - '0'; 
          numdigits++;         
        }
        else
          okay = 0;
      }

      else if(state==2)
      {
        if('0'<=ch && ch<='9')
        {
          //state = 2;  (stay in state 2)
          val = 10*val + ch - '0';
          numdigits++;
          if(numdigits > 9)
            okay = 0;
        }
        else if(ch==' ')
        {
          state = 3;
        }
        else
          okay = 0;
      }

      else if(state==3)
      {
        if(ch==' ')
        {//stay in state 3
        }
        else
          okay = 0;
      }
      else
        errorexit("illegal state in string::toInt");

      k++;  // move on to next char

    }// end of main processing loop

    // check for not stopping in a stopping state
    if( state != 2 && state != 3 )
      okay = 0;

    // produce the value
    if( okay )
     result = sign * val;
    else
      result = 0;
    
    return okay;
  }

  int mystring::toFloat( double& result )
  {

    char ch;
    double leftval, rightval;  // accumulate the value
    int expval = 0;
    int expdigits = 0;
    int sign = 1, expsign=1;  // note the sign  
    int numdigits = 0; // allow up to 12 digits plus 2 in exp
    int state = 0;  // which state am I in
    int okay = 1;

    double shift = 1.;
    
    int k = 0;  // index into my array s

    rightval = 0;  // may stop in state 2, before were never setting
                   // it to anything in that case

    while( okay && k < numused )
    {
      ch = s[k];  // for typing convenience

      if(state==0)
      {
        if(ch==' ')      state=0;
        else if(ch=='+') { state=1; sign=1; leftval=0;}
        else if(ch=='-') { state=1; sign=-1; leftval=0;}
        else if('0'<=ch && ch<='9')
        { 
          state=2; 
          sign=1; 
          leftval = ch - '0'; 
          numdigits++;
        }
        else if(ch=='.')
        {
          state = 3;
          leftval = 0;
          rightval = 0;
        }
        else
          okay = 0;
      }

      else if(state==1)
      {
        if('0'<=ch && ch<='9')
        {
          state=2;
          leftval = ch - '0'; 
          numdigits++;         
        }
        else if(ch=='.')
        {
          state = 3;
          leftval = 0;
          rightval = 0;
        }
        else
          okay = 0;
      }

      else if(state==2)
      {
        if('0'<=ch && ch<='9')
        {
          //state = 2;  (stay in state 2)
          leftval = 10*leftval + ch - '0';
          numdigits++;
          if(numdigits > 12)
            okay = 0;
        }
        else if(ch=='.')
        {
          rightval = 0;
          state = 3;
        }
        else if(ch=='e' || ch=='E')
        {
          state = 4;
          rightval = 0;
        }
				else if(ch==' ')
				{
				  state = 7;
				}
        else
          okay = 0;
      }

      else if(state==3)
      {
        if('0'<=ch && ch<='9')
        {
          // stay in state 3
          shift = shift / 10;
          rightval = rightval + shift * ( ch - '0' );
          numdigits++;  // in case worried about getting up to 1!
          // allow any number of digits to right of decimal
        }
        else if(ch=='e' || ch=='E')
        {
          state = 4;
        }
				else if(ch==' ')
				{
				  state = 7;
				}
        else
          okay=0;
      }

      else if(state==4)
      {
        if(ch=='+')
        {
          state = 5;
          expsign = 1;
        }
        else if(ch=='-')
        {
          state = 5;
          expsign = -1;
        }
        else if('0'<=ch && ch<='9')
        {
          state = 6;
          expsign = 1;
          expval = ch - '0';
          expdigits++;
        }
        else
          okay = 0;
      }

      else if(state==5)
      {
        if('0'<=ch && ch<='9')
        {
          state = 6;
          expval = ch - '0';
          expdigits++;
        }
        else
          okay = 0;
      }

      else if(state==6)
      {
        if('0'<=ch && ch<='9')
        {
          // stay in state 6
          expval = 10*expval + ch - '0';
          expdigits++;
          if( expdigits > 3 )
            okay = 0;
        }
				else if(ch==' ')
				{
				  state = 7;
				}
        else
          okay = 0;
      }

      else if(state==7)
			{
			  if(ch==' ')
				  state = 7;
				else
				  okay = 0;
			}

      else
        errorexit("illegal state in mystring::toFloat");

      k++;  // move on to next char

    }// end of main processing loop

    // check for not stopping in a stopping state
    if( state != 2 && state != 3 && state !=6 && state != 7)
      okay = 0;

    if( numdigits == 0 )
      okay = 0;

    // produce the value
    if( okay )
    {// assemble the value
      double basevalue, factor;
      basevalue = sign * (leftval + rightval);
      // adjust for the exponent
      if( expsign == 1 )
        factor = 10;
      else
        factor = 1/10.;
      for( k=1; k<=expval; k++ )
        basevalue = factor * basevalue;

      result = basevalue;
    }
    else
      result = 0;
    
    return okay;

  }

int mystring::numberUsed()
{
  return charsused;
}

// send my chars to file and return # sent, just in case needed
int mystring::toFile( FILE* handle )
{
  int k;
	for( k=1; k <= numused; k++ )
	  fprintf( handle, "%c", s[k-1] );
	return numused;
}

// send my chars to file, except as I send a [, send matching
// ] right then!  and tell how many sent and change
//  string to String and float to double
int mystring::toFileArrayAdjusted( FILE* handle )
{
  int k;
	int count = numused;

	for( k=1; k <= numused; k++ )
	{
	  if( k== 1 )
		{
		  if( frontEquals( "string" ) )
      {
			  fprintf( handle, "S" );
			}
			else if( frontEquals( "float" ) )
			{
			  fprintf( handle, "double" );
				count++;  // extra letter in double vs. float
				k += 5;
			}
			else
  	    fprintf( handle, "%c", s[k-1] ); 		  
		}
		else
	    fprintf( handle, "%c", s[k-1] ); 

		if( s[k-1] == '[' )
		{
		  fprintf( handle, "]" );
			count++;  // sent an extra
		}
	}
	return count;
}

// send my chars to file, transforming special chars (\n) to
// Java string format, and return # sent, just in case needed
int mystring::toFileStringForm( FILE* handle )
{
  int k;
	int count = 0;
	for( k=1; k <= numused; k++ )
	  if( s[k-1] == enterkey )
		{
		  fprintf( handle, "\\n" );
		  count += 2;
		}
		else
		{
  	  fprintf( handle, "%c", s[k-1] );
			count++;
		}
	return count;
}

// jb22:
// given a standard string and a position in this mystring,
//  return whether or not the strings match to the end of the
//  given one, and if they do, move the cursor and the position
//  to the end of the string
int mystring::matches( int& pos, char* w )
{
  int k, j;
  int match = 1;

  k = pos;  j = 0;
  while( match && k<=numused && w[j]!='\0' )
  {
    if( s[k-1] == w[j] )
    {// match so far, try to move on
      k++; j++;
    }
    else
      match = 0; // found a discrepancy
  }

  if( match )
  {// move pos, cursor to the spot
    pos = k-1;
    cursor = pos;
  }
  
  return match;
}// matches

// jb25:  produce from this mystring a "typeset" mystring

// jb26:  fine points of typesetting require shared flags between
//        these two functions (finishWord and typeset)

int firstWordInLine;

// utility (called identically in 3 places
void finishWord( float& leftx, float spacewidth, float& wordwidth,
                 float rightx, mystring* text, mystring* word,
                 int leftmargin, int& linecount )
{
  int k;

  // gets called somewhat foolishly with no word, skip it
  if( word->equals("") )
    return;
           
  if( !firstWordInLine && 
           (leftx + spacewidth + wordwidth <= rightx) 
         ) 
  {// later words need a space before them
    text->appendChar( ' ' );
    text->append( word );  // append a copy of the word
    leftx += spacewidth + wordwidth;
    firstWordInLine = 0;
  }
  else  
  {// start a new line, put in its first word 
    if( !firstWordInLine )
    {// forcing start of a new line due to lack of space
     // rather than doing the first line overall
      text->appendChar( 13 );
      linecount++;
    }
    for( k=1; k<=leftmargin; k++ )
      text->appendChar( ' ' );   
    text->append( word );
    leftx = leftmargin * spacewidth + wordwidth;
    firstWordInLine = 0; // just put in the first word
  }

  // either way, tidy up for a new word
  word->destroy();
  word = new mystring("");
  wordwidth = 0;
}

// typeset this mystring to width w and compute height h
mystring* mystring::typeset( float w, float& h )
{
  firstWordInLine = 1;  // reset global each time start typesetting

  mystring* word = new mystring(""); // used to build up current word
                                     // before committing to putting it
                                     // on the line
  mystring* text = new mystring(""); // the typeset text to return

  int state = 0;  // the fsm state
  int sym;    // the current symbol
  int k=1;      // index of the current symbol
  int rawtextlength = length();  

  // the only errors for this fsm involve meaningless escape
  // sequences

  int error = 0;
  char command;
  int code;
  int leftmargin = 1;  // minimum # of spaces to indent each new line
                       // from the border
  float spacewidth = widthsym[ ' ' ];

  int j;

  float leftx = spacewidth * leftmargin;

  float rightx = w - spacewidth; // keep border on the right
  float wordwidth = 0;  // maintain current width of word
  int linecount = 1;  // count nothing as one line, increase by 1
                      // each time char 13 is inserted

  while( k <= rawtextlength && !error )
  {
    sym = charAt( k );   k++;  // for next time around

    if( state==0 )
  
    {
      if( sym==' ' || sym==13 || sym==tabsymbolcode )
      {// whitespace
        state = 0;  // toss whitespace preceding word
      }
      else if( sym=='\\' )
      {// escape sequence
        state = 2;
      }
      else
      {// anything else
        word->appendChar( (char) sym );
        wordwidth += widthsym[ sym ];
        state = 1;  
      }      
    }
    else if( state==1 )
    {
      if( sym==' ' || sym==13 || sym==tabsymbolcode )
      {// whitespace --- have hit definite end of word

        finishWord( leftx, spacewidth, wordwidth,
                    rightx, text, word, leftmargin, linecount );

        state = 0;  // toss whitespace preceding word
      }
      else if( sym=='\\' )
      {// escape sequence starting
        state = 2;
      }
      else
      {// anything else --- continue building the word
        word->appendChar( (char) sym );
        wordwidth += widthsym[ sym ];
        state = 1;  
      }      
    }
    else if( state==2 )
    {
      if( sym==' ' )
      {// hard-coded space---add to current word
        word->appendChar( ' ' );
        wordwidth += widthsym[ sym ];
        state = 1;  
      }
      else if( sym=='\\' )
      {// finishing up \\ escape sequence, add \ to current word
        word->appendChar( '\\' );
        wordwidth += widthsym[ sym ];
        state = 1;
      }
      else if( '0'<=sym && sym<='9' )
      {// escape sequence for symbol
        code = sym - '0';  // start building the symbol's code
        state = 3;
      }
      else if( sym=='i' || sym=='o' )
      {// single letter command sequence to be followed later by digit
       // first finish up the pending word

        finishWord( leftx, spacewidth, wordwidth,
                    rightx, text, word, leftmargin, linecount );

        command = sym;
        state = 4;  // go looking for the digit
      }
      else if( sym=='b' )
      {// finish up the current word, and end the line,
       // note now starting a new line
         
        finishWord( leftx, spacewidth, wordwidth,
                    rightx, text, word, leftmargin, linecount );

        text->appendChar( 13 );

        linecount += 1;

        firstWordInLine = 1;

        state = 0;
      }
      else if( sym=='h' )
      {// escape sequence \h, need digit
        state = 5;
      }
      else
      {// anything else
        error = 1;
      }      
    }
    else if( state==3 )
    {// collecting digits
      if( '0'<=sym && sym<='9' )
      {// add digit to code
        code = 10*code + sym - '0';
        state = 3;
      }
      else if( sym==' ' || sym==13 || sym==tabsymbolcode )
      {// required terminator, check code and add symbol to word if okay
        if( 32<=code && code<=256 )
        {// code is okay, more or less
          word->appendChar( code );
          wordwidth += widthsym[ code ];
        }
        else
          error = 1;

        state = 1;
      }
      else
        error = 1;      
    }
    else if( state==4 )
    {
      if( '1'<=sym && sym<='9' )
      {// depending on command, change leftmargin by sym amount
        if( command == 'i' )
        {// indent by sym
          leftmargin += sym - '0';
        }
        else if( command == 'o' )
        {// outdent by sym, down to at least 1
          leftmargin -= sym - '0';
          if( leftmargin < 1 )
            leftmargin = 1;
        }
        else
          error = 1;

        state = 0;
      }
      else
        error = 1;      
    }
    else if( state==5 )
    {// finish \h#
      if( '1'<=sym && sym<='9' )
      {// add in sym spaces
        for( j=1; j<=sym-'0'; j++ )
          word->appendChar( ' ' );
        wordwidth += (sym-'0')*spacewidth;
        state = 1;
      }
      else 
        error = 1;
    }
    else
      errorexit("mystring::typeset:  illegal fsm state");

  }// fsm loop

  if( error )
  {// signal error to author by obviously fake text
    text->destroy();
    text = new mystring("Error!");
    h = 2*baseline;
  }
  else
  {// was no error, spit last word, compute height

    finishWord( leftx, spacewidth, wordwidth,
                rightx, text, word, leftmargin, linecount );
   
    h = linecount*baseline;
  }
 
  return text;
}// typeset
