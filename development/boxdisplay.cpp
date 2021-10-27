  // draw my special "symbol0" for my kind,
  // at coords x,y, using my internal info. together with
  // "showfocus", to decide on color,
  // return width,height w,h for positioning next guy

  // upgrade to new scheme ---note that this guy is only called
  // to provide symbol0 for various FULL aspects, so color
  // is fulltextcolor, may have focus background
  void box::drawsymbol0( float x, float y, int displaylevel,
                     float& w, float& h )
  {
	  // traceEnter( tid, "box:drawsymbol0" );

	// new scheme---draw symbol0 in fulltextcolor at textz,
	  // or not, then draw focuscolor background 
/*
    if(displaylevel==2 )
      glColor3fv( focuscolor ); // draw in focuscolor
    else if(displaylevel==1)
      glColor3fv( normalcolor ); // draw standard color
    else
    {// won't draw at all, just return w, h
    }
*/

	glColor3fv( fulltextcolor );

// kind zone ++++++++++++++++++++++++++++++++++++++++++++++++++++
//     draw symbol0 for box with 0 or more style inner list
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // for now, and probably always, use same symbol for all
    // these kinds of guys
    if(kind == univbox || kind==classdatabox || 
       kind==classmethodsbox || kind==instancedatabox ||
       kind==instancemethodsbox || kind==paramsbox ||
       kind==localsbox || kind==instancebox 
			 // v10
			 || kind==classdatacopybox
      )
    {// draw standard weird triangle guy
      // set w, h:
      w = typsymwidth;
      h = typsymheight;

      if(displaylevel>0)
      {// actually draw it:

        if( !texdraw )
        {// draw to screen
          glBegin(GL_POLYGON);
            glVertex3f(x, y, textz );
            glVertex3f( x+w, y-0.5*h, textz );
            glVertex3f( x+0.3*w, y-0.5*h, textz );
          glEnd();
        
          glBegin(GL_POLYGON);
            glVertex3f( x+0.3*w, y-0.5*h, textz );
            glVertex3f( x+w, y-0.5*h, textz );
            glVertex3f( x, y-h, textz );
          glEnd();
        }
        else
        {// draw to texfile
				  if( pictex )
					{
					  pictexLine( x, y, x+w, y-0.5*h );
						pictexLine( x+w, y-0.5*h, x, y-h );
						pictexLine( x, y-h, x+0.3*w, y-0.5*h );
						pictexLine( x+0.3*w, y-0.5*h, x, y );
					}
					else
					{// postscript
            psTriangle( x, y, x+w, y-0.5*h, x+0.3*w, y-0.5*h );
            psTriangle( x+0.3*w, y-0.5*h, x+w, y-0.5*h, x, y-h );
					}
        }// to texfile

      }
    } // end of standard horizontal list symbol0

    else if( kind==seqbox || kind==branchbox 
		             ||
						 kind==lessonbox //jb19
					 )
    {// has a vertical, downward pointing triangle

      // set w, h:
      w = typsymwidth;
      h = typsymheight;

      if(displaylevel>0)
      {// actually draw it:

        if( !texdraw )
        {// draw to screen
          glBegin(GL_POLYGON);
            glVertex3f(x, y, textz );
            glVertex3f( x+w, y, textz );
            glVertex3f( x+0.5*w, y-h, textz );
          glEnd();       
        }
        else
        {// draw to texfile
				  if( pictex )
					  pictexTriangle( x, y, x+w, y, x+0.5*w, y-h );
					else
            psTriangle( x, y, x+w, y, x+0.5*w, y-h );
        }

      }

    } // end of seqbox 

    else
      errorexit("drawsymbol0 not implemented for this kind");

	  // now, depending on displaylevel, maybe draw focuscolor
	  // background rectangle
  	if( displaylevel == 2 )
	  {// draw focus background
	    glColor3fv( focuscolor );
  	  glBegin(GL_POLYGON);
	      glVertex3f( x, y, textfocusz );
	      glVertex3f( x, y-h, textfocusz );
  	    glVertex3f( x+w, y-h, textfocusz );
	      glVertex3f( x+w, y, textfocusz );	
	    glEnd();
	  }

    // // traceLeave();

  }// box::drawsymbol0

  // utility to ease coding in sizeLocDisplay of part where
  // we get info on the symbolic "apparent focus"
  //        ulx,y is the ulc of the string s that holds the
  //                            apparent focus,
  //   s==NULL means apparent focus is on symbol0 of inner list,
  //        rather than on a string,
  //        newlines tells whether the string honors newlines,
  //         return offset of cursor symbol and that symbol's size
  //
  // also, if not inside, all bets are off, simply return
  // basic info for the box as a whole
  void box::getAFInfo( int doit, mystring* s, float ulx, float uly,
                      int newlines,
                     float& afx, float& afy,
                     float& afw, float& afh )
  {

    // traceEnter( tid, "box:getAFInfo" );

    if(doit)
    {
      if( ! isInside() )
      {// focus is actually on my border,
        // so don't use ulx,uly, which is likely to be wrong
        // in this case, as it was provided for one of the
        // string parts who are no longer relevant since we're
        // outside!
        afx = 0; afy = 0;
        afw = width; afh = height;
      }
      else
      {// apparent focus is inside me

        int index, row, col;

        if (s == NULL )
        {// apparent focus is not on a string
          afx = ulx+0; afy = uly+0; 
                    // symbol0 of inner list is at start
          // find size of symbol0 for inner list
          afw = widthsym[trianglecode];
          afh = typsymheight;
        }
        else
        {// apparent focus is on a string
          index = s->whereCursor(); // first real char is 1, etc.
  
          if(index==0)
          {// af is on symbol0 of the string
            afx = ulx+0; afy = uly+0;
            // find size of symbol0 for this string
            afw = widthsym[trianglecode];
            afh = typsymheight;
          }
          else
          {// af is on an actual symbol, have some interesting
            // work to do
          
            s->getInfoAt( 0, index, newlines,
                       afx,  afy,
                       row, col );

             afx += ulx;  afy += uly;

             afw = widthsym[ unsignchar(s->charAt( index )) ];
             afh = typsymheight;

          }// focus on a symbol

        }// focus on a string

      } // not inside
          
    }  // else don't waste any more time!

		// // traceLeave();

  }// end of box::getAFInfo

  // 9/29/2000:  if do coreDisplay, must use coreHowBig and
  //  specialGetAFInfo, to adjust for symbol0 variance
  void box::specialGetAFInfo( int symbol0Choice,
	                  int doit, mystring* s, float ulx, float uly,
                      int newlines,
                     float& afx, float& afy,
                     float& afw, float& afh )
  {

    // traceEnter( tid, "box:getAFInfo" );

    if(doit)
    {
      if( ! isInside() )
      {// focus is actually on my border,
        // so don't use ulx,uly, which is likely to be wrong
        // in this case, as it was provided for one of the
        // string parts who are no longer relevant since we're
        // outside!
        afx = 0; afy = 0;
        afw = width; afh = height;
      }
      else
      {// apparent focus is inside me

        int index, row, col;

        if (s == NULL )
        {// apparent focus is not on a string
          afx = ulx+0; afy = uly+0; 
                    // symbol0 of inner list is at start
          // find size of symbol0 for inner list            			
   		  afw = widthsym[trianglecode];
          afh = typsymheight;
        }
        else
        {// apparent focus is on a string
          index = s->whereCursor(); // first real char is 1, etc.
  
          if(index==0)
          {// af is on symbol0 of the string
            afx = ulx+0; afy = uly+0;
            // find size of symbol0 for this string

 		    if( symbol0Choice == 1 )
              afw = widthsym[ littlepagecode ];
		    else if( symbol0Choice == 2 )
			  afw = widthsym[ absimagecode ];
		    else
			  afw = widthsym[ trianglecode ];
          
		    afh = typsymheight;
          }
          else
          {// af is on an actual symbol, have some interesting
            // work to do
          
            s->getInfoAt( symbol0Choice, index, newlines,
                       afx,  afy,
                       row, col );

             afx += ulx;  afy += uly;

             afw = widthsym[ unsignchar(s->charAt( index )) ];
             afh = typsymheight;

          }// focus on a symbol

        }// focus on a string

      } // not inside
          
    }  // else don't waste any more time!

		// // traceLeave();

  }// end of box::specialGetAFInfo

  // display myself with ulc at (x,y) in current viewport
  // coordinates, using showfocus to determine whether or not
  // to highlight which part of me is currently the focus
  //  if I am in fact the focus

  // newish scheme to save really fairly silly extra effort:
  //  "drawit" flag determines whether we actually do the
  //  drawing (cut off recursive calls if not drawing---don't
  //  do sizeloc work on inner boxes), 
  //  compute width and height of the box as we display it
  //  (or not), and return whether or not changed
  //
  //  and, change the locations of the inner boxes as the
  //  size is figured and the display is done

  // compute width, height, and locate inner boxes and return
  // "changed" no matter what
  // actually draw with ulc at (x,y) according to "draw",
  //  using "showfocus" and "focusbox" to determine color
  // find and return "partnumber" and the box on which the
  // offset "innerx" "innery" is located as the function value,
  //  depending on "find" using "targetx" and "targety"
  // 
  //  partnumber == -2       -->  weren't looking
  //                -1       -->  target is not over me at all!
  //                31            abstract part/view
  //                21            documentation part/view
  //                11            name in full view
  //                12            type in full view
  //                13            value in full view
  //                14            symbol0 of inner list in full view
  //         
  //                 1            target on me, nobody inside
  //                 2            target is on an inner box
  //
  // innerx,innery is offset from ulc of innerbox,
  //         or of part in innerbox
  //
  // goal here is to be really organized, because later may
  // have other related guys to parallel in---specifically,
  // will have TeX output guy, and probably will have a
  // tool that will support up/down
  //
  // main point is to maintain ulx, uly, lrx, lry as we move,
  // over parts and then along the inner list

  //  <inner box target is over, if any> =
  //     sizeLocDisplay[ relocate inners?,  did size change?,
  //      draw?, screen x to draw at, screen y, 
  //         right edge of drawing area, bottom edge of drawing area,
  //        should show focus?, pointer to focus,
  //        find a target point?, offset to target x, target y,
  //        return part where found, return offset x over inner, iy,
  //          whether to return apparent focus info,
  //            apparent focus x offset from my ulc, afy,
  //            apparent focus width, afh,
  //               number in inspection list, 
  //     list of boxes to inspect, list of their colors )
  //
  // if "find", return partnumber == 'a', 'd', or specific part
  //       label within 'f' --- 'n', 'i', 
  //        'i' means on symbol0 of inner, 'I' means on a box
  //      '.' means on me
  //      '?' means wasn't on the box at all!??

  // if not inside, apparent focus is me, otherwise notice
  // the info for the symbol inside me that is the apparent
  // focus
  // 
  // checking use of getAFInfo --- 
  //    if have like a name part, it will have getAFInfo called
  //     to check if 
  //
  // late change:  at end have "inspection list" variables
  //                ni == number to inspect
  //                boxlst = array of ni box pointers to inspect
  //                colorlst = whatever by 3 float array of rgb
  //                           pastel colors
  //
  //  The idea is simple:  when I'm asked to draw a box --- and
  //   only then, not for locating or sizing, scan the inspection
  //   list and if I'm in it, draw my background the special
  //   pastel color

  // okay, now we're changing this for the new scheme, wherein
  // every box knows its color, dye, depth, and takes care of
  // its own display!  

  // I'm thinking temporarily use showfocus, focusbox as they
  // are currently, later put in stuff that dyes the focusbox
  // the desired color---right now it's not there, so do this
  // as intermediate scheme.   Also, use the boxlst to color
  // ... no, it's got nothing much in it---scrap it!

  // also, changing so aspect drives color of text stuff

  box* box::sizeLocDisplay( int locate, int& changed,
    int draw, 
    float x, float y, 
    float left, float right, float top, float bottom,
    int showfocus, box* focusbox,
           int find, float targetx, float targety,
           char& partnumber, float& innerx, float& innery,
           int getafinfo, float& afx, float& afy,
                          float& afw, float& afh       )
  {

    float w1, h1, w2, h2, w3, h3;  // utility variables
    float ulx, uly; // maintain  standard coords
              // over the box, of guy currently being drawn           
              //  --- not offsets, but actual coords
    float maxheight, leftmargin, maxwidth, midx, midy;
    float maxheight2, midy2, ulx2, uly2, ulx0, uly0;
    int k;

    box* curbox;  // utility box pointer
    box* innerbox;  // used to store box the offset is on until
                    // ready to return

    box* firstbox;
    box* secondbox;

    int innercount; // utility index variable

    float fx, fy, sx, sy;

    // utilities for displaying various code boxes
    float retx, rety, outx, outy;  // offsets for arrows                              
    float dx = 0.5*hpad;

    // remember so can tell if changed
    long oldwidth, oldheight;
    oldwidth = width; oldheight = height;

    if( draw && !locate && !find && !getafinfo )
    {// only drawing, see if I'm completely out of sight,
      // hence should just return
      //    before had just left, right, top, bottom, did partial test,
      //    now I'm thinking that when display is called,
      //    my size is correct up front --- I had once hoped
      //    to efficiently overlap the work of size/locating
      //    with displaying, but it certainly hasn't happened,
      //    so should be safe to use width and height here
      
      if( (right < x || y < bottom ||
           left > x+width || top < y-height)
           &&
           !texdraw
        )
      {
			  // // traceLeave();
        return NULL;   // can't matter since only drawing
      }

      
    }// just drawing, maybe skip because I'm out of sight!

    partnumber = '?';  // means haven't computed a value yet
    if(!find)
      partnumber = '-';  // means didn't want to look---
                         // so '?' later will mean haven't found
                         // yet and wanted to
                         
    innerbox = this;  // may never change to the real thing
                      // or this may turn out to be the real thing,
                      // if a part is hit, rather than inner box,
                      // or if I am hit, but all innards are missed

    // for convenience on string displays
    int dofocus = showfocus && (focusbox==this);

    // size and maybe draw the guts

    // jb16:  javabox has different abstract
    if( aspect == 'a' && kind != javabox &&
		                     kind != combox // jb19
                &&
          kind != parbox && kind != presbox // jb21
		  )  // abstract view
    {// draw name, independent of kind

      name->coreHowBig( 0, 2, w1, h1 ); // get size of name
	             // 2 == symbol0 choice for absimage
      // account for padding around the name
      width = myround(hpad + w1 + hpad);
      height = myround(vpad + h1 + 2*vpad);

      if(draw)
        name->coreDisplay( x+hpad, y-vpad, 2,
		        1,
              dofocus && (inside=='y'), abstextcolor );
           // 
       
      if ( insideRect( find,
               0, 0, width, -height,
                     targetx, targety,
                    innerx, innery ) )
      {// inside this guy, see if on bottom strip,
			 //  or really on string
			  if( outsideRect( 0, 0, width, -height+2*vpad,
				                  targetx, targety ) 
					)
				  partnumber = '.';  // on the box, not the string
				else
          partnumber = 'a';
      }
       
      specialGetAFInfo( 2, getafinfo, name, 
                    hpad, -vpad,
                    0,
                      afx, afy, afw, afh );

    }

		// jb16:  abstract aspect of javabox displays like 
		//        doc aspect of all boxes, except for color
    // jb19:  add combox as displaying abstract aspect like javabox
    // jb21:  add parbox and presbox
		else if( (kind==javabox || kind==combox ||
              kind==parbox || kind==presbox
             )
		          && aspect=='a' 
					 )
		{
		  int sym0code;          // do J for javabox, ordinary for com
			if( kind==javabox )
			  sym0code = 3;
			else
			  sym0code = 2;

      name->coreHowBig( 1, sym0code, w1, h1 );
      width = myround(hpad + w1 + hpad);
      height = myround(vpad + h1 + vpad );

      if(draw)
        name->coreDisplay( x+hpad, y-vpad, sym0code,
		          1, 
                 dofocus && inside=='y', abstextcolor );

      // okay to use width, height because accurate at this point
      if ( insideRect( find, 
                0, 0, width, -height, 
                targetx,targety,
                      innerx, innery ) )
			{// target is inside the whole rectangle
			  // find out area for symbol0, use left edge below that
				// jb19:  noticed littlepagecode here, seems wrong,
				// use boxedjavacode---same width as absimagecode
			  if( targetx < (hpad+widthsym[boxedjavacode]) && 
				  targety < -h1 )
				  partnumber = '.';  // on the left edge, not the string
				else     
          partnumber = 'a';
      }

      specialGetAFInfo( 1, getafinfo, name, 
                       hpad, -vpad, 
                       1,
                           afx, afy, afw, afh );

		}// abstract aspect of javabox and combox

    else if ( aspect == 'd' )
    {// draw doc, independent of kind
      doc->coreHowBig( 1, 1, w1, h1 );  // second 1 is symbol0choice
      width = myround(hpad + w1 + hpad);
      height = myround(vpad + h1 + vpad );
      
      if(draw)
        doc->coreDisplay( x+hpad, y-vpad, 1,
		          1, 
                 dofocus && inside=='y', doctextcolor );

      // okay to use width, height because accurate at this point
      if ( insideRect( find, 
                0, 0, width, -height, 
                targetx,targety,
                      innerx, innery ) )
			{// target is inside the whole rectangle
			  // find out area for symbol0, use left edge below that
			  if( targetx < (hpad+widthsym[littlepagecode]) && 
				  targety < -h1 )
				  partnumber = '.';  // on the left edge, not the string
				else     
          partnumber = 'd';
      }

      specialGetAFInfo( 1, getafinfo, doc, 
                       hpad, -vpad, 
                       1,
                           afx, afy, afw, afh );

    } // doc aspect

    else if( aspect == 'v' )  // value aspect
    {// draw value and type, independent of kind
      //   Note:  "inside" is irrelevant for this aspect---
      //           simulate being on border, can't go "in"
      //           (only have data member for which part of
      //            full aspect)

      value->howBig( 0, w1, h1 ); // get size of value
      type->howBig( 0, w2, h2 );  // size of type

      // figure the box size
      width = myround(hpad + 1.5*mymax(w1,w2) + hpad);
      height = myround( h1 + + h2 + vpad);

      if(draw)
      {
        type->display( x + width - hpad - w2,
                       y, 0,
                       0,
                       valuetextcolor);
        value->display( x+hpad, y-h2, 0, 
                    0, // had this: dofocus && (inside=='y'),
                      valuetextcolor );
      }

      // will just be on me
      if ( insideRect( find,
               0, 0, width, -height,
                     targetx, targety,
                    innerx, innery ) )
      {
        partnumber = '.';
      }
       
      //  getAFInfo( getafinfo, NULL, 
      //              hpad, -vpad,
      //              0,
      //                afx, afy, afw, afh );
      // instead, just produce af info for me:
      afx=0; afy=0; afw = width; afh = height;

    }// value aspect

    else if ( aspect == 'f' )
    {// branch on kind to draw full display of some sort

	   // set default for various little drawing parts
       glColor3fv( fulltextcolor );

// kind zone ++++++++++++++++++++++++++++++++++++++++++++++++++++++ 
//     draw, find target, update locations of inners, all depending
//     on my kind
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

      if( kind == databox || kind==retvalbox )  // v10
      {// show name, type, value 

        name->howBig( 0, w1, h1 );
        type->howBig( 0, w2, h2 );
        value->howBig( 0, w3, h3 );
        width = myround( hpad + mymax(w1+w2+hpad,w3) + hpad );
        height = myround( vpad + mymax(h1,h2) + vpad + h3 + vpad);

        if(draw)
        {
          name->display( x+hpad, y-vpad, 0, 
            dofocus && inside=='y' && (part=='n'), fulltextcolor );
          type->display( x+width-w2-hpad, y-vpad, 0,
            dofocus && inside=='y' && (part=='t'), fulltextcolor );
          value->display( x+hpad, y-2*vpad-mymax(h1,h2), 0, 
            dofocus && inside=='y' && (part=='v'), fulltextcolor );
        }

        if(insideRect( find,
          0, 0, hpad+w1, -(vpad+h1),
                      targetx,targety,
                      innerx, innery ))
         partnumber = 'n';

        if(insideRect( find,
            width - (2*hpad+w2),0, width,-(vpad+h2),
                     targetx,targety,
                      innerx, innery ))
          partnumber = 't';

        if(insideRect( find,
                    0,-(vpad+mymax(h1,h2)+vpad),
                    hpad + w3, -height,
                    targetx,targety,
                      innerx, innery ))
          partnumber = 'v';

        // v10:  for retvalbox, change 'n' and 'v' to '.'
				if( kind==retvalbox && (partnumber=='n' || partnumber=='v') )
				{
				  partnumber = '.';
				}
				        
        // determine which string part is the focus
        if(getafinfo)
        {
          if(part=='n')
            getAFInfo( getafinfo, name, 
               hpad, -vpad,
               0, 
               afx, afy, afw, afh );
          else if(part=='t')
            getAFInfo( getafinfo, type, 
               width-w2-hpad, -vpad,
               0, 
               afx, afy, afw, afh );
          else if(part=='v')
            getAFInfo( getafinfo, value, 
               hpad, -2*vpad-mymax(h1,h2),
               0, 
               afx, afy, afw, afh );
          else
            errorexit("bad part in box::sizeLocDisplay");
        } // get af info

      }// end of databox, retvalbox

      else if( kind == portbox )
      {// show name, type, value 

        name->howBig( 0, w1, h1 );
        type->howBig( 0, w2, h2 );
        value->howBigPortValue( w3, h3 );

				// special work to maybe display end of file symbol
				// for the portbox value
			  int portNumber = positionInInnerList();
				int specialEndSymbol = 0;
				if( ports->getStatus( portNumber ) == emptyPortStatus )
				{// have reached end of file, display special guy on end
				  specialEndSymbol = 1;
				  w3 += maxcharwidth;
					h3 = mymax( h3, typsymheight );
				}
					
        width = myround( hpad + mymax(w1+w2+hpad,w3) + hpad );
        height = myround( vpad + mymax(h1,h2) + vpad + h3 + vpad);

        if(draw)
        {
          name->display( x+hpad, y-vpad, 0, 
            dofocus && inside=='y' && (part=='n'), fulltextcolor );
          type->display( x+width-w2-hpad, y-vpad, 0,
            dofocus && inside=='y' && (part=='t'), fulltextcolor );

					value->portDisplay( x+hpad, y-2*vpad-mymax(h1,h2), 
					                     specialEndSymbol, fulltextcolor );

        }

        if(insideRect( find,
          0, 0, hpad+w1, -(vpad+h1),
                      targetx,targety,
                      innerx, innery ))
         partnumber = 'n';

        if(insideRect( find,
            width - (2*hpad+w2),0, width,-(vpad+h2),
                     targetx,targety,
                      innerx, innery ))
          partnumber = 't';

        if(insideRect( find,
                    0,-(vpad+mymax(h1,h2)+vpad),
                    hpad + w3, -height,
                    targetx,targety,
                      innerx, innery ))
          partnumber = 'v';
        
        // determine which string part is the focus
        if(getafinfo)
        {
          if(part=='n')
            getAFInfo( getafinfo, name, 
               hpad, -vpad,
               0, 
               afx, afy, afw, afh );
          else if(part=='t')
            getAFInfo( getafinfo, type, 
               width-w2-hpad, -vpad,
               0, 
               afx, afy, afw, afh );
          else if(part=='v')
            getAFInfo( getafinfo, value, 
               hpad, -2*vpad-mymax(h1,h2),
               0, 
               afx, afy, afw, afh );
          else
            errorexit("bad part in box::sizeLocDisplay");
        } // get af info

      }// end of portbox

      else if( kind == valuebox )
      {// just display my value, cause that's all I've got!
        // one strange thing---only exists within a gridbox,
        // my width is "figured" by my owner, I should never
        // mess with it!  Point is, will never get to a
        // valuebox for any purpose without first going through
        // its gridbox, which will set width correctly.  Height
        // too, for that matter

        if(draw)
        {
          value->display( x+hpad, y-vpad, 0, 
            0, fulltextcolor );
        }

        // jb21:  compiler warned w1, h1 have no meaning here
        if(insideRect( find,
            0, 0, hpad+w1, -(vpad+h1),
                      targetx,targety,
                      innerx, innery )  )
         partnumber = '.';
       
        // determine which string part is the focus
        if(getafinfo)
        {
          getAFInfo( getafinfo, value, 
             hpad, -vpad,
               0, 
               afx, afy, afw, afh );
        } // get af info

      }// valuebox

      else if( kind == gridbox )
      {// scan inner list,

        // size up name and type
        name->howBig(0,w1,h1);
        type->howBig(0,w2,h2);

        if(draw)
        {
          name->display( x+hpad, y-vpad, 0, 
                     0, fulltextcolor );
        }

        // can't draw type in urc until know width

        // figure width:

           // first, find maxwidth, maxheight, innercount
           maxwidth = 0;
           maxheight = 0;
           innercount = 0;
           curbox = first;  // can't build gridbox without at least 1
           
           while( curbox != NULL ) 
           {
             // check out physical size of value in curbox
             (curbox->value)->howBig(0,w3,h3);
             if( w3 > maxwidth )
               maxwidth = w3;
             if( h3 > maxheight )
               maxheight = h3;
             innercount++;
             curbox = curbox->next;
           }

           // adjust maxwidth for padding
           maxwidth = maxwidth + hpad;  // plenty on left with
                                        // the symbol0
      
           // next, get info about labels
             // to hold label size info
             float rowlabelwidth, collabelwidth, labelheight; 
             mystring* labstr;
             int numrows, numcols;
             float rowheight;

           if( state == 0 )
           {// 1D case, only have columns
             rowlabelwidth = 0;
             numrows = 1;
             numcols = innercount;
             labstr = build4s( numcols );
             labstr->howBig(0,collabelwidth,labelheight);
             labstr->destroy();
           }
           else
           {// 2D case
             numcols = state;
             numrows = innercount / state;
             labstr = build4s( numrows );
             labstr->howBig(0,rowlabelwidth,labelheight);
             labstr->destroy();
             labstr = build4s( numcols );
             labstr->howBig(0,collabelwidth,labelheight);
             labstr->destroy();
           } 

           // make sure wider than collabelstring
           maxwidth = mymax( maxwidth, hpad+collabelwidth+hpad );
           maxwidth = myround( maxwidth );

        // can now set the width of me
          // set width to minimum top line width
          width = hpad + w1 + hpad + w2 + hpad;
          // now see if the grid part is wider
          width = mymax( width, 
                  hpad + rowlabelwidth + numcols*maxwidth + hpad );

          rowheight = myround(0.5*vpad + maxheight + 0.5*vpad);

        // now that we have the width, draw the type
        if( draw )
          type->display( x+width-w2-hpad, y-vpad, 0,
                              0, fulltextcolor );

        // draw the inner boxes, labels, set inner box widths, heights,
        //  figure my height

        ulx = hpad;
        if( state > 0 )
          ulx += rowlabelwidth;
        ulx += hpad;  // shift each label a little
        uly = -vpad - h1 - 0.5*vpad;

        // little loop to draw column labels, which are there in
        // both 1D and 2D
                char temps[6];
        if( draw )
        {
          for( k = 1; k<=numcols; k++ )
          { 
            sprintf( temps, "%d", k-1 );
            labstr = new mystring( temps );
            labstr->display( x+ulx, y+uly, 0, 0, fulltextcolor );
            labstr->destroy();
            ulx += maxwidth;
          } 
        }

        uly -= labelheight + 0.5*vpad;
        ulx = hpad;     

        int row, col;
        curbox = first;

        for( row=0; row<numrows; row++ )
        {
          // start row
          if( state == 0 )
            ulx = hpad; // ready to draw the inners
          else
          {// draw row label
            ulx = hpad;
            // draw it
            sprintf( temps, "%d", row );
            labstr = new mystring( temps );
            labstr->display( x+ulx, y+uly, 0, 0, fulltextcolor );
            labstr->destroy();
            ulx += rowlabelwidth;          
          }

          for( col=0; col<numcols; col++ )
          {// handle inner box

            curbox->width = maxwidth;
            curbox->height = rowheight;

            if(draw)
              curbox->display( x + ulx, y+uly,
                             left, right, top, bottom,
                             showfocus, focusbox );
            // locate curbox?
            if(locate)
            {
              curbox->ulcx = ulx;  // offsets in outer
              curbox->ulcy = uly;
            }

            // find?
            if(insideRect(find,
                ulx, uly, 
               ulx+curbox->width, uly-curbox->height,
               targetx, targety, innerx, innery ))
            {
              partnumber = 'I';
              innerbox = curbox;
            }

            ulx += curbox->width;
            curbox = curbox->next;  // where we really move ahead
          }

          // finish the row
          uly -= rowheight;

        }

        // finish up the height calculation:
        height = -uly + vpad;

        // get apparent focus info---
        //  or if not inside, will kick back my info
        getAFInfo( getafinfo, NULL, 
                     0, 0, 
                     0,
                      afx, afy, afw, afh );

      }// gridbox

      else if(kind == emptybox)
      {// just draw the insides, in normal color, no need to
        // do locating, but do need to do finding

        width = 4*hpad;     // constant size
        height = 4*vpad;

        if(draw)
        {// draw emptybox inner image
          float x1, y1, x2, y2;
          x1 = x + width/4;  y1 = y - height/4;
          x2 = x + 0.75*width; y2 = y - 0.75*height;

          if( !texdraw )
          {// draw to screen
            glColor3fv( fulltextcolor );
            glBegin(GL_LINE_STRIP);
              glVertex3f( x, y, textz );
              glVertex3f( x1, y1, textz );
              glVertex3f( x2, y1, textz );
              glVertex3f( x2, y2, textz );
              glVertex3f( x1, y2, textz );
              glVertex3f( x1, y1, textz );
            glEnd();
            glBegin(GL_LINES);
              glVertex3f( x+width, y, textz );
              glVertex3f( x2, y1, textz );
  
              glVertex3f( x2, y2, textz );
              glVertex3f( x+width, y - height, textz );

              glVertex3f( x1, y2, textz );
              glVertex3f( x, y - height, textz );
            glEnd();
          }
          else
          {// draw to texfile
					  if( pictex )
						{
              pictexLine( x, y, x1, y1 );
              pictexLine( x1, y1, x2, y1 );
              pictexLine( x2, y1, x2, y2 );
              pictexLine( x2, y2, x1, y2 );
              pictexLine( x1, y2, x1, y1 );

              pictexLine( x+width, y, x2, y1 );
              pictexLine( x2, y2, x+width, y-height );
              pictexLine( x1, y2, x, y-height );
						}
						else
						{
              psLine( x, y, x1, y1 );
              psLine( x1, y1, x2, y1 );
              psLine( x2, y1, x2, y2 );
              psLine( x2, y2, x1, y2 );
              psLine( x1, y2, x1, y1 );

              psLine( x+width, y, x2, y1 );
              psLine( x2, y2, x+width, y-height );
              psLine( x1, y2, x, y-height );
						}
          }// to texfile

        }

        // find?
        if(insideRect( find,
                        0, 0, width, -height,
                      targetx,targety,
                      innerx, innery ))
          partnumber = '.';

        // get apparent focus info?  since
        // not inside, will kick back info for me
        getAFInfo( getafinfo, NULL, 
                     0, 0, 
                     0,
                      afx, afy, afw, afh );

      }// emptybox

      else if(kind == consolebox)
      {// a very special guy:  has fixed size,
        // focus must stay on border, display chargrid
        // as 25 by 80 display grid with input buffer string
        // displayed below
        
        width = hpad + chargrid->howWide() + hpad;
        height = chargrid->howHigh();              

        if(draw)
        {// draw 2d grid of chars and input buffer
          chargrid->display( x+hpad, y-vpad );    
        }

        // no need to locate!

        // find?
        if(insideRect( find,
                        0, 0, width, -height,
                      targetx,targety,
                      innerx, innery ))
          partnumber = '.';

        // get apparent focus info?  since
        // not inside, will kick back info for me
        getAFInfo( getafinfo, NULL, 
                     0, 0, 
                     0,
                      afx, afy, afw, afh );

      } // consolebox

      else if( kind==idbox || literalkind(kind) ||
               kind==memberbox || kind==opbox ||
               kind==unopbox ||
               kind==newopbox
             )
      {// value boxes with name and 0 or more inners
        // figure out the height so we can vertically center 
        // the inner boxes:

        name->howBig( 0, w1, h1 );      
        maxheight = h1;

        // account for decorations
        if( literalkind(kind))
          maxheight = mymax( maxheight, typsymheight );

        curbox = first;
        while( curbox != NULL )
        {
          maxheight = mymax( maxheight, curbox->height );
          curbox = curbox->next;
        }

        height = vpad + maxheight + vpad;
        midy = -height/2;

        ulx = hpad;

        // maybe draw a decoration at very first, and
        // while we're at it, the last, since in this case
        // we know the width already
        if( literalkind(kind) )
        {
				  // jb17:  toss the old scheme of using existing letters,
					//        do hard-coded smallish block letters
          uly = -vpad;

          // all literal symbols drawn 6 wide 
          width = myround( hpad + w1 + hpad + 6 + hpad );

          // draw the type letter I F C B S on the upper right
          if(draw)
          {
            if( !texdraw )
              glColor3fv( fulltextcolor );

			      drawTypeSymbol( x + hpad + w1 + hpad, 
							            y - 10, kind );

          }

        }// decorate for literal kinds
        else if(kind==newopbox)
        {// put "copy of" image at left

          ulx = hpad;
          
          float unit = 0.5*hpad;

          if(draw)
          {
            // draw "copy of" image
            if( !texdraw )
            {// draw to screen
              glColor3fv( fulltextcolor );
              glBegin(GL_LINE_LOOP);
                glVertex3f( x+ulx, y+midy+2*unit, textz );
                glVertex3f( x+ulx+4*unit, y+midy+2*unit, 
                                                 textz );
                glVertex3f( x+ulx+4*unit, y+midy-2*unit, 
                                                  textz );
                glVertex3f( x+ulx, y+midy-2*unit, textz );
              glEnd();
              glBegin(GL_LINE_STRIP);
                glVertex3f( x+ulx+2*unit, y+midy+2*unit, 
                                                   textz );
                glVertex3f( x+ulx+2*unit, y+midy+4*unit, 
                                                  textz );
                glVertex3f( x+ulx+6*unit, y+midy+4*unit, 
                                                 textz );
                glVertex3f( x+ulx+6*unit, y+midy, textz );
                glVertex3f( x+ulx+4*unit, y+midy, textz );
              glEnd();
            }
            else
            {// draw to texfile
						  
							if( pictex )
							{
                // loop
								pictexRect( x+ulx, y+midy-2*unit,
								            x+ulx+4*unit, y+midy+2*unit, 'F' );
      
                // strip
                pictexLine( x+ulx+2*unit, y+midy+2*unit,
                             x+ulx+2*unit, y+midy+4*unit);
                pictexLine(  x+ulx+2*unit, y+midy+4*unit,
											       x+ulx+6*unit, y+midy+4*unit);
							  pictexLine( x+ulx+6*unit, y+midy+4*unit,
                            x+ulx+6*unit, y+midy  );
								pictexLine( x+ulx+6*unit, y+midy,
                             x+ulx+4*unit, y+midy  );
							}
							else
							{
                // loop
                psMoveTo( x+ulx, y+midy+2*unit  );
                psLineTo( x+ulx+4*unit, y+midy+2*unit );
                psLineTo( x+ulx+4*unit, y+midy-2*unit);
                psLineTo( x+ulx, y+midy-2*unit  );
                fprintf( texfile, "closepath\n" );
      
                // strip
                psMoveTo( x+ulx+2*unit, y+midy+2*unit);
                psLineTo( x+ulx+2*unit, y+midy+4*unit);
                psLineTo( x+ulx+6*unit, y+midy+4*unit);
                psLineTo( x+ulx+6*unit, y+midy  );
                psLineTo( x+ulx+4*unit, y+midy  );

                psStroke();
              }
            }// to texfile
          }

          ulx += 9*unit;

        }// decorate newbox on left

        // maybe handle name at far left
        if( kind==idbox || literalkind(kind) ||
            kind==unopbox || kind==newopbox )
        {// name at left

          uly = midy + (h1)/2;

          if(draw)
          {
            name->display( x+ulx, y+uly, 0, 
              dofocus && inside=='y' && (part=='n'), fulltextcolor );         
          }
          if(insideRect( find,
            ulx, uly, ulx+w1, uly-(h1),
                      targetx,targety,
                      innerx, innery ))
            partnumber = 'n';

          // is only part, just call here
          getAFInfo( getafinfo, name, 
               ulx, uly,
               0, 
               afx, afy, afw, afh );

          ulx += w1 + hpad;

        }// handle name up front
  
        // loop through the boxes
        curbox = first;
        innercount = 0;
        while( curbox !=NULL )         
        {// handle an inner box

          uly = midy + curbox->height/2;  
     
          if(draw)
            curbox->display( x+ulx, y+uly, 
                               left, right, top, bottom,
                               showfocus, focusbox );           
       
          innercount++;

          // locate curbox?
          if(locate)
          {
            curbox->ulcx = ulx;
            curbox->ulcy = uly;
          }

          // target inside this inner box?
          if(insideRect( find,
               ulx, uly, 
               ulx+curbox->width, uly-curbox->height,
             targetx, targety, innerx, innery ))
          {// found it!
            partnumber = 'I';
            innerbox = curbox;
          }

          // move to the right
          ulx += curbox->width + hpad;
          curbox = curbox->next;

          // maybe draw a decoration after the box
          if(kind==memberbox)
          {// draw the period

            ulx += hpad;
            uly = -height + 3*dx;

            if(draw)
            {// the dot
              if( !texdraw )
              {// draw dot to screen
                glColor3fv( fulltextcolor );
                glBegin(GL_POLYGON);
                  glVertex3f( x+ulx-dx, y+uly+dx, 
                                               textz );
                  glVertex3f( x+ulx+dx, y+uly + dx, 
                                                textz );
                  glVertex3f( x+ulx+dx, y+uly-dx, 
                                                textz );
                  glVertex3f( x+ulx-dx, y+uly - dx, 
                                             textz );
                glEnd();
              }
              else
              {// draw dot to texfile
							  if( pictex )
								  pictexRect( x+ulx-dx, y+uly-dx,
									            x+ulx+dx, y+uly+dx, 'F' );
								else
                  psRectangle( 1,   // fill it
                             x+ulx-dx, y+uly+dx,
                             x+ulx+dx, y+uly-dx );
              }
            }

            ulx += 2*hpad;
          }

          // maybe draw the name here
          if(innercount==1 && (kind==memberbox || kind==opbox ) )
          {// draw the name
        
            uly = midy + (h1)/2;

            if(draw)
            {
              name->display( x+ulx, y+uly, 0, 
                dofocus && inside=='y' && (part=='n'), fulltextcolor );         
            }
            if(insideRect( find,
                ulx, uly, ulx+w1, uly-(h1),
                      targetx,targety,
                      innerx, innery ))
              partnumber = 'n';

            // call either above or here, for some kinds
            getAFInfo( getafinfo, name, 
               ulx, uly,
               0, 
               afx, afy, afw, afh );

            ulx += w1 + hpad;

          }// do name after inner box 1

        } // end of loop to draw inner boxes

        // figured width for literal box earlier, others need
				// transfer from ulx
        if( ! literalkind(kind) )
          width = ulx;

      } // end of value boxes with name and inners

      else if( kind==assbox || kind==growbox
             )
      {// action boxes with a name, and special arrows that mess up the
        // vertical work somewhat

        // figure out the height so we can vertically center 
        // the inner boxes:
        name->howBig( 0, w1, h1 );      
        maxheight = h1;

        curbox = first;
        while( curbox != NULL )
        {
          maxheight = mymax( maxheight, curbox->height );
          curbox = curbox->next;
        }

        // allow space on bottom for data flow indicator arrow
        //  later, so doesn't get split top/bottom
        height = vpad + maxheight + vpad;
        midy = -height/2;

        ulx = hpad;

        // maybe handle name at far left
        if( kind==growbox )
        {// name at left

          uly = midy + (h1)/2;

          if(draw)
          {
            name->display( x+ulx, y+uly, 0, 
              dofocus && inside=='y' && (part=='n'), fulltextcolor);         
          }
          if(insideRect( find,
            ulx, uly, ulx+w1, uly-(h1),
                      targetx,targety,
                      innerx, innery ))
            partnumber = 'n';

          getAFInfo( getafinfo, name, 
               ulx, uly,
               0, 
               afx, afy, afw, afh );

          ulx += w1 + hpad;

        }// handle name up front
  
        // loop through the boxes
        curbox = first;
        innercount = 0;
        while( curbox !=NULL )         
        {// handle an inner box

          uly = midy + curbox->height/2;  
     
          if(draw)
            curbox->display( x+ulx, y+uly, 
                               left, right, top, bottom,
                               showfocus, focusbox );
          
          innercount++;

          // locate curbox?
          if(locate)
          {
            curbox->ulcx = ulx;
            curbox->ulcy = uly;
          }

          // target inside this inner box?
          if(insideRect( find,
               ulx, uly, 
               ulx+curbox->width, uly-curbox->height,
             targetx, targety, innerx, innery ))
          {// found it!
            partnumber = 'I';
            innerbox = curbox;
          }

          // maybe note an important location
          if(innercount==1)
          {// note location of center bottom of first inner box
            fx = ulx + curbox->width/2;
            fy = uly - curbox->height;
          }
          else if(innercount==2 && kind==assbox)
          {// note location of center bottom of second inner box
            sx = ulx + curbox->width/2;
            sy = uly - curbox->height;
          }

          // move to the right
          ulx += curbox->width + hpad;
          curbox = curbox->next;

          // maybe draw the name here
          if(innercount==1 && kind==assbox)
          {// draw the name
        
            uly = midy + (h1)/2;

            if(draw)
            {
              name->display( x+ulx, y+uly, 0, 
                dofocus && inside=='y' && (part=='n'), fulltextcolor);         
            }
            if(insideRect( find,
                ulx, uly, ulx+w1, uly-(h1),
                      targetx,targety,
                      innerx, innery ))
              partnumber = 'n';

            getAFInfo( getafinfo, name, 
               ulx, uly,
               0, 
               afx, afy, afw, afh );

            ulx += w1 + hpad;

          }// do name after inner box 1

        } // end of loop to draw inner boxes

        width = ulx;
        height += flowarrowpad; // leave room on bottom for arrow

        // draw data flow indicator arrow
        if(kind==assbox)
        {// draw from second back to first
          drawline( x+sx, y+sy, x+sx, y - height + 0.5*flowarrowpad,
			       fulltextcolor );
          drawline( x+sx, y - height + 0.5*flowarrowpad,
                    x+fx, y - height + 0.5*flowarrowpad, 
					    fulltextcolor );
          drawflowarrow( x+fx, y-height+0.5*flowarrowpad,
                     x+fx, y+fy, fulltextcolor );
        }
        else if(kind==growbox)
        {// draw from left edge to first
          drawline( x+hpad, y-height+0.5*flowarrowpad,
                    x+fx, y-height+0.5*flowarrowpad, fulltextcolor );
          drawflowarrow( x+fx, y-height+0.5*flowarrowpad,
                         x+fx, y+fy, fulltextcolor );
        }

      }// end of action boxes with special data flow arrow decorations

      else if(kind==returnbox)
      {// just has a single inner box, and a decorating
        // outflow arrow

        ulx = hpad;
        uly = -flowarrowpad;  // space at top for outflow arrow

        curbox = first;

        if(draw)
          curbox->display( x+ulx, y+uly, 
                               left, right, top, bottom,
                               showfocus, focusbox );           
        // locate curbox?
        if(locate)
        {
          curbox->ulcx = ulx;
          curbox->ulcy = uly;
        }

        // target inside this inner box?
        if(insideRect( find,
             ulx, uly, 
             ulx+curbox->width, uly-curbox->height,
             targetx, targety, innerx, innery ))
        {// found it!
          partnumber = 'I';
          innerbox = curbox;
        }

        // note location of middle right of this single box
        fx = ulx + curbox->width;
        fy = uly - curbox->height/2;

        width = fx + hpad + flowarrowpad + hpad;
        height = flowarrowpad + curbox->height + vpad;
        
        // draw data flow indicator arrow
        drawline( x+fx, y+fy, x+width-0.5*flowarrowpad-hpad, y+fy,
			         fulltextcolor );
        drawflowarrow( x+width-0.5*flowarrowpad-hpad, y+fy,
                       x+width-0.5*flowarrowpad-hpad, y-vpad,
					   fulltextcolor );
        
        // return apparent focus info, will just be for me, of course
        getAFInfo( getafinfo, NULL, 
               ulx, uly,
               0, 
               afx, afy, afw, afh );

      } // returnbox

      else if(kind==javabox)   // jb16
      {// just has a single inner box

        ulx = hpad;
        uly = -vpad;

        curbox = first;

        if(draw)
          curbox->display( x+ulx, y+uly, 
                               left, right, top, bottom,
                               showfocus, focusbox );           
        // locate curbox?
        if(locate)
        {
          curbox->ulcx = ulx;
          curbox->ulcy = uly;
        }

        // target inside this inner box?
        if(insideRect( find,
             ulx, uly, 
             ulx+curbox->width, uly-curbox->height,
             targetx, targety, innerx, innery ))
        {// found it!
          partnumber = 'I';
          innerbox = curbox;
        }

        width = hpad + curbox->width + hpad;
        height = vpad + curbox->height + vpad;
        
        // return apparent focus info, will just be for me, of course
        getAFInfo( getafinfo, NULL, 
               ulx, uly,
               0, 
               afx, afy, afw, afh );

      } // javabox

      else if( kind==arraybox || kind==array2dbox ||
               kind==callbox )
      {// kinds with at least one inner box and some decorations

        // figure out the height so we can vertically center 
        // the inner boxes:

        // initialize to height of decorations
        if( kind==callbox )
          maxheight = 18*dx;
        else
          maxheight = 0;

        curbox = first;
        while( curbox != NULL )
        {
          maxheight = mymax( maxheight, curbox->height );
          curbox = curbox->next;
        }

        height = vpad + maxheight + vpad;
        midy = -height/2;

        ulx = 0;

        // maybe do initial decoration---hmmm, I guess not!
          
        // loop through the boxes, with decoration in the middle
        curbox = first;
        ulx += hpad;
        innercount = 0;

        while( curbox !=NULL )         
        {// handle an inner box

          uly = midy + curbox->height/2;  
     
          if(draw)
            curbox->display( x+ulx, y+uly, 
                               left, right, top, bottom,
                               showfocus, focusbox );           
       
          innercount++;

          // locate curbox?
          if(locate)
          {
            curbox->ulcx = ulx;
            curbox->ulcy = uly;
          }

          // target inside this inner box?
          if(insideRect( find,
               ulx, uly, 
               ulx+curbox->width, uly-curbox->height,
             targetx, targety, innerx, innery ))
          {// found it!
            partnumber = 'I';
            innerbox = curbox;
          }

          // move to the right
          ulx += curbox->width + hpad;
          curbox = curbox->next;

          // decorate if on correct inner
          if(innercount==1 && kind==callbox)
          {// draw left paren after method reference
            if(draw)
              drawparen( x+ulx, y+midy, height, 1,
			                fulltextcolor );
            ulx += (4./36)*height + hpad;
          }

          else if(innercount==1 && 
                    (kind==arraybox || kind==array2dbox ) )
          {// left bracket

            if(draw)
            {
              drawbracket( x+ulx, y-height/2, (7./9)*height, 1,
				  fulltextcolor );
            } 

            // adjust geo info, even if not drawing
            ulx += (1./9)*height + hpad; // for going on

          } // left bracket

          else if(curbox==NULL && 
                    (kind==arraybox || kind==array2dbox ) )
          {// right bracket

            ulx += (1./9)*height;

            if(draw)
            {
              drawbracket( x+ulx, y-height/2, (7./9)*height, -1,
				  fulltextcolor );
            } 

            // adjust geo info, even if not drawing
            ulx += hpad; // for going on

          } // left bracket
         
          else if(innercount==2 && kind==array2dbox )
          {// comma

            ulx += hpad;

            uly = -height + 3*dx;

            if(draw)
            {
              if( !texdraw )
              {// draw to screen
                // the dot
                glColor3fv( fulltextcolor );
                glBegin(GL_POLYGON);
                  glVertex3f( x+ulx-dx, y+uly+dx, 
                                               textz );
                  glVertex3f( x+ulx+dx, y+uly + dx, 
                                                textz );
                  glVertex3f( x+ulx+dx, y+uly-dx, 
                                                textz );
                  glVertex3f( x+ulx-dx, y+uly - dx, 
                                             textz );
                glEnd();
                // the tail
                glBegin(GL_POLYGON);
                  glVertex3f( x+ulx-dx, y+uly - dx, 
                                               textz );
                  glVertex3f( x+ulx+dx, y+uly - dx, 
                                                textz );
                  glVertex3f( x+ulx - 2*dx, y+uly-3*dx, 
                                                textz );
                glEnd();
              }
              else
              {// draw comma to texfile
							  if( pictex )
								{
								  pictexRect( x+ulx-dx, y+uly-dx,
									            x+ulx+dx, y+uly+dx, 'F' );
                  pictexTriangle( x+ulx-dx, y+uly-dx,
									                x+ulx+dx, y+uly-dx,
																	x+ulx-2*dx, y+uly-3*dx );
								}
								else
								{
                  psRectangle( 1,  // fill it
                             x+ulx-dx, y+uly+dx,
                             x+ulx+dx, y+uly-dx );
                  psTriangle( x+ulx-dx, y+uly-dx,
                            x+ulx+dx, y+uly-dx,
                            x+ulx-2*dx, y+uly-3*dx );
								}
              }
            }

            ulx += 2*hpad;
        
          }// comma

          // more decoration, possibly after first if only one inner
          if(curbox==NULL && kind==callbox)
          {// draw right paren at end of inner list
            if(draw)
              drawparen( x+ulx+(4./36)*height, y+midy, height, -1,
			               fulltextcolor );
            ulx += (4./36)*height + hpad;
          }

        } // end of loop to draw inner boxes

        width = ulx;

        // if I'm focus, guaranteed is on my border, so
        //  this guy will kick back my info, ignoring the
        //  argument for the parameter "s"
        getAFInfo( getafinfo, NULL, 
               ulx, uly,
               0, 
               afx, afy, afw, afh );

      } // end of arraybox

      // quite unique---has multiple rows of different kinds
      // of inner boxes --- 
      //         classes
      //         stack
      //         heap
      //         console (later)
      else if( kind == univbox )
      {//  universe box

        // handle name
        name->howBig(0,w1,h1);

        if(draw)
          name->display( x+hpad, y-vpad, 0, 
                     dofocus && inside=='y' && (part=='n'), 
					  fulltextcolor );

        width = hpad + w1; 
        height = 2*vpad + h1;  // height of first "row"

        if(insideRect( find,
                        0, 0, 2*hpad+w1, -(2*vpad+h1),
                      targetx,targety,
                      innerx, innery ))
          partnumber = 'n';

        // draw the symbol0 for inner list
        ulx = hpad; uly = -vpad-h1-vpad;

        if(draw)
        {
          if(dofocus && inside=='y' && (part=='i'))
            drawsymbol0(x+ulx,y+uly,2,w2,h2);
          else
            drawsymbol0(x+ulx,y+uly,1,w2,h2);
        }
        else
          drawsymbol0(x+ulx,y+uly,0,w2,h2); // just obtain size

        // symbol0 might be wider than the name
        width = mymax( width, hpad + w2);
        
        // determine whether on part 'i' --- symbol0 for inner list       
        if(insideRect( find,
            0, -(2*vpad+h1), hpad+w2, -(3*vpad+h1+h2),
            targetx, targety, innerx, innery ))
           partnumber = 'i';

        ulx += w2 + hpad;  // adjust to the right on same y value
        maxheight = h2;  // monitor highest box in first row
                         // without vpadding
        leftmargin = ulx;

        // jb19:  some reworking here to allow for lessonboxes
				//        being horizontal

				int doinglessons = 0;

        // draw all the inner boxes
        // (just take care to move to next line when doing
        //  any non-class box)
        curbox = first;
        while( curbox != NULL )
        {// curbox is inner box to be displayed at current spot

          // if curbox is not a class box, move to next line
          if( curbox->kind != classbox 
					          &&
							!doinglessons // jb19: flag first lessonbox
					  )
          {
            ulx = leftmargin;
            uly = uly - maxheight - vpad;
          }

          if( !doinglessons && curbox->kind == lessonbox )
					{// hit first lessonbox
					  doinglessons = 1;
						maxheight = curbox->height;
					}

          // draw the box
          // recursive call to this guy, not finding or locating
          if(draw)
            curbox->display( x+ulx, y+uly, 
                             left, right, top, bottom,
                             showfocus, focusbox );           

          // update maxheight, width
          if( curbox->kind == classbox || 
					    doinglessons )
          {// updating maxheight = height on classbox row,
            // width = width of classbox row
            maxheight = mymax( maxheight, curbox->height );
            width = mymax( width, ulx + curbox->width );
          }
          else
          {// updating vertically now
            maxheight = curbox->height;
            width = mymax( width, ulx + curbox->width );
          }

          // locate curbox?
          if(locate)
          {
            curbox->ulcx = ulx;
            curbox->ulcy = uly;
          }

          // target inside this inner box?
          if(insideRect( find,
               ulx, uly, 
               ulx+curbox->width, uly-curbox->height,
               targetx, targety, innerx, innery ))
          {// found it!
            partnumber = 'I';
            innerbox = curbox;
          }

          // move to the right pointerwise and geometrically
          ulx += curbox->width + hpad;
          curbox = curbox->next;

        }

        // add last row
        height = -uly + maxheight + vpad;

        // have found width as farthest extent, add right hpad
        width += hpad;

        // only now that we know the width can we return 
        // accurate afinfo
        if(getafinfo)
        {
          if(part=='n')
            getAFInfo( getafinfo, name,
                  hpad, -vpad,
                  0,
                  afx, afy, afw, afh );
          else if(part=='i')       
            getAFInfo( getafinfo, NULL,
                  hpad, -vpad-h1-vpad,
                  0,
                  afx, afy, afw, afh );
          else
            getAFInfo( getafinfo, NULL,
                  0, 0,
                  0,
                  afx, afy, afw, afh );
        }

      }// end of univbox

      // kinds that have a name and even left edges
      // vertical inner list
			// v10:  now is classbox, portsbox, put the method boxes
			//       in their own chunk with addition of retvalbox
      else if(kind==classbox ||	kind==portsbox )
      {
        // handle name
        name->howBig(0,w1,h1);
      
        if(draw)
        {
          name->display( x+hpad, y-vpad, 0, 
                     dofocus && inside=='y' && (part=='n'), 
					 fulltextcolor );
        }

        // initialize the width to the least it can be across
        // the top row
        width = hpad + w1; 

        height = 2*vpad + h1;  // height of first "row"

        if(insideRect(find,
                     0, 0, hpad+w1, -(vpad+h1),                    
                     targetx,targety,
                      innerx, innery ))
          partnumber = 'n';

        ulx = hpad;  // adjust to the right on same y value
        leftmargin = ulx;
        uly = -height;

        // draw the inner boxes
        curbox = first;
        while( curbox != NULL )
        {// curbox is to be displayed at current spot
          // draw the box
          // recursive call to this guy, not finding or locating
          if(draw)
            curbox->display( x+ulx, y+uly,
                             left, right, top, bottom,
                             showfocus, focusbox );           

          // update height, width
          height += curbox->height + vpad;         
          width = mymax( width, ulx + curbox->width );

          // locate curbox?
          if(locate)
          {
            curbox->ulcx = ulx;  // offsets in outer
            curbox->ulcy = uly;
          }

          // find?
          if(insideRect(find,
               ulx, uly, 
               ulx+curbox->width, uly-curbox->height,
               targetx, targety, innerx, innery ))
          {
            partnumber = 'I';
            innerbox = curbox;
          }

          // move to next box pointerwise and geometrically
          uly -= curbox->height + vpad;
          curbox = curbox->next;

        }

        // have found width as farthest extent, add right hpad
        width += hpad;

        // now that we have the true width, finish up:
        
        if( part=='n')
          getAFInfo( getafinfo, name,
                  hpad, -vpad,
                  0,
                  afx, afy, afw, afh );
        else if( part=='i' )
          getAFInfo( getafinfo, NULL,
                       0, 0, 
                       0,
                       afx, afy, afw, afh );
        else
          errorexit("unknown part in class, portsbox");

      } // name and vertical inner list kinds of boxes

      // v10:
			// method box kinds (three)
			//  change so second inner is arranged to the right of first
      else if( kind==methodbox ||
               kind==classmethodbox || kind==instancemethodbox )
      {
        // handle name
        name->howBig(0,w1,h1);
      
        // maybe do type
        int dotype;  // little flag for my convenience
        if( kind==classmethodbox || kind==instancemethodbox )
          dotype=1;
        else
          dotype=0;

        if(dotype)
          type->howBig( 0, w2, h2 );

        if(draw)
        {
          name->display( x+hpad, y-vpad, 0, 
                     dofocus && inside=='y' && (part=='n'), 
					 fulltextcolor );
        }

        // initialize the width to the least it can be across
        // the top row
        if( dotype )
          width = hpad + w1 + hpad + w2;
        else
          width = hpad + w1; 

        height = 2*vpad + h1;  // height of first "row"

        if(insideRect(find,
                     0, 0, hpad+w1, -(vpad+h1),                    
                     targetx,targety,
                      innerx, innery ))
          partnumber = 'n';

        ulx = hpad;  // adjust to the right on same y value
        leftmargin = ulx;
        uly = -height;

        // draw the inner boxes
        curbox = first;
        int innercount = 1;  // second inner behaves differently
        float firstrowheight = 0;
				float firstrowwidth = 0;

        while( curbox != NULL )
        {// curbox is to be displayed at current spot
          // draw the box
          // recursive call to this guy, not finding or locating
          if(draw)
            curbox->display( x+ulx, y+uly,
                             left, right, top, bottom,
                             showfocus, focusbox );           

          // update height, width
					if( innercount == 1 )
					{//just drew the first guy, don't know all about first row yet
					  firstrowheight = curbox->height;
						firstrowwidth = ulx + curbox->width;
					}
					else if( innercount == 2 )
					{// finished first row, update first row info, overall info
            firstrowheight = mymax( firstrowheight, curbox->height );
						firstrowwidth += curbox->width + hpad;
            height += firstrowheight + vpad;
						width = mymax( width, firstrowwidth );
					}
					else // innercount == 3 or 4				
					{// placed this inner vertically
            height += curbox->height + vpad;         
            width = mymax( width, ulx + curbox->width );
					}

          // locate curbox?
          if(locate)
          {
            curbox->ulcx = ulx;  // offsets in outer
            curbox->ulcy = uly;
          }

          // find?
          if(insideRect(find,
               ulx, uly, 
               ulx+curbox->width, uly-curbox->height,
               targetx, targety, innerx, innery ))
          {
            partnumber = 'I';
            innerbox = curbox;
          }

          // move to next box pointerwise and geometrically
					if( innercount == 1 )
					{// move on to second inner
            ulx += curbox->width + hpad; // tops even on same row					  
					}
					else if( innercount == 2 )
					{// move on to third inner
					  uly -= firstrowheight + vpad;
						ulx = hpad;
					}
					else // innercount == 3 or 4
					{// placed vertically 
            uly -= curbox->height + vpad;
					}

          curbox = curbox->next;

					innercount++;

        }

        // have found width as farthest extent, add right hpad
        width += hpad;

        // now that we have the true width, finish up:
        
        if(dotype)
        {
          type->display( x+width-w2-hpad, y-vpad, 0,
                         dofocus && inside=='y' && (part=='t'),
                               fulltextcolor );

          if( insideRect(find,
                     width-w2-hpad, 0, 
                         width, -(vpad+h1),                    
                     targetx,targety,
                      innerx, innery ) )
            partnumber = 't';
        }

        if( part=='n')
          getAFInfo( getafinfo, name,
                  hpad, -vpad,
                  0,
                  afx, afy, afw, afh );
        else if( part=='t' )
          getAFInfo( getafinfo, type,
                  width-w2-hpad, -vpad,
                  0,
                  afx, afy, afw, afh );
        else if( part=='i' )
          getAFInfo( getafinfo, NULL,
                       0, 0, 
                       0,
                       afx, afy, afw, afh );
        else
          errorexit("unknown part in class, method,...");

      } // name and vertical inner list kinds of boxes

      // name, type across top row, inner flat tops horizontal list
			// 6/26/2000:  change, like other databox holders, to wrap
			//    after 4 
      else if(kind==instancebox )        
      {
        // handle name
        name->howBig(0,w1,h1);     
        type->howBig( 0, w2, h2);

        if(draw)
          name->display( x+hpad, y-vpad, 0, 
                     dofocus && inside=='y' && (part=='n'), 
					 fulltextcolor );

        width = hpad + w1 + hpad + w2 + hpad; 
        height = 2*vpad + mymax(h1,h2);  // height of top row
        
        if(insideRect( find,
                        0, 0, 2*hpad+w1, -(2*vpad+h1),
                      targetx,targety,
                      innerx, innery ))
          partnumber = 'n';

        // draw the symbol0 for inner list
        ulx = hpad; uly = -vpad-h1-vpad;

        if(draw)
        {
          if(dofocus && inside=='y' && (part=='i'))
            drawsymbol0(x+ulx,y+uly,2,w2,h2);
          else
            drawsymbol0(x+ulx,y+uly,1,w2,h2);
        }
        else
          drawsymbol0(x+ulx,y+uly,0,w2,h2); // just obtain size

        // symbol0 might be wider than the name
        width = mymax( width, hpad + w2);
        
        // determine whether on part 'i' --- symbol0 for inner list       
        if(insideRect( find,
            0, -(2*vpad+h1), hpad+w2, -(3*vpad+h1+h2),
            targetx, targety, innerx, innery ))
           partnumber = 'i';

        ulx += w2 + hpad;  // adjust to the right on same y value
        maxheight = h2;  // monitor highest box in first row
                         // without vpadding
        leftmargin = ulx;

        // draw all the inner boxes, with tops on same level
				//  wrap after 4

				int maxInRow = 4, numInRow = 0;

        curbox = first;
        while( curbox != NULL )
        {// curbox is the box to be displayed at current spot
        
          // draw the box
          // recursive call to this guy, not finding or locating
          if(draw)
            curbox->display( x+ulx, y+uly, 
                             left, right, top, bottom,
                             showfocus, focusbox );           

          // update maxheight, width
          maxheight = mymax( maxheight, curbox->height );
          width = mymax( width, ulx + curbox->width );

          // locate curbox?
          if(locate)
          {
            curbox->ulcx = ulx;
            curbox->ulcy = uly;
          }

          // target inside this inner box?
          if(insideRect( find,
               ulx, uly, 
               ulx+curbox->width, uly-curbox->height,
               targetx, targety, innerx, innery ))
          {// found it!
            partnumber = 'I';
            innerbox = curbox;
          }

          // move to the right pointerwise and geometrically
          ulx += curbox->width + hpad;
          curbox = curbox->next;

  				// see if should start new row
					numInRow++;  // just did an inner box

					if( numInRow == maxInRow && curbox != NULL )
					{// finish current row---there is a next row
					  height += maxheight + vpad;  // the row is done
            width = mymax( width, ulx ); // update width for row
            ulx = leftmargin;  // move to beginning of next row
					  uly -= maxheight + vpad;

						numInRow = 0;
					}

        }

        // add row of inner boxes to total height
        height += maxheight + vpad;

        // update width with final hpad
        width += hpad;
        
        // now that we have width, do stuff for the type
        type->howBig( 0, w2, h2 );  // had reused w2,h2, lazy way
                                       // out!
        if( draw )
          type->display( x+width-w2-hpad, y-vpad, 0,
                         dofocus && inside=='y' && (part=='t'),
                               fulltextcolor );

        if( insideRect(find,
                   width-w2-hpad, 0, 
                     width, -(vpad+h1),                    
                   targetx,targety,
                    innerx, innery ) )
          partnumber = 't';

        // only now that we know the width can we return 
        // accurate afinfo
        if(getafinfo)
        {
          if(part=='n')
            getAFInfo( getafinfo, name,
                  hpad, -vpad,
                  0,
                  afx, afy, afw, afh );
          else if(part=='t')
            getAFInfo( getafinfo, type,
                  width-w2-hpad, -vpad,
                  0,
                  afx, afy, afw, afh );
          else if(part=='i')       
            getAFInfo( getafinfo, NULL,
                  hpad, -vpad-h1-vpad,
                  0,
                  afx, afy, afw, afh );
          else
            getAFInfo( getafinfo, NULL,
                  0, 0,
                  0,
                  afx, afy, afw, afh );
        }

      } // name, type, horizontal flat tops innerlist      
      
      // seq box --- is quite unique --- centered vertical
      // list of 0 or more, with connecting central arrows drawn
      else if( kind == seqbox ||
			         kind == lessonbox //jb19 --- just like seqbox
						 )
      {
        // first determine the width for centering:

        drawsymbol0(x,y,0,w1,h1); // just obtain size of symbol0
        maxwidth = w1;

        // find widest inner box:

          curbox = first;
          while( curbox != NULL )
          {
            if(curbox->width > maxwidth)
              maxwidth = curbox->width;
            curbox = curbox->next;
          }

        // can now set the width of me
        width = hpad + maxwidth + hpad;

        // midx is the center x value 
        midx = hpad + maxwidth/2;

        // draw the symbol0 for inner list
        ulx = midx - w1/2;
        uly = -vpad;   

        if(draw)
        {
          if(dofocus && inside=='y' && (part=='i'))
            drawsymbol0(x+ulx,y+uly,2,w2,h2);
          else
            drawsymbol0(x+ulx,y+uly,1,w2,h2);
        }
        // else --- no need to obtain size again --- have w1,h1
      
        // adjust downwards
        uly -= h1;           // had h2 before, screws up finding

        // determine whether on part 'i' --- symbol0 for inner list       
        if(insideRect( find,
            ulx, -vpad, ulx+w1, -(vpad+h1),
            targetx, targety, innerx, innery ))
           partnumber = 'i';

        // maintain height as we draw downward
        height = vpad + h1;  // height of symbol0
        
        // draw the inner boxes
        curbox = first;
        while( curbox != NULL )
        {
          // draw arrow from (midx, uly) down distance arrowpad
          drawarrow( x + midx, y + uly, 
                         x + midx, y + uly - arrowpad,
						 fulltextcolor );

          height += arrowpad;

          // adjust uly downward
          uly -= arrowpad;

          // set ulx correctly:
          ulx = midx - curbox->width/2;

          // draw the box
          // recursive call to this guy, not finding or locating
          if(draw)
            curbox->display( x + ulx, y+uly,
                             left, right, top, bottom,
                             showfocus, focusbox );           

          // update height, width
          height += curbox->height;         
          
          // locate curbox?
          if(locate)
          {
            curbox->ulcx = ulx;  // offsets in outer
            curbox->ulcy = uly;
          }

          // find?
          if(insideRect(find,
               ulx, uly, 
               ulx+curbox->width, uly-curbox->height,
               targetx, targety, innerx, innery ))
          {
            partnumber = 'I';
            innerbox = curbox;
          }

          // move to next box pointerwise and geometrically
          uly -= curbox->height;
          curbox = curbox->next;

        }

        // draw final arrow and set final height
        drawarrow( x+midx, y+uly, 
                       x+midx, y+uly-arrowpad,
					   fulltextcolor );

        height += arrowpad;

        // get apparent focus info---asking for symbol0,
        //  or if not inside, will kick back my info
				// v11:  noticed not working on symbol0, replace 0,0
				//   by ulc of symbol0, namely midx-w1/2,-vpad
        getAFInfo( getafinfo, NULL, 
                     midx-w1/2, -vpad,  // v11:  was 0,0
                     0,
                      afx, afy, afw, afh );

      } // seq box

      // jb21:  a new kind of box
      else if( kind == notebox )
      {// 5 inner parboxes in rows of length 2, 2, 1

        // maintain height as we draw downward
        height = vpad;  // leave a little space at the very top
        width = hpad;
        ulx = hpad;
        uly = -vpad;
        
        // draw the inner boxes and figure width, height as we go
        curbox = first;
        for( k=1; k<=5; k++ )
        {
          // draw the box
          // recursive call to this guy, not finding or locating
          if(draw)
            curbox->display( x + ulx, y+uly,
                             left, right, top, bottom,
                             showfocus, focusbox );           

          // locate and find before updating ulx, uly:

          // locate curbox?
          if(locate)
          {
            curbox->ulcx = ulx;  // offsets in outer
            curbox->ulcy = uly;
          }

          // find?
          if(insideRect(find,
               ulx, uly, 
               ulx+curbox->width, uly-curbox->height,
               targetx, targety, innerx, innery ))
          {
            partnumber = 'I';
            innerbox = curbox;
          }

          // update height, width, and ulx, uly for next inner

            // update guys depending on position in row
            if( k == 5 )
            {// last one is first and last in its row
              height +=  curbox->height + vpad;
              maxwidth = hpad + curbox->width + hpad;
              if( maxwidth > width )
                width = maxwidth;
            }
            else if( k % 2 == 1 )
            {// first in its row
              maxheight = curbox->height;
              maxwidth = hpad + curbox->width;

              ulx += curbox->width + hpad;
            }
            else 
            {// second and last in its row
              if( curbox->height > maxheight )
                maxheight = curbox->height;
              maxwidth += hpad + curbox->width + hpad;

              height += maxheight + vpad;
              if( width < maxwidth )
                width = maxwidth;

              ulx = hpad;
              uly -= maxheight + vpad;                 
            }

          // move to next box pointerwise
          curbox = curbox->next;

        }// end of for loop to draw 5 inner boxes

        getAFInfo( getafinfo, NULL, 
                     0, 0, 
                     0,
                      afx, afy, afw, afh );

      }// end of notebox display

      else if( kind == branchbox )
      {
        // determine maxheight and maxheight2 for the two rows

        // fool!  I didn't have them initialized, was behaving
        //     very strangely (mouse clicking apparently worked
        //     when put in a cout way off somewhere else, didn't
        //      work if didn't).  Let this be a lesson to you!
        //  The funny (?) thing is that when we ran through this
        //  code to display, it was getting correct height, then
        //  screwing up later when ran through to do a find.
        maxheight = 0;
        maxheight2 = 0;

        curbox = first;
        innercount = 0;
        while( curbox != NULL )
        {
          innercount++;  // now is index of curbox

          if( innercount % 2 == 0 || curbox->next == NULL )
          {// curbox is on second row
            if( curbox->height > maxheight2 )
              maxheight2 = curbox->height;
          }
          else
          {// curbox on first row
            if( curbox->height > maxheight )
              maxheight = curbox->height;
          }

          curbox = curbox->next;
        }

        // set the centerlines

        // need height of symbol0
        drawsymbol0(x, y,0,w2,h2); // ulc doesn't matter here

        midy = -h2 - arrowpad - maxheight/2;
        midy2 = -h2 - arrowpad -maxheight - arrowpad - maxheight2/2;

        height = h2 + arrowpad + maxheight + arrowpad + 
                            maxheight2 + arrowpad;

        // scan in pairs
        firstbox = first;
        leftmargin = hpad;

        for( k=1; k<= innercount/2; k++ )
        {// handle pair k

          secondbox = firstbox->next;

          // determine (ulx,uly) and (ulx2,uly2)
          width = mymax( firstbox->width, secondbox->width );
            // (using width as a temporary utility here)
          midx = leftmargin + width/2;
          ulx = midx - firstbox->width/2;
          ulx2 = midx - secondbox->width/2;
          uly = midy + firstbox->height/2;
          uly2 = midy2 + secondbox->height/2;

          if(k==1)
          {// handle symbol0 and arrow down to first

            ulx0 = midx - w2/2;
            uly0 = -vpad;   

            if(draw)
            {
              if(dofocus && inside=='y' && (part=='i'))
                drawsymbol0(x+ulx0,y+uly0,2,w2,h2);
              else
                drawsymbol0(x+ulx0,y+uly0,1,w2,h2);
            }
            else
              drawsymbol0(x+ulx0, y+uly0,0,w2,h2);
              
            // determine whether on part 'i' --- symbol0 for inner list       
            if(insideRect( find,
                ulx0, uly0, ulx0+w2, uly0-h2,
                targetx, targety, innerx, innery ))
              partnumber = 'i';

            // get apparent focus info for symbol0 if inside,
            // or just return info for me
            getAFInfo( getafinfo, NULL, 
                     ulx0, uly0, 
                     0,
                      afx, afy, afw, afh );
            
            // draw the arrow
            if(draw)
              drawarrow( x+midx, y+uly0-h2, x+midx, y+uly, 
			               fulltextcolor );

          } // special work for first box

          // do all the stuff for first
            // draw the box
            if(draw)
              firstbox->display( x + ulx, y+uly,
                             left, right, top, bottom,
                             showfocus, focusbox );                   
            // locate firstbox?
            if(locate)
            {
              firstbox->ulcx = ulx;  // offsets in outer
              firstbox->ulcy = uly;
            }

            // find?
            if(insideRect(find,
                 ulx, uly, 
                 ulx+firstbox->width, uly-firstbox->height,
                 targetx, targety, innerx, innery ))
            {
              partnumber = 'I';
              innerbox = firstbox;
            }

            // draw arrow from stored "out" point of guy to my
            // left, straight over to my left edge (before I
            // set my own out point)
            if( draw && k > 1 )
              drawarrow( x+outx, y+midy, x+ulx, y+midy,
			                 fulltextcolor );
            
            // note in (fx,fy) my bottom, and (outx,outy) my right
            //  except fx is always midx, outy is always midy,
            //  so don't waste time
            fy = uly - firstbox->height;
            outx = ulx + firstbox->width;

          // do all the stuff for second
            // draw the box
            if(draw)
              secondbox->display( x + ulx2, y+uly2,
                             left, right, top, bottom,
                             showfocus, focusbox );                   
            // locate secondbox?
            if(locate)
            {
              secondbox->ulcx = ulx2;  // offsets in outer
              secondbox->ulcy = uly2;
            }

            // find?
            if(insideRect(find,
                 ulx2, uly2, 
                 ulx2+secondbox->width, uly2-secondbox->height,
                 targetx, targety, innerx, innery ))
            {
              partnumber = 'I';
              innerbox = secondbox;
            }

          // draw two downward arrows
          if(draw)
          {
            drawarrow( x+midx, y+fy, x+midx, y+uly2,
				          fulltextcolor );
            drawarrow( x+midx, y+uly2-secondbox->height, 
                       x+midx, y-height,
					    fulltextcolor );
          }
          
          // move on to next pair
          firstbox = secondbox->next;
          leftmargin += width + arrowpad;

        }// loop to handle pairs

        // handle the last guy (in firstbox at loop exit)

            // if loop was skipped, need to handle symbol0 still
            if(innercount==1)
            {
              // handle symbol0 since loop was skipped
              ulx0 = hpad;
              uly0 = -vpad;   

              if(draw)
              {
                if(dofocus && inside=='y' && (part=='i'))
                  drawsymbol0(x+ulx0,y+uly0,2,w2,h2);
                else
                  drawsymbol0(x+ulx0,y+uly0,1,w2,h2);
              }
              else
                drawsymbol0(x+ulx0, y+uly0,0,w2,h2);
              
              // determine whether on part 'i'       
              if(insideRect( find,
                  ulx0, uly0, ulx0+w2, uly0-h2,
                  targetx, targety, innerx, innery ))
                partnumber = 'i';

              // get apparent focus info for symbol0 if inside,
              // or just return info for me
              getAFInfo( getafinfo, NULL, 
                       ulx0, uly0, 
                       0,          
                       afx, afy, afw, afh );

              // compute info for last box in this case
              // get ulx, uly for this guy as if second in pair:
              width = firstbox->width;
              // (using width as a temporary utility here)
              leftmargin += arrowpad/2;
              midx = leftmargin + width/2;
              ulx = midx - firstbox->width/2;
              uly = midy2 + firstbox->height/2;

              // line down from symbol0 to center of first row,
              // sideways to center of default last box
              outx = ulx0+w2/2;  // coming out of the side of
                                 // the nonexistent first row box
              if(draw)
              {
                drawline( x+outx, y+uly0-h2, 
                          x+outx, y+midy, fulltextcolor );
                drawline( x+outx, y+midy, 
                          x+ulx+firstbox->width/2, y+midy,
						  fulltextcolor );
              }

            }// have just the last box
            else
            {// compute info for last box in standard case
              // get ulx, uly for this guy as if second in pair:
              width = firstbox->width;
              // (using width as a temporary utility here)
              midx = leftmargin + width/2;
              ulx = midx - firstbox->width/2;
              uly = midy2 + firstbox->height/2;
            }
            
            // draw the box
            if(draw)
            firstbox->display( x + ulx, y+uly,
                             left, right, top, bottom,
                             showfocus, focusbox );                   
            // locate firstbox?
            if(locate)
            {
              firstbox->ulcx = ulx;  // offsets in outer
              firstbox->ulcy = uly;
            }

            // find?
            if(insideRect(find,
                 ulx, uly, 
                 ulx+firstbox->width, uly-firstbox->height,
                 targetx, targety, innerx, innery ))
            {
              partnumber = 'I';
              innerbox = firstbox;
            }       

            if(draw && innercount > 1)
            {// draw line sideways from previous value box
              drawline( x+outx, y+midy, x+midx, y+midy, 
				       fulltextcolor );
            }

            // draw arrow down from top row to my top
            // and from my bottom to edge
            if(draw)
            {
              drawarrow( x+midx, y+midy, x+midx, y+uly, 
				          fulltextcolor );
              drawarrow( x+midx, y+uly-firstbox->height, 
                         x+midx, y-height, fulltextcolor );
            }

        // finalize the width
        leftmargin += width + hpad;
        width = leftmargin;
 
      } // branch box

      // group loop boxes together due to some similarities:
      //    vertical list of horizontally centered inner boxes,
      //    no name, no symbol0, some arrows
      else if( kind==dobox || kind==whilebox || kind==forbox )
      {
        // first determine the width for centering        
          maxwidth = 0;
          curbox = first;
          while( curbox != NULL )
          {
            if(curbox->width > maxwidth)
              maxwidth = curbox->width;
            curbox = curbox->next;
          }

        // can now set the width of me, allowing extra margin
        // space on both sides:
        leftmargin = 6*hpad;
        width = leftmargin + maxwidth + leftmargin;

        // midx is the center x value 
        midx = leftmargin + maxwidth/2;

        // maintain height as we draw downward
        height = 0; 

        uly = 0;  // start at upper edge;

        // draw the inner boxes
        curbox = first;     innercount = 0;
        while( curbox != NULL )
        {
          // draw arrow from (midx, uly) down distance arrowpad
          drawarrow( x + midx, y + uly, 
                         x + midx, y + uly - arrowpad,
						 fulltextcolor );

          height += arrowpad;

          // adjust uly downward
          uly -= arrowpad;

          // set ulx correctly:
          ulx = midx - curbox->width/2;

          // draw the box
          // recursive call to this guy, not finding or locating
          if(draw)
            curbox->display( x + ulx, y+uly,
                             left, right, top, bottom,
                             showfocus, focusbox );           

             // depending on kind, note (retx,rety) 
             // and (outx,outy) info
             innercount++;  // have just drawn another
             if( (innercount==1 && (kind==dobox || kind==whilebox))
                  ||
                 (innercount==2 && kind==forbox )
                )
             {// note where to draw return arrow
               retx = ulx;
               rety = uly - 0.5*curbox->height;
             }

             if( (innercount==2 && (kind==dobox || kind==forbox) )
                 ||
                 (innercount==1 && kind==whilebox)
               )
             {// note where to draw out arrow
               outx = midx + 0.5*curbox->width;
               outy = uly - 0.5*curbox->height;
             }

          // update height, width
          height += curbox->height;         
          
          // locate curbox?
          if(locate)
          {
            curbox->ulcx = ulx;  // offsets in outer
            curbox->ulcy = uly;
          }

          // find?
          if(insideRect(find,
               ulx, uly, 
               ulx+curbox->width, uly-curbox->height,
               targetx, targety, innerx, innery ))
          {
            partnumber = 'I';
            innerbox = curbox;
          }

          // move to next box pointerwise and geometrically
          uly -= curbox->height;
          curbox = curbox->next;

        }

        // draw path from bottom of last guy back up to
        // noted (retx,rety) point

        // down a ways,
        ulx = midx;
        drawline( x+ulx, y+uly, x+ulx, y+uly-arrowpad,
			         fulltextcolor );
        uly -= arrowpad;

        // straight left halfway into open space on left
        drawline( x+ulx, y+uly, x+0.5*leftmargin, y+uly,
			         fulltextcolor );
        ulx = 0.5*leftmargin;

        // straight up
        drawline( x+ulx, y+uly, x+ulx, y+rety, fulltextcolor );
        uly = rety;

        // straight sideways
        drawarrow( x+ulx, y+uly, 
                       x+retx, y+uly, fulltextcolor );

        height += arrowpad + 2*vpad;

        // draw arrow out from (outx,outy) straight right to border
        drawarrow( x+outx, y+outy, x+width, y+outy,
			        fulltextcolor );

        // get apparent focus info---will just kick by for me,
        // since I can't have focus "inside"
        getAFInfo( getafinfo, NULL, 
                     0, 0, 
                     0,
                      afx, afy, afw, afh );

      } // end of loop boxes
   
      // all kinds that have exactly a name and "0 or more" style
      // horizontal "tops even" list of inners
			//  6/26/2000:  new version will have just the methods boxes
			//               here, data boxes below to do wrapping
      else if( kind==classmethodsbox || kind==instancemethodsbox )	
      {
        // handle name
        name->howBig(0,w1,h1);

        // 6/27/2000:  changed newlines==0 to 1 to suppress
				//     rectangle space image --- similarly in others
				//     with hard-coded names involving spaces
        if(draw)
          name->display( x+hpad, y-vpad, 1, 
                     dofocus && inside=='y' && (part=='n'), 
					 fulltextcolor );

        width = hpad + w1; 
        height = 2*vpad + h1;  // height of name row
        
        if(insideRect( find,
                        0, 0, 2*hpad+w1, -(2*vpad+h1),
                      targetx,targety,
                      innerx, innery ))
          partnumber = 'n';

        // draw the symbol0 for inner list
        ulx = hpad; uly = -vpad-h1-vpad;

        if(draw)
        {
          if(dofocus && inside=='y' && (part=='i'))
            drawsymbol0(x+ulx,y+uly,2,w2,h2);
          else
            drawsymbol0(x+ulx,y+uly,1,w2,h2);
        }
        else
          drawsymbol0(x+ulx,y+uly,0,w2,h2); // just obtain size

        // symbol0 might be wider than the name
        width = mymax( width, hpad + w2);
        
        // determine whether on part 'i' --- symbol0 for inner list       
        if(insideRect( find,
            0, -(2*vpad+h1), hpad+w2, -(3*vpad+h1+h2),
            targetx, targety, innerx, innery ))
           partnumber = 'i';

        ulx += w2 + hpad;  // adjust to the right on same y value
        maxheight = h2;  // monitor highest box in first row
                         // without vpadding
        leftmargin = ulx;

        // draw all the inner boxes, with tops on same level
        curbox = first;
        while( curbox != NULL )
        {// curbox is the box to be displayed at current spot
        
          // draw the box
          // recursive call to this guy, not finding or locating
          if(draw)
            curbox->display( x+ulx, y+uly, 
                             left, right, top, bottom,
                             showfocus, focusbox );           

          // update maxheight, width
          maxheight = mymax( maxheight, curbox->height );
          width = mymax( width, ulx + curbox->width );

          // locate curbox?
          if(locate)
          {
            curbox->ulcx = ulx;
            curbox->ulcy = uly;
          }

          // target inside this inner box?
          if(insideRect( find,
               ulx, uly, 
               ulx+curbox->width, uly-curbox->height,
               targetx, targety, innerx, innery ))
          {// found it!
            partnumber = 'I';
            innerbox = curbox;
          }

          // move to the right pointerwise and geometrically
          ulx += curbox->width + hpad;
          /* seems wrong (done above), so took it out 
             here and in stackbox
             if(maxheight < curbox->height)
               maxheight = curbox->height;
          */
          curbox = curbox->next;

        }

        // add row of inner boxes to total height
        height += maxheight + vpad;

        // update width with final hpad
        width += hpad;
        
        // only now that we know the width can we return 
        // accurate afinfo
        if(getafinfo)
        {
          if(part=='n')
            getAFInfo( getafinfo, name,
                  hpad, -vpad,
                  0,
                  afx, afy, afw, afh );
          else if(part=='i')       
            getAFInfo( getafinfo, NULL,
                  hpad, -vpad-h1-vpad,
                  0,
                  afx, afy, afw, afh );
          else
            getAFInfo( getafinfo, NULL,
                  0, 0,
                  0,
                  afx, afy, afw, afh );
        }

      }

      // kinds that have wrapped inners with fixed number on a line,
			// tops even
      else if( kind==classdatabox || kind==instancedatabox || 
			         kind==paramsbox || kind==localsbox 
							 //v10
							 || kind==classdatacopybox
						 )
      {
        // handle name
        name->howBig(0,w1,h1);

        if(draw)
          name->display( x+hpad, y-vpad, 1, 
                     dofocus && inside=='y' && (part=='n'), 
					 fulltextcolor );

        width = hpad + w1; 
        height = 2*vpad + h1;  // height of name row
        
        if(insideRect( find,
                        0, 0, 2*hpad+w1, -(2*vpad+h1),
                      targetx,targety,
                      innerx, innery ))
          partnumber = 'n';

        // draw the symbol0 for inner list
        ulx = hpad; uly = -vpad-h1-vpad;

        if(draw)
        {
          if(dofocus && inside=='y' && (part=='i'))
            drawsymbol0(x+ulx,y+uly,2,w2,h2);
          else
            drawsymbol0(x+ulx,y+uly,1,w2,h2);
        }
        else
          drawsymbol0(x+ulx,y+uly,0,w2,h2); // just obtain size

        // symbol0 might be wider than the name
        width = mymax( width, hpad + w2);
        
        // determine whether on part 'i' --- symbol0 for inner list       
        if(insideRect( find,
            0, -(2*vpad+h1), hpad+w2, -(3*vpad+h1+h2),
            targetx, targety, innerx, innery ))
           partnumber = 'i';

        ulx += w2 + hpad;  // adjust to the right on same y value
        maxheight = h2;  // monitor highest box in first row
                         // without vpadding
        leftmargin = ulx;

        // draw all the inner boxes, with tops on same level,
				// starting new row when hit limit

				int maxInRow = 4, numInRow = 0;

        curbox = first;
        while( curbox != NULL )
        {// curbox is the box to be displayed at current spot
        
          // draw the box
          // recursive call to this guy, not finding or locating
          if(draw)
            curbox->display( x+ulx, y+uly, 
                             left, right, top, bottom,
                             showfocus, focusbox );           

          // update maxheight, width
          maxheight = mymax( maxheight, curbox->height );
          width = mymax( width, ulx + curbox->width );

          // locate curbox?
          if(locate)
          {
            curbox->ulcx = ulx;
            curbox->ulcy = uly;
          }

          // target inside this inner box?
          if(insideRect( find,
               ulx, uly, 
               ulx+curbox->width, uly-curbox->height,
               targetx, targety, innerx, innery ))
          {// found it!
            partnumber = 'I';
            innerbox = curbox;
          }

          // move to the right pointerwise and geometrically
          ulx += curbox->width + hpad;
          curbox = curbox->next;

					// see if should start new row
					numInRow++;  // just did an inner box

					if( numInRow == maxInRow && curbox != NULL )
					{// finish current row---there is a next row
					  height += maxheight + vpad;  // the row is done
            width = mymax( width, ulx ); // update width for row
            ulx = leftmargin;  // move to beginning of next row
					  uly -= maxheight + vpad;

						numInRow = 0;
					}

        }

        // add row of inner boxes to total height
        height += maxheight + vpad;

        // update width with final hpad
        width += hpad;
        
        // only now that we know the width can we return 
        // accurate afinfo
        if(getafinfo)
        {
          if(part=='n')
            getAFInfo( getafinfo, name,
                  hpad, -vpad,
                  0,
                  afx, afy, afw, afh );
          else if(part=='i')       
            getAFInfo( getafinfo, NULL,
                  hpad, -vpad-h1-vpad,
                  0,
                  afx, afy, afw, afh );
          else
            getAFInfo( getafinfo, NULL,
                  0, 0,
                  0,
                  afx, afy, afw, afh );
        }

      }// static boxes that hold wrapped sequences of data boxes

      // all kinds that have exactly a name and variable
      // size without symbol0
      // horizontal "tops even" list of inners
			          // v10:  added allclassdatabox display here
      else if( kind==stackbox || kind==heapbox || kind==allclassdatabox
             )
      {
        // handle name
        name->howBig(0,w1,h1);

        if(draw)
          name->display( x+hpad, y-vpad, 1, 
                     dofocus && inside=='y' && (part=='n'), 
					 fulltextcolor );

        width = hpad + w1; 
        height = 2*vpad + h1;  // height of name row
        
        if(insideRect( find,
                        0, 0, 2*hpad+w1, -(2*vpad+h1),
                      targetx,targety,
                      innerx, innery ))
          partnumber = 'n';

        ulx = hpad; uly = -vpad-h1-vpad;

        maxheight = 0;  // monitor highest box in first row
                         // without vpadding
        
        // draw all the inner boxes, with tops on same level
        curbox = first;
        while( curbox != NULL )
        {// curbox is the box to be displayed at current spot
        
          // draw the box
          // recursive call to this guy, not finding or locating
          if(draw)
            curbox->display( x+ulx, y+uly, 
                             left, right, top, bottom,
                             showfocus, focusbox );           

          // locate curbox?
          if(locate)
          {
            curbox->ulcx = ulx;
            curbox->ulcy = uly;
          }

          // target inside this inner box?
          if(insideRect( find,
               ulx, uly, 
               ulx+curbox->width, uly-curbox->height,
               targetx, targety, innerx, innery ))
          {// found it!
            partnumber = 'I';
            innerbox = curbox;
          }

          // move to the right pointerwise and geometrically
          // update maxheight
          maxheight = mymax( maxheight, curbox->height );
          ulx += curbox->width + hpad;
          curbox = curbox->next;

        }

        // add row of inner boxes to total height
        height += maxheight + vpad;

        // width is width of name part or farthest right inner
        width = mymax( width+hpad, ulx );

        // only now that we know the width can we return 
        // accurate afinfo
        if(getafinfo)
        {
          if(part=='n')
            getAFInfo( getafinfo, name,
                  hpad, -vpad,
                  0,
                  afx, afy, afw, afh );
                 
          else if(part=='i')       
            getAFInfo( getafinfo, NULL,
                  hpad, -vpad-h1-vpad,
                  0,
                  afx, afy, afw, afh );
          else
            getAFInfo( getafinfo, NULL,
                  0, 0,
                  0,
                  afx, afy, afw, afh );
        }

      }// name, variable inner with no symbol0

      else
        errorexit("illegal kind in box::display, full display");

    }// branch on kind for full display
    else
      errorexit("illegal displaymode in box::sizeLocDisplay");

    //-----------------------------------------------------------

    // now that we have correct width and height, draw the
    // background rectangle, 
	// and maybe decide that the target is off me

    if(draw)
    {
      // set boundary color depending on aspect
      if( !texdraw )
      {// draw to screen
        if( aspect == 'a' )
		    	glColor3fv( abstextcolor );
        else if( aspect == 'd' )
    			glColor3fv( doctextcolor );
        else if( aspect == 'f' )
			    glColor3fv( fulltextcolor );
        else if( aspect == 'v' )
    			glColor3fv( valuetextcolor );
  	  	else
	  	  	errorexit("unknown text color in drawing boundary");

        // draw my boundary
        glBegin(GL_LINE_LOOP);
          glVertex3f( x, y, textz );
          glVertex3f( x, y-height, textz );
          glVertex3f( x+width, y-height, textz );
          glVertex3f( x+width, y, textz );
        glEnd();

        // jb24:  if authoring and asked to draw, draw extra corner
        //        decorations to show permissions, using same color
        //        as the boundary
        if( lesson->isAuthor() )
        {// urc is kill, lrc is add, llc is view change
          glBegin( GL_TRIANGLES );
            if( !canKill() )
            {// mark urc
              glVertex3f( x+width-1.5*hpad, y, textz );
              glVertex3f( x+width, y-1.5*vpad, textz );
              glVertex3f( x+width, y, textz );
            }
            if( !canAdd() )
            {// mark lrc
              glVertex3f( x+width-1.5*hpad, y-height, textz );
              glVertex3f( x+width, y-height+1.5*vpad, textz );
              glVertex3f( x+width, y-height, textz );
            }
            if( !canChange() )
            {// mark llc
              glVertex3f( x, y-height+1.5*vpad, textz );
              glVertex3f( x+1.5*hpad, y-height, textz );
              glVertex3f( x, y-height, textz );
            }
          glEnd();
        }// extra marks showing permissions


      }// not doing texdraw, doing screen draw
      else
      {// draw boundary to texfile

        if( pictex )
				{
				  pictexRect( x,  y-height, x+width, y, 'R' );
				}
				else
				{
          fprintf( texfile, "newpath\n" );
          fprintf( texfile, "%g %g moveto\n", x, y );
          fprintf( texfile, "%g %g lineto\n", x, y-height );
          fprintf( texfile, "%g %g lineto\n", x+width, y-height );
          fprintf( texfile, "%g %g lineto\n", x+width, y );
          fprintf( texfile, "closepath stroke\n" );
				}

        // jb24:  chose to not draw permission marks to
        //        texfile---reader and book not to be made aware of
        //        permissions except when try to violate

      }

    }// end of possibly not drawing at all

    // note:  this is compromised---eventually, will just draw
	//        the background based on dye and color, 
	//        dofocus will be irrelevant!

    if(draw)
    {// focus, colors irrelevant if not drawing
      // see if I should draw as focus
      // set correct boundary color 
      if( !texdraw )
      {// draw to screen

        if( dofocus && inside=='n' && !touring )
        {
          glColor3fv( focuscolor ); 
        }
        else
          setGLColor();

        // draw my background

    		float boxz = zValue();
		
        glBegin(GL_POLYGON);
          glVertex3f( x, y, boxz );
          glVertex3f( x, y-height, boxz );
          glVertex3f( x+width, y-height, boxz );
          glVertex3f( x+width, y, boxz );
        glEnd();

				// jb18:  draw extra magenta border if is a breakpoint			
				glColor3f( 1, 0, 1 );
				if( isBreakpoint() )
				{
          glBegin(GL_LINE_LOOP);
            glVertex3f( x, y, breakz );
            glVertex3f( x, y-height, breakz );
            glVertex3f( x+width, y-height, breakz );
            glVertex3f( x+width, y, breakz );
          glEnd();
					float smidge = 1.0;
          glBegin(GL_LINE_LOOP);
            glVertex3f( x+smidge, y-smidge, breakz );
            glVertex3f( x+smidge, y-height+smidge, breakz );
            glVertex3f( x+width-smidge, y-height+smidge, breakz );
            glVertex3f( x+width-smidge, y-smidge, breakz );
          glEnd();
				}

      }
      else
      {// draw boundary to texfile

			  if( pictex )
				{
				  pictexRect( x, y-height, x+width, y, 'R' );
				}
				else
				{
          fprintf( texfile, "newpath\n" );
          fprintf( texfile, "%g %g moveto\n", x, y );
          fprintf( texfile, "%g %g lineto\n", x, y-height );
          fprintf( texfile, "%g %g lineto\n", x+width, y-height );
          fprintf( texfile, "%g %g lineto\n", x+width, y );
          fprintf( texfile, "closepath stroke\n" );
				}
      }

    }// end of possibly not doing all this display stuff

    // see if target is not on me at all (if partnumber is still
    // '?', means we haven't located target over any parts or inners,
    //  so is either just on me around the edges of the
    // inner stuff, or is actually off me entirely!
    if(find && partnumber=='?') 
    {
      if(0<= targetx && targetx <= width &&
         -height <= targety && targety <= 0 )
      {// is on me, not on any interior part or box
         partnumber = '.';
      }
      // else stays '?' indicating target was not on me at all
    }

    // report whether changed size
    if(oldwidth==width && oldheight==height)
      changed = 0;
    else
      changed = 1;

    // // traceLeave();
    return innerbox;  

  }// end of sizeLocDisplay method


  // just for convenience, define auxiliary methods that
  // simply call the main tool correctly

  void box::figureSizeLocs( int& modified )
  {// all is neutral except modified
	  // traceEnter( tid, "box:figureSizelocs" );

    box* tempbox;
    char pn;
    float ix, iy, f1, f2, f3, f4;

    tempbox = sizeLocDisplay( 1, modified,
    0, 
    0.0, 0.0,    // x,y
    0.0, 0.0, 0.0, 0.0,    // left, right, top, bottom
    0, NULL,
    0, 0.0, 0.0,
    pn, ix, iy,
    0, f1, f2, f3, f4 );

		// // traceLeave();
  }

  void box::display( float ulcx, float ulcy, 
                 float left, float right, float top, float bottom,
                 int showfocus, box* focusbox )
  {
	  // traceEnter( tid, "box:display" );
    box* tempbox;
    int chnged;
    char pn;
    float ix, iy, f1, f2, f3, f4;

    tempbox = sizeLocDisplay( 0, chnged,
         1, ulcx, ulcy, 
            left, right, top, bottom,
            showfocus, focusbox,
          0, 0.0, 0.0,
          pn, ix, iy,
          0, f1, f2, f3, f4 );
		// // traceLeave();
  }

  // these parameters are not used, pass along to avoid 
  // irritation of declaring substitutes locally
  void box::texdisplay()
  {
    // traceEnter( tid, "box:texdisplay" );
    box* tempbox;
    int chnged;
    char pn;
    float ix, iy, f1, f2, f3, f4;

    // only want to draw to tex file, 
    // ulcx, ulcy, left, right, top, bottom are only possibly
    // interesting guys, 

    float ulcx, ulcy;
//    ulcx = 72.00; ulcy = 720.0;  // upper left corner of page,
                                 // indented 1 inch from left

      ulcx = 0.0;  ulcy = height;

    // 
    tempbox = sizeLocDisplay( 0, chnged,
         1, ulcx, ulcy, 
            0.0, 72*8, ulcy, 0.0,  
            0, NULL,
          0, 0.0, 0.0,
          pn, ix, iy,
          0, f1, f2, f3, f4 );

		// // traceLeave();
  }

