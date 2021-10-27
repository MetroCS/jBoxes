#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <glut/glut.h> // Mac OS X
//#include <gl/glut.h>
//#include "glut.h"

#include "mystring.h"
#include "box.h" 
#include "viewer.h"
#include "translator.h"
#include "MessageQueue.h"

MessageQueue* mq;  // main message queue
MessageQueue* smq;  // high priority queue for execution step messages

// jb21
#include "Lesson.h" // header for class Lesson
Lesson* lesson;  // big global, contains all for lesson facility

int authorVersion = 1;  // change to 0 to make into a student/reader
                        // version of code, 1 to make author version

// make one copy here of guys in "constants.h" and then in
// that file, give access to same guys as extern
//******************************************************************
//   the real guys are here, elsewhere they're copies
//******************************************************************

// allow TeX snapshots?
  int TeXauthorized = 1;  // allow snapshots to files
	// jb16:  phase out pictex option completely, leave
	//         drawing code in case, plus lots of work to
	//         pull it out!
	int pictex = 0;  // 1 means do output of drawing in pictex format,
	                 // 0 means do postscript
									 // v11:  -1 means option not chosen yet

const  int inputbuffersize = 80;

// ****************************************************************
// ************************* colors *******************************
// ****************************************************************

// define opengl color arrays and corresponding colorcode ints

// color codes are stored in both the color and dye members of box,
// so need a code meaning "no color" in dye so natural color can
// come out
  int nocolorcode = 0;

// neutral is the color of stuff that is not viewed as needing any
// special color, like the universe, the stack, the heap, the console
//  --- it's the classes, new'd boxes, stack frames, that have
//      special colors indicating their semantics --- the neutral
// guys are just containers
float neutralcolor[3] = {0.95, 0.95, 0.95 };
  int neutralcolorcode = 1;

// blueprint is the color of the static class stuff
float blueprintcolor[3] = {0.9, 0.9, 1}; 
  int blueprintcolorcode = 2;

// copy is the color of the method copies that are put in the 
// stack box
float stackcolor[3] = {1,0.9,0.9};
  int stackcolorcode = 3;

// guys created on the heap have their own special color---
//  object color!
float heapcolor[3] = {0.9, 0.95, 0.9};
  int heapcolorcode = 4;

float staticdatacolor[3] = {1.0, 1.0, 0.8};
  int staticdatacolorcode = 14;

// the focus (actually the interactive focus) has a vivid color
// so's we can spot it quickly
float focuscolor[3] = {1, 1, .5};
  int focuscolorcode = 5;

// for inspection list --- color of an action box whose initial
// signaling is interesting (probably not an assignment box,
// where the right hand side value appearing will be plenty)
float signalcolor[3] = {0.95, 0.75, 0.95};
  int signalcolorcode = 6;

// for inspection list --- color of a newly changed value
float newvaluecolor[3] = {1.0, 0.85, 0.7};
  int newvaluecolorcode = 7;

// for inspection list --- color of a just created instance on heap
float newobjectcolor[3] = {0.5, 0.95, 0.95};
  int newobjectcolorcode = 8;

// special tracing color --- leave call boxes that are waiting for
// value to return this color
float waitcolor[3] = {0.5, 0.95, 0.5};
  int waitcolorcode = 9;

// when execution stops with an error box, the box identified as
// holding the error is set to special errorcolor, just for a while
float errorcolor[3] = {1.0, 0.85, 0.0};
  int errorcolorcode = 10;

// the console has a special background color
float consolecolor[3] = {1.0,1.0,1.0};
  int consolecolorcode = 11;

// and the cursor background is special (the drawing color in
// the console is permanently set at black)
float consolecursorcolor[3] = {0.0, 1.0, 0.0};
  int consolecursorcolorcode = 12;

// when execution reaches end,
float successcolor[3] = {0.85, 0.85, 1.0};
  int successcolorcode = 13; 

float stoplessoncolor[3] = {1,0.6,0.6};
  int stoplessoncolorcode = 14;

// the textcolors are automatic, permanent, depend only on
// the aspect showing --- codes not needed, because these
// are not stored anywhere, except indirectly through aspect of box
float abstextcolor[3] = {0.0, 0.0, 1.0};
float doctextcolor[3] = {0.0, 0.5, 0.0};
float fulltextcolor[3] = {0.0, 0.0, 0.0};
float valuetextcolor[3] = {1.0, 0.0, 0.0};

// jb19:  need special vivid color for note arrows and border
float notearrowcolor[3] = { 1.0, 0.314, 0.078};

int messagecolorcode; // global to communicate which color for
                      // "error"messages

// ****************************************************************
// *********************** symbol stuff ***************************
// ****************************************************************

  int trianglecode = 128;  // code numbers for special symbols
                         // didn't really follow this scheme, note!
  int littlepagecode = 129;
  int absimagecode = 130;
	int boxedjavacode = 131;

	// jb16:  add tabsymbol, bump up minprogcode to 133
  int tabsymbolcode = 132;

  int minProgCode = 133;  // minimum programmable char code

  int endoffilecode = 256;  // special symbol that doesn't conflict
                          // with 0..255 or -128..127,
													// isn't really a symbol, but can display

  int nextkey = 14;
  int prevkey = 16;
  int backspacekey = 8;
  int deletekey = 127;

  int killkey = 11;   

  int wipekey = 23;

  int enterkey = 13; 

  int tabkey = 255;  // jb16 --- key code that misses all others

  int jumpkey = 10;
  int toggleviewskey = 3;  

  int upkey = 21;
  int downkey = 4;

  int firstkey = 6;
  int lastkey = 12;

  int inkey = 9;
  int outkey = 15;

  int specialspacekey = 27; // alternative space key
  int escapeKey = 27;  // alternate name for esc key

  int xeroxkey = 24;
  int gluekey = 7;

  int showkey = 19;
  // jb23:  change mnemonic from "tell" to "remarkize"
	int tellkey = 18;

// v11: int exporttojavakey = 5;

  // jb18
	// jb19:  used f3 instead
//	int breaktogglekey = 2;

// v10: key codes for alt meta keys are regular codes plus 1000 or 2000
//       v11: various export ops
  int exporttojavakey = 106; // j plus kind will be alt
	int snapshotKey = 115; // s plus kind will be alt

// jb23:  change alt-t to ctrl-t for translate
//         then change "tell" mnemonic to "abstractify" 
	int translatekey = 20; 

// jb23:  new features---export and build
  int exportkey = 5;
  int buildkey = 2;

// new version of symbol dimensions
  float typsymwidth = 8; // makes for 80 across
  float typsymheight = 16;
  float typsymkern = 1;

// is the permanent,  ant distance between baselines of
//  successive lines in a doc aspect
  float baseline = 18;  

const int maxsymcode = 257;  // 0..255 plus 256

  float minpadsym = 1.5;

  float symdepth = 4;

  float hpad = 3.0, vpad = 3.0;  // padding between boundaries
                                     // and parts
          // was 2, 2
  float vbpad = 10;  // viewer border pad 

  float arrowpad = 12;  // standard arrow length

  float arrowheadwidth = 3;
  float arrowheadheight = 5;

  float flowarrowpad = 6;  // smaller arrow length

// ***************************************************************
// ************************** depth info *************************
// ***************************************************************

/*  the orthographic viewing volume is from minz to maxz, so all
     depths actually used should be safely in between --- actual
	 amounts don't matter with orthographic projection, just the
	 < relationships

      a box's display depth is determined by its depth, with
	  the universe at univz, working toward z=univz+1 at larger depths

      all non-background pieces of a box are displayed at textz---
	  none of this material is ever obscured by a box on top

      the error box is displayed way out in front, at errorz
*/

  float minz = -1.0;
	float notetextz = -1.2; // jb19
	float notez = -1.3;   // jb19:  notes go in front of all
	float breakz = -1.5;  // jb18 level for breakpoint image
  float textz = -2.0;
	  float savetextz = -2.0;  // jb19:  for temp change to textz
		          // must match textz
  float errorz = -3.0;
  float textfocusz = -4.0;
// box backgrounds display from univz to univz+1,
// (asymptotically hitting -5), depending on their depth
  float univz = -6.0;
  float maxz = -7.0;

// ***************************************************************
// *************************** codes *****************************
// ***************************************************************

  int methodnamenotvalueerror = -1;
  int typesdontmatcherror = -2;
  int firstemptysecondnoterror = -3;
  int classnotfounderror = -4;
  int typenotfounderror = -5;
  int triedtonewprimerror = -6;
  int illegalassoperror = -7;
  int cantplusequalobjecterror = -8;
  int incomptypeserror = -9;
  int divbyzeroerror = -10;
  int nofloatmoderror = -11;
  int datanotiniterror = -12;
  int cantjumpemptyerror = -13;
  int idnotfounderror = -14;
  int valuetypeincompaterror = -15;
  int duplicateclassnameserror = -16;
  int duplicatemembernameserror = -17;
  int canthaveemptynameerror = -18;
  int missingtypeerror = -19;
  int parammustbeemptyerror = -20;
  int badprimvalueerror = -21;
  int objectrefnotemptyerror = -22;
  int duplicatelocalnameserror = -23;
  int illegalgrowoperror = -24;
  int notintegererror = -25;
  int nosuchclassmembererror = -26;
  int nosuchinstancedataerror = -27;
  int namecantbereservederror = -28;
  int notvalidheapaddresserror = -29;
  int emptyidnameerror = -30;
  int illegalmethodreferror = -31;
  int argsnotequalparamserror = -32;
  int incompatargparamerror = -33;
  int nosuchinstancemembererror = -34;
  int canthavemethodreferror = -35;
  int mustreturnvalueerror = -36;
  int argnotlegalinterror = -37;
  int negsizerequestederror = -38;
  int nosuchtypeerror = -39;
  int notagridboxerror = -40;
  int nonemptysecondfor1derror = -41;
  int colindextoolargeerror = -42;
  int rowindextoolargeerror = -43;
  int illegaloperatorerror = -44;
  int illegaloperanderror = -45;
  int cantcoercetofloaterror = -46;
  int invalidbooleanerror = -47;
  int badbifnameerror = -48;
  int need2argserror = -49;
  int illegalcursorlocationerror = -50;
  int need1argerror = -51;
  int nosuchcharerror = -52;
  int indextoolargeerror = -53;
  int neednoargserror = -54;
  int cantbearrayerror = -55;
  int negativeargerror = -56;
  int notastringerror = -57;
  int notfloatformerror = -58;
  int portrangeerror = -59;
  int openerrornot3args = -60;
  int openerrorportbusy = -61;
  int openerroriochoice = -62;
  int portnotreadingerror = -63;
  int endoffileerror = -64;
  int portnotwritingerror = -65;
  int notcharerror = -66;
  int portnotopenerror = -67;
  int badliteralformaterror = -68;
  int notintegerformerror = -69;
  int badstringindexerror = -70;
  int badfirstsymbolerror = -71;
  int badtargetlocationerror = -72;
	int argoutofrangeerror = -73;
	int notfloaterror = -74;
	int notachararrayerror = -75;
	int progcharrangeerror = -76;
	int badprogcharstringerror = -77;
	int returningwrongtypeerror = -78;
  int intrangeerror = -79;
	int notininstancemethoderror = -80;
	int nosuchinstanceerror = -81;
	int erroralreadyrequested = -82;
	int notarraytypeerror = -83;
	int illegaltypeerror = -84;

// operator codes (includes built in "methods")
  int noopcode = 0;  // nonexistent or illegal operation
  int eqcode = 1;  // relational ops
  int ltcode = 2;
  int gtcode = 3;
  int lecode = 4;
  int gecode = 5;
  int necode = 6;
  int pluscode = 7;  // arithmetic ops
  int minuscode = 8;  
  int timescode = 9;
  int divcode = 10;
  int modcode = 11;
  int andcode = 12;  // logical ops
  int orcode = 13;
  int notcode = 14;  // unary ops
  int oppcode = 15;

  int moveTocode = 16;
  int printcode = 17;
  int charcode = 18;
  int getcode = 19;
  int getLinecode = 20;
  int clearcode = 21;
  int randomcode = 22;
  int abscode = 23;
  int sqrtcode = 24;
  int sincode = 25;
  int coscode = 26;
  int atancode = 27;
  int floorcode = 28;  // was round
  int ceilcode = 29;    // was truncate
  int intToStringcode = 30;
  int isIntcode = 31;
  int isFloatcode = 32;
  int toIntcode = 33;
  int toFloatcode = 34;
  int lengthcode = 35;
  int charAtcode = 36;
//   int copycode = 37;
  int opencode = 38;
  int closecode = 39;
  int fgetcode = 40;
  int fputcode = 41;
  int eofcode = 42;
  int tancode = 43;    
	int asincode = 44;
	int acoscode = 45;
	int expcode = 46;
	int logcode = 47;
	int seedcode = 48;
	int floatToStringcode = 49;
	int charToStringcode = 50;
	int charsToStringcode = 51;
  int progCharcode = 52;
	int whatRowcode = 53;
	int whatColcode = 54;
	int hideCursorcode = 55;
	int showCursorcode = 56;
	int haltcode = 57;
	int asciicode = 58;
	int nullcode = 59;    // jb17 --- 3 new bifs
	int thiscode = 60;
	int destroycode = 61;
    int timecode = 62;  // jb2006a:  adding _time
// states of execution
  int initialstate = 0;
  int waittastate = 1;  
  int waittpvstate = 2;
  int waittplstate = 3;
  int waittocallstate = 4;
  int waittoreturnstate = 5;
  int returnstate = 6;
  int waittogetinputstate = 7;
	int javawaitstate = 8;        // jb16

  // the modes of operation:
    int interactivemode = 1;
    int pausedmode = 2;
    int executingmode = 3;
    int errormode = 4;
    int input1mode = 5;
    int inputlinemode = 6;
    int haltmode = 7;
		int consoleviewmode = 8;  // v11
		int pausedconsoleviewmode = 9;
		int runtobreakmode = 10;  // jb18

// jb21:  do it differently!
//		int lessoneditmode = 11;   // jb19
//		int lessonrunmode = 12;   // jb19

  int interactiveviewer = 0;
  int executionviewer = 1;
  int consoleviewer = 2;
  int portsviewer = 3;
	int userviewer = 4;//v11: add full screen view of just console box
	int lessonviewer = 5;  // jb19

// kinds of boxes
  int databox = 1;
  int univbox = 2;
  int classbox = 3;
  int classdatabox = 4;
  int classmethodsbox = 5;
  int instancedatabox = 6;
  int instancemethodsbox = 7;
  int methodbox = 8;
  int paramsbox = 9;
  int localsbox = 10;
  int seqbox = 11;
  int emptybox = 12;
  int branchbox = 13;
  int dobox = 14;
  int whilebox = 15;
  int forbox = 16;
  int idbox = 17;
// deprecated   int literalbox = 18; // deprecated --- 
  int callbox = 19;
  int memberbox = 20;
  int arraybox = 21;
  int returnbox = 22;
  int opbox = 23;
  int unopbox = 24;
  int assbox = 25;
  int growbox = 26;
  int array2dbox = 27;
  int newopbox = 28;
  int classmethodbox = 29;
  int instancemethodbox = 30;
  int stackbox = 31;
  int heapbox = 32;
  int instancebox = 33;
  int consolebox = 34;
  int valuebox = 35;   // cell in a gridbox on heap
  int gridbox = 36;
  int portsbox = 37;
  int portbox = 38;
  int intbox = 39;    // these 5 replace literalbox
  int floatbox = 40;
  int charbox = 41;
  int booleanbox = 42;
  int stringbox = 43;
	int retvalbox = 44;  //v10
	int allclassdatabox = 45; // v10:  add static data box
	int classdatacopybox = 46;
	int javabox = 47;          // jb16: new kind of box!

	int lessonbox = 48;  // jb19
// jb21:  add in parbox	int combox = 49;     // jb19
  int parbox = 49;
  int combox = 50;
  int presbox = 51;
  int notebox = 52;

//******************************************************************

int snapshotnumber = 0;  // sequence number for snapshot file naming

// change later as window is resized
int screenwidth=620;
int leftscreenwidth = 310;
int screenheight=380;
int consolewidth, consoleheight;

float physwidthsym[maxsymcode];  //actual width of each sym
float widthsym[maxsymcode];  // display width, includes padding
                             // on both sides
float padsym[maxsymcode];  // amount to pad on both sides

float maxcharwidth;  // physical width of widest symbol


// ***************************************************************
// *************** managerial global variables *******************
// ***************************************************************

int needredraw;  // use to completely turn off display on next doit
int needrefocus;

int touring;  // flags whether we are touring while paused
              // when touring, have no focus

int texdraw;  // whether drawing to screen or to texfile
char texfilename[300];   // the TeX file
FILE* texfile;

char univfilename[300];
FILE* univstream;

int actionmode;  // current mode of operation
//int savemode;    // when go to errormode, need to be able to restore

// jb18:  had this global all along, wasn't being used, use it
//        now, so pausetoinspect=1 allows pausing, =0 runs without
//        pausing until hit break box
int pausetoinspect;  // flag for running or stepping

int splitviewing;      // flag for showing single viewer or 3 while
                    // executing

// v11:
int userview = 0;  // start out not just showing console view

long currentHeapAddress = 0; // as boxes are new'd, just keep 
                            // incrementing

/* jb21:  trashing the earlier jb19 lesson stuff, leave for ideas
//  =============================================================
// jb19:  add some globals for lesson facility

int author = 1;   // use to enable/disable various powers,
                  // such as editing notes while lesson is running,
									// accessing lots of strange keys
// create student version by changing author to 0

// should match screenwidth, etc., give 
int authorwidth = 620;       // jb19
int authorheight = 380;
int authorleftwidth = 310;

int authorscaling = 0;  // jb19 use special uniform size
int lessonrunning = 0;  // jb19 a lesson is currently running
int dolessonstep = 0;   // manages whether want standard or lesson
                        //  step in doit
int lessonmode;   // jb19:  what sort of lesson work is happening

int lessonmouse;  // lets lesson control mouse response
// ==================================================================
jb21 */

int activeviewer;

/* jb21:  trash some more jb19 stuff
box* thelesson;  // remember the original lessonbox

#include "notegroup.cpp"
notegroup* notes; // jb19:  holds active notes
jb21 */

// ==================================================================
int mousestatus;  // 0-->no click, 1-->left click, 2-->right click
float mousex, mousey;  // coords of left click, if any
float mouseupx, mouseupy;  // coords of left release
int mouseviewer;  // remember which viewer received the press,
                  // so can ignore the release unless it's in
                  // the same viewer
float mouseoffx, mouseoffy;
float tinymousedistance = vbpad;

// v11:  last minute globals in attempt to patch right click shifting
float notemousex;
float	notemousey;

char inputstring[inputbuffersize];
int inputlength;

//==================================================================
// utility functions

int legalsymbolcode( int x )
{
  if( 0<= x && x<=255 )
    return 1;
  else
    return 0;
}

void psScale( float xs, float ys )
{
  fprintf( texfile, "%g %g scale\n", xs, ys );
}

void errorexit( char message[] )
{
  std::cout << message << std::endl;
  exit(0);
}

int debugcount = 0;
int debughalt = 0;
void debug(int threshhold)
{
  if(debugcount > threshhold )
  {
    debugcount = 0;
    std::cout << "debug pause";
  }
  debugcount++;
}

char errormessage[600];

void beep()
{
  std::cout << (char) 7;
}

float myabs( float x )
{
  if(x>=0)
    return x;
  else
    return -x;
}

float mymax( float x, float y )
{
  if( x < y )
    return y;
  else
    return x;
}

float mymin( float x, float y )
{
  if( x < y )
    return x;
  else
    return y;
}

long myround( float x )
{
  long temp;
  temp = x + 0.5;
  return temp;
}

long badFirstSymbol( char ch )
{
  if( ('a'<=ch && ch<='z') ||
	    ('A'<=ch && ch<='Z')
		)
		return 0;
	else
	  return 1;
}

// postscript drawing utilities

void psLine( float x1, float y1, float x2, float y2 )
{
  fprintf( texfile, "%g %g moveto %g %g lineto stroke\n", 
                        x1, y1, x2, y2 );
}

void psMoveTo( float x1, float y1 )
{
  fprintf( texfile, "%g %g moveto\n", x1, y1 );
}

void psTranslate( float x1, float y1 )
{
  fprintf( texfile, "%g %g translate\n", x1, y1 );
}

void psLineTo( float x1, float y1 )
{
  fprintf( texfile, "%g %g lineto\n", x1, y1 );
}

void psStroke()
{
  fprintf( texfile, "stroke\n" );
}


void psTriangle( float x1, float y1, float x2, float y2,
                 float x3, float y3 )
{
  fprintf( texfile, " newpath %g %g moveto", x1, y1 );
  fprintf( texfile, " %g %g lineto", x2, y2 );
  fprintf( texfile, " %g %g lineto", x3, y3 );
  fprintf( texfile, " closepath fill\n" );
}

