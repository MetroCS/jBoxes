// implementation of Lesson class

#include <iostream>

#include <stdio.h>
#include <string.h>

#include <glut/glut.h>  // Mac OS X
//#include <gl/glut.h>
//#include "glut.h"

#include "constants.h"

#include "mystring.h"
#include "box.h"

#include "utility.h"

#include "viewer.h"
#include "MessageQueue.h"
#include "Lesson.h"

Lesson::Lesson( box* ubox, viewer* iv )
{
  authorized = 0;  // start out not authorized
  state = disabledLessonState;  // and not authoring
  numNotes = 0;       // and with no active notes
  badNotes = 0;       // no notes, so they're current
  universe = ubox;     // convenient frame of reference
  interactiveViewer = iv;
}

int Lesson::getState()
{
  return state;
}

int Lesson::getCommandState()
{
  return commandState;
}

box* Lesson::getBox()
{
  return lessonBox;
}

void Lesson::toggleAuthority()
{
  authorized = ! authorized;
}

int Lesson::isAuthor()
{
  return authorized;
}

void Lesson::enableAuthoring( box* lb, viewer* lv )
{
  lessonViewer = lv;  // keep access to the lessonviewer
  // center the lesson viewer over the lesson box as its initial focus
  lessonViewer->setFocus( 1, lb, NULL ); 
  lessonBox = lb;
  focus = lb;

  // jb26:  try to keep lessonbox out of the way
  lessonBox->setAspect( 'a' );
  lessonBox->fixupReality( 0 );

  state = authoringLessonState;
}

void Lesson::disableAuthoring()
{
  state = disabledLessonState;
  lessonViewer->setFocus( 1, focus, NULL );

  // as add features to lesson, this will expand
  //  like must restore backups

}

void Lesson::pausedToAuthoring()
{
  state = authoringLessonState;

  record( 1, -1 );  // special "symbol" that causes recording
                 // of ) <enter>
}

void Lesson::recordingToAuthoring()
{
  state = authoringLessonState;

  record( 1, -1 );  // special "symbol" that causes recording
                 // of ) <enter>
}

// fbox is the final interactively achieved focus to be moved to
void Lesson::movingToAuthoring( box* fbox )
{
  state = authoringLessonState;

  // get location string from the fbox itself, no mouse involvement
  mystring* locstring = fbox->getLocString( 0, ' ', -1 );

  // store the location that has been built up in Lesson.locstring
  (focus->whatName())->append( locstring );

  locstring->destroy();  // save memory space

  // finish the command line
  char s[3];
  s[0] = ')'; s[1] = 13; s[2] = '\0';
  focus->appendToCommand( s );
  
}// movingToAuthoring

void Lesson::authoringToPaused()
{
  state = pausedLessonState;

  box* cbox;
  cbox = focus->getComboxAtFocus();
  focus = cbox;  // keep focus on the box where recording

  // jb30:  lessonviewer must have its focus changed, too
  lessonViewer->setFocus( 1, focus, NULL );

  // set up recording
  focus->appendToCommand( "pause(" );
  repCount = 0;
  repChar = -1;  // must do this to signal first call
  lineCount = 6;
  indent = 6;

  firstPlayBack = 0;  // make sure doesn't dismiss on a pause key hit
}

void Lesson::authoringToRecording( char* method )
{
  state = recordingLessonState;

  box* cbox;
  cbox = focus->getComboxAtFocus();
  focus = cbox;  // keep focus on the box where recording

  // jb30:  lessonviewer must have its focus changed, too
  lessonViewer->setFocus( 1, focus, NULL );

  // set up recording
  focus->appendToCommand( method );
  focus->appendToCommand( "(" );
  repCount = 0;
  repChar = -1;  // must do this to signal first call
  lineCount = strlen(method) + 1;
  indent = lineCount;

  // jb5a:  set firstPlayBack to 1, then when recording and first
  //        key is recorded, dismiss then
     // jb31:  dismiss when start pplay, expect, not play
  if( strcmp( "play", method ) != 0 )
    firstPlayBack = 1;
  else
    firstPlayBack = 0;  // so play won't dismiss

}

void Lesson::authoringToMoving()
{
  state = movingLessonState;

  box* cbox;
  cbox = focus->getComboxAtFocus();
  focus = cbox;  // keep focus on the box where recording

  // jb30:  lessonviewer must have its focus changed, too
  lessonViewer->setFocus( 1, focus, NULL );

  // set up for reporting of location
  focus->appendToCommand( "move(" );

}// authoringToMoving

void Lesson::authoringToEditing( int av )
{
  state = editingLessonState;
  focus = lessonViewer->whoFocus();
  formerActiveviewer = av;

  lessonBox->setAspect( 'f' );
  lessonBox->fixupReality( 0 );

}// authoringToEditing

