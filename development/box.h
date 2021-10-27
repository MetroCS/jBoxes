// interface for the box class

class box
{
private:

  // this guy---and all other "trace" things---can be removed to
	// improve efficiency
  // jb23:  remove tid from all boxes, hasn't really been there
  //        for a long time, anyway
//	int tid;  

  int kind;           // kind of box I am

  char aspect;  // which aspect currently showing, 'a' 'd' 'f'
                // and, for some kinds, possibly 'v'

  int color;    // my natural color
  int dye;      // the color I am currently dyed to
      // both these colors are for background layer, other stuff
      // is color depending on aspect, or its own boxy color
  int depth;    // increases by 1 for each ancestor, universe is 0
                // 
  char inside;  // whether inside or not 'y' 'n' on the current aspect
  char part;   // which part of full aspect is active 'n' 't' 'v' 'i'

  // jb18:  code up various properties in props, bit 0 is
	//        whether breakpoint is set or not
  // jb24:  bit 1:  permission to kill this box
  //        bit 2:  permission to insert in this box
  //        bit 3:  permission to change views of this box
	int props;

  // boxy stuff
  float ulcx, ulcy;  // my offset in outer box, when inners showing
  long width, height;  // my size (rounded to nearest long)
  box *next;         // my next box
  box *outer;        // my outer box
  box *first;        // my first inner box
  box *current;      // my current inner box
  int cursor;        // index of my current inner box
                     // 0 means inside but focused on marker,
                     // 1 is first actual inner box

  // part stuff
  mystring *name;       // holds my name, used in abstract part
                     // which shows by itself, and in handle
                     // part, which shows in full view
                     //  along with type and dim
  
  mystring *type;       // type of data I hold if I'm a data box,
                      // followed by nothing, [] or [][]
                      // as in:  int int[] int[][]

  mystring *value;      // my value, if I'm a box that has a value

  mystring *doc;        // my documentation, shows only in comment
                     // part

  // execution stuff
  int state;  // my current execution state

  box *source;  // points back to an important box for my execution

public:

  box();

