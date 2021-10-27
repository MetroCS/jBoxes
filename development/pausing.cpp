// paused state:  toss all input except for "regular" keys, which
//                are tidily recorded,
//        leave back to authoring state when get enableAuthoringKey

  // make sure that the focus is initially set to somewhere
  focus = v[activeviewer].whoFocus();

  if( mqKind == MQ_ALT_KEY )
	{// toss any alt key
	  needredraw = 0; // meta key that does nothing
	}

  else if( mqKind == MQ_SPEC_KEY )
  {// toss special key
    needredraw = 0;
  }// special key was available

  else if( mqKind == MQ_REG_KEY )
  {// a regular key is available

      if( mqCode == enableAuthoringKey )
      {// go back to authoring state, tidy up the pause recording
        lesson->pausedToAuthoring();
        needredraw = 1;  // to see the border color change
      }

      else if( (' '<=mqCode &&  mqCode <='~') || 
                 mqCode ==13 ||  mqCode ==27 ||
                mqCode ==8
             )
      {// record acceptable keys
        lesson->record( mqKind, mqCode  );
        needredraw = 1; // in case the lessonbox is visible
      } 
  
      else
        needredraw = 0;
      
  }// regular key

  else if( mqKind == MQ_CLICK )
  {// toss mouse click
    needredraw = 0;
  }   

  else 
  {// some other inappropriate message arrived
    std::cout << "in pausing.cpp, got message kind "
            << mqKind << "???" << std::endl;
    exit(1);
  } 

