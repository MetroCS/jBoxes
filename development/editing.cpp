// jb2a:  manually editing a lesson box,
//  things get pretty simplified---
//  actionmode irrelevant, working in
//  lesson viewer

  focus = v[lessonviewer].whoFocus();
  touring = 0;

  if( mqKind == MQ_ALT_KEY )
	{// toss alt keys
    needredraw = 0; // alt key that does nothing
	}

  else if( mqKind == MQ_SPEC_KEY )
  {// only special keys are scrollers
      if( mqCode == GLUT_KEY_END )
      {// scroll viewer offset by half a screen to the left
        v[activeviewer].shift( -0.5, 0.0 );
        needredraw = 1;
        needrefocus = 0;
      }
      else if( mqCode==GLUT_KEY_HOME)
      {// scroll to right
        v[activeviewer].shift( 0.5, 0.0 );
        needredraw = 1;
        needrefocus = 0;
      }
      else if(mqCode==GLUT_KEY_PAGE_DOWN)
      {// scroll up
        v[activeviewer].shift( 0.0, 0.5 );
        needredraw = 1;
        needrefocus = 0;
      }
      else if(mqCode==GLUT_KEY_PAGE_UP)
      {// scroll down
        v[activeviewer].shift( 0.0, -0.5 );
        needredraw = 1;
        needrefocus = 0;
      }
      else
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

      else if( ( mqCode == backspacekey || mqCode == killkey) 
               &&
               focus->whichKind() != lessonbox 
             )
      {
        if( focus->whichKind() == notebox )
        {
          lesson->removeNote( focus );
        }

        focus = focus->removeFromInner( mqCode, needredraw );         
      }

      else if( mqCode == wipekey)
			{
			  focus = focus->wipeAspect( needredraw );
			}

      else if(  mqCode == ' ' || 
               ( mqCode == specialspacekey && 
                 focus->whichKind() != lessonbox
               ) 
             )
      {
        focus = focus->processSpace( mqCode, needredraw );
      }

      else if((' '<  mqCode && mqCode <= '~') ||
                mqCode == enterkey  || //jb16: add tab as possible input
							  mqCode == tabkey )
      {// process printables, enterkey, in various elaborate ways
        focus = focus->processPrintable(  mqCode, needredraw );
      }// printable

      else if( mqCode == jumpkey)
      {
        focus = focus->jumpParts( needredraw );
      }

      else if( mqCode == toggleviewskey ||  mqCode == deletekey)
      {
        focus = focus->cycleAspects( needredraw );
      }

      else if( mqCode == enableAuthoringKey )
      {// return to authoring mode

        // restore activeviewer to the previous value the lesson noted
        lesson->editingToAuthoring( activeviewer );

        // make the various global changes		

        v[lessonviewer].setOnScreen('n');

        if( splitviewing )
        {
          v[consoleviewer].setOnScreen('y');
          v[portsviewer].setOnScreen('y');
        }

        v[activeviewer].setOnScreen('y');

        setviewers( 0 );

        focus = v[activeviewer].whoFocus();

        needredraw = 1;

      }

      else if( mqCode == showkey)
			{
			  focus->changeAllToFull( needredraw );
			}

			else if( mqCode == tellkey)
			{
			  focus->changeAllToDoc( needredraw );
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

      if( whichviewer != lessonviewer )
      {// missed the lessonviewer
        needredraw = 0;  // mark the world unchanged
      }
      else
      {// hit lessonviewer
        // determine the focus box for the click, without changing
        //  anything yet, in case is not in the lessonbox
        
        char tpart;  // not really desired for this call          
        int tindex;  // since if is inside lessonbox, will redo
                     // using setFocusToLocation

        newfocus = v[whichviewer].findTargetBox( vx, vy, tpart, tindex );

        // determine whether the click is inside the lessonbox
        if( newfocus->getLessonbox() == lesson->getBox() )
        {// okay to edit newfocus, is inside lesson's lessonbox    

          if( mqCode == MQ_LEFT_BUTTON )
          {// process left click by making that spot the focus

            newfocus = v[whichviewer].setFocusToLocation(
                                          vx, vy, needredraw );
            focus->simulateMove( newfocus );
            focus = newfocus;
		  			needrefocus = 2; // ask for special refocus

            needredraw = 1;

          }
          else if( mqCode == MQ_RIGHT_BUTTON )
          {// process right click by making that spot the focus
           // and toggling the aspect of the new focus

            newfocus = v[whichviewer].setFocusToLocation(
                                          vx, vy, needredraw );

					  float goalx, goaly;  // goal fractions of mouse in focus
					  goalx = mouseoffx / newfocus->howWide();
  					goaly = mouseoffy / newfocus->howHigh();
 
            focus->simulateMove( newfocus );
            focus = newfocus;
            focus = focus->cycleAspects( needredraw );

  					int junk;
	  				focus->figureSizeLocs( junk ); // make sure it knows new size
								         
            needrefocus = 3;				
			  		// note global infos (doing at the end, not worth making into
				  	//                     parameters)
					  notemousex = vx - goalx * focus->howWide(); // using new size
  					notemousey = vy - goaly * focus->howHigh();
           
            needredraw = 1;

          }
          else
            errorexit("bad button in editing.cpp");

        }// box under click was inside the lesson's lessonbox
        else
        {// clicked off the lessonbox
          needredraw = 0;
        }       
   
      }// hit lessonviewer
 
    }// process mouse click

    else
    {// some other inappropriate message arrived
      std::cout << "in editing.cpp, got message kind "
           << mqKind << "???" << std::endl;
      exit(1);
    }
