// the box class implementation

#include <stdio.h>
#include <iostream>

#include <glut/glut.h> // Mac OS X
//#include <gl/glut.h> 
//#include "glut.h"

#include <math.h>
#include <stdlib.h>

#include "constants.h"

#include "mystring.h"
#include "box.h"  
#include "chargridtype.h"
#include "portstype.h"
#include "utility.h"

#include "translator.h"

extern translator* tran;

extern portstype* ports;
extern chargridtype* chargrid;

#include "viewer.h"
#include "MessageQueue.h"

#include "Lesson.h"

extern Lesson* lesson;

long boxesused = 0; // global used in this class

  box::box()
  {
    next = NULL;
    outer = NULL;
    first = NULL;

    current = NULL;
    cursor = 0;
    
    name = NULL;
    type = NULL;
    value = NULL;
    doc = NULL;

    // jb18:  all special props are unset at creation
		// props = 0;
    // jb24:  set props to 14 which is bit pattern 1110
    //        meaning can kill, add, change, not breakpoint
    props = 14;

        boxesused++;  // keep track of how many are in use on heap

  }

  // deallocate the guts of the box, but leaves the name, doc
  // intact, and leaves the box
  void box::coredestroy()
  {
    if(type != NULL )
    {
      type->destroy();
      type = NULL;
    }   
    if(value != NULL )
    {
      value->destroy();
      value = NULL;
    }

    // recursively deallocate my inner list:
    box* curbox;
    box* nextbox;

    nextbox = first;
    while( nextbox != NULL )
    {
      curbox = nextbox;
      nextbox = curbox->next;  // get ready to move on

      curbox->destroy(); // toss all the innards

    }

  }// end of coredestroy

  // deallocate all heap space used by this box,
  // still need to "delete" pointer to the box to release
  // the space it uses.  This guy is recursive, takes care
  // to deallocate all the strings used, too

  //   Must take care not to deallocate guys that are pointed
  //   to by many boxes, such as hard-coded box names
  void box::destroy()
  {
    // deallocate parts I use
    if(doc != NULL )
    {
      doc->destroy();
      doc = NULL;
    }

// kind zone +++++++++++++++++++++++++++++++++++++++++++++++++++++
//  don't deallocate the name string for certain kinds of boxes
//   when destroying them because a single string is shared 
//   and/or protected from changing
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    if(name != NULL && kind != classdatabox && 
                       kind != classmethodsbox && 
                       kind != instancedatabox &&
                       kind != instancemethodsbox &&
                       kind != paramsbox &&
                       kind != localsbox &&
                       kind != stackbox && 
                       kind != heapbox && 
                       kind != consolebox &&
											 kind != retvalbox  &&       // v10
											 kind != allclassdatabox )       
    {
      name->destroy();
      name = NULL;
    }

    coredestroy();  // deallocate the main part of this box

    delete this;

      boxesused--;  // update global resource count --- I'm gone!

    // // 

  } // end of destroy

  // build myself as a new, empty box of the specified kind
  // using default values---if want others, must set them
  // separately
  void box::build( int knd )
  {

    int junk;
    box* newbox;

    int numinner, k;

    // set all INTERNAL (not next, outer which may need to leave
    // intact when converting)
    // structural pointers to default values, just to be
    // tidy (yes!)
    first = NULL;
    current = NULL;  cursor = 0;

    type = NULL;
    value = NULL;
    source = NULL;
    state = initialstate;

    // leave name, doc as they are, really may only be building
    // the full aspect --- "converting"

  	dye = nocolorcode;  // brand new guy can't be dyed!
	  color = blueprintcolorcode; // a few aren't, but most are,
	                            // since that's where interactive
	                            // creation happens
    kind = knd;

// kind zone ++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    build each kind of box 
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    if(kind==databox)
    {
      setAspect('f'); moveInside(); setPart('n');
    }
    else if(kind==emptybox)
    {
      setAspect('f'); moveOutside(); setPart(' ');
    }
    else if( kind == valuebox )
    {
      setAspect('f'); moveOutside(); setPart('v');
      name = NULL; doc = NULL;
      value = new mystring("");
    }
    else if( kind == gridbox )
    {
      setAspect('f'); moveOutside(); setPart('i');
      name = new mystring(""); 
      doc = NULL;
      type = new mystring("");
	  }
    else if(kind==univbox)
    {
      setAspect('f'); moveOutside(); setPart('n');

      name = new mystring("");  

  	  color = neutralcolorcode; // is not a blueprint
	    depth = 0;
  
      newbox = new box;
      newbox->build( stackbox );
      addInnerAtCursor( newbox );

      newbox = new box;
      newbox->build( heapbox );
      addInnerAtCursor( newbox );

// v10:  add allclassdatabox here
      newbox = new box;
			newbox->build( allclassdatabox );
			addInnerAtCursor( newbox );

      newbox = new box;
      newbox->build( consolebox );
      addInnerAtCursor( newbox );

//jer
      // build portsbox, store pointer into global ports,
			// cast to box* type
      newbox = new box;
			newbox->build( portsbox );
			addInnerAtCursor( newbox );

      // put cursor back on first guy for tidiness
      current = first;
      cursor = 1;
    }

// v10:  build allclassdatabox
    else if(kind==allclassdatabox )
		{
		  setAspect('f'); moveOutside(); setPart('i');
      name = allclassdatastring;
		}

// v10:  classdatacopybox
    else if(kind==classdatacopybox)
		{
		  setAspect('f');  moveOutside();  setPart('i');
			name = new mystring("");
		}

		else if(kind==portsbox)
		{
      setAspect('f'); moveOutside(); setPart('n');
      
      name = new mystring("ports");

      for(k=1; k<=numports; k++)
			{
        newbox = new box;
				// set name to "k:" BEFORE calling build,
				//  type and value are set to standard values inside build
				//  (all must be set before finish build, so that
				//    sizeLocDisplay doesn't crash)
				char tempstring[6];
	      sprintf( tempstring, "%d:", k );			
        newbox->name = new mystring( tempstring );
        
				newbox->build( portbox );

        addInnerAtCursor( newbox );
      }

      // put cursor back on first guy for tidiness
      current = first;
      cursor = 1;
		}

		else if(kind==portbox)
		{
      setAspect('f'); moveOutside(); setPart('n');	  
		  type = new mystring("--");
			value = new mystring("");
		}

    else if(kind==classbox)
    {
      setAspect('f'); moveOutside(); setPart('n');
      
      name = new mystring("");

	  // note that color is set to blueprint here, so inners can
	  // inherit it when added, later when add this new classbox,
	  // it will try to inherit neutralcolor, but is special case
      newbox = new box;
      newbox->build( classdatabox );
      addInnerAtCursor( newbox );

      newbox = new box;
      newbox->build( classmethodsbox );
      addInnerAtCursor( newbox );

      newbox = new box;
      newbox->build( instancedatabox );
      addInnerAtCursor( newbox );
      
      newbox = new box;
      newbox->build( instancemethodsbox );
      addInnerAtCursor( newbox );

      // put cursor back on first guy for tidiness
      current = first;
      cursor = 1;

    }
    else if( kind==classdatabox )
    {
      setAspect('f'); moveOutside(); setPart('i');     
      name = classdatastring;     
    }

    else if( kind==classmethodsbox )
    {
      setAspect('f'); moveOutside(); setPart('i');     
      name = classmethodsstring;     
    }
    else if( kind==instancedatabox )
    {
      setAspect('f'); moveOutside(); setPart('i');     
      name = instancedatastring;     
    }
    else if( kind==instancemethodsbox )
    {
      setAspect('f'); moveOutside(); setPart('i');     
      name = instancemethodsstring;     
    }
    else if( kind==stackbox )
    {
      setAspect('f'); moveOutside(); setPart('i');
      name = stackstring;
	}
    else if( kind==heapbox )
    {
      setAspect('f'); moveOutside(); setPart('i');
      name = heapstring;
    }
    else if( kind==consolebox )
    {
      setAspect('f'); moveOutside(); setPart('i');
      name = consolestring;
    }
    // some kinds just never get built, because they
    //  can't be added interactively, can't be saved
    //  hence no need to load, since can't save when
    //  paused or executing, and when stop, insides of
    //  heap and stack will be deleted.

    else if(kind==methodbox)
    {
      setAspect('f'); moveOutside(); setPart('n');
      
      name = new mystring("");  // method name left blank

      newbox = new box;
      newbox->build( paramsbox );
      addInnerAtCursor( newbox );

      newbox = new box;
      newbox->build( retvalbox );
      addInnerAtCursor( newbox );
      
      newbox = new box;
      newbox->build( localsbox );
      addInnerAtCursor( newbox );

      newbox = new box;
      newbox->build( seqbox );  // the "code box"
      addInnerAtCursor( newbox );
      
      // put cursor back on first guy for tidiness
      current = first;
      cursor = 1;

    }
    else if( kind==paramsbox )
    {
      setAspect('f'); moveOutside(); setPart('i');     
      name = paramsstring;
    }
//v10:
    else if( kind==retvalbox )
		{
      setAspect('f'); moveInside(); setPart('t');     
      name = retvalstring;  
		}
    else if( kind==localsbox )
    {
      setAspect('f'); moveOutside(); setPart('i');     
      name = localsstring;     
    }
    else if( kind==seqbox )
    {
      setAspect('f'); moveOutside(); setPart('i');
    }

    // jb19, jb21
		else if( kind==lessonbox )
		{// full aspect is like a seqbox, but has a name,
		 // starts with 0 inners
		  name = new mystring("Demo ");
			setAspect('a'); moveInside(); setPart('n');
		}
    else if( kind==parbox )
		{// use name to hold the comstring, similar to javabox
      setAspect('a'); moveInside(); setPart('n');
      name = new mystring("");
		}
    else if( kind==combox )
		{// use name to hold the comstring, similar to javabox
      setAspect('a'); moveInside(); setPart('n');
      name = new mystring("");
		}
    else if( kind==presbox )
		{// use name to hold the comstring, similar to javabox
      setAspect('a'); moveInside(); setPart('n');
      name = new mystring("");
		}
    else if( kind==notebox )
    {//has 5 inner parboxes
      setAspect('f'); moveOutside(); setPart('i');

      for(k=1; k<=5; k++)
      {
        newbox = new box;
        newbox->build( parbox );
        addInnerAtCursor( newbox );
      }

      // put cursor back on first guy for tidiness
      current = first;
      cursor = 1;    
    }// notebox

    else if( kind==dobox || kind==whilebox || kind==forbox ||
             kind==branchbox )
    {// build a kind with just a fixed number of hard-coded emptyboxes
      // at start

      setAspect('f'); moveOutside(); setPart('i');

      if(kind==branchbox)
        numinner = 1; // the default action box
      else if(kind==dobox || kind==whilebox )
        numinner = 2;
      else if(kind==forbox)
        numinner = 4;

      for(k=1; k<=numinner; k++)
      {
        newbox = new box;
        newbox->build( emptybox );
        addInnerAtCursor( newbox );
      }

      // put cursor back on first guy for tidiness
      current = first;
      cursor = 1;
    }

    // jb16
		else if( kind == javabox )
		{// has a single empty inner, starts showing name part

      setAspect('a'); moveInside(); setPart('n');

      name = new mystring("");
      // jb23:  was just a mistake to set doc to empty,
      //            don't do anywhere else
			// doc = new mystring("");

			// value, type used by execution 

      newbox = new box;
      newbox->build( emptybox );
      addInnerAtCursor( newbox );
      		  
		}

    else if( valuekind( kind ) || 
             kind==assbox || kind==growbox || kind==returnbox
           )
    {// build all value kinds and some others, 
      // characterized by number of 
      // mandatory boxes and whether or not they have a name part
      
      setAspect('f'); moveOutside(); 

      if( kind==idbox || literalkind(kind) ||
          kind==memberbox || kind==opbox || kind==unopbox ||
          kind==assbox || kind==growbox ||
          kind==newopbox 
        )
        setPart('n');
      else
        setPart('i');
      
      if(kind==idbox || literalkind(kind) )
        numinner = 0;
      else if(kind==callbox || kind==memberbox || kind==unopbox ||
              kind==growbox || kind==returnbox )
        numinner = 1;
      else if(kind==arraybox || kind==opbox || kind==assbox ||
              kind==newopbox )
        numinner = 2;
      else if(kind==array2dbox )
        numinner = 3;
      else
        errorexit("illegal kind in value box section");

      for(k=1; k<=numinner; k++)
      {
        newbox = new box;
        newbox->build( emptybox );
        addInnerAtCursor( newbox );
      }

      // put cursor back on first guy for tidiness
      current = first;
      if( current != NULL ) // allow for numinner=0 case
        cursor = 1;
      else
        cursor = 0;

    }

    else
      errorexit("tried to build illegal kind of box");

    figureSizeLocs(junk);  // compute my size and locs of inners

    // // 

  }// build

  // I'm a newopbox, using my name as the requested type,
  // build a grid and return it, or NULL if type is no good
  //
  //  numrows = 0 means build a 1D grid, else build a 2D
  //  grid (is really 1D, of course, but store #cols in 
  //  the member "state" --- 0 if is really 1D
  //  
  // go ahead and do all the work to check out my name as a
  // type, build the grid, add to the heap, return it
  box* box::addGridboxToHeap( box* u,
                   int numrows, int numcols, int& status )
  {
    box* curbox;
    box* newbox;
    box* vbox;
    int k;

    // see if my name is a legitimate primitive type or
    // class name
    if( !isprimtype( name ) )
    {// not primitive, see if is a class
      curbox = u->first;
      while( curbox->kind==classbox &&
              ! (curbox->name)->equals( name ) )
        curbox = curbox->next;
      if( curbox->kind == stackbox )
      {
        status = nosuchtypeerror;
				// // 
        return this;
      }
    } 
    // if still here, know that my name is either primitive type
    // or name of a class, so go ahead and build the grid

    newbox = new box();    // allocate space
    newbox->build( gridbox );  // build the empty gridbox

    int numelts;

    if( numcols == 0 )
    {
      newbox->state = 0;  // shows is 1D, else would need row size
      numelts = numrows;
    }
    else
    {
      newbox->state = numcols;  // is 2D, state is row size
      numelts = numrows * numcols;
    }
  
    for( k=1; k<=numelts; k++ )
    {
      vbox = new box();  // empty space to use
      vbox->build( valuebox );
      newbox->addInnerAtFront( vbox );
    }

    // now that newbox is built as a tidy little grid,
    // insert it into the heap
      // set curbox to heap
      curbox = u->first;
      while( curbox->kind != heapbox )
        curbox = curbox->next;
      // add the gridbox into the heap
      curbox->addInnerAtFront( newbox );

      // hmmm, source left hanging, I guess?

      // type and name built NULL, so nothing to release
         // store the newbox's type as my name
         // jboxes1d: above comment should be 
         // "store my name as the newbox's type" I believe
         newbox->storeType( name );

      // key bit:  get next available heap address and make
      //           that into newbox's name
      currentHeapAddress++;  // will never have time to new more than
                             // a few billion boxes!
      char where[15];
      sprintf( where, "#%ld", currentHeapAddress );

      // jboxes1d:  small leak here, storeName always makes a copy,
      //   old version left the original out there
      //newbox->storeName( new mystring( where ) );
      mystring* whereString = new mystring( where );
      newbox->storeName( whereString );
      whereString->destroy();
      // end of jboxes1c leak fix

      newbox->fixupReality(0);

      // hey, give me my info:
      storeValue( newbox->name );
      storeType( name );

      // append 1 or 2 [ guys to indicate dimension
      type->insertCharAt( type->length()+1, '[' );
      if( numcols > 0 )
        type->insertCharAt( type->length()+1, '[' );

      status = 1;

			// // traceLeave();
      return newbox;

  }// addGridboxToHeap

  // I'm a gridbox, given row, col, 
  //  and if col==0 then 1D and row is the index,
  //  return errorcode and myself as source of error if
  //  these guys are out of range, or don't match my state,
  //  else return pointer to my
  //  inner box matching the request
  box* box::accessArrayElement( int row, int col, int& status )
  {
	  // traceEnter( tid, "box:accessArrayElement" );

    if( kind != gridbox )
      errorexit("non-gridbox in accessArrayElement");

    int index; // is position of specified element, first is 1...

    // look at my state to determine case
    if( state == 0 ) 
    {// 1D case
      if( col != 0 )
      {
        status = nonemptysecondfor1derror;
				// // traceLeave();
        return this;
      }

      index = row+1;
    }
    else
    {// 2D case
      if( col >= state )
      {
        status = colindextoolargeerror;
				// // traceLeave();
        return this;
      }

      index = row*state + col + 1;    
    }

    // step through until hit index in count, or realize
    //  row (first index) is too large, because run out too soon
    box* curbox;   int count;

    curbox = first;  count = 1;
    while( curbox != NULL && count<index )
    {
      curbox = curbox->next;
      count++;
    }

    // decide if ran out or got it
    if( curbox == NULL )
    {
      if( col == 0 )    // send out correct error message
        status = indextoolargeerror;
      else
        status = rowindextoolargeerror;
			// // traceLeave();
      return this;
    }
    else
    {
      status = 1;
			// // traceLeave();
      return curbox;
    }

    // // traceLeave();

  }// accessArrayElement
      
  // load myself from the univstream, of course involves
  // recursively loading my inner boxes as well
  //   Doesn't allocate space for me, nor set my outer/next pointers
  //   Assumes I'm brand new (haven't got any string resources
  //    to be lost, in particular)
  void box::load( FILE* infile )
  {
	  // traceEnter( tid, "box:load" );

    // get the non-recursive parts of myself
    fscanf( infile, "%d\n", &kind); 

    char asp, ins, par;

    fscanf( infile, "%c%c%c\n", &asp, &ins, &par );

    setAspect( asp );  
    inside = ins;
    setPart( par );  

 	  fscanf( infile, "%d\n", &color );
	  dye = nocolorcode;

	  fscanf( infile, "%d\n", &depth );

    // jb18:  load the new props field
		fscanf( infile, "%d\n", &props );

    // next, outer will be set 
    //  when I am added to my owner's inner list

    //  first will be set below when loading my inners

    int mycursor;  // can't store safely in cursor since
                   // addInnerAtCursor changes it below

    fscanf( infile, "%d\n", &mycursor );// use to set cursor later

    // load my strings
    mystring* w; // so same code can work for all strings
    int count, k, code;

// kind zone +++++++++++++++++++++++++++++++++++++++++++++++++++
    // oops---certain kinds use shared name strings, so
    //  mustn't load or save their names
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    if( kind==classdatabox )
      name = classdatastring;
    else if(kind==classmethodsbox)
      name = classmethodsstring;
    else if(kind==instancedatabox)
      name = instancedatastring;
    else if(kind==instancemethodsbox)
      name = instancemethodsstring;
    else if(kind==paramsbox)
      name = paramsstring;

		else if(kind==retvalbox)   // v10
		  name = retvalstring;
		else if(kind==allclassdatabox) 
		  name = allclassdatastring;

    else if(kind==localsbox)
      name = localsstring;
    else if(kind==stackbox)
      name = stackstring;
    else if(kind==heapbox)
      name = heapstring;
    else if(kind==consolebox)
      name = consolestring;
    else
    {// actually load name (vs. using a shared name string)

      // load a string---   name
      fscanf( infile, "%d ", &count ); // number in string
      if(count>0)
      {// load the string
        w = new mystring();

        for( k=1; k<=count; k++ )
        {// add in next char
          fscanf( infile, "%d ", &code );
          w->insertCharAt( k, (char) code );
        }
        w->setCursor( 1 );

        name = w;  
      }

// kind zone +++++++++++++++++++++++++++++++++++++++++++++++++++++
//   some kinds expect a name, so make their's "", rather than 
//    NULL
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      if( count == 0 &&
          (kind==univbox || kind==databox || kind==classbox ||
           kind==methodbox || kind==idbox || literalkind(kind) ||
           kind==memberbox || kind==opbox || kind==unopbox ||
           kind==assbox || kind==growbox || kind==newopbox ||
					 kind==javabox // jb16
					          ||
					 kind==lessonbox || kind==combox // jb19
                    ||
           kind==parbox || kind==presbox || kind==notebox // jb21
          )
        )
        name = new mystring( "" );

    } // actually load a name

    // load a string--- type
    fscanf( infile, "%d ", &count ); // number in string
    if(count>0)
    {// load the string
      w = new mystring();

      for( k=1; k<=count; k++ )
      {// add in next char
        fscanf( infile, "%d ", &code );
        w->insertCharAt( k, (char) code );
      }
      w->setCursor( 1 );

      type = w;  
    }

    // load a string--- value
    fscanf( infile, "%d ", &count ); // number in string
    if(count>0)
    {// load the string
      w = new mystring();

      for( k=1; k<=count; k++ )
      {// add in next char
        fscanf( infile, "%d ", &code );
        w->insertCharAt( k, (char) code );
      }
      w->setCursor( 1 );

      value = w;  
    }

    // load a string--- doc
    fscanf( infile, "%d ", &count ); // number in string
    if(count>0)
    {// load the string
      w = new mystring();

      for( k=1; k<=count; k++ )
      {// add in next char
        fscanf( infile, "%d ", &code );
        w->insertCharAt( k, (char) code );
      }
      w->setCursor( 1 );

      doc = w;  
    }

    // read how many inners I have stored
    fscanf( infile, "%d\n", &count );

    // loop through and load all my inners and
    // set my first and current
    box* curbox;
    box* mycurrent;

    for( k=1; k<=count; k++ )
    {// load inner number k
      curbox = new box();  // allocate space
      curbox->load( infile );      // load the guy in
      addInnerAtCursor( curbox );  // add it---and set outer to me

      // note data for me
      if(k==1)
        first = curbox;
      if(k==mycursor)
        mycurrent = curbox;

    }

    // now, put the cursor on mycursor
    if(mycursor==0)
    {// on symbol0
      cursor = 0;
      current = NULL;
    }
    else
    {// on a real inner
      cursor = mycursor;
      current = mycurrent;
    }

    // finally, update my inner locs and size:
    int junk;
    figureSizeLocs( junk );

    // set my execution stuff just in case:
    state = 0;  // really only need for method boxes
    source = NULL;  // probably a total waste

    // // traceLeave();

  }// load

  // save myself to outfile
  void box::save( FILE* outfile )
  {
	  // traceEnter( tid, "box:save" );

    // save the non-recursive parts of myself
    fprintf( outfile, "%d\n", kind); 
    fprintf( outfile, "%c%c%c\n", aspect, inside, part );
  	fprintf( outfile, "%d\n", color );
	  fprintf( outfile, "%d\n", depth );
		// jb18
		fprintf( outfile, "%d\n", props );
    fprintf( outfile, "%d\n", cursor );

    // save my strings
    mystring* w; // so same code can work for all strings
    int count, k;

// kind zone +++++++++++++++++++++++++++++++++++++++++++++++++++++
    // for certain kinds that share a constant name string,
    // just skip saving name
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    if( kind==classdatabox || kind==classmethodsbox ||
        kind==instancedatabox || kind==instancemethodsbox ||
        kind==paramsbox || kind==localsbox || 
        kind==stackbox || kind==heapbox || kind==consolebox ||
				kind==retvalbox || kind==allclassdatabox )   //v10
    {// skip saving any name
    }
    else
    {// save the name
     
      // save a string---   name
      w = name;
      if( w==NULL)
        count = 0;
      else
        count = w->length();
      fprintf( outfile, "%d ", count ); // number in string
      for( k=1; k<=count; k++ )
      {// write out next char
        fprintf( outfile, "%d ", (int) w->charAt( k ) );
      }
      fprintf( outfile, "\n" );
    }// save the name

    // save a string---   type
    w = type;
    if( w==NULL)
      count = 0;
    else
      count = w->length();
    fprintf( outfile, "%d ", count ); // number in string
    for( k=1; k<=count; k++ )
      {// write out next char
        fprintf( outfile, "%d ", (int) w->charAt( k ) );
      }
    fprintf( outfile, "\n" );

    // save a string---   value
    w = value;
    if( w==NULL)
      count = 0;
    else
      count = w->length();
    fprintf( outfile, "%d ", count ); // number in string
    for( k=1; k<=count; k++ )
      {// write out next char
        fprintf( outfile, "%d ", (int) w->charAt( k ) );
      }
    fprintf( outfile, "\n" );

    // save a string---   doc
    w = doc;
    if( w==NULL)
      count = 0;
    else
      count = w->length();
    fprintf( outfile, "%d ", count ); // number in string
    for( k=1; k<=count; k++ )
      {// write out next char
        fprintf( outfile, "%d ", (int) w->charAt( k ) );
      }
    fprintf( outfile, "\n" );

    // loop through and save all my inners 
    box* curbox;

    // first, need to count them
    curbox = first;  count = 0;
    while( curbox != NULL )
    {
      count++;
      curbox = curbox->next;
    }

    fprintf( outfile, "%d\n", count ); // number of inners

    // now save the inner boxes
    curbox = first;
    for( k=1; k<=count; k++ )
    {// save inner number k
      curbox->save( outfile );     
      curbox = curbox->next;
    }

    // // traceLeave();

  }// box::save

  // replacements for setDisplaymode---safely (in terms of
  // creating strings only as needed for storage efficiency)
  // change various settings

  void box::moveInside()
  {
    inside = 'y';
  }

  void box::moveOutside()
  {
    inside = 'n';
  }

  // possible part values are:
  //   'i'  --> inner list, cursor determines rest
  //   'n'      name
  //   't'      type
  //   'v'      value
  void box::setPart( char which )
  {
	  // traceEnter( tid, "box:setPart" );
    part = which;
		// // traceLeave();
  }

  void box::setAspect( char which )
  {
	  // traceEnter( tid, "box:setAspect" );
    if(which=='a')
    {// switching to abstract aspect, make sure name string exists
      if(name==NULL)
        name = new mystring;
    }
    else if(which=='d')
    {// switching to doc aspect, make sure doc string exists
      if(doc==NULL)
        doc=new mystring;
    }
    
    else if(which=='v')
    {// switching to value aspect, if value or type
      // doesn't exist, build them empty
      if(value==NULL)
        value = new mystring;
      if(type==NULL)
        type = new mystring;
    }

    else if(which=='f')
    {// switching to full aspect, make sure all part strings
      // exist, depends on kind

// kind zone: ++++++++++++++++++++++++++++++++++++++++++++++++++++
//    when switch to full aspect of guy that has a non-constant
//      name, need to build it
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

      // kinds that have a name that is not a constant
      //  (kinds built during execution will always have a name,
      //   so they don't need to be listed here)
      if(kind==databox || kind==univbox || kind==classbox ||
         kind==idbox || literalkind(kind) || kind==memberbox ||
         kind==opbox || kind==unopbox || kind==assbox ||
         kind==growbox || kind==newopbox || kind==portbox )
      {
        if(name==NULL)
          name = new mystring();  // jboxes1d:  added () on end?
      }

      // kinds  that have a type part
      if(kind==databox || kind==portbox || kind==retvalbox )  //v10
      {
        if(type==NULL)
          type = new mystring;
      }

      // kinds that have a value part
      if(kind==databox || valuekind( kind ) ||
			   kind==portbox || kind==retvalbox            //v10
				)
      {
        if(value==NULL)
        {
          value = new mystring;
//   value->setFont('0'); // mono to make it look more computerish!

        }
      }

    }// 'f' aspect

    aspect = which;

    // // traceLeave();

  }// setAspect

  // if on or in abstract or doc, or in full and on a string
  // cycle to being on the next aspect
  // (basically meant to be border to border guy, add on the
  // inside cases for convenience, since there is no other
  // interpretation of cycling when in these situations)
  box* box::cycleAspects( int& didit )
  {
    // jb24:  see if permitted before doing it
    if( ! canChange() )
    {
      didit = 0;
      return this;
    }

    int junk;

// kind zone ++++++++++++++++++++++++++++++++++++++++++++++++++++
//   depending on kind, may have any of these possible aspects
//   to cycle through interactively:
//       abstract (name), doc, full, value
//         Some allow cycling to empty aspect so can edit,
//         but others don't
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // kinds with meaningful abstract, doc, full aspects
    if( kind==univbox || kind==databox || kind==classbox || 
        kind==methodbox ||
				kind==lessonbox  //jb19
            ||
        kind==notebox  // jb21
      )
    {
      if(aspect=='a')
      {// abstract --> doc
        setAspect( 'd' );
      }
      else if(aspect=='d')
      {// doc --> full
        setAspect( 'f' );
      }
      else if (aspect=='f')
      {// full --> abstract
        setAspect( 'a' );
      }
    }

    // jb19:  kinds with just abstract and doc!  jb21, too
		else if( kind==combox || kind==parbox || kind==presbox )
		{
      if(aspect=='a')
      {// abstract --> doc
        setAspect( 'd' );
      }
      else if(aspect=='d')
      {// doc --> abstract
        setAspect( 'a' );
      }
		}

    // kinds with abstract, doc, full, and value
    else if( kind==javabox )
		{
      if(aspect=='a')
      {// abstract --> doc
        setAspect( 'd' );
      }
      else if(aspect=='d')
      {// doc --> full
        setAspect( 'f' );
      }
      else if (aspect=='f')
      {// full --> value
        setAspect( 'v' );
      }
			else if(aspect=='v')
			{// value-->abstract
			  setAspect( 'a' );
			}
		}

    // kinds with just full and doc---would be silly to name
    // (if want to "abstract visually" use doc)
    else if( actionkind( kind ) )     // callbox lands here, good
    {
      if(aspect=='d')
      {// doc --> full
        setAspect( 'f' );
      }
      else if (aspect=='f')
      {// full --> doc
        setAspect( 'd' );
      }
    }

    // kinds with full, doc, and value
    else if( valuekind( kind ) )      
    {
      if(aspect=='v')
      {// value --> doc
        setAspect( 'd' );
      }
      else if(aspect=='d')
      {// doc --> full
        setAspect( 'f' );
      }
      else if (aspect=='f')
      {// full --> value
        setAspect( 'v' );
      }
    }

    // kinds with full and name, no doc (owned by system,
    //  silly and confusing to let user document them, or
    //  created during execution, so any documenting would be
    //  wasted totally in short order!)
    else if( kind==stackbox || kind==heapbox || kind==consolebox ||
             kind==classdatabox || kind==classmethodsbox ||
             kind==instancedatabox || kind==instancemethodsbox ||
             kind==paramsbox || kind==localsbox || 
             kind==classmethodbox || kind==instancemethodbox ||
             kind==instancebox || kind==gridbox || 
						 kind==portsbox  // 8/9/2000:  eliminate: || kind==portbox
						  || // v10
						 kind==retvalbox || kind==allclassdatabox ||
						 kind==classdatacopybox
           )
    {
      if(aspect=='f')
      {
        setAspect( 'a' );
      }
      else if(aspect=='a')
      {
        setAspect( 'f' );
      }
    }

    // kinds with just full aspect
    else if( kind==valuebox || kind==portbox )
    {
      // nothing to do---won't cycle off full
    }

    else
    {
      errorexit("unhandled kind in box::cycleAspects");
    }

    moveOutside();

    figureSizeLocs( junk );

    didit = 1;

		// // traceLeave();

    return this;
  }

  // jump between parts within the full aspect --- means that
  // if we are on border, may need to jump away from inner
  // list and to some other part
  // Doesn't need to update reality
  // Almost always "does it"
  // May return "this" or outer

  // modifying this guy after realizing was quite wrong!
  box* box::jumpParts( int& didit )
  {
	  // traceEnter( tid, "box:jumpParts" );

    int knd;

    didit = 1;  // lazy --- avoid shipping out in various good cases

    if( isInside() )
    {// focus is inside, on symbol0 or on a string part
      // depending on kind, jump to another part

      if(aspect != 'f' )
      {// inside abs or doc, jump makes no sense
        didit = 0;
				// // traceLeave();
        return this;
      }

// kind zone ++++++++++++++++++++++++++++++++++++++++++++++++++++++
//   jump part to part within full aspect
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

      if(kind == databox )   
      {// has name, type, value parts
        if( part == 'n' )
        {
          setPart( 't' );
					// // traceLeave();
          return this;
        }
        else if( part == 't' )
        {
          setPart( 'v' );
					// // traceLeave();
          return this;
        }
        else if( part == 'v' )
        {
          setPart( 'n' );
					// // traceLeave();
          return this;
        }     
        else
        {                                       //v10
          errorexit("illegal part in databox or retvalbox in box::jumpParts");
					// // traceLeave();
          return NULL;
        }

      }// databox

      else if(kind==classmethodbox || kind==instancemethodbox ||
              kind==instancebox || kind==gridbox
             )
      {// guys with name, type, inner list
        if( part == 'n' )
        {
          setPart( 't' );
					// // traceLeave();
          return this;
        }
        else if( part == 't' )
        {
          setPart( 'i' );
          if(cursor==0)
          {// focus stays on me
  					// // traceLeave();
            return this;
          }
          else
          {// actually moving to an inner box
  					// // traceLeave();
            return current;
          }
        }
        else if( part == 'i' )
        {
          setPart( 'n' );
 					// // traceLeave();
          return this;
        }     
        else
        {
          errorexit("illegal part in cimethodbox in box::jumpParts");
  				// // traceLeave();
          return NULL;
        }
      } // kinds with name, type, inner list

      else if(kind==univbox || kind==classbox || kind==methodbox ||
              kind==memberbox || kind==opbox ||
              kind==unopbox ||
              kind==assbox || kind==growbox ||
              kind==newopbox 
							//v10
							|| kind==classdatacopybox )
      {// kind with exactly an editable name part and an inner list,
         //  both  "0 or more" style or fixed length

        if( part == 'n' )
        {// jump from name part to inner list

          setPart( 'i' );

          if(cursor==0)
          {// focus stays on me
 					  // // traceLeave();
            return this;
          }
          else
          {// actually moving to an inner box
            // // traceLeave();
            return current;
          }
          
        }
        else if( part == 'i' )
        {// since inside, must be on symbol0 of inner list,
          // jump to name part
          setPart( 'n' );
          // // traceLeave();
          return this;
        }
        else
        {
           errorexit("illegal part in box::jumpParts");
 					 // // traceLeave();
           return NULL;
        }

      }// kind with name part and inner list

      else
      {// inside a box that simply doesn't allow a jump,
        // including all with just an inner list and no other parts,
        // and ones with constant name part and inner list
        didit = 0;
     		// // traceLeave();
        return this;
      }

    } // inside

    else
    {// on my border, so jumping depends entirely on my
      // outer

      // focus is on me as part of inner list of my outer,
      // depending on its kind, jump to one of its parts

      box* owner = outer;

      if(owner==NULL)
      {
        didit = 0;
				// // traceLeave();
        return this;
      }

      knd = owner->kind;

// kind zone +++++++++++++++++++++++++++++++++++++++++++++++++++++
//   focus is on my border, jump depends on owner's kind==knd
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

      // kinds that have exactly an editable name and an inner list
      // jumping from me as part
      // of inner list to owner's name part
      if( knd==univbox || knd==classbox || knd==methodbox ||
          knd==memberbox || knd==opbox ||
              knd==unopbox || knd==assbox || knd==growbox ||
              knd==newopbox 
				)

      {
        owner->setPart( 'n' );
        owner->moveInside();
        // // traceLeave();
        return owner;
      }

      else if( knd==classmethodbox || knd==instancemethodbox 
             )
      {
        owner->setPart( 'n' );
        owner->moveInside();
        // // traceLeave();
        return owner;
      }

    // if no cases are picked up, is no jumping possible,
    // report didn't happen, stay on me
              // includes ones with a constant name
      didit = 0;
      // // traceLeave();
      return this;

    }// end of not inside me

    // // traceLeave();

  } // jumpParts

  float box::howWide()
  {
	  // traceEnter( tid, "box:howWide" );
		// // traceLeave();
    return width;
  }

  float box::howHigh()
  {
	  // traceEnter( tid, "box:howHigh" );
		// // traceLeave();
    return height;
  }