void Lesson::editingToAuthoring( int& av )
{
  state = authoringLessonState;

  av = formerActiveviewer;  // restore activeviewer

  badNotes = 1;  // note that lesson might have changed, hence
                 // prepareNotes might be needed before display

  lessonBox->setAspect( 'a' );
  lessonBox->fixupReality( 0 );

}// editingToAuthoring

// given the lessonbox lbox and the lesson viewer lv, initialize
// everything and start reading the lesson, or fast forwarding it
// if ff 
void Lesson::startReading( box* lbox, viewer* lv, int ff,
                            MessageQueue* mq )
{
  state = readingLessonState;  
  // in case notes are lingering from previous work with this lesson,
  // clean up
  dismissNotes( 0 ); // don't record dismiss() command

  lessonViewer = lv;
  lessonBox = lbox;

  focus = lbox;

  // fix up all the input processing guys
  putbackKey = 0;
  repCount = 0;

  commandState = neutralCommandState;

  // prepare for fast forwarding
  if( ff )
    stopBox = lessonViewer->whoFocus();
  else
    stopBox = NULL;  // not fastforwarding

  mq->insert( MQ_READ_ON, 0, "start reading", 0, 0 );

}// startReading

int Lesson::shouldStopReading()
{
  if( focus == stopBox )
    return 1;
  else
    return 0;
}

void Lesson::stopReading()
{
  state = stoppingLessonState; // put up message and wait
                                       // for enter, when get it,
                                       // return to normal
  // manage "error message" stuff
  messagecolorcode = stoplessoncolorcode;
  strcpy( errormessage, "Lesson reading is stopped." );

  doRedraw = 1;

}// stopReading

// really stop, return to "normal state" after
// <enter>ing to remove message
void Lesson::stopStopping()
{
  if( stopBox != NULL )
  {// reset everything to authoring mode
    state = authoringLessonState;
  }
  else
  {// reset everything to normal interactive mode
    state = disabledLessonState;   
    dismissNotes(0);
  }

  messagecolorcode = errorcolorcode;

}

// "record" appropriately the key with kind k, code x,
// where (-1, whatever) means do )<enter> to end a session

// "internally" in a lesson, code up (knd,cod), where
// knd=1 for regular, 2 for special, 3 for alt,
// by 10000(k-1)+cod, -1 is an exception
// (do this so that a single int repChar can hold all the info)
void Lesson::record( int knd, int cod )
{
  if( firstPlayBack )
  {
    dismissNotes(0);
    firstPlayBack = 0;
  }

  char s[80];  // for holding the string to append

  // encode (knd,cod) in x
  int x;
  if( knd==-1 )
    x = -1;
  else
    x = 10000*(knd-1)+cod;

  if( repCount > 0 && x == repChar )
  {// x is same as current, just count it
    repCount++;
  }
  else if( repChar == -1 )
  {// have no repChar because is first call, note x unless x is
   // -1, in which case quitting immediately
    if( x == -1 )
    {
      s[0] = ')'; s[1] = 13; s[2] = '\0';
      focus->appendToCommand( s );     
    }
    else
    {
      repChar = x;
      repCount = 1;
    }
  }
  else
  {// seeing a new key, record current and switch

    int index = 0;  // position for inserting into s
    int k;

    // depending on repChar, put 1 or 2 symbols into s
      int r = repChar;  // for typing convenience below

      // jb2a:  note that for regular keys, r is correct
      //   and s holds actual standard symbols

      // translate shortcut keys to control keys
      // (others are translated in processspecialkey)

      if( r==deletekey )
        r=3;  // i.e., ctrl-c

      // # \ ~ ) ^ have special meaning in recording, so
      //   must escape them if literally want them in s
      if( r=='#' || r=='\\' || r=='~' || r==')' || r=='^' )
      {  s[0]='\\'; s[1]=r; index=2; }
      else if( r==' ' )
      { s[0]='~'; index=1; }
      else if( '!'<=r && r<='~' )
      { s[0]=r; index=1; }
      else if( 1<=r && r<=26 )
      { s[0]='^'; s[1]=96+r; index=2; }
      else if( r == specialspacekey )
      { s[0] = '\\'; s[1] = 'e'; index=2; }

      // jb2a:  special keys changed

      // relying on codes for F1 through F10 being in order
      else if( GLUT_KEY_F1 + 10000 <= r && r <= GLUT_KEY_F9 + 10000 )
      { s[0] = '\\'; s[1] = 48 + r-10000; index = 2; }
      else if( GLUT_KEY_F10 + 10000 == r )
      { s[0] = '\\'; s[1] = '0'; index = 2; }
      else if( r == tabkey )
      { s[0] = '\\'; s[1] = 't'; index = 2; }
      else if( r== GLUT_KEY_HOME + 10000 )
      { s[0] = '\\'; s[1] = 'l'; index = 2; }
      else if( r== GLUT_KEY_END + 10000 )
      { s[0] = '\\'; s[1] = 'r'; index = 2; }
      else if( r== GLUT_KEY_PAGE_UP + 10000 )
      { s[0] = '\\'; s[1] = 'u'; index = 2; }
      else if( r== GLUT_KEY_PAGE_DOWN + 10000 )
      { s[0] = '\\'; s[1] = 'd'; index = 2; }
      else
        errorexit("Lesson::record---unhandled key code");

    // depending on repCount, put # followed by digits into s
    // jb5a:  followed by a space!
    if( repCount == 2 )
    {// put in whatever is in s again
      int m = index;
      for( k=0; k<m; k++ )
      {
        s[m+k] = s[k];  index++;
      }
    }
    else if( repCount > 2 )
    {// put in count for 3 or more reps
      s[index] = '#'; index++;
      char temp[10];
      sprintf( temp, "%d", repCount );
      for(k=0;k<strlen(temp);k++)
      { s[index]=temp[k]; index++; }
      s[index] = ' '; index++;  // jb5a:  add in a space on end
    }
    // else if repCount == 1, have already put in s

    // depending on lineCount, put <enter> followed by indent spaces
    // into s
    lineCount += index;  // will be adding this many symbols
    if( lineCount > 30 )
    {// start a new line
      s[index] = 13; index++;
      for(k=1;k<=indent;k++)
      {  s[index]=' '; index++; }
      lineCount = indent;
    }

    // send s off
    s[index] = '\0';
    focus->appendToCommand( s );

    // update rep info using x
    if( x == -1 )
    {// special "key" meaning finish up
      s[0] = ')'; s[1] = 13; s[2] = '\0';
      focus->appendToCommand( s );
    }
    else
    {// a real key
      repChar = x;
      repCount = 1;
    }

  }// a new key seen

}// Lesson::record

