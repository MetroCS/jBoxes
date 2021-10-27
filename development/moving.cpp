// moving.cpp:

// only allow moving in intmode and paumode, behave exactly the same
// in both, except for which viewer is being used,
// and need to set touring to 0 if paused

  if( actionmode == interactivemode )
    activeviewer = interactiveviewer;
  else
    activeviewer = executionviewer;

  if( actionmode == pausedmode )
    touring = 0;

  focus = v[activeviewer].whoFocus();

  if( mqKind == MQ_ALT_KEY )
	{
    needredraw = 0;
  }

  else if( mqKind == MQ_SPEC_KEY )
  {// there is a special key, and the mouse is not busy

    if( mqCode == GLUT_KEY_END )
      {// scroll viewer offset by half a screen to the left
        v[activeviewer].shift( -0.5, 0.0 );
        needredraw = 1;
        needrefocus = 0;
      }
    else if( mqCode == GLUT_KEY_HOME)
      {// scroll to right
        v[activeviewer].shift( 0.5, 0.0 );
        needredraw = 1;
        needrefocus = 0;
      }
    else if( mqCode == GLUT_KEY_PAGE_DOWN)
      {// scroll up
        v[activeviewer].shift( 0.0, 0.5 );
        needredraw = 1;
        needrefocus = 0;
      }
    else if( mqCode == GLUT_KEY_PAGE_UP)
      {// scroll down
        v[activeviewer].shift( 0.0, -0.5 );
        needredraw = 1;
        needrefocus = 0;
      }
    else // illegal function key
      needredraw = 0;

  }// special key was available

  else if( mqKind == MQ_REG_KEY )
  {// a regular key is available

      if( mqCode == nextkey)
      {
        focus = focus->moveNext( needredraw );
      }

      else if( mqCode == prevkey)
      {       
        focus = focus->movePrev( needredraw );
      }

      else if( mqCode == upkey)
      {
        focus = focus->moveUpDown('u',needredraw);
      }

      else if( mqCode == downkey)
      {
        focus = focus->moveUpDown('d',needredraw);
      }

      else if( mqCode == firstkey)
      {
        focus = focus->moveHomeEnd('h',needredraw);
      }

      else if( mqCode == lastkey)
      {
        focus = focus->moveHomeEnd('e',needredraw);
      }

      else if( mqCode == outkey)
      {
        focus = focus->moveOut( needredraw );
      }

      else if( mqCode == inkey)
      {
       focus = focus->moveIn( needredraw );
      }

      else if( mqCode == jumpkey)
      {
        focus = focus->jumpParts( needredraw );
      }

      else if( mqCode == enableAuthoringKey )
      {// figure out the current focus location and add
       // code to combox specifying a move( to that location
        lesson->movingToAuthoring( focus );
        needredraw = 1;                
      }

      else
        needredraw = 0;

  }// reg key

  else if( mqKind == MQ_CLICK )
  {// a mouse click is waiting to be processed
    // to change the focus
  
    int whichviewer;
    float vx, vy;

      // determine which viewer received the click and
      // produce vx,vy as location of click in that viewer
      whichviewer = locateLocationInViewer(mousex, mousey, vx, vy);

      mouseviewer = whichviewer;  // note for use by release

      if( whichviewer != interactiveviewer )
      {// no viewer was hit by the click
        needredraw = 0;  // mark the world unchanged
        mousestatus = 0;  // so won't care about the release
      }
      else
      {// hit a viewer
    
        if( mqCode == MQ_LEFT_BUTTON )
        {// process left click by making that spot the focus

          newfocus = v[whichviewer].setFocusToLocation(
                                          vx, vy, needredraw );

            focus->simulateMove( newfocus );
            focus = newfocus;
		  			needrefocus = 2; // ask for special refocus

//          mousestatus = 0; 
        }
        else if( mqCode == MQ_RIGHT_BUTTON )
        {// process right click by making that spot the focus
          // and toggling the aspect of the new focus

          newfocus = v[whichviewer].setFocusToLocation(
                         vx, vy, needredraw );

          // 10/8/2000:
          // called findLocation which set mouseoffx,y, use to
					// reposition viewer

					/* v11;  new scheme for shifting to keep mouse over box
					float prewidth, preheight, postwidth, postheight,
					       premousex, premousey, postmousex, postmousey,
								 viewshiftx, viewshifty;
					prewidth = newfocus->howWide();
					preheight = newfocus->howHigh();
					premousex = mouseoffx;  // save in case someone messes with
					premousey = mouseoffy;
					*/

					float goalx, goaly;  // goal fractions of mouse in focus
					goalx = mouseoffx / newfocus->howWide();
					goaly = mouseoffy / newfocus->howHigh();


            focus->simulateMove( newfocus );
            focus = newfocus;
            focus = focus->cycleAspects( needredraw );

					// 10/8/2000:  shift viewer using pre and post info
					int junk;
					focus->figureSizeLocs( junk ); // make sure it knows new size

					/* v11:  now that the focus has been cycled, reality has been
					//       updated, shift the viewer by the right amount so
					//       that the goal spot in the focus is under the 
					//       physical mouse location
				  postwidth = focus->howWide();
          pre-v11:  postheight = focus->howHigh();

          // postmouse same percentage of post size as premouse is
					// of presize
					postmousex = (premousex / prewidth) * postwidth;
					postmousey = (premousey / preheight) * postheight;
					// shift viewer so postmouse position is under physical mouse
					viewshiftx = premousex - postmousex;
					viewshifty = premousey - postmousey;
          v[whichviewer].translate( viewshiftx, viewshifty );	
					*/
					
					// v11:  ask for refocus 3, which will use global info 
					//       noted here to position viewer so that the 
					//       physical mouse location is above desired spot on
					//       the focus box
					         
          needrefocus = 3;				
					// note global infos (doing at the end, not worth making into
					//                     parameters)
					notemousex = vx - goalx * focus->howWide(); // using new size
					notemousey = vy - goaly * focus->howHigh();

//          mousestatus = 0; // don't care about release

        }// right button

        else
        {
          errorexit("fatal error in moving.cpp, bad click code");
        }

        // either way, make whichviewer the interactive viewer
        if(activeviewer != whichviewer )
        {// need to switch
          activeviewer = whichviewer;
          needredraw = 1;
        }
        
      }// hit a viewer
 
    }// process mouse click

    else
    {// some other inappropriate message arrived
      std::cout << "in moving.cpp, got message kind "
           << mqKind << "???" << std::endl;
      exit(1);
    }
