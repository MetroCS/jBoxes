// header for Lesson entity

// key codes for lesson interactions
const int authorizeKey = 65;        // A
const int enableAuthoringKey = 1;  // ctrl-a

const int disableAuthoringKey = 113;  // q
const int pauseAuthoringKey = 112;    // p
const int recordToPlayAuthoringKey = 114;  // r for record, authoring
const int recordToPPlayAuthoringKey = 115;  // s for step record
const int recordToExpectAuthoringKey = 101;  // e for expect record
const int moveAuthoringKey = 109;  // m to move
const int editLessonKey = 108;  // l for lesson edit
const int createNoteKey = 110;  // n for create a note
const int dismissNotesKey = 100;  // d for dismiss notes
const int pointingKey = 46;  // . for pointing

// jb27:  used to call this runLessonKey, now "run" means for author,
//        "read" for reader
const int fastForwardLessonKey = 102;   // f for fast forward
// reader hits escapeKey to read a lesson or move on in pplay
const int quitLessonKey = 17;  // ^q for quit running lesson

// internal states for the lesson
const int disabledLessonState = 0; // not authoring
const int authoringLessonState = 1;// neutral authoring state
const int pausedLessonState = 2;   // recording symbols for pause(
const int recordingLessonState = 3;// recording for play, etc.
const int movingLessonState = 4;   // moving to locate a desired focus
const int editingLessonState = 5;  // manually editing lesson
const int readingLessonState = 6;  // reading a lesson (reader or 
                                   //  fast forwarding author)
const int stoppingLessonState = 7;
// internal command states
const int doneCommandState = 8;
const int pauseCommandState = 9;
const int playCommandState = 10;
const int pplayCommandState = 11;
const int expectCommandState = 12;
const int errorCommandState = 13;
const int neutralCommandState = 14;

#include "noteinfo.h"  // provide access to noteinfo class

class Lesson
{
  public:

    // constructor
    Lesson( box* ubox, viewer* iv );

    // methods
    void toggleAuthority();
    int isAuthor();
    // given the lessonbox and
    // lessonviewer to work with, enable authoring
    void enableAuthoring( box* lb, viewer* lv );

    int getState();
    box* getBox();

    void disableAuthoring();
    void pausedToAuthoring();

    void authoringToPaused();
    void authoringToRecording( char* method );
    void authoringToMoving();

    void recordingToAuthoring();
    void movingToAuthoring( box* fbox );    
    void authoringToEditing( int av );
    void editingToAuthoring( int& av );

    // ff tells whether fast forwarding or plain reading
    void startReading( box* lbox, viewer* lv, int ff,
                         MessageQueue* mq );
    int shouldStopReading();
    void stopReading();
    void stopStopping();

    void relocateNote( mystring* newlocstring );

    int getCommandState();

    // scan ahead in simulated key code sequence given by current
    // argument string in lessonbox, return next key code
    int nextKey();

    // store symbol or escape sequence for kind k, code c in the
    // combox where the lesson focus is
    void record( int k, int c );

    // note-related methods

    void addNote( box* nbox );

    void removeNote( box* nbox );

    void prepareNotes( viewer actviewer );
    void displayNotes( viewer* actviewer );
    void dismissNotes( int recording );

    void updateNotes( float cvw, float cvh, float pvw, float pvh );

    void invalidateNotes();
    void validateNotes();

    int getNumNotes();

    void putback( int key );

    void setActionStatus( int s );
    int getActionStatus();
    int shouldDoAnother();
    int shouldRedraw();

    void processMessage( int& mk, int& mc, MessageQueue* mq );
    void readOn( MessageQueue* mq );
    int processNextStoredKey( int& mk, int& mc );

  private:

    box* universe;  // universal box for convenience in determining
                    // absolute coordinates

    int authorized;  // tells whether author or reader

    int numNotes;  // current number of active notes
    noteinfo* notes[maxNotes];

    viewer* lessonViewer;
    box* lessonBox;  // the lessonbox for this lesson

    // jb31:  think lesson needs to have access to this guy
    viewer* interactiveViewer;

    int formerActiveviewer;  // when switch to lessonviewer must 
                             // remember activeviewer for restoration

    int badNotes;  // tells whether notes need preparation before
                   // display

    box* focus;  // convenience to hold lessonViewer's focus
                 // use focus, update to/from lessonViewer when
                 // transition to/from editing state, and when
                 // enable authoring

    int state;  // the state the lesson is in, namely
                // disabled, authoring, pausing, recording, moving,
                // editing, reading

    // utilities for recording, playing
    int repChar;
    int repCount;
    int lineCount;
    int indent;

    int putbackKey;  // 0 if none, else stores same int code as
                     // get from nextKey
  
    int actionStatus;  // tells whether standard code should be done

    int doRedraw;  // tells whether lesson wants another display

    int commandState;  // what command is currently running

    box* stopBox;      // the box to fast-forward to, or NULL if none

    int firstPlayBack;  // tells whether interactively playing back 
                        // first key and therefore need to dismiss 
                        // notes

    int doStoredKey;  
};