void Lesson::setActionStatus( int s )
{
  actionStatus = s; 
}

int Lesson::getActionStatus()
{
  return actionStatus;
}

int Lesson::shouldRedraw()
{
  return doRedraw;
}

// get the next stored key (and return it) and extract it into 
// mk,mc
int Lesson::processNextStoredKey( int& mk, int& mc )
{
  int sym = nextKey();
  mc = sym % 10000;
  mk = (sym-mc)/10000 + 1;
  return sym;
}

// only called when reading
void Lesson::processMessage( int& mk, int& mc, MessageQueue* mq )
{

  actionStatus = 0;  // in case forget to set
  doRedraw = 0;

  if( mk == MQ_HALT )
  {// user aborted reading by F6
    actionStatus = 0;
    doRedraw = 1;
    stopReading();
  }

  else if( mk == MQ_CLICK || mk == MQ_ALT_KEY )
  {// ignore mouse clicks and alt keys while reading
    actionStatus = 0;
    doRedraw = 0;
  }

  else if( mk == MQ_REQUEST )
  {// requests are honored in any state,
    actionStatus = 1;  // will do standard code with request
  }

  else if( mk == MQ_READ_ON )
  {// sent when a command or notebox has finished
    // or at start
    readOn( mq );  // just because it's big
  }

  else if( mk == MQ_REG_KEY || mk == MQ_SPEC_KEY )
  {// message says a physical key was pressed

    if( commandState == pauseCommandState )
    {// process mk,mc through pausing

      doRedraw = 0;  // invisibly tracking pause keys
      actionStatus = 0;  // do not do standard code

      // move through the stored symbols
      int pbk = nextKey();
      
      if( pbk != 10000*(mk-1)+mc )
      {// key pressed (mk,mc) doesn't match 
       // what is being waited for (pbk)
        putback( pbk );
      }
      else
      {// key matches, see if done
        pbk = nextKey();
        if( pbk != 0 )  // code for actual ) marking end of command 
        {// not at end of pausing command
          putback( pbk );
        }
        else
        {// at end of pausing command
         // Go on with next stored lesson stuff
          commandState = neutralCommandState;
          mq->insert( MQ_READ_ON, 0, "finished pause", 0, 0 );
        }

      }// key matched
    }
    else if( commandState == pplayCommandState )
    {
      if( mk == MQ_REG_KEY && mc == escapeKey )
      {// correctly hit esc key while doing prompted play
       // so get the stored key and play it

        int pbk = processNextStoredKey( mk, mc );

        if( firstPlayBack )
        {// is first key hit for playback, dismiss notes
          dismissNotes(0);
          firstPlayBack = 0; // only do it once
        }

        actionStatus = 1; // do the standard code 
        
        // look ahead to see if next key is ) so know to quit
        pbk = nextKey();
        if( pbk != 0 )  // code for actual ) marking end of command 
        {// not at end of pplaying command
          putback( pbk );
        }
        else
        {// at end of pplaying command
          commandState = neutralCommandState;
          mq->insert( MQ_READ_ON, 0, "finished pplay", 0, 0 );
        }

      }// hit esc while pplaying
      else
      {// silly key hit while expecting esc
        actionStatus = 0;
        doRedraw = 0;
      }
    }// pplayCommandState
    else if( commandState == expectCommandState )
    {
      int mkStored, mcStored;

      int pbk = processNextStoredKey( mkStored, mcStored );

      if( mk != mkStored || mc != mcStored )
      {// key pressed doesn't match what is expected
        putback( pbk );
        actionStatus = 0;
        doRedraw = 0;
      }
      else
      {// key matches, play it, then see if done with the expect call

        if( firstPlayBack )
        {// is first key hit for playback, dismiss notes
          dismissNotes(0);
          firstPlayBack = 0; // only do it once
        }
        
        // mk,mc holds expected key already, so leave alone!

        actionStatus = 1; // do the standard code 
     
        // look ahead to see if next key is ) so know to quit
        pbk = nextKey();
        if( pbk != 0 )  // code for actual ) marking end of command 
        {// not at end of expecting command
          putback( pbk );
        }
        else
        {// notice end of expect
          commandState = neutralCommandState;
          mq->insert( MQ_READ_ON, 0, "finished expect", 0, 0 );
        }

      }// hit matching key while expecting

    }// expectCommandState

    else
    {// ignore physical keys while playing, requesting
      actionStatus = 0;
      doRedraw = 0;     
    }
  }// physical key

  else if( mk == MQ_SIM_REG_KEY || mk == MQ_SIM_SPEC_KEY )
  {// message says a simulated key was encountered
   // so had better be playing!

    if( commandState == playCommandState )
    {
      // convert simulated key to physical
      if( mk == MQ_SIM_REG_KEY )
        mk = MQ_REG_KEY;
      else if( mk == MQ_SIM_SPEC_KEY )
        mk = MQ_SPEC_KEY;

      actionStatus = 1;  // go ahead and send sim. key through 
                         // as if real

      // look ahead and either enqueue the next simulated key
      // or enqueue a read on message

      int mkSim, mcSim;
      int pbk = processNextStoredKey( mkSim, mcSim );
  
      if( pbk != 0 )  // code for actual ) marking end of command 
      {// have a key to play 

        if( mkSim == MQ_REG_KEY )
          mq->insert( MQ_SIM_REG_KEY, mcSim, "sim a reg key", 0, 0 );
        else if( mkSim == MQ_SPEC_KEY )
          mq->insert( MQ_SIM_SPEC_KEY, mcSim, "sim a spec key", 0, 0 );
        else
          errorexit("Lesson::processMessage, bad kind of key");

      } // have a key to play
      else
      {// at end of playing command so read on
        commandState = neutralCommandState;
        mq->insert( MQ_READ_ON, 0, "finished play", 0, 0 );
      }

    }// sim key in play command state

    else
    {// if hit simulated key in any other commandState, something
     // is wrong
      std::cout << 
        "reading.cpp, sim. key message with bad commandState " <<
                  commandState << std::endl;
      exit(1);
    }

  }// simulated key

  else
  {
    std::cout << 
       "reading.cpp, got bad message kind " << mk << "???" 
         << std::endl;
    exit(1);
  }

}// processMessage