// for editing convenience and to keep file size smaller,
//  put in a separate file sizelocdisplay and related guys
#include "boxdisplay.cpp"

  // given (offsetx,y) from my ulc, return NULL if not over me,
  // else return the innermost box that the offset is over,
  // and return the part in that box that it is over,
  // and the offset of the location from the ulc of that box
  //
  //   if is just over a box, not over any part, partx,y will
  //     be the offset in the box,
  //   if is over a part, will be the offset from the ulc
  //    of the part
	
	//   May 31, 2000:  this is only called by 
	//    viewer::setFocustolocation, change internally to
	//     do correct thing when the click ends up being on
	//     a box (portbox is only kind for now) that wants to
	//     pass the focus outward

  // 10/7/2000:  make globals mouseoffx,y be correct on all exit
	//             points (seems like a good idea anyway!), will use
	//             where called to provide offset of mouseclick in the
	//             new focus box, so on right click can reposition 
	//             viewer correctly
  box* box::findLocation( float offsetx, float offsety,
                      char& whichpart, 
                      float& partx, float& party )
  {
	  // traceEnter( tid, "box:findLocation" );

    int changed;
    float ix, iy, f1, f2, f3, f4;
    char wp;

    box* curbox;
    box* innerbox;

    curbox = this;  // start with me---might be on me, you know!
    float curx, cury;

    curx = offsetx;  cury = offsety;  
       // curx,y will hold offset of search location in curbox
       // as we move inward

    // search inward for where click occurred
    int done = 0;
    do{

      // may be happy to stop looking on curbox
			if( curbox->kind == portbox )
			{// happy --- adjust offset properly and quit
        whichpart = '.';  // say just over the portsbox
        partx = curbox->ulcx + curx;  party = curbox->ulcy + cury;

        // 10/8/2000
        mouseoffx = partx;  mouseoffy = party;

			  return curbox->outer;  // return the portsbox
			}
			else
			{// still searching

        // use sizeLocDisplay to locate curx,cury in curbox
        innerbox = curbox->sizeLocDisplay( 0, changed,
                    0, 
                    0.0, 0.0,  // x,y
                    0.0, 0.0, 0.0, 0.0,  // left,right,top,bottom
                    0, NULL,
                    1, curx, cury,
                    wp, ix, iy,
                    0, f1, f2, f3, f4 );

        if( wp == '?' )
        {// curx,y not over curbox at all (can only happen on first
          //  box)
  				// // traceLeave();
          return NULL;
        }
        else if( wp=='.' || wp=='a' || wp=='d' ||
                 wp=='n' || wp=='t' || wp=='v' || wp=='i' )
        {// curx,y over curbox, not over an inner box
          whichpart = wp;
          partx = ix;  party = iy;

         // note offset from ulc of newfocusbox to mouse click
          // location
          mouseoffx = curx;  mouseoffy = cury;
  
	  			// // traceLeave();
          return curbox;
        }
        else if( wp=='I' )
        {// over an inner box, so continue search
  
          curx = curx - innerbox->ulcx;
          cury = cury - innerbox->ulcy;

          curbox = innerbox;

        }
        else
          errorexit("error in box::findLocation");
      }// still searching

    }while(!done);

		// // traceLeave();
    return curbox;

  }// box:findLocation

  // given a possibly new part "whichpart", and offset partx,y
  // within the "part" (might be whole box),
  // fix myself up to be correct for this new focus
  void box::setDataToLocation( char whichpart, float partx, float party )
  {
	  // traceEnter( tid, "box:setDataToLocation" );

    // involves some simple bookkeeping, and a little string
    // geometry

    if(whichpart=='.')
    {// means just over me, not on an inside part
      inside = 'n';  // make sure focus is on my border
      // leave part alone, can come back in to it

    }
    else if(whichpart=='i')
    {// apparent focus is on my inner list symbol0
      inside = 'y';
      part = 'i';

      current = NULL;  // memo myself that current is now on                      
      cursor = 0;      // symbol0

    }

// kind zone +++++++++++++++++++++++++++++++++++++++++++++++++++
//    kinds with constant name strings (or otherwise just not
//     clickable) that mustn't be focused on
//     even if mouse clicked on the name
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
   
    else if( (whichpart=='n' || whichpart=='a')
              && 
             ( kind==classdatabox || kind==classmethodsbox ||
               kind==instancedatabox || kind==instancemethodsbox ||
               kind==paramsbox || kind==localsbox ||
               kind==stackbox || kind==heapbox || kind==consolebox ||
							 kind==portsbox || kind==portbox || 
							 //v10
							 kind==retvalbox || kind==allclassdatabox 
							 //v10
             )
           )
    {// put focus on my border
      inside = 'n';
    }

    else
    {// apparent focus is on a string part
      inside = 'y';

      // if whichpart holds a real part of full aspect,
      // note it---otherwise, leave it alone!
      if( whichpart != 'a' && whichpart != 'd' )
        part = whichpart;

       // fix up which symbol in the string
       mystring* which; // the apparent focus string

       which = whichString();  // picks up doc,abs,value from aspect

       float actx, acty; 
       int index, row, col;

       // adjust cursor on which to reflect the offset partx,party

     // jb16:  separate javabox abstract from all others
		 //        as symbol0 == 3
		 // jb19:  have to include combox like javabox
     // jb21:  and parbox and presbox
	   int symbol0;
	   if( whichpart == 'a' && kind != javabox && kind != combox 
               && kind != parbox && kind != presbox )
  		 symbol0 = 2;
		 else if( whichpart == 'a' && 
		          (kind == javabox || kind==combox ||
               kind == parbox || kind==presbox 
              )
					  )
		   symbol0 = 3;
	   else if( whichpart == 'd' )
	  	 symbol0 = 1;
	   else 
		   symbol0 = 0;

       which->findNearest( symbol0,
		                    partx, party, 
												// jb16:  doc or abstract of javabox
								(aspect=='d') || (kind==javabox && aspect=='a')
								   ||
								 ((kind==combox || kind==parbox || kind==presbox) 
                    && aspect=='a'),
                            index, actx, acty, row, col );

       which->setCursor( index );

    }

    // // traceLeave();
		    
  }// setDataToLocation