void psRectangle( int filled, 
                  float ulx, float uly, 
                  float lrx, float lry )
{
  fprintf( texfile, " newpath %g %g moveto", ulx, uly );
  fprintf( texfile, " %g %g lineto", lrx, uly );
  fprintf( texfile, " %g %g lineto", lrx, lry );
  fprintf( texfile, " %g %g lineto", ulx, lry );
  fprintf( texfile, " %g %g lineto", ulx, uly );
  fprintf( texfile, " closepath" );
  if( filled )
    fprintf( texfile, " fill\n" );
  else
    fprintf( texfile, " stroke\n" );
}

void psStartPath( float x, float y )
{
  fprintf( texfile, " newpath %g %g moveto ", x, y );
}

void psEndPath( int filled )
{
  fprintf( texfile, " closepath " );
  if( filled )
    fprintf( texfile, " fill\n");
  else
    fprintf( texfile, " stroke\n");
}

// pictex drawing utilities

void pictexLine( float x1, float y1, float x2, float y2 )
{
  fprintf( texfile, "\\plot %g %g %g %g / \n", 
                        x1, y1, x2, y2 );
}

// hollowness is 'R' for hollow, 'F' for filled
void pictexRect( float x1, float y1, float x2, float y2, char hollow )
{
//  if( hollow == 'F' )
//	{// filled
//	  fprintf( texfile, "\\putbar breadth <%g> from %g %g to %g %g \n ",
//		          x2-x1, x1, y1, x1, y2 );
//	}
//	else
//	{// hollow
	  fprintf( texfile, "\\putrectangle corners at %g %g and %g %g \n",
		                      x1, y1, x2, y2 );
//	}
}

void pictexTriangle( float x1, float y1, float x2, float y2, 
                        float x3, float y3 )
{
  fprintf( texfile, "\\setlinear\\plot %g %g  %g %g %g %g %g %g / \n",
	            x1, y1, x2, y2, x3, y3, x1, y1 );
}

#include "prtsym.cpp"

// display errormessage alone on screen
void displayerrormessage()
{
  int k,n, linecount;

  glViewport(0,0,screenwidth,screenheight);
  
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0,screenwidth,0,screenheight,-minz, -maxz );

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  
//    glScalef( xscale, yscale, 1.0 );

  if( messagecolorcode == errorcolorcode )
    glColor3fv( errorcolor );
	else if( messagecolorcode == successcolorcode )
	  glColor3fv( successcolor );
  else if( messagecolorcode == stoplessoncolorcode )
    glColor3fv( stoplessoncolor );
	else
	  errorexit("illegal messagecolorcode");

  glBegin(GL_POLYGON);
    glVertex3f( 0.25*screenwidth, 0.25*screenheight, 
                                           errorz );
    glVertex3f( 0.75*screenwidth, 0.25*screenheight, 
                                           errorz );
    glVertex3f( 0.75*screenwidth, 0.75*screenheight, 
                                           errorz );
    glVertex3f( 0.25*screenwidth, 0.75*screenheight, 
                                           errorz );
  glEnd();

  // get ready to draw the message, in fullcolor
  glColor3fv( fulltextcolor );
  
  float x, y;  // keep track of where the symbol should be
  float leftmargin = 0.28*screenwidth;

  x = leftmargin;  y = 0.75*screenheight - 2*baseline;

  n=strlen(errormessage);  linecount = 0;
  for(k=0; k<n; k++)
  {
    drawprintable( x, y, 
              errormessage[k] );       

    x += widthsym[ errormessage[k] ];

    linecount++;

    if( (linecount > 20 && errormessage[k] == ' ' ) 
          ||
        errormessage[k] == '\n'
      )
    {// have more than 20 on line, one just done was a space,
      // so start a new line
      x = leftmargin;
      y -= baseline;
      linecount = 0;
    }
      
  }
  
  char bottom[] = "(press <enter> to go on)";
  n= strlen(bottom);

  x = leftmargin;  y = 0.25*screenheight + baseline;

  for(k=0; k<n; k++)
  {
    drawprintable( x, y, bottom[k] );       
    x += widthsym[ bottom[k] ];
  }

}

// convert signed char being used usually to unsigned int,
//  needed for use as index, passing in to drawsymbol
int unsignchar( char x )
{
  int code;
  if( x < 0 )
	{
	  code = x;
		code += 256;
		return code;
	}
  return (int) x;
}

int LEDdigits[] = {63, 3, 118, 103, 75, 109, 121, 7, 127, 79};

// draw specified segment of LED digit
const float segsize = 2.0;

void drawLEDsegment( float x, float y, int segment )
{
  float onex, oney, twox, twoy;

  if( segment == 0 )
  { onex = x;  oney = y; twox = x+segsize; twoy = y; }
	else if( segment == 1 )
  { onex = x+segsize;  oney = y; twox = x+2*segsize; twoy = y; }	
	else if( segment == 2 )
  { onex= x+2*segsize; oney=y; twox= x+2*segsize; twoy= y+segsize;}	
	else if( segment == 3 )
  { onex= x+2*segsize; oney=y+segsize; twox= x+segsize; 
	           twoy= y+segsize;}	
	else if( segment == 4 )
  { onex= x+segsize; oney=y+segsize; 
	  twox= x; twoy= y+segsize;}	
	else if( segment == 5 )
  { onex= x; oney=y+segsize; 
	  twox= x; twoy= y;}	
	else if( segment == 6 )
  { onex= x+segsize; oney=y; 
	  twox= x+segsize; twoy= y+segsize;}	
  else
	  errorexit("unknown segment in drawLEDsegment");
		
	if( !texdraw )
	{// draw the segment to screen 
      glBegin(GL_LINES);
        glVertex3f( onex, oney,textz);
        glVertex3f( twox, twoy,textz);
      glEnd();
	}
	else
	{// draw segment to tex file
	  if( pictex )
		{
		  pictexLine( onex, oney, twox, twoy );
		}
		else
		{
     psStartPath( onex, oney );
     psLineTo( twox, twoy ); 
     psStroke();
		}
	}
		 
}

// draw LED digit laying on its side, with llc at (x,y)
void drawLEDdigit( float x, float y, int digit )
{
  if( digit == 1 )
	{// 1 is special because violates the segment grid
	  float onex, oney, twox, twoy;
	  onex = x; oney = y+segsize/2;
		twox = x+2*segsize;  twoy = oney;
  	if( !texdraw )
	  {// draw the special line
      glBegin(GL_LINES);
        glVertex3f( onex, oney, textz );
        glVertex3f( twox, twoy, textz );
      glEnd();
	  }
	  else
	  {// draw segment to tex file
	    if( pictex )
		  {
			  pictexLine( onex, oney, twox, twoy );
		  }  
		  else
		  {
        psStartPath( onex, oney );
        psLineTo( twox, twoy ); 
        psStroke();
			}
    }
	}// draw 1 special
	else
	{// all others follow segment scheme
    int pattern = LEDdigits[digit];

    // loop through all bits and draw (or not) corresponding segment
	  int k, bit;
	  for( k=0; k<7; k++ )
	  {
	    bit = pattern % 2;     pattern /= 2;
  		if( bit )
	  	  drawLEDsegment( x, y, k );
	  }
  }// not 1
}// drawLEDdigit

char* progChars[126];

// v13:  change progChar to use 'a' to whatever, for uniformity
//   (was silly to mix, unless really expect more than 26 by 26
//     chars, which I don't --- actually only like 16 by 17
// convert ch == '0'..'9' 'a'..'z' to int
int base36ToInt( char ch )
{
  // v13:  replace this by what's below
	/*
  if( '0'<=ch && ch<='9' )
	  return ( (int) ch ) - 48;
	else
	  return ( (int) ch ) - 87; // so 'a' --> 10, etc.
	*/
	return (int) ch - 'a';  // so 'a' --> 0, etc.

}

// given ulc coords (x,y) of symbol, draw it by
// interpreting the standard C string s
//  which is correct (_progChar checked its mystring argument
//  for legality before storing into progChars as a standard string)
void drawProgChar( int x, int y, char* s )
{
  int x1, y1, x2, y2;
  int left, right, bottom, top;
	char com;

  int k=0;  // index into s
	while( s[k] != '\0' )
	{// s[k] holds command char, grab args and draw

    com = s[k];

    if( com=='L' || com=='R' || com=='F' )
		{// guys that take 4 args
      x1 = base36ToInt(s[k+1]);  y1 = base36ToInt(s[k+2]);
      x2 = base36ToInt(s[k+3]);  y2 = base36ToInt(s[k+4]);

      k += 5;

      left = x + x1;  right = x + x2;
		  bottom = y + y1;  top = y + y2;
    }
		// else if .... ever have other commands

	  if( com == 'L' )
		{// draw line
      if( !texdraw )
      {// to screen
        glBegin(GL_LINES);
          glVertex3f( left, bottom, textz);
          glVertex3f( right, top, textz);
        glEnd();
      }
      else
      {// to texfile
    	  if( pictex )
		    {
				  pictexLine( left, bottom, right, top );
		    }
		    else
		    {
          psStartPath( left, bottom );
          psLineTo( right, top );
          psStroke();
				}
      } 
			  
		}// line
		else if( com=='R' || com=='F' )
		{// draw rectangle or filled
      if( !texdraw )
      {// to screen

        if( com == 'R' )
			    glBegin(GL_LINE_LOOP);
				else
				  glBegin(GL_POLYGON);
  				  glVertex3f( left, bottom, textz );
	  			  glVertex3f( right, bottom, textz );
		  		  glVertex3f( right, top, textz );
			  	  glVertex3f( left, top, textz );
        glEnd();
      }
      else
      {// to texfile
    	  if( pictex )
		    {
				  pictexRect( left, bottom, right, top, com );
		    }
		    else
		    {
          psStartPath( left, bottom );
          psLineTo( right, bottom );
          psLineTo( right, top );
          psLineTo( left, top );

  				if( com == 'R' )
            psEndPath( 0 ); // close the loop, don't fill
		  		else
			  	  psEndPath( 1 );  // close and fill
        }
      } 
		}// rectangle or filled
			
	}// loop to process s[k]

}// drawProgChar

// a specific symbol is a font (0 for monospaced, 1 for non-mono,
//      later ones for further fonts, if I develop my own
//      stroke fonts using display lists) plus the basic code
//      sym

// draw symbol at current origin, color, scaling, etc.,
// translating by correct
// padding amounts on left and right, depending on the
// symbol, using isfocus, newlines to determine
// desired image (space, triangle, enter behave differently
// depending on these)

// new version:  takes in the LOWER left corner coordinates at
//               which to draw, returns the width cw of the
//               symbol that was drawn (drawed?)

// this guy has to do glTranslate and glScale, but restores them
//  directly by unscaling and untranslating, not any sneaky
//  stack stuff (that I was having trouble with)

// mode determines whether to actually draw (includes both screen
//    and texfile) or just to obtain cw, ch
//
//   I'm putting the smarts here, forget all those stupid char
//    arrays and such --- must retool various string methods

// back to where I was before (no "sizing" going on)!

//   oops 6/5:  allow ALL 0..255 values to be drawn ---
//       many are drawn specially, but the rest will display
//        with tiny vertical ascii code
//   6/9:  allow 256, to, as special non-existent symbol for EOF
//  6/27:  isfocus, newlines changed to "showSpecials", put all
//         logic for whether a given guy (enter or space) should
//          be shown on the caller
//  8/7/2000:  guys that would draw with the codebox scheme will now
//        check to see if they have a drawing string available, and
//        if so, will interpret it instead!
//  9/28/2000:  add in littlepagecode, absimagecode
void drawsymbol( float x, float y,
                     int sym,
                     int showSpecials )
{
  float left, right, top, bottom; // utility storage 

  float llx, lly;

  if(sym==trianglecode)
  {// draw standard little triangle --- this guy can be done
    // "normally" with no fancy stuff

    llx = x;   lly = y;

    top = 0.8*typsymwidth;

    if( !texdraw )
    {// to screen
      glBegin(GL_POLYGON);
        glVertex3f( llx, lly, textz );
        glVertex3f(llx + typsymwidth, lly + 0.5*top,
                       textz );
        glVertex3f(llx, lly + top, textz );
      glEnd();
    }
    else
    {// to texfile
  	  if( pictex )
		  {
			  pictexTriangle( llx, lly, 
				                llx+typsymwidth, lly + 0.5*top,
												llx, lly+top );
		  }
		  else
		  {
        psStartPath( llx, lly );
        psLineTo( llx+typsymwidth, lly+0.5*top );
        psLineTo( llx, lly+top );
        psEndPath( 1 );  // draw filled polygon
			}
    }// to texfile

  }// triangle

  else if(sym==littlepagecode)
  {// draw little page symbol

    llx = x;   lly = y;

    top = 0.8*typsymwidth;

    if( !texdraw )
    {// to screen
      glBegin(GL_LINE_LOOP);
        glVertex3f( llx, lly, textz );
        glVertex3f(llx + 12, lly, textz );
        glVertex3f(llx + 12, lly + typsymheight, textz );
 		    glVertex3f( llx, lly+typsymheight, textz );
      glEnd();
	    glBegin(GL_LINES);
	      glVertex3f( llx+2, lly+3, textz );
  	    glVertex3f( llx+10, lly+3, textz );
	      glVertex3f( llx+2, lly+6, textz );
	      glVertex3f( llx+10, lly+6, textz );
	      glVertex3f( llx+2, lly+9, textz );
  	    glVertex3f( llx+10, lly+9, textz );
	      glVertex3f( llx+2, lly+12, textz );
	      glVertex3f( llx+10, lly+12, textz );
	    glEnd();
    }
    else
    {// to texfile
      psStartPath( llx, lly );
        psLineTo(llx + 12, lly );
        psLineTo(llx + 12, lly + typsymheight );
 		    psLineTo( llx, lly+typsymheight );
      psEndPath( 0 );
      psLine( llx+2, lly+3, llx+10, lly+3 );
      psLine( llx+2, lly+6, llx+10, lly+6 );
      psLine( llx+2, lly+9, llx+10, lly+9 );
      psLine( llx+2, lly+12, llx+10, lly+12 );
    }// to texfile

  }// littlepage

  else if( sym==absimagecode )
  {// hollow triangle --- to draw less attention to itself
    llx = x;   lly = y;

    top = 0.8*typsymwidth;

    if( !texdraw )
    {// to screen
      glBegin(GL_LINE_LOOP);
        glVertex3f( llx, lly, textz );
        glVertex3f(llx + typsymwidth, lly + 0.5*top,
                       textz );
        glVertex3f(llx, lly + top, textz );
      glEnd();
    }
    else
    {// to texfile
  	  if( pictex )
		  {
			  pictexTriangle( llx, lly, 
				                llx+typsymwidth, lly + 0.5*top,
												llx, lly+top );
		  }
		  else
		  {
        psStartPath( llx, lly );
        psLineTo( llx+typsymwidth, lly+0.5*top );
        psLineTo( llx, lly+top );
        psEndPath( 0 );  // draw hollow triangle
			}
    }// to texfile

  }// absimage

  else if(sym==boxedjavacode)
  {// draw J inside a box, rather small

    llx = x;   lly = y;
		top = 0.8*typsymheight;

    if( !texdraw )
    {// to screen
      // the enclosing box
      glBegin(GL_LINE_LOOP);
        glVertex3f( llx, lly, textz );
        glVertex3f(llx + 1.2*typsymwidth, lly, textz );
        glVertex3f(llx + 1.2*typsymwidth, lly + top, textz );
 		    glVertex3f( llx, lly+top, textz );
      glEnd();
      // body of J
			glBegin(GL_LINE_STRIP);
        glVertex3f( llx+0.3*typsymwidth, lly + 0.3*top, textz );
        glVertex3f( llx+0.3*typsymwidth, lly + 0.2*top, textz );
        glVertex3f( llx+0.7*typsymwidth, lly + 0.2*top, textz );
        glVertex3f( llx+0.7*typsymwidth, lly + 0.8*top, textz );
			glEnd();
      // top crossbar of J
			glBegin(GL_LINES);
        glVertex3f( llx+0.4*typsymwidth, lly + 0.8*top, textz );
        glVertex3f( llx+ typsymwidth, lly + 0.8*top, textz );
			glEnd();

    }
    else
    {// to texfile

        psStartPath( llx, lly );
          psLineTo(llx + 1.2*typsymwidth, lly );
          psLineTo(llx + 1.2*typsymwidth, lly + top );
 		      psLineTo( llx, lly+top );
			  psEndPath( 0 );

        psStartPath( llx+0.3*typsymwidth, lly + 0.3*top );
        psLineTo( llx+0.3*typsymwidth, lly + 0.2*top );
        psLineTo( llx+0.7*typsymwidth, lly + 0.2*top );
        psLineTo( llx+0.7*typsymwidth, lly + 0.8*top );
				psStroke();

        psStartPath( llx+0.4*typsymwidth, lly + 0.8*top );
          psLineTo( llx+ typsymwidth, lly + 0.8*top );
				psStroke();

    }// to texfile

  }// boxedjava symbol

  else if(sym==' ' && showSpecials )
  {// draw little empty box representing a space
    left = typsymkern;
    right = typsymwidth - typsymkern;
    bottom = 0; 
    top = 0.9*typsymheight;

    llx = x;  lly = y;

    if( !texdraw )
    {// to screen
      glBegin(GL_LINE_LOOP);
        glVertex3f( llx + left, lly + bottom,textz);
        glVertex3f( llx + right, lly + bottom,textz);
        glVertex3f( llx + right, lly + top,textz);
        glVertex3f(llx+left,lly + top,textz);
      glEnd();
    }
    else
    {// to texfile
  	  if( pictex )
	  	{
			  pictexRect( llx+left, lly+bottom, 
				            llx+right, lly+top,   'R' );
		  }
  		else
	  	{
        psStartPath( llx + left, lly + bottom);
        psLineTo( llx + right, lly + bottom);
        psLineTo( llx + right, lly + top);
        psLineTo(llx+left,lly + top);
        psEndPath( 0 ); // close the loop, don't fill
			}
    }// to texfile
    
  }// space
  else if(sym==enterkey )
  {
	  if( showSpecials )
		{// draw enterkey picture if showing at all

      // draw a little picture of enter key

      left = 0.2*typsymwidth;
      right = 0.8*typsymwidth;
      bottom = 0.2*typsymheight;
      top = 0.8*typsymheight;

      if( !texdraw )
      {// to screen
        glBegin(GL_LINE_STRIP);
          glVertex3f(x+left,y+bottom,textz);
          glVertex3f(x+right,y+bottom,textz);
          glVertex3f(x+right,y+top,textz);
        glEnd();
        glBegin(GL_LINE_STRIP);
          glVertex3f(x+2*left,y, textz);
          glVertex3f(x+left,y+bottom,textz);
          glVertex3f(x+2*left,y+2*bottom, textz);
        glEnd();
      }
      else
      {// to texfile
    	  if( pictex )
		    {
				  pictexLine( x+left, y+bottom, x+right, y+bottom );
					pictexLine( x+right, y+bottom, x+right, y+top );
					pictexLine( x+2*left, y, x+left, y+bottom );
					pictexLine( x+left, y+bottom, x+2*left, y+2*bottom );
		    }
    		else
		    {
          psStartPath(x+left,y+bottom);
          psLineTo(x+right,y+bottom);
          psLineTo(x+right,y+top);
          psStroke();
				
          psStartPath(x+2*left,y);
          psLineTo(x+left,y+bottom);
          psLineTo(x+2*left,y+2*bottom);
          psStroke();
				}
      } // to texfile
		}// draw it
		else
		{// don't draw it --- done this way to prevent showing 013 form
		}

  }// enterkey

  // jb16:  tabsymbol is drawn like enter, only if focus
	//        but, somewhere must allow for its width!
  else if(sym==tabsymbolcode )
  {
	  if( showSpecials )
		{// draw tabsymbol picture if showing at all

      left = 0.1*typsymwidth;  // 0.2*typsymwidth;
      right = 2.9*typsymwidth; // 0.8*typsymwidth;
      bottom = 0.1*typsymheight;              // 0.2*typsymheight;
      top = 0.3*typsymheight;  // 0.8*typsymheight;

      if( !texdraw )
      {// to screen
			  glBegin(GL_LINE_LOOP);
				  glVertex3f( x+left, y+bottom, textz );
				  glVertex3f( x+right, y+bottom, textz );
				  glVertex3f( x+right, y+top, textz );
				  glVertex3f( x+left, y+top, textz );
				glEnd();
      }
      else
      {// to texfile
    	  if( pictex )
		    {
				  pictexLine( x+left, y+bottom, x+right, y+bottom );
					pictexLine( x+right, y+bottom, x+right, y+top );
					pictexLine( x+right, y+top, x+left, y+top );
					pictexLine( x+left, y+top, x+left, y+bottom );
		    }
    		else
		    {
          psStartPath( x + left, y + bottom);
          psLineTo( x + right, y + bottom);
          psLineTo( x + right, y + top);
          psLineTo( x + left,  y + top);
          psEndPath( 0 ); // close the loop, don't fill
				}
      } // to texfile
		}// draw it
		else
		{// don't draw it --- done this way to prevent showing 131 form
		}

  }// tab symbol

  else if(' '<=sym && sym<= '~')
  {// draw usual printable

    drawprintable( x, y, sym );

  }// printable

  else if( sym == endoffilecode )
	{// draw special symbol for end of file
    left = typsymkern;
    right = typsymwidth - typsymkern;
    bottom = 0; 
    top = 0.9*typsymheight;

    llx = x;  lly = y;

    if( !texdraw )
    {// to screen
      glBegin(GL_LINE_LOOP);
        glVertex3f( llx + left, lly + bottom,textz);
        glVertex3f( llx + right, lly + bottom,textz);
        glVertex3f( llx + right, lly + top,textz);
        glVertex3f(llx+left,lly + top,textz);
      glEnd();
			glBegin(GL_LINES);
        glVertex3f( llx + left, lly + bottom,textz);
        glVertex3f( llx + right, lly + top,textz);
        glVertex3f(llx+left,lly + top,textz);
        glVertex3f( llx + right, lly + bottom,textz);		  
			glEnd();
    }
    else
    {// to texfile
  	  if( pictex )
	  	{
        pictexRect( llx+left, lly+bottom, llx+right, lly+top, 'R' );
				pictexLine( llx+left, lly+bottom, llx+right, lly+top );
				pictexLine( llx+left, lly+top, llx+right, lly+bottom );			  
		  }
  		else
	  	{
        psStartPath( llx + left, lly + bottom);
        psLineTo( llx + right, lly + bottom);
        psLineTo( llx + right, lly + top);
        psLineTo(llx+left,lly + top);
        psEndPath( 0 ); // close the loop, don't fill

        psStartPath( llx + left, lly + bottom);
        psLineTo( llx + right, lly + top);
        psEndPath( 0 ); // close the loop, don't fill
        psStartPath( llx + left, lly + top);
        psLineTo( llx + right, lly + bottom);
        psEndPath( 0 ); // close the loop, don't fill
			}
    }// to texfile
    
	}// endoffile symbol

  // *************************************************
	//  special appearances handled above here, others all
	//  just get standard ascii code appearance

	else
	{
	  if( sym >= minProgCode && 
		    progChars[ sym - minProgCode ] != NULL )
	  {// symbol is programmed --- progChars[ sym-131 ] is its mystring
		  drawProgChar( x-hpad-1, y-vpad-1, 
			            progChars[ sym-minProgCode ] );
	  } 
	  else
	  {// default ascii code box

  	  // get the individual digits
      int temp, hundreds, tens, ones; 	
		  temp = sym;
  		ones = temp % 10;
	  	temp = temp / 10;
		  tens = temp % 10;
  		temp = temp / 10;
	  	hundreds = temp % 10;

  		// draw each LEDfont digit in correct position
	  	drawLEDdigit( x+2, y+10, hundreds );
		  drawLEDdigit( x+2, y+6, tens );
  		drawLEDdigit( x+2, y+2, ones );

      // draw border rectangle
      if( !texdraw )
      {// to screen
        glBegin(GL_LINE_LOOP);
          glVertex3f( x, y,textz);
          glVertex3f( x+8, y,textz);
          glVertex3f( x+8, y+14,textz);
          glVertex3f( x, y+14,textz);
        glEnd();
      }
      else
      {// to texfile
    	  if( pictex )
		    {
				  pictexRect( x, y, x+8, y+14, 'R' );
		    }
    		else
		    {
          psStartPath( x, y );
          psLineTo( x + 8, y );
          psLineTo( x + 8, y + 14 );
          psLineTo( x, y + 14 );
          psEndPath( 0 ); // close the loop, don't fill
				}
      }// to texfile
    }// not programmed

	}// ascii code display sym (or programmed)

}// drawsymbol