// scan ahead in stored lesson and decide what to do next,
// leading always to sending of a message
//  This guy takes care of the stopBox, too, when fast forwarding
void Lesson::readOn( MessageQueue* mq )
{
  int pbk, mk, mc;  // for extracting stored key info
                    // with pause, play, pplay, expect

  if( commandState != neutralCommandState )
  {// got here erroneously somehow
    errorexit("Lesson::readOn called erroneously in non-neutral state");
  }
  else
  {// in neutral state, called by read on message

    int situation; // local state variable
    mystring* s = NULL;   // holds handy access to string in combox 
    int pos;     // handy storage of position in s

    situation = 0; // initial state

    while( situation <= 5 )
    {// make another transition

      if( situation == 0 )
      {// starting point
        if( focus->whichKind() == lessonbox )
        {
          if( shouldStopReading() )
          {// author wants to insert material at beginning
              // indicated by putting stopbox on mark in lessonbox
              // which puts stopbox as lessonbox
            stopReading();
            situation = 6;
          }
          else
            situation = 1;  // first command for this reading
        }// focus is lessonbox
        else if( focus->whichKind() == combox )
        {// ended up with focus on a combox, current in name is ')'
          // or on mark at start (if got here on first command)
          s = focus->whatName(); // get the combox's name string
          // the mystring's cursor is storing the position
          pos = s->whereCursor();
          situation = 3;      
        }
        else if( focus->whichKind() == notebox )
        {// last time displayed the notebox, ready to go on
          if( shouldStopReading() )
          {// the notebox was the stopbox
            stopReading();
            situation = 6;
          }
          else
          {// if is next box, move on to it
            if( focus->whoNext() != NULL )
            {// have a next inner box after the notebox focus
              focus = focus->whoNext();
              situation = 2;
            } 
            else
            {// notebox is last, so lesson is over
              stopReading();
              situation = 6; 
            }
          }// // have next box?

        }// focus is a notebox in sit 0
        else
          errorexit("Lesson::carryOn---sit 0");
        }
      
      else if( situation == 1 )
      {// focus on a lessonbox
        if( focus->whoFirst() != NULL )
        {// have an inner box in the lessonbox
          focus = focus->whoFirst();  // move to inner box
          situation = 2;
        } 
        else
        {// is no inner box in lesson box, so lesson is over
         // by hitting end
          stopReading();
          situation = 6; 
        }
      }// situation 1
      
      else if( situation == 2 )
      {// focus on an inner box of lessonbox
        if( focus->whichKind() == combox )
        {// have a combox, set cursor to start on name string
          s = focus->whatName();
          pos = 0;  // on the mark
          s->setCursor(0);
          situation = 3;
        }
        else if( focus->whichKind() == notebox )
        {// have a notebox
          situation = 5;  
        }
        else
          errorexit("Lesson::carryOn, sit 2");
      }
      
      else if( situation == 3 )
      {// focus is on a combox, cursor in name string s on mark or )
       // or whitespace
        
        // pull out next symbol, if any
        if( pos == s->length() )
          situation = 4;  // ran out of symbols in combox
                            // so look ahead
        else
        { // have a next symbol
          int sym = s->charAt( pos+1 );

          if( sym == ' ' || sym == 13 )
          {// scan past whitespace
            pos++;
            s->advanceCursor();
            // situation stays at 3
          }
          else 
          {// maybe have a command word

            // get on the first non-whitespace symbol
            pos++; 

            // if matches, pos and cursor in s are advanced to 
            // sit on end
            if( s->matches( pos, "dismiss()" ) )
            {
              dismissNotes(0); // toss all the active notes,
                               // jb30:  not recording
              doRedraw = 1;
              situation = 6; 
              mq->insert( MQ_READ_ON, 0, "dismissed notes", 0, 0 );
            }
            else if( s->matches( pos, "move(" ) )
            {// do the move immediately in reality and continue
             // looking for lingering commands

              s->advanceCursor(); // is on whatever follows (
              // do the move in the interactive viewer
              box* intfocus = interactiveViewer->whoFocus();
              box* newFocus = intfocus->findBoxFromLocString( s );
              pos = s->whereCursor();
              interactiveViewer->setFocus( 1, newFocus, NULL );
            
              situation = 6; 
              mq->insert( MQ_READ_ON, 0, "moved focus", 0, 0 );
            } 
            else if( s->matches( pos, "pause(" ) )
            {           
              situation = 6; // to force exit
              commandState = pauseCommandState;

              // look ahead --- if next key is ), read on
              // (must not passively wait for a key in this case)
              pbk = processNextStoredKey( mk, mc );
              if( pbk != 0 )
              {// isn't immediate )
                putback( pbk );
              }
              else
              {// have a silly pause(), so keep looking
                commandState = neutralCommandState;
                situation = 0;
              }

            }
            else if( s->matches( pos, "pplay(" ) )
            {
              situation = 6; // to force exit
              commandState = pplayCommandState;
              firstPlayBack = 1;  // so can dismiss notes next time

              // look ahead --- if next key is ), read on
              // (must not passively wait for a key in this case)
              pbk = processNextStoredKey( mk, mc );
              if( pbk != 0 )
              {// isn't immediate )
                putback( pbk );
              }
              else
              {// have a silly pplay(), so keep looking
                commandState = neutralCommandState;
                situation = 0;
              }
              
            }
            else if( s->matches( pos, "expect(" ) )
            { 
              situation = 6; // to force exit
              commandState = expectCommandState;
              firstPlayBack = 1;  // so can dismiss

              // look ahead --- if next key is ), read on
              // (must not passively wait for a key in this case)
              pbk = processNextStoredKey( mk, mc );
              if( pbk != 0 )
              {// isn't immediate )
                putback( pbk );
              }
              else
              {// have a silly expect(), so keep looking
                commandState = neutralCommandState;
                situation = 0;
              }
            
            }
            else if( s->matches( pos, "play(" ) )
            {
              situation = 6; // to force exit
              commandState = playCommandState;

              // enqueue first simulated key or realize is )

              pbk = processNextStoredKey( mk, mc );

              if( pbk != 0 )
              {// isn't immediate ), enqueue it as simulated key
 
                if( mk == MQ_REG_KEY )
                  mq->insert( MQ_SIM_REG_KEY, mc, 
                                 "sim a first reg key in play(", 0, 0 );
                else if( mk == MQ_SPEC_KEY )
                  mq->insert( MQ_SIM_SPEC_KEY, mc,
                          "sim a first spec key in play(", 0, 0 );
                else
                  errorexit("Lesson::carryOn, bad kind of key");
                
              }
              else
              {// have a silly play(), so keep looking
                commandState = neutralCommandState;
                situation = 0;
              }
            }
            else
            {
              errorexit(
                    "Lesson::carryOn---fatal error in situation 3");
            }

          }// maybe have a command word
        }// have a next symbol
      }// situation 3

      else if( situation == 4 )
      {// ran out of symbols in current focus combox

        if( shouldStopReading() )
        {// the focus has finished being read, and might be stopbox
          situation = 6;
          stopReading();
        }
        else if( focus->whoNext() != NULL )
        {// move to next box
          focus = focus->whoNext();
          situation = 2;          }
        else
        {// lesson is over by running out of material
         // ---there is no next box
          situation = 6;
          stopReading();
        }
      }

      else if( situation == 5 )
      {// reached a notebox focus
         addNote( focus ); // add the focus notebox to the lesson list
         situation = 6;
         doRedraw = 1;  // see the note displayed
         // go around again and continue reading lesson
         commandState = neutralCommandState;
         mq->insert( MQ_READ_ON, 0, "display note", 0, 0 );
      }// situation 5

      else if( situation == 6 )
      {// exit loop because have set commandState
      }

      else
        errorexit("unknown situation in Lesson::carryOn");

    }// loop to make repeated transitions

  }// neutral command state

}// readOn

