// implementation of the noteinfo class

#include <stdio.h>
#include <iostream>
#include <math.h>

#include <glut/glut.h> // Mac OS X
//#include <gl/glut.h> 
//#include "glut.h"

#include "constants.h"
#include "mystring.h"
#include "box.h"
#include "noteinfo.h"

// constructor
noteinfo::noteinfo( box* abox )
{
  nb = abox;
  text = NULL;
}

// extract current info from nb 
// and store in more convenient form in
// this object, including typesetting the note text
// (pass in for convenience the universebox)
void noteinfo::update( box* unibox, 
                       float cvw, float cvh, float pvw, float pvh)
{
  // first access the 5 mystrings contained in the notebox:
  mystring* locstring;
  mystring* style;
  mystring* width;
  mystring* position;
  mystring* rawtext;
  box* curbox;
  curbox = nb->whoFirst();
  locstring = curbox->whatName();
  curbox = curbox->whoNext();
  style = curbox->whatName();
  curbox = curbox->whoNext();
  width = curbox->whatName();
  curbox = curbox->whoNext();
  position = curbox->whatName();
  curbox = curbox->whoNext();
  rawtext = curbox->whatName();

  // process the strings to get available info:

  // starting from the universe box, obtain from the locstring
  //   and style
  //     coords (ax,ay) in the universe box
  //     size (aw,ah) of the framed material, if any
  //   with astyle determining how to do it

  // returns - if have actual box loc info, viewer letter if had ? form
  viewerId = unibox->getInfoFromLocString( locstring, style,
                                ax, ay, aw, ah, astyle,
                                cvw, cvh, pvw, pvh );

  // check position for legality and store
  //   must have one of l, r, b, t for first char,
  //  or c
  //   second char is optional, if there must be one of n, l, r, b
  // Record error as edge=='e'
  if( position->length() == 1 || position->length() == 2 )
  {// has correct length
    edge = position->charAt(1);
    if( edge=='c' )
    {
      if( position->length() == 2 )
        edge = 'e';  // error to follow 'c' by any filling
    }
    else if( edge=='l' || edge=='r' || edge=='b' || edge=='t' )
    {// good edge choice, look into filling
      if( position->length() == 2 )
      {// have possible filling, check it out
        filling = position->charAt(2);
        if( filling!='n' && filling!='l' && filling!='r' &&
            filling!='b'
          )
          edge = 'e';
      }
      else
        filling = 'n';  // default is neither
    }
    else
      edge = 'e';  // bad edge
  }
  else
    edge = 'e';  // indicates bad position

  // process width string to check legality, if good store in w,
  // else record error in edge
  int okay;
  double value;
  okay = width->toFloat( value );
  if( okay && value>0 )
  {// value is a reasonable width
    w = value;
  }
  else
    edge = 'e';

  // typeset the rawtext to text using given width w and determine
  //  height h
  text = rawtext->typeset( w, h );

}// update

// Lesson::displayNotes sets up the OpenGL environment correctly
// to allow each noteinfo to draw anywhere in the window
void noteinfo::display()
{
  // draw background rectangle for the note text
  glColor3f( 1.0, 1.0, 1.0 );
  glBegin(GL_POLYGON);
    glVertex3f( nx, ny, notez );
    glVertex3f( nx, ny-h, notez );
    glVertex3f( nx+w, ny-h, notez );
    glVertex3f( nx+w, ny, notez );
  glEnd();

  // draw the note text
  textz = notetextz;
  text->coreDisplay( nx, ny, 4, 1, 0, fulltextcolor );
  textz = savetextz;

  // draw the vivid border
  glColor3f( 1, 0, 0 );
  glBegin(GL_LINE_LOOP);
    glVertex3f( nx, ny, notetextz );
    glVertex3f( nx, ny-h, notetextz );
    glVertex3f( nx+w, ny-h, notetextz );
    glVertex3f( nx+w, ny, notetextz );
  glEnd();

  // draw the arrow (note that (ax,ay) has been transformed by
  //  lesson into same coord system as (nx,ny), w, h, and all,
  //  using its access to the viewer

  if( (astyle == 'a' || astyle == 'l' || astyle == 'f') &&
       edge != 'c' 
    )
  {// draw the line for the arrow, 
   //   is not an error in its style, and not centering, so have a line

    // determine (cx,cy) as center of inner edge of note
    float cx, cy;

    if( edge == 'l' )
    {// center of right edge
      cx = nx + w;
      cy = ny - h/2;
    }
    else if( edge == 'r' )
    {// center of left edge
      cx = nx;
      cy = ny - h/2;
    }
    else if( edge == 't' )
    {// center of bottom edge
      cx = nx + w/2;
      cy = ny - h;
    }
    else //  edge == 'b' 
    {// center of top edge
      cx = nx + w/2;
      cy = ny;
    }     

    // leave color vivid (red)
    glLineWidth( 2 );  // draw a thicker line

    // now draw the head
    if( astyle == 'l' )
    {// just draw the line
      glBegin(GL_LINES);
        glVertex3f( cx, cy, notetextz ); // center of correct edge
        glVertex3f( ax, ay, notetextz ); // arrowhead location
      glEnd();
    }
    else if( astyle == 'a' )
    {// draw line with actual arrowhead---simple mathy stuff
      glBegin(GL_LINES);
        glVertex3f( cx, cy, notetextz ); // center of correct edge
        glVertex3f( ax, ay, notetextz ); // arrowhead location
      glEnd();

      // compute angle of rotation
      float beta, dx, dy;

      dx = cx-ax;  dy = cy-ay;

      // 57.blah = 180/pi
      beta = (float) acos( dx/sqrt( dx*dx + dy*dy)) * 57.29577951;

      if( dy < 0 )
        beta = -beta;

      glPushMatrix(); 
        // translate from origin 
        glTranslatef( ax, ay, 0 );
        // turn it
        glRotatef( beta, 0, 0, 1 );
        // draw arrowhead with tip at origin, pointing right to left
        // along x axis
        glLineWidth( 1 );  // put it back!
        glBegin( GL_POLYGON );
          glVertex3f( 0, 0, notetextz );
          glVertex3f( 2*typsymwidth, typsymwidth, notetextz );
          glVertex3f( 2*typsymwidth, -typsymwidth, notetextz );         
        glEnd();  
      glPopMatrix();
    }
    else if( astyle == 'f' )
    {// draw the frame, using aw, ah so laboriously obtained

      // first, get bx, by as closest corner of the framed region
      // to cx, cy
      float bx, by;

      if( fabs( cx - (ax+aw) ) < fabs( cx - ax ) )
        bx = ax+aw;
      else
        bx = ax;
      if( fabs( cy - (ay-ah) ) < fabs( cy - ay ) )
        by = ay-ah;
      else
        by = ay;

      // draw the adjusted line
      glBegin(GL_LINES);
        glVertex3f( cx, cy, notetextz ); // center of correct edge
        glVertex3f( bx, by, notetextz ); // arrowhead location
      glEnd();

      glBegin(GL_LINE_LOOP);
        glVertex3f( ax, ay, notetextz );
        glVertex3f( ax, ay-ah, notetextz );
        glVertex3f( ax+aw, ay-ah, notetextz );
        glVertex3f( ax+aw, ay, notetextz );
      glEnd();     
    }
    // else do nothing

    glLineWidth( 1 );  // put it back!

  }// draw the arrow

}// display