// jb17:  added this
// special guy to draw small B C F I S symbols for literals
// Behaves a lot like drawsymbol, except it takes a litcode
//  which is the kind of literal box instead of a symbol code,
//  and doesn't care about focus
void drawTypeSymbol( float x, float y,
                     int litcode )
{
  if( litcode == booleanbox )
  {// draw a little B

    if( !texdraw )
    {// to screen
      glBegin(GL_LINE_LOOP);
        glVertex3f( x, y, textz );
        glVertex3f( x + 4, y, textz );
        glVertex3f( x+6, y+1, textz );
				glVertex3f( x+6, y+3, textz );
				glVertex3f( x+4, y+4, textz );
				glVertex3f( x, y+4, textz );
      glEnd();
			glBegin(GL_LINE_STRIP);
        glVertex3f( x, y+4, textz );
        glVertex3f( x, y+8, textz );
				glVertex3f( x+4, y+8, textz );
				glVertex3f( x+6, y+7, textz );
				glVertex3f( x+6, y+5, textz );
				glVertex3f( x+4, y+4, textz );
			glEnd();
    }
    else
    {// to texfile
		  psStartPath( x, y );
        psLineTo( x, y  );
        psLineTo( x + 4, y  );
        psLineTo( x+6, y+1  );
				psLineTo( x+6, y+3  );
				psLineTo( x+4, y+4  );
				psLineTo( x, y+4  );
      psEndPath( 0 );

      psStartPath( x, y+4  );
        psLineTo( x, y+8  );
				psLineTo( x+4, y+8  );
				psLineTo( x+6, y+7  );
				psLineTo( x+6, y+5  );
				psLineTo( x+4, y+4  );
      psEndPath( 0 ); 
    }// to texfile

  }// B
  
	else if( litcode == charbox )
  {// draw a little C
    if( !texdraw )
    {// to screen
			glBegin(GL_LINE_STRIP);
        glVertex3f( x+6, y+1, textz );
        glVertex3f( x+6, y, textz );
				glVertex3f( x, y, textz );
				glVertex3f( x, y+8, textz );
				glVertex3f( x+6, y+8, textz );
				glVertex3f( x+6, y+7, textz );
			glEnd();
    }
    else
    {// to texfile
      psStartPath( x+6, y+1  );
        psLineTo( x+6, y  );
				psLineTo( x, y );
				psLineTo( x, y+8  );
				psLineTo( x+6, y+8  );
				psLineTo( x+6, y+7  );
      psStroke(); 
    }// to texfile
  }// C

	else if( litcode == floatbox )
  {// draw a little F
    if( !texdraw )
    {// to screen
			glBegin(GL_LINE_STRIP);
        glVertex3f( x+6, y+8, textz );
        glVertex3f( x, y+8, textz );
				glVertex3f( x, y, textz );
			glEnd();
			glBegin(GL_LINE_STRIP);
        glVertex3f( x, y+4, textz );
        glVertex3f( x+4, y+4, textz );
			glEnd();
    }
    else
    {// to texfile
      psStartPath( x+6, y+8 );
        psLineTo( x, y+8 );
				psLineTo( x, y );
			psStroke();
      psStartPath( x, y+4 );
        psLineTo( x+4, y+4 );
			psStroke();
    }// to texfile
  }// F

	else if( litcode == intbox )
  {// draw a little I
    if( !texdraw )
    {// to screen
			glBegin(GL_LINES);
        glVertex3f( x, y+8, textz );
        glVertex3f( x+6, y+8, textz );
				glVertex3f( x, y, textz );
				glVertex3f( x+6, y, textz );
				glVertex3f( x+3, y, textz );
				glVertex3f( x+3, y+8, textz );
			glEnd();
    }
    else
    {// to texfile
      psLine( x, y+8, x+6, y+8 );
			psLine( x, y, x+6, y );
			psLine( x+3, y, x+3, y+8 );
    }// to texfile
  }// I

	else if( litcode == stringbox )
  {// draw a little S
    if( !texdraw )
    {// to screen
			glBegin(GL_LINE_STRIP);
        glVertex3f( x+6, y+7, textz );
        glVertex3f( x+6, y+8, textz );
				glVertex3f( x, y+8, textz );
				glVertex3f( x, y+4, textz );
				glVertex3f( x+6, y+4, textz );
				glVertex3f( x+6, y, textz );
				glVertex3f( x, y, textz );
				glVertex3f( x, y+1, textz );
			glEnd();
    }
    else
    {// to texfile
      psStartPath( x+6, y+7 );
        psLineTo( x+6, y+8 );
				psLineTo( x, y+8 );
				psLineTo( x, y+4 );
				psLineTo( x+6, y+4 );
				psLineTo( x+6, y );
				psLineTo( x, y );
				psLineTo( x, y+1 );
			psStroke();
    }// to texfile
  }// S

	else
	  errorexit("unknown symbol requested in drawTypeSymbol");
  
}// drawTypeSymbol

// jb16:  for now, this guy's only purpose is to draw 
//         single-spot tab image, with minimum of fuss
void drawspecialsymbol( float x, float y,
                     int sym,
                     int showSpecials )
{
  float left, right, top, bottom; // utility storage 

  if(sym==tabsymbolcode )
  {// draw single-width tab symbol independent of showSpecials

      left = 0.1*maxcharwidth; 
      right = 0.9*maxcharwidth; 
      bottom = 0.1*typsymheight;
      top = 0.3*typsymheight;  

      if( !texdraw )
      {// to screen
			  glBegin(GL_LINE_LOOP);
				  glVertex3f( x+left, y+bottom, textz );
				  glVertex3f( x+right, y+bottom, textz );
				  glVertex3f( x+right, y+top, textz );
				  glVertex3f( x+left, y+top, textz );
				glEnd();
      }
      else
      {// to texfile
    	  if( pictex )
		    {
				  pictexLine( x+left, y+bottom, x+right, y+bottom );
					pictexLine( x+right, y+bottom, x+right, y+top );
					pictexLine( x+right, y+top, x+left, y+top );
					pictexLine( x+left, y+top, x+left, y+bottom );
		    }
    		else
		    {
          psStartPath( x + left, y + bottom);
          psLineTo( x + right, y + bottom);
          psLineTo( x + right, y + top);
          psLineTo( x + left,  y + top);
          psEndPath( 0 ); // close the loop, don't fill
				}
      } // to texfile
  }// tab symbol

  else
	  errorexit( "drawspecialsymbol only draws tab?");

}// drawspecialsymbol

// hard-code widths 
void buildsymbolinfo()
{
  int k;
  // set to reasonable value just in case
  for( k=0; k<maxsymcode; k++ )
    physwidthsym[ k ] = typsymwidth;

  // override for printables:
  physwidthsym['a'] = 6;
  physwidthsym['b'] = 6;
  physwidthsym['c'] = 6;
  physwidthsym['d'] = 6;
  physwidthsym['e'] = 6;
  physwidthsym['f'] = 5.8;
  physwidthsym['g'] = 6;
  physwidthsym['h'] = 4.8;
  physwidthsym['i'] = 2;
  physwidthsym['j'] = 4.6;
  physwidthsym['k'] = 6;
  physwidthsym['l'] = 0;
  physwidthsym['m'] = 9.6;
  physwidthsym['n'] = 4.8;
  physwidthsym['o'] = 7.4;
  physwidthsym['p'] = 6;
  physwidthsym['q'] = 6;
  physwidthsym['r'] = 4.8;
  physwidthsym['s'] = 5.4;
  physwidthsym['t'] = 4.2;
  physwidthsym['u'] = 4.8;
  physwidthsym['v'] = 8;
  physwidthsym['w'] = 12;
  physwidthsym['x'] = 8;
  physwidthsym['y'] = 8;
  physwidthsym['z'] = 6.4;

  physwidthsym[ 'A' ] = 11;
  physwidthsym[ 'B' ] = 8.2;
  physwidthsym[ 'C' ] = 9;
  physwidthsym[ 'D' ] = 9.4;
  physwidthsym[ 'E' ] = 8;
  physwidthsym[ 'F' ] = 8;
  physwidthsym[ 'G' ] = 9;
  physwidthsym[ 'H' ] = 9;
  physwidthsym[ 'I' ] = 6;
  physwidthsym[ 'J' ] = 6;
  physwidthsym[ 'K' ] = 9;
  physwidthsym[ 'L' ] = 7.5;
  physwidthsym[ 'M' ] = 12;
  physwidthsym[ 'N' ] = 9;
  physwidthsym[ 'O' ] = 10.4;
  physwidthsym[ 'P' ] = 8.2;
  physwidthsym[ 'Q' ] = 10.4;
  physwidthsym[ 'R' ] = 8.2;
  physwidthsym[ 'S' ] = 6.8;
  physwidthsym[ 'T' ] = 10.5;
  physwidthsym[ 'U' ] = 8.4;
  physwidthsym[ 'V' ] = 11;
  physwidthsym[ 'W' ] = 16;
  physwidthsym[ 'X' ] = 10;
  physwidthsym[ 'Y' ] = 11.5;
  physwidthsym[ 'Z' ] = 11;

  // do the other scattered guys:
  physwidthsym[ trianglecode ] =  typsymwidth;
  physwidthsym[ littlepagecode ] = 12;
  physwidthsym[ absimagecode ] = typsymwidth;
	physwidthsym[ boxedjavacode ] = typsymwidth;  // jb16

	// jb16:  tabsymbol has quite a hefty width
	physwidthsym[ tabsymbolcode ] = 3*typsymwidth;

  physwidthsym[ enterkey ] =  typsymwidth;
  physwidthsym[ ' ' ] =  typsymwidth; 

  physwidthsym[ '1' ] =6;
  physwidthsym['2']=6;
  physwidthsym['3']=6;
  physwidthsym['4']=7;
  physwidthsym['5']=5.2;
  physwidthsym['6']=5.6;
  physwidthsym['7']=6.6;
  physwidthsym['8']=6;
  physwidthsym['9']=5.6;
  physwidthsym['0']=6;
  physwidthsym['`']=2;
  physwidthsym['~']=10.8;
  physwidthsym['!']=1.8;
  physwidthsym['@']=8.4;
  physwidthsym['#']=11.8;
  physwidthsym['$']=5.6;
  physwidthsym['%']=10.2;
  physwidthsym['^']=4.6;
  physwidthsym['&']=10.2;
  physwidthsym['*']=8.66;
  physwidthsym['(']=3;
  physwidthsym[')']=3;
  physwidthsym['-']=8;
  physwidthsym['_']=6;
  physwidthsym['=']=10.8;
  physwidthsym['+']=10.8;
  physwidthsym[92]=6;
  physwidthsym['|']=0;
  physwidthsym[']']=2.2;
  physwidthsym['[']=2.2;
  physwidthsym['{']=5.6;
  physwidthsym['}']=5.6;
  physwidthsym[ 39 ]=2;
  physwidthsym['"']=4.4;
  physwidthsym[';']=2;
  physwidthsym[':']=1.8;
  physwidthsym[',']=2;
  physwidthsym['.']=1.8;
  physwidthsym['/']=6;
  physwidthsym['?']=5.1;
  physwidthsym['>']=9.4;
  physwidthsym['<']=9.8;

  // now set padsym amounts for all symbols:
  // set default padding as percentage of physical width,
  //  or minimum value
  for( k=0; k<maxsymcode; k++ )
  {
  //  padsym[k] = mymax( 1.0*physwidthsym[ k ], minpadsym );
    // hey, I don't want proportional, use fixed amount
    padsym[k] = minpadsym;
  }

  padsym[ enterkey ] = 0;  // padding included in display
	// jb16:  same for tab
	padsym[ tabsymbolcode ] = 0;
  
	padsym[ ' ' ] = 0;       //    "     "        "     "

  // now build the display width guy and find maxcharwidth

  // jb16:  leave tab symbol out of the maxcharwidth computation,
	//        because it can't be drawn in the consolebox

  maxcharwidth = 0;
  for( k=0; k<maxsymcode; k++ )
  {
    widthsym[k] = 2*padsym[k] + physwidthsym[k];
    if( physwidthsym[k] > maxcharwidth && k != tabsymbolcode )
      maxcharwidth = physwidthsym[k];
  }

  widthsym[ tabsymbolcode ] = physwidthsym[ tabsymbolcode ];

}

// draw normalcolor arrow from (x1,y1) to (x2,y2), with the
// only interesting part being that the arrow head is drawn
// depending on the orientation of the two points
//
//   assuming that the two points are far enough apart that
//   the arrowhead constants make sense, and that the arrow
//   is horizontal or vertical (could (and really aching to!)
//   to general arrow, but Boxes has no such arrows!)
void drawarrow( float x1, float y1, float x2, float y2,
			      float color[] )
{
  if( !texdraw )
  {// draw arrow body to screen
  	glColor3fv( color );

    glBegin(GL_LINES);
      glVertex3f( x1, y1, textz );  // body of arrow
      glVertex3f( x2, y2, textz );
    glEnd();
  }
  else
  {// draw arrow body to texfile
 	  if( pictex )
    {
		  pictexLine( x1, y1, x2, y2 );
    }
 		else
    {
      psLine( x1, y1, x2, y2 );
		}
  }// to texfile

  float x3, y3, x4, y4;

  if( myabs(x1-x2) < myabs(y1-y2) )
  {// closer to vertical (allowing for float errors---can't
    // say if(x1==x2) very safely)
    x3 = x1 - arrowheadwidth;
    x4 = x1 + arrowheadwidth;
    if( y1 < y2 )
      y3 = y2 - arrowheadheight;
    else
      y3 = y2 + arrowheadheight;
    y4 = y3;
  }
  else
  {// horizontal
    y3 = y1 - arrowheadwidth;
    y4 = y1 + arrowheadwidth;
    if( x1 < x2 )
      x3 = x2 - arrowheadheight;
    else
      x3 = x2 + arrowheadheight;
    x4 = x3;
  }

  if( !texdraw )
  {// draw arrow head to screen
    glBegin(GL_LINES);
      glVertex3f( x2, y2, textz );  // left arrow head edge
      glVertex3f( x3, y3, textz );

      glVertex3f( x2, y2, textz ); // right edge
      glVertex3f( x4, y4, textz );
    glEnd();
  }
  else
  {// draw arrow head to texfile
 	  if( pictex )
    {
		  pictexLine( x2, y2, x3, y3 );
			pictexLine( x2, y2, x4, y4 );
    }
 		else
    {
      psLine( x2, y2, x3, y3 );
      psLine( x2, y2, x4, y4 );
    }
	}// to texfile

}

void drawflowarrow( float x1, float y1, float x2, float y2,
				       float color[] )
{
  float x3, y3, x4, y4, x5, y5, size, aw;

  size = 0.5*flowarrowpad;
  aw = 0.5*size;  // arrowwidth

  if( myabs(x1-x2) < myabs(y1-y2) )
  {// closer to vertical (allowing for float errors---can't
    // say if(x1==x2) very safely)
    if( y1 < y2 )
    {// pointing up
      y3 = y2-size;       x3 = x2;
      x4 = x3 - aw;    y4 = y3;
      x5 = x3 + aw;    y5 = y3;
    }
    else
    {// down
      y3 = y2-size;       x3 = x2;
      x4 = x3 - aw;    y4 = y3;
      x5 = x3 + aw;    y5 = y3;
    }
  }// vertical
  else
  {//horizontal
    if( x1 < x2 )
    {// rightward
      x3 = x2-size;  y3 = y2;
      y4 = y3+aw;  x4 = x3;
      y5 = y3-aw;  x5 = x3;
    }
    else
    {// leftward
      x3 = x2+size;  y3 = y2;
      y4 = y3+aw;  x4 = x3;
      y5 = y3-aw;  x5 = x3;
    }
  }

  if( !texdraw )
  {// draw the flow arrow to screen

	  glColor3fv( color );

    // draw the body line  
    glBegin(GL_LINES);
      glVertex3f( x1, y1, textz ); 
      glVertex3f( x3, y3, textz );
    glEnd();

    // draw the head triangle
    glBegin(GL_LINE_LOOP);
      glVertex3f( x2, y2, textz );
      glVertex3f( x4, y4, textz );
      glVertex3f( x5, y5, textz );
    glEnd();
  }
  else
  {// draw flow arrow to texfile
 	  if( pictex )
    {
		  pictexTriangle( x2, y2, x4, y4, x5, y5 );
			pictexLine( x1, y1, x3, y3 );
    }
 		else
    {
      psLine( x1, y1, x3, y3 );
      psLine( x2, y2, x4, y4 );
      psLine( x4, y4, x5, y5 );
      psLine( x5, y5, x2, y2 );
      fprintf( texfile, "closepath\n" );
		}
  }// to texfile

} // drawflowarrow