// given a string w, and a valid position in it,
//  namely on an actual symbol, where previous work left it,
// move the cursor ahead in it until the
// cursor is positioned over the last consecutive whitespace
// and put pos there too
//   If whitespace goes to the end of the string, stop on the
//   last symbol
void scanOverWhitespace( int& pos, mystring* w )
{
  int len, sym;
  
  len = w->length();

  if( pos < len )
    sym = w->charAt( pos+1 );

  while( pos < len && (sym==' ' || sym==13)
       ) 
  {// next is whitespace, move ahead onto it
    pos++;  w->advanceCursor();

    if( pos < len )
      sym = w->charAt( pos+1 ); // try ahead another
  }
    
}

// scan ahead in simulated key code sequence given by current
// argument string in lessonbox, return next key code
//   At start, name string in focus box has its cursor sitting on
//   the symbol previously processed, and repCount/repCode contain
//   0/- or positive/code,
int Lesson::nextKey()
{
  // jb31:  install putback facility
  if( putbackKey > 0 )
  {
    int temp = putbackKey;
    putbackKey = 0;  // signal now have none
    return temp;
  }

  if( repCount > 0 )
  {// have a repeated code ready to go
    repCount--;
    return repChar;
  }
  else
  {// continue scanning the focus's name string 
    mystring* w = focus->whatName();  // handy access to the name
      // be sure to move actual cursor to pos when done

    // whitespace can only occur between symbol packages such
    // as a, ^a, \a, a#123, \a#123 so only need to scan past whitespace
    // at start of each call

    // maintain pos for convenience
    int pos = w->whereCursor(); // sitting on previous processed

    // for convenience
    int len = w->length();

    scanOverWhitespace( pos, w );

    if( pos == len )
      return -1;     // error --- was only whitespace left in the string

    // there is a next symbol, move onto it
    pos++; w->advanceCursor();
    int sym = w->charAt( pos );

    // determine  repChar

    if( sym == ')' )
      return 0;   // found end of argument string

    else if( sym == '\\' )
    {// starting escape sequence

      // move onto the next symbol    
      if( pos == len )
        return -1;  
      pos++; w->advanceCursor();
      sym = w->charAt( pos );

      // determine code for this escape symbol
      if( sym=='#' || sym=='\\' || sym=='~' ||
          sym==')' || sym=='^' 
        )
        repChar = sym;
      else if( '1' <= sym && sym <= '9' )
        repChar = 10000 + sym - '0';
      else if( sym == '0' )
        repChar = 10010;
      else if( sym == 'e' )
        repChar = 27;
      else if( sym == 't' )
        repChar = tabkey;
      else
        return -1; // illegal symbol after backslash

    }// starting escape sequence

    else if( sym == '^' )
    {// ctrl char

      // move onto the next symbol    
      if( pos == len )
        return -1;  
      pos++; w->advanceCursor();
      sym = w->charAt( pos );

      if( 'a' <= sym && sym <= 'z' )
        repChar = sym - 'a' + 1;
      else
        return -1;

    }// ctrl char

    else if( sym == '~' )
    {// hard-space

      repChar = ' ';

    }// hard-space
    else if( '!' <= sym && sym < '~' )
    {// printable

      repChar = sym;

    }// printable
    else
      return -1;  // error --- unexpected symbol

    // now, see if there is a repetition factor

      // glance ahead
      if( pos == len || w->charAt( pos+1 ) != '#' )
      {// on end or next is not #, so no rep factor
       // so done
        repCount = 0;  // just to be safe
        return repChar; // spit the single code
      }
      else
      {// not on end and next is #, so move on to the number
        pos++; w->advanceCursor(); // now on the # symbol

        // scan 1 or more digits to build up repCount
        repCount = 0;
        if( pos < len )
          sym = w->charAt( pos+1 );
        while( pos < len && '0'<=sym && sym<='9' )
        {// next is a digit, move onto it and process

          repCount = 10*repCount + sym - '0';
          pos++;
          w->advanceCursor();

          // move ahead
          if( pos < len )
            sym = w->charAt( pos+1 );
        }

        // spit repChar and reduce repCount by 1
        repCount--;
        return repChar;

      }// have # after symbol
    
  }// scan the name string

}// Lesson::nextKey