// 8/9/2000:  special portsDisplay to show it no matter what
// jb6a:  a very "special" method, if you know what I mean!
//   needs changing to not screw up the portsbox 
//  replace width, height by locals tempwidth, tempheight
  void box::portsDisplay( float x, float y )
  {
    float infinity = 1e30;

    float w1, h1;  // utility variables
    float ulx, uly; // maintain  standard coords
              // over the box, of guy currently being drawn           
              //  --- not offsets, but actual coords
    float leftmargin;
 
    box* curbox;  // utility box pointer

    // utilities for displaying various code boxes                           
    float dx = 0.5*hpad;

    glColor3fv( fulltextcolor );

    // handle name
    name->howBig(0,w1,h1);
      
    name->display( x+hpad, y-vpad, 0, 
                     0, fulltextcolor );

    // initialize the width to the least it can be across
    // the top row

    // jb6a:
    float tempwidth, tempheight;

    tempwidth = hpad + w1; 
    tempheight = 2*vpad + h1;  // height of first "row"

    ulx = hpad;  // adjust to the right on same y value
    leftmargin = ulx;
    uly = -tempheight;

    // draw the inner boxes
    curbox = first;
    while( curbox != NULL )
    {// curbox is to be displayed at current spot
     // draw the box
     // recursive call to this guy, not finding or locating
      curbox->display( x+ulx, y+uly,
                             -infinity, infinity, infinity, -infinity,
                             0, NULL );           

      // update height, width
      tempheight += curbox->height + vpad;         
      tempwidth = mymax( tempwidth, ulx + curbox->width );

      // move to next box pointerwise and geometrically
      uly -= curbox->height + vpad;
      curbox = curbox->next;

    }

    // have found width as farthest extent, add right hpad
    tempwidth += hpad;

    // now that we have correct width and height, draw the
    // background rectangle, 
  	// and maybe decide that the target is off me

    // set boundary color depending on aspect
  	glColor3fv( fulltextcolor );
  
    // draw my boundary
    glBegin(GL_LINE_LOOP);
      glVertex3f( x, y, textz );
      glVertex3f( x, y-tempheight, textz );
      glVertex3f( x+tempwidth, y-tempheight, textz );
      glVertex3f( x+tempwidth, y, textz );
    glEnd();

    setGLColor();

    // draw my background

		float boxz = zValue();
		
    glBegin(GL_POLYGON);
      glVertex3f( x, y, boxz );
      glVertex3f( x, y-tempheight, boxz );
      glVertex3f( x+tempwidth, y-tempheight, boxz );
      glVertex3f( x+tempwidth, y, boxz );
    glEnd();

    // // traceLeave();

  }// end of portsDisplay method