// draw a line in color specified
void drawline( float x1, float y1, float x2, float y2, 
			     float color[] )
{
  if( !texdraw )
  {// draw line to screen
    glColor3fv( color );

      glBegin(GL_LINES);
      glVertex3f( x1, y1, textz );  // body of arrow
      glVertex3f( x2, y2, textz );
    glEnd();
  }
  else
  {// draw line to texfile
 	  if( pictex )
    {
		  pictexLine( x1, y1, x2, y2 );
    }
 		else
    {
      psLine( x1, y1, x2, y2 );
		}
  }// to texfile
}

// draw a parenthesis of height h, with center (cx,cy),
//  left paren if leftright==1, right if -1
void drawparen( float cx, float cy, float h, float leftright,
			      float color[] )
{
  float dx = h/36;
  float dy = dx;
  float x, y;

  dx = leftright * dx;

  int k;

  if( !texdraw )
  {// draw paren to screen
  
    glColor3fv( color );

	  // upper half
    x = cx;  y = cy;
    glBegin(GL_LINE_STRIP);
      glVertex3f( x, y, textz );
      y += 6;
      glVertex3f( x, y, textz );
      for(k=4; k>=1; k-- )
      {
        x += dx;
        y += k*dy;
        glVertex3f( x, y, textz );
      }
    glEnd();
    // lower half
    dy = - dy;
    x = cx;  y = cy;
    glBegin(GL_LINE_STRIP);
      glVertex3f( x, y, textz );
      y += 6*dy;
      glVertex3f( x, y, textz );
      for(k=4; k>=1; k-- )
      { 
        x += dx;
        y += k*dy;
        glVertex3f( x, y, textz );
      } 
    glEnd();
  }
  else
  {// draw paren to texfile
    if( pictex )
    {
		  float oldx, oldy;
  	  // upper half
      oldx = cx;  oldy = cy;
      x = oldx; y = oldy + 6;
			pictexLine( oldx, oldy, x, y );
			oldx = x;  oldy = y;
      for(k=4; k>=1; k-- )
      {
        x += dx;
        y += k*dy;
				pictexLine( oldx, oldy, x, y );
				oldx = x;  oldy = y;
      }
      // lower half
      dy = - dy;
      oldx = cx;  oldy = cy;
      x = oldx;  y = cy + 6*dy;
			pictexLine( oldx, oldy, x, y );
			oldx = x;  oldy = y;
      for(k=4; k>=1; k-- )
      { 
        x += dx;
        y += k*dy;
				pictexLine( oldx, oldy, x, y );
				oldx = x; oldy = y;
      } 
    }// pictex
 		else
    {
      // upper half
      x = cx;  y = cy;
      psMoveTo( x, y );
      y += 6*dy;
      psLineTo( x, y );
      for(k=4; k>=1; k-- )
      {
        x += dx;
        y += k*dy;
        psLineTo( x, y );
      }
      // lower half
      dy = - dy;
      x = cx;  y = cy;
      psMoveTo( x, y );
      y += 6*dy;
      psLineTo( x, y );
      for(k=4; k>=1; k-- )
      { 
        x += dx;
        y += k*dy;
        psLineTo( x, y );
      } 

      psStroke();  // actually draw all this stuff!
    
		}// not pictex

  }// texfile gets a paren

}// drawparen

// draw a bracket of height h, with center (cx,cy),
//  left if leftright==1, right if -1
void drawbracket( float cx, float cy, float h, float leftright,
				    float color[] )
{
  if( !texdraw )
  {// draw bracket to screen

    glColor3fv( color );

    glBegin(GL_LINE_STRIP);
      glVertex3f( cx + leftright*(1./9)*h, cy+h/2, textz );
      glVertex3f( cx, cy+h/2, textz );
      glVertex3f( cx, cy-h/2, textz );
      glVertex3f( cx + leftright*(1./9)*h, cy-h/2, textz );  
    glEnd();
  }
  else
  {// draw bracket to texfile
 	  if( pictex )
    {
		  pictexLine( cx+ leftright*(1./9)*h, cy+h/2,
			            cx, cy+h/2 );
			pictexLine( cx, cy+h/2, cx, cy-h/2 );
			pictexLine( cx, cy-h/2, cx+leftright*(1./9)*h, cy-h/2 );
    }
 		else
    {
      psMoveTo( cx + leftright*(1./9)*h, cy+h/2 );
      psLineTo( cx, cy+h/2 );
      psLineTo( cx, cy-h/2 );
      psLineTo( cx + leftright*(1./9)*h, cy-h/2 );  
      psStroke();
		}
  }// to texfile
}

// if supposed to "find", and "partnumber" is '?', implying not
// already changed to something meaningful earlier,
//
// given rectangle ulx, uly, lrx, lry, and point targetx,target,
//   all using standard MATH coordinate scheme (i.e. pos y up)
// compute OFFSET of the target point from the ulc of
// the rectangle,
// return in innerx,innery, and return in partnumber
// nothing --- leave it as '?' --- or whichpart, to tell
/// whether or not the
// target is actually inside the rectangle
// (a very simple little utility guy, used a zillion times)

// new scheme:  ulx,uly is offset from ulc of a box to ulc of 
//              rectangle of interest, 
//              lrx,lry is similar offset to lrc of rectangle,
//     see if targetx,y, also as an offset from the ulc of the
// same box, is inside the specified rectangle
//
//     and return innerx,y as offset from ulc of rectangle to
//       the specified point

// find tells whether to bother looking, even
// if target offset is inside rectangle, return 1 and inner offset
//  else return 0
int insideRect( int find, 
                 float ulx, float uly, float lrx, float lry, 
           float targetx, float targety,
                    float& innerx, float& innery )
{
  if( find )
  {
    if( ulx <= targetx && targetx <= lrx &&
       lry <= targety && targety <= uly )
    {// target is inside, return info
      innerx = targetx - ulx; innery = targety - uly;
      return 1;  // found it!
      
    }
    else
      return 0;  // looking and not found
  }
  else
    return 0;  // not looking
}

// tell whether targetx,y is outside the given rectangle
int outsideRect( float ulx, float uly, float lrx, float lry, 
           float targetx, float targety )
{
  if( targetx < ulx || lrx < targetx ||
	    targety > uly || lry > targety
	  )
    return 1; 
  else
    return 0; 
}

// simple utilities, categorize symbols
int legalinname( int sym )
{
  if( ('a'<=sym && sym<='z') ||
      ('A'<=sym && sym<='Z') ||
      ('0'<=sym && sym<='9') ||
      (sym=='_') 
    )
    return 1;
  else
    return 0;
}

int legalinoperator( int sym )
{
  if( sym=='=' || sym=='+' || sym=='-' || sym=='*' || sym=='/' ||
      sym=='%' || sym=='&' || sym=='|' || sym=='<' || sym=='>' ||
      sym=='!' 
    )
    return 1;
  else
    return 0;
}

// mystring and "string" facilities

  // tell whether the type code x can legally be coerced
  // to float (still need to see if its string actually
  // converts, though)
  int coercibletofloat( char x )
  {
    if( x == 'f' || x == 'i' ) // v14:  removed '?' here
      return 1;
    else
      return 0;
  }

  int numeric( char x )
  {
    if( x == 'f' || x == 'i' )
      return 1;
    else
      return 0;
  }

  // given target and source strings, want to make target point
  // to a copy of source string, but deallocate resources first
  //
  //  usage:  target =  storeString( target, source );
  mystring* storeString( mystring* target, mystring* source )
  {
    if( target != NULL ) 
      target->destroy();
    target = source->copy();  // just using target as convenient local
    return target;
  }

  mystring* storeCharString( mystring* target, char* str )
  {
    if( target != NULL )
      target->destroy();
    target = new mystring( str );
    return target;
  }

  // given "string" constant, box string, "string" constant,
  // build a new "string" concatenating them and return its
  // address  (get it:  CBC is one of several)
  char* builderrormessageCBC( char* first, mystring* second,
                               char* third )
  {
    int k,j;

    int num = strlen(first) + second->length() +
                 strlen(third) + 1;
    char* s = new char[num];
    for(k=0; k<strlen(first); k++ )
      s[k] = first[k];

    j = k;
    for(k=1; k<=second->length(); k++, j++ )
      s[j] = second->charAt( k );

    for(k=0; k<strlen(third); k++, j++ )
      s[j] = third[k];

    s[j] = '\0';

    return s;
  }

  // given pointers to 
  // mystring a, char c, mystring b,
  //  build acb, return its pointer
  mystring* concatSCS( mystring* a, char c, mystring* b )
  {
    int k, j;

    mystring* v = new mystring(); // build new empty mystring

    for( k=1; k<=a->length(); k++ )
      v->insertCharAt( k, a->charAt( k ) );

    v->insertCharAt( k, c );

    j = v->length();
    for( k=1; k<=b->length(); k++ )
    {
      v->insertCharAt( j+k, b->charAt( k ) );
    }

    return v;

  }

  // return whether or not s matches any primitive type name
  int isprimtype( mystring* s )
  {
    if( s->equals("char") || s->equals("int") || 
        s->equals("float") || s->equals("boolean") || 
        s->equals("string") 
      )
      return 1;
    else
      return 0;
  }

  // jb18:  return whether or not s is a legal type
  int isLegalType( mystring* s )
	{
	  if( s == NULL )
		  return 0;

	  int state = 0;
		int k = 1;  // position in s
		int n = s->length();  // for efficiency
		int error = 0;

    char ch;

		while( k<=n && !error )
		{
		  ch = s->charAt( k );
			k++;
			  
		  if( state == 0 )
			{// initial state --- must have letter
        if( ('a'<=ch && ch<='z') ||
            ('A'<=ch && ch<='Z')				    
					)
				  state = 1;
				else
				  error = 1;
			}
			else if( state == 1 )
			{// main state
			  if( legalinname( ch ) )
  				{} // stay in state 1
				else if( ch == '[' )
				  state = 2;
				else
				  error = 1;
			}
			else if( state == 2 )
			{// have [
			  if( ch == '[' )
				  state = 3;
				else
				  error = 1;
			}
			else if( state == 3 )
			{// have [[
			  error = 1;
			}
			else
			  errorexit("fsm error in isLegalType");
		}

    if( state == 0 )
		  error = 1;

    return !error;

	}// isLegalType

  // set up to disallow any specific words, can't think of any!
  int isReserved( mystring* s )
	{
	  if( isprimtype( s ) 
		  )
			return 1;
		else
		  return 0;
	}

  // really little utility:  build a mystring of the
  //   correct number of 4's (determined by experiment to
  //   be the widest digits)
  mystring* build4s( int x )
  {
    mystring* temp;

    if( x < 10 )
      temp = new mystring( "4" );
    else if( x < 100 )
      temp = new mystring( "44" );
    else if( x < 1000 )
      temp = new mystring( "444" );
    else if( x < 10000 )
      temp = new mystring( "4444" );
    else
      errorexit( "error in build4s---too many digits");

    return temp;
  }

// kind zone ++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// build constant label mystrings to be pointed at by many
// boxes
mystring* classdatastring;
mystring* classmethodsstring;
mystring* instancedatastring;
mystring* instancemethodsstring;
mystring* paramsstring;
mystring* localsstring;
// v10:
mystring* retvalstring;
mystring* allclassdatastring;

mystring* stackstring;
mystring* heapstring;
mystring* consolestring;

void buildconstantstrings()
{
  classdatastring = new mystring( "class data" );
  classmethodsstring = new mystring("class methods");
  instancedatastring = new mystring("instance data");
  instancemethodsstring = new mystring("instance methods");

//v10:  changed parameters, local variables to inputs, locals,
//       add output
  paramsstring = new mystring("inputs");
  localsstring = new mystring("locals");
	retvalstring = new mystring("output");
	allclassdatastring = new mystring("static data");

  stackstring = new mystring("stack");
  heapstring = new mystring("heap");
  consolestring = new mystring("console");
}

// kind zone ++++++++++++++++++++++++++++++++++++++++++++++++++++
//    set the box kind constants
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// utility kind classification functions

int actionkind( int k )
{
  if( k==seqbox || k==emptybox || 
      k==callbox || k==branchbox ||
      k==dobox || k==whilebox || k==forbox ||
      k==assbox || k==growbox || k==returnbox )
    return 1;
  else
    return 0;
}

int literalkind( int k )
{
  if( k == intbox || k == floatbox || k == charbox ||
	    k == booleanbox || k == stringbox )
		return 1;
	else
	  return 0;
}

int valuekind( int k )
{
  if( k==idbox || literalkind(k) || k==callbox ||
      k==memberbox || k==arraybox || k==opbox ||
      k==unopbox || k==array2dbox ||
      k==newopbox )
    return 1;
  else
    return 0;
}

// will have a single instance chargrid, global, used by
// the single consolebox to store its symbols.
//  Could store the grid using a string with new methods
//  and such, but this seems cleaner---has a cursor, too,
//  and all sorts of fine features, accessed by io....
//
//  chargrid is notified if viewers change, remembers 
//  offset last time was drawn, and remembers how it was
//  changed since last drawing

#include "chargridtype.h"

chargridtype* chargrid;  // the guy used by the consolebox

#include "portstype.h"

portstype* ports = new portstype; // gives nice access to the portsbox and portboxes
                  // inside it

// jb16:
translator* tran = new translator;  // refers to single translator object

// global box variables 
box* universe; // pointer to the univbox
box* clipbox = NULL;  // global clipbox
box* stack;
box* heap;
box* staticdata;  // v10:  global 
box* console;
box* errreturn;

// put in a request to display error message s,
// intelligently set up the world depending
//
//  must be given retbox which is box we should
//  return focus to after come back from the message
void requesterrormessage(char s[], box* retbox )
{ 
  errreturn = retbox;
  //  errreturn->setCurrentOutward(); doing simulateMove instead
  actionmode = errormode; 
  strcpy( errormessage, s );

  userview = 0;  // v11: just in case needs it

  smq->insert( MQ_REQUEST, MQ_ERROR_MESSAGE, s, 0, 0 );

}// requesterrormessage

// small utility to put up dialog box informing of texfilename
//  and waiting for a key press before returning to interaction
void informOfSnapshot( box* focus )
{
  char message[200];

  sprintf( message, "Just made snapshot to the file " );
  strcat( message, texfilename );

  messagecolorcode = successcolorcode;
  requesterrormessage( message, focus );

}

// a tour is a list of boxes that can be cleared, added to,
// stepped through

const int maxtour = 10;

class tour
{
public:

	// set all guys on tour back to natural color, then empty list
	void clear()
	{
	  int k;

	  for( k=0; k<size; k++ )
	    list[k]->setDye( nocolorcode );

	  size = 0;
	}

	// add the given box to the tour, and set its color, crash
	// if is not enough room
	void add( box* ibox, int colorcode )
	{

    // jb18:  improve so won't add a guy that's already in
		//        (don't need on addBreakbox because it's called first)
		int k;
		for( k=0; k<size; k++ )
		  if( list[k] == ibox )
			  return;

      if( size == maxtour )
        errorexit("Tour is too long");

      if( (ibox->whichAspect() != 'a' ||
			     ibox->whichKind() == javabox)  &&
			    ibox->whichAspect() != 'd' &&
					ibox->findCover() == ibox )
      {
        list[size] = ibox;
				cursor = size;  // last box added to tour is the current
        ibox->setDye( colorcode );
        size++;
      }
      // else not visible, so forget it

	}// tour::add

	// jb18:  add the given box, which has its breakpoint set,
	//  to the tour, even if it is using its doc aspect
	void addBreakbox( box* ibox, int colorcode )
	{
    if( size == maxtour )
      errorexit("Tour is too long");

    // add unless is buried
    if( ibox->findCover() == ibox )
      {
        list[size] = ibox;
				cursor = size;  // last box added to tour is the current
        ibox->setDye( colorcode );
        size++;
      }
      // else not visible, so forget it
	}// tour::addBreakbox

  // go through the given primitive list of boxes that
	// would like to be added to the tour and add them---needs
	// to be called after any use of box::evaluate that might have
	// wanted to put some boxes on tour
  void transfer( box* ilist[], int cclist[], int ni )
	{
	  int k;
		for( k=0; k<ni; k++ )
		  add( ilist[k], cclist[k] );
	}

  // tell how many boxes are on the tour
  int howMany()
	{ 
	  return size; 
	}

  // give the box that is current on the tour
	box* current()
	{
	  return list[ cursor ];
	}

  // advance the current box to the next one on the tour
	void advance()
	{
	  cursor++;
		if( cursor >= size )
		  cursor = 0;
	}

private:
  int size;      // number of boxes on the tour
	int cursor;   // index of current box on the tour
  box* list[maxtour]; // the boxes
};

// global variables, THE tour 
tour insplist;

// prototype
void reshape( int w, int h );