void Lesson::putback( int key )
{
  if( putbackKey == 0 )
  {
    putbackKey = key;
  }
  else
    errorexit("fatal error in Lesson::putback---already have one");
}

// note-related methods

// add the given notebox to this lesson's active list
void Lesson::addNote( box* nbox )
{
  if( numNotes == maxNotes )
    errorexit("Lesson::addNote: tried to add one note too many");

  notes[numNotes] = new noteinfo( nbox );
  numNotes++;

  badNotes = 1;  // might be first, need to reposition anyway

}

// given pointer to note box that is about to be deleted,
// go through active notes and remove it if it's there
void Lesson::removeNote( box* nbox )
{
  int k, index=-1;
  for(k=0; k<numNotes; k++ )
    if( notes[k]->getNote() == nbox )
      index = k;

  if( index >= 0 )
  {// remove it
    noteinfo* victim = notes[index];

    for(k=index+1; k<numNotes; k++ )
      notes[k-1]=notes[k];

    victim->destroy();

    numNotes--;

    badNotes = 1;  // definitely needs reworking, probably

    // now set lesson focus to correct thing
    // which is the box immediately after the notebox nbox 
    // or before if it's last
    if( nbox->whoNext() == NULL )
    {
      box* pbox = nbox->findPrev();

      if( pbox != NULL )
        focus = pbox;  // nbox is last in lesson, use prev
      else
        focus = nbox->whoOuter();  // nbox only box inside lessonbox?
    }
    else
    {// have a next, make it the lesson focus
      focus = nbox->whoNext();
    }
  }

}// removeNote

