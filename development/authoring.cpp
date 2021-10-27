// the base authoring state

  // make sure that the focus is initially set to somewhere
  focus = v[activeviewer].whoFocus();

  if( mqKind == MQ_ALT_KEY )
	{// toss any alt key
	  needredraw = 0; // meta key that does nothing
	}

  else if( mqKind == MQ_SPEC_KEY )
  {// ignore special key
    needredraw = 0;
  }// special key

  else if( mqKind == MQ_REG_KEY )
  {// a regular key is available

      if( mqCode == disableAuthoringKey )
      {// disable authoring
        lesson->disableAuthoring();
        needredraw = 1;  // to show border filling
      }
      else if( mqCode == pauseAuthoringKey )
      {// move to paused state and start recording pause keys
        debugcount++;
        lesson->authoringToPaused();
        needredraw = 1;
      }

      else if( mqCode == recordToPlayAuthoringKey )
      {// move to recording state and start recording to play
        lesson->authoringToRecording( "play" );
        needredraw = 1;
      }

      else if( mqCode == recordToPPlayAuthoringKey )
      {// move to recording state and start recording to play
        lesson->authoringToRecording( "pplay" );
        needredraw = 1;
      }

      else if( mqCode == recordToExpectAuthoringKey )
      {// move to recording state and start recording to play
        lesson->authoringToRecording( "expect" );
        needredraw = 1;
      }

      else if( mqCode == moveAuthoringKey &&
               (actionmode == interactivemode ||
                actionmode == pausedmode
               )
             )
      {// move to moving state and allow free changing of focus
       // if in an interactive mode, otherwise is silly
        lesson->authoringToMoving();
        needredraw = 1;
      }

      else if( mqCode == fastForwardLessonKey )
      {// start fast forwarding lesson
        actionmode = interactivemode;
        activeviewer = interactiveviewer;

        v[interactiveviewer].setFocus( 1, lesson->getBox(), NULL );

        // last argument is yes, fast forwarding
        lesson->startReading( lesson->getBox(), &v[lessonviewer], 1,
                                   mq );
        needredraw = 1;  // for the border filling 

      }// transition from authoring to reading/fast forwarding lesson

      else if( mqCode == editLessonKey ||
               mqCode == createNoteKey
             )
      {// transition to manually editing lesson and
       // maybe insert a notebox

        if( mqCode == createNoteKey )
        {// insert a notebox at the lessonviewer focus with the
         // current mouse info inserted

          // use mouse info to produce string telling where it
          // is, including viewer it's over and whether over a box
          mystring* locstring;
          locstring = getLocstringFromMouse();

          box* nbox;

          nbox = v[lessonviewer].addNotebox( locstring );

          lesson->addNote( nbox );

        }// adding a notebox

        // make sure that the lessonbox is using its full view,
        // now that the notebox, if any, has computed its arrowhead
        // offset with the size of the universe determined while
        // the lessonbox is using its abstract aspect
        lesson->authoringToEditing( activeviewer );

        // make the various global changes to viewers
        		
        v[lessonviewer].setOnScreen('y');

        if( splitviewing )
        {
          v[consoleviewer].setOnScreen('n');
          v[portsviewer].setOnScreen('n');
        }

        v[activeviewer].setOnScreen('n');

        setviewers( 0 );

			  focus = v[lessonviewer].whoFocus();

        v[lessonviewer].setFocus( 1, focus, NULL );

        needredraw = 1;
			  needrefocus = 0;

        activeviewer = lessonviewer;

      }// transition authoring to editing

      else if( mqCode == dismissNotesKey )
      {
        lesson->dismissNotes(1); // jb30:  record the command
        needredraw = 1;  // have to redraw with notes gone
      }// dismiss notes

      else if( mqCode == pointingKey )
      {
        // obtain info about where the mouse is
        mystring* newlocstring;  
        newlocstring = getLocstringFromMouse();

        // maybe use info to change a notebox
        lesson->relocateNote( newlocstring );

        needredraw = 1;  // might need to

      }// pointing 

      else
        needredraw = 0;
      
  }// there is a key at all

  else if( mqKind == MQ_CLICK )
  {// a mouse click is waiting to be processed
    needredraw = 0;   
  }   

  else 
  {// some other inappropriate message arrived
    std::cout << "in authoring.cpp, got message kind "
            << mqKind << "???" << std::endl;
    exit(1);
  } 