// implement viewer class right here, so can use various
// globals with which I was too sloppy
  // construct an empty viewer
  viewer::viewer()
  {
    onscreen = 'n';
  }
	
  // initialize viewer to given settings, given in
  // scale=1 units, same as window units
  void viewer::init( int idcode )
  {
    id = idcode;
    focusbox = NULL;  // not hooked over box reality yet
    backbox = NULL;
  }

  // set pixel size to (w,h), screen offset to (ulx,uly),
  // adjust conceptual size, in between rescale if says to
	//   8/9/2000:  if I'm portsviewer, set my backx, backy once
	//              know my scaled height
	//  jb19:  if doing author scaling, and this is a main viewer,
	//         set scale to match author window size
  void viewer::setSizeLocScale( int rescale,
            float w, float h, float ulx, float uly,
           float xs, float ys )
  {
    pixelwidth = w; pixelheight = h; 
    ulcx = ulx; ulcy = uly;  

    if( rescale )
    {
      xscale = xs; 
      yscale = ys;
    }

    width = pixelwidth / xscale;
    height = pixelheight / yscale;

		if( id == portsviewer )
		{// set backx, backy correctly
		  backx = 6*hpad;
			backy = height - 3*vpad;
		}

  }

  // initialize viewer to given settings, given in
  // scale=1 units, same as window units
  void viewer::resetSize( float w, float h )
  {
    pixelwidth = w; pixelheight = h; 
    width = pixelwidth / xscale;
    height = pixelheight / yscale;
  }// resetSize

  void viewer::setOnScreen( char state )
  {
    onscreen = state;
  }

  int viewer::isOnScreen()
  {
    return (onscreen=='y');
  }

  box* viewer::whoFocus()
  {
    return focusbox;
  }

  box* viewer::whoExec()
  {
    return execbox;
  }

  int viewer::getId()
  {
    return id;
  }

  void viewer::getSize( float& vw, float& vh )
  {
    vw = width;
    vh = height;
  }// getSize

  void viewer::multScale( float f )
  {
    xscale *= f;  yscale *= f;

    width = pixelwidth / xscale;  height = pixelheight / yscale;
  }

  void viewer::divScale( float f )
  {
    multScale( 1/f );
  }

  // given window point (px,py), determine if is in my view area,
  // and if so, return 1 and (vx,vy) as scaled viewer coords of the
  // point,  if not found, return 0
  int viewer::locateLocation( float px, float py, float& vx, float& vy )
  {
    // can't have the point in my view area if I'm not on screen
    if( onscreen != 'y' )
	  {
      return 0;  
	  }

    if( ulcx <= px && px <= ulcx + pixelwidth &&
        ulcy - pixelheight <= py && py <= ulcy )
    {// is in my area

      // convert to scaled viewer units
      vx = (px-ulcx) / xscale;
      vy = (ulcy - pixelheight + py ) / yscale;
      return 1;
    }
    else
	  {
      return 0;
  	}
  }

  // given pixel location (px,py), return string telling 
  // percentage offsets in 3 digits of that physical
  // location within this viewer's region
  //    Assumes that (px,py) is in the viewer region
  //  (very similar code to computePercentageOffsets utility function)
  mystring* viewer::getPercentageOffsets( float px, float py )
  {
    mystring* p;

    int xpercent, ypercent;
    float sx, sy;

    // figure percentage accurately
    sx = (px-ulcx)/pixelwidth;
    sy = (ulcy-py)/pixelheight;

    // convert to 3 digit form
    xpercent = (int) ( 1000 * ( sx + 0.0005 ) );
    ypercent = (int) ( 1000 * ( sy + 0.0005 ) );

    char s[9]; // like @513,246
    sprintf( s, "@%d:%d", xpercent, ypercent );

    p = new mystring( s );

    return p;
  }

  // figure my focusx,y such that the given
  // newfocusbox will be centered in my view area
  void viewer::centerFocus( box* newfocusbox )
  {
    float inx, iny, w, h, left, up;

    newfocusbox->findApparentOffset( inx, iny, w, h );

    left = (width - w)/2;    
    up = (height - h)/2 + h;

    if(left<0)
      left = hpad;  // too wide, shove to far left

    if(up > height)
      up = height - vpad; // too high, shove down from top

    // set focus offset so newfocusbox ulc is where it needs
    // to be so that the apparent focus is centered:
    focusx = left - inx;  focusy = up - iny;
  }

  // set my focusbox to the new one
  // and compute my backbox from it,
  // given a limitbox (if NULL, ignore)
  
  // "style" tells how to position over newfocus
  //     == 1 --> center the focus
  //     == 2 --> try to not move the focus 
  //     == 3 --> try to put the center of the apparent focus
  //              at the mouseupx, mouseupy point
	//     == 4 --> try to not move, even if means the focus is
	//               largely out of sight
  
  void viewer::setFocus( int style,
    box* newfocusbox, box* limitbox )
  {
    float left, up;
    float inx, iny, outx, outy, inw, inh;
    int found;

    if(style==1)
    {// first style is to center the focus, but shift up/left
      // as needed to make the entire box fit in view, if possible 

      centerFocus( newfocusbox ); // sets focusx,y

    }
    else if(style==2)
    {// from newfocus, scan out until hit my current backbox,
      // or realize never will.  If hit it, try to maintain
      // same view, else fall back on centering

      if(backbox==NULL)
      {
        centerFocus( newfocusbox );
      }
      else
      {// have a backbox, maybe want to use it
        
        // get outx,y as vector from backbox ulc to my ulc
        found = newfocusbox->findOffsetOutward( backbox,
                       outx, outy );

        if(!found)
        {// didn't encounter backbox in outward scan from
          // newfocusbox, so center
          centerFocus( newfocusbox );
        }
        else
        {// found backbox, see if it covers new focus, using backx,y

          // find info about apparent focus 
          newfocusbox->findApparentOffset( inx, iny, inw, inh );

          left = backx + outx + inx;  // coords of ulc of newfocus
          up = backy + outy + iny;

          if( vbpad <= left && left+inw <= width-vbpad
               &&
              up <= height-vbpad && up-inh >= vbpad )
          {// yea, it fits, so go with it
                // focusx,y is to ulc of actual focus box, not
                 // apparent focus
            focusx = left - inx;
            focusy = up - iny;
          }
          else
          {// is out of sight, center it
            centerFocus( newfocusbox );
          }

        }

      }
    }// style 2

    else if( style == 3 )
    {// 
      newfocusbox->findApparentOffset( inx, iny, inw, inh );

      /* first try --- ignores offset of click in newfocusbox
      left = mouseupx - inw/2 - inx;    
      up = mouseupy + inh/2 - iny;
      */
      
      // new try --- use stored actual offset --- looks good
      left = mouseupx - mouseoffx;
      up = mouseupy - mouseoffy;

    // adjust as needed so that entire apparent focus is in
    // view, if too big, keep ulc in view

    if( left+inw > width-vbpad )
      left = width - inw - vbpad;
    if( left+inx < vbpad )
      left = vbpad-inx;

    if( up+iny-inh < vbpad )
      up = vbpad+inh-iny;
    if( up+iny > height - vbpad )
      up = height - vbpad - iny;
    
    focusx = left;  focusy = up;

    }

    else if( style == 4 )
		{// be happy with current positioning!
      if(backbox==NULL)
      {
        centerFocus( newfocusbox );
      }
      else
      {// have a backbox, maybe want to use it
        
        // get outx,y as vector from backbox ulc to my ulc
        found = newfocusbox->findOffsetOutward( backbox,
                       outx, outy );

          // find info about apparent focus 
          newfocusbox->findApparentOffset( inx, iny, inw, inh );

          left = backx + outx + inx;  // coords of ulc of newfocus
          up = backy + outy + iny;

            focusx = left - inx;
            focusy = up - iny;

      }

		}// style 4

    else if( style == 5 )
		{// v11:  last-minute style---use "note" global info to put focus
		 //        positioned under the mouse

		   focusx = notemousex;
			 focusy = notemousey;

		}

    else
      errorexit("illegal style in viewer::setFocus");

    // however positioned over focus, now move to newfocus
    // and set backbox stuff
    
    focusbox = newfocusbox;
    backbox = focusbox->findBackBox(width, height,
                    focusx, focusy, limitbox,
                    backx, backy );
  
  } // end of viewer::setFocus

  // given (mx,my) as location in my view area,
  // set my focus, backbox, and all, based on moving the focus
  // to the part of box reality at that location, in terms of
  // my current backbox

	// 10/7/2000:  newly return in mx,my (turned in & params) the
	//             offset of the focus in the new focus box
	// 10/8/2000:  now use mouseoffx,y, so no changes here
  box* viewer::setFocusToLocation( float& mx, float& my, int& needredraw )
  {
    box* newfocusbox;

    char whichpart;
    float partx, party;

    // obtain box info
      // note: must pass in location as offset from
      // ulc of backbox
    newfocusbox = backbox->findLocation(
               mx-backx, my - backy, 
               whichpart, partx, party );
    
    if(newfocusbox==NULL)
    {// location was not over my backbox
      needredraw = 0;
      return focusbox;
    }
    else
    {// have a new focus to set to, maybe
    
      needredraw = 1;

      // fix up newfocusbox to correctly reflect focus on
      // whichpart, with given offset
      newfocusbox->setDataToLocation( whichpart, partx, party );

      // 10/7/2000: right here is where we can determine the
			//   offset
			// 10/8/2000:  huh, uh --- use mouseoff instead

      // scan outward from new focus and mark all along the
      // way as current in their outer box's inner list
      newfocusbox->setCurrentOutward();

      return newfocusbox;

    }// have a new focus

  }// setFocusToLocation

  // jb25:  like setFocusToLocation, but doesn't change anything
  //         just finds the box, or null, and
  //   also returns which part and the symbol number within the part,
  //     if appropriate
  box* viewer::findTargetBox( float mx, float my,
                   char& whichpart, int& index )
  {
    box* newfocusbox;

    float partx, party;
    newfocusbox = backbox->findLocation(
               mx-backx, my - backy, 
               whichpart, partx, party );

    if( newfocusbox == NULL )
    {// mouse wasn't over any box, use the universe as safety valve
/* jb6a:  just return NULL
      whichpart = '.';  // just over the universe
      index = 0;
      return universe;
*/
      return NULL;
    }
    else
    {// mouse was over a box
      // depending on the part the offset is over, determine the
      // symbol number in the string
      newfocusbox->findSymbolInPart( whichpart, partx, party, index );         

      return newfocusbox;      
    }
  
  }// findTargetBox

// utility function, not part of viewer class
//  given tbox and distances in it tx, ty, produce a mystring
//  that gives three digits each of x and y fractions of the box
//   For example, 513 means .513 or about half of the width or height
mystring* computePercentageOffsets( box* tbox, float tx, float ty )
{
  mystring* p;

  int xpercent, ypercent;

  xpercent = (int) ( 1000 * (tx / tbox->howWide() + 0.0005) );
  ypercent = (int) ( 1000 * ((-ty) / tbox->howHigh() + 0.0005) );

  char s[9]; // like @513,246
  sprintf( s, "@%d:%d", xpercent, ypercent );

  p = new mystring( s );

  return p;
  
}

  // create and add a new notebox using locstring
  // telling where the mouse was
  box* viewer::addNotebox( mystring* locstring )
  {
    // build the new notebox
    box* newbox = new box();  
    newbox->build( notebox );

    box* owner;  // typically add to inner list of owner of focusbox
    int index;
    box* oldfocus = focusbox;

    // depending on the lesson focus, add in newbox 
    int knd = focusbox->whichKind();

    if( knd == lessonbox )
    {// insert newbox at front of focusbox
      focusbox->addInnerAtFront( newbox );
      newbox->fixupReality(0);
    }
    else
    {// focusbox is inside the lessonbox

      // if inside a notebox, move out to the notebox to add after it    
      if( knd == parbox )
      {
        focusbox = focusbox->whoOuter();
        knd = focusbox->whichKind();
      }

      if( knd == combox || knd == notebox )
      {// insert immediately after the focusbox
       // make the focusbox the current box in its owner, add at cursor
       owner = focusbox->whoOuter();  
       index = focusbox->positionInInnerList();
       owner->moveCurrentTo( index );
       owner->addInnerAtCursor( newbox );

       newbox->fixupReality(0);

      }
      else
        errorexit("viewer::addNotebox --- bad focusbox kind");

    }// focus not on the lessonbox

    // put the lesson focus inside the newbox
    focusbox = newbox->accessInnerAt( 1 );
    // jb26:  just leave this out---situation is simple enough that
    //        doesn't seem to need to be done, if does, can do manually
    //        in this bad situation, where the newfocus is not 
    //        actually visible
//    oldfocus->simulateMove( focusbox );

    focusbox->storeName( locstring );
    locstring->destroy(); // storeName makes a copy
    focusbox->fixupReality(0);
            
    focusbox = focusbox->whoNext();// move focus to second inner 
      
    return newbox;  // let the outside have the added notebox

  }// addNotebox

  // change offset of view area over focus by requested
  // fractions of full view width,height, then refigure
  // backbox
  void viewer::shift( float dx, float dy )
  {
    focusx += dx * width;
    focusy += dy * height;

    backbox = focusbox->findBackBox(width, height,
                    focusx, focusy, limitbox,
                    backx, backy );
  }

  // given translation vector (dx,dy) in pixels, translate
  // focus by that vector, transformed by scaling, and refigure
  // backbox
  void viewer::translate( float dx, float dy )
  {
    focusx += dx/xscale;
    focusy += dy/yscale;

    backbox = focusbox->findBackBox(width, height,
                    focusx, focusy, limitbox,
                    backx, backy );
  }