// similar to setDataToLocation, except doesn't change the box, just
// determines the symbol in the mystring part, if appropriate for
// whichpart
  void box::findSymbolInPart( char whichpart, float partx, float party,
                              int& index )
  {
    if(whichpart=='.')
    {// means just over me, not on an inside part
      index = -1;  // just in case try to use it accidentally
    }
    else if( aspect == 'v' )
    {
      index = -1;  // can't use value or type part for value aspect
    }
    else if(whichpart=='i')
    {// apparent focus is on my inner list symbol0
      index = 0;
    }

    // in setDataToLocation, more complex because wanted to
    // refuse to "set data" inward on certain boxes, but here
    // we just gather info for arrowhead pointing, so not an issue

    else
    {// mouse over a string part

      // determine which symbol in the string
      mystring* which; // the string corresponding to given whichpart

      if( aspect == 'f' )
      {
        if( whichpart == 'n' )
          which = name;
        else if( whichpart == 't' )
          which = type;
        else if( whichpart == 'v' )
          which = value;
        else
          errorexit("box::findSymbolInPart: illegal whichpart");
      }
      else if( aspect == 'd' )
      {
        which = doc;
      }
      else if( aspect == 'a' )
      {
        which = name;
      }
      else
        errorexit("box::findSymbolInPart: illegal aspect");
    
      float actx, acty; 
      int row, col;

      // determine code for symbol 0 of string to pass to findNearest
	    int symbol0;
	    if( whichpart == 'a' && kind != javabox && kind != combox 
               && kind != parbox && kind != presbox )
  	 	  symbol0 = 2;
		  else if( whichpart == 'a' && 
		          (kind == javabox || kind==combox ||
               kind == parbox || kind==presbox 
              )
					  )
		    symbol0 = 3;
	    else if( whichpart == 'd' )
	  	  symbol0 = 1;
	    else 
		    symbol0 = 0;

      // determine index
      which->findNearest( symbol0,
		                     partx, party, 
			 					(aspect=='d') || (kind==javabox && aspect=='a')
								   ||
								 ((kind==combox || kind==parbox || kind==presbox) 
                    && aspect=='a'),
                            index, actx, acty, row, col );

    }//mouse over a string part
	    
  }// findSymbolInPart

  // return my position (first box is 1) in inner list,
  // if I have an outer, 1 if I don't
  int box::positionInInnerList()
  {
	  // traceEnter( tid, "box:positionInInnerList" );
    int index;
    box* owner;
    box* curbox;

    owner = outer;

    if( owner == NULL )
    {// no owner, so my position is 1, obviously!
		  // // traceLeave();
      return 1;
    }
    else
    {// have an owner

      curbox = owner->first; // can't be NULL, since I'm in the list
      index = 1;

      while( curbox != this && curbox != NULL )
      {// step ahead if not me
        curbox = curbox->next;
        index++;
      }

      if( curbox==NULL )
        errorexit("oops in box:: positionInInnerList");

 		   // // traceLeave();
       return index;
     
    }

  } // box::positionInInnerList

  // change box reality to simulate interactive moving of
  // focus from me (the current focus) to newfocus
  //    Do this by moving outward to the universe, then 
  //     outward from newfocus, but simulate moving inward,
  //     in terms of how we set the data for the boxes being
  //     visited
  void box::simulateMove( box* newfocus )
  {
	  // traceEnter( tid, "box:simulateMove" );

    box* curbox;
    box* owner;

    // save info to restore details of newfocus at end
    char nfi = newfocus->inside;
    char nfp = newfocus->part;
		int onSymbol0;
		if( newfocus->cursor == 0 && newfocus->inside )
		  onSymbol0 = 1;
		else
		  onSymbol0 = 0;
    
    // scan outward
    curbox = this;
    owner = curbox->outer;
    while( owner != NULL )
    {// have curbox and owner, is all info needed to set data
      // simulating move outward from curbox to owner
      
      curbox->moveOutside(); 
      owner->current = curbox;
      owner->cursor = curbox->positionInInnerList();

      // me and newfocus must be visible, means everybody is
      // showing full aspect all the way out
      if( owner->aspect != 'f' )
        errorexit("hmmm, strange thing in box::simulateMove");

      owner->part = 'i';  

      // actually move outward
      curbox = owner;
      owner = curbox->outer;
    }// outward scan

    // simulate scan inward from universe to newfocus by
    // scan outward, setting owner as it would be if were moving
    // in from owner to curbox
    curbox = newfocus;
    owner = curbox->outer;

    while( owner != NULL )
    {// simulate focus on owner moving into curbox---what changes
      // happen to owner?
      owner->current = curbox;
      owner->cursor = curbox->positionInInnerList();
			// jb18:  wow!  moveInside seems wrong, actually want to
			//         set inside to 'n' meaning focus is on the box,
			//         not on a symbol in a string part
               // owner->moveInside();
      owner->moveOutside();
      owner->part = 'i';
      
      // move out
      curbox = owner;
      owner = curbox->outer;
    }// simulated inward scan

    // now, restore state of newfocus, carefully!
    newfocus->inside = nfi;
    newfocus->part = nfp;

/*  6/17/2000:  leave this out---certainly can have
       nfi = y and nfp = i and not be on symbol0!

    6/19/2000:  oops---mouse click on symbol0 apparently
		             moves focus, but then behaves as if hasn't

								 need to note whether originally on symbol0,
								 restore
*/

    if( onSymbol0 )
    {// were on symbol0, but moved off during scan
      newfocus->current = NULL;
      newfocus->cursor = 0;
    }

    // // traceLeave();

  }// simulateMove

  // I have just become the focus, set me and all my outward
  // ancestors to be current in their outer's inner lists
  //   This must be done to keep box reality true---otherwise,
  //   inward nested mouse click can cause current to be 
  //   incorrect
  void box::setCurrentOutward()
  {
	  // traceEnter( tid, "box:setCurrentOutward" );

    box* curbox;
    box* owner;

    curbox = this;  // can't be NULL at first loop 
    owner = curbox->outer;


    while( owner != NULL )
    {
      // set curbox as current inner box
      owner->current = curbox;
      owner->cursor = curbox->positionInInnerList();

      // move outward
      curbox = owner;
      owner = owner->outer;

    }

    // // traceLeave();

  }// box::setCurrentOutward

  // move outward from me until hit universe,
  // setting data correctly
  box* box::setFocusToUniv( int& didit )
  {
	  // traceEnter( tid, "setFocusToUniv" );

    //setCurrentOutward();

    box* curbox = this;
    didit = 1;
    while( didit )
      curbox = curbox->moveOut( didit );
    didit = 1;

    // newfocus will be the universe
    simulateMove( curbox );

    // // traceLeave();
    return curbox;
  }

  // utility to call sizeLocDisplay just for the purpose
  // of finding the offset and size of the apparent focus 
  // for me---might be 0,0, my size, or might be size of
  // apparent focus symbol inside me
  void box::findApparentOffset( float& inx, float& iny, 
                           float& inw, float& inh )
  {
	  box* tempbox;
    int chnged;
    char pn;
    float ix, iy;

    tempbox = sizeLocDisplay( 0, chnged,
         0, 
         0.0, 0.0,  // x,y
         0.0, 0.0, 0.0, 0.0,  // left, right top, bottom
         0, NULL,
          0, 0.0, 0.0,
          pn, ix, iy,
          1, inx, iny, inw, inh );
  }

  // my apparent focus (either my ulc or something inside me)
  //  is at the point (viewx, viewy) in the coord system
  //   with its origin at the llc of a view area of given
  //    size viewwidth by viewheight. 
  //  Scan outward from me until find first 
  //    outer box that covers the view area, or hit the
  //    limitbox, or hit the univbox
  //  (limitbox is so we can not show extraneous stuff in
  //   a viewer that is intending to show, say, the local
  //   variables of the currently running method)

  //  can send in limitbox as NULL if don't want to cut off early
  box* box::findBackBox( float viewwidth, float viewheight,
           float viewx, float viewy, 
           box* limitbox,
           float& backx, float& backy )
  {
	  // traceEnter( tid, "box:findBackBox" );

    box* curbox;

    float curx, cury;

    curbox = this;  // start with me---can't be NULL,
      
    curx = viewx; cury = viewy;
         
    // then, we can scan outward for first box---might even be me!
    // that covers the view area

    int done = 0;

    while( !done )
    {
      // see if curbox covers view area
      if( curx <= 0 && cury >= viewheight &&
          curx + curbox->width >= viewwidth &&
          cury - curbox->height <= 0 )
      {// curbox covers, pass out info
        backx = curx; backy = cury;
        done = 1;
      }
      else
      {// curbox doesn't cover, move outward if can

        if(curbox==limitbox || curbox->kind==univbox)
          done = 1;  // can't go any further
        else
        {// move out
          curx = curx - curbox->ulcx;
          cury = cury - curbox->ulcy;

          curbox = curbox->outer;

        }
      }

    }// loop to scan outward

    // pass back coords in view area of back box ulc
    backx = curx;  backy = cury;

    // // traceLeave();
    return curbox;  // is either me or an outer one

  }// end of findBackBox

  // search outward from my ulc (not my apparent focus)
  // for targetbox,
  // maintaining offset in inx,y, return 0 if never hit
  // before forced to stop, return 1 and the offset if hit it
  int box::findOffsetOutward( box* targetbox, 
                           float& inx, float& iny )
  {
	  // traceEnter( tid, "box:findOffsetOutward" );

    box* curbox;
  
    curbox = this;  // start with me---can't be NULL,

    // initialize inx,y to apparent offset
//    findApparentOffset( inx, iny, w, h );  // don't care w, h

    inx = 0; iny = 0;

    // before had inx=0, iny=0, worked okay, but had problem,
    // with the change above, became awful!
    
    int done = 0, found = 0;    

    while( !done )
    {
      if( curbox == NULL )
      {
        done = 1;
        found = 0; 
      }
      else if( curbox == targetbox )
      {// hit it!
        done = 1;
        found = 1;
      }
      else
      {// haven't hit, try to move outward
          
        inx += curbox->ulcx;
        iny += curbox->ulcy;

        curbox = curbox->outer;

      }

    }// loop to scan outward

    // // traceLeave();
    return found;
  }

  // starting from me, scan outward doing size/loc updating
  // until hit univbox or encounter a box whose size doesn't
  // change---so no one outer needs work
  //
  // note that figureSizeLocs assumes that all the inner boxes
  // of the guy who is asked to do it are correctly sized---it
  // doesn't recurse inward (though it would be trivial to make
  // it do so---we've cut it off intentionally for efficiency)
  //
  //  later may want to produce helpful info, say for 
  // positioning a viewer over reality, but I don't know what
  // yet!
  void box::fixupReality( int dontstopearly )
  {
	  // traceEnter( tid, "box:fixupReality" );

    box* curbox;
    int changed;

    curbox = this;  // start with me---can't be NULL   

    int done = 0;

    // have to fix me up, but won't stop until an outer box
    // hasn't changed
    curbox->figureSizeLocs( changed );

    while( !done )
    {
      // try to move out:
      if(curbox->outer == NULL )
        done = 1;  // can't move out so the process stops
      else
      {// have an outer, move to it and continue

        curbox = curbox->outer;

        curbox->figureSizeLocs( changed );

        if(!changed && !dontstopearly )
          done = 1;  // stop---this outer box didn't change size
        // else loop around 
      }

    }// loop to scan outward

    // // traceLeave();

  }// end of fixupReality

  // return pointer to string corresponding to the guy we
  // are looking for, based both on the aspect and the part
  mystring* box::whichString()
  {
	  // traceEnter( tid, "box:whichString" );

    mystring *temp;  // alias the desired string for convenience
  
    if(aspect=='a')
      temp = name;  // grab the name string
    else if(aspect=='d')
      temp = doc;   // grab the doc string
    else
    {// full aspect, use part to determine which string
      if (part == 'n')
        temp = name;
      else if (part=='t')
        temp = type;
      else if (part=='v')
        temp = value;
      else
        errorexit("illegal part in box::whichString");
    }

	  // // traceLeave();
    return temp;
  }

  char box::whichAspect()
	{
	  return aspect;
	}

  // manage everything for moving focus from me to my next,
  // including possibility that focus is inside me
  box* box::moveNext( int& didit )
  {
	  // traceEnter( tid, "box:moveNext" );

    if(isInside())
    {// move to next symbol in my focal part
      // (1/4/00: this seems ridiculous:  if(part=='i' && cursor==0)
			if( aspect=='f' && part=='i' && cursor==0)
			// only way can be inside and be the focus is if
      {// focus is on symbol0 of inner list
        current = first;  // move my "cursor" to first box, if any

        if(current==NULL)
        {// inner list is empty, refuse to move "next"
          didit = 0;
 		      // // traceLeave();
          return this;
        }
        else
        {// have an inner box, move onto it
          cursor = 1;

          didit = 1;
          // // traceLeave();
          return first;
        }

      }
      else
      {// focus is on an actual string part
        mystring *temp = whichString(); // decide which part  
        if(temp->whereCursor() < temp->length() )
        {
          temp->advanceCursor();
          didit = 1;
        }
        else
          didit = 0;

        // // traceLeave();
        return this;  // focus stays on me either way

      }

    }// focus is inside

    else
    {// move to my next box
   
      if(next==NULL)
      {// have no next, keep focus on me
        didit = 0;
 		    // // traceLeave();
        return this;
      }
      else
      {
        // update my outer's current from me to my next
        box* owner = outer;
        owner->current = next;
        owner->cursor += 1;
        
        didit = 1;
        // // traceLeave();
        return next;
      }
    }

    // note:  is no need to update box reality at all
    //        after this operation

  }

  // boxes don't store pointer to previous, so need to
  // find it by searching from start
  box* box::findPrev()
  {
	  // traceEnter( tid, "box:findPrev" );

    box *current;
    box *prev;

    if(outer==NULL)
      errorexit("tried to do prev on box with no outer???");

    current = outer;  
    current = current->first;

    if(current == this)
    {// I am the first box
      prev = NULL;
      // somebody else will need to notice and set focus on my
      // outer box, with displaymode = 14, to put it on
      // box #0 of the list I am first in
    }
    else
    {// loop to find guy before me
      while( current != this )
      {
        prev = current;
        current = current->next;
      }
    }

	  // // traceLeave();
    return prev;

  }

  box* box::movePrev( int& didit )
  {
	  // traceEnter( tid, "box:movePrev" );

    if(isInside())
    {// move prev on my focal part
      // 1/4/00:  was missing "aspect=='f' before
      if(aspect=='f' && part=='i' && cursor==0)
      {// on symbol0 of inner list, can't move prev
        didit = 0;
  		  // // traceLeave();
        return this;
      }
      else
      {// on an actual string part

        mystring *temp = whichString();  // see which part is focal
        if(temp->whereCursor() > 0 )
        {// not on symbol0 of the string, so move cursor back
          temp->retreatCursor();
          didit = 1;
        }

        else // on symbol0 
          didit = 0;

  		  // // traceLeave();
        return this;  // focus stays on me
      }

    } // inside
    else
    {// move to the box before me, or symbol0 before me,
      // or realize there is no box before me
      //  (either by checking with outer, or realizing I'm
      //   the univbox)

      box* newfocus;

      if(kind == univbox )
      {
        didit = 0;
   		  // // traceLeave();
        return this;
      }

      // didn't return because not universe box, go on:

        newfocus = findPrev();

        if(newfocus==NULL)
        {// depending on kind, move to symbol0, which means
         // changing display mode, etc.

          box* owner = outer;

          int knd = owner->kind;  // have to type knd a zillion times

        // kinds of owners that have a symbol0
        if(

// kind zone +++++++++++++++++++++++++++++++++++++++++++++++++++++
//        moveprev to symbol0 from me if owner has a symbol0
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        
            knd==univbox || knd==classdatabox || 
            knd==classmethodsbox || knd==instancedatabox ||
            knd==instancemethodsbox || knd==paramsbox ||
            knd==localsbox || knd==seqbox || knd==branchbox          
						  ||
						knd==lessonbox //jb19 (had combox)
          )
        {// I am first in an inner list that has a symbol0

          owner->current = NULL;  // mark current as on symbol0
          owner->cursor = 0;         // of inner list

          owner->setPart( 'i' );
          owner->moveInside();

          didit = 1;
    		  // // traceLeave();
          return owner; // my outer gets "inside focus"
        }
        else
        {// there is no previous box to me, nor symbol0
          didit = 0;       // so can't move previous
    		  // // traceLeave();
          return this;
        }

      }
      else
      {// I have a previous box, move to it

        // update my outer's current info
        box* owner = outer;
        owner->current = newfocus;
        owner->cursor -= 1;

        didit = 1;
    	  // // traceLeave();
        return newfocus;

      }

    }

    // note:  no need to update reality

  }// box::movePrev

  // given a positive index, return pointer to my inner box
  // at that position, NULL if there is none such
  box* box::accessInnerAt( int index )
  {
    // traceEnter( tid, "box:accessInnerAt" );

    int k;
    box* curbox;
    
    k = 1;  curbox = first;

    while( k < index && curbox != NULL )
    {
      k++;
      curbox = curbox->next;
    }

    if(k != index)
		{
      return NULL;  // couldn't get to index
		}
    else
		{
      return curbox;  // may be NULL, if first is NULL
		}
  }

  // return number of inner boxes
  int box::innerLength()
  {
    int k;
    box* curbox;
    k=0; curbox = first;
    while( curbox != NULL )
    {
      k++;
      curbox = curbox->next;
    }
    return k;
  }

  // move focus out, either from inner part, or in standard
  // box sense
  box* box::moveOut( int& didit )
  {   
	  // traceEnter( tid, "box:moveOut" );

    if(isInside())
    {// focus is on an inner part, just move to border
      moveOutside();
      didit = 1;
      // // traceLeave();
      return this;    
    }
    else
    {
      if(kind == univbox )
      {// can't move out from the universe box
        didit = 0;
        // // traceLeave();
        return this;        
      }
      else
      {// standard box moving out
        // make sure actually move out
        moveOutside();
        didit = 1;
        box* owner = outer;
        owner->moveOutside();
        // // traceLeave();
        return outer;
      }
    }

    // no need to update reality on this method
  } // moveOut

  // depending on my kind, move focus to an inner part,
  // namely the preferred one for my kind,
  // return the new focus (which may be me)
  box* box::moveIn( int& didit )
  {
    // traceEnter( tid, "box:moveIn" );

    box* curbox;

    // late change, so splice it in here to avoid changing 
    // existing behavior
    if( aspect == 'v' )
    {// refuse to move in on value aspect
      didit = 0;
      // // traceLeave();
      return this;
    }

    if( inside=='n' && aspect == 'a' )
    {// on border of abstract, move in unless I have a constant name

// kind zone ++++++++++++++++++++++++++++++++++++++++++++++++++++++
//     not allowed to move in on constant names, others
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

      if( kind==classdatabox || kind==classmethodsbox ||
          kind==instancedatabox || kind==instancemethodsbox ||
          kind==paramsbox || kind==localsbox ||
          kind==stackbox || kind==heapbox || kind==consolebox ||
					kind==portsbox || kind==portbox || 
					//v10 
					kind==retvalbox  || kind==allclassdatabox
        )
      {// don't allow move in
        didit = 0;
    	  // // traceLeave();
        return this;
      }
      else
      {// okay to move in
        moveInside();
        didit = 1;
    	  // // traceLeave();
        return this;
      }
    }
    else if( inside=='n' && aspect == 'd' )
    {// move in on doc aspect
      moveInside();
      didit = 1;
      // // traceLeave();
      return this;
    }
    else
    {// branch on kind to move in on others
      // either outside, or on full aspect

      // so, if inside, must be on full, but
      // can't go in
      if(inside=='y')
      {
        didit = 0;
     	  // // traceLeave();
        return this;
      }

// kind zone ++++++++++++++++++++++++++++++++++++++++++++++++++++++
//     now, outside on full aspect, move in properly
//     depending on kind
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

      // kinds with just inner parts that are strings, no inner list
      if(kind==databox ||
         kind==idbox || literalkind(kind) ||
				 kind==retvalbox )                       //v10
      {// keep focus on same box
          moveInside();
          didit = 1;
	    	  // // traceLeave();
          return this;       
      }

      // kinds that have a name and an inner list --- even if
      // don't allow move to name, still works, since part won't
      // ever be 'n'
      else if(kind == univbox  || kind==classbox ||
              kind==classdatabox || kind==classmethodsbox ||
              kind==instancedatabox || kind==instancemethodsbox ||
              kind==methodbox || kind==paramsbox || 
              kind==localsbox ||
              kind==memberbox || kind==opbox || kind==unopbox ||
              kind==assbox || kind==growbox ||
              kind==newopbox ||
              kind==classmethodbox || kind==instancemethodbox ||
              kind==instancebox || kind==gridbox 
							//v10
							|| kind==classdatacopybox
						)
      {

        if(part=='n')
        {
          moveInside();
          didit = 1;
      	  // // traceLeave();
          return this;
        }
        else
        {// move in to inner list

          // jb21:  compiler caught this warning part=='i' 
          part='i'; // make sure part is set correctly

          if(first==NULL || current==NULL)
          {// have no inners or cursor left on symbol0
           // so move in to be on symbol0 of inner list
           
            moveInside();
            didit = 1;
      		  // // traceLeave();
            return this;
          }
          else
          {// on an actual inner box
            curbox = current;
            if(curbox == NULL)
              errorexit("box::moveIn has bad current");
            didit = 1;
      		  // // traceLeave();
            return curbox;
          }
        }// move in to inner list

      }// kinds that have name and inner list

      // kinds that have a "0 or more" style
      // inner list, with a symbol0, no name
      else if( kind==seqbox || kind==branchbox 
			            || // jb19:  no name showing, anyway
							 kind==lessonbox 
						 )
      {

        // move in to inner list

        // jb19:  noticed terrible mistake--- part=='i'!
        part='i'; // make sure part is set correctly

        if(first==NULL || current==NULL)
        {// have no inners or cursor left on symbol0
         // so move in to be on symbol0 of inner list
           
          moveInside();
          didit = 1;
      	  // // traceLeave();
          return this;
        }
        else
        {// on an actual inner box
          curbox = current;
          if(curbox == NULL)
            errorexit("box::moveIn has bad current");
          didit = 1;
      	  // // traceLeave();
          return curbox;
        }
      
      }// kinds with 0 or more inner list, with symbol0

      // kinds that have fixed size inner list (or
      //  just don't have symbol0 possibility), and that's all
      else if( kind==dobox || kind==whilebox || kind==forbox ||
               kind==callbox || kind==arraybox || kind==array2dbox ||
               kind==returnbox ||
							 kind==javabox  // jb16
                 ||
               kind==notebox // jb21
						 )
      {
        curbox = current;
        if(curbox == NULL)
          errorexit("bad current in just inner list cases");
        didit = 1;
        // // traceLeave();
        return curbox;
      }

      // kinds that only allow move in to an inner, and 
      // can't move in if has no inners
      else if( kind==stackbox || kind==heapbox ||
			         // v10
 							 kind==allclassdatabox 
             )
      {
        if( cursor == 0 )
        {// no inners, don't move in
          didit = 0;
      	  // // traceLeave();
          return this;
        }
        else
        {
          didit = 1;
       	  // // traceLeave();
          return current;
        }
      }

      else if(kind==emptybox ||
              kind==consolebox || kind==valuebox ||
							kind==portsbox || kind==portbox )
      {// can't move in, has no inside, really, or not allowed
        // to mess with it
        didit = 0;
        // // traceLeave();
        return this;
      }

      else
      {
        errorexit("unhandled kind in box::moveIn");
        // // traceLeave();
        return NULL;
      }

    }// full displaymodes

  } // moveIn

  // see if moving "up" makes sense, and do it,
  // return new focus, whether did it (for comment part now,
  // but may want for 2D arrays on heap)
  box* box::moveUpDown( char dirn, int& didit)
  {
	  // traceEnter( tid, "box:moveUpDown" );

    // jb16:  allow on 'a' aspect of javabox, too
		// jb19:  also on 'a' aspect of combox
    // jb21:    and parbox and presbox
    if(isInside() && aspect=='d' )
    {// focus inside comment part, do string move up/down
      doc->upDownCursor( 1, dirn, 1 );  
      didit = 1;
		  // // traceLeave();
      return this;
    }
  
	  // jb16:  allow on 'a' aspect of javabox, too
    // jb21:   had combox from jb19 without comment, add in
    //         parbox, presbox
    else if(isInside() && aspect=='a' && 
		          (kind==javabox || kind==combox ||
               kind==parbox || kind==presbox
              ) 
           )
    {// focus inside name part, do string move up/down
      name->upDownCursor( 1, dirn, 1 );  
      didit = 1;
		  // // traceLeave();
      return this;
    }
    else
    {// for a while, are no other up/down moving possibilities
      didit = 0;

// kind zone ++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    later may allow moving up/down other than in doc aspect
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

		  // // traceLeave();
      return this;
    }
  }

  // similar to moveUpDown, except works on all strings,
  // not just multi-liners
  box* box::moveHomeEnd( char dirn, int& didit)
  {
	  // traceEnter( tid, "box:moveHomeEnd" );

    mystring* temp;

    if(isInside() && (aspect!='f' || part!='i'))
    {// focus inside a string part, do string move home/end
      temp = whichString();
	  if( aspect == 'd' )
        temp->homeEndCursor( 1, dirn, 1 ); 
	  else if( aspect == 'a' )
		temp->homeEndCursor( 2, dirn, 1 );
	  else
		temp->homeEndCursor( 0, dirn, 1 );
      didit = 1;
		  // // traceLeave();
      return this;
    }
    else
    {// on symbol0 or border

// kind zone ++++++++++++++++++++++++++++++++++++++++++++++++++++++
//      later may allow move home/end on other than strings
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

      // needs work!!!

      didit = 0;
		  // // traceLeave();
      return this;
    }
  }

  box* box::whoOuter()
  {
	  // traceEnter( tid, "box:whoOuter" );
	  // // traceLeave();
    return outer;
  }

  // set just my dye to the specified code --- dyeing doesn't
  // work inwardly recursively
  void box::setDye( int colorcode )
  {
	  // traceEnter( tid, "box:setDye" );
	  dye = colorcode;
	  // // traceLeave();
  }

  // set my color to colorcode, my depth to dpth, my dye to none,
  // and work recursively inward doing the same
  // (used when I am about to be spliced into reality, I'm either
  //  brand-new or a copy)
  //   but a few kinds don't accept their owner's colorcode,
  //   set them manually
  void box::setDepthAndColors( int colorcode, int dpth )
  {
	  // traceEnter( tid, "box:setDepthAndColors" );

    box* curbox;

  	// spiff up me
	  if( kind==classbox )              // special kinds force color
		  color = blueprintcolorcode;
  	else if( kind==stackbox )
	  	color = stackcolorcode;
	  else if( kind==heapbox )
  		color = heapcolorcode;
		else if( kind==allclassdatabox )  // v10
		  color = staticdatacolorcode;
	  else if( kind==consolebox )
		  color = consolecolorcode;
	  else
	      color = colorcode;       // most inherit from owner
	
	  dye = 0; depth = dpth+1;

	  curbox = first;
	  while( curbox != NULL )
	  {// spiff up curbox --- an inner of mine---to have my color
  	  // (different from colorcode, note!) and my depth plus 1
	    curbox->setDepthAndColors( color, depth+1 );
	    curbox = curbox->next;
	  }

	  // // traceLeave();

  }// box::setDepthAndColors

  // a little utility---do the mechanics of inserting
  //  newbox immediately after my current inner box
  void box::addInnerAtCursor(  box* newbox )
  {
     // have newbox---and its inners---receive my color,
	  // set dye to none, and build from my depth
 	  newbox->setDepthAndColors( color, depth );

    if(current == NULL)
    {// add newbox as the new first inner box
     // set external links here, newbox's other stuff (first,
     // current, cursor, etc.) set when created
     newbox->next = first;
     newbox->outer = this;  

     first = newbox;
     current = first;
     cursor = 1;
        
    }
    else
    {// add newbox after curbox
      newbox->next = current->next;
      newbox->outer = this;

      current->next = newbox;
      current = newbox;
      cursor++;
    }

    // either way, clean up after adding
		// jb16:  make this guy fix up stuff itself, like
		//        replaceInnerAt
    int junk;
    newbox->figureSizeLocs( junk );

  }// box::addInnerAtCursor

  // an even littler utility---do the mechanics of inserting
  //  newbox at front of me
  void box::addInnerAtFront(  box* newbox )
  {
	  // traceEnter( tid, "box:addInnerAtFront" );

    newbox->setDepthAndColors( color, depth );
   
    newbox->next = first;
    newbox->outer = this;  

    first = newbox;
    current = first;
    cursor = 1;        

    // jb16:  need this to keep things correct
    int junk;
    newbox->figureSizeLocs( junk );

    // traceLeave();

  } // addInnerAtFront

  // "scan" until hit a kind of box that demands cessation,
  // or a context in which an emptybox can be added, or landed
  // on

	// 6/27/2000:  key will be either space or escape,
	//  only difference is that in a literal string or char,
	//  space just inserts into the string quietly
	// 8/19/2000:  also want value part of databox to take space
  box* box::processSpace( int key, int& didit )
  {
	  // traceEnter( tid, "box:processSpace" );

    int knd;
    box* owner;
    box* curbox;
    box* newbox;

    // first, handle possible jumping--- at end of this,
    // either move internally and quit, or have set owner to
    // the "owner" of the original focus 

    // focus is inside my doc facet
    if( isInside() && aspect=='d' )
    {
		  if( key == ' ' )
			{// add the space
        // jb24: unless permission is violated:
        if( canAdd() )
        {
          doc->insertCharAt( doc->whereCursor()+1, ' ' );
          doc->advanceCursor();
          didit = 1;
        }
        else
        {
          didit = 0;
        }
        return this;
			}
			else
			{// escape key moves to outside
			  moveOutside(); 
			  didit = 1;
				return this;
			}
    } // add to my doc facet

    // jb16:  space inside name of javabox is just a space!
		// jb19:  combox acts like javabox
    // jb21:   add parbox, presbox
    // jb5a:  add lessonbox
    else if( isInside() && aspect=='a' && 
		         (kind==javabox || kind==combox ||
              kind==parbox || kind==presbox ||
              kind==lessonbox
             )
					 )
    {
		  if( key == ' ' )
			{// add the space
        // jb24:  unless permission is not given
        if( canAdd() )
        {
          name->insertCharAt( name->whereCursor()+1, ' ' );
          name->advanceCursor();
          didit = 1;
        }
        else
          didit = 0;
        return this;
			}
			else
			{// escape key moves to outside
			  moveOutside(); 
			  didit = 1;
				return this;
			}
    } // add to my abstract aspect

    else if( isInside() && (aspect=='a' || part=='n' ) )
    {    
// kind zone +++++++++++++++++++++++++++++++++++++++++++++++++++++
//    handle all jump possibilities from a space key when 
//     inside name 
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

      // 6/27/2000:  when focus is on name part of stringbox or
			//  charbox and hit space
      if( aspect == 'f' && (kind==stringbox || kind==charbox) &&
			    key == ' ' )
			{// just add the space to the literal, otherwise will behave
			 // as before
        // jb24: if have permission to add
        if( canAdd() )
        {
  			  name->insertCharAt( name->whereCursor()+1, ' ' );
	  			name->advanceCursor();
		  		didit = 1;
        }
        else
          didit = 0;
				return this; 
			}
			                                        //v10
      else if( aspect=='f' && (kind==databox || kind==retvalbox) )
      {// jump name to type
        didit = 1;
        setPart('t');
        return this;
      }
      else if( aspect=='f' && 
               (kind==univbox ||
                  kind==classdatabox || kind==classmethodsbox ||
                  kind==instancedatabox || kind==instancemethodsbox ||
                  kind==paramsbox || 
                  kind==localsbox
               )
             )          
      {// jump name to inner list and add a specific kind of
        // box (depending on my kind) after cursor
        didit = 1;
        setPart('i');
        owner = this;  // box to add to inner list of
      }
      else if( aspect=='f' &&
                ( kind==opbox || kind==unopbox ||
                  kind==assbox || kind==growbox ||
                  kind==newopbox
                )
             )
      {// move from name to first inner box
        didit = 1;
        setPart('i');
        return first;  
      }

      else if( aspect=='f' &&
               kind==memberbox
             )
      {// name is at the end of the entry sequence,
        // move out
        owner = outer;  // box to scan on from
        moveOutside();
        owner->moveOutside();
      }

      else // kind of box that has a name, but is nowhere within
             // this box to
             // jump to from the name, or on abstract aspect,
             // so try to move on
      {
        if( kind==univbox )
        {// has no outer, so done
          didit = 0;
          return this;
        }
        else
        {// prepare to scan on later
          owner = outer;  // box to scan on from
          moveOutside();
          owner->moveOutside();
        }

      }

    } // inside on abstract aspect or name part

    else if( isInside() && part=='t' )
    {// on type part --- 

// kind zone +++++++++++++++++++++++++++++++++++++++++++++++++++++++
//            handle space for inside type cases
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      
      if( kind==databox )  
      {
          didit = 1;
          setPart('v');
          return this;
      } // databox

			else if( kind==retvalbox )
			{// leave it on type
        didit = 0;
    	  // traceLeave();
        return this;
			}

      else
        errorexit("no other kinds with types implemented yet");

    } // inside on type part

    else if( isInside() && part=='v' )
    {// inside on value part

// kind zone +++++++++++++++++++++++++++++++++++++++++++++++++++++++
//            handle space for inside value cases
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

      if( kind==databox )
      {// add a box in my outer, after me since I'm current,
			 // except in special case that type is string, make space
			 // actually insert a space
			  if( type->equals("string") && key == ' ' )
				{
          // jb24: if have permission to add:
          if( canAdd() )
          {
  			    value->insertCharAt( value->whereCursor()+1, ' ' );
	  			  value->advanceCursor();
		  		  didit = 1;
          }
          else
            didit = 0;
				  return this; 
				}
				else
				{
          moveOutside();
          owner = outer;
				}

      }// inside value on databox

      else
        errorexit("no other kinds with values implemented yet");

    }// inside value part

    else if( isInside() && part=='i' )
    {// inside on inner list---hence symbol0, or I wouldn't
      // have been the focus
      owner = this;
    }// on symbol0

    else if ( isInside() )
      errorexit("unhandled inside case in processSpace");

    else
    {// started outside
      if( kind != univbox )
      {
        owner = outer;
        owner->moveOutside();
      }
      else
      {// silly special case
        didit = 0;
        return this;
      }
    }

    // if still here, owner becomes starting point for
    // scan, looking for a way to move on

    curbox = owner;

    // move curbox along until hit emptybox, context where
    // can add, or a search-cutoff kind of box

    newbox = NULL;
    int newknd = 0;
    int inlevel;

    while( newknd == 0 )           
    {// try to move on --- newbox will become the focus

      knd = curbox->kind;

      // some owners cause halt, don't move because was kind
      // of a silly request
      if( knd==classbox || knd==methodbox ||
			    knd==javabox  // jb16: keep it simple when sitting on
					     // inner box of a javabox, because wasn't built
							 // interactively
				)
      {
        didit = 0;
        return this;
      }

      // very special case---if owner is univ, back up as needed
      // until cursor is on symbol0 or a classbox,
      // then add classbox
			//   jb19:  noticed the comment above is wrong---actually,
			//          adds a classbox only if on symbol0 of univbox
			//          or on a classbox;
			//          Now change so if sitting on portsbox or lessonbox,
			//          adds a lessonbox after
      else if( knd==univbox )
      {
        box* tempbox;
        if( curbox->cursor == 0 )
        {// sitting on symbol0 so fine
          newknd=classbox; inlevel=2;
		    }
        else
        {// current is on an actual box, be happy with
          // moving there, if has wrong kind, or add after
          tempbox = curbox->current;
          if( tempbox->kind == classbox )
          {// happy to add after
            newknd=classbox; inlevel=2;
          }
					else if( lesson->isAuthor() &&  // jb23
                     (tempbox->kind == portsbox ||
					            tempbox->kind == lessonbox )
								 )
					{// add lessonbox after
					  newknd=lessonbox; inlevel=2;
					}
					else
          {// just land on the current non-classbox
            didit = 1;
            return curbox->current;
          }
          
        }// on an inner box
      }// univbox

      // some owners simply add to their inner list
      else if(knd==classdatabox || knd==instancedatabox ||
              knd==paramsbox || knd==localsbox )
      { newknd=databox; inlevel=2;}
      else if(knd==classmethodsbox || knd==instancemethodsbox )
      { newknd=methodbox; inlevel=2;}
      // jb21:  put lessonbox here
      else if(knd==seqbox || knd==callbox 
                 || knd==lessonbox
		         )
      { newknd=emptybox; inlevel=1;}

      /* jb21:  now have several inner kinds in lesson box,
         put lessonbox back like seqbox --- adds empty
			else if(knd==lessonbox)  // jb19: add combox to inner
			{ newknd=combox; inlevel=1;} 
      */
      
      // branchbox is special:  doesn't allow add if curbox's
      //  current is last, if do add, add a pair if on second of
      //  pair, else move to second
      else if(knd==branchbox)
      {// special case:  add 2 new emptyboxes to curbox, etc...

        // find out about the current box inside curbox
        newbox = curbox->current;
        
        // avoid crash by short-circuit evaluation when 
        //  curbox is on its symbol0
        if( curbox->cursor > 0 && newbox->next == NULL )  
        {// on last inner in curbox, refuse to add after it,
          // move outward instead and continue scan

          if( curbox->outer == NULL )
          {
            didit = 0;
            return this;
          }
          else
          {
            curbox = curbox->outer;
            curbox->moveOutside();
          }

        }
        else if( curbox->cursor % 2 == 1 && curbox->cursor != 0 )
        {// on first of pair, move to next
          newbox = newbox->next;  // second guy in pair
          didit = 1;
          curbox->cursor++;
          curbox->current = newbox;
          return newbox;
        }
        else
        {// add a pair --- maybe immediately after symbol0
          // jb24: if have permission to add to curbox
          if( curbox->canAdd() )
          {
            box* prevbox;
            newbox = new box();         // actually build newbox
            prevbox = newbox;  // note this box to return to
            newbox->build( emptybox );
            curbox->addInnerAtCursor( newbox );
            newbox = new box();
            newbox->build( emptybox );
            curbox->addInnerAtCursor( newbox );
            // move back to first box added
            curbox->cursor--;  curbox->current = prevbox;
            didit = 1;
            return prevbox;  // first box added becomes the focus
          }
          else
          {
            didit = 0;
            return curbox;
          }
        }// add a pair

      }

      else if(knd==memberbox)
      {// guaranteed to be able to jump ahead to the name part
        curbox->setPart('n');
        curbox->moveInside();
        didit = 1;
        return curbox;
      }

      // some owners try to move to next inner
      else if(knd==arraybox || knd==array2dbox ||
              knd==opbox || knd==assbox ||
              knd==newopbox || 
              knd==whilebox || knd==dobox || knd==forbox
              // jb21:  put notebox here, just for consistency
                || knd==notebox
             )
      {// try to advance to next inner---if can't, do same
        // as move outer below

        // note:  these guys must have a current, have
        //         fixed structure

        newbox = curbox->current;
        newbox = newbox->next;
        if(newbox==NULL)
        {// can't step ahead, so do outer move---same as below
          if( curbox->outer == NULL )
          {
            didit = 0;
            return this;
          }
          else
          {
            curbox = curbox->outer;
            curbox->moveOutside();
          }
        }
        else
        {// step ahead, making sure curbox is kept correct, and quit
          didit = 1;
          curbox->cursor++;
          curbox->current = newbox;
          return newbox;
        }

      }

      // otherwise, move outer and try again
      else
      {// move outer
        if( curbox->outer == NULL )
        {
          didit = 0;
          return this;
        }
        else
        {
          curbox = curbox->outer;
          curbox->moveOutside();
        }
      }

    }// loop to scan for appropriate context

    // if still here, add new box
    // jb24: unless not permitted to add
    if( curbox->canAdd() )
    {
      newbox = new box();         // actually build newbox
      newbox->build( newknd );

      curbox->addInnerAtCursor( newbox );

      if(inlevel==1)              // adjust apparent focus
      {
        didit=1;
        newbox->moveOutside();  // in case "build" left it inside
        return newbox;
      }
      else if(inlevel==2)
      {// put focus inside newbox on its name
        didit = 1;
        newbox->setPart('n');
        newbox->moveInside();
        return newbox;
      }
      else
      {
        errorexit("illegal inlevel processSpace");
        return this;
      }
    }// jb24: permitted to add
    else
    {// not permitted to add,
      didit = 0;
      return curbox;
    }

  }// end of processSpace

  // I hold the focus, receive a printable symbol sym,
  //  (includes specialspacekey)
  // must completely handle what should be done:
  //
  //    first, take care of all the really free-form cases,
  //    where any symbol is treated the same, namely 
  //    editing the doc
  //
  //    then, since focus is on me, handle what to do if
  //     apparent focus is on one of my string parts
  //      --- these have restrictions, and, depending on
  //      kind, space can cause a jump 
  //
  //    apparent focus might be on my inner list's symbol0,
  //    or on my border, have to consider my
  //     owner's kind (where I am my symbol0's owner, to combine
  //     the two cases), and other factors, to determine what
  //     to do
   box* box::processPrintable( int sym, int& didit )
  {
    int knd;
    int slot;
    box* owner = NULL;  // to signal whether have it

    // jb24:  not permitted to add a symbol to a string part
    //  (trying to catch all string adds in one shot
    if( !canAdd() && isInside() && part != 'i' )
    {
      didit = 0;
      return this;
    }
    
    // focus is inside my doc facet
    if( isInside() && aspect=='d' )
    {
      if( sym == specialspacekey )
        sym = ' ';  // space is accepted here as well as in value
    	else if( sym == tabkey )
		    sym = tabsymbolcode;
      doc->insertCharAt( doc->whereCursor()+1, sym );
      doc->advanceCursor();
      didit = 1;
      return this;
    } // add to my doc facet

    // jb16:  javabox name can have any symbol a doc can
		// jb19:  so can a combox
    // jb21:  and parbox and presbox
    // jb5a:  and a lessonbox
    else if( isInside() && aspect=='a' && 
		         (kind==javabox || kind==combox ||
              kind==parbox || kind==presbox ||
              (kind==lessonbox && sym!=13)
             ) 
					 )
    {
      if( sym == specialspacekey )
        sym = ' ';  // space is accepted here
	  	else if( sym == tabkey )
	      sym = tabsymbolcode;
      name->insertCharAt( name->whereCursor()+1, sym );
      name->advanceCursor();
      didit = 1;
      return this;
    } // add to my name

    else if( isInside() && (aspect=='a' || part=='n' ) )
    {// on name string, either add, reject, or jump
      if( (kind != opbox && kind !=unopbox && 
           kind != assbox && kind != growbox &&
            legalinname( sym ) )
            ||
          (literalkind(kind) && sym!=' ') // very permissive
            ||
          (kind==opbox && legalinoperator( sym ) )
            ||
          (kind==unopbox && (sym=='-' || sym=='!')) 
            ||
          (kind==assbox && (sym=='=' || sym=='+' || sym=='-' ||
                            sym=='*' || sym=='/' || sym=='%'))
            ||
          (kind==growbox && (sym=='+' || sym=='-'))
                  
        )
      {// is allowed in name, so add it
        if(sym==specialspacekey)
          sym=' ';
        name->insertCharAt( name->whereCursor()+1, sym );
        name->advanceCursor();
        didit = 1;
        return this;
      }
      else // inside on name, ridiculous symbol
      {// reject the symbol
        didit = 0;
        return this;
      }
    } // inside on abstract aspect or name part

    else if( isInside() && part=='t' )
    {// on type part --- allow different symbol sets depending
      // on my kind:

// kind zone +++++++++++++++++++++++++++++++++++++++++++++++++++++++
//            handle printable for inside type cases
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      
      if( kind==databox || kind==retvalbox )     //v10
      {// accept legalinname plus [ ---at execution, will complain
        // if have other than [ or [[ on end, but don't try to
        // enforce here
        // and space jumps to value part

        if( legalinname(sym)
                 || 
            sym=='[' 
          )
        {// insert sym in the string
          type->insertCharAt( type->whereCursor()+1, sym );
          type->advanceCursor();
          didit = 1;
          return this;
        }
        else
        {// reject the symbol
          didit = 0;
          return this;
        }
      } // databox
      else
        errorexit("no other kinds with types implemented yet");

    } // inside on type part

    else if( isInside() && part=='v' )
    {// inside on value part

// kind zone +++++++++++++++++++++++++++++++++++++++++++++++++++++++
//            handle printable for inside value cases
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

      if( kind==databox )
      {// accept any non-blank
          if( sym==specialspacekey ) // set esc back to space
            sym = ' ';
          value->insertCharAt( value->whereCursor()+1, sym );
          value->advanceCursor();
          didit = 1;
          return this;
      }// inside value on databox

      else
        errorexit("no other kinds with values implemented yet");

    }// inside value part

    else if( isInside() && part=='i' )
    {// inside on inner list---hence symbol0, or I wouldn't
      // have been the focus

      owner = this;  // pass on info 
    
    }// on symbol0

    else if ( isInside() )
      errorexit("unhandled inside case in processPrintable");

    // -------------------------------------------------------
    //  if get here, have owner, or were not inside

    if( !isInside() )
    {// started on my border, or moved out to it,
      // process directly or get my owner
      // to allow processing below

      // jb21:  when not inside the focus box, allow 
      //        various special printables to do lesson work
      //  authorVersion is the global that turns off author features
      //   and makes a student/reader version of the code
      if( sym == authorizeKey && authorVersion )
      {// toggle authorization
        lesson->toggleAuthority();
        didit = 1;  // to draw new border filling
        return this;
      }

      // jb24:  put in ability to change permissions interactively
      else if( lesson->isAuthor() &&
               ( sym==')' || sym=='!' || sym=='@' || sym=='#' ||
                 sym=='$' || sym=='%' || sym=='^' || sym=='&'
               )
             )
      {// set permission on just this box
        setPerm( sym );
        didit = 1;
        return this;
      }
      else if( '0' <= sym && sym <= '7' &&
                lesson->isAuthor()
             )
      {// set permission on this box and recursively inward
        setPermInward( sym );
        didit = 1;
        return this;
      }

      // jb24:  any remaining processing is shut off if can't add
      if( !canAdd() )
      {
        didit = 0;
        return this;
      }

      // need owner whether checking for conversion or end up
      // going on
      if( outer != NULL )
        owner = outer;
      else
      {
        didit = 0;
        return this;
      }

      // if is emptybox, try for conversion
      if( kind==emptybox )
      {// printable on border of emptybox is request for conversion

// kind zone +++++++++++++++++++++++++++++++++++++++++++++++++++++
//       depending on sym, owner kind, 
//       convert me to a new box of specific kind
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        int newknd = 0;
        int inlevel = 0;

        int oknd = owner->kind;        // owner's kind
        slot = positionInInnerList(); // emptybox's (my) position
                                          // in owner

        // go through all cases and set newknd to convert to,
        // and how to set the apparent focus

        // jb16:  if this emptybox has owner that is a javabox,
				//        refuse all conversions
				if( oknd == javabox )
				{
          didit = 0;
    		  // traceLeave();
          return this;
				}

        // situations where an action box is allowed

				// jb16:  first check for 'j', it's allowed always
				if( sym=='j' )
				{ newknd=javabox; inlevel=2;}

        else if( actionContext( oknd, slot ) )
        {// now, depending on sym, maybe convert to action box       
          if( sym=='s' )
          { newknd = seqbox; inlevel = 3;}
          else if( sym=='d')
          { newknd=dobox; inlevel=4;}
          else if(sym=='w')
          { newknd=whilebox; inlevel=4;}
          else if(sym=='f')
          { newknd=forbox; inlevel=4;}
          else if(sym=='c')
          { newknd=callbox; inlevel=4;}
          else if(sym=='a')
          { newknd=assbox; inlevel=5;}
          else if(sym=='g')
          { newknd=growbox; inlevel=2;}
          else if(sym=='r')
          { newknd=returnbox; inlevel=4;}
          else if(sym=='b')
          { newknd=branchbox; inlevel=3;}

        }

        // situations where a value box (includes "ref") is allowed
        else if( (oknd==dobox && slot==2) ||
                 (oknd==whilebox && slot==1) ||
                 (oknd==forbox && (slot==2) ) ||
                 (oknd==callbox && slot>1) ||
                 (oknd==arraybox && slot==2) ||
                 (oknd==array2dbox && slot>=2) ||
                 (oknd==opbox) ||
                 (oknd==unopbox) ||
                 (oknd==assbox && slot==2) ||
                 (oknd==returnbox) ||
                 (oknd==branchbox && (slot%2!=0) && next != NULL) ||
                 (oknd==newopbox)
               )
        {// depending on sym, convert emptybox to specified value box
          if(sym=='i')
          { newknd=idbox; inlevel=2;}
/* deprecated literalbox
          else if(sym=='l')
          { newknd=literalbox; inlevel=2;}
*/
          else if(sym=='I')
					{ newknd=intbox; inlevel=2;}
					else if(sym=='F')
					{ newknd = floatbox; inlevel=2;}
					else if(sym=='C')
					{ newknd = charbox; inlevel=2;}
					else if(sym=='B')
					{ newknd = booleanbox; inlevel=2;}
					else if(sym=='S')
					{ newknd = stringbox; inlevel=2;}
          else if(sym=='.')
          { newknd=memberbox; inlevel=4;}
          else if( sym=='c')
          { newknd=callbox; inlevel=4;}
          else if(sym=='[')
          { newknd=arraybox; inlevel=4;}
          else if(sym==',')
          { newknd=array2dbox; inlevel=4;}
          else if(sym=='o')
          { newknd=opbox; inlevel=2;}
          else if(sym=='u')
          { newknd=unopbox; inlevel=2;}
          else if(sym=='n')
          { newknd=newopbox; inlevel=2;}
                 
        }

        // situations where a reference box is allowed, but
        // other kinds of value boxes are not 
        // (literal, op, unop)
        else if( 
                 (slot==1 &&
                 ( // v10:  oknd==callbox removed, handled separately
								             // below
								   oknd==memberbox ||
                   oknd==arraybox || oknd==array2dbox  ||
                   oknd==assbox || oknd==growbox )                                  
                 )
               )
        {// depending on sym, convert emptybox to spec. ref box
          if(sym=='i')
          { newknd=idbox; inlevel=2;}
          else if(sym=='c')
          { newknd=callbox; inlevel=4;}
          else if(sym=='.')
          { newknd=memberbox; inlevel=4;}
          else if(sym=='[')
          { newknd=arraybox; inlevel=4;}
          else if(sym==',')
          { newknd=array2dbox; inlevel=4;}

        }

        else if( slot==1 && oknd==callbox )
        {// depending on sym, convert emptybox to id or member
				  if(sym=='i')
          { newknd=idbox; inlevel=2;}
          else if(sym=='.')
          { newknd=memberbox; inlevel=4;}

        }

        // jb21: convert emptybox inside lessonbox to ...
        else if( oknd==lessonbox )
        {
          if( sym == 'p' )
          { newknd=presbox; inlevel=2;}
          else if( sym == 'c' )
          { newknd=combox; inlevel=2;}
          else if( sym == 'n' )
          { newknd=notebox; inlevel=4;}
        }

        // jb21:  I was confused with this comment!      
        // will grow hugely with all the codebox cases later

        // else will go on below

        if( newknd != 0 )
        {// convert and quit
          didit = 1;

          build( newknd );  // re-build me as desired kind
                            // no need to deallocate resources,
                            // name and doc are only ones might
                            // have, want to keep them (well, not
                            // name, but won't have one anyway)

          // adjust the focus
          if(inlevel==1)
          {// leave focus on border
            moveOutside();
      		  // traceLeave();
            return this;
          }
          else if(inlevel==2)
          {// focus on name 
            setPart('n');
            moveInside();
      		  // traceLeave();
            return this;
          }
          else if(inlevel==3)
          {// put focus on symbol0
            setPart('i');
            cursor = 0; current = NULL;
            moveInside();
      		  // traceLeave();
            return this;
          }
          else if(inlevel==4)
          {// move focus to first child
            fixupReality(0);  // have to warn boxes outer to me,
                             // before moving focus in
            setPart('i');
            cursor = 1; current = first;
            moveInside();
      		  // traceLeave();
            return first;
          }
          else if(inlevel==5)
          {// special work for assbox---add = to name, put 
            // focus on string symbol0
            name->insertCharAt( 1, '=' );
            name->setCursor( 0 );
            setPart('n');
            moveInside();
      		  // traceLeave();
            return this;
          }
          else
            errorexit("illegal inlevel in emptybox conversion");
          
      	  // traceLeave();
          return this;

        } // hit a legal conversion

      }// convert emptybox?

    }// were on my border, needed to do some more work

    // now, for sure have owner and need to add appropriate
    // new box, depending on kind of owner and sym,
    // after owner's current

    knd = owner->kind;

// kind zone +++++++++++++++++++++++++++++++++++++++++++++++++++++
//      depending on sym and owner kind, maybe add after the 
//      current inner box in owner
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // note:  build the new guy to be added in newbox,
    //        do "after the current" insertion later

    box* newbox;
    int newknd = 0;
    int inlevel = 0;  // in various cases set how to move in
                      // including variety of possible features
                      //   1-->in to part 'i'
                      //   2-->in to part 'n'

    slot = positionInInnerList(); // my position so can decide if
                                  // adding certain kinds after me
                                  // is allowed
    
    // situations where the various keys are allowed to produce 
    // the various action boxes

    if( knd==seqbox )
    {// depending on sym, maybe add specified action boxes
      if(sym=='s')
      { newknd = seqbox; inlevel = 3;}
      else if(sym=='d')
      { newknd=dobox; inlevel=4;}
      else if(sym=='w')
      { newknd=whilebox; inlevel=4;}
      else if(sym=='f')
      { newknd=forbox; inlevel=4;}
      else if(sym=='c')
      { newknd=callbox; inlevel=4;}
      else if(sym=='a')
      { newknd=assbox; inlevel=5;}
      else if(sym=='g')
      { newknd=growbox; inlevel=2;}
      else if(sym=='r')
      { newknd=returnbox; inlevel=4;}
      else if(sym=='b')
      { newknd=branchbox; inlevel=3;}
			else if(sym=='j')
			{ newknd=javabox; inlevel=2;}

    }

    // situations where various value boxes can be added
    else if( knd==callbox )
    {// add an argument to a method call
      if(sym=='i')
      { newknd=idbox; inlevel=2;}
/* deprecated literalbox
      else if(sym=='l')
      { newknd=literalbox; inlevel=2;}
*/
      else if(sym=='I')
			{ newknd=intbox; inlevel=2;}
      else if(sym=='F')
			{ newknd=floatbox; inlevel=2;}
      else if(sym=='C')
			{ newknd=charbox; inlevel=2;}
      else if(sym=='B')
			{ newknd=booleanbox; inlevel=2;}
      else if(sym=='S')
			{ newknd=stringbox; inlevel=2;}
      else if( sym=='c')
      { newknd=callbox; inlevel=4;}
      else if( sym=='.')
      { newknd=memberbox; inlevel=4;}
      else if(sym=='[')
      { newknd=arraybox; inlevel=4;}
      else if(sym==',')
      { newknd=array2dbox; inlevel=4;}
      else if(sym=='o')
      { newknd=opbox; inlevel=2;}
      else if(sym=='u')
      { newknd=unopbox; inlevel=2;}
      else if(sym=='n')
      { newknd=newopbox; inlevel=2;}
			else if(sym=='j')
			{ newknd=javabox; inlevel=2;}

    }

    // jb21:  lessonbox can have new guys created when hit keys
    else if( knd == lessonbox )
    {
      if(sym=='c')
      { newknd = combox; inlevel = 2;}
      else if(sym=='n')
      { newknd=notebox; inlevel=4;}
      else if(sym=='p')
      { newknd=presbox; inlevel=2;}
    }
        
    // jb21:  hmmm, seemed really confused here, too
    // will grow hugely with all the codebox possibilities later
    //  (but should be straightforward!)

    // add in newbox, if there is one

    if( newknd == 0 )
    {// sym not correct to add, so just ignore
        didit = 0;
  		  // traceLeave();
        return this;
    }
    else
    {// build and add newbox after owner's current, 
     // make it the focus

      didit = 1;

      newbox = new box();         // actually build newbox
      newbox->build( newknd );
      // insert newbox:
      owner->addInnerAtCursor( newbox );

      // adjust the focus
      if(inlevel==1)
      {// leave focus on border of newbox
        newbox->moveOutside();
  		  // traceLeave();
        return newbox;
      }
      else if(inlevel==2)
      {// focus on name 
        newbox->setPart('n');
        newbox->moveInside();
  		  // traceLeave();
        return newbox;
      }
      else if(inlevel==3)
      {// put focus on symbol0
        newbox->setPart('i');
        newbox->cursor = 0; newbox->current = NULL;
        newbox->moveInside();
  		  // traceLeave();
        return newbox;
      }
      else if(inlevel==4)
      {// move focus to first child
        newbox->fixupReality(0);  // have to warn boxes outer to me,
                             // before moving focus in
        newbox->setPart('i');
        newbox->cursor = 1; newbox->current = newbox->first;
        newbox->moveInside();
  		  // traceLeave();
        return newbox->first;
      }
      else if(inlevel==5)
      {// special work for assbox---add = to name, put 
       // focus on string symbol0
        (newbox->name)->insertCharAt( 1, '=' );
        (newbox->name)->setCursor( 0 );
        newbox->setPart('n');
        newbox->moveInside();
        int junk;
        newbox->figureSizeLocs( junk );
  		  // traceLeave();
        return newbox;
      }
      else
      {
        errorexit("illegal inlevel in adding a new box");   
  		  // traceLeave();
        return this;
      }

    }// build and add newbox

  } // processPrintable

  // remove this box from reality, or a symbol inside this
  // box near the cursor
  box* box::removeFromInner(int key, int& didit)
  {
    // jb24:  if can't kill, just leave
    if( ! canKill() )
    {
      didit = 0;
      return this;
    }

    // first, decide whether need to remove a symbol, or
    // a box
    if( isInside() && 
        ( aspect=='a' || aspect=='d' || 
          part=='n' || part=='t' || part=='v' 
        )
      )
    {// remove a symbol, possibly

      mystring *temp = whichString();

      if(key==backspacekey)
      {// kill the guy we're on, make cursor the previous
        if(temp->whereCursor() > 0)
        {// not on symbol 0, so kill it
         temp->deleteCharAt( temp->whereCursor() );
         temp->retreatCursor();
         didit = 1;
         return this;  // focus stays on same box
        }
        else
        {
          didit = 0;
          return this;
        }
      }
      else if(key==killkey)
      {// kill the guy under the cursor,
       // put cursor on next 
        if(temp->whereCursor()>0)
        {
          temp->deleteCharAt( temp->whereCursor() );
          if(temp->whereCursor() > temp->length() )
            temp->retreatCursor();
          didit = 1;
          return this;  // focus stays on same box
        }
        else
        {
          didit = 0;
          return this;
        }
      } 
      else
      {
        errorexit("illegal key choice in box::removeFromInner");
        return this;
      }
    
    }// remove a symbol

    else if ( isInside() )
    {// focus is inside me, but in a way that can't be
      // removed --- symbol0 of my inner list, for example
      didit = 0;
      return this;
    }

    else // focus is on me
    {// may remove myself from reality
      // need to check legality of killing me, possibly refuse
      // If do kill me, must deallocate heap resources

// kind zone +++++++++++++++++++++++++++++++++++++++++++++++++++++++
//       remove from inner
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

      if(kind==univbox)
      {// has no owner, can't be removed
        didit = 0;
        // traceLeave();
        return this;
      }
      else
      {// has owner, use owner info and my info to decide whether
         // deletion is allowed
        box* owner;
        box* newfocus;
        box* prevbox;
        box* curbox;
        box* nextbox;

        owner = outer;

// kind zone +++++++++++++++++++++++++++++++++++++++++++++++++++++++
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

        // standard removal of me is allowed
        // (vs. converting me to an emptybox or not allowed
        //  at all)

        int oknd = owner->kind;  // because use a lot below

        // jb19:  add lessonbox as kind can remove and can remove
				//        inner from
        if( kind==classbox || kind==databox || kind==methodbox ||
				    kind==lessonbox ||
            (oknd==seqbox) ||  // can remove any inner from seqbox
						(oknd==lessonbox) ||
            (oknd==callbox && this != owner->first)         
          )
        {// standard removal

          // grab previous to me
          prevbox = owner->accessInnerAt( owner->cursor - 1 );

          if(prevbox==NULL)
          {// is no previous to me
            owner->first = next;
            owner->current = next;
            newfocus = owner->current;

            if(newfocus==NULL)
            {// deleting first and only inner box,
              // set owner correctly
              owner->cursor = 0;
              owner->setPart( 'i' );  // probably already on innerlist
              owner->inside = 'y'; 
              newfocus = owner;
            }
            
          }
          else
          {// have a previous
            prevbox->next = next;
            owner->current = next;
            newfocus = owner->current;

            if(newfocus==NULL)
            {// deleting last inner box, current must become
              // the previous
              owner->current = prevbox;
              owner->cursor -= 1;
              newfocus = owner->current;
            }

          }
          
          // deallocate my innards and my space
          destroy();
          
          didit = 1;
          return newfocus;

        }// standard removal

        // kill by converting to emptybox
        else if( oknd==dobox || oknd==whilebox || oknd==forbox ||
                 (oknd==callbox && this == owner->first) ||
                 oknd==memberbox || oknd==arraybox ||
                 oknd==array2dbox || oknd==opbox || oknd==unopbox ||
                 oknd==assbox || oknd==growbox || oknd==returnbox ||
                 oknd==newopbox ||
								 oknd==javabox   // jb16: can convert single inner
								                 // of a javabox to emptybox
               )
        {// convert me to emptybox --- do in place to avoid 
          // worrying about rebuilding pointers and all --- only
          // issue is tossing now unused resources
          //  Keep doc

          // deallocate all guys not used by emptybox
          coredestroy();

          if(name != NULL )  // okay to do directly---can't
          {                  // be a kind with protected name
            name->destroy();
            name = NULL;
          }

         // old approach:
          //  kind = emptybox;  // do the conversion trivially!

          build( emptybox );

          didit = 1;
          return this;

        }// kill by converting to emptybox

        // special kill rules if owner is a branchbox
        else if(oknd==branchbox)
        {// remove the pair, or wipe the default guy to empty
          
          if( next == NULL )
          {// I'm the default last inner, so wipe me to empty
          // deallocate all guys not used by emptybox
            coredestroy();
            if(name != NULL )  // okay to do directly---can't
            {                  // be a kind with protected name
              name->destroy();
              name = NULL;
            }

            build( emptybox );

            didit = 1;
            return this;
          }
          else
          {// I'm not last, so delete my pair

            // find out my index in branchbox owner
            int slot = positionInInnerList();          

            // get triple of relevant boxes
            if( slot % 2 == 1 )
            {// I'm an odd guy, hence first in pair
              curbox = this;
              nextbox = next;
              prevbox = owner->accessInnerAt( owner->cursor - 1 );
            }
            else
            {// I'm second in my pair
              curbox = owner->first; // see if happens to be first
                                     // in my pair
              if( curbox->next == this )
              {// I'm second guy in first pair
                prevbox = NULL;
              }
              else
              {// are two guys before me
                prevbox = owner->accessInnerAt( owner->cursor - 2 );
                curbox = prevbox->next;
              }

              nextbox = this;
              // jb24:  had this here, now put it below, after
              //        know that the removal of the pair will occur
              //   owner->cursor--;  
            }
          
            // now, delete curbox and nextbox, knowing that
            // the newfocus will be after nextbox, since there
            // is sure to be such a box

                // jb24:  both boxes must be removeable.  Wouldn't
                //        be here unless one of them was removeable,
                //    worried that the other isn't
                if( ! curbox->canKill() || ! nextbox->canKill() )
                {
                  didit = 0;
                  return this;
                }

            // jb24:  now move cursor back if need to
            if( slot % 2 == 0 )
              owner->cursor--;

            newfocus = nextbox->next;

            if(prevbox==NULL)
            {// is no previous to curbox, just hop owner ahead
              owner->first = nextbox->next;
              owner->current = owner->first;
            }
            else
            {// have a previous
              prevbox->next = newfocus;
              owner->current = newfocus;
            }
          
            // deallocate resources for both curbox and nextbox
            curbox->destroy();
            nextbox->destroy();

            didit = 1;
            return newfocus;       
          }// not last

        }// special kill in branchbox

        else
        {// removal disallowed
          didit = 0;
          // traceLeave();
          return this;
        }

      }// maybe deletion allowed

    }// end of remove me from reality

  } // end of removeFromInner

  // wipe the current aspect of this box, only
	// works on any doc aspect and certain kinds of full aspects
	// (for example, can't wipe full aspect of a class box)
	box* box::wipeAspect(int& didit)
  {
    // jb24:  disallow wiping if can't kill
    if( ! canKill() )
    {
      didit = 0;
      return this;
    }

    didit = 0;// be pessimistic, set to 1 if turn out to really do it

    if( aspect == 'd' )
		{// can erase the contents of any doc aspect
		  didit = 1;

      // simply destroy doc aspect
      if( doc != NULL )
			  doc->destroy();

			doc = new mystring("");

			return this;

		}

		else if( aspect == 'f' )
		{// can wipe most full aspects
      
       if( (actionkind( kind ) || valuekind( kind )) &&
			     outer->kind != methodbox
				 )
			 {// okay to wipe me to empty
          // deallocate all guys not used by emptybox
          coredestroy();

          if(name != NULL )  // okay to do directly---can't
          {                  // be a kind with protected name
            name->destroy();
            name = NULL;
          }

         // old approach:
          //  kind = emptybox;  // do the conversion trivially!

          build( emptybox );

          didit = 1;
          return this;

       }// okay to wipe full aspect

		}// try to wipe full aspect

    // jb24:  realized want to be able to wipe abstract aspect of
    //        some kinds, such as javabox
    else if( aspect == 'a' && 
             ( kind == javabox ||
               kind == combox || kind == parbox || kind == presbox
             )
           )
    {// can wipe abstract aspect of a javabox and others like it     
		  didit = 1;
      if( name != NULL )
        name->destroy();
			name = new mystring("");
			return this;
    }

    didit = 0;  // if didn't return above, failed
		return this;
   
  } // end of wipeAspect

  // return 0/1 if I can be nicely construed as having a
  //  boolean value of false/true, else return negative error code
  int box::isTrue()
  {
    // traceEnter( tid, "box:isTrue" );

    if( type == NULL || value == NULL )
      errorexit("box::isTrue detected null value or type");

    if( type->equals( "boolean" ) 
        )
    {// legit boolean value
      if( value->equals("true") )
			{
        // traceLeave();
        return 1;
			}
      else
			{
        // traceLeave();
        return 0;
			}
    }
    else
    {
      // traceLeave();
      return invalidbooleanerror;
    }

  }// box::isTrue

  mystring* box::whatName()
  {
	  // traceEnter( tid, "box:whatName" );
    // traceLeave();
    return name;
  }

  int box::whichKind()
  {
	  // traceEnter( tid, "box:whichKind" );
    // traceLeave();
    return kind;
  }

  box* box::whoNext()
  {
	  // traceEnter( tid, "box:whoNext" );
    // traceLeave();
    return next;
  }

  box* box::whoFirst()
  {
	  // traceEnter( tid, "box:whoFirst" );
    // traceLeave();
    return first;
  }

  int box::whichState()
  {
	  // traceEnter( tid, "box:whichState" );
    // traceLeave();
    return state;
  }

  // set the gl color to the color corresponding to my color code,
  // or to my dye
  void box::setGLColor()
  {
	  // traceEnter( tid, "box:setGLColor" );

	int drawcolor;

	// if any dye set, it overrides natural color, else use
	// natural color
	if( dye != nocolorcode )
		drawcolor = dye;
	else
		drawcolor = color;

    if( drawcolor == neutralcolorcode )
      glColor3fv( neutralcolor );
	  else if( drawcolor == blueprintcolorcode )
	    glColor3fv( blueprintcolor );
	  else if( drawcolor==stackcolorcode )
		  glColor3fv( stackcolor );
	  else if( drawcolor ==heapcolorcode )
		  glColor3fv( heapcolor );
	  else if( drawcolor == signalcolorcode)
		  glColor3fv( signalcolor );
	  else if( drawcolor == newvaluecolorcode )
		  glColor3fv( newvaluecolor );
	  else if( drawcolor == newobjectcolorcode )
		  glColor3fv( newobjectcolor );
	  else if( drawcolor == waitcolorcode )
		  glColor3fv( waitcolor );
	  else if( drawcolor == focuscolorcode )
		  glColor3fv( focuscolor );
	  else if( drawcolor == errorcolorcode )
		  glColor3fv( errorcolor );
	  else if( drawcolor == consolecolorcode )
		  glColor3fv( consolecolor );
	  else if( drawcolor == consolecursorcolorcode )
		  glColor3fv( consolecursorcolor );
		else if( drawcolor == staticdatacolorcode )
		  glColor3fv( staticdatacolor );
	  else
	  {
   	  std::cout << "oops, color " << 
                        drawcolor << " is bad" << std::endl;
	    errorexit("color not implemented");
	  }

	  // traceLeave();

  }// box:setGLColor

  // depending on my depth, return z value at which my
  // background rectangle should be drawn
  float box::zValue()
  {
	  // traceEnter( tid, "box:zValue" );

  	if( depth > 1000 )
	  {
  	  errorexit("depth too large in zValue");
      // traceLeave();
      return -5;
	  }
	  else
	  {
      // traceLeave();
	    return univz + depth/1000.0;
  	}
  
	}// box::zValue

  void box::setState( int s )
  {
	  // traceEnter( tid, "box:setState" );
    // traceLeave();
    state = s;
  }

  // tell whether displaymode puts actual focus inside
  // me (assuming that I am the focus box)
  int box::isInside()
  {
	  // traceEnter( tid, "box:isInside" );
    if( inside == 'y' )
		{
		  // traceLeave();
      return 1;
		}
    else
		{
		  // traceLeave();
      return 0;
		}
  }// box::isInside

  // tell whether I will accept an enterkey in my focal part
  int box::acceptEnterkey()
  {
	  // traceEnter( tid, "box:acceptEnterkey" );

    if( (aspect=='d' && isInside()) ||
        (aspect=='f' && isInside() && part=='v') )
		{
		  // traceLeave();
      return 1;
		}
    else
		{
		  // traceLeave();
      return 0;
		}
  }// box::acceptEnterkey

  // make a copy of me and return it as the function value
  //   someone else responsible for not calling this method
  //   in a silly way
  //  Is severely recursive, of course, and preserves the
  //  entire entity, except in completely separate memory space
	//  The color is copied, too, so must be set to something
	//  separate if needed
  box* box::copy()
  {
    // traceEnter( tid, "box:copy" );

    box* newbox;
    newbox = new box();

    // go through all the members and copy
    // (note that outer and next are set to NULL,
    //  need to be hooked in to reality, ulcx,y meaningless,

    // Hey!                               ^^^^^^^^^^^^^^^^^
    //  severe stupidity here---copy all the ulc's, only the
    //  outermost one is not perfectly valid---and essential!
    
    //  and the
    //  inner list needs to be copy-created)
   
    newbox->kind = kind;  
    newbox->aspect = aspect;
    newbox->inside = inside;
    newbox->part = part;
    newbox->ulcx = ulcx;       // outer guy will need these
    newbox->ulcy = ulcy;       // reset correctly by figureSizeLocs
    newbox->width = width;
    newbox->height = height;
    newbox->next = NULL;     // set later
    newbox->outer = NULL;

    newbox->source = NULL;  // want these to start fresh in copy
    newbox->state = 0;

    newbox->color = color;
		newbox->dye = nocolorcode;

    // jb18
		newbox->props = props;

    // loop through and create copy of
    // inner list, and note my first, current
    box* curbox = first;
    int count = 0;
    box* copybox;
    box* cursorbox;

    while( curbox != NULL )
    {// make a copy of curbox and stick it in newbox

      copybox = curbox->copy();

      newbox->addInnerAtCursor( copybox );

      count++;

      if( count == 1 )
        newbox->first = copybox;

      if( count == cursor )
        cursorbox = copybox;  // remember for later
               // can't make it the current now, or would
               // screw up the adding after stuff

      curbox = curbox->next;

    }

    // now set current,cursor, maybe first
     // this was a terrible error:!!!    if( cursor == 0 ) 
          // stupid Jerry---cursor can be 0 when focus is on
          // symbol0 of a guy with many inners!
    if( count == 0 )
    {// if I have no innards, newbox still needs first, current
      newbox->current = NULL;
      newbox->first = NULL;
    }
    else
      newbox->current = cursorbox; // first was set in loop
    
    newbox->cursor = cursor;

    // create copies of my strings
/* old:
    if( name != NULL )
      newbox->name = name->copy();
    if( type != NULL )
      newbox->type = type->copy();
    if( value != NULL )
      newbox->value = value->copy();
    if( doc != NULL )
      newbox->doc = doc->copy();
*/

    // note:  when copying certain kinds of boxes, must respect
		//        the fact that they have pointers to permanent 
		//        strings like "loca variables" and "parameters"
		//        and just point to them instead of copying my name
    if( kind==classdatabox )
      newbox->name = classdatastring;
    else if(kind==classmethodsbox)
      newbox->name = classmethodsstring;
    else if(kind==instancedatabox)
      newbox->name = instancedatastring;
    else if(kind==instancemethodsbox)
      newbox->name = instancemethodsstring;
    else if(kind==paramsbox)
      newbox->name = paramsstring;
    // jboxes1c:  retvalbox,string were missing here!
    else if(kind==retvalbox)
      newbox->name = retvalstring;
    else if(kind==localsbox)
      newbox->name = localsstring;
    else if(kind==stackbox)
      newbox->name = stackstring;
    else if(kind==heapbox)
      newbox->name = heapstring;
    else if(kind==consolebox)
      newbox->name = consolestring;
    else
    {// actually copy my name
      newbox->storeName( name );
		}

		// the other strings are simply copies:
    newbox->storeType( type );
    newbox->storeValue( value );
    newbox->storeDoc( doc );

    // ship it out
    // traceLeave();
    return newbox;

  }// box::copy

  // make a copy of me (I'd better be a methodbox!) and 
  // set it up properly to be a classmethodbox or instancemethodbox
  // inserted in the stackbox
  //  Is asymmetrical--- for class method, set its source to point
  //  to the classbox I belong to, for instance method, more work
  //  with info unavailable here has to be done
  box* box::copyMethodbox( int whichkind )
  {
	  // traceEnter( tid, "box:copyMethodbox" );

    box* newbox;  

    newbox = copy();  // make a standard copy of me

    newbox->kind = whichkind; // set its kind correctly

    if( whichkind == classmethodbox )
    {// set source to the class
      box* owner = outer;  // the classmethodsbox
      owner = owner->outer;  // the classbox
      newbox->source = owner;
      // also set "type" to the class name
      newbox->storeType( owner->name );
    }

    // freshly translate all java boxes nested within newbox
 //   newbox->translateJava();

    return newbox;

  }// box::copyMethodbox

  // if appropriate kind, make a copy of me and
  // make it the clipbox (if clipbox is
  // not null when get ready to do this, destroy
  // it to save resources)
  box* box::xerox( box* clipbox, int& didit )
  {
	  // traceEnter( tid, "box:xerox" );

    // only allow xerox of guys that it might make sense to
    // xerox, in the course of producing a program
    if( valuekind(kind) || actionkind(kind) ||
         kind==classbox || kind==methodbox || 
				 kind==javabox
      )
    {// legal kind to xerox
      if( clipbox != NULL )
      {// recover resources before losing the pointer
        clipbox->destroy();
        clipbox = NULL;
      }

      // make the copy of me and put it in clipbox
      clipbox = copy();
      
      didit = 1;   // really telling did it, not requesting redraw
      // traceLeave();
      return clipbox;  // returning new clipbox, not focus
    }
    else
    {// illegal kind for xeroxing
      didit = 0;
      // traceLeave();
      return clipbox; // keep the same clipbox
    }
  }// box::xerox

  // make a COPY of clipbox and paste it onto me, clearing
  // out my resources first
  //  Refuse to do this if I'm not an emptybox, or if
  //  the guy being pasted in doesn't fit there, and certainly,
  //  though I missed it at first, if clipbox doesn't exist!
  box* box::glue( box* clipbox, int& didit )
  {
	  // traceEnter( tid, "box:glue" );

    if( clipbox == NULL )
    {
      didit = 0;
      // traceLeave();
      return this;
    }

    box* newbox = clipbox->copy();  // make the copy
    int newknd = newbox->kind;  // just for ease of typing

    if( kind == emptybox )
    {// for sure okay to proceed, will paste if context correct

      // see if the conversion is allowed---this is quite a pain!
      //  Is same as in processPrintable

      box* owner = outer;  // since I'm empty, must have an outer
      int oknd = owner->kind;        // my owner's kind
      int slot = positionInInnerList(); // my position in owner

      int okay = 0; // below set okay to 1 if find is okay
                     // (just a device to avoid one gigantic if)

      // situations where an action box is allowed
      if( actionContext( oknd, slot )
          &&  actionkind( newknd )
        )
        okay = 1;
      // situations where a value box (includes "ref") is allowed
      else if( (// context allows a value box
                 (oknd==dobox && slot==2) ||
                 (oknd==whilebox && slot==1) ||
                 (oknd==forbox && (slot==2) ) ||
                 (oknd==callbox && slot>1) ||
                 (oknd==arraybox && slot==2) ||
                 (oknd==array2dbox && slot>=2) ||
                 (oknd==opbox) ||
                 (oknd==unopbox) ||
                 (oknd==assbox && slot==2) ||
                 (oknd==returnbox) ||
                 (oknd==branchbox && (slot % 2 != 0) && next != NULL)
               )
               &&
               (
                 valuekind( newknd )
               )
             )
          okay = 1;
      // situations where a reference box is allowed, but
      // other kinds of value boxes are not 
      // (literal, op, unop)
      else if( ( // context allows lvalue
                 (slot==1 &&
                  (oknd==callbox || oknd==memberbox ||
                   oknd==arraybox || oknd==array2dbox  ||
                   oknd==assbox || oknd==growbox )                                  
                 )
               )
               &&
               (// value box that is an lvalue
                 valuekind( newknd ) &&
                 !literalkind(newknd) && newknd!=opbox &&
                 newknd!=unopbox
               )
             )
        okay = 1;
			// jb18:  allow glue of javabox any context
			else if( newknd == javabox )
			  okay = 1;
    
      if( okay )
      {// clipbox kind allowed in my context

        // gracefully release my name and doc (only resources I
        //  might have other than my own self, which is going to
        // be overwritten, mostly, keeping links to reality
        if(doc != NULL )
        {
          doc->destroy();
          doc = NULL;
        }
        if(name != NULL)
        {
          name->destroy();
          name = NULL;
        }

        // copy desired data from newbox to me (keep my
        // hooks into box reality)
        //   Uh oh!  Must make all my new inner boxes point
        //     to me as their outer!
        kind = newbox->kind;
        aspect = newbox->aspect;
        inside = newbox->inside;
        part = newbox->part;
        // ulc is not correct in the clipbox, will be fixed
        // when do fixupReality from me as the focus
        width = newbox->width;
        height = newbox->height;
        // next, outer must not be copied!
        first = newbox->first;
        current = newbox->current;
        cursor = newbox->cursor;
        name = newbox->name;
        doc = newbox->doc;
        type = newbox->type;
        value = newbox->value;

        color = newbox->color;
				dye = nocolorcode;

        // make all the outers point to me
        box* curbox;
        curbox = first;
        while( curbox != NULL )
        {
          curbox->outer = this;
          curbox = curbox->next;
        }

        // release just the box, which we have copies in
        // me of all the desired data
        delete newbox; 
 
        // new as of June 16, 2000:
				//   my depth is wrong, so need to set it, using outer's
        setDepthAndColors( owner->color, owner->depth );

        didit = 1;
        // traceLeave();
        return this;

      }// context matches clip kind
      else
      {// clipbox is wrong kind for my context
        didit = 0;
        // traceLeave();
        return this;
      }

    }// I'm an emptybox

    else if( newknd==classbox || newknd==methodbox )
    {// try for other type of pasting

      if( (kind==univbox || kind==classmethodsbox || 
              kind==instancemethodsbox
          )  &&
          isInside() && cursor==0
        )
      {// add the newbox to me
        addInnerAtCursor( newbox );
        didit = 1;
        // traceLeave();
        return newbox;
      }
      else
      {// owner might be of correct kind
      
        int oknd;

        // get ownerkind
        box* owner = outer; 
        if( owner == NULL )
          oknd = 0;             // leave it so won't match
        else
          oknd = owner->kind;        // my owner's kind

        if( (newknd==classbox && oknd==univbox) ||
          (newknd==methodbox && (oknd==classmethodsbox ||
                                 oknd==instancemethodsbox ) )

        )
        {// just insert the newbox and make it the focus
          owner->addInnerAtCursor( newbox );
          didit = 1;
          // traceLeave();
          return newbox;
        }
        else
        {// screwup
          didit = 0;
          // traceLeave();
          return this;
        }

      }// add after me
    }// newbox is class or method
    else
    {// for sure can't paste
      didit = 0;
      // traceLeave();
      return this;
    }
  
  }// box::glue

  // return "codebox" I am nested within, defined as seqbox
  // that has outer of kind cimbox
	//  7/21/2000:  add methodbox so can search for codebox statically,
	//        too---shouldn't hurt execution because won't hit one
  box* box::findCodebox()
  {
	  // traceEnter( tid, "box:findCodebox" );

    box* curbox = this;
    box* prevbox = curbox;  // jb18: in case do loop 0 times

    // jb18:  change so will return null if the search never 
		//        encounters a codebox---used in toggleBreakpoint
    while( curbox != NULL &&
		       curbox->kind != classmethodbox &&
           curbox->kind != instancemethodbox &&
					 curbox->kind != methodbox )
    {
      prevbox = curbox;
      curbox = curbox->outer;
    }

    // jb18: if prevbox is in fact a seqbox, return it, else NULL
		if( prevbox->kind == seqbox )
      return prevbox; 
		else
		  return NULL;

  }// box::findCodebox

  // from me, the universe, return the stack
  box* box::findStack()
  {
	  // traceEnter( tid, "box:findStack" );

    if( kind != univbox )
      errorexit("can only start findStack from universe");
    else
    {
      box* curbox = first;
      while( curbox->kind != stackbox )
        curbox = curbox->next;   
      // traceLeave();
      return curbox;  // the stack
    }
    // traceLeave();
    return NULL;
  }// box::findStack

  // I'm the universe, find the first in stack and
  // then get that guy's codebox, and return it
  box* box::findCallStart()
  {
	  // traceEnter( tid, "box:findCallStart" );

    box* curbox = findStack();
    // curbox is now the stack
    curbox = curbox->first;    // the top frame on stack
    curbox = curbox->accessInnerAt( 4 ); //v10: the code box of that frame
    // traceLeave();
    return curbox;
  }// box::findCallStart

  // find the visible guy that covers me,
  // which is me if I'm visible
  box* box::findCover()
  {
	  // traceEnter( tid, "box:findCover" );

    box* cover = this;  // figure it's me at the start
    box* curbox = outer;

    while( curbox != NULL )
    {
      if( curbox->aspect != 'f' )
        cover = curbox;

      curbox = curbox->outer;
    }

    // traceLeave();
    return cover;
  }// box::findCover

  // I'm the codebox that's finishing up execution of my call,
  // ship my value back to caller, if any caller and if any
  // value, spiff up the callbox, and return it --- or NULL if 
  // I'm the last call on the stack
  box* box::returnFromCall( box* ilist[], int cclist[], int& ni )
  {
	  // traceEnter( tid, "box:returnFromCall" );

    if( source == NULL )
		{
      // traceLeave();
      return NULL;   // not much to do here
		}

    // have a callbox to return to:

    // turn off special coloring of callbox
		source->setDye( nocolorcode );

    // jb28, April 4:  not returning a value was wrong,
    //    might be returning "" as a string
    if( value == NULL || 
        ( value->equals("") && 
          (type==NULL || !type->equals("string") )
        )
      )
    {// not returning a value for the callbox
      
			// so don't show any value, but put in tour
      ilist[ni] = source; cclist[ni] = signalcolorcode; ni++;
      
      // traceLeave();
      return source;
    }
    else
    {// slap my value on the call box (when it is used, may be
      // incompatible type, but the call box will cheerfully
      // take any value

/* old:
      if( source->value != NULL ) (source->value)->destroy();
      source->value = value->copy();
      if( source->type != NULL ) (source->type)->destroy();
      source->type = type->copy();
*/

      source->storeValue( value );
			source->storeType( type );

      if( source->aspect == 'f' )  // full aspect means I need to show
      {                       // my newly created "value"
        source->setAspect( 'v' );
      }

      fixupReality(0);

      ilist[ni] = source; cclist[ni] = newvaluecolorcode; ni++;

    }
    
		// traceLeave();
    return source; 
  }

  // remove my first inner, release all its resources
  void box::removeFirst()
  {
	  // traceEnter( tid, "box:removeFirst" );
          
    if( first == NULL )
      errorexit("I have no first guy to remove");

    box* firstbox = first;

    // adjust everything:

    if( firstbox->next == NULL )
    {// removing my only inner, so I become quite empty
      first = NULL;
      current = NULL;
      cursor = 0;
    }
    else
    {// have an inner after the first one
      first = firstbox->next;
      if( current == firstbox )
        current = first;  // cursor stays at 1
      else
        cursor--;  // current stays on same box, but is one slot
                   // to the left
    }

    // now release the resources:
    firstbox->destroy();

		// traceLeave();

  }// box::removeFirst

  // given the universe box u, and a heap address,
  // return pointer to the guy with that address, NULL if not found
  box* box::findInHeap( box* u, mystring* address )
  {
	  // traceEnter( tid, "box:findInHeap" );

    box* curbox = u->first;
    while( curbox->kind != heapbox )
      curbox = curbox->next;

    box* tempbox;

    tempbox = curbox->searchInner( address );

		// traceLeave();
    return tempbox;
  }// box::findInHeap

  // search my inner list for a box that has
  // s as its name, return NULL if none found
  box* box::searchInner( mystring* s )
  {
	  // traceEnter( tid, "box:searchInner" );

    box* curbox = first;
    while( curbox != NULL && 
              ! (curbox->name)->equals( s )
         )
      curbox = curbox->next;
		// traceLeave();
    return curbox;
  }// box::searchInner

  // the following four guys (storeName,..Type .. Value .. Doc)
  //   are the only approved way to set the name, type, value, doc
  //   of a box---they make sure that any overwritten string
  //   is safely deallocated, and make sure that it is a copy
  // of the given string that is stored, not just a pointer to
  // the existing string

  void box::storeValue( mystring* s )
  {
	  // traceEnter( tid, "box:storeValue" );

    if( value != NULL )
    {// I have a value currently

      // later:  do all the garbage collection stuff

      value->destroy();  // release string resources
    }

    if( s != NULL )
      value = s->copy();
    else
      value = NULL;

 		// traceLeave();
  }// box::storeValue

  void box::storeType( mystring* s )
  {
	  // traceEnter( tid, "box:storeType" );

    if( type != NULL )
    {
      type->destroy();  // release string resources
    }
    
    if( s != NULL )
      type = s->copy();
    else
      type = NULL;

 		// traceLeave();
  }// box::storeType

  void box::storeName( mystring* s )
  {
	  // traceEnter( tid, "box:storename" );

    if( name != NULL )
    {
      name->destroy();  // release string resources
    }
    
    if( s != NULL )
      name = s->copy();
    else
      name = NULL;

 		// traceLeave();
  }// box::storename

  void box::storeDoc( mystring* s )
  {
	  // traceEnter( tid, "box:storedoc" );

    if( doc != NULL )
    {
      doc->destroy();  // release string resources
    }
    
    if( s != NULL )
      doc = s->copy();
    else
      doc = NULL;

 		// traceLeave();
  }// box::storedoc

  // make inputstr into my value, as if done in evaluate
  // (used by input guys that can't get their value right away)
  //  Determine my type depending on the method name in
  //  my first box
	//
	//  this guy is like a broken-off fragment of evaluate,
	//  so it puts the box on tour, just like evaluate
  void box::storeInputString( char* inputstr,
	                         box* ilist[], int cclist[], int& ni )												
  {
	  // traceEnter( tid, "box:storeInputString" );

    value = storeCharString( value, inputstr );

    if( (first->name)->equals( "_get" ) )
      type = storeCharString( type, "char" );
    else
      type = storeCharString( type, "string" );

    // since produced value, maybe show it
    if( aspect == 'f' )  // full aspect means I need to show
    {                       // my newly created "value"
      setAspect( 'v' );
    }

    fixupReality(0);

    // strip consumed inners
    first->stripValue();
    
		// put this box on tour
		ilist[ni] = this;  cclist[ni] = newvaluecolorcode; ni++;

		// traceLeave();

  }// box::storeInputString

  // build an instance of me, a class box, return its pointer
  box* box::makeInstance()
  {
	  // traceEnter( tid, "box:makeInstance" );

    box* curbox = accessInnerAt( 3 ); // my instance data box    
    // make a copy of the instancedata box
    box* newbox = curbox->copy();
    // modify to be an instancebox
    newbox->kind = instancebox;
    newbox->source = this;  // for convenience

    newbox->storeType( name );

    // key bit:  get next available heap address and make
    //           that into newbox's name
    currentHeapAddress++;  // will never have time to new more than
                             // a few billion boxes!
    char where[15];
    sprintf( where, "#%ld", currentHeapAddress );

/* old:
    (newbox->name)->destroy();  // is a copy of "instance data"
    newbox->name = new mystring( where );
*/

/*  this is awful --- destroys the permanent shared string
      "instance data" --- all we have to do is
			build the new string and make newbox->name point to it---
			no need to tidy up the shared guy!

     newbox->name = storeCharString( newbox->name, where );
*/

    // store current heap address as name
    newbox->name = new mystring( where );
				      
 		// traceLeave();
    return newbox;
  }// box::makeInstance

  // search for my name as name of class in universe u,
  // if not found return status = classnotfounderror,
  // else build it as copy of that class's instance data,
  // suitably modified, and put it in the heap, with refcount 1
  // and note in my value the heapaddress and put the class
  // in my type
	//
	//  new scheme:  return the new box so it can be toured
  box* box::addInstanceToHeap( box* u, int& status )
  {
	  // traceEnter( tid, "box:addInstanceToHeap" );

    // search for it
    box* curbox = u->first;

    // after 0 or more classboxes there is guaranteed to be
    // the stackbox, stops the search if need be
    while( curbox->kind==classbox &&
              ! (curbox->name)->equals( name ) )
      curbox = curbox->next;

    if( curbox->kind == stackbox )
    {
      status = classnotfounderror;
			return NULL;
    }
    else
    {// found the class, now make and insert the instance

      // make the instancebox
      box* newbox = curbox->makeInstance();  
      // set curbox to heap
      curbox = u->first;
      while( curbox->kind != heapbox )
        curbox = curbox->next;
      // add the instancebox into the heap
      curbox->addInnerAtFront( newbox );

      newbox->fixupReality(0); 

      // hey, give me my info:
/* old:
      if( value != NULL ) value->destroy();
      value = (newbox->name)->copy();
      if( type != NULL ) type->destroy();
      type = name->copy();
*/
      storeValue( newbox->name );
      storeType( name );

      status = 1;

			return newbox;

    }

		// traceLeave();

  }// box::addInstanceToHeap

  // given my owner's kind, and my position in its inner list,
  // return whether or not I'm in a context where an action box
  // is allowed
  int box::actionContext( int oknd, int slot )
  {
	  // traceEnter( tid, "box:actionContext" );

    if( oknd==seqbox || (oknd==dobox && slot==1) ||
            (oknd==whilebox && slot==2) || 
            (oknd==forbox && slot!=2) ||          
            (oknd==branchbox && 
               ( (slot % 2 == 0) || next==NULL )
            )
    )
		{
  		// traceLeave();
      return 1;
		}

    // jb16:  oops --- this box is in context where an actionbox
		//                 is allowed if its owner is a javabox
		//   that is in an action context
		else if( oknd==javabox && outer->isAction() )
		{
		  return 1;
		}

    else
		{
  		// traceLeave();
      return 0;
		}
  }// box::actionContext

  // destroy all my inners
  void box::removeAllInners()
  {
	  // traceEnter( tid, "box:removeAllInners" );

    box* curbox = first;
    box* nextbox;

    while( curbox != NULL )
    {
      nextbox = curbox->next;
      curbox->destroy();
      curbox = nextbox;
    }

    first = NULL;
    current = NULL;
    cursor = 0;

    int junk;
    figureSizeLocs( junk );

 		// traceLeave();

  }// box::removeAllInners

  // remove contents of stack and heap, set heap address to 0
  //  (done before start execution, and before save)
  void box::clearOutStackAndHeapAndStatic()
  {
	  // traceEnter( tid, "box:clearOutStackAndHeap" );

    if(kind!=univbox)
      errorexit("Can't call clearoutstackandheap on non-universe box");
    else
    {// nothing can go wrong
      box* curbox = first;  // first class, if any, or stack

      while( curbox->kind != stackbox )
        curbox = curbox->next;
      // now curbox is the stackbox, clear it out
      curbox->removeAllInners();
      curbox = curbox->next; // now is the heap     
      curbox->removeAllInners();

      // v10:  clear out static data, too
			curbox = curbox->next; // now is the static data box
			curbox->removeAllInners();

      curbox->fixupReality( 0 ); 

      currentHeapAddress = 0;  // have cleared out the heap

    }

		// traceLeave();
  }// box::clearOutStackAndHeap

  // starting from me, must be the universe, 
	//  close files and clear out portboxes
	//   NO---now ports->closeAll is being used lots
	//        to make sure files are safely closed,
	//   here we just need to make sure all the portboxes
	//   are empty, independent of files---so remove the
	//   "if port is in use"
  void box::clearOutPorts()
	{
    // traceEnter( tid, "box:clearOutPorts" );
		int k;
		box* pb;

    pb = accessPort( this, 1 );  // pb is portbox 1
    
		for( k=1; k<=numports; k++ )
		{
		  pb->value = storeCharString( pb->value, "" );
			pb->type = storeCharString( pb->type, "--" ); 
			pb = pb->next; // move to next portbox
		}

    // traceLeave();
	}
	  
  // make attempt to detect a bogus empty value
  int box::valueNotInit()
  {
	  // traceEnter( tid, "box:valueNotInit" );

    if( value == NULL ||
        ( value->equals("") && !(type->equals("string")) )
      )
		{
      // traceLeave();
      return 1;
		}
    else
		{
      // traceLeave();
      return 0;
		}
  }// box::valueNotInit

  // v10:  added this to detect no type returned, hence no value
	//       returned (avoid problem with "" return type string)
  int box::typeNotInit()
	{
	  if( type == NULL || type->equals("") )
		  return 1;
		else
		  return 0;
	}

  // depending on mode, move ahead to the next appropriate
  // box and return it, return NULL if is none
	//   jb18:  messed this up by inserting output box in methods,
	//          fix it!
  box* box::nextToCheck( char mode )
  {
	  // traceEnter( tid, "box:nextToCheck" );

    box* curbox;
    box* nextbox;

    if( mode=='c' )
    {// I'm univ or class, give next class
      if( kind == univbox )
      {
        if( first->kind == classbox )
				{
          // traceLeave();
          return first;
        }
				else
				{
          // traceLeave();
          return NULL;
				}
      }
      else if( kind == classbox )
      {
        if( next->kind == classbox )
				{
          // traceLeave();
          return next;
        }
        else
				{
          // traceLeave();
          return NULL;
				}
      }
      else
        errorexit("error in box::nextToCheck");
    }
    else if( mode=='m' )
    {// I'm a classbox or a member of one of the 4 inner boxes
      // i.e., a "member" of the class

      if( kind == classbox )
      {
        curbox = first;  // the first of 4 inners in the class
      }
      else
      {// search from me --- if next don't work, have to go out,
        // search along
        if( next != NULL )
				{
          // traceLeave();
          return next;  // easy---my next works
				}
        else 
        {
          curbox = outer; // start hard search from my outer's next
          curbox = curbox->next;
        }
      }

        // if still here, search from startbox for first with
        // nonnull first, use it
        while( curbox != NULL && curbox->first == NULL )
          curbox = curbox->next;

        if( curbox == NULL )
				{
          // traceLeave();
          return NULL;
				}
        else
				{
          // traceLeave();
          return curbox->first;
				}
   
    }

    else if( mode == 'd' )
    {
      if( kind == methodbox ) // first call
      {// I'm a methodbox looking for my first data box
        curbox = first;  // the paramsbox
        nextbox = curbox->first;  // first parameter, if any
        if( nextbox != NULL )
				{
          // traceLeave();
          return nextbox;
				}
        else
        {
				  // jb18:  need an extra jump to get past output box
					curbox = curbox->next;  // the output box

          curbox = curbox->next;  // the locs box
          curbox = curbox->first;
          // traceLeave();
          return curbox;  // either first local, or NULL if none
        }
      }
      else
      {// I'm a databox in params or locs, looking for next
        if( next != NULL )
				{
          // traceLeave();
          return next;
				}
        else
        {// I'm last in my box
          curbox = outer;  // params or locals
          if( curbox->kind == localsbox )
					{
          	// traceLeave();
            return NULL;
					}
          else
          { // curbox is the params box, just finished it,
            // so move to locals
						// jb18:  extra hop 
						curbox = curbox->next; // curbox is now output box
            curbox = curbox->next; // now it's the locals box
            curbox = curbox->first;
           	// traceLeave();
            return curbox;  // first in locals or NULL if none
          }
        }
      }

    }

    else
      errorexit("illegal mode in box::nextToCheck");
    // traceLeave();
    return NULL;  
  }// box::nextToCheck

  // return whether or not my (I must be a databox) value and
  // type are good, with 1 meaning good, return negative error
  // code for various problems
  // 
  //   This guy is used for initial static check, does some
  //  slightly strange things ---
  //  
  //  use "checkCompat" for general purpose checking that
  //  the value just stored in a data box is okay --- this
  //  one is for checking interactively entered values
  //  before execution
  void box::checkValueType( int& status )
  {
	  // traceEnter( tid, "box:checkValueType" );

    if( kind != databox )
      errorexit("can't call except on data box");

    if( type == NULL || type->equals("") )
      status = missingtypeerror;
    else
    {// have non-empty type

      // jb18:  add in this check that type is legal
			if( ! isLegalType( type ) )
			{
			  status = illegaltypeerror;
				return;
			}

      if( value == NULL || value->equals("") )
        status = 1;  // empty value is always okay
      else
      {// value is non-empty

        box* owner = outer;
        int code;
        long iresult;
        double dresult;

        if( owner->kind == paramsbox )
          status = parammustbeemptyerror;
        else if( type->equals("char") )
        {
          if( value->length() == 1 )           
            status = 1;
          else
            status = badprimvalueerror;
        }
        else if( type->equals("boolean") )
        {
          if( value->equals( "true" ) ||
              value->equals( "false" ) )
            status = 1;
          else
            status = badprimvalueerror;
        }
        else if( type->equals("int") )
        {
          code = value->toInt( iresult );
          if( code )
            status = 1;
          else
            status = badprimvalueerror;
        }
        else if( type->equals("float") )
        {
          code = value->toFloat( dresult );
          if( code )
            status = 1;
          else
            status = badprimvalueerror;
        }
        else if( type->equals("string") )
        {
          status = 1;  // any string is cool
        }
        else
          status = objectrefnotemptyerror;
    
      }// have non-empty value

    }// have non-empty type

    // traceLeave();
		    
  }// box::checkValueType

  // return whether I, according to my type, am compatible
  // to have the value/type from secondbox stored as my value
  //   I am either a databox, or a valuebox.  If a valuebox,
  //   I have no type, inherit from outer.  Try setting it
  //   to point to same string, though seems a little dangerous
	//
	// 6/14/00:  removed all the options where secondbox had type ?
	//  so now this guy accepts only the exact same type and
	//   int to float
	//     6/19/2000:  returns 1 if everything looks cool, 0 if
	//                 general failure, -1 if this box is not
	//                 sourcing a decent box
  int box::storageCompatible( box* secondbox )
  {
	  // traceEnter( tid, "box:storageCompatible" );

    box* owner;

    if( kind == valuebox )
    {
      owner = outer;

      // jboxes1d:  tiny leak?
      if( type != NULL )
        type->destroy();
      // jboxes1d

      // jboxes1e:  had code that let the gridbox type be changed to ""
      // type = owner->type;  // inherit it!
      // instead:
      storeType( owner->type );  // makes a copy of the owner type

    }
    else if( kind == databox )
    {
      // fine
    }
    else
		  return -1; // this box is not pointing to anything plausible
//      errorexit("storagecompatible only works on databox, valuebox");

    // jb17:  allow value #0 to be stored in any non-prim
    if( type->equals( secondbox->type )  // have exact same types
		    ||
        ( !isprimtype( type ) &&
				  (secondbox->value)->equals( "#0" )
			  )  // store #0 in any non-prim
				||
            (  type->equals( "float" ) &&
                 (secondbox->type)->equals( "int" )    )  							 
      )
		{
    	// traceLeave();
      return 1;
		}
    else
		{
    	// traceLeave();
      return 0;
		}
  
  }// storageCompatible

  // check names don't overlap and databoxes all have values
  //    matching their types, return status with errorcode
	// jb18:  rework to include check that all data boxes have
	//        legal types and fix name overlap problem
  box* box::staticCheck( int& status )
  {
	  // traceEnter( tid, "box:staticCheck" );

    if( kind!=univbox )
      errorexit("only universe can do staticCheck");

    status = 1;  // assume everything will be okay!

    // plan:  using special nextToCheck( mode ), do the
    //        scanning

    box* left;
    box* right;
    box* leftmem;
    box* rightmem;
    box* leftdata;
    box* rightdata;

    left = nextToCheck( 'c' );
    while( left != NULL )
    {// left is the next class to work on

      if( left->name == NULL || (left->name)->equals("") )
      {
        status = canthaveemptynameerror;
    		// traceLeave();
        return left;
      }

      if( badFirstSymbol( (left->name)->charAt(1) ) )
			{
			  status = badfirstsymbolerror;
				// traceLeave();
				return left;
			}

      if( isReserved( left->name ) )
      {
        status = namecantbereservederror;
    		// traceLeave();
        return left;
      }

      // keep left fixed, work on all later classes
      right = left->nextToCheck( 'c' );  // next class after left
      while( right != NULL )
      {// have left-right pair of classes to check

        if( right->name == NULL ||
            (right->name)->equals("") )
        {
          status = canthaveemptynameerror;
      		// traceLeave();
          return right;
        }
        else
        {// have both names, make sure not equal
          if( (left->name)->equals( right->name ) )
          {
            status = duplicateclassnameserror;
        		// traceLeave();
            return left;
          }
        }

        right = right->nextToCheck( 'c' );
      }

      // check out class "left" in more detail

        // for each member of left, compare to later members
        leftmem = left->nextToCheck( 'm' );
        while( leftmem != NULL )
        {// work on leftmem

          // check for leftmem having no name
          if( leftmem->name == NULL || (leftmem->name)->equals("") )
          {
            status = canthaveemptynameerror;
        		// traceLeave();
            return leftmem;
          }

          if( badFirstSymbol( (leftmem->name)->charAt(1) ) )
		    	{
			      status = badfirstsymbolerror;
				    // traceLeave();
				    return leftmem;
			    }

          if( isReserved( leftmem->name ) )
          {
            status = namecantbereservederror;
    		    // traceLeave();
            return leftmem;
          }

          // compare to all later members
          rightmem = leftmem->nextToCheck( 'm' );
          while( rightmem != NULL )
          {

            if( rightmem->name == NULL || 
                (rightmem->name)->equals("")  
              )
            {
              status = canthaveemptynameerror;
          		// traceLeave();
              return rightmem;
            }
            else
            {// have both names, see if the same
              if( (leftmem->name)->equals( rightmem->name ) )
              {
                status = duplicatemembernameserror;
            		// traceLeave();
                return leftmem;
              }
            }
            rightmem = rightmem->nextToCheck( 'm' );
          }

          // check out leftmem in more detail:
          //    if is a databox, check type vs. value
          //    if is a methodbox, go inside and do yet
          //      an inward loop making sure all the params
          //      and locs check out
          if( leftmem->kind == databox )
          {// see if value matches type           
            leftmem->checkValueType( status );
            if( status < 0 )
						{
              // traceLeave();
              return leftmem;
						}
          }
          else if( leftmem->kind == methodbox )
          {// burrow inside and make sure params and locs
            // check out okay --- none empty, no overlap, match

            leftdata = leftmem->nextToCheck( 'd' );
            while( leftdata != NULL )
            {
              // perform all checks on this databox
                // make sure name is not empty
                if( leftdata->name == NULL || 
                    (leftdata->name)->equals("") )
                {
                  status = canthaveemptynameerror;
              		// traceLeave();
                  return leftdata;
                }
                // check value and type
                leftdata->checkValueType( status );
                if( status < 0 )
								{
                	// traceLeave();
                  return leftdata;
								}

                if( badFirstSymbol( (leftdata->name)->charAt(1) ) )
		    	      {
      			      status = badfirstsymbolerror;
			      	    // traceLeave();
      				    return leftdata;
			          }

                if( isReserved( leftdata->name ) )
                {   
                  status = namecantbereservederror;
    		          // traceLeave();
                  return leftdata;
                }


              // loop through all later databoxes to compare names
              rightdata = leftdata->nextToCheck( 'd' );
              while( rightdata != NULL )
              {
                if( rightdata->name == NULL || 
                    (rightdata->name)->equals("")  
                  )
                {
                  status = canthaveemptynameerror;
                	// traceLeave();
                  return rightdata;
                }
                else
                {// have both names, see if the same
                  if( (leftdata->name)->equals( rightdata->name ) )
                  {
                    status = duplicatelocalnameserror;
                		// traceLeave();
                    return leftdata;
                  }
                }

                rightdata = rightdata->nextToCheck( 'd' );
              }

              leftdata = leftdata->nextToCheck( 'd' );
            }// scan over databoxes in the method

          }// burrow inside methodbox
          else
            errorexit("bad kind in box::staticCheck");

          // move leftmem on to next one
          leftmem = leftmem->nextToCheck( 'm' );
        }      

      // move on to the next class
      left = left->nextToCheck( 'c' );

    }// outer class loop

    // traceLeave();
    return NULL;
  // if get here, everything checked out
  }// box::staticCheck


  // v10:  fill static data box with copies of all 
	//       non-empty class data boxes, no error possible
  void box::copyClassData()
  {
	  // traceEnter( tid, "box:copyClassData" );

    if( kind!=univbox )
      errorexit("only universe can copy class data");

    // first, find the allclassdata box
		box* target = first; // might be a class box, or stack box
		while( target->kind != allclassdatabox )
		  target = target->next;

    // now, scan through all the class boxes, for each one copying
		// its class data box over to a classdatacopybox, if non-empty

		box* curbox = first;  
		box* cdbox;
		box* newbox;
		box* dbox;
		box* nbox;
		int junk;

		while( curbox->kind == classbox )
		{// curbox is the current class box being examined
		  cdbox = curbox->first;  // cdbox is the classdata box
			if( cdbox->innerLength() > 0 )
			{// the classdata box has at least one member, so make copy

			  // build the classdatacopy box and insert it 
        newbox = new box;
				newbox->build( classdatacopybox );
				target->addInnerAtCursor( newbox );		
				// give it name of class it belongs to

        // jboxes1d:  this call is silly --- storeName already
        //  copies its argument, can't possibly need to copy when 
        // pass in!
				//  newbox->storeName( (curbox->name)->copy() );
        // instead:
        newbox->storeName( curbox->name );

        // crucial to keep geometry correct
        newbox->fixupReality(0);

        // scan through cdbox and copy all data boxes over to
				// newbox (ditch comments, if any)
        dbox = cdbox->first;
				while( dbox != NULL )
				{// copy dbox to nbox (since is simple, do the parts manually)
				 // and add to newbox
				  nbox = new box;
					nbox->build( databox );

          // jboxes1d:  lose the redundant copy in argument
					nbox->storeName( dbox->name );
					nbox->storeType( dbox->type );

          // v13:
          // dbox might not have a value, in which case give it empty 
					//  string value
					if( dbox->value == NULL )
					{
					  dbox->value = new mystring("");
					}

          // jboxes1d:  lose the copy in argument below
					nbox->storeValue( dbox->value );

					newbox->addInnerAtCursor( nbox );
					nbox->moveOut( junk );
					nbox->fixupReality(0);

					dbox = dbox->next;
				}

			}// cdbox has some data boxes

			curbox = curbox->next;

		}// handle next class box

    // traceLeave();

  }// box::copyClassData


  // clean out my value and type, manage aspect
  void box::stripValue()
  {
	  // traceEnter( tid, "box:stripValue" );

    if( value != NULL ) value->destroy();
    if( type != NULL ) type->destroy();

    if( aspect == 'v' )
    {
      setAspect( 'f' );
      fixupReality(0);
//      figureSizeLocs(junk); // I need to fix myself, later my
                             // owner will fixupReality
    }

		// traceLeave();

  }// box::stripValue

  // go through and strip all my condition inners values,
  // I must be a branchbox, of course, who has just finished
  // execution (leave the values earlier to show progress across
  // the list of cases)
  void box::stripAllBranchboxCondValues()
  {
	  // traceEnter( tid, "box:stripAllBranchboxCondValues" );

    box* curbox = first;         int count = 1;
    int number = innerLength();
    while( count <= number-2 )
    {// curbox is a conditional

      curbox->stripValue();

      // move to next next box (might be the default action box)
      curbox = curbox->next;  count++;
      curbox = curbox->next;  count++;
    }
		// traceLeave();
  }// box::stripAllBranchboxCondValues

  // given universe u, scan until hit portsbox, then go in
	//  and move to desired port, return that portbox
	//   NOTE:  later might replace by void* approach using portstype
  box* box::accessPort( box* u, int portNumber )
	{
	  // traceEnter( tid, "box:accessPort" );

	  box* curbox = u;
		curbox = curbox->first;
		while( curbox->kind != portsbox )
		  curbox = curbox->next;

    int count;
		curbox = curbox->first;  count = 1; // on port 1

		while( count < portNumber )
		{
		  curbox = curbox->next;  count++;
		}

		// traceLeave();

		return curbox; // the desired portbox
	}

  // I must be a portbox, load buffer full of syms from
	//  the given file
  void box::loadBuffer( int whichPort)
	{
	  FILE* handle = ports->getFile( whichPort );

    int k;
		int ch;

		for( k=1; k<=buffersize; k++ )
		{
		  ch = fgetc( handle );
			
			// put in special symbol when notice end of file
			if( ch == EOF )
			{// make this port have eof status
        ports->noteEmpty( whichPort );
			  break;
      }

      // add to buffer (=value string):
      value->insertCharAt( k, ch );

  	}

	}


  // I must be a portbox, send my value to file
	void box::saveBuffer( int whichPort)
	{
	  FILE* handle = ports->getFile( whichPort );

    int k;

		for( k=1; k<=value->length(); k++ )
		{
		  fputc( value->charAt( k ), handle );
  	}

    // set value to empty string
		value = storeCharString( value, "" );

	}// saveBuffer

  // return whether or not my type is "int" and my value
	//   represents an int, and return my int value
  int box::haveIntValue(long& x)
	{
    if( ! value->toInt( x ) ||
		    ! type->equals("int") 
			)
			return 0;
		else
		  return 1;
	}

  // same as isInt, but for float (boxes float, implement as double)
  int box::haveFloatValue(double& x)
	{
    if( ! value->toFloat( x ) ||
		    ! type->equals("float") 
			)
			return 0;
		else
		  return 1;
	}

