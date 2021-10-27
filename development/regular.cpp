// uniform code for processing in regular way, with 
// extras if recording or reading

// jb9a:  huh?  was this some kind of debugging thing, or
//        what?  Anyway, get rid of the ugly output
if( actionmode==errormode && mqKind==MQ_REG_KEY )
{
// cout << "about to crash" << endl;
}

// up-front extra actions:

int recordIt = 0;  // is processed in regular code, ignored except
                   // when recording

int allowed;  // flag for turning off features when recording

if( lesson->getState() == disabledLessonState )
{// nothing special to do
  allowed = 1;  // everything possible is allowed
  lesson->setActionStatus( 1 );  // do the regular code
}
else if( lesson->getState() == recordingLessonState )
{// recording
  allowed = 0;   // disallow various keys
  lesson->setActionStatus( 1 );  // do the regular code
}
else if( lesson->getState() == readingLessonState )
{// reading a lesson
  lesson->processMessage( mqKind, mqCode, mq );
  needredraw = lesson->shouldRedraw();
  focus = NULL;  // so can tell below if needs setting still
}

if( lesson->getActionStatus() != 0 )
{

//********************* regular code *******************************
//********************* starts *************************************
//********************* here   *************************************

if( actionmode == interactivemode )
{// full, ordinary interactive mode 

  activeviewer = interactiveviewer;
  focus = v[activeviewer].whoFocus();

// jb22: used to do this:   #include "intmode.cpp"
//       now include physically:
//******************** intmode *************************************
//***************************************************************
//***************************************************************

  if( mqKind == MQ_ALT_KEY )
	{

	  if( mqCode == snapshotKey && TeXauthorized && allowed )
		{
			  // okay to do snapshot
        v[activeviewer].startDrawToTeX( focus );
        if( texfile != NULL )
        {// have file, have done startup, draw the box
          focus->texdisplay();
          v[activeviewer].finishDrawToTeX();
        }
  			// put up info box telling filename 
	     informOfSnapshot( focus );
		}     
    else if( mqCode == exporttojavakey && allowed )
  	{
		  needredraw = focus->exportToJava();
			if( needredraw == -1 )
        requesterrormessage(
 		    "Focus must be on a class method box named main",
							  v[interactiveviewer].whoFocus() );
			else if( needredraw == -2 )
			  requesterrormessage(
				"Could not write to file for export to Java",
				  v[interactiveviewer].whoFocus() );
			// jb18: export can fail if a javabox doesn't translate
			//       so use global variable to avoid retooling totally
			else if( needredraw == erroralreadyrequested )
			{// was translation error within process
			  // nothing to do, error was already requested
			}
    	else
	  	{
		    messagecolorcode = successcolorcode;
			  requesterrormessage(
			          "Export to Java succeeded",
        		    v[interactiveviewer].whoFocus() );
      }        				  
		}// exporttojavakey

		else
		  needredraw = 0; // alt key that does nothing

	}

  else if( mqKind == MQ_SPEC_KEY )
  {// there is a special key, and the mouse is not busy

    if ( ( mqCode == GLUT_KEY_F1 || mqCode == GLUT_KEY_F2 ) 
           
       )
    {// start execution

      // check out whether focus is legit to start execution,
      // namely a class method
      if(focus->whichKind()==methodbox)
      {// might be, for sure have owner
        box* owner = focus->whoOuter();
        if( owner->whichKind() == classmethodsbox )
        {// do all the work to start executing

          // move interactive viewer off screen
          v[interactiveviewer].setOnScreen( 'n' );
    
		      // jb18: switch to execution mode with desired pausing
					actionmode = executingmode;
          if( mqCode == GLUT_KEY_F1 )
            pausetoinspect = 1;
					else
					  pausetoinspect = 0;

          // set initial execution box for running viewer
          if( v[executionviewer].startExecution( focus ) )
					{// execution started successfully

            // June 16, 2000:  since startExecution can easily
					  //  fail, must check for success before going on

            v[executionviewer].setOnScreen('y');

            // set up viewing correctly
            if( splitviewing )
            {
              v[consoleviewer].setOnScreen('y');
              v[portsviewer].setOnScreen('y');
              setviewers(0);
            }
            else
            {// will just view the single execution viewer
              setviewers(0);
            }

            needredraw = 1;  // give it a chance to show green 
						                 // border

            smq->insert( MQ_REQUEST, MQ_START_EXEC, "start execution",
                           0, 0 );

					}// execution started successfully
  				else
					{// start up of execution failed in more subtle way
					 // error message will be given
					  // jb18: noticed there was no code here, so...
						//   further jb18 --- realized this must be blank
						//   to allow for various errors requested already!
					  //errorexit("F1/F2 execution start failed strangely?");
					}

        }
        else
        {// not good box to start on
				  actionmode = errormode; 
          requesterrormessage(
					"Focus must be on a class method box to start execution",
                  v[interactiveviewer].whoFocus() );
          needredraw = 1;
        }

      }// is methodbox

      else
      { // not good to start on
        requesterrormessage("Focus must be on a class method box to start\nexecution",
                  v[interactiveviewer].whoFocus() );
        needredraw = 1;
      }

    }// F1 and F2 (jb18 adds F2)

    // jb18
	  else if( mqCode == GLUT_KEY_F3  )
		{
			  focus->toggleBreakpoint( needredraw );
		}

    else if( mqCode == GLUT_KEY_F4  )
    {
      requesterrormessage(
        "Can't stop because not currently executing",
          v[interactiveviewer].whoFocus() );
      needredraw = 1;
    }

    else if( mqCode == GLUT_KEY_F5 && allowed )
    {// save the current universe

      // move focus all the way out to universe box
      focus = focus->setFocusToUniv( needredraw );

      // prepare the stream for writing to
      univstream = fopen( univfilename, "w" );
      // save the universe
          // first, clean up the stack and heap
          universe->clearOutStackAndHeapAndStatic();
					// and the ports
					universe->clearOutPorts();

      focus->save( univstream );
      // tidy up
      fclose( univstream );
    }

    else if( mqCode == GLUT_KEY_F6 && allowed )
    {// save and then exit

      // save the universe
        // move focus all the way out to universe box
      focus = focus->setFocusToUniv( needredraw );

      // prepare the stream for writing to
      univstream = fopen( univfilename, "w" );
      // save the universe
          // first, clean up the stack and heap
          universe->clearOutStackAndHeapAndStatic();
					universe->clearOutPorts();

       focus->save( univstream );
      // tidy up
      fclose( univstream );

      // jboxes1c:  turn off this meaningless to student output:
/*
      // report resources used
			mystring* reporter = new mystring();
      cout << "Used " << universe->numberUsed() << " boxes and " <<
                 reporter->numberUsed() << " symbols" << endl;
      //
*/

      // leave
      exit(0);

    }

    // v11:  f7 used to do snapshot, now alt-s does
    else if( mqCode == GLUT_KEY_F7  )
    {// toggle to consoleviewmode, userviewing
		  actionmode = consoleviewmode;
			userview = 1;

      v[interactiveviewer].setOnScreen('n');
			v[ consoleviewer].setOnScreen('n');
			v[ portsviewer].setOnScreen('n');

			v[ userviewer].setOnScreen('y');

      setviewers(0);

			needredraw = 1;
			needrefocus = 0;

    }// F7 toggle userviewing

    // v11: only allow split/full toggle when not just showing
		//    user view of console box.  If aren't, is just like old approach
    else if( mqCode == GLUT_KEY_F8 && !userview 
                
           ) 
    {// toggle splitviewing

      if( splitviewing )
      {// switch to full-screen interactive and execution viewer
        v[consoleviewer].setOnScreen('n');
        v[portsviewer].setOnScreen('n');
        v[interactiveviewer].setOnScreen('y');
//        v[interactiveviewer].resetSize( 
//                                screenwidth, screenheight);
//        v[executionviewer].resetSize( 
//                                screenwidth, screenheight );
        splitviewing = !splitviewing;
        setviewers( 0 );
        v[interactiveviewer].setFocus( 1, focus, NULL );
        needredraw = 1;
        needrefocus = 0; 
      }
      else
      {// switch to 3 viewers
        splitviewing = !splitviewing;
        v[consoleviewer].setOnScreen('y');
        v[portsviewer].setOnScreen('y');
        v[interactiveviewer].setOnScreen('y');
//        v[interactiveviewer].resetSize( 
//                  screenwidth - leftscreenwidth, screenheight );
        setviewers( 0 );
        v[interactiveviewer].setFocus( 1, focus, NULL );
        needredraw = 1;
        needrefocus = 0;
      }

    }// F8

    else if( mqCode == GLUT_KEY_F9 && allowed )
    {// make scale smaller

      v[activeviewer].divScale( 1.2 );
      needredraw = 1;
      needrefocus = 0; 
      // refigure backbox now that the screen is a different
      // size
      v[activeviewer].setFocus( 1, focus, NULL );
    }

    else if( mqCode == GLUT_KEY_F10 && allowed )
    {// make scale bigger
      v[activeviewer].multScale( 1.2 );
      needredraw = 1;
      needrefocus = 0; 
      // refigure backbox now that the screen is a different
      // size
      v[activeviewer].setFocus( 1, focus, NULL ); 
    }
    else if( mqCode == GLUT_KEY_END && allowed )
      {// scroll viewer offset by half a screen to the left
        v[activeviewer].shift( -0.5, 0.0 );
        needredraw = 1;
        needrefocus = 0;
      }
    else if( mqCode == GLUT_KEY_HOME && allowed )
      {// scroll to right
        v[activeviewer].shift( 0.5, 0.0 );
        needredraw = 1;
        needrefocus = 0;
      }
    else if( mqCode == GLUT_KEY_PAGE_DOWN && allowed )
      {// scroll up
        v[activeviewer].shift( 0.0, 0.5 );
        needredraw = 1;
        needrefocus = 0;
      }
    else if( mqCode == GLUT_KEY_PAGE_UP && allowed )
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

      // jb21:  enable authoring if authorized and focus is a
      //        lessonbox and authoring is disabled
      if( mqCode == enableAuthoringKey )
      {// behavior depends on lesson state 
        if( lesson->getState()==disabledLessonState &&
            lesson->isAuthor() && focus->whichKind()==lessonbox 
          )
        {// enable authoring from disabled state
          lesson->enableAuthoring( focus, &v[lessonviewer] );
          needredraw = 1;  // for the border filling
        }
        else if( lesson->getState()==recordingLessonState )
        {
          lesson->recordingToAuthoring();
          needredraw = 1;  // for the border filling
          recordIt = -1;  // don't record the key that stops recording
        }
        else
          needredraw = 0;
      }// enableAuthoring key

      // jb22: run the lesson
      // jb31:  why should they have to be authorized?  Change to
      //        just hit the escape key with the focus on a lessonbox
      else if( mqCode == escapeKey && focus->whichKind() == lessonbox )
      {// start reading, not fast forwarding
        lesson->startReading( focus, &v[lessonviewer], 0, mq );
        needredraw = 1;  // for the border filling 
      }

      else if( mqCode == nextkey)
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

      else if( mqCode == backspacekey ||  mqCode == killkey)
      {
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

      // jb23: 
		  else if(  mqCode == translatekey && focus->whichKind() == javabox )
		  {
		    needredraw = tran->translate( focus );
		  	tran->processStatusCode( needredraw );
		  }

      // jb23:  install interactive ability to load and save
      //        individual class boxes:

      else if(  mqCode == exportkey && focus->whichKind() == classbox &&
               lesson->isAuthor()
        )
      {// export the focus to a file with its name
  		  needredraw = focus->exportToFile();
  			if( needredraw == -1 )
          requesterrormessage(
 		      "File could not be opened to export to", focus );
        else if( needredraw == 1 )
        {
		      messagecolorcode = successcolorcode;
			    requesterrormessage(
			          "Class box has been saved to a file", focus );
        }
      }

      else if(  mqCode == buildkey && focus->whichKind() == classbox &&
               lesson->isAuthor()
        )
      {// build a classbox at the focus from a file with current name
  		  needredraw = focus->buildFromFile();
        if( needredraw == -1 )
          requesterrormessage(
 		      "File could not be opened", focus );
        else if( needredraw == 1 )
        {// no need to do a dialog box, will be obvious it's loaded!
        }
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

      else if( mqCode == xeroxkey)
      {// this guy works a little differently from
        // others --- returns new clipbox value, not focus
        // and focus is unchanged, anyway
        clipbox = focus->xerox( clipbox, needredraw );
        needredraw = 0;
      }

      else if( mqCode == gluekey)
      {
      focus = focus->glue( clipbox, needredraw );
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

    if( allowed )
    {// mouse click allowed
 
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

        }
        else if( mqCode == MQ_RIGHT_BUTTON )
        {// process right click by making that spot the focus
          // and toggling the aspect of the new focus

          newfocus = v[whichviewer].setFocusToLocation(
                         vx, vy, needredraw );

          // 10/8/2000:
          // called findLocation which set mouseoffx,y, use to
					// reposition viewer

					float goalx, goaly;  // goal fractions of mouse in focus
					goalx = mouseoffx / newfocus->howWide();
					goaly = mouseoffy / newfocus->howHigh();


          focus->simulateMove( newfocus );
          focus = newfocus;
          focus = focus->cycleAspects( needredraw );

					// 10/8/2000:  shift viewer using pre and post info
					int junk;
					focus->figureSizeLocs( junk ); // make sure it knows new size
				
					// v11:  ask for refocus 3, which will use global info 
					//       noted here to position viewer so that the 
					//       physical mouse location is above desired spot on
					//       the focus box				         
          needrefocus = 3;				
					// note global infos (doing at the end, not worth making into
					//                     parameters)
					notemousex = vx - goalx * focus->howWide(); // using new size
					notemousey = vy - goaly * focus->howHigh();

        }// right button

        else
        {
          errorexit("fatal error in disabled.cpp, bad click code");
        }

        // either way, make whichviewer the interactive viewer
        if(activeviewer != whichviewer )
        {// need to switch
          activeviewer = whichviewer;
          needredraw = 1;
        }
        
      }// hit a viewer

    }// click allowed

  }// process mouse click

  else
  {// some other inappropriate message arrived
    std::cout << 
      "in interactive disabled mode, got message kind "
         << mqKind << "???" << std::endl;
    exit(1);
  }

//************* end of interactive mode inclusion *****************
//***************************************************************
//***************************************************************

}// end of interactivemode

  // v11:  can only do userviewing as brief detour from interactive,
	//       or while executing or inputing
else if( actionmode == consoleviewmode ||
	         actionmode == pausedconsoleviewmode )
{// a very small mode, manage interactive<-->userviewing
	  // and paused<-->userviewing
	  // can only hit f7 to go back to interactive or paused
// jb22:  used to be :  #include "consoleviewmode.cpp"
//***************** consoleview mode *******************************
//***************************************************************
//***************************************************************

// handle the very few keys that are allowed in consoleviewmode
//  namely f7 --- toss everything else to avoid using later

  if( mqKind == MQ_SPEC_KEY )
  {// there is a special key

    // v11:  f7 used to do snapshot, now alt-s does
    if( mqCode == GLUT_KEY_F7 )
    {// switch back to interactive or paused mode and regular viewing
  		userview = 0;

      if( !splitviewing )
      {
        v[consoleviewer].setOnScreen('n');
        v[portsviewer].setOnScreen('n');
      }
      else
      {// switch to split screen viewers
        v[consoleviewer].setOnScreen('y');
        v[portsviewer].setOnScreen('y');
      }

      // switch back to main viewer
			if( actionmode == consoleviewmode )
			{// switch back to interactive mode
        v[interactiveviewer].setOnScreen('y');
        setviewers( 0 );
        focus = v[interactiveviewer].whoFocus(); 
        v[interactiveviewer].setFocus( 1, focus, NULL );
      }
			else if( actionmode == pausedconsoleviewmode )
			{// switch back to paused mode
        v[executionviewer].setOnScreen('y');
        setviewers( 0 );
        focus = v[executionviewer].whoFocus(); 
        v[executionviewer].setFocus( 1, focus, NULL );
			}
			else
  		  errorexit( "illegal actionmode in consoleviewmode.cpp" );

      if( actionmode == consoleviewmode )
		    actionmode = interactivemode;
			else if( actionmode == pausedconsoleviewmode )
			  actionmode = pausedmode;
			else
			  errorexit( "illegal actionmode in consoleviewmode.cpp" );

      needredraw = 1;
			needrefocus = 0;

    }// F7 toggle userviewing
  
	  else if( mqCode == GLUT_KEY_F1 )
	  {// maybe start up execution with userviewing, or continue
	   // from a pause

      userview = 0;  // v11:  so if do error in any way, will be okay

	    if( actionmode == consoleviewmode )
		  {// equivalent to starting from interactive mode, but stay with
  		  // userviewing
       // check out whether focus is legit to start execution,
       // namely a class method

			 focus = v[interactiveviewer].whoFocus();

        if(focus->whichKind()==methodbox)
        {// might be, for sure have owner
          box* owner = focus->whoOuter();
          if( owner->whichKind() == classmethodsbox )
          {// do all the work to start executing

            // switch to executing
            actionmode = executingmode;

            // set initial execution box for running viewer
            if( v[executionviewer].startExecution( focus ) )
					  {// execution started successfully
              userview = 1; 

              smq->insert( MQ_REQUEST, MQ_START_EXEC, 
                "start exec. in consoleview mode", 0 , 0 );
  					}// execution started successfully
    				else
		  			{// start up of execution failed in more subtle way
			  		 // error message will be given
				  	}

          }
          else
          {// not good box to start on
            requesterrormessage("Focus must be on a class method box to start\nexecution",
                    v[interactiveviewer].whoFocus() );
            needredraw = 1;
          }

        }// is methodbox

        else
        { // not good to start on
          requesterrormessage("Focus must be on a class method box to start\nexecution",
                  v[interactiveviewer].whoFocus() );
          needredraw = 1;
        }

  		}// f1 while in consoleviewmode
	
	  	else if( actionmode == pausedconsoleviewmode )
		  {// kick off execution at full speed by switching to
			 // executionmode, and so on.
			  actionmode = executingmode;
				userview = 1;
				needredraw = 1;
				needrefocus = 0;
  
        smq->insert( MQ_REQUEST, MQ_START_EXEC, 
                "start exec. in pausedconsoleview mode", 0 , 0 );
		  }
  	  else
		    errorexit("bad f1 in consoleviewmode");

  	}// F1

    else // illegal function key
      needredraw = 0;

  }// special key was available

  else if( mqKind == MQ_REG_KEY )
  {// a regular key is available

    needredraw = 0;  // ignore the reg key

  }// reg key 
  else if( mqKind == MQ_ALT_KEY )
	{// ignore   
    needredraw = 0;
	}

  else if( mqKind == MQ_CLICK )
  {// a mouse click is waiting to be processed
    // to change the focus
    // ignore
    needredraw = 0;
  }// process mouse click

  else
  {// some other inappropriate message arrived
      std::cout << "in console mode, got message kind "
           << mqKind << "???" << std::endl;
      exit(1);
  }
 
//***************************************************************
//***************************************************************
//********* end of console view mode inclusion *********************

	}// actionmode == consoleviewmode

  else if( actionmode == executingmode )
  {
    // jb1a:  handle all message types
    
    if( mqKind == MQ_REG_KEY || mqKind == MQ_ALT_KEY ||
        mqKind == MQ_CLICK )
    {// ignore these while executing
      needredraw = 0;
    }
    else if( mqKind == MQ_SPEC_KEY )
    {// handle special keys (F4 is only one)

      if( mqCode == GLUT_KEY_F4 )
      {// okay, go ahead and halt
        v[executionviewer].setOnScreen('n');
        // return to interactive viewer
        actionmode = interactivemode;
        activeviewer = interactiveviewer;
        v[activeviewer].setOnScreen( 'y' );
        setviewers(0);
        focus = v[activeviewer].whoFocus(); // for redraw below

        // v11:
	  		if( userview )
		  	{
			    userview = 0;
				  v[userviewer].setOnScreen('n');
			  }

        touring = 0;
  	  	// for safety, close files (ports are left looking attached,
	 		  //  but they are cleared when execution starts)
	    	ports->closeAll();

        needredraw = 1;

        // jb1a:  F4 is interrupting execution, might have
        //        requests to toss
        smq->removeAll();

		  }// F4 
      else
      {// ignore others
        needredraw = 0;
      }
     
    }// spec message

    else if( mqKind == MQ_REQUEST )
    {// do a step of execution, decide whether or not
      // to pause, or what all
		  // jb18:  now pausetoinspect has to be considered, too
    
      v[executionviewer].step();  // do the step

      // now, decide what to do next
      if( actionmode==errormode )
      {// an error was detected in the step,
       // requesterrormessage was called
       // 

        focus = errreturn;
        
	  		touring = 0;

		  	ports->closeAll();  // for safety --- error halt

      } 

      else if( actionmode==haltmode )
      {// the step just signaled out its desire to halt 
       // execution, so do it

       // transition from executionviewer to interactive viewer,
       // so must make sure that interactive is using correct
       // screen space
        v[executionviewer].setOnScreen('n');
        // return to interactive viewer
        actionmode = interactivemode;
        activeviewer = interactiveviewer;
        v[activeviewer].setOnScreen( 'y' );
        setviewers(0);

        // v11:
	  		if( userview )
        {
			    userview = 0;
  				v[userviewer].setOnScreen('n');
        }

        focus = v[activeviewer].whoFocus(); // for redraw below
        needredraw = 1;

		  	touring = 0;

			  ports->closeAll();  // for safety

        // must specially ask for non-error color
        messagecolorcode = successcolorcode;
		  	requesterrormessage("Execution has halted.",focus);

      } 

      else if( actionmode==input1mode || actionmode==inputlinemode )
      {// last step executed a bif that switched into input mode,
		   // force a redraw so can see the new border color
  		  needrefocus = 0;
	  		needredraw = 1;

      }

      else
      {// no error or normal halt or F4 halt
       // check info returned to see if should pause or go on
   		 //  (all this following a step of execution)

        if( !userview )
        {// normal execution, maybe pause
          if( insplist.howMany() > 0 
	  			       &&                     // jb18
		  				pausetoinspect
				    )
          {// something inspectable happened, pause
            // jb1a:  no message insertion needed---pause
            //   waits for input event
      
            actionmode = pausedmode;  // paused
  			    touring = 1;  //  start out touring, so no focus needed

            activeviewer = executionviewer;
            v[activeviewer].setOnScreen('y');

            // make the last (possibly only) box in the tour be
		  	  	// the focus of the activeviewer
            v[activeviewer].setFocus( 1, insplist.current(), NULL );
        
            focus = v[activeviewer].whoFocus();
  
            needredraw = 1;
            needrefocus = 0;
          }
          else
          {// just keep executing?
				    needredraw = 0;  // jb18: this cuts out all redisplay
					                 // when doing F2 work
            // jb1a:  go ahead and request another step
            smq->insert( MQ_REQUEST, MQ_STEP, "step (no pause)", 0, 0 );
          } 
        }// normal viewing of execution
			  else
        {// user viewing, so no pause
          // jb1a:  go ahead and request another step
          smq->insert( MQ_REQUEST, MQ_STEP, "step (not pausing)", 0, 0 );
        }

      }// no error nor halt in step

    }// process request

    else
    {// some other inappropriate message arrived
      std::cout << "in executing mode, got message kind "
           << mqKind << "???" << std::endl;
      exit(1);
    }// bad message

  }// executingmode

  else if( actionmode == pausedmode )
  {
    activeviewer = executionviewer;
    focus = v[activeviewer].whoFocus();

// jb22:  used to be this:    #include "paumode.cpp"
//******************** paused mode ******************************
//***************************************************************
//***************************************************************

  // v11:  look at alt keys first for special actions
  if( mqKind == MQ_ALT_KEY )
	{

		if( mqCode == snapshotKey && TeXauthorized && allowed )
		{
        v[activeviewer].startDrawToTeX( focus );
        if( texfile != NULL )
        {// have file, have done startup, draw the box
          focus->texdisplay();
          v[activeviewer].finishDrawToTeX();
        }
  			// put up info box telling filename --- no, do to console
	     //informOfSnapshot( focus );
			 std::cout << 
               "Made snapshot to " << texfilename << std::endl;
		}
		else
		  needredraw = 0; // meta key that does nothing

	}// alt key message

  else if( mqKind == MQ_SPEC_KEY )
  {// there is a special key

    if ( mqCode == GLUT_KEY_F1 ||  mqCode == GLUT_KEY_F2)
    {// continue execution, using single or split view
      
      actionmode = executingmode;
      needredraw = 0; 

      // jb18: set pausing or run to break depending on F1/F2
			if(  mqCode == GLUT_KEY_F1 )
			  pausetoinspect = 1;
			else
			  pausetoinspect = 0;

      needredraw = 1;  // jb18:  give chance to display green

      smq->insert( MQ_REQUEST, MQ_START_EXEC, "exec from pause",
                        0, 0 );
    
    }// F1, F2 (jb18)

    // jb18
		else if(  mqCode == GLUT_KEY_F3 )
		{
		  focus->toggleBreakpoint( needredraw );
		}

    else if( mqCode == GLUT_KEY_F4)
    {// stop executing
      // clean up boxes reality, and this viewer

      v[activeviewer].setOnScreen('n');
      // return to interactive viewer
      actionmode = interactivemode;
      activeviewer = interactiveviewer;
      v[activeviewer].setOnScreen( 'y' );
      focus = v[activeviewer].whoFocus(); // for redraw below
      needredraw = 1;
			touring = 0;
			// for safety, close files (ports are left looking attached,
			//  but they are cleared when execution starts)
			ports->closeAll();

      // jb1a:  F4 is interrupting execution, might have
      //        requests to toss
      smq->removeAll();

    }

    // v11:  f7 used to do snapshot, now alt-s does
    else if( mqCode == GLUT_KEY_F7 )
    {// toggle to consoleviewmode, userviewing
		  actionmode = pausedconsoleviewmode;
			userview = 1;

      v[ executionviewer].setOnScreen('n');
			v[ consoleviewer].setOnScreen('n');
			v[ portsviewer].setOnScreen('n');

			v[ userviewer].setOnScreen('y');

      setviewers(0);

			needredraw = 1;
			needrefocus = 0;

    }// F7

    else if( mqCode == GLUT_KEY_F8 )
    {// toggle splitviewing
      if( splitviewing )
      {// switch to full-screen interactive and execution viewer
        v[consoleviewer].setOnScreen('n');
        v[portsviewer].setOnScreen('n');
        v[executionviewer].setOnScreen('y');
        splitviewing = !splitviewing;
        setviewers(0);
        v[executionviewer].setFocus( 1, focus, NULL );
        needredraw = 1;
        needrefocus = 0;
      }
      else
      {// switch to 3 viewers
        v[consoleviewer].setOnScreen('y');
        v[portsviewer].setOnScreen('y');
        v[executionviewer].setOnScreen('y');
        splitviewing = !splitviewing;
        setviewers(0);
        v[executionviewer].setFocus( 1, focus, NULL );
        needredraw = 1;
        needrefocus = 0;
      }
    }// F8

    else if( mqCode == GLUT_KEY_F9 && allowed )
    {// make scale smaller
      v[activeviewer].divScale( 1.2 );
      needredraw = 1;
      needrefocus = 0; 
      // refigure backbox now that the screen is a different
      // size
      v[activeviewer].setFocus( 1, focus, NULL );
    }

    else if( mqCode == GLUT_KEY_F10 && allowed )
    {// make scale bigger
      v[activeviewer].multScale( 1.2 );
      needredraw = 1;
      needrefocus = 0; 
      // refigure backbox now that the screen is a different
      // size
      v[activeviewer].setFocus( 1, focus, NULL ); 
    }

    else if( mqCode == GLUT_KEY_END && allowed )
    {// scroll viewer offset by half a screen to the left
      v[activeviewer].shift( -0.5, 0.0 );
      needredraw = 1;
      needrefocus = 0;
    }
    else if( mqCode == GLUT_KEY_HOME && allowed )
    {// scroll to right
      v[activeviewer].shift( 0.5, 0.0 );
      needredraw = 1;
      needrefocus = 0;
    }
    else if( mqCode == GLUT_KEY_PAGE_DOWN && allowed )
    {// scroll up
      v[activeviewer].shift( 0.0, 0.5 );
      needredraw = 1;
      needrefocus = 0;
    }
    else if( mqCode == GLUT_KEY_PAGE_UP && allowed )
    {// scroll down
      v[activeviewer].shift( 0.0, -0.5 );
      needredraw = 1;
      needrefocus = 0;
    }
    else // illegal function key, ignore
      needredraw = 0;
 
  }// special key message

  else if( mqKind == MQ_REG_KEY )
  {// a regular key is available
    
    if( mqCode == enableAuthoringKey && 
        lesson->getState() == recordingLessonState 
      )
    {// go back to authoring state from recording state
     // tidy up the pause recording
      lesson->recordingToAuthoring();
      needredraw = 1;  // to see the border color change
      recordIt = -1;  
    }

    else if( mqCode == nextkey)
    {
      focus = focus->moveNext( needredraw );
 		  touring = 0;
    }

    else if( mqCode ==prevkey)
    {       
      focus = focus->movePrev( needredraw );
 		  touring = 0;
    }

    else if( mqCode ==upkey)
    {
      focus = focus->moveUpDown('u',needredraw);
			touring = 0;
    }

    else if( mqCode ==downkey)
    {
      focus = focus->moveUpDown('d',needredraw);
			touring = 0;
    }

    else if( mqCode ==firstkey)
    {
      focus = focus->moveHomeEnd('h',needredraw);
			touring = 0;
    }

    else if( mqCode ==lastkey)
    {
      focus = focus->moveHomeEnd('e',needredraw);
      touring = 0;
    }

    else if( mqCode ==outkey)
    {
      focus = focus->moveOut( needredraw );
			touring = 0;
    }

    else if( mqCode ==inkey)
    {
      focus = focus->moveIn( needredraw );
		  touring = 0;
    }

    else if( mqCode ==jumpkey)
    {
      focus = focus->jumpParts( needredraw );
			touring = 0;
    }

    else if( mqCode ==toggleviewskey ||  mqCode ==deletekey)
    {
      focus = focus->cycleAspects( needredraw );
			touring = 0;
    }

    else if( mqCode ==' ')
		{// spacekey with big tour sends on to next guy,
	   // turns on touring and makes the tour guy the focus

			insplist.advance();
			focus = insplist.current();
			needredraw = 1;
			touring = 1;

      v[activeviewer].setFocus( 1, insplist.current(), NULL );
        
      focus = v[activeviewer].whoFocus();

      // make the guy just moved to current in its outer, and
 		  // similarly for all outward
			focus->setCurrentOutward();
				 			  
    }// move to next box on tour

    else if( mqCode ==showkey)
		{
		  focus->changeAllToFull( needredraw );
			touring = 0;
		}

		else if( mqCode ==tellkey)
		{
		  focus->changeAllToDoc( needredraw );
			touring = 0;
		}

  }// regular key message

  else if( mqKind == MQ_CLICK )
  {// a mouse click is waiting to be processed
    // to change the focus

    if( allowed )
    {// mouse click allowed
  
      int whichviewer;
      float vx, vy;

      // determine which viewer received the click and
      // produce vx,vy as location of click in that viewer
      whichviewer = locateLocationInViewer(mousex, mousey, vx, vy);
      mouseviewer = whichviewer;  // note for use by release

      if( whichviewer != executionviewer )
      {// no viewer was hit by the click
        needredraw = 0;  // mark the world unchanged
        mousestatus = 0;  // so won't care about the release
      }
      else
      {// hit a viewer
    
        if( mqCode == MQ_LEFT_BUTTON )
        {// process left click by making that spot the focus
          /*focus = v[whichviewer].setFocusToLocation(
                                          vx, vy, needredraw );
					*/
          newfocus = v[whichviewer].setFocusToLocation(
                                          vx, vy, needredraw );
					focus->simulateMove(newfocus);
					focus = newfocus;
					
					needrefocus = 2;

        }
        else if( mqCode == MQ_RIGHT_BUTTON )
        {// process right click by making that spot the focus
          // and toggling the aspect of the new focus

          newfocus = v[whichviewer].setFocusToLocation(
                                          vx, vy, needredraw );

    			float goalx, goaly;  // goal fractions of mouse in focus
					goalx = mouseoffx / newfocus->howWide();
					goaly = mouseoffy / newfocus->howHigh();

					focus->simulateMove(newfocus);
					focus = newfocus;
          focus = focus->cycleAspects( needredraw );

					int junk;
					focus->figureSizeLocs( junk ); // make sure it knows new size

          needrefocus = 3;
					notemousex = vx - goalx * focus->howWide(); // using new size
					notemousey = vy - goaly * focus->howHigh();

        }
        else
        {// bad code for click
          errorexit("disabled.cpp, bad code for click in paused mode");
        }

        // either way, make whichviewer the interactive viewer
        if(activeviewer != whichviewer )
        {// need to switch
          activeviewer = whichviewer;
          needredraw = 1;
        }
        
				touring = 0;

      }// hit a viewer
    }// click allowed
  }// process click message

   else
   {// some other inappropriate message arrived
     std::cout << "in paused mode, disabled.cpp, got message kind "
           << mqKind << "???" << std::endl;
     exit(1);
   }

//***************************************************************
//***************************************************************
//******************** end of paused mode inclusion *************

  }// paused mode

//***************************************************************
//********************* errormode *******************************
  else if( actionmode == errormode )
  {// in errormode, wait for a key before returning to prior mode

    needredraw = 1;

    if( mqKind == MQ_ALT_KEY || mqKind == MQ_SPEC_KEY || 
        mqKind == MQ_CLICK 
      )
    {// ignore
      needredraw = 0;
    }
    else if( mqKind == MQ_REQUEST )
    {// first arrival in errormode
      needredraw = 1;
    }
    else if( mqKind == MQ_REG_KEY )
    {// there is a key

      if( mqCode == enableAuthoringKey &&
          lesson->getState() == recordingLessonState 
        )
      {// go back to authoring state, tidy up the pause recording
        lesson->recordingToAuthoring();
        needredraw = 1;  // to see the border color change
        recordIt = -1;  
      } 

      else if( mqCode == enterkey )
      {// leave errormode --- in new scheme, any error flips us
        // out to interactivemode

        actionmode = interactivemode;    
        activeviewer = interactiveviewer;

        focus = errreturn->findCover();
			  (v[activeviewer].whoFocus())->simulateMove( focus );
				 // tiny problem --- but after error return, 
				 //  always focus on a "badbox" never inside it
				 focus->moveOutside();
				 v[activeviewer].setFocus( 1, focus, NULL );

        v[executionviewer].setOnScreen('n');
        if(splitviewing)
        {
          v[consoleviewer].setOnScreen('y');
          v[portsviewer].setOnScreen('y');
        }
        else
        {
          v[consoleviewer].setOnScreen('n');
          v[portsviewer].setOnScreen('n');
        }

        v[activeviewer].setOnScreen( 'y' );

        // v11:  
				if( userview )
				{
				  userview = 0;
					v[userviewer].setOnScreen('n');
				}

        ports->closeAll();

        messagecolorcode = errorcolorcode;
				  
      }// enterkey pressed

      else
        needredraw = 0;

    }// reg key

    else
    {// some other inappropriate message arrived
      std::cout << "in error mode, disabled.cpp, got message kind "
            << mqKind << "???" << std::endl;
      exit(1);
    } 

  }// errormode
//********************* end errormode ***************************
//***************************************************************

  else if( actionmode == input1mode ||
           actionmode == inputlinemode )
  {// processing input until.... get 1 in input1mode,
    //  or hit enterkey in inputlinemode, or
    // hit stop

    // grab and toss any keys except stop and the printable(s)
    // we're looking for
    if( mqKind == MQ_SPEC_KEY )
    {
      if( mqCode == GLUT_KEY_F4 )
      {// okay, go ahead and halt
        v[executionviewer].setOnScreen('n');
        // return to interactive viewer
        actionmode = interactivemode;
        activeviewer = interactiveviewer;
        v[activeviewer].setOnScreen( 'y' );
        setviewers(0);
        focus = v[activeviewer].whoFocus(); // for redraw below

				if( userview )
				{// clean it up
				  userview = 0;
					v[userviewer].setOnScreen('n');
				}

        touring = 0;
  			// for safety, close files (ports are left looking attached,
	  		//  but they are cleared when execution starts)
		  	ports->closeAll();

        needredraw = 1;
			
        // jb1a:  F4 is interrupting execution, might have
        //        requests to toss
        smq->removeAll();

			}
			else if( mqCode == GLUT_KEY_F7 && userview )
			{// switch back to normal viewer for input mode
			  userview = 0;
				v[userviewer].setOnScreen('n');

        v[executionviewer].setOnScreen('y');
        focus = v[executionviewer].whoExec();
        v[executionviewer].setFocus( 1, focus, NULL );

				needredraw = 1;
        needrefocus = 0;
			}
      else
        needredraw = 0;

    }// spec key message

    else if( mqKind == MQ_REG_KEY )
    {// a regular key is available

      if( actionmode == input1mode )
      {// we're happy --- got the single key
       // store in global inputstring
        int tempkey = mqCode;
        sprintf( inputstring, "%c", tempkey );
          
        // set state of things to resume execution, 
        actionmode = executingmode;     
        activeviewer = executionviewer;

        needredraw = 1;
        recordIt = 1;  // since not echoing, not doing redraw,
                       // need to demand recording 

        smq->insert( MQ_REQUEST, MQ_HAVE_KEY, "have key", 0, 0 );
      }// input1mode 
      else
      {// inputlinemode--- got a key, might be happy
       
        if( mqCode == enterkey )
        {// hurrah, we can finish inputmode, return inputstring
         // set state of things to resume execution, 

          // make array into standard string
          inputstring[ inputlength ] = '\0';
          // echo the enterkey:
          mystring* tempstring = new mystring;
          tempstring->insertCharAt( 1, (char) mqCode );
          chargrid->print( tempstring );
          tempstring->destroy();

          actionmode = executingmode;     
          activeviewer = executionviewer;

          needredraw = 1;

          smq->insert( MQ_REQUEST, MQ_HAVE_LINE, "have line", 0, 0 );
        }
        else if( mqCode == backspacekey )
        {// need to process backspace
          if( inputlength > 0 )
          {// knock off last symbol
            // move back on screen and replace with a blank         
            mystring* tempstring = new mystring;
            chargrid->moveBack();
            tempstring->insertCharAt( 1, ' ' );
            chargrid->print( tempstring );
            chargrid->moveBack();
            tempstring->destroy();

            inputlength--;

            needredraw = 1;
          }
          else
            needredraw = 0;
        }
        else
        {// is a key to append to inputstring        

          if( inputlength < inputbuffersize )
          {// have space in buffer so take the key
            // echo the new key:
            mystring* tempstring = new mystring;
            tempstring->insertCharAt( 1, (char) mqCode );
            chargrid->print( tempstring );
            tempstring->destroy();

            inputstring[ inputlength ] = (char) mqCode;
            inputlength++;
					          
            needredraw = 1;
          }
          else
            needredraw = 0; // don't take the key

        }// append key to inputstring
         
      }// input line mode

    }// reg key message

    else if( mqKind == MQ_ALT_KEY || mqKind == MQ_CLICK )
    {// ignore
      needredraw = 0;
    }
    else
    {// some other inappropriate message arrived
      std::cout << "in input mode, disabled.cpp, got message kind "
            << mqKind << "???" << std::endl;
      exit(1);
    } 

  }// input modes

//********************* regular code *******************************
//********************* ends ***************************************
//********************* here   *************************************

}// lesson action status allowed regular code action

if( lesson->getState() == disabledLessonState )
{// nothing special to do
  if( focus == NULL )
  {// just finished reading lesson, need to set
    focus = v[activeviewer].whoFocus();
  }
}
else if( lesson->getState() == recordingLessonState )
{// recording
  if( ((needredraw==1 && recordIt==0) || recordIt==1) &&
      (mqKind==MQ_REG_KEY || mqKind==MQ_SPEC_KEY)
    )
    lesson->record( mqKind, mqCode );
// else recordIt==-1 or recordIt==0 and needredraw == 0 so don't
}
else if( lesson->getState() == readingLessonState ||
         lesson->getState() == stoppingLessonState
       )
{// reading a lesson

  if( lesson->shouldRedraw() )
    needredraw = 1;

  // if didn't do it, put it to what reality thinks it is
  if( focus == NULL )
   focus = v[activeviewer].whoFocus();
}