/* jb21:  toss for now

  // if any notes are positioned over me, draw them at their
	//  offset from my backbox
	void noteDisplay( notegroup* notes )
	{
	  int k;

		for( k=0; k<notes->howMany(); k++ )
		{

			if( (notes->data[k]).whichViewer() == id )
			{// hey, this note is over me, so have it draw itself
			 // at its offset relative to my backbox
			 // Also tell it viewer info so if it is default form,
			 //  can show up in sight
        (notes->data[k]).draw( backx, backy, width, height );
			}

		}// loop through all active notes

	}// viewer::noteDisplay

jb21  */

  // draw my portion of box reality in my viewport
  // using focus methodology, or in some special way depending
  // on my id
  void viewer::display()
  { 
    // manage opengl issues
    // viewport wants lower left corner
    glLoadIdentity();
    // had this for a long time!
    //glViewport(ulcx,screenheight-ulcy,pixelwidth,pixelheight);
    glViewport(ulcx,ulcy-pixelheight,pixelwidth,pixelheight);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0,pixelwidth,0,pixelheight,-minz,-maxz);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  
    glScalef( xscale, yscale, 1.0 );

    // if needed, draw my background rectangle
    if( id == consoleviewer || id == portsviewer || id == userviewer )
    {
      GLfloat zd = maxz+0.001;
      glColor3fv( neutralcolor );
      glBegin( GL_POLYGON );
        glVertex3f( 0, 0, zd );
        glVertex3f( width, 0, zd );
        glVertex3f( width, height, zd );
        glVertex3f( 0, height, zd );
      glEnd();
    }

    // draw my border
    int fudge;
    float fud;

    for(fudge = 0; fudge <=2; fudge++)
    {
			// new scheme 
			if( id==interactiveviewer || id== executionviewer 
			    // v11:  make border of userviewer show what mode
					|| id==userviewer
				)
			{// non-fixed viewers
			  if( actionmode == interactivemode 
				    //v11
						|| actionmode == consoleviewmode
					)
			    glColor3f( 1.0, 0.0, 0.0 );
  			else if( actionmode == pausedmode 
				    //v11
						 || actionmode == pausedconsoleviewmode )
	  		  glColor3f( 1.0, 1.0, 0.0 );
		  	else if( actionmode == executingmode )
				  glColor3f( 0.0, 1.0, 0.0 );
				else if(actionmode == input1mode ||
				        actionmode == inputlinemode )
					glColor3f( 0.7, 0.7, 0.7 );
				else
				  errorexit("illegal actionmode in viewer::display");
			}

			// jb19: give lessonviewer its own color, just for fun
			else if( id==lessonviewer )
			{
			  glColor3f( 0, 1, 1 );
			}
			else
			{// stable viewers just get boring blue borders
			  glColor3f( 0.0, 0.0, 1.0 );
			}

// jb21: put in filling of color showing the lesson state

			if( fudge == 1 )
      {
        if( lesson->isAuthor() )
        {// author has variety of fillings
          if( lesson->getState()==authoringLessonState )
  			    glColor3f( 0, 0, 0 );
          else if( lesson->getState()==pausedLessonState )
    			  glColor3f( 1.0, 0., 0. );
          else if( lesson->getState()==recordingLessonState )
  		  	  glColor3f( 0, 0, 1.0 );
          else if( lesson->getState()==readingLessonState )
            glColor3f( 1, 1, 1 );
          else if( lesson->getState()==movingLessonState )
            glColor3f( 0, 1, 0 );
          else if( lesson->getState()==editingLessonState )
            glColor3f( 0, 1, 1 );
          else if( lesson->getState()==disabledLessonState )
          {// don't set color specially
          }
          else
            errorexit("unknown filling color");
        }// only do when authoring
   
      }// special color for center?
    
      // now that color has been determined, draw the rectangle border
      fud = fudge / xscale;
      glBegin(GL_LINE_LOOP);
        glVertex3f( 0+fud, 0+fud, textz );
        glVertex3f( width-fud, 0+fud, textz );
        glVertex3f( width-fud, height-fud, textz );
        glVertex3f( 0+fud, height-fud, textz );
      glEnd();
      
    }// for loop to draw fancy border

    // after drawing border, draw contents:
    if( id == consoleviewer )
    {// just draw chargrid
       chargrid->display( hpad, height - vpad );
    }
    else if( id == portsviewer )
    {// draw portsbox specially, so shows up even if cycled down
		 //  idea is that in split-screen mode, always want to see
		 //  console and ports, but often without pausing

      backbox->portsDisplay( backx, backy );

//        backbox->display( backx, backy,
//				                   0, width, height, 0,
//													 1, focusbox );
    }
		//v11:
		else if( id == userviewer )
		{// draw chargrid
		  chargrid->display( hpad, height - vpad );
		}

    else
    {// standard viewer
      // draw my backbox at correct offset
      backbox->display( backx, backy, 
                       0, width, height, 0,
                       1, focusbox );
    }

    // jb7a:  display this viewer's notes right now,
    // using the same viewport and scaling as the box reality uses
    if( lesson->getState() != disabledLessonState &&
        lesson->getState() != editingLessonState          
      )
    lesson->displayNotes( this );

  }// viewer::display

  // jb26:  obtain backbox info
  box* viewer::getBackboxInfo( float& bx, float& by )
  {
    bx = backx;  by = backy;
    return backbox;
  }// getBackboxInfo

  // set eb to execution box, I start going as the execution
	//  viewer, and return 1, else is error, and 0 is returned
  int viewer::startExecution( box* eb )
  {
    // clean out stack and heap --- they're left full when
    // execution ends, however it ends, so can examine,
    // need to clean up (also clean up before save---we don't
    // want all that stuff cluttering up the disk)

    universe->clearOutStackAndHeapAndStatic();

    // also, clear out ports
		universe->clearOutPorts();

    // set cursor to visible in case was left hidden:
		chargrid->showCursor();

		// remove all programmed characters
		int k;
		for( k=0; k<126; k++ )
		  progChars[k] = NULL;

    box* badbox;
    int status;

    badbox = universe->staticCheck( status );

    if( badbox != NULL )
    {// detected an error in the static check

      if( status == valuetypeincompaterror )
        requesterrormessage(
        "The value of this box doesn't match its type",
          badbox );
      else if( status == duplicateclassnameserror )
        requesterrormessage(
        "Can't have classes with the same name", badbox );
      else if( status == duplicatemembernameserror )
        requesterrormessage(
        "Can't have class members with the same name", badbox );        
      else if( status == canthaveemptynameerror )
        requesterrormessage(
        "This box must have a name", badbox );               
      else if( status == missingtypeerror )
        requesterrormessage(
        "This data box must have a type specified", badbox );                     
      else if( status == parammustbeemptyerror )
        requesterrormessage(
        "Not allowed to put value in a parameter interactively", 
                 badbox );                     
      else if( status == badprimvalueerror )
        requesterrormessage(
        "This is not a legal value of the specified type", badbox );                     
      else if( status == objectrefnotemptyerror )
        requesterrormessage(
        "References to instances can't be put in interactively", 
                  badbox );                     
      else if( status == duplicatelocalnameserror )
        requesterrormessage(
        "Can't have parameters or local variables with the same name",
                        badbox );                     
      else if( status == namecantbereservederror )
        requesterrormessage(
        "Identifier can't be a reserved word",
                                badbox );                     
      else if( status == badfirstsymbolerror )
			  requesterrormessage(
				"Identifier must start with a letter", badbox );
      else if( status == illegaltypeerror )
			  requesterrormessage(
				"Type must be primitive or class name followed by 0, 1, or 2 ['s", 
				badbox );
      else
        errorexit("illegal errorcode in viewer::startExecution");

      return 0;  // failure to start execution

    }
    else
    {// go ahead and start up execution

		  // v10:  first, fill the static data
			universe->copyClassData();

      execbox = eb;
      reqbox = NULL;  // requested "out of nowhere" !

      // put interactive focus somewhere!
      setFocus( 1, execbox, NULL );

      // added 8/12/2000 to fix corrupted reality bug
			//   hop focus away from method box -> must fix it up
      eb->simulateMove( universe );

      return 1;  // success in starting execution

    }

  }// viewer::startExecution

  // position myself tidily over the box of unique kind specialbox
  void viewer::fixOver( int specialbox )
  {
    if( specialbox == consolebox )
    {// fix over console
      backbox = console;
      backx = hpad;
      backy = hpad + screenheight/2;
      focusbox = backbox;
    }
		else if( specialbox == portsbox )
		{// find portsbox as next of console, fix over it
		  box* pb = console->whoNext();
			if( pb->whichKind() != portsbox )
			{	  
        errorexit("box after console box is not portsbox?");
			}

      // set my data members to fix over portsbox
      backbox = pb;
      backx = hpad;
      backy = hpad + 0.75*screenheight;
      focusbox = backbox;

		}

  }// viewer::fixOver
 
  // manage a step of execution, from execbox
  //
  //  states are:
  //               initialstate
  //               waittoactstate
  //               waittoproducevaluestate
  //               waittoproducelocationstate
  //
  // new inspection list approach:  status is expunged---will
  //  pause when is something to look at
  void viewer::step()
  {
    // before taking the next step, clear out the inspection list
    insplist.clear();  
		// and set up the local list to feed into insplist
		//  from evaluate tour selections
		box* ilist[maxtour];
		int cclist[maxtour];
		int ni = 0;

    int status1, oknd, slot;
    
    box* tempbox;  // junky utility 
    box* owner = execbox->whoOuter();
    box* newbox;  // utility for when need to build a new box

    box* badbox; 

    int est = execbox->whichState();
    int knd = execbox->whichKind();

    // jb18:  if execbox has breakpoint set, set pausetoinspect
		if( execbox->isBreakpoint() && est == initialstate )
		{
		  pausetoinspect = 1;
      insplist.addBreakbox( execbox, signalcolorcode );
		}

// kind zone +++++++++++++++++++++++++++++++++++++++++++++++++++++
//   execute current box, depending on its kind, state
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    // branch on state first to allow for overlapping actions
    // among different kinds
    if( est == initialstate )
    {// it's being signaled for the first time
  
      if( knd==callbox )
      {
        // must have an inner, just signal it
          execbox->setState( waittocallstate );
          reqbox = execbox;
          execbox = execbox->whoFirst();
          execbox->moveOutside();

          insplist.add( execbox, signalcolorcode );

      }// callbox in initialstate

      else if( literalkind(knd) )
      {
        badbox = execbox->evaluate( universe, 1, status1, 
				                      ilist, cclist,  ni );
				                                      
        if( status1 > 0 )
        {// succeeded, didn't request error message

          insplist.transfer( ilist, cclist, ni );

          execbox->setState( initialstate ); 
          reqbox = execbox;  
       
          execbox = owner; 
          execbox->moveOutside();  // just in case

        }// eval succeeded
        else
        {// error detected

          if( status1 == badliteralformaterror )
            requesterrormessage("Literal value has incorrect type",
                       badbox );
          else
            errorexit("unhandled errorcode in step, literal kind");
        }

      }// literal kinds

      else if( knd==idbox )
      {
        // get info to decide whether want to evaluate execbox
        //  for a location or a value

        oknd = owner->whichKind();
        slot = execbox->positionInInnerList(); 

        // guys that treat idbox as specifying a location,
        // rather than a value
        if( (oknd==callbox && slot==1) ||
            (oknd==assbox && slot==1) ||
            (oknd==growbox) 
          )
          badbox = execbox->evaluate( universe, 0, status1, 
					                                 ilist, cclist,  ni );
        else
          badbox = execbox->evaluate( universe, 1, status1,
					                                 ilist, cclist,  ni ); 

        if( status1 > 0 )
        {// succeeded, didn't request error message

          insplist.transfer( ilist, cclist, ni );

          execbox->setState( initialstate ); 
          reqbox = execbox;  
       
          execbox = owner; 
          execbox->moveOutside();  // just in case

        }// eval succeeded
        else
        {// error detected

          if( status1 == datanotiniterror )
            requesterrormessage("The box referred to has no value",
                       badbox );
          else if( status1 == idnotfounderror )
            requesterrormessage("There is no such identifier",
                            badbox );
          else if( status1 == emptyidnameerror )
            requesterrormessage("Identifier name can't be empty",
                            badbox );
          else if( status1 == methodnamenotvalueerror )
            requesterrormessage("There is no such identifier",
                            badbox );
          else if( status1 == badbifnameerror )
            requesterrormessage("There is no such system method",
                            badbox );
          else if( status1 == nosuchinstanceerror )
            requesterrormessage(
						"The instance this was a member of has been destroyed",
                            badbox );
          else
            errorexit("unhandled errorcode in step, idbox");
        }

      }// idbox

      else if( knd==methodbox )
      {// it's a class method called interactively 

        // need to perform most of the tasks that a callbox
        // would ordinarily be doing:

        //  make appropriate copy of execbox, 
        newbox = execbox->copyMethodbox( classmethodbox );

        // any parameter values must have been set interactively,
        // so nothing to do there

        // snugly install newbox at the front of stackbox:
        stack->addInnerAtFront( newbox );

        // jb16: translate javas on this startup method box
				// jb18:  get error code and maybe stop
				status1 = newbox->translateJava();

        if( status1 == 0 )
				{// translation succeeded

        // now, to fix up everything after magically popping
        // a new box in the stack box, need to fixupReality
        // from that new box
// jb16:  translateJava does this and more: newbox->fixupReality(0);

        // jb16:  put inputs box of this methodbox on tour,
				//        so all method calls start the same way,
				//        pausing on inputs (more importantly, showing
				//        the ulc of the method box just called,
				//        and stackbox upper-left of that
          tempbox = newbox->accessInnerAt( 1 );
			    insplist.add( tempbox, newvaluecolorcode );

  //        execbox->setState( waittastate ); 
          reqbox = execbox;  
          execbox = newbox->accessInnerAt( 4 );  // v10: the code box 
          execbox->moveOutside();
        }// translation succeeded

      }// methodbox

      else if(knd==seqbox)
      {// when first signaled, it signals first child, if any,
        // and goes into waitstate, else (no inners) signals
        // out
        //    except, if is "codebox", needs to do the finishing
        //    up stuff---silly case of empty codebox for a method
        //    call

        insplist.add( execbox, signalcolorcode );

        if(execbox->whoFirst()==NULL)
        {// has no inners
          if( owner->whichKind() != classmethodbox &&
              owner->whichKind() != instancemethodbox )
          {// ordinary, empty seqbox
            execbox->setState( initialstate );
                            // ready to start work all over again
            reqbox = execbox;
            execbox = owner;
            execbox->moveOutside();
        
          }
          else
          {// empty codebox, need to do finishing up actions
            // Uniform approach, so returnbox can cause halt:
            //  set my state to "returnstate", signal myself
            execbox->setState( returnstate );
            reqbox = execbox;
            // execbox stays the same
            execbox->moveOutside();

          }

        }
        else
        {// signal first inner and wait to act
          execbox->setState( waittastate );
          reqbox = execbox;
          execbox = execbox->whoFirst();
          execbox->moveOutside();

//          addToInspList( execbox, signalcolorcode );

        }

      }// seqbox

      // action knds that are guaranteed to have a first inner,
      // just signal it and wait
      else if( knd == assbox || knd == growbox || knd == returnbox ||
               knd == branchbox || knd == whilebox || knd == dobox ||
               knd == forbox 
             )
      {

			  insplist.add( execbox, signalcolorcode );

        execbox->setState( waittastate );
        reqbox = execbox;
        execbox = execbox->whoFirst();
        execbox->moveOutside();
  
//        addToInspList( execbox, signalcolorcode );

      }

      // jb16:      javabox in initialstate 
      else if( knd == javabox )
      {
			  insplist.add( execbox, signalcolorcode );
				 
        execbox->setState( javawaitstate );
        reqbox = execbox;
        execbox = execbox->whoFirst();
        execbox->moveOutside();
  
//        addToInspList( execbox, signalcolorcode );

      }// javabox in initialstate

      else if( knd == memberbox )
      {
        // I will later end up producing a value or a location,
        // have to realize that now and go into correct waiting
        // state now
        oknd = owner->whichKind();
        slot = execbox->positionInInnerList(); 

        // guys that want me to produce a location, not a value
        if( (oknd==callbox && slot==1) ||
            (oknd==assbox && slot==1) ||
            (oknd==growbox)
          )
          execbox->setState( waittplstate );
        else
          execbox->setState( waittpvstate );

        // signal first (and only) inner
        reqbox = execbox;
        execbox = execbox->whoFirst();
        execbox->moveOutside();

//        addToInspList( execbox, signalcolorcode );
     
      }// memberbox
       
      else if( knd == arraybox || knd == array2dbox )
      {
        // I will later end up producing a value or a location,
        // have to realize that now and go into correct waiting
        // state now
        oknd = owner->whichKind();
        slot = execbox->positionInInnerList(); 

        // guys that want me to produce a location, not a value
        if( (oknd==callbox && slot==1) ||
            (oknd==assbox && slot==1) ||
            (oknd==growbox)
          )
          execbox->setState( waittplstate );
        else
          execbox->setState( waittpvstate );

        // signal first (of 2 or 3) inner
        reqbox = execbox;
        execbox = execbox->whoFirst();
        execbox->moveOutside();

//        addToInspList( execbox, signalcolorcode );
      
      }// arraybox, array2dbox

      // newop is special, because if inners are empty, want
      // to just evaluate it right away, else need to 
      // get 1 or 2 inners done first
      else if( knd==newopbox )
      {
        tempbox = execbox->whoFirst(); // first inner
        if( tempbox->whichKind() == emptybox )
        {// make sure second is empty, move right on to build 1
          tempbox = tempbox->whoNext(); // is now second inner
          if( tempbox->whichKind() == emptybox )
          {// everything cool, move right to evaluation, use
            // first argument to signal want a scalar,
            //  must be a non-primitive type
            execbox->evaluate( universe, 0, status1, ilist, cclist,  ni );

            if( status1 <= 0 )
            {// evaluation failed in some way, even though
            // inners must have succeeded for us to be here!
            // depending on status1, do errormessage
              if( status1==classnotfounderror)
                requesterrormessage(
                "The specified class name does not exist", execbox );
              else if( status1==triedtonewprimerror )
                requesterrormessage("Can't build a primitive type box on heap",
                   execbox );
              else
                errorexit("illegal errorcode in viewer::step, tpv");
            }
            else
            {// finish up, signal out

              insplist.transfer( ilist, cclist, ni );

              execbox->setState( initialstate );
              reqbox = execbox;
              execbox = owner;
              execbox->moveOutside();

            }// succeeded
          }// both inners are empty
          else
          {
            requesterrormessage(
              "Can't have this box\nnon-empty when previous one is empty",
              tempbox );

          }

        }
        else
        {// have first, maybe first and second waiting to 
          // evaluate, so signal first
          execbox->setState( waittpvstate );
          reqbox = execbox;
          execbox = execbox->whoFirst();
          execbox->moveOutside();

        } // have first
      }// newopbox

      else if( knd==emptybox )
      {// depending on context, take appropriate "empty" action,
        // or produce appropriate "empty" value, or
        // halt with an error
        oknd = owner->whichKind();
        slot = execbox->positionInInnerList(); 

        // if owner expects an actionbox, always okay to do nothing
        if( execbox->actionContext( oknd, slot ) )
        {// fine to do empty action 

				  insplist.add( execbox, signalcolorcode );

          reqbox = execbox;

//          addToInspList( execbox, signalcolorcode );

          execbox = owner;
          execbox->moveOutside();

        }   
				// jb17:  special case --- allow emptybox in ,
				//        catch the miss-match with output later
				else if( oknd == returnbox )
				{// don't pay any attention to it, vis-a-vis tour
				  reqbox = execbox;
					execbox = owner;
					execbox->moveOutside();
				}     

        else
          requesterrormessage("Can't have an empty box here",
                  execbox );
      }// emptybox

      else if( knd==opbox || knd==unopbox )
      {
        // must have an inner, just signal it
        execbox->setState( waittpvstate );
        reqbox = execbox;
        execbox = execbox->whoFirst();
        execbox->moveOutside();

//        addToInspList( execbox, signalcolorcode );

      }// opbox, unopbox

      else
        errorexit("box::step---initialstate, illegal kind");

    } // initial state

    else if( est == waittastate )
    {// standard state of waiting for inners to signal back
      // before taking action

      if(knd==classmethodbox || 
         knd==instancemethodbox)
      {
        errorexit("in newer work, these guys never get signaled");
      }

      else if(knd==seqbox)
      {// signaled back by finished inner, decide whether to
        // signal next and continue waiting, or signal out
        // because there are no more inners, and if that
        // signal out should be to self in returnstate

        if( reqbox->whoNext() == NULL )
        {// signaled by last inner, so signal out or to self
          if( owner->whichKind() != classmethodbox &&
              owner->whichKind() != instancemethodbox 
            )
          {// not a code box so signal out
            execbox->setState( initialstate );
            reqbox = execbox;
            execbox = owner;
            execbox->moveOutside();
      
          }
          else
          {// finishing execution of this call because hit end
            // of me
            //  code box realizing is done
            execbox->setState( returnstate );
            reqbox = execbox;
            // leave execbox on same guy
            execbox->moveOutside();
          }

        }
        else
        {// signal next
          //  execbox->setState( waittastate ); --- already there
          tempbox = execbox;
          execbox = reqbox->whoNext();
          execbox->moveOutside();
          reqbox = tempbox; // previous execbox---me, the owner

          //  addToInspList( execbox, signalcolorcode );

        }

      }// seqbox

      else if(knd==branchbox)
      {// if signaled by a completing action inner, which is
        // even or last slot, done

        slot = reqbox->positionInInnerList(); 

        if( reqbox->whoNext() == NULL || (slot % 2 == 0 ) )
        {// signaled by an action inner, so signal out---we're
          // done --- and now strip all my value box conditionals

           execbox->stripAllBranchboxCondValues();

           execbox->setState( initialstate );
           reqbox = execbox;

           //  addToInspList( execbox, signalcolorcode );

           execbox = owner;
           execbox->moveOutside();

        }// action inner is done
        else
        {// signaled by a conditional box, either signal next
          // or next-next, depending

          // returns 0, 1 if reqbox has type ? or boolean and
          // non-empty value, returns a negative code if is error
          status1 = reqbox->isTrue();

          if( status1 < 0 )
          {// error
            if( status1==invalidbooleanerror)
                requesterrormessage(
                "Must have a boolean value here", reqbox );
              else
                errorexit("illegal errorcode in viewer::step, branch");
          }
          else
          {// have good boolean value, signal in
            tempbox = execbox; // note so can set reqbox
            execbox = reqbox->whoNext();  // one ahead
            if( status1 == 0 )
              execbox = execbox->whoNext();  // two ahead if false

            execbox->moveOutside();
            reqbox = tempbox; // previous execbox---me, the owner

            //  addToInspList( execbox, signalcolorcode );

          }// conditional box has good value
        }// signaling back in

      }// branchbox

      else if(knd==whilebox || knd==dobox || knd==forbox )
      {// depending on which knd and slot of signaling inner,
        // signal next, signal out, signal back up somewhere

        slot = reqbox->positionInInnerList(); 

        if( ( knd==whilebox && slot==1 ) ||
            ( knd==dobox && slot==2 ) ||
            ( knd==forbox && slot==2 )
          )
        {// signaled by the exit clause box, check out its value
          // to decide whether to signal out (I'm done), or
          // signal next of reqbox or back up

          status1 = reqbox->isTrue();
               reqbox->stripValue();  // so can see it eval next time
                                      // around
          if( status1 < 0 )
          {// error
            if( status1==invalidbooleanerror)
                requesterrormessage(
                "Must have a boolean value here", reqbox );
              else
                errorexit("illegal errorcode in viewer::step,loops");
          }
          else
          {// have good boolean value, signal out or on

            if( status1 == 0 )
            {// condition was false, exit the loop by signaling out
              execbox->setState( initialstate );
              reqbox = execbox;

              //  addToInspList( execbox, signalcolorcode );

              execbox = owner;
              execbox->moveOutside();

            }
            else
            {// depending on knd, signal on in the loop
              if( knd==dobox )
              {// loops up to first
                tempbox = execbox;
                execbox = execbox->whoFirst();
                execbox->moveOutside();
                reqbox = tempbox;
 
                //  addToInspList( execbox, signalcolorcode );

              }
              else
              {// signal next
                tempbox = execbox;
                execbox = reqbox->whoNext();
                execbox->moveOutside();
                reqbox = tempbox; // previous execbox---me, the owner
        
                //  addToInspList( execbox, signalcolorcode );
                
              }
            }// signal on inside loopbox

          }// conditional box has good value

        }// conditional case

        else if( (knd==whilebox && slot==2 ) ||
                 (knd==forbox && slot==4 )
               )
        {// signal back up
          tempbox = execbox;
          execbox = execbox->whoFirst();
          if( knd==forbox )
            execbox = execbox->whoNext();
          execbox->moveOutside();
          reqbox = tempbox;

          //  addToInspList( execbox, signalcolorcode );

        }// signal back up

        else
        {// signal next
          tempbox = execbox;
          execbox = reqbox->whoNext();
          execbox->moveOutside();
          reqbox = tempbox; // previous execbox---me, the owner

          //  addToInspList( execbox, signalcolorcode );

        }// signal next

      }// loop boxes

      else if(knd==assbox)
      {// signaled back by finished inner, decide whether to
        // signal next and continue waiting, or do its
        // work and signal out
        // because all the inners have executed

        if( reqbox->whoNext() == NULL )
        {// signaled by last inner, so act and signal out

          badbox = execbox->evaluate( universe, 0, status1, 
					                        ilist, cclist,  ni );

          if( status1 <= 0 )
          {
            if( status1 == illegalassoperror )
              requesterrormessage(
              "This is not a legal\nassignment operator", execbox );
            else if( status1 == incomptypeserror )
              requesterrormessage(
              "The types don't match for the requested storage operation",
                   execbox );
            else if( status1 == divbyzeroerror )
              requesterrormessage(
              "Can't divide by 0", execbox );
            else if( status1 == intrangeerror )
              requesterrormessage(
              "The resulting value is too large for an int", execbox );
            else if( status1 == nofloatmoderror )
              requesterrormessage(
              "Can't do remainder operator with float operands", 
                      execbox );
            else if( status1 == cantjumpemptyerror )
              requesterrormessage(
              "Can't modify because not initialized",
                    execbox->whoFirst() );
            else if( status1 == datanotiniterror )
              requesterrormessage(
              "The value to store has not been initialized",
                (execbox->whoFirst())->whoNext() );
            else if( status1 == badtargetlocationerror )
              requesterrormessage(
              "This box must be a valid location",
                badbox );
            else
              errorexit("errorcode not handled in assbox");
          }
          else
          {// finish up, signal out

					  insplist.transfer( ilist, cclist, ni );

            execbox->setState( initialstate );
            reqbox = execbox;
            execbox = owner;
            execbox->moveOutside();

          }// ass succeeded

        }// inners are ready
        else
        {// signal next
          //  execbox->setState( waittastate ); --- already there
          tempbox = execbox;
          execbox = reqbox->whoNext();
          execbox->moveOutside();
          reqbox = tempbox; // previous execbox---me, the owner

          //  addToInspList( execbox, signalcolorcode );

        }

      }// assbox

      else if(knd==growbox)
      {// signaled back by the one inner, it's got a
        // location ready
        badbox = execbox->evaluate( universe, 0, status1, 
				                                   ilist, cclist,  ni );

        if( status1 <= 0 )
        {
          if( status1 ==  illegalgrowoperror )
            requesterrormessage(
            "The operator must be ++ or --", execbox );
          else if( status1 == notintegererror )
            requesterrormessage(
            "This box must have type int", execbox );
          else if( status1 == argnotlegalinterror )
            requesterrormessage(
            "This box doesn't hold a legal int to grow", 
               badbox );
          else if( status1 == intrangeerror )
            requesterrormessage("The value is too large for an int",
                            badbox );
          else
            errorexit("errorcode not handled in growbox");
        }
        else
        {// finish up, signal out

				  insplist.transfer( ilist, cclist, ni );

          execbox->setState( initialstate );
          reqbox = execbox;
          execbox = owner;
          execbox->moveOutside();

        }// growbox succeeded

      }// growbox

      else if( knd == returnbox )
      {// must be that my one inner has signaled back, it has
        // a value/type ready (not location)

        badbox = execbox->evaluate( universe, 0, status1,
				                             ilist, cclist,  ni );

        if( status1 <= 0 )
        {
          if( status1 ==  mustreturnvalueerror )
            requesterrormessage(
            "This box must produce a value to return", badbox );
          else if( status1 ==  returningwrongtypeerror )
            requesterrormessage(
            "The type returned must match the output type", badbox );
          else
            errorexit("errorcode not handled in growbox");
        }
        else
        {// finish up, signal my codebox (returnbox evaluation
          //  stuck in the value/type in the codebox

				  insplist.transfer( ilist, cclist, ni );

          execbox = execbox->findCodebox();
          execbox->setState( returnstate );
          reqbox = execbox;

        }// returnbox succeeded
      }

      else
        errorexit("illegal kind in waittastate");

    }// waitta state

    // only difference between these two states is in
    //  how they ask execbox to evaluate, so lump them
    //  together
    else if( est == waittpvstate || est == waittplstate )
    {
      if( knd==newopbox )
      {
        tempbox = reqbox->whoNext();

        if( tempbox == NULL  || 
            tempbox->whichKind() == emptybox )
        {// signaled by last non-empty inner, so act and signal out

          // ask for evaluation with 1 meaning first inner was
          // non-empty, 2 meaning both were non-empty

          // reqbox is the last inner to be evaluated, if
          // is second, have two non-empty
          if( reqbox->whoNext() != NULL  )
            badbox = execbox->evaluate( universe, 1, status1,
						                               ilist, cclist,  ni );
          else
            badbox = execbox->evaluate( universe, 2, status1,
						                                ilist, cclist,  ni );

          if( status1 <= 0 )
          {// evaluation failed in some way, even though
            // inners must have succeeded for us to be here!
            // depending on status1, do errormessage
            if( status1==argnotlegalinterror)
              requesterrormessage(
              "This is not a legal integer value", badbox );
            else if( status1==negsizerequestederror )
              requesterrormessage("Can't build array with negative number of elements",
                badbox );
            else if( status1==nosuchtypeerror )
              requesterrormessage("There is no such class or primitive type",
                badbox );
            else
              errorexit("illegal errorcode in viewer::step, tpv");
          }
          else
          {// finish up, signal out

            insplist.transfer( ilist, cclist, ni );

            execbox->setState( initialstate );
            reqbox = execbox;

            execbox = owner;
            execbox->moveOutside();

          }// succeeded

        }// inners are ready
        else
        {// signal next
          //  execbox->setState( waittpvstate ); --- already there
          tempbox = execbox;
          execbox = reqbox->whoNext();
          execbox->moveOutside();
          reqbox = tempbox; // previous execbox---me, the owner

        }

      }// newopbox

      else if( knd == memberbox )
      {// the first and only inner has evaluated to get
        // a value (or class name), now execbox evaluates to produce 
        // a location or a value, depending on how it's waiting
        if( est == waittplstate )
          badbox = execbox->evaluate( universe, 0, status1,
					                              ilist, cclist,  ni );
        else
          badbox = execbox->evaluate( universe, 1, status1,
					                             ilist, cclist,  ni );

        if( status1 <= 0 )
        {// evaluation found error
          if( status1== nosuchclassmembererror)
            requesterrormessage(
            "There is no such class member", execbox );
          else if( status1== datanotiniterror)
            requesterrormessage(
            "The specified data has not been initialized", execbox );
          else if( status1== methodnamenotvalueerror)
            requesterrormessage(
            "Can't have a method name here", execbox );
          else if( status1== nosuchinstancedataerror)
            requesterrormessage(
            "There is no such data member", execbox );
          else if( status1== nosuchinstancemembererror)
            requesterrormessage(
            "There is no such member of an instance of this class",
               execbox );
          else if( status1== notvalidheapaddresserror)
            requesterrormessage(
            "This is not a valid heap address", execbox );
          else if( status1== canthavemethodreferror)
            requesterrormessage(
            "Can't have a method reference here", execbox );
					else if( status1 == cantbearrayerror )
					  requesterrormessage(
						"Can't be a reference to an array", execbox );
          else
            errorexit("illegal errorcode in viewer::step, memberbox");
        }
        else
        {// finish up, signal out
  
	        insplist.transfer( ilist, cclist, ni );

          execbox->setState( initialstate );
          reqbox = execbox;

          //  addToInspList( execbox, signalcolorcode );

          execbox = owner;
          execbox->moveOutside();

        }// succeeded
      }// memberbox producing location or value

      else if( knd == arraybox || knd == array2dbox )
      {
        if( reqbox->whoNext() == NULL )
        {// signaled by last inner, so all inners are ready to go

          if( est == waittplstate )
            badbox = execbox->evaluate( universe, 0, status1,
						                              ilist, cclist,  ni );
          else
            badbox = execbox->evaluate( universe, 1, status1,
						                               ilist, cclist,  ni );

          if( status1 < 0 )
          {// process returned error code
            if( status1== notagridboxerror)
              requesterrormessage(
              "This is not referring to an array in the heap", 
                        badbox );
            else if( status1== argnotlegalinterror)
              requesterrormessage(
              "This is not an integer", badbox );
            else if( status1== nonemptysecondfor1derror)
              requesterrormessage(
              "The array is 1D, can't have second index", badbox );
            else if( status1== colindextoolargeerror)
              requesterrormessage(
              "The column index is too large for this array", badbox );
            else if( status1== rowindextoolargeerror)
              requesterrormessage(
              "The row index is too large for this array", badbox );
            else if( status1== indextoolargeerror)
              requesterrormessage(
              "The index is too large for this array", badbox );
            else if( status1== negsizerequestederror)
              requesterrormessage(
              "Can't have a negative index", badbox );
						// jb18:  catching string faking as an array
            else if( status1== notarraytypeerror )  
              requesterrormessage(
              "Does not have a reference to an array as the type", 
							badbox );
            else
              errorexit("illegal errorcode in step, callbox, wtcstate");
          }
          else
          {// succeeded, go on

            insplist.transfer( ilist, cclist, ni );

            execbox->setState( initialstate );
            reqbox = execbox;

            //  addToInspList( execbox, signalcolorcode );

            execbox = owner;
            execbox->moveOutside();

          }// succeeded
        }// evaluate the array reference
        else
        {// signal next inner
            // stay in waittplstate or waittpvstate
          tempbox = execbox;
          execbox = reqbox->whoNext();
          execbox->moveOutside();
          reqbox = tempbox; // previous execbox---me, the owner

          //  addToInspList( execbox, signalcolorcode );

        }// continue inside

      }// array boxes

      else if( knd == opbox || knd == unopbox )
      {// if signaled by last inner, evaluate, else signal back in

        if( reqbox->whoNext() == NULL )
        {// signaled by last inner, so all inners are ready to go
      
          badbox = execbox->evaluate( universe, 1, status1,
					                              ilist, cclist,  ni );

          if( status1 <= 0 )
          {
            if( knd == opbox )
            {
              if( status1== illegaloperatorerror)
                requesterrormessage(
                "There is no such binary operator", badbox );
              else if( status1== illegaloperanderror)
                requesterrormessage(
                "The value of this operand is illegal", badbox );
              else if( status1== incomptypeserror)
                requesterrormessage(
                "The types of the operands are incompatible for this operator", badbox );
              else if( status1== cantcoercetofloaterror)
                requesterrormessage(
                "This operand's value is not a legal float value", badbox );
              else if( status1== divbyzeroerror)
                requesterrormessage(
                "Can't divide by 0", badbox );
              else if( status1== intrangeerror)
                requesterrormessage(
                "This value is too large for an int", badbox );
              else
              {
                errorexit("illegal errorcode in viewer::step, opbox");
              }
            }
            else
            {
              if( status1== illegaloperatorerror)
                requesterrormessage(
                "There is no such unary operator", badbox );
              else if( status1== illegaloperanderror)
                requesterrormessage(
                "The value of this operand is illegal", badbox );
              else if( status1== incomptypeserror)
                requesterrormessage(
                "The type of this operand is incompatible with this operator", 
                       badbox );
              else
                errorexit("illegal errorcode in viewer::step, unopbox");
            }

          }// hit error
          else
          {// finish up, signal out

            insplist.transfer( ilist, cclist, ni );

            execbox->setState( initialstate );
            reqbox = execbox;

            execbox = owner;
            execbox->moveOutside();

          }// succeeded
        }
        else
        {// signal next inner
            // stay in waittpvstate
          tempbox = execbox;
          execbox = reqbox->whoNext();
          execbox->moveOutside();
          reqbox = tempbox; // previous execbox---me, the owner
    
        }// continue inside
        
      }// opbox
      
      else
        errorexit("illegal knd in waittpvstate in viewer::step");

    }// wait to produce value/location states

    else if( est == waittocallstate )
    {
      if( knd != callbox )
        errorexit("Only callbox has special waittocallstate");
      else
      {// see if last inner
        if( reqbox->whoNext() == NULL )
        {// signaled by last inner, so all inners are ready to go

          badbox = execbox->evaluate( universe, 0, status1,
					                              ilist, cclist,  ni );

          if( status1 < 0 )
          {// process returned error code
            if( status1== illegalmethodreferror)
              requesterrormessage(
              "This is not a legal method reference", badbox );
            else if( status1== argsnotequalparamserror)
              requesterrormessage(
              "The number of arguments is different from the number of parameters", badbox );
            else if( status1== incompatargparamerror)
              requesterrormessage(
              "This argument has wrong type for its parameter", badbox );
            else if( status1 == need2argserror )
              requesterrormessage(
              "This system method requires two arguments",
              badbox );
            else if( status1 == need1argerror )
              requesterrormessage(
              "This system method requires one argument",
              badbox );
						else if( status1 == nosuchcharerror )
						  requesterrormessage(
							"The argument must be between 0 and 255 inclusive", 
							  badbox );
            else if( status1 == neednoargserror )
              requesterrormessage(
              "This system method has no arguments",
              badbox );
            else if( status1 == notintegererror )
              requesterrormessage(
              "This argument must have an integer value", badbox );
            else if( status1 == notfloaterror )
              requesterrormessage(
              "This argument must have a float value", badbox );
            else if( status1 == notcharerror )
              requesterrormessage(
              "This argument must have a char value", badbox );
            else if( status1 == illegalcursorlocationerror )
              requesterrormessage(
              "The cursor location specified is illegal", badbox );

            else if( status1 == cantcoercetofloaterror )
						  requesterrormessage(
							"The argument must be a number", badbox );
						else if( status1 == negativeargerror )
						  requesterrormessage(
							"The argument must be non-negative", badbox );
						else if( status1 == argoutofrangeerror )
						  requesterrormessage(
							"The argument is out of the legal range", badbox );
						else if( status1 == notastringerror )
						  requesterrormessage(
							"The argument must be a string", badbox );
						else if( status1 == notachararrayerror )
						  requesterrormessage(
							"The argument must be a char array", badbox );
						else if( status1 == notfloatformerror )
						  requesterrormessage(
							"The argument must have form of a float value", badbox );
						else if( status1 == notintegerformerror )
						  requesterrormessage(
							"The argument must have form of an integer value", 
							           badbox );
						else if( status1 == badstringindexerror )
						  requesterrormessage(
							"Invalid index for the string", badbox );

					  else if( status1 == openerrornot3args )
						  requesterrormessage(
							 "Must have three arguments (port, input or output, file name)",
							      badbox );
					  else if( status1 == openerrorportbusy )
						  requesterrormessage(
							 "The port is already open", badbox );
					  else if( status1 == openerroriochoice )
						  requesterrormessage(
        		 "This argument must be one of the strings `input' or `output'", 
          							 badbox );
					  else if( status1 == portrangeerror )
						  requesterrormessage(
							 "This is a not a legal port number", badbox );

					  else if( status1 == portnotreadingerror )
						  requesterrormessage(
							 "This port is not open for input", badbox );
					  else if( status1 == endoffileerror )
						  requesterrormessage(
							 "End of file has been reached", badbox );
					  else if( status1 == notcharerror )
						  requesterrormessage(
							 "Argument is not a char value", badbox );
					  else if( status1 == portnotwritingerror )
						  requesterrormessage(
							 "This port is not open for output", badbox );
					  else if( status1 == portnotopenerror )
						  requesterrormessage(
							 "The port is not open, so can't be closed", badbox );
					  else if( status1 == progcharrangeerror )
						  requesterrormessage(
							 "Only codes 133 to 255 can be programmed", badbox );
					  else if( status1 == badprogcharstringerror )
						  requesterrormessage(
							 "This string can't be interpeted to draw a symbol", 
							  badbox );
						// v13:  _print can have a bad argument
            else  if( status1== badprimvalueerror)
              requesterrormessage(
              "Arguments to _print must be primitive types", badbox );
						// jb17:
            else  if( status1== notvalidheapaddresserror)
              requesterrormessage(
              "Argument to _destroy must be a valid heap reference", badbox );
            else  if( status1== notininstancemethoderror )
              requesterrormessage(
              "Can only use _this in an instance method", badbox );
            // jb18: if java translate failed, error already done
						else if( status1==erroralreadyrequested )
						{ }

            else
              errorexit("illegal errorcode in step, callbox, wtcstate");
          }
          else if( status1 == 2 )
          {// special call happened immediately --- system method
            // that doesn't produce a value

            // v15:  realized had this check on non-system methods,
						//       needed it on system ones!
 			      if( ! execbox->actionContext( owner->whichKind(),
           			          execbox->positionInInnerList() ) 
						     &&
						     execbox->typeNotInit()
				      )
				    {// error---supposed to produce value but didn't
				      requesterrormessage(
				      "The method call needed to return a value here", execbox );
				    }
						else
						{

  						// even though doesn't produce a value, might want
	  					// to have somebody on tour
		  				insplist.transfer( ilist, cclist, ni );

              execbox->setState( initialstate );
              reqbox = execbox;
              execbox = owner; 
              execbox->moveOutside();  // just in case

            }

          }// status1 == 2

          else if( status1 == 3 )
          {// system method that produces a value

						insplist.transfer( ilist, cclist, ni );
					  
            execbox->setState( initialstate );
            reqbox = execbox;
            //  addToInspList( execbox, signalcolorcode );
            execbox = owner; 
            execbox->moveOutside();  // just in case
          }
          else if( status1 == 4 || status1 == 5 )
          {// system methods that switch into input mode

					  // no touring in these special input cases

            execbox->setState( waittogetinputstate );
            reqbox = execbox;
            if( status1 == 4 )
            {
              actionmode = input1mode;
            }
            else
            {
              actionmode = inputlinemode;
            }
          }
					else if( status1 == 6 )
					{// _halt signaled back, so request halt
				     actionmode = haltmode;  
					}
          else
          {// succeeded, go on

            insplist.transfer( ilist, cclist, ni );

            execbox->setState( waittoreturnstate );
            reqbox = execbox;

            // turn over control to codebox of first guy in stack
            execbox = universe->findCallStart();
            execbox->moveOutside();

          }// succeeded
        }// start the call
        else
        {// signal next inner
            // stay in waittocallstate
          tempbox = execbox;
          execbox = reqbox->whoNext();
          execbox->moveOutside();
          reqbox = tempbox; // previous execbox---me, the owner
        }// go on inside

      }// is a callbox
    }// waittocallstate

    else if( est == waittoreturnstate )
    {
      if( knd != callbox )
        errorexit("Only callbox has special waittoreturnstate");
      else
      {// callbox signaled back---change state and signal out
       // just like anybody who has just finished up

			  // v10:  if execbox is in a context that needs a value,
				//       and value is null (type, actually), raise error
				if( ! execbox->actionContext( owner->whichKind(),
                      			          execbox->positionInInnerList() ) 
						&&
						execbox->typeNotInit()
				  )
				{// error---supposed to produce value but didn't
				  requesterrormessage(
				 "The method call needed to return a value here", execbox );
				}
				else
				{// proceed with value or value not needed
          execbox->setState( initialstate );
          reqbox = execbox;

          // show value just returned
          //  addToInspList( execbox, signalcolorcode );

          execbox = owner;
          execbox->moveOutside();
				}

      }// handle callbox being signalled back

    }// waittoreturnstate

    else if( est == returnstate )
    {
      if( knd != seqbox ||
          (owner->whichKind() != classmethodbox &&
           owner->whichKind() != instancemethodbox )
        )
        errorexit("shouldn't have seqbox in returnstate");

      // execbox is the codebox, has signaled itself or been
       // signaled by a returnbox, know that execution of this
     // call is over

        // this code is only here, but want full access to box reality
        execbox = execbox->returnFromCall( ilist, cclist, ni );

        stack->removeFirst();  // remove the top stack guy

        if( execbox == NULL )
        {// have returned from the last call, stop execution
          stack->fixupReality(0);
          actionmode = haltmode;  // just to signal desire

          smq->insert( MQ_REQUEST, MQ_HALT, "halt", 0, 0 );

          return;  // kick on out

        }
        else
        {// continue with execution on execbox, which will be
          // the callbox that has been in "waittoreturnstate"
          // patiently waiting

  				insplist.transfer( ilist, cclist, ni );

          execbox->fixupReality(1);  // refigure after stick in
                                    // value and pop
            // had bug here, with (0) above, when not returning
            // a value, the fixupReality(0) did nothing, didn't
            // spiff up the stack box

          reqbox = NULL;
          execbox->moveOutside();

        }
        
    }// returnstate

    else if( est == waittogetinputstate )
    {// execbox is a call box to _get or _getLine that wants
      // to get global "inputstring" that is waiting to be its
      // value

      execbox->storeInputString(inputstring, 
			                            ilist, cclist, ni );
                              // do all the steps
                              // that evaluate would have done had
                              // the string been available before
      
			// storeInputString is similar to evaluate
			insplist.transfer( ilist, cclist, ni );

      execbox->setState( initialstate );
      //  addToInspList( execbox, signalcolorcode );
      reqbox = execbox;
      execbox = owner; 
      execbox->moveOutside();  // just in case

    }

    // jb16:  add in special state for javabox waiting to 
		//        be signaled back by its single inner, then
		//        transfer value, if any, and signal out
		else if( est = javawaitstate )
		{
      if( execbox->isAction() )
			{// the javabox translated to a statement
        execbox->setState( initialstate );
        reqbox = execbox;
        execbox = owner; 
        execbox->moveOutside(); 
			}
			else
			{// the javabox translated to a value, need to move the value
			 // before signalling out

			  // actually "move" the value string
				execbox->promoteInnerValue();

        execbox->setState( initialstate );
        reqbox = execbox;
        execbox = owner; 
        execbox->moveOutside(); 
			}

		}// javawaitstate

    else
      errorexit("illegal state in viewer::step");

    // have inspection list info now, return happily

// jb1a:  decided to do this inside executing mode, depending
    //mq->insert( MQ_REQUEST, MQ_STEP, "step", 0, 0 );

  } // viewer::step

  // need "box to draw"  (btd) to put in bounding box
  void viewer::startDrawToTeX( box* btd )
  { 
    // move to next snapshot code
		snapshotnumber++;

    char chopfilename[100];
		strcpy( chopfilename, univfilename );
		chopfilename[ strlen( univfilename ) - 4 ] = '\0';

		if( pictex )
		{
		  sprintf( texfilename, "%s-%d.tex", chopfilename, 
			           snapshotnumber );

      texfile = fopen( texfilename, "w" );

      if( texfile != NULL )
      {// have a file opened to write to, write the starting lines to it
        texdraw = 1;
        fprintf( texfile, "\\beginpicture\n" );
        fprintf( texfile, "\\setcoordinatesystem units <%g pt,%g pt>\n",
                  xscale, yscale );
				fprintf( texfile, "\\setlinear \n" );
      }
		  else
  		  errorexit("couldn't open texfile");
		}
		else
		{// postscript
  		sprintf( texfilename, "%s-%d.eps", chopfilename, 
		            snapshotnumber );	

      texfile = fopen( texfilename, "w" );

      if( texfile != NULL )
      {// have a file opened to write to, write the starting lines to it
        texdraw = 1;
        fprintf( texfile, "%%!PS-Adobe-3.0 EPSF-3.0\n" );
        fprintf( texfile, "%%%%BoundingBox: %d %d %d %d\n",
                    0, 0,
		// jb27:  new ghostview doesn't like non integer
		//        values:
		     (int) (xscale* btd->howWide()+0.5) ,
			 (int) (yscale * btd->howHigh()+ 0.5) );

		// jb27:  couldn't find the font, leave out, not necessary?:
        // load the font to use
        fprintf( texfile, "%%/Times-Roman findfont\n");
        fprintf( texfile, "%%120 scalefont\n");
        fprintf( texfile, "%%setfont\n");

        // since inside viewer, do scaling from the viewer:
        fprintf( texfile, "%g %g scale\n", 
                          xscale, yscale );
      }
		  else
  		  errorexit("couldn't open texfile");

    }// postscript

  } // startDrawToTeX

  void viewer::finishDrawToTeX()
  {
    // write finishing lines to texfile
		if( pictex )
		{
		  fprintf( texfile, "\\endpicture\n" );
		}
		else
		{// postscript
      fprintf( texfile, "%% End snapshot\n" );
		}
    
		  // close the file
      fclose( texfile );

    texdraw = 0;  // turn it off for future sizeLocDisplay calls
  } 