// jb23:  pulled out the box::evaluate method to a separate file,
//        purely for editing convenience, and because of fear---when
//        adding code to the giant box.cpp file, was sometimes 
//        absurdly slow to respond
#include "boxevaluate.cpp"

  int box::numberUsed()
	{
	  return boxesused;
	}

  // starting from me, recursively change to my full aspect and
	// ask inners to do the same
  //   jb24:  except certain kinds show their names aspects instead
  //          as being more fundamental to their natures
  void box::changeAllToFull( int& didit )
	{
    // jb24:  stop if not permitted
    if( ! canChange() )
    {
      didit = 0;
      return;
    }

    // jb24:  set to full or abstract depending on kind
    if( kind != javabox && kind != combox && kind != presbox &&
        kind != parbox 
      )
    {
      setAspect( 'f' );
	  	moveOutside();  // if were inside before, makes no sense now
    }
    else
    {
      setAspect( 'a' );
      moveOutside();
    }
		
		box* curbox = first;
		
		while( curbox != NULL )
		{
		  curbox->changeAllToFull( didit );
      curbox = curbox->next;
		}

    // now that inners may be different, 
		fixupReality( 0 );
						
    didit = 1;
	}

  void box::changeAllToDoc( int& didit )
	{
    // jb24:  if not permitted to change views, stop
    if( ! canChange() )
    {
      didit = 0;
      return;
    }

	  // if I have a non-empty doc part, use that aspect
	  if( doc != NULL && !doc->equals("") )
		{
		  setAspect( 'd' );
			moveOutside();
			didit = 1;
		}
		else
		  didit = 0;

    int innerdidit; 
		box* curbox = first;

		while( curbox != NULL )
		{
		  curbox->changeAllToDoc( innerdidit );
      // still redraw if I'm using full and an inner changes
			//    appearance
			if( aspect == 'f' && innerdidit )
			  didit = 1;

			curbox = curbox->next;
    }

		fixupReality( 0 );
	}

