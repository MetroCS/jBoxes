// provide access to data and functions defined in the main file

extern int snapshotnumber;  // sequence number for snapshot file naming

// change later as window is resized
extern int screenwidth;
extern int leftscreenwidth;
extern int screenheight;
extern int consolewidth, consoleheight;

extern float physwidthsym[];  //actual width of each sym
extern float widthsym[];  // display width, includes padding
                             // on both sides
extern float padsym[];  // amount to pad on both sides

extern float maxcharwidth;  // physical width of widest symbol

extern int needredraw;  // use to completely turn off display on next doit
extern int needrefocus;

extern int touring;  // flags whether we are touring while paused
              // when touring, have no focus

extern int texdraw;  // whether drawing to screen or to texfile
extern int pictex;
extern char texfilename[];   // the TeX file

extern FILE* texfile;

extern char univfilename[];
extern FILE* univstream;

extern int actionmode;  // current mode of operation
extern int savemode;    // when go to errormode, need to be able to restore

extern int pausetoinspect;  // flag for running or stepping
extern int splitviewing;      // flag for showing single viewer or 3 while
                    // executing

// v11:
extern int userview;

extern long currentHeapAddress; // as boxes are new'd, just keep 
                            // incrementing

// ==================================================================
extern int mousestatus;  // 0-->no click, 1-->left click, 2-->right click
extern float mousex, mousey;  // coords of left click, if any
extern float mouseupx, mouseupy;  // coords of left release
extern int mouseviewer;  // remember which viewer received the press,
                  // so can ignore the release unless it's in
                  // the same viewer
extern float mouseoffx, mouseoffy;
extern float tinymousedistance;

extern int inputbuffersize;
extern char inputstring[];
extern int inputlength;

//==================================================================
// utility functions

int legalsymbolcode( int x );
void psScale( float xs, float ys );
void errorexit( char message[] );
extern int debugcount;
extern int debughalt;
void debug(int threshhold);
extern int traceId;  // unique id for each entity traced
extern int traceIndent; // amount currently indenting
extern int traceSearchId; // set manually to debug halt in correct
extern char traceSearchMethod[];  //  box method
extern long traceStep; // monitor how many calls to traceEnter, allows
                    // easier stopping
extern long traceThreshhold;

void traceInit();

void traceHalt();
void traceEnter( int id, char label[] );
void traceLeave();
extern char errormessage[];

void beep();
float myabs( float x );
float mymax( float x, float y );
float mymin( float x, float y );
long myround( float x );
long badFirstSymbol( char ch );
void psLine( float x1, float y1, float x2, float y2 );
void psMoveTo( float x1, float y1 );
void psTranslate( float x1, float y1 );
void psLineTo( float x1, float y1 );
void psStroke();
void psTriangle( float x1, float y1, float x2, float y2,
                 float x3, float y3 );
void psRectangle( int filled, 
                  float ulx, float uly, 
                  float lrx, float lry );
void psStartPath( float x, float y );
void psEndPath( int filled );

void pictexLine( float x1, float y1, float x2, float y2 );
void pictexRect( float x1, float y1, float x2, float y2, char hollow );
void pictexTriangle( float x1, float y1, float x2, float y2, 
                       float x3, float y3 );
void displayerrormessage();
int unsignchar( char x );
extern int LEDdigits[];
extern const float segsize;
void drawLEDsegment( float x, float y, int segment );
void drawLEDdigit( float x, float y, int digit );
void drawsymbol( float x, float y,
                     int sym,
                     int showSpecials );
void drawTypeSymbol( float x, float y, int litcode );
void drawspecialsymbol( float x, float y,
                     int sym,
                     int showSpecials );
void drawarrow( float x1, float y1, float x2, float y2,
			      float color[] );
void drawflowarrow( float x1, float y1, float x2, float y2,
				       float color[] );
void drawline( float x1, float y1, float x2, float y2, 
			     float color[] );
void drawparen( float cx, float cy, float h, float leftright,
			      float color[] );
void drawbracket( float cx, float cy, float h, float leftright,
				    float color[] );
int insideRect( int find, 
                 float ulx, float uly, float lrx, float lry, 
           float targetx, float targety,
                    float& innerx, float& innery );
int outsideRect( float ulx, float uly, float lrx, float lry, 
           float targetx, float targety );
int legalinname( int sym );
int legalinoperator( int sym );
int coercibletofloat( char x );
int numeric( char x );
  mystring* storeString( mystring* target, mystring* source );
  mystring* storeCharString( mystring* target, char* str );
  char* builderrormessageCBC( char* first, mystring* second,
                               char* third );
  mystring* concatSCS( mystring* a, char c, mystring* b );
  int isprimtype( mystring* s );
	int isLegalType( mystring* s );  // jb18
  int isReserved( mystring* s );
  mystring* build4s( int x );

// build constant label mystrings to be pointed at by many
// boxes
extern mystring* classdatastring;
extern mystring* classmethodsstring;
extern mystring* instancedatastring;
extern mystring* instancemethodsstring;
extern mystring* paramsstring;

extern mystring* retvalstring;          //v10
extern mystring* allclassdatastring;

extern mystring* localsstring;
extern mystring* stackstring;
extern mystring* heapstring;
extern mystring* consolestring;
int actionkind( int k );
int literalkind( int k );
int valuekind( int k );

// global box variables 
extern box* universe; // pointer to the univbox
extern box* clipbox;  // global clipbox
extern box* stack;
extern box* heap;
extern box* staticdata;
extern box* console;
extern box* errreturn;

void requesterrormessage(char s[], box* retbox );
void informOfSnapshot( box* focus );

void errorexit( char message[] );// holds all constants == any variables that can be
//   included in any file, because have same value throughout

int base36ToInt( char x );