//*****************************************
// managerial global variables

const int maxviewer = 8;
viewer v[maxviewer];  // provide for up to 8 viewers
              // manager keeps track of who's actually
              // on-screen, processes all key strokes,
              // knowing who is the "live" viewer, if any,
              // or whatever


// jb1a:  when regular key is hit, put it in message queue
//        as kind 1
void processRegularKey( unsigned char key, int x, int y )
{

//std::cout << "got key " << (int) key << std::endl;

  // kludge to make ctrl-z do snapshot
  if( key == 26 && (glutGetModifiers() & GLUT_ACTIVE_CTRL) )
  {
//std::cout << " got key = " << key << " and simulated snapshotkey" << 
//           std::endl;
    mq->insert( MQ_ALT_KEY, snapshotKey, "alt key", x, y );
    return;
  }

// v10:  changed so alt keys go into meta buffer
  if( glutGetModifiers() & GLUT_ACTIVE_ALT )
	{// got a regular key but with alt shift down

    mq->insert( MQ_ALT_KEY, key, "alt key", x, y );
	
  }
	else
	{// regular key
  
    if( key == 9 && 
			    !(glutGetModifiers() & GLUT_ACTIVE_CTRL)
			)
		{// need to distinguish between tab and ctrl-i
		  key = tabkey;
		}
    // jb7a:  make delete key (code 127) be ctrl-c
    else if( key == deletekey )
    {
      key = toggleviewskey;
    }

    mq->insert( MQ_REG_KEY, key, "reg key", x, y );

  }

}// processRegularKey

// some "special" keys immediately get converted to
// their aliases and put in the regular key buffer,
//   others are really special, go into special key buffer
void processSpecialKey( int key, int x, int y )
{
// v10:  treat alt'd special keys separately
  if( glutGetModifiers() & GLUT_ACTIVE_ALT )
	{// alt'd special key
    std::cout << "fatal error---are no alt special keys" 
              <<  std::endl;
    exit(1);
	}
	else
	{// unshifted special key

    // translate some special keys to regular equivalents

    int code;  // flag not changed, or changed

    if(key==GLUT_KEY_LEFT)
      code = prevkey;
    else if(key==GLUT_KEY_RIGHT)
      code = nextkey;
    else if(key==GLUT_KEY_UP)
      code = outkey;
    else if(key==GLUT_KEY_DOWN)
      code = inkey;
    else if(key==GLUT_KEY_INSERT)
      code = jumpkey;
    else
      code = 0; 

    if( key != GLUT_KEY_F4 )
    {// regular priority 

      if( key == GLUT_KEY_F6 && lesson->getState()==readingLessonState )
      {// special---F6 aborts reading
        mq->removeAll();
        smq->removeAll();
        smq->insert( MQ_HALT, 0, "user halted lesson by F6", 0, 0 );
      }
      else
      {// all function keys except F4 go into regular message queue mq
        if( code != 0 )
          mq->insert( MQ_REG_KEY, code, "reg key", x, y );
        else
          mq->insert( MQ_SPEC_KEY, key, "spec key", x, y );
      }
    }
    else
    {// high priority keys
      smq->insert( MQ_SPEC_KEY, key, "spec high priority key", x, y );
    }

  }// unshifted special key

}// processSpecialKey