// if I'm a class method named main, proceed with
//   exporting recursively, report status at end

box* mainbox;  // note the special starting main
int exporterror;  // jb18: for monitoring for error

int box::exportToJava()
{
  // make sure I'm a class methodbox named "main"
  if(	outer==NULL ||  // so I am universe box
			outer->whichKind() != classmethodsbox ||
			!name->equals("main")
		)
		return -1; // not good to start on
	else
  {// go ahead and do the export, may fail in various ways

    mainbox = this;  // note the starting point
		exporterror = 0;  // no error noted yet

    // save the universe before attempting export 
        // move all the way out to universe box
			int junk;
			box* ubox = setFocusToUniv( junk );
      univstream = fopen( univfilename, "w" );
      universe->clearOutStackAndHeapAndStatic();
      universe->save( univstream );
      fclose( univstream );

	  // use my class's name as the javafile name, get file ready
		FILE* javafile;

		box* owner = outer;  // the classmethodsbox
		owner = owner->outer;  // the classbox

		char* filename = new char[(owner->name)->length()+6];
		int k;
		for( k=1; k<=(owner->name)->length(); k++ )
		  filename[k-1] = (owner->name)->charAt( k );
		filename[k-1] = '.';
		filename[k] = 'j';  k++;
		filename[k] = 'a';  k++;
		filename[k] = 'v';  k++;
		filename[k] = 'a';  k++;
		filename[k] = '\0';

		javafile = fopen( filename, "w" );

		  // jb18:  small memory leak
			delete filename;

		if( javafile == NULL )
		  return -2;  // couldn't open file

		// have file ready, get to universe and go for it
		owner = owner->outer;  // owner is now the universebox

		owner->translateToJava( javafile, 0 );

	  fclose( javafile );  // close the file tidily whether was
		                     // export error or not

    if( exporterror )
		  return erroralreadyrequested;
		else
		  return 0;

	}// have good starting main

}// exportToJava