  void coredestroy(); 
  void destroy();
  void build( int knd );
  box* addGridboxToHeap( box* u,
                   int numrows, int numcols, int& status );
  box* accessArrayElement( int row, int col, int& status );
  void load( FILE* infile );
  void save( FILE* outfile );
  void moveInside();
  void moveOutside();
  void setPart( char which );
  void setAspect( char which );
  box* cycleAspects( int& didit );
  box* jumpParts( int& didit );
  float howWide();
  float howHigh();
  void drawsymbol0( float x, float y, int displaylevel,
                     float& w, float& h );
  void getAFInfo( int doit, mystring* s, float ulx, float uly,
                      int newlines,
                     float& afx, float& afy,
                     float& afw, float& afh );
  void specialGetAFInfo( int symbol0Choice,
	                 int doit, mystring* s, float ulx, float uly,
                      int newlines,
                     float& afx, float& afy,
                     float& afw, float& afh );
  box* sizeLocDisplay( int locate, int& changed, 
	    int draw, 
    float x, float y, 
    float left, float right, float top, float bottom,
    int showfocus, box* focusbox,
           int find, float targetx, float targety,
           char& partnumber, float& innerx, float& innery,
           int getafinfo, float& afx, float& afy,
                          float& afw, float& afh       );
  void figureSizeLocs( int& modified );
  void display( float ulcx, float ulcy, 
                 float left, float right, float top, float bottom,
                 int showfocus, box* focusbox );
  void texdisplay();
  box* findLocation( float offsetx, float offsety,
                      char& whichpart, 
                      float& partx, float& party );
  void setDataToLocation( char whichpart, float partx, float party );
  void findSymbolInPart( char whichpart, float partx, float party,
                              int& index );
  int positionInInnerList();
  void simulateMove( box* newfocus );
  void setCurrentOutward();
  box* setFocusToUniv( int& didit );
  void findApparentOffset( float& inx, float& iny, 
                           float& inw, float& inh );
  box* findBackBox( float viewwidth, float viewheight,
           float viewx, float viewy, 
           box* limitbox,
           float& backx, float& backy );
  int findOffsetOutward( box* targetbox, 
                           float& inx, float& iny );
  void fixupReality( int dontstopearly );
  mystring* whichString();
  char whichAspect();
  box* moveNext( int& didit );
  box* findPrev();
  box* movePrev( int& didit );
  box* accessInnerAt( int index );
  int innerLength();
  box* moveOut( int& didit );
  box* moveIn( int& didit );
  box* moveUpDown( char dirn, int& didit);
  box* moveHomeEnd( char dirn, int& didit);
  box* whoOuter();
  void setDye( int colorcode );
  void setDepthAndColors( int colorcode, int dpth );
  void addInnerAtCursor(  box* newbox );
  void addInnerAtFront(  box* newbox );
  box* processSpace( int key, int& didit );
  box* processPrintable( int sym, int& didit );
  box* removeFromInner(int key, int& didit);
	box* wipeAspect(int& didit);
  int isTrue();
  mystring* whatName();
  int whichKind();
  box* whoNext();
  box* whoFirst();
  int whichState();
  void setGLColor();
  float zValue();
  void setState( int s );
  int isInside();
  int acceptEnterkey();
  box* copy();
  box* copyMethodbox( int whichkind );
  box* xerox( box* clipbox, int& didit );
  box* glue( box* clipbox, int& didit );
  box* findCodebox();
  box* findStack();
  box* findCallStart();
  box* findCover();
  box* returnFromCall( box* ilist[], int cclist[], int& ni );
  void removeFirst();
  box* findInHeap( box* u, mystring* address );
  box* searchInner( mystring* s );
  void storeValue( mystring* s );
  void storeType( mystring* s );
  void storeName( mystring* s );
  void storeDoc( mystring* s );
  void storeInputString( char* inputstr,
	                         box* ilist[], int cclist[], int& ni );
  box* makeInstance();
  box* addInstanceToHeap( box* u, int& status );
  int actionContext( int oknd, int slot );
	int isAction();   // jb16 just convenience call to actionContext
  void removeAllInners();
  void clearOutStackAndHeapAndStatic();
  void clearOutPorts();
  int valueNotInit();
	int typeNotInit();
  box* nextToCheck( char mode );
  void checkValueType( int& status );
  int storageCompatible( box* secondbox );
  box* staticCheck( int& status );
  void stripValue();
  void stripAllBranchboxCondValues();
  box* accessPort( box* u, int portNumber );
  void loadBuffer( int whichPort);
	void saveBuffer( int whichPort);
  int haveIntValue(long& x);
  int haveFloatValue(double& x);  
	box* evaluate( box* u, int getvalue, int& status,
	                  box* ilist[], int cclist[], int& ni );
  int numberUsed();  
  void changeAllToFull( int& didit );
  void changeAllToDoc( int& didit );
	int exportToJava();
	void translateToJava( FILE* javafile, int indent );
  mystring* determineReturnType();
  mystring* searchForReturnbox();		
  int legalAssGrowSeq( int justAss );
  void translateInlineToJava( FILE* javafile );
	box* determineIdSource();
  void portsDisplay( float x, float y );
	void copyClassData();
	void replaceInnerAt( box* newbox, int pos );
	void promoteInnerValue();  // jb16:  special for javabox 
  int translateJava();
  void remove();
	box* findInstance();
	int isBreakpoint();   // jb18
	void toggleBreakpoint( int& didit );  // jb18
	box* rewind();

  // jb21:  new methods related to lessons
  void moveCurrentTo( int position );
  box* getComboxAtFocus();
  void appendToCommand( char* s );
  box* getLessonbox();

  // jb23:  save/load to/from file
  int exportToFile();
  int buildFromFile();

  // jb24:  permission related methods
  void setPerm( char sym );
  void setPermInward( char sym );
  int canKill();
  int canAdd();
  int canChange();
  int isAllowed( int bit );

  // jb25:  box can spell out how to get to it
  mystring* getBoxLocString();
  mystring* getLocString( int useMouseInfo, char tpart, int tindex );
  char getInfoFromLocString( mystring* locstring,  mystring* style,
                      float& ax, float& ay, float& aw, float& ah,
                      char& astyle,
                      float cvw, float cvh, float pvw, float pvh );
  void locatePartOrSymbol( char whichpart, int index,
                 float& inx, float& iny, float& aw, float& ah,
                 int& error );

  // jb31:  can find a box from the universe and a string
  box* findBoxFromLocString( mystring* locstring );
    // utilities for finding box from loc string
    box* moveInwardToNamedInner( mystring* word );
    box* moveInwardToIndexedInner( mystring* word );
    void moveInwardToPart( char newpart, mystring* word );

};