// set globals in response to mouse click
//   Is a one-position queue, really!
void processMouseButton( int button, int state,
                             int x, int y )
{
  // left button put down
	//   jb21: took out jb19 lesson stuff, here and below, for sure won't use
  if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN 
    )
    {
      mousey = screenheight - y;  // flip it to opengl orientation
      mousex = x;

      mq->insert( MQ_CLICK, MQ_LEFT_BUTTON, 
                              "left click", mousex, mousey );

  }

   // right button put down
   else if(button==GLUT_RIGHT_BUTTON && state==GLUT_DOWN
				  )
    {
      mousey = screenheight - y;
      mousex = x;

      mq->insert( MQ_CLICK, MQ_RIGHT_BUTTON, 
            "right click", mousex, mousey );

    }
 
}// processMouseButton

void display();  // forward reference to main display function

// notice that mouse has moved with button down to
// (x,y) --- if was left button, and has moved far enough,
// shift reality and ask for a redraw
void dragMouse( int x, int y )
{
  // jb25:  split into lesson states, allow dragging only where we did
  //        before and in appropriate new states
  if( (
       lesson->getState()==disabledLessonState &&
       (
        (mouseviewer==interactiveviewer && actionmode==interactivemode)
        ||
        (mouseviewer==executionviewer && actionmode==pausedmode)
       )
      )
      ||
      lesson->getState()==movingLessonState
      ||
      lesson->getState()==editingLessonState
      || 
      // jb2a:  allow mouse dragging while reading?
      lesson->getState()==readingLessonState
	  )
  {
    y = screenheight-y;
    v[mouseviewer].translate( x-mousex, y-mousey );
    mousex = x;  mousey = y;
    needredraw = 1;
  
    if( lesson->getState()==readingLessonState )
      lesson->invalidateNotes();

	  // had this before:  glutPostRedisplay();
// jb5a: no!		display();  // just call it now
    glutPostRedisplay();

  }
}

// whenever the mouse moves without a button down,
// update its location in global variables 
void updateMouse( int x, int y )
{
  mousex = x;  mousey = screenheight - y;
}

// determine which viewer holds the position (px,py)
// and
// produce vx,vy as that location relative to that viewer,
// in scaled viewer coordinates,
//   return -1 if no viewer is under the location
int locateLocationInViewer(float px, float py, float& vx, float& vy)
{
  int found = 0;

  int k = 0;

  while( !found && k<maxviewer )
  {
    found = v[k].locateLocation( px, py, vx, vy );
    if(!found)
      k++;
  }

  if(found)
    return k;
  else 
    return -1;

}

// using globals, set viewer sizes and locations to reflect
// possibly changed value for screen size, rescale if says to
// rescale all or is console 
void setviewers( int scaleall )
{
  // change sizes of the viewers to reflect new screen size

  //v11:
	if( !userview )
	{// standard views
    if( !splitviewing )
    {// using full screen
      v[interactiveviewer].setSizeLocScale( scaleall,
          screenwidth, screenheight, 0, screenheight, 1.0, 1.0 );
      v[executionviewer].setSizeLocScale( scaleall,
                   screenwidth, screenheight,
                  0, screenheight, 1.0, 1.0 );
    }
    else
    {
      v[interactiveviewer].setSizeLocScale( scaleall,
          leftscreenwidth, screenheight, 0, screenheight, 1.0, 1.0 );
      v[executionviewer].setSizeLocScale( scaleall,
                   leftscreenwidth, screenheight,
                  0, screenheight, 1.0, 1.0 );
    }

    // jb19: set up lessonviewer same as others
    v[lessonviewer].setSizeLocScale( scaleall,
        screenwidth, screenheight, 0, screenheight, 1.0, 1.0 );

    float tempscale = chargrid->consoleScale( consolewidth,
                                            consoleheight );

    v[consoleviewer].setSizeLocScale( 1,
               consolewidth, consoleheight, 
               leftscreenwidth, screenheight, 
                        tempscale, tempscale );

    //jer
    // instead of scaleall passed in, force rescaling
	  
	  v[portsviewer].setSizeLocScale( 1,
                     consolewidth, screenheight - consoleheight, 
                     leftscreenwidth, 
										    screenheight - consoleheight, 
                        1.5*tempscale, 1.5*tempscale );
  }// end of standard viewers
	else
	{// doing userview
    float tempscale = chargrid->consoleScale( screenwidth,
                                              screenheight );

    v[userviewer].setSizeLocScale( 1, 
               screenwidth, screenheight,
               0, screenheight, 
               tempscale, tempscale );
		         
	}

}

// utility function

// produce loc string for current mouse
mystring* getLocstringFromMouse()
{
  int whichviewer;
  float tx, ty;
  char tpart;
  int tindex;
  float vx, vy;
  box* targetbox;

  mystring* locstring;
  mystring* tempstring;

  whichviewer = locateLocationInViewer(mousex, mousey, vx, vy);

  // depending on which viewer, try to find box the mouse is over
  if( whichviewer == consoleviewer )
  {// click over consoleviewer
    locstring = new mystring( "?c" );
    tempstring = v[consoleviewer].getPercentageOffsets( mousex, mousey);
    locstring->append( tempstring );
    tempstring->destroy();
  }
  else if( whichviewer == portsviewer )
  {// click over portsviewer
    locstring = new mystring( "?p" );
    tempstring = v[portsviewer].getPercentageOffsets( mousex, mousey);
    locstring->append( tempstring );
    tempstring->destroy();
  }
  else
  {// could be over a box

    targetbox = v[whichviewer].findTargetBox( vx, vy, tpart, tindex );

    if( targetbox == NULL )
    {// over reasonable viewer but not over the universe box,
      // set locstring to default
      locstring = new mystring( "@0:0" );
    }// off the universe
    else
    {// over some box in the universe
      mystring* p;
      // get the info from the box using mouse info
      locstring = targetbox->getLocString( 1, tpart, tindex );
      // append the x and y percentage offsets
      tx = mouseoffx;  ty = mouseoffy;
      p = computePercentageOffsets( targetbox, tx, ty );
      locstring->append( p );
      p->destroy();
    }// over a box
  }

  return locstring;

}// locateBoxUnderMouse

// "doit" is called after each display, 
//  and causes another call to display,
// here's where Boxes gets a chance to
// ask for something to happen, or not, depending

int doitLevel = 1;  // monitor nesting level of calls to doit
int everNeedRedraw;     // if needredraw is set to 1 anywhere in
                        // the sequence of doit calls, note it

long doitCount=0;

void doit()
{

  // jboxes1c:  only ever crashed this way stupidly, when
  //            user shrinks window to task bar, so eliminate
  /*
  GLenum openglerror = glGetError();
  if( openglerror != GL_NO_ERROR )
  {
    std::cout << "Encountered OpenGL error " << 
                gluErrorString( openglerror ) << std::endl;
    exit(1);
  }
  */

  // grab first waiting message, if any

  int mqKind, mqCode;  // store the message info
  mystring* mqLabel;

  if( smq->isEmpty() )
  {// no system messages waiting to process, 
    // so check low priority queue
    if( mq->isEmpty() )
      return;         // no messages of any kind, spin idly
    else
    {// have regular message
      mqKind = mq->getKind(),
      mqCode = mq->getCode();
      mqLabel = mq->getLabel();

      mq->remove();

    }
  }
  else
  {// have system message
 
    mqKind = smq->getKind(),
    mqCode = smq->getCode();
    mqLabel = smq->getLabel();

    smq->remove();

  }

  doitCount++;  // count the calls that aren't idling

  box* newfocus;  // utility 
  // depending on flags, set "focus" for convenience in 
  //  interactive and paused modes
  box* focus;
  needredraw = 0;
  needrefocus= 1;  // only refocus if needredraw,
                    // some actions will turn off the need for
                    // automatic refocusing, some will request
										// special refocusing
 
  if ( lesson->getState() == disabledLessonState ||
       lesson->getState() == recordingLessonState ||
       lesson->getState() == readingLessonState 
     )
  {// normal reality with front and end material conditional
    // on lesson state
    #include "regular.cpp"
  }

  else if( lesson->getState() == authoringLessonState )
  {// neutral authoring state --- grab regular keys for authoring
    #include "authoring.cpp"
  }

  else if( lesson->getState() == pausedLessonState )
  {// recording regular keys to fill in after "pause("
    #include "pausing.cpp"
  }

  else if( lesson->getState() == movingLessonState )
  {// moving freely to change focus during lesson creation
    #include "moving.cpp"
  }

  else if( lesson->getState() == editingLessonState )
  {// manually editing lesson
    #include "editing.cpp"
  }

  else if( lesson->getState() == stoppingLessonState )
  {// special state for displaying message about stopping
    if( mqCode == enterkey )
    {// really stop
      lesson->stopStopping();
      needredraw = 1;
      focus = v[activeviewer].whoFocus();
    }
    else
    {// spin again
      needredraw = 0;
    }
  }

  else
    errorexit("illegal lesson state in doit");

  // do standard display processing no matter 
  // how came through the lesson
  
  if( actionmode != errormode &&
       actionmode != executingmode &&
       actionmode != input1mode && actionmode != inputlinemode
       &&
       needredraw
    )
  {
     // something changed, forcing a redraw, so
      // to be sloppy and safe, make sure everything from
      // focus out is correct

     focus->fixupReality(0);

     if(needrefocus == 1) 
       v[activeviewer].setFocus( 2, focus, NULL );
     else if(needrefocus == 2 )
		   v[activeviewer].setFocus( 4, focus, NULL );
		 else if( needrefocus == 3 )
		   v[activeviewer].setFocus( 5, focus, NULL );

  }

  if( needredraw )
  {
   // jb5a:  no!?   display();
    glutPostRedisplay();
  }

}// end of doit

void reshape( int w, int h )
{
  screenwidth = w;
  screenheight = h;

//      leftscreenwidth = screenwidth - screenheight/2;

  // assumes window approx 3 by 4:
	consolewidth = screenheight * 0.8;
	consoleheight = (consolewidth * 5)/7.0;
    leftscreenwidth = screenwidth - consolewidth;

  setviewers( 0 ); // don't want to rescale all viewers

  needredraw = 1;

  lesson->invalidateNotes();

  // jb5a: no!?  display();
  glutPostRedisplay();

}

// draw the image on the screen, as needed, then 
// call doit to allow a step of action to be done
// --- and doit does a postredisplay to keep things going
// 
//  actionmode determines exactly who we display
//  needredraw determines whether anything at all is done
void display()
{

  if(needredraw)
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // position viewpoint over box reality
    glLoadIdentity();

    // depending on global state variables, draw some stuff

    if( lesson->getState() == editingLessonState )
    {// draw the lessonviewer
      v[lessonviewer].display();
    }

    else if( lesson->getState() == stoppingLessonState )
    {// stopping the lesson reading
      displayerrormessage();  // just for convenience
    }

    else
    {// draw viewers depending on actionmode and userview
      // v11:

      // jb25:  depending on activeviewer and lesson status,
      //        do preparation for drawing notes, namely shifting
      //        the activeviewer appropriately over reality
      //   Might not need to do anything, for efficiency

      // jb6a:  put the check for disabled or editing---hence skip
      //        prepping notes in lesson->prepareNotes and
      //        merge neatly into the logical structure of which
      //        viewers to display---no need to prep unless displaying

      // make sure all notes are updated before "prepare" for any
      // viewer

      float cvw, cvh, pvw, pvh;
      v[consoleviewer].getSize( cvw, cvh );
      v[portsviewer].getSize( pvw, pvh );
      lesson->updateNotes( cvw, cvh, pvw, pvh );

	  	if( !userview )
		  {// doing standard viewers
        if( actionmode == interactivemode )
        {// single interactive viewer --- so has a focus
          if( ! splitviewing )
          {
            lesson->prepareNotes( v[interactiveviewer] );
            v[interactiveviewer].display();
          }
          else
          {
            lesson->prepareNotes( v[interactiveviewer] );           
            v[interactiveviewer].display();
            lesson->prepareNotes( v[consoleviewer] );
            v[consoleviewer].display();
            lesson->prepareNotes( v[portsviewer] );
            v[portsviewer].display();
          } 
        }
        else if( actionmode == executingmode ||
                 actionmode == inputlinemode || 
				    		 actionmode == input1mode )
        {
          if( ! splitviewing )
          {
            lesson->prepareNotes( v[interactiveviewer] );
            v[executionviewer].display();
          }
          else
          { 
            lesson->prepareNotes( v[executionviewer] );
            v[executionviewer].display();
            lesson->prepareNotes( v[consoleviewer] );
            v[consoleviewer].display();
            lesson->prepareNotes( v[portsviewer] );
            v[portsviewer].display();
          }
        }
        else if( actionmode == pausedmode )
        {
          if( ! splitviewing )
          {
            lesson->prepareNotes( v[executionviewer] );
            v[executionviewer].display();
          }
          else
          {
            lesson->prepareNotes( v[executionviewer] );
            v[executionviewer].display();
            lesson->prepareNotes( v[consoleviewer] );
            v[consoleviewer].display();
            lesson->prepareNotes( v[portsviewer] );
            v[portsviewer].display();
          }
        }
        else if( actionmode == errormode )
        {// display the appropriate error message
          displayerrormessage();
        }

        else
          errorexit("no other action modes are implemented");

      }// standard viewers
		  else
		  {// user viewing
        lesson->prepareNotes( v[userviewer] );
		    v[userviewer].display();
		  }

/* jb7a:  put displayNotes in each viewer display
      // jb25:  now, depending on various things, possibly go ahead
      //        and draw the notes on top of everything
      if( lesson->getState() != disabledLessonState &&
          lesson->getState() != editingLessonState          
        )
      {
        lesson->displayNotes();
      }
*/

    }// actionmode based selection of viewer

    // make the drawing visible
    glutSwapBuffers();
    // v13:  this was stupid?  glFlush();

    lesson->validateNotes();  // signal that all notes have been
                             // prepared, hence updated

  }// redisplay was needed

  needredraw = 0;  // just drew, or didn't need to, so now don't
                   // need to until do some more action
}

// set things up at the very beginning
void init()
{
  // set color off edge of universe to almost black (jb5a)
  glClearColor( 0.6, 0.6, 0.6, 0.0 );
//  glClearColor(neutralcolor[0],neutralcolor[1],neutralcolor[2],0.0);

  glEnable( GL_DEPTH_TEST );

  buildsymbolinfo();  // compute widths of all the symbols

  buildconstantstrings();  // build all constant strings for
                           // multiple use

  chargrid = new chargridtype( chargridrows, chargridcols );

  // initialize all viewers

  splitviewing = 0;  

  // init all the viewers (new version:  only does id)
  v[interactiveviewer].init( interactiveviewer );
  v[executionviewer].init( executionviewer );
  v[consoleviewer].init( consoleviewer );
  v[portsviewer].init( portsviewer );
	v[userviewer].init( userviewer );
	v[lessonviewer].init( lessonviewer );

  // set up the size offset and scaling
  setviewers( 1 ); // need to scale all viewers at start 

  v[interactiveviewer].setOnScreen( 'y' );

  actionmode = interactivemode;
  needredraw = 1;  
  
  // load universe from univstream or build from scratch
  
  if( univstream != NULL )
  {// stream is valid, load universe from it

    universe = new box();
    universe->load( univstream );
    universe->moveOutside();
    fclose( univstream );

  }
  else
  {// stream not valid, build standard default initial universe

    universe = new box();
    universe->build( univbox );
  }

  // note globals for convenience:
   // a little init utility that finds and sets stack, heap, console
  // for convenient reference
  box* tempbox = universe->whoFirst();
  while( tempbox->whichKind() == classbox )
    tempbox = tempbox->whoNext();

    stack = tempbox;
    heap = stack->whoNext();
    staticdata = heap->whoNext(); // is now static data
		// v10:
		console = staticdata->whoNext();

  // set up focus and backbox for the viewers
  v[consoleviewer].fixOver( consolebox );
  v[portsviewer].fixOver( portsbox );
  v[interactiveviewer].setFocus( 2, universe, universe );
	// v11:
	v[userviewer].fixOver( consolebox );

  // set color for first error message
	messagecolorcode = errorcolorcode;

  // jb19:  have permanent notes guy
// jb21:  toss it:  	notes = new notegroup();
// jb21:  set up the lesson facility
  lesson = new Lesson( universe, &v[interactiveviewer] );

  mq = new MessageQueue();
  smq = new MessageQueue();

  // only now that things are good to go do we want to
  // have a reshapefunc
  glutReshapeFunc( reshape );

  // let's go!
  // jb5a:  no!?  display();  // just display at the beginning
  glutPostRedisplay();

}

// return whether the tail of filename matches match
int tailEquals( char* filename, char* match )
{
  int n = strlen(match);
  int m = strlen( filename );

  if( m < n )
    return 0;

  int k;
  for( k=0; k<n; k++ )
    if( filename[m-n+k] != match[k] )
      return 0;

  return 1;
}

int main(int argc, char **argv)
{
  std::cout << "(JBoxes version 2006A, copyright 2000--2006 by Gerald Shultz)" 
            << std::endl;
  // jb7a:  allow command line argument
  if( argc == 1 )
  {
    // before
    // getting out of easy console mode, go here:

    std::cout << "Please enter the path/name of the Boxes universe to load or create: "
	          << std::endl;

    std::cin >> univfilename;

    if( tailEquals( univfilename, ".box" ) )
    {
      std::cout << std::endl <<
          "You have entered a file name ending in .box" 
          << std::endl
          << "Please try again with just the universe name" 
          << std::endl
          << "(like `fish' instead of `fish.box')" 
          << std::endl << std::endl;
      exit(0);
    }

    // append .box
  	sprintf( univfilename, "%s.box", univfilename );

  }  
  else if( argc == 2 )
  {// have supposed universe name, including the .box, 
    // probably from double-clicking the universe
    sprintf( univfilename, "%s", argv[1] );
  }
  else
  {
    std::cout << "Can't start JBoxes with more than 1 universe name" 
         << std::endl;
    exit(1);
  }

  char junkstring[81];

  univstream = fopen( univfilename, "r" );
  if( univstream == NULL )
  {
    std::cout << "There is no such universe.  A new universe " <<
		         "will be built."
            << std::endl;
  }
  else
  {
    std::cout << "The universe is being loaded." << std::endl;
 // jb26:  why is this here?   cin.getline( junkstring, 80 );
  }

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
  glutInitWindowSize(screenwidth,screenheight);
  glutInitWindowPosition(0,0);

  int k, j;
  /*
	for( k=0; k< strlen(univfilename)-4; k++ )
	  junkstring[k] = univfilename[k];
	for( j=1; j<=80-strlen(univfilename)-4-36; j++, k++ )
	  junkstring[k] = ' ';
  junkstring[k] = '\0';
  */

  int titleLength = 80;  // number of symbols allowed in title
  char copyright[100];
  sprintf( copyright, "%s%c%s", 
            "        [JBoxes     Copyright ", 169,
            "2000--2006 G. Shultz]" );

	char titlestring[350];  // holds complete title string
  char truncName[300];  // holds the possibly initially 
                        // shortened, definitely last .box removed
                        // universe name
  char spaces[100];   // padding spaces
  int num = strlen(univfilename)-4;  // length of univname
  int start;

  if( num + strlen(copyright) < titleLength )
  {// need padding spaces in between
    for( k=0; k<titleLength-num-strlen(copyright); k++ )
      spaces[k] = ' ';
    spaces[k] = '\0';
    start = 0;
    // num stays where it is
    for( k=0; k<num; k++ )
      truncName[k] = univfilename[start+k];
    truncName[k] = '\0';
  }
  else
  {// need to lop front off of univ name to fit
    spaces[0] = '\0';
    num = titleLength - strlen(copyright) - 3;
    start = strlen(univfilename) - 4 - num;
    truncName[0]='.'; truncName[1]='.'; truncName[2]='.';
    for( k=3; k<num; k++ )
      truncName[k] = univfilename[start+k];
    truncName[k] = '\0';
}


	sprintf( titlestring, 
           "%s%s%s",
           truncName, spaces, copyright );	                             

  glutCreateWindow(titlestring);

  // purely for updating display when external events demand it
  // (most displays are done when doit runs and want to update display)
  glutDisplayFunc( display );

  glutIdleFunc( doit );

  glutKeyboardFunc(processRegularKey);
  glutSpecialFunc(processSpecialKey);
  glutMotionFunc(dragMouse);

  // jb25:  wow, putting in ability to detect mouse location
  glutPassiveMotionFunc( updateMouse );

  glutMouseFunc( processMouseButton );
//  glutMotionFunc( processMouseDrag );

  init();

  glutMainLoop();

  return 0;
}