// local utilities
void spaces( FILE* javafile, int number )
{
  int k;
	for( k=1; k<=number; k++ )
	  fprintf( javafile, " " );
}// spaces

// I'm an idbox, figure out what data box I refer to (source)
//  (I must refer to a databox, rather than being a
//    method name)
box* box::determineIdSource()
{
      box* curbox;
      box* codebox;
			box* cimbox;
			box* prevbox;
			box* sourcebox;
			  
      if( name == NULL || name->equals("") )
      {// no name filled in for me
			 // (note:  is error that should have been caught before
			 //         exporting, so just want to avoid crash)
			  sourcebox = NULL; 
      }
			else
			{// have id->name as an actual string, search it out
        // get bearings      
        codebox = findCodebox();  // get to the codebox from me
        cimbox = codebox->outer;  // the cimbox
        prevbox = cimbox->first;   // the paramsbox
				curbox = prevbox->next;    // v10: the output box
        curbox = curbox->next;    // the localsbox

				// search locals first
        sourcebox = curbox->searchInner( name );

        if( sourcebox == NULL )
				{// search params
				  sourcebox = prevbox->searchInner( name );
				}

        if( sourcebox == NULL )
				{// search classdata box
          curbox = cimbox->outer;// the class or instance methods box
					curbox = curbox->outer; // the class box
					curbox = curbox->first;  // the classdata box
				  sourcebox = curbox->searchInner( name );
				}

        if( sourcebox == NULL )
				{// search instance data box, if is instance method
				  if( (cimbox->outer)->kind == instancemethodsbox )
					{// is an instance method, search instance data
  				  curbox = curbox->next; // class methods
						curbox = curbox->next;  // instance methods
	  				sourcebox = curbox->searchInner( name );
					}
				}

      }

  return sourcebox;

}// determineIdSource

// produce a mystring for my return type---have to
//   deep search my third inner box---the codebox
// v10:  no, is no searching now that we have the retvalbox
//  just use its type
mystring* box::determineReturnType()
{
  mystring* rettype;

	box* curbox = first; // params box
	curbox = curbox->next; // retval box

	if( curbox->type==NULL || (curbox->type)->equals("") )
    rettype = new mystring( "void" );
	else
	  rettype = (curbox->type)->copy();

/*  not any more, not with v10:
	curbox = curbox->next;  // codebox to be searched

  // call the recursive guy, get back code
	rettype = curbox->searchForReturnbox(); 

	if( rettype == NULL )
	  rettype = new mystring( "void" );
*/

	return rettype;
}

// recursively search for type of simple returnbox
mystring* box::searchForReturnbox()
{
  mystring* rettype = NULL;
	box* curbox;
	box* sourcebox = NULL;

  if( kind == returnbox )
	{// I'm a returnbox, stop and return a mystring answer
	  int knd = first->kind;
	  if( knd == idbox )
		{// find the databox I refer to, return copy of its type
      sourcebox = first->determineIdSource();

      // return string based on sourcebox found
			if( sourcebox == NULL ||
			    sourcebox->type == NULL )
			  return new mystring("UNKNOWN");
			else
			{// have a databox somewhere id->name matches, return its type
  		  return  (sourcebox->type)->copy();// is literal, type is known
			}

		}// figure out type of idbox
		else if( knd == intbox ||
				     knd == charbox ||
				     knd == floatbox ||
				     knd == booleanbox ||
				     knd == stringbox
	         )
		  return  (first->type)->copy();	 // is literal, type is known
		else
		{// I'm not a simple enough return box, so return unknown type
		  rettype = new mystring( "UNKNOWN" );
			return rettype;
		}
	}
	else
	{// check out my inners
    curbox = first;
	  while( curbox != NULL && rettype == NULL )
		{// check out next inner curbox
      rettype = curbox->searchForReturnbox();
		  curbox = curbox->next;
		}	 	  

    return rettype; // will be NULL if exited by hitting end of list,
		                // or not NULL if found one
	}

}

// determine whether I am a legal assignment/growbox sequence
// that can be put in the comma form in a for loop;
//   if justAss, only allow assignments
int box::legalAssGrowSeq( int justAss )
{
  if( kind == assbox ||
	    (kind == growbox && !justAss )
		)
	  return 1; // easiest---single assignment or increment

	if( kind != seqbox )
	  return 0;  // if not simple, must be seq of good ones

	// now know I'm a seqbox, scan all inners for goodness
	box* curbox = first;
	while( curbox != NULL )
	{
	  if( curbox->kind != assbox &&
		    ( curbox->kind != growbox || justAss )
			)
			return 0; // found a bad inner
	  curbox = curbox->next;
	}

	return 1;  // all the inners checked out

}// legalAssGrowSeq

// translate me, a legal ass grow sequence guy, to java
//  Is recursive just for coding convenience, can't actually
//  be nested more than one seqbox deep
void box::translateInlineToJava( FILE* javafile )
{
  box* curbox;

  if( kind == growbox )
	{// single growbox  
		name->toFile( javafile ); // ++ or --
		curbox = first;
		curbox->translateToJava( javafile, 0 ); // the idbox
	}
	else if( kind == assbox )
	{// single assbox
		curbox = first; // the left hand side
		curbox->translateToJava( javafile, 0 );
		name->toFile( javafile ); // the = or += or etc.
		curbox = curbox->next;
		curbox->translateToJava( javafile, 0 ); // rhs
	}
	else
	{// must be a seqbox with assbox or growbox inners
	  curbox = first;
		while( curbox != NULL )
		{
		  curbox->translateInlineToJava( javafile );
		  curbox = curbox->next;
			if( curbox != NULL ) // is another one to follow
			  fprintf( javafile, ", " );
		}
	}
}// translateInlineToJava