// utilities for arrangement:

int noteinfo::isPinned()
{
  return pinned;
}// isPinned
  
// pin down this note, using current as the position along the
// edge that is currently used, and using glue as the glue amount
//   Need view region dimensions, too
//  Return new value for current
float noteinfo::pin( float current, float glue, 
                     float vw, float vh )
{
  // if have edge error, mark as pinned and skip it
  if( edge == 'e' )
  {
    pinned = 1;
    return -1;
  }

  // do centered note independent of all others
  // jb30
  if( edge == 'c' )
  {
    pinned = 1;
    nx = (vw-w)/2;
    ny = vh-(vh-h)/2;
    return current;  // unchanged
  }

  float result = current;

  // add glue at front?
  if( filling == 'b' || filling == 'l' )
  {
    if( edge=='l' || edge=='r' )
      result -= glue;   // move down
    else
      result += glue;   // move right
  }

  // depending on edge, set nx or ny to result and then adjust
  // result for moving on, and set the other coordinate, too
  if( edge == 'l' || edge == 'r' )
  {//moving down
    ny = result;
    result -= h;
    if( edge == 'l' )
      nx = 0;
    else
      nx = vw - w;
  }
  else // top or bottom
  {// moving right
    nx = result;
    result += w;
    if( edge == 't' )
      ny = vh;
    else
      ny = h;
  }

  // add glue at end?
  if( filling == 'b' || filling == 'r' )
  {
    if( edge=='l' || edge=='r' )
      result -= glue;
    else
      result += glue;
  }

  pinned = 1;     // mark as pinned

  return result;  // current current

}// pin

void noteinfo::unpin()
{
  pinned = 0;
}// unpin

// return width or height of this note, depending on edge
float noteinfo::getSize()
{
  if( edge == 't' || edge == 'b' )
    return w;
  else if( edge == 'l' || edge == 'r' )
    return h;
  else
    return 0;
}// getSize

// return number of glues requested
int noteinfo::getGlue()
{
  if( filling == 'n' || filling == 'e' )
    return 0;
  else if( filling == 'l' || filling == 'r' )
    return 1;
  else if( filling == 'b' )
    return 2;
  else
  {
    return -1;
  }
}// getGlue

char noteinfo::getEdge()
{
  return edge;
}// getEdge

// given current corners of inner region, update using this noteinfo's
// size and position
//   Note:  the code assumes that there is an inner region once all the
//          notes have squeezed it inward.  If not, strange things
//          may happen, which are all the author's fault
void noteinfo::adjustInnerBorder( float& ulx, float& uly,
                        float& lrx, float& lry )
{
  if( pinned )
  {// depending on which edge this note is positioned on, it might
   // shrink two coordinates of the inner region

    if( edge == 'l' && nx+w > ulx )
      ulx = nx+w;

    if( edge == 'r' && nx < lrx )
      lrx = nx;

    if( edge == 't' && ny-h < uly )
      uly = ny-h;

    if( edge == 'b' && h > lry )
      lry = h;
  }
  // else don't adjust, is in error, probably

}// adjustInnerBorder

void noteinfo::convertArrowheadToViewCoords( float cx, float cy )
{
  ax += cx;  
  ay += cy;
}// convertArrowheadToViewCoords

char noteinfo::getViewer()
{
  return viewerId;
}

box* noteinfo::getNote()
{
  return nb;
}

// release heap resources, namely the text
// (notebox will be released elsewhere)
void noteinfo::destroy()
{
  if( text != NULL )
    text->destroy();
}