void Lesson::invalidateNotes()
{
  badNotes = 1;
}

void Lesson::validateNotes()
{
  badNotes = 0;
}

// get rid of all notes (in response to dismiss command or 'd' in
// authoring mode), 
// jb30:  recording tells whether need to record the
//  "dismiss()" command in the lesson in addition to actually
// tossing the notes
void Lesson::dismissNotes( int recording )
{
  int k;
  for( k=0; k<numNotes; k++ )
  {
    notes[k]->destroy();  // release text memory space
    delete notes[k];      // recover memory space for the noteinfo
  }

  numNotes = 0;
  badNotes = 1;

  if( recording )
  {// slap the "dismiss()" command in appropriate combox
    box* cbox;
    cbox = focus->getComboxAtFocus();
    focus = cbox;  // keep focus on the box where recording

    // jb30:  lessonviewer must have its focus changed, too
    lessonViewer->setFocus( 1, focus, NULL );

    // set up recording
    focus->appendToCommand( "dismiss(" );
    repCount = 0;
    repChar = -1;  // must do this to signal first call
    lineCount = 8;
    indent = 8;

    record( 1, -1 ); // record ) enter

  }// record the "dismiss()" command

}// dismissNotes

// do everything necessary to prepare the notes in a particular
// viewer for display, from
// scratch, unless believe that the notes are current
void Lesson::prepareNotes( viewer actviewer )
{
  // only prep notes for this viewer if there are any bad notes
  // at all and the lesson state allows for note display
  if( badNotes &&
      ( state != disabledLessonState &&
        state != editingLessonState          
      )
    )
  {// might need to recompute note stuff

    // get viewer id in convenient local variable
    // only console viewer and ports viewer 
    char vid;
    if( actviewer.getId() == consoleviewer )
      vid = 'c';
    else if( actviewer.getId() == portsviewer )
      vid = 'p';
    else
      vid = '-';

    int k;

    // jb6a:  this doesn't work when calling prepareNotes
    //        with multiple viewers!
    // badNotes = 0;  // so know not to do again unnecessarily
    
    // jb6a:  in all the work below, filter out all notes whose
    //        viewer is not vid

    // position the notes and
    // determine so-called "outer rectangle" which is the largest 
    // rectangle that misses all the notes

      // get size of view region in convenient form
      float viewwidth, viewheight;
      actviewer.getSize( viewwidth, viewheight );

      // using first come, first served rule, position notes along
      // edges in order, skipping notes with errors
      //   (was planning to take into account used space on later
      //    edges, but cheaped out)
      
        // first, unpin all good ones, pin ones with errors and report
        for( k=0; k<numNotes; k++ )
        {
          // only look at notes for the current viewer
          if( notes[k]->getViewer() != vid )
            continue;  // jump to end of loop

          if( notes[k]->getEdge() == 'e' )
          {
            notes[k]->pin( 0, 0, viewwidth, viewheight );
            std::cout << "Note " << k << " has error in edge" 
                        << std::endl;
          }
          else
            notes[k]->unpin();
        }

        int done = 0;
        char currentEdge;  // the edge currently being worked on
        int glueCount;
        float totalSize;
 
        // main loop to pin all notes in correct order
        // (no longer important, but might as well, avoid like
        // looping through 'l', 't', ...)
        do{
          // find first unpinned note for vid
          k=0;
          currentEdge = '?'; // mark as not having one

          while( k<numNotes && currentEdge == '?' )
            if( notes[k]->isPinned() || 
                notes[k]->getViewer() != vid 
              )
              k++;
            else
              currentEdge = notes[k]->getEdge();

          if( currentEdge == '?' )
            done = 1;
          else
          {// still have an edge to work on

            // add up sizes and glues for all notes on current edge
            glueCount = 0;
            totalSize = 0;
            for( k=0; k<numNotes; k++ )
              if( notes[k]->getEdge() == currentEdge &&
                  notes[k]->getViewer() == vid 
                )
              {
                glueCount += notes[k]->getGlue();
                totalSize += notes[k]->getSize();
              }

            // now figure the glue amount and go back through and
            // pin all the notes on this edge
            float totalSpace, glueAmount, current;

            if( currentEdge == 'l' || currentEdge == 'r' )
              totalSpace = viewheight;
            else
              totalSpace = viewwidth;      
       
            if( glueCount == 0 )
              glueAmount = 0;  // butt to the front edge
            else
              glueAmount = (totalSpace - totalSize) / glueCount;

            if( currentEdge == 'l' || currentEdge == 'r' )
              current = viewheight;
            else
              current = 0;

            for( k=0; k<numNotes; k++ )
              if( notes[k]->getEdge() == currentEdge &&
                  notes[k]->getViewer() == vid
                )
              {
                current = notes[k]->pin( current, glueAmount,
                                         viewwidth, viewheight );
              }

          }// have an edge to do

        }while( !done );
         

    // jb7a:  this backbox stuff is inappropriate for non-'-' vid
    if( vid == '-' )
    {
     //   find the offset of the ulc of the backbox to the 
     //          universe ulc, use this info to transform each note's
     //          universal coordinate info into offset within the viewer

      box* viewbackbox;
      float bx, by;

      viewbackbox = actviewer.getBackboxInfo( bx, by );

      int junk;  // to receive success code
    
      float vx, vy;  // offset from univbox ulc to viewbackbox ulc
      junk = viewbackbox->findOffsetOutward( universe, vx, vy );

      float convertx, converty;  // add convertx,y to universal coords
      convertx = bx - vx;        // to convert to view coords
      converty = by - vy;

      // convert arrow info to view coords
      for( k=0; k<numNotes; k++ )
      {
        // jb6a:  only do conversion for notes in this viewer
        if( notes[k]->getViewer() == vid )
          notes[k]->convertArrowheadToViewCoords( convertx, converty );
      }

    }// vid is '-' so need to do backbox work
    else
    {// no adjustment to ax,ay needed
    }
  
  }// have bad notes

}// prepareNotes