// recursively translate me to output to javafile
//  with indent as # of columns of indentation to use for
//  LINES of code I generate
// jb18:  flag error in global exporterror
void box::translateToJava( FILE* javafile, int indent )
{
  int k;
	char ch;
	box* curbox;
	int column;

  // jb18:  once error is noted, all following calls quit
  if( exporterror )
	  return;

  // uniformly for most kinds, first display my documentation
	// at current level of indentation
	//    suppress this automatic commenting for some kinds
	if( doc != NULL && !doc->equals("") && 
	     (outer ==NULL || outer->kind != paramsbox )
	  )
	{// have a doc, send it out
    spaces( javafile, indent ); // start a new line --- the first  
		fprintf( javafile, "//" );
    
		for( k=1; k<=doc->length(); k++ )
		{// send char #k
		  ch = doc->charAt(k);
			if( ch != 13 )
		    fprintf( javafile, "%c", ch );
			else
			{// end of line in doc
			  fprintf( javafile, "\n" );
				spaces( javafile, indent );
				fprintf( javafile, "//" );
			}
		}
		fprintf( javafile, "\n" );
	}

  // depending on my kind, do it
  // kind zone ====================================================

	if( kind==univbox )
	{// initial setup, then ship out my class boxes
//	  fprintf( javafile, "import java.io;\n\n" );

		// jb18:  change to shipping out as long as they're classboxes
		//   (had a for loop with counting, screws up as add extra
		//    static data, lessons after ports)
		curbox = first;  
		while( curbox->kind == classbox )
		{// ship out classbox curbox --- with no additional indentation
		  curbox->translateToJava( javafile, indent );
			curbox = curbox->next;
		}
		// final actions for entire file:

	}// univbox

	else if( kind==classbox )
	{
    spaces( javafile, indent );  // certainly 0!
    fprintf( javafile, "class " );
		name->toFile( javafile );
		fprintf( javafile, 
		"//===================================================\n" );
		spaces( javafile, indent );
		fprintf( javafile, "{\n" );	  

    curbox = first;
		for( k=1; k<= 4; k++ )
		{
		  curbox->translateToJava( javafile, indent+2 );
			curbox = curbox->next;
		}

		spaces( javafile, indent );
		fprintf( javafile, "}// end of class " );	  
		name->toFile( javafile );
		fprintf( javafile, "\n\n" );	
	}// classbox

  else if( kind==classdatabox || kind==instancedatabox )
	{
	  if( first != NULL )
		{// have at least one, else skip the labelling
  	  spaces( javafile, indent );
	  	if( kind==classdatabox )
	      fprintf( javafile, 
				"// class data members------------------------------\n\n" );
  		else
	      fprintf( javafile, 
				"// instance data members------------------------------\n\n" );

      curbox = first;
	  	while( curbox != NULL )
		  {
		    curbox->translateToJava( javafile, indent );
			  curbox = curbox->next;
      }

      fprintf( javafile, "\n" ); // separate sections

    }// have at least one inner

	}// class/instance databox

  else if( kind==databox )
	{
	  spaces( javafile, indent );
		if( outer->kind == classdatabox )
		  fprintf( javafile, "public static " );
		else if( outer->kind == instancedatabox )
		  fprintf( javafile, "public " );
		else if( outer->kind == localsbox )
		{ // no keyword proceeds local variable declaration
		}
		else
		  errorexit(
			  "inappropriate use of translate to Java on databox");

    type->toFileArrayAdjusted( javafile );

    fprintf( javafile, " " );
		name->toFile( javafile );

		if( value != NULL && !value->equals("") )
		{
		  fprintf( javafile, " = " );
			if( type->equals("char") )
			{
			  fprintf( javafile, "'" );
				value->toFileStringForm( javafile );
				fprintf( javafile, "'" );
			}
			else if ( type->equals("string") )
			{
			  fprintf( javafile, "\"" );
				value->toFileStringForm( javafile );
				fprintf( javafile, "\"" );
			}
			else
  			value->toFile( javafile );
		}

		fprintf( javafile, ";\n" );

	}// databox

  else if( kind==classmethodsbox || kind==instancemethodsbox )
	{
	  if( first != NULL )
		{// have at least one, else nothing to output
  	  spaces( javafile, indent );
	  	if( kind==classmethodsbox )
	      fprintf( javafile, 
				"// class methods------------------------------\n\n" );
  		else
	      fprintf( javafile, 
				"// instance methods------------------------------\n\n" );

      curbox = first;
	  	while( curbox != NULL )
		  {
		    curbox->translateToJava( javafile, indent );
			  curbox = curbox->next;
      }

      fprintf( javafile, "\n" ); // separate sections

    }// have at least 1

	}// class/instance methodsbox

  else if( kind==methodbox )
	{
	  spaces( javafile, indent );  column = indent;

		fprintf( javafile, "public " ); column += 7;

    if( outer->kind == classmethodsbox )
		  fprintf( javafile, "static " ); column += 7;

    /* jb18:  obsolete
	  // produce return type---actually builds a string no matter what
		mystring* returnType = determineReturnType();
    returnType->toFile( javafile );
		  column += returnType->length();
		returnType->destroy();  // so no waste builds up
    */

		// jb18:  find the output box and use its type
		curbox = first;  // the inputs box
    curbox = curbox->next;  // the output box
		mystring* returntype;
		if( curbox->type == NULL || (curbox->type)->equals( "" ) )
		  returntype = new mystring( "void" );
		else 
		{// have a return type, might need to change string to String
		  if( curbox->type->equals( "string" ) )
			  returntype = new mystring( "String" );
			else
			  returntype = (curbox->type)->copy();
    }
 		returntype->toFile( javafile );
	  	  column += returntype->length();
		returntype->destroy();
    // j18:  end of new stuff, continue

    fprintf( javafile, " " );   column++;

		name->toFile( javafile ); 
		  column += name->length();

		// parameter list
    curbox = first;  // the parameters box
		if( this == mainbox )
		{// special case
 		  fprintf( javafile, "( String[] args )\n" );
			if( curbox->innerLength() > 0 )
			{
			  spaces( javafile, indent );
			  fprintf( javafile, "// Your main method had parameters that\n");
			  spaces( javafile, indent );
			  fprintf( javafile, "// have been removed during translation\n");
			}
		}
		else // any method other than starting main
    	curbox->translateToJava( javafile, column );

    // jb18:  hop over the output box, already used above
		curbox = curbox->next;

    // local variables
		curbox = curbox->next;  // the local variables box
		
		spaces( javafile, indent );
		fprintf( javafile, "{\n" );

		if( this == mainbox )
		{// initialize special io class
		  spaces( javafile, indent+2 );
			fprintf( javafile, "JB.init();\n\n" );
		}

    curbox->translateToJava( javafile, indent+2 );

    curbox = curbox->next;  // the codebox
		curbox->translateToJava( javafile, indent+2 );

    fprintf( javafile, "\n" );
    spaces( javafile, indent);
		fprintf( javafile, "}// end of method " );
		name->toFile( javafile );
		fprintf( javafile, "\n" );
		
	}// methodbox

  else if( kind==paramsbox )
	{// do parameter list here
    fprintf( javafile, "( " );
		curbox = first;
		int col = indent+2; // track where on line printing for wrap
		while( curbox != NULL )
		{// spit out curbox

		  // handle the parameter and adjust col:
			col += (curbox->type)->toFileArrayAdjusted( javafile );
			fprintf( javafile, " " ); col++;
			col += (curbox->name)->toFile( javafile );

			curbox = curbox->next;

			if( curbox != NULL ) // is another one following
			{
			  fprintf( javafile, ", " ); 

				if( col > 60 )
				{// wrap to next line
				  fprintf( javafile, "\n" );
					spaces( javafile, indent+3 );
					col = indent+2;
				}// time to wrap?

			}// is another one to come?

		}// loop through all parameters
		
		fprintf( javafile, " )\n" );
		         					
	}// paramsbox

  else if( kind==localsbox )
	{// declare all the local variables
    curbox = first;
		while( curbox != NULL )
		{// ship out curbox which is a local variable declaration
			curbox->translateToJava( javafile, indent );
			curbox = curbox->next;
		}
		fprintf( javafile, "\n" ); // end of locals
	}// locals

  else if( kind==seqbox )
	{// do braces and translate inners, skip braces if codebox
	  if( outer->kind != methodbox )
		{
		  spaces( javafile, indent );
			fprintf( javafile, "{\n" );
			indent += 2;
		}

    curbox = first;
		while( curbox != NULL )
		{// ship out curbox which is an action box
			curbox->translateToJava( javafile, indent );
			curbox = curbox->next;
		}

	  if( outer->kind != methodbox )
		{
		  spaces( javafile, indent-2 );
			fprintf( javafile, "}\n" );
		}

	}// seqbox

  else if( kind==emptybox )
	{// if in context where action is expected, do ; otherwise nothing
	  if( actionContext( outer->kind, positionInInnerList() ) )
		{
		  spaces( javafile, indent );
			fprintf( javafile, "{}\n" );
		}
		
	}// emptybox

  else if( kind==branchbox )
	{// build appropriate if, if else if .... structure

    int n = innerLength();

		int k;

    curbox = first;

    // do the pairs
		for( k=1; k<n; k += 2 )
		{
		  // do the question on a line
		  spaces( javafile, indent );
			if( k > 1 )
			  fprintf( javafile, "else " );
			fprintf( javafile, "if( " );
			curbox->translateToJava( javafile, 0 );
			fprintf( javafile, " )\n" );
			
			curbox = curbox->next;  // now do the action
			curbox->translateToJava( javafile, indent+2 );

			curbox = curbox->next;
		}

		// do the else guy
		if( n==1 )
		{
		  spaces( javafile, indent );
			fprintf( javafile, "if( true )\n" );
			spaces( javafile, indent+2 );
			fprintf( javafile, "{}\n" );
		}
    spaces( javafile, indent );
		fprintf( javafile, "else\n" );
		curbox->translateToJava( javafile, indent+2 );

	}// branchbox

  else if( kind==dobox )
	{// do loop 

	  spaces( javafile, indent );
   	fprintf( javafile, "do\n" );
		curbox = first;
		curbox->translateToJava( javafile, indent+2 ); // the body
		
		curbox = curbox->next;  // the question
		spaces( javafile, indent );
		fprintf( javafile, "while( " );
		curbox->translateToJava( javafile, 0 );
		fprintf( javafile, " );\n" );

	}// dobox
    
  else if( kind==whilebox )
	{// while loop 

	  spaces( javafile, indent );
   	fprintf( javafile, "while( " );
		curbox = first;
		curbox->translateToJava( javafile, 0 ); // the question
		fprintf( javafile, " )\n" );
		
		curbox = curbox->next;  // the body
		curbox->translateToJava( javafile, indent+2 );

	}// whilebox

	else if( kind==forbox )
	{
	  box* initbox;
		box* incbox;
		initbox = first;
		curbox = initbox->next;  // question
		curbox = curbox->next;  // body
		incbox = curbox->next;  // increment 

		if( initbox->legalAssGrowSeq( 1 ) &&
		    incbox->legalAssGrowSeq( 0 ) )
		{// can translate as a for loop
		  spaces( javafile, indent );
			fprintf( javafile, "for( " );
			initbox->translateInlineToJava( javafile );
			fprintf( javafile, "; " );
			curbox = initbox->next; // the question
			curbox->translateToJava( javafile, 0 );
			fprintf( javafile, "; " );
			incbox->translateInlineToJava( javafile );
			fprintf( javafile, " )\n" );
			curbox = curbox->next;  // the body
			curbox->translateToJava( javafile, indent+2 );
		}
		else
		{// translate to equivalent while loop
		  spaces( javafile, indent );
      fprintf(javafile, "// The JBoxes for loop used features not available in Java\n");
		  spaces( javafile, indent );
			fprintf( javafile, "// so was not translated.\n" );
		  spaces( javafile, indent );
			fprintf( javafile, "// Try rewriting to only use assignment boxes\n" );
		  spaces( javafile, indent );
			fprintf( javafile, "// in the first inner box, and only assignment boxes\n");
		  spaces( javafile, indent );
			fprintf( javafile, "// and grow boxes in the fourth inner box.\n");
		}

	}// forbox

  else if( kind==idbox )
	{// most just become a name, string type gets wrapped unless
	 //   is lhs of assbox
	  box* sourcebox = determineIdSource();// maybe find who I refer to

	  if( sourcebox != NULL &&
		    (sourcebox->type)->equals("string") &&
		    (outer->kind != assbox || outer->first != this)
			)
		{// string ids are always wrapped, except if first in assbox
      fprintf( javafile, "(new String(" );
			name->toFile( javafile );
			fprintf( javafile, "))" );
		}
		else
      name->toFile( javafile );	  
	}// idbox

  else if( kind==callbox )
	{
	  // determine if is procedural call or functional call
		//  (so know whether to put on a line with ; at end, or not)
    // Can tell from the context

    int proc;
		proc = actionContext( outer->kind, 
		                      positionInInnerList() );
    
	  curbox = first;  // the function name box

    if( proc )
		  spaces( javafile, indent ); 

    // branch to handle all bims specially, but some are more
		// special than others.  flag special is set in branching,
		//  non-special bims and built-ins handle arguments same

    // jb18:  eliminating a bunch of bims, changing print
				
		int special = 0;  // only a few are special

    // so, this structure handles name translation, and does
		// the rest for special ones
		if( curbox->kind == idbox )
		{// only idbox can be a bim

      // non-special bims listed first
  	  if( curbox->name->equals("_getLine") )
			{
			  fprintf( javafile, "JB.getLine" );
			}
			else if( curbox->name->equals("_ascii") )
			{
			  fprintf( javafile, "(int)" );
			}
			else if( curbox->name->equals("_open") )
			{
			  fprintf( javafile, "JB.open" );
			}
			else if( curbox->name->equals("_close") )
			{
			  fprintf( javafile, "JB.close" );
			}
			else if( curbox->name->equals("_fget") )
			{
			  fprintf( javafile, "JB.fget" );
			}
			else if( curbox->name->equals("_fput") )
			{
			  fprintf( javafile, "JB.fput" );
			}
			else if( curbox->name->equals("_eof") )
			{
			  fprintf( javafile, "JB.eof" );
			}
			else if( curbox->name->equals("_sin") )
			{
			  fprintf( javafile, "Math.sin" );
			}
			else if( curbox->name->equals("_cos") )
			{
			  fprintf( javafile, "Math.cos" );
			}
			else if( curbox->name->equals("_tan") )
			{
			  fprintf( javafile, "Math.tan" );
			}
			else if( curbox->name->equals("_asin") )
			{
			  fprintf( javafile, "Math.asin" );
			}
			else if( curbox->name->equals("_acos") )
			{
			  fprintf( javafile, "Math.acos" );
			}
			else if( curbox->name->equals("_atan") )
			{
			  fprintf( javafile, "Math.atan" );
			}
			else if( curbox->name->equals("_exp") )
			{
			  fprintf( javafile, "Math.exp" );
			}
			else if( curbox->name->equals("_log") )
			{
			  fprintf( javafile, "Math.log" );
			}
			else if( curbox->name->equals("_sqrt") )
			{
			  fprintf( javafile, "Math.sqrt" );
			}
			else if( curbox->name->equals("_ceil") )
			{
			  fprintf( javafile, "Math.ceil" );
			}
			else if( curbox->name->equals("_floor") )
			{
			  fprintf( javafile, "Math.floor" );
			}
			else if( curbox->name->equals("_abs") )
			{
			  fprintf( javafile, "Math.abs" );
			}
			else if( curbox->name->equals("_random") )
			{
			  fprintf( javafile, "JB.random" );
			}
			else if( curbox->name->equals("_seed") )
			{
			  fprintf( javafile, "JB.seed" );
			}
			else if( curbox->name->equals("_isInt") )
			{
			  fprintf( javafile, "JB.isInt" );
			}
			else if( curbox->name->equals("_isFloat") )
			{
			  fprintf( javafile, "JB.isFloat" );
			}
			else if( curbox->name->equals("_toInt") )
			{
			  fprintf( javafile, "JB.toInt" );
			}
			else if( curbox->name->equals("_toFloat") )
			{
			  fprintf( javafile, "JB.toFloat" );
			}

			// special bims listed last
			else if( curbox->name->equals("_halt") )
			{
			  fprintf( javafile, "System.exit(0)" );
				special = 1;
      }
			else if( curbox->name->equals("_char") )
			{
			  fprintf( javafile, "(char) (" );
				curbox = curbox->next; // the arg
        curbox->translateToJava( javafile, 0 );
				fprintf( javafile, ")" );
			  special = 1;
			}
			else if( curbox->name->equals("_print") )
			{// easy, except args must have + in between
			  fprintf( javafile, "System.out.print( " );

  		  curbox = curbox->next; // first arg
    		while( curbox != NULL )
	    	{// ship out argument in curbox
          curbox->translateToJava( javafile, 0 );
  		    curbox = curbox->next; // move to next argument
  			  if( curbox != NULL )
    			  fprintf( javafile, " + " );
  		  }// ship out all args

				fprintf( javafile, " )" );

				special = 1;
			}

			else if( curbox->name->equals("_intToString") )
			{// call Integer.toString
        curbox = curbox->next;  // is the argument
				fprintf( javafile, "(new Integer(" );
				curbox->translateToJava( javafile, 0 );
				fprintf( javafile, ")).toString()" );

			  special = 1;
			}
			else if( curbox->name->equals("_floatToString") )
			{// call Double.toString
        curbox = curbox->next;  // is the argument
				fprintf( javafile, "(new Double(" );
				curbox->translateToJava( javafile, 0 );
				fprintf( javafile, ")).toString()" );

			  special = 1;
			}
			else if( curbox->name->equals("_charToString") )
			{// call Integer.toString
        curbox = curbox->next;  // is the argument
				fprintf( javafile, "(new Character(" );
				curbox->translateToJava( javafile, 0 );
				fprintf( javafile, ")).toString()" );

			  special = 1;
			}
			else if( curbox->name->equals("_charsToString") )
			{// create a new String using the given char[] argument
        curbox = curbox->next;  // is the argument
				fprintf( javafile, "(new String(" );
				curbox->translateToJava( javafile, 0 );
				fprintf( javafile, "))" );

			  special = 1;
			}
		  else if( curbox->name->equals("_length") )
		  {// form is:    argument.length()
        curbox = curbox->next;  // is the argument
				// for safety's sake, feed argument into constructor,
				//  apply function to it
				fprintf( javafile, "(new String(" );
				curbox->translateToJava( javafile, 0 );
				fprintf( javafile, ")).length()" );

			  special = 1; // indicating is all done right here
		  }
		  else if( curbox->name->equals("_charAt") )
		  {// form is:    argument.length()
        curbox = curbox->next;  // is the argument---the String
				// for safety's sake, feed argument into constructor,
				//  apply function to it
				fprintf( javafile, "(new String(" );
				curbox->translateToJava( javafile, 0 );
				fprintf( javafile, ")).charAt(" );
				curbox = curbox->next; // the index
				curbox->translateToJava( javafile, 0 );
				fprintf( javafile, ")" );

			  special = 1; // indicating is all done right here
		  }
			else if( curbox->name->equals("_null") )
			{
			  fprintf( javafile, "null" );
				special = 1;
			}
			else if( curbox->name->equals("_this") )
			{
			  fprintf( javafile, "this" );
				special = 1;
			}
			else if( curbox->name->equals("_destroy") )
			{
			  fprintf( javafile, "/* 'destroy' is automatic in Java */" );
				special = 1;
			}

      else 
			  (curbox->name)->toFile( javafile );

		}// idbox
		else // more complex "name" to translate
    	curbox->translateToJava( javafile, 0 ); // function "name"

    // now, if not special, do argument list
		if( ! special )
		{// not special form
		  curbox = curbox->next; // first arg

		  if( curbox != NULL )
  		  fprintf( javafile, "( " ); // at least one arg
	  	else
		    fprintf( javafile, "()" );

  		while( curbox != NULL )
	  	{// ship out argument in curbox

        curbox->translateToJava( javafile, 0 );

		    curbox = curbox->next; // move to next argument

			  if( curbox != NULL )
  			  fprintf( javafile, ", " );
	  		else
          fprintf( javafile, " )" );
		
		  }// ship out all args

    }// not special form

    if( proc )
		  fprintf( javafile, ";\n" );

	}// callbox

  else if( kind==memberbox )
	{// do the first box, followed by . and the name
	  
		curbox = first;
		curbox->translateToJava( javafile, 0 );
		fprintf( javafile, "." );
		name->toFile( javafile );
			  
	}// memberbox

  else if( kind==arraybox )
	{// first [ second ]
	  curbox = first;
		curbox->translateToJava( javafile, 0 );
		fprintf( javafile, "[ " );
		curbox = curbox->next;
		curbox->translateToJava( javafile, 0 );
		fprintf( javafile, " ]" );

	}// arraybox

  else if( kind==returnbox )
	{// return followed by first followed by ; 
	  spaces( javafile, indent );
		fprintf( javafile, "return " );
		curbox = first;
		curbox->translateToJava( javafile, 0 );
		fprintf( javafile, ";\n" );
	}// returnbox

  else if( kind==opbox )
	{// ( first name second )
	  fprintf( javafile, "(" );
		curbox = first;
		curbox->translateToJava( javafile, 0 );
		name->toFile( javafile );
		curbox = curbox->next;
		curbox->translateToJava( javafile, 0 );
		fprintf( javafile, ")" );
	}// opbox

  else if( kind==unopbox )
	{// name first
	  name->toFile( javafile );
		curbox = first;
		curbox->translateToJava( javafile, 0 );
  }// unopbox

	else if( kind==assbox )
	{// first name second ; on a line
	  spaces( javafile, indent );
		curbox = first;
		curbox->translateToJava( javafile, 0 );
		fprintf( javafile, " " );
		name->toFile( javafile );
		fprintf( javafile, " " );
		curbox = curbox->next;
		curbox->translateToJava( javafile, 0 );
		fprintf( javafile, ";\n" );
	}// assbox

  else if( kind==growbox )
	{// name followed by first
	  spaces( javafile, indent );
		name->toFile( javafile );
		curbox = first;
		curbox->translateToJava( javafile, 0 );
    fprintf( javafile, ";\n" );
	}// growbox

  else if( kind==array2dbox )
	{// first [ second ][ third ]
		curbox = first;
		curbox->translateToJava( javafile, 0 );
		fprintf( javafile, "[ " );
		curbox = curbox->next;
		curbox->translateToJava( javafile, 0 );
		fprintf( javafile, " ][ " );
		curbox = curbox->next;
		curbox->translateToJava( javafile, 0 );
		fprintf( javafile, " ]" );
	}// array2dbox

  else if( kind == newopbox )
	{// new name  or new name [ first ] or new name [ first ][ second ]
	  fprintf( javafile, "new " );
		name->toFileArrayAdjusted( javafile ); // to get String, double

		curbox = first;
		if( curbox->kind != emptybox )
		{
		  fprintf( javafile, "[ " );
			curbox->translateToJava( javafile, 0 );
			fprintf( javafile, " ]" );
			curbox = curbox->next;
  		if( curbox->kind != emptybox )
	  	{
		    fprintf( javafile, "[ " );
			  curbox->translateToJava( javafile, 0 );
			  fprintf( javafile, " ]" );
			}// have 2 dims		
		}// have at least one dimension
		else // scalar
			fprintf( javafile, "()" );
  
	}// newopbox

  else if( kind==intbox || kind==floatbox ||
	         kind==booleanbox
				 )
	{// literal with no decorations
	  name->toFile( javafile );
	}// literal with no decorations

	else if( kind==charbox )
	{// ' name '
	  fprintf( javafile, "'" );
	  name->toFileStringForm( javafile );
	  fprintf( javafile, "'" );
	}// charbox

	else if( kind==stringbox )
	{// " name "
	  fprintf( javafile, "(new String(\"" );
	  name->toFileStringForm( javafile );
	  fprintf( javafile, "\"))" );
	}

  // jb18
  else if( kind==javabox )
	{
		// translate java code to box format
    int status = translateJava();
		
		if( status != 0 )
		{
		  exporterror = 1;
			return;
		}
    else
		{
		  // export the box format if this javabox translated correctly
		  first->translateToJava( javafile, indent );
    }

	}// javabox

  else if( kind==lessonbox )
  {// do nothing
  }

	else
	{
	  spaces( javafile, indent );
		fprintf( javafile, "Kind %d not done yet\n", kind );
	}

}// translateToJava

// jb16: auxiliary guy --- replace inner at pos with newbox
void box::replaceInnerAt( box* newbox, int pos )
{
  box* vicbox;
	box* prevbox;

  if( pos > 1 )
	{// replace other than first
	  // access relevant boxes
    prevbox = accessInnerAt( pos-1 );
		if( prevbox == NULL )
		  errorexit("box::replaceInnerAt---bad pos-1");
    vicbox = prevbox->next;  // for efficiency 
		if( vicbox == NULL )
		  errorexit("box::replaceInnerAt---bad pos-1+1");
		// make previous point now to new
		prevbox->next = newbox;
		// make new point to victim's
		newbox->next = vicbox->next;
		newbox->outer = vicbox->outer;
    // if victim is current, make new
		if( pos == cursor )
		  current = newbox;
	}
	else
	{// replacing first box
	  vicbox = first;
		first = newbox;
		if( cursor == 1 )
		  current = newbox;
		newbox->next = vicbox->next;
		newbox->outer = vicbox->outer;
	}

  vicbox->destroy();

  newbox->setDepthAndColors( color, depth+1 );

  int junk;
  newbox->figureSizeLocs( junk );

}

// jb16:  convenient guy
// return whether this box is an action or not
int box::isAction()
{
	return actionContext( outer->kind, positionInInnerList() );
}

// jb16:  had error, this guy must manage aspect of first,
//        like box::stripValue does 
void box::promoteInnerValue()
{
  // jboxes1d:  small leak?
  if( value != NULL )
    value->destroy();
  value = first->value;
	first->value = NULL;
  if( type != NULL )
    type->destroy();
	type = first->type;

	first->type = NULL;
	// manage first's aspect---can't leave it using 'v'
	if( first->aspect == 'v' )
	{
    first->setAspect( 'f' );
    first->fixupReality(0);
	}
}

// jb16: go through this box and
//       translate all javaboxes encountered --- is recursive
// jb18:  have this guy return status code so stops as soon as
//         hits a bad one and can inform the two places where it
//         is called
int box::translateJava()
{
  int status = 0;

  // translate this box, if needed
	if( kind==javabox )
	{
	  status = tran->translate( this );
		if( status != 0 )
		{// encountered error in translation, raise error
		  tran->processStatusCode( status );
			return status;  
		}

  }

  // loop through and recursively translate all inners

  box* curbox = first;

  while( curbox != NULL && status == 0)
  {// translate curbox or detect error
    status = curbox->translateJava();
    curbox = curbox->next;
  }

  fixupReality(0);  // jb16:  was having trouble with display
	                  //        so did this on a sort of guess!

  return status;

}// box::translateJava

// remove this box from its inner list
void box::remove()
{
  box* prevbox;

  // grab previous to me
	int index = positionInInnerList();

  if( index == 1 )
  {// is no previous to me
    outer->first = next;
    outer->current = next;

    if(next==NULL)
    {// deleting first and only inner box,
     // set outer correctly
      outer->cursor = 0;
      outer->setPart( 'i' );  // probably already on innerlist
      outer->inside = 'y'; 
    }
            
  }
  else
  {// have a previous

	  // access it
	  prevbox = outer->accessInnerAt( index - 1 );

    prevbox->next = next;
    outer->current = next;

    if(next==NULL)
    {// deleting last inner box, current must become
     // the previous
      outer->current = prevbox;
      outer->cursor -= 1;
    }

  }
          
  // deallocate my innards and my space
  destroy();

}

// jb17:
// return the instance box
// in the heap for the instancemethod box that this box is
// nested inside, or NULL if none such
box* box::findInstance()
{
  // search outward until hit NULL or classmethodbox or
	// instancemethodbox
	box* curbox;

	curbox = outer;

	while( curbox != NULL &&
	       curbox->kind != classmethodbox &&
				 curbox->kind != instancemethodbox 
			 )
	curbox = curbox->outer;

	if( curbox == NULL || curbox->kind == classmethodbox )
	  return NULL;

  // jb17:  had this below, now realize must search heap for
	//         guy with correct name, matching curbox's type
  //	return curbox->source;  // source of instancemethodbox is the instance

  // jb17:  heap is a global box, so just search it
  box* temp;
	temp = heap->searchInner( curbox->type );

	return temp;

}// findInstance

// jb18:  tell whether this box is a breakpoint
int box::isBreakpoint()
{
  return props % 2;
}

// jb18:  flip breakpoint property of this box if it is
//        of appropriate kind
void box::toggleBreakpoint( int& didit )
{
  box* temp = findCodebox();

  if( temp != NULL )
	{// this box is a codebox or inward nested
    if( props % 2 == 0 )
	    props += 1;
  	else
	    props -= 1;
		didit = 1;
	}
	else // silly box to put a breakpoint on, so don't!
	  didit = 0;
}

// jb19:  set first inner as current, return it or NULL if none
box* box::rewind()
{
  if( first == NULL )
	  return NULL;
	else
	{// have an inner
	  current = first;
		cursor = 1;
		return current;
	}
}

// jb21:  new methods for lessons
//------------------------------------------------

// move my current inner to the given position, 1 is first,
//  errorexit if there aren't that many inner boxes
void box::moveCurrentTo( int position )
{
  box* curbox = first;
  int k = 1;

  while( k < position && curbox != NULL )
  {
    k++;
    curbox = curbox->next;
  }

  if( curbox == NULL )
    errorexit("bad position for box::moveCurrentTo");
  else
  {
    current = curbox;
    cursor = position;
  }

}

// from this box, produce combox ready for recording
box* box::getComboxAtFocus()
{
  box* newbox;
  box* owner;

  // build the new empty combox
  newbox = new box();
  newbox->build( combox );
  newbox->setAspect('a'); newbox->moveInside();

  if( kind == lessonbox )
  {// focus is on or in lessonbox
// jb26:  don't want this anymore:
//    setAspect( 'f' );

    // focus now on mark or inner box,
    // now insert the new combox and return it
    addInnerAtCursor( newbox );   
    newbox->fixupReality(0);
    return newbox;
  }
  else if( kind == notebox )
  {
    owner = outer;
    owner->moveCurrentTo( positionInInnerList() );
    owner->addInnerAtCursor( newbox );   
    newbox->fixupReality(0);
    return newbox;
  }
  else if( kind == parbox )
  {// this box is inside notebox to add combox after
    box* nbox = outer;  // the enclosing notebox
    owner = nbox->outer;  // the lessonbox
    owner->moveCurrentTo( nbox->positionInInnerList() );
    owner->addInnerAtCursor( newbox );
    newbox->fixupReality(0);
    return newbox;
  }
  else if( kind == combox )
  {// focus is on or in a combox, just add a combox after or use
   // this one
   if( inside == 'n' )
   {// add a combox after this box
      owner = outer;
     // make sure the lessonbox has this box as its current inner box
     owner->moveCurrentTo( positionInInnerList() );
     owner->addInnerAtCursor( newbox );   
     newbox->fixupReality(0);
     return newbox;
   }
   else
   {// use this one
     newbox->destroy(); // to save memory resources, turned out not used
     return this;
   }
  }
  else
  {
    errorexit("bad kind in box::getComboxAtFocus");
    return NULL;
  }

}// getComboxAtFocus

// put the symbols in standard C string s at the end of the
// name string in this box
void box::appendToCommand( char* s )
{
  int n = name->length()+1;
  int k=0;
  while( s[k] != '\0' ) 
  {
    name->insertCharAt( n, s[k] );
    k++; n++; name->advanceCursor();
  }

  fixupReality(0);

}

// jb23:  new methods for import/export of class boxes

int box::exportToFile()
{
  // prepare output file with same name as this box
  //  (rely on author to make sure it has a good name)
  FILE* outfile;
  char* filename;

  mystring* tail = new mystring( ".cbx" );
  mystring* myname = name->copy();
  myname->append( tail );

  filename = myname->charCopy();

  outfile = fopen( filename, "w" );

  delete filename;

  if( outfile == NULL )
    return -1;
  else
  {// opened file for output

    save( outfile );

    fclose( outfile );
    return 1;
  }
}// exportToFile

// load a classbox with same name as this one and insert it
// after this one
int box::buildFromFile()
{
  FILE* infile;
  char* filename;

  mystring* tail = new mystring( ".cbx" );
  mystring* myname = name->copy();
  myname->append( tail );

  filename = myname->charCopy();

  infile = fopen( filename, "r" );

  delete filename;

  if( infile == NULL )
    return -1;
  else
  {// opened file for input

    box* newbox = new box();

    newbox->load( infile );
    fclose( infile );

    // insert newbox after this box and put "cursor" on this box
    box* owner = whoOuter();

    owner->addInnerAtCursor( newbox );
    owner->current = this;
    owner->cursor--;

    return 1;
  }

}// buildFromFile

// jb24:  permission related methods

// use the sym to determine code and set permission in props
void box::setPerm( char sym )
{
  int code;

  // translate from sym as a digit or uppercase digit to corresponding
  // number
  if( '0'<=sym && sym<='7' )
    code = sym - '0';
  else// uppercase digits must be done manually
  {
    if( '!'<=sym && sym<='%' )
      code = sym-32;  // 34 is " is skipped, can't be there, anyway
    else if( sym=='@' )
      code = 2;
    else if( sym=='^' )
      code = 6;
    else if( sym=='&' )
      code = 7;
    else if( sym==')' )
      code = 0;
    else
      errorexit("illegal sym in box::setPerm");
  }

  // extract the individual bits of code
  int bp, kill, add, change;
  kill = code % 2;
    code /= 2;
  add = code % 2;
    code /= 2;
  change = code % 2;
  // extract existing breakpoint property 
  bp = props % 2;
  // combine bits to produce new props
  props = ((change*2 + add)*2 + kill)*2 + bp;
}// setPerm

// set permission on this box and all inward recursively to sym
void box::setPermInward( char sym )
{
  setPerm( sym );  // set permission on this box

  box* curbox = first;

  while( curbox != NULL )     // set inward on all inners
  {
    curbox->setPermInward( sym );
    curbox = curbox->next;
  }

}// setPermInward

int box::canKill()
{
  if( props & 2 )
    return 1;
  else
    return 0;
}// canKill

int box::canAdd()
{
  if( props & 4 )
    return 1;
  else
    return 0;
}// canAdd

int box::canChange()
{
  if( props & 8 )
    return 1;
  else
    return 0;
}// canChange

// scan outward from this box to produce a mystring telling
// how to get to this box
mystring* box::getBoxLocString()
{
  mystring* temp;
  mystring* w;  // utility mystring
  char s[12];  // utility string

  if( kind == univbox )
  {
    temp = new mystring(""); // pass back empty string for universe
  }
  else
  {// must have an outer, get its loc string and append detail
   // to finish getting to me

   box* owner = outer;

   temp = owner->getBoxLocString();

   int knd = owner->whichKind();

   // if owner is kind that has named inners, use name, else use #
   //  jb6a:  for some kinds---univ is the only one I know---must
   //         avoid some of its inners since their names may be
   //         bad (lessonboxes)
   if( (knd == univbox && kind!=lessonbox) || 
       knd == databox || knd == classdatabox ||
       knd == classmethodsbox || knd == instancedatabox ||
       knd == instancemethodsbox || knd == paramsbox ||
       knd == localsbox || knd == allclassdatabox )
   {// owner is kind that has named inners
    // append . followed by my name to temp
     temp->insertCharAt( temp->length()+1, '.' );
     temp->append( name );
   }
   else
   {// owner is kind that has numbered inners
    // append # followed by my position to temp
     int index = positionInInnerList();
     sprintf( s, "#%d", index );
     w = new mystring( s );
     temp->append( w );
     w->destroy();
   }

  }// have an outer

  return temp;

}// getBoxLocString

// produce a mystring specifying textually the location of this
// box, and its inner part, in the universe
//    inner part (if appropriate) is determined either from the
//    passed in mouse info, or from the box part and aspect
//  (uses the recursive getBoxLocString for the nesting
mystring* box::getLocString( int useMouseInfo, char tpart, int tindex )
{
  mystring* temp;
  mystring* w;  // utility mystring
  char s[12];  // utility string

  temp = getBoxLocString();

  // have temp holding location string to this box, now need to
  // append part info, as needed and depending on useMouseInfo
  if( !useMouseInfo )
  {// use internals of this box to determine part appendage
    if( isInside() )
    {// focus is inside, so need to adjust for parts
      if( aspect == 'f' && part == 'i' )
      {// focus is on mark for inner list, append #0
        sprintf( s, "#%d", 0 );
        w = new mystring( s );
        temp->append( w );
        w->destroy();
      }
      else 
      {// focus is on one of the four string parts, append
       // it after a comma
       // and then append the symbol number
        if( (aspect == 'f' && part == 'n') ||
            aspect == 'a' )
          sprintf( s, ",n%d", name->whereCursor() );
        else if( aspect == 'f' && part == 't' )
          sprintf( s, ",t%d", type->whereCursor() );
        else if( aspect == 'd' )
          sprintf( s, ",d%d", doc->whereCursor() );
        else if( aspect == 'v' ||
                 (aspect == 'f' && part == 'v' )
               )
          sprintf( s, ",v%d", value->whereCursor() );
        else
          errorexit("unknown aspect/part in box::getLocString");

        w = new mystring( s );
        temp->append( w );
        w->destroy();
      }// focus on a string part
    }
    // else no part adjustment needed
  }// not using mouse info passed in

  else
  {// using mouse info provided
    if( tpart == '.' )
    {// pointing to the box itself, no part info needed
    }
    else if( tpart == 'i' )
    {// not the box itself, on inner, must be on symbol 0 of inner list
     // so append #0 to the box's string
      sprintf( s, "#%d", 0 );
      w = new mystring( s );
      temp->append( w );
      w->destroy();
    }
    else 
    {// use the given part and index
      sprintf( s, ",%c%d", tpart, tindex  );
      w = new mystring( s );
      temp->append( w );
      w->destroy();
    }

  }// using mouse info provided

  return temp; // ship out the location string

}// getLocString

// search outward from this box and find the outward nested lessonbox
//  and return it, or return NULL
box* box::getLessonbox()
{
  box* curbox = this;
  while( curbox != NULL && curbox->kind != lessonbox )
    curbox = curbox->outer;

  return curbox;

}

// little utilities for the fsm
int letter( char x )
{
  return x=='_' || ('a'<=x && x<='z') || ('A'<=x && x<='Z');
}

int digit( char x )
{
  return '0'<=x && x<='9';
}

// this box must be the universe, or is an error, then:
// given locstring and style from the notebox, which give the
// desired arrowhead location and style of arrowhead, produce
// coords (ax,ay) in universe box grid of ulc, and the width and
// height of the framed symbol, mystring, or box, depending on style
//   In other words, this is the guy that converts from the readable,
//   more robust, high level locstring format to purely geometric
//   format, together with convenient "astyle" info coding up how the
//   arrowhead should be drawn
//   'a' means line with head to (ax,ay) in universal coords
//   'l' means just the line
//   'f' means frame with ulc at (ax,ay) and size aw,ah
//   'n' means none, don't want any arrow (jb5a)
//   'e' means error, draw stub from note to center of window
//
// jb6a:  if locstring starts with '?' treat differently, purely
//        offset within a viewer's rectangle
// return - if have box info, viewer letter otherwise
char box::getInfoFromLocString( mystring* locstring,  mystring* style,
                      float& ax, float& ay, float& aw, float& ah,
                      char& astyle,
                      float cvw, float cvh, float pvw, float pvh )
{
  // jb6a:  pick off the initial ? right away
  if( locstring->charAt( 1 ) == '?' )
  {// extract offset info and return the viewer letter
   // jb7a:  must produce ax,ay as actual world coords, not
    //       percents

    char vid = locstring->charAt(2);  // will be 'c' or 'p'

    int k;
    long value;
    char ch;

    k=4;  // position 3 holds a basically worthless @
    mystring* xstring = new mystring("");

    do{
      ch = locstring->charAt(k);
      if( ch != ':' )
      {
        xstring->appendChar( ch );
        k++;
      }
    }while( ch != ':' );
    
    xstring->toInt( value );
    xstring->destroy();
    ax = value;

    // k is index of the :, move ahead for getting ystring
    k++;
    mystring* ystring = new mystring("");

    do{
      ch = locstring->charAt(k);
      if( digit(ch) )
      {
        ystring->appendChar( ch );
        k++;
      }
      else
        k = locstring->length()+1;
    }while( k<=locstring->length() );

    ystring->toInt( value );
    ystring->destroy();
    ay = value;

    float vwidth, vheight; // size of relevant viewer

    if( vid == 'c' )
    {// get size of console viewer
      vwidth = cvw;
      vheight = cvh;
    }
    else
    {// get size of ports viewer
      vwidth = pvw;
      vheight = pvh;
    }

    // adjust ax,ay to be world coords, not percents
    ax = vwidth * (ax / 1000.0 );
    ay = vheight * (1 - ay / 1000.0 );

    if( style->equals("") || style->equals("n") )
      astyle = 'n';
    else if( style->equals("a") )
      astyle = 'a';
    else if( style->equals("l") )
      astyle = 'l';
    else
      astyle = 'e';

    // will be 'c' or 'p', as opposed to '-'
    return vid;  // signals no box info
  }
  
  // jb5a: allow empty style box or 'n' to mean no arrow
  if( style->equals("") || style->equals("n") )
  {
    astyle = 'n';
    return '-';  // signal have box info
  }

  // errorexit if called other than on the universe box
  if( kind != univbox )
    errorexit(
          "box::getInfoFromLocString can only be called on universe");

  int locstringlength = locstring->length();

  // will put a fake '?' to mark end, error if the author has already
  // put one in the locstring
  int j;
  for( j=1; j<=locstringlength; j++ )
  {
    if( locstring->charAt( j ) == '?' )
    {
      astyle = 'e';
      return '-';
    }
  }

  locstring->appendChar( '?' );  // artificial final char

  // scan the locstring starting from this universe box and obtain
  // ax, ay for the ulc of the box that is targeted as well as noting
  // part info, if any

  box* curbox = this;
  float curx = 0, cury = 0;  // offset from universe ulc to current spot
  char ch;  // use to conveniently hold symbol k from locstring
  int k = 1;  // location in the string
  int fsmstate = 0;
  int error = 0;

  long index;   // utilities for converting mystring to int
  int okay;

  mystring* targetstring; // string that tells which inner box

  char partinfo = ' ';  // tells which part was detected, ' ' means
                        // none, just the box
  int partindex = -1; // index in part, if any, -1 means the entire part
  float inx=0, iny=0;  // x,y offset inside the specified box

  int done = 0;  // flag for exiting processing when hit '?'

  while( !done && !error )
  {
    ch = locstring->charAt( k ); k++; // move ahead for next time around

    if( fsmstate == 0 )
    {
      if( ch == '.' )
        fsmstate = 1;
      else if( ch == '#' )
        fsmstate = 2;
      else if( ch == '@' )
        fsmstate = 8;
      else if( ch == ',' )
        fsmstate = 5;
      else if( ch == '?' )
        done = 1;
      else
        error = 1;
    }

    else if( fsmstate == 1 )
    {
      if( letter( ch ) )
      {
        targetstring = new mystring("");
        targetstring->appendChar( ch );
        fsmstate = 3;
      }
      else
        error = 1;
    }

    else if( fsmstate == 2 )
    {
      if( digit( ch ) )
      {
        targetstring = new mystring("");
        targetstring->appendChar( ch );
        fsmstate = 4;
      }
      else
        error = 1;
    }

    else if( fsmstate == 3 )
    {
      if( letter( ch ) || digit( ch ) || ch=='_' || ch==' ' )
      {
        targetstring->appendChar( ch );
        fsmstate = 3;
      }
      else if( ch == '.' || ch == '#' || ch == ',' || ch == '@' ||
               ch == '?'
             )
      {// have an inner box specified by name, moving on to next step
        // find inner box with targetstring as its name
        curbox = curbox->searchInner( targetstring );

        if( curbox == NULL )
          error = 1;
        else
        {// found the inner box with desired name

          // update offset
          curx += curbox->ulcx;
          cury += curbox->ulcy;

          // clean up string memory space
          targetstring->destroy();

          if( ch == '.' )
            fsmstate = 1;
          else if( ch == '#' )
            fsmstate = 2;
          else if( ch == ',' )
            fsmstate = 5;
          else if( ch == '@' )
            fsmstate = 8;
          else if( ch == '?' )
            done = 1;
          else
            error = 1;
        }// found inner box with desired name
      }
      else
        error = 1;
    }// fsmstate 3

    else if( fsmstate == 4 )
    {
      if( digit( ch ) )
      {
        targetstring->appendChar( ch );
        fsmstate = 4;
      }
      else if( ch == '.' || ch == '#' || ch == ',' || ch == '@' ||
               ch == '?'
             )
      {// have an inner box specified by number

        okay = targetstring->toInt( index );
                 
        if( !okay )
          error = 1;
        else
        {// have legal index

          if( index == 0 )
          {
            partinfo = 'i';  
            partindex = 0;  
          }
          else
          {// find the inner box with desired position

            curbox = curbox->accessInnerAt( (int) index );

            if( curbox == NULL )
            {
              error = 1;
            }
            else
            {
              curx += curbox->ulcx;
              cury += curbox->ulcy;
            }
          }// found inner box with desired position

          // move on
 
          if( ch == '.' )
            fsmstate = 1;
          else if( ch == '#' )
            fsmstate = 2;
          else if( ch == ',' )
            fsmstate = 5;
          else if( ch == '@' )
            fsmstate = 8;
          else if( ch == '?' )
            done = 1;
          else
            error = 1;
      
        }// got something

        targetstring->destroy();  // tidy up no matter what

      }// have a good symbol
      else
        error = 1;

    }// fsmstate 4

    else if( fsmstate == 5 )
    {// have curbox, curx,y, now a part is going to be specified

      if( ch=='a' || ch=='d' || ch=='n' || ch=='t' || ch=='v' )
      {
        partinfo = ch;
        fsmstate = 6;
      }
      else
        error = 1;
      
    }// fsmstate 5

    else if( fsmstate == 6 )
    {
      if( digit(ch) )
      {// start collecting the index in targetstring
        targetstring = new mystring("");
        targetstring->appendChar( ch );
        fsmstate = 7;
      }
      else if( ch == '@' || ch == '?' )
      {// have noted a part, leaving without index
        partindex = -1;

        if( ch == '@' )
        {
          fsmstate = 8;
        }
        else // must be ?
        {// stopping with just the part, no offset
          done = 1;
        }
      }
      else
        error = 1;
    }// fsmstate 6

    else if( fsmstate == 7 )
    {
      if( digit( ch ) )
      {
        targetstring->appendChar( ch );
        fsmstate = 7;
      }
      else if( ch == '@' || ch == '?' )
      {// gather the index and move on
        
        okay = targetstring->toInt( index );
        if( okay )
        {
          partindex = (int) index;
          fsmstate = 8;
        }
        else
          error = 1;

        targetstring->destroy();

        if( ch == '?' )
          done = 1;

      }// hit @ or ?
      else
        error = 1;
    }// fsmstate 7

    else if( fsmstate == 8 )
    {
      if( digit( ch ) )
      {
        targetstring = new mystring("");
        targetstring->appendChar( ch );
        fsmstate = 9;      
      }
      else
        error = 1;      
    }// fsmstate 8

    else if( fsmstate == 9 )
    {
      if( digit( ch ) )
      {
        targetstring->appendChar( ch );
        fsmstate = 9;
      }
      else if( ch == ':' )
      {// gather the inner x offset and move on
        
        okay = targetstring->toInt( index );
        if( okay )
        {
          inx = (((int) index)/1000.0) * curbox->width;
          fsmstate = 10;
        }
        else
          error = 1;

        targetstring->destroy();
      }// ch is :
      else
        error = 1;     
    }// fsmstate 9

    else if( fsmstate == 10 )
    {
      if( digit( ch ) )
      {
        targetstring = new mystring("");
        targetstring->appendChar( ch );
        fsmstate = 11;      
      }
      else
        error = 1;           
    }// fsmstate 10

    else if( fsmstate == 11 )
    {
      if( digit( ch ) )
      {
        targetstring->appendChar( ch );
        fsmstate = 11;
      }
      else if( ch == '?' )
      {
        okay = targetstring->toInt( index );
        if( okay )
        {
          iny = - (((int) index)/1000.0) * curbox->height;
        }
        else
          error = 1;

        targetstring->destroy();

        done = 1;  // yea!
      }
      else
        error = 1;
    }// fsmstate 11

    else
      errorexit("box::getInfoFromLocString: unknown fsmstate" );

  }// end of fsm processing

  // whatever is going on, remove the ? on end
  locstring->deleteCharAt( locstring->length() );

  if( error )
  {// hit error somewhere, leave
    astyle = 'e';
    return '-';
  }

  // no error found, finish refining the outputs
   //   Note that many officially approved final states can only be
   //   attained by manually editing the locstring, such as deleting
   //   the part index or the inner offset or both

  // no matter what state was in when hit '?', have
  // curbox, curx,y, partinfo, partindex, inx, iny all accurate for
  // that situation, so go on based purely on this info, and
  // now using style 

  if( style->equals( "a" ) || style->equals( "l" ) )
  {// just point to the point obtained
    ax = curx + inx;
    ay = cury + iny;
    astyle = style->charAt( 1 );
      aw = 0; ah = 0;  // shouldn't be used, anyway
  }
 
  else if( style->charAt(1) == 'f' && style->length() == 2 
         )
  {//  style starts with "f" and has length 2:

    if( style->charAt(2) == 'b' )
    {// frame the entire box
      ax = curx; ay = cury;  // mark frame ulc as ulc of box
      aw = curbox->width;  
      ah = curbox->height; // set size of frame to size of box
      astyle = 'f';       // set style to frame
    }

    else if( style->charAt(2) == 'p' )
    {// frame a part
      if( partinfo == ' ' || partinfo == 'i' )
      {// locstring specified no part
        astyle = 'e';  
      }
      else
      {// have a legitimate string part specified by locstring
        // determine ulc (in inx,y), width,height (in aw, ah)
        // of the specified part

        // index -1 indicates want the info for entire string part
        curbox->locatePartOrSymbol( partinfo, -1, 
                                    inx, iny, aw, ah, error );
        
        if( error )
        {
          astyle = 'e';
          return '-';
        }

        ax = curx + inx;  ay = cury + iny;
        astyle = 'f';
      }// frame a legitimate string part

    }// wanting to frame a part

    else if( style->charAt(2) == 's' )
    {// frame a symbol, including mark for inner list

      if( partinfo == ' ' || partindex < 0 )
      {// locstring specified no part or no index
        astyle = 'e';  
      }
      else
      {// have a legitimate string part and index

        curbox->locatePartOrSymbol( partinfo, partindex, 
                                    inx, iny, aw, ah, error );
        
        if( error )
        {
          astyle = 'e';
          return '-';
        }

        ax = curx + inx;  ay = cury + iny;
        astyle = 'f';

      }// frame a legitimate symbol in a string part
      
    }
    else
      astyle = 'e';  // signal out an error
  }
  else
    astyle = 'e';

  return '-';

}// getInfoFromLocString

  // given whichpart as 'i' for mark on inner list,
  //                    or 'a', 'd', 'n' 't' 'v' 
  //   index as -1 if want entire part, else 0.. if want specific
  //   symbol, return offset inx,y within box, size aw,ah of the
  //   entity, and error so can realize something wrong
void box::locatePartOrSymbol( char whichpart, int index,
                float& inx, float& iny, float& aw, float& ah,
                 int& error )
{
  mystring* thepart;  // note for further work after kind zone
  error = 0;  // optimistic!

  if( whichpart == 'i' )
  {// locate mark for inner list in similar groups

    // kinds that have inner list mark at center of upper edge
    if( kind==seqbox || kind==lessonbox ||
        kind==branchbox
      )
    {
      aw = typsymwidth;
      ah = typsymheight;
      iny = -vpad;

      if( kind==branchbox && first != NULL )
      {// center above first inner
        inx = first->ulcx + first->width/2;
      }
      else
      {// center in the entire box
        inx = width/2 - aw/2;
      }

    }
    
    // kinds that have inner list mark at far left below the top line
    // and have a "boomerang" style mark
    else if( kind==univbox || kind==classdatabox || 
             kind==classmethodsbox || kind==instancedatabox ||
             kind==instancemethodsbox || kind==paramsbox ||
             kind==localsbox || kind==instancebox || 
             kind==classdatacopybox
           )
    {
      aw = typsymwidth;
      ah = typsymheight;
      inx = hpad;
      iny = -vpad-baseline-vpad;
    }

    // all other kinds don't have flexible length inner lists
    else
    {
      error = 1;
      return;
    } 

    return;  // bail out so don't try to mess with thepart later!

  }// locate mark for inner list

  else if( whichpart == 'n' )
  {

    // kinds that have their name simply in the ulc
    //  with a little extra on the left for newopbox
    if( kind==databox || kind==univbox || kind==classbox ||
        kind==classdatabox || kind==classmethodsbox ||
        kind==instancedatabox || kind==instancemethodsbox ||
        kind==methodbox || kind==paramsbox || kind==localsbox ||
        kind==idbox || kind==unopbox || kind==growbox || 
        kind==newopbox || kind==classmethodbox || 
        kind==instancemethodbox || kind==stackbox || kind==heapbox ||
        kind==instancebox || kind==gridbox || kind==portsbox ||
        kind==portbox || kind==intbox || kind==floatbox ||
        kind==charbox || kind==booleanbox || kind==stringbox ||
        kind==allclassdatabox || kind==classdatacopybox ||
        kind==retvalbox
      )
    {
      thepart = name;  // note for later figuring width height in aw,ah
      inx = hpad;
      if( kind==newopbox )
        inx += 3.5*hpad;
      iny = -vpad;
    }

    // kinds that have the name as a sort of utility string after the
    // first inner, with additional space for memberbox
    else if( kind==idbox || kind==opbox || kind==assbox ||
             kind==memberbox
           )
    {
      thepart = name;
      iny = -vpad;
      inx = first->ulcx + first->width + hpad;
      if( kind==memberbox )
        inx += 3*hpad;
    }

    // kinds that don't have a name at all
    else
    {
      error = 1;
      return;
    }

  }// locate name 
  
  else if( whichpart == 't' )
  {

    // kinds that have a type always put it in the upper right corner!
    if( kind==databox || kind==retvalbox || kind==portbox || 
        kind==classmethodbox || kind==instancemethodbox ||
        kind==instancebox || kind==gridbox
      )
    {
      thepart = type;
      float w, h;
      type->howBig(0,w,h);  // determine width
      inx = width-w-2*hpad;
      iny = -vpad;
    }

    // kinds that don't have a type
    else
    {
      error = 1;
      return;
    }

  }// locate type

  else if( whichpart == 'v' )
  {
    // kinds that have a usual value on second line
    if( kind==databox || kind==retvalbox || kind==portbox )
    {
      thepart = value;
      inx = hpad;
      iny = -vpad - baseline - vpad;
    }

    // kind that has just a value---live only inside gridbox
    else if( kind==valuebox )
    {
      thepart = value;
      inx = hpad;
      iny = -vpad;
    }

    // kinds that have no value
    else
    {
      error = 1;
      return;
    }

  }// locate value

  else if( whichpart == 'a' || whichpart == 'd' )
  {
    if( whichpart == 'a' )
      thepart = name;
    else if( whichpart == 'd' )
      thepart = doc;

    inx = hpad;
    iny = -vpad;

  }// locate silly ones that are the same as the box for uniformity

  else
    errorexit("box::locatePartOrSymbol: unknown whichpart");

  // now have thepart (gone already if doing the inner list mark),
  // take care of part vs. symbol issue while figuring aw,ah which
  // is the size of the part or symbol

  // decide whether want to honor newlines, and what symbol0 choice
  // is needed:
  int newlines, sym0choice;

  if( whichpart == 'a' ) 
  {
    if( kind == javabox )
    {
      newlines = 1;
      sym0choice = 3;
    }
    else if( kind == combox || kind == parbox || kind == presbox )
    {
      newlines = 1;
      sym0choice = 0;
    }
    else
    {
      newlines = 0;
      sym0choice = 2;
    }
  }
  else if( whichpart == 'd' )
  {
    newlines = 1;
    sym0choice = 1;
  }
  else // includes n, t, v
  {
    newlines = 0;
    sym0choice = 0;
  }

  // now update inx,y and aw,h

  if( index < 0 )
  {// want to frame the entire part
    // determine size, using newlines, sym0choice
    thepart->coreHowBig( newlines, sym0choice,
                aw, ah );
  }
  else
  {// only want to frame the symbol in position index

    // upgrade offset inx,y to account for moving to the symbo
    float offsetx, offsety; // obtain offset within mystring to symbol
    int row, col;  // not used, have to put something in parameter spot

    thepart->getInfoAt( sym0choice, index, newlines,
                        offsetx, offsety, row, col );

    // now figure inx, iny:
    inx += offsetx;
    iny += offsety;

    // compute the size aw,ah of the symbol
    ah = baseline;  // all have standard height
    if( index == 0 )
    {// symbol0, not actually stored
			if( sym0choice == 1 )
        aw = widthsym[ littlepagecode ];
			else if( sym0choice == 2 )
				aw = widthsym[ absimagecode ];
			else if( sym0choice == 3 )
			  aw = widthsym[ boxedjavacode ];
			else
				aw = widthsym[ trianglecode ];
    }// symbol0
    else
    {// access and determine size of actual symbol
      if( index > thepart->length() )
      {
        error = 1;
        return;
      }
      else
      {// have the symbol
        int sym = thepart->charAt( index );
        aw = widthsym[sym];
      }
    }// actual symbol

  }// frame a symbol

}// locatePartOrSymbol

// local utilities
int isLetter( char ch )
{
  return ('a'<=ch && ch<='z')
          ||
         ('A'<=ch && ch<='Z' );
}

int isDigit( char ch )
{
  return '0'<=ch && ch<='9';
}

// move from this box (the "oldfocus")
//  out to universe box, maintaining correctness
// of stuff just as if had been done interactively,
// then scan locstring from universe  inward, making changes to
// boxes and mystrings encountered along the way just as if
// had moved in interactively
box* box::findBoxFromLocString( mystring* locstring )
{
  box* curbox = this;
  box* owner;

  // move outward with integrity
  owner = curbox->outer;
  while( owner != NULL )
  {
    curbox->moveOutside();
    owner->current = curbox;
    owner->cursor = curbox->positionInInnerList();

    if( owner->aspect != 'f' )
      errorexit("hmmm, strange thing in box::findBoxFromLocString");

    owner->part = 'i';  

    curbox = owner;
    owner = curbox->outer;
  }

  // now move inward, following locstring's instructions
  // and maintaining correct box settings along the way just
  // as if moved interactively

  int sit = 0;  // sit for "situation" or state
  char ch;  // hold the next symbol
  int k;    // index of symbol
  int error = 0;  // flag whether have found error or not

  mystring* word;  // build up name or number here
  char newpart;

  k = locstring->whereCursor();  // pointing to first guy after move(

  while( k<=locstring->length() && sit < 6  && !error )
  {
    ch = locstring->charAt( k ); k++;  // convenience
                locstring->advanceCursor();

    if( sit==0 )
    {// just starting
      if( ch == ')' )
        sit = 6;
      else if( ch=='.' )
      {
        word = new mystring( "" ); // starting a name
        sit = 1;
      }
      else if( ch=='#' )
      {
        word = new mystring( "" ); // starting a natural number
        sit = 3;
      }
      else if( ch==',' )
      {
        word = new mystring( "" ); // starting a natural
        sit = 4;
      }
      else
        error = 1;
    }
    else if( sit==1 )
    {// following a period, ready to build up a name in word
      if( isLetter( ch ) )
      {
        word->appendChar( ch );
        sit = 2;
      }
      else
        error = 1;
    }
    else if( sit==2 )
    {// continuing to build up a name
      if( isLetter( ch ) || ch == '_' || isDigit( ch ) )
      {// add a symbol to name
        word->appendChar( ch );
      }
      else if( ch == ')' )
      {// locstring is done, send off current name
        curbox = curbox->moveInwardToNamedInner( word );
        sit = 6;
      }
      else if( ch=='.' )
      {// finished with current name, starting another
        curbox = curbox->moveInwardToNamedInner( word );
        word = new mystring( "" ); // starting a name
        sit = 1;
      }
      else if( ch=='#' )
      {// finished with current name, starting a natural
        curbox = curbox->moveInwardToNamedInner( word );
        word = new mystring( "" ); // starting a natural number
        sit = 3;
      }
      else if( ch==',' )
      {// finished with current name, prepare to do part index
        curbox = curbox->moveInwardToNamedInner( word );
        word = new mystring( "" ); // starting a natural
        sit = 4;
      }
      else
        error = 1;
    }
    else if( sit==3 )
    {// working on index following #
      if( isDigit( ch ) )
      {// add a digit to index
        word->appendChar( ch );
      }
      else if( ch == ')' )
      {// locstring is done, send off current index
        curbox = curbox->moveInwardToIndexedInner( word );
        sit = 6;
      }
      else if( ch=='.' )
      {// finished with current index, starting a name
        curbox = curbox->moveInwardToIndexedInner( word );
        word = new mystring( "" ); // starting a name
        sit = 1;
      }
      else if( ch=='#' )
      {// finished with current index, starting another
        curbox = curbox->moveInwardToIndexedInner( word );
        word = new mystring( "" ); // starting a natural number
        sit = 3;
      }
      else if( ch==',' )
      {// finished with current index, prepare to do part index
        curbox = curbox->moveInwardToIndexedInner( word );
        word = new mystring( "" ); // starting a natural
        sit = 4;
      }
      else
        error = 1;
    }
    else if( sit==4 )
    {// saw a comma, ready to do part index
      if( ch=='n' || ch=='d' || ch=='t' || ch=='v' )
      {// have legitimate part selector
        newpart = ch;
        word = new mystring( "" );
        sit = 5;
      }
      else
        error = 1;
    }
    else if( sit==5 )
    {// building up part index
      if( isDigit( ch ) )
      {// adding a digit to index
        word->appendChar( ch );
      }
      else if( ch==')' )
      {// done with locstring
        curbox->moveInwardToPart( newpart, word );
        sit = 6;
      }
      else
        error = 1;
    }
    else
     error = 1;

  }// fsm to scan locstring and move curbox to desired location

  if( error || sit != 6 )
  {
    errorexit("Illegal situation in box::findBoxFromLocString" );
    exit(1);
  }

  return curbox;

}

// each of these moves from this box to named or indexed inner
// box, returns that box.
//   If index if 0, return the box owning the outer list and
//    set it to have inside focus, part 'i', etc.
//   Also, release memory resources of word
box* box::moveInwardToIndexedInner( mystring* word )
{
  long lindex;
  int status = word->toInt( lindex );
  int index = (int) lindex; // won't ever have very large index

  box* curbox;

  if( index > 0 )
  {
    curbox = accessInnerAt( index );
    // update this box's data to reflect the move
    current = curbox;
    cursor = index;
  }
  else
  {// special case:  index 0 means on the box itself
    curbox = this;
    current = NULL;
    cursor = 0;
    inside = 'y';
    part = 'i';
  }

  word->destroy();  // free up resources

  return curbox;
}

box* box::moveInwardToNamedInner( mystring* word )
{
  // find the inner with correct name
  box* curbox = searchInner( word );

  // update this box's data to reflect the move
  current = curbox;
  cursor = curbox->positionInInnerList();

  word->destroy();  // free up resources

  return curbox;
}

void box::moveInwardToPart( char newpart, mystring* word )
{
  // have this box as the location, just need to put focus
  // on the correct part, and move cursor in that part

  // identify correct part
  mystring* partstring;

  if( newpart=='n' )
    partstring = name;
  else if( newpart=='d' )
    partstring = doc;
  else if( newpart=='t' )
    partstring = type;
  else if( newpart=='v' )
    partstring = value;
  else
    errorexit("fatal error in box::moveInwardToPart---bad part");

  // get the index from word
  long lindex;
  int status = word->toInt( lindex );
  int index = (int) lindex; // won't ever have very large index

  partstring->setCursor( index );

  inside = 1;
  if( newpart != 'd' )
    part = newpart;

  word->destroy();  // free up resources
}