// go ahead and draw the active notes for the current viewer
void Lesson::displayNotes( viewer* actviewer )
{
/* jb7a:  use the viewer's viewport and scaling and all
          that is already in effect

  // set up OpenGL whole-window drawing
    glLoadIdentity();
    glViewport(0, 0, screenwidth, screenheight );
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,screenwidth,0,screenheight,-minz,-maxz);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();  
    glScalef( 1.0, 1.0, 1.0 );
*/

  int k;
  char vid;

  if( actviewer->getId() == consoleviewer )
    vid = 'c';
  else if( actviewer->getId() == portsviewer )
    vid = 'p';
  else
    vid = '-';

  for( k=0; k<numNotes; k++ )
  {
    if( notes[k]->getViewer() == vid )
      notes[k]->display();
  }

}// displayNotes

// put in for debugging, decided, why not leave it, in case?
int Lesson::getNumNotes()
{
  return numNotes;
}

// install the newly pointed to location, if possible
void Lesson::relocateNote( mystring* newlocstring )
{
  if( (focus->whichKind() != notebox  &&
       focus->whichKind() != parbox )
       || 
       newlocstring == NULL )
  {// lesson focus is not on a notebox, so refuse to
    // continue
    std::cout << 
      "lesson focus not on notebox or no location found" 
        << std::endl;
  }
  else
  {// install newlocstring in the focus box

    box* locbox = focus;
    
    if( locbox->whichKind() == parbox )
      locbox = locbox->whoOuter();

    // now locbox is the notebox

    locbox = locbox->accessInnerAt( 1 ); // now is the location parbox
  
    locbox->storeName( newlocstring );
    newlocstring->destroy();  // storeName makes a copy

    locbox->fixupReality(0);  // string changed size

    badNotes = 1;  

  }
}// relocateNote

void Lesson::updateNotes( float cvw, float cvh, float pvw, float pvh )
{
  if( badNotes )
  {
    int k;

    for( k=0; k<numNotes; k++ )
    {
      notes[k]->update( universe, cvw, cvh, pvw, pvh );
    }
  }
}
