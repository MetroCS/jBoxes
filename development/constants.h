extern int authorVersion;

// allow TeX snapshots?
extern int TeXauthorized;

extern int trace;  // 0 means skip, 1 means do it

extern int inputbuffersize;

// jb26:  didn't have before!
extern int screenwidth;
extern int screenheight;
extern float notetextz;
extern float savetextz;

// ****************************************************************
// ************************* colors *******************************
// ****************************************************************

// define opengl color arrays and corresponding colorcode ints

// color codes are stored in both the color and dye members of box,
// so need a code meaning "no color" in dye so natural color can
// come out
extern int nocolorcode;

// neutral is the color of stuff that is not viewed as needing any
// special color, like the universe, the stack, the heap, the console
//  --- it's the classes, new'd boxes, stack frames, that have
//      special colors indicating their semantics --- the neutral
// guys are just containers
extern float neutralcolor[];
extern int neutralcolorcode;

// blueprint is the color of the static class stuff
extern float blueprintcolor[];
extern int blueprintcolorcode;

// copy is the color of the method copies that are put in the 
// stack box
extern float stackcolor[];
extern int stackcolorcode;

// guys created on the heap have their own special color---
//  object color!
extern float heapcolor[];
extern int heapcolorcode;

// v10:  add in static data box
extern float staticdatacolor[];
extern int staticdatacolorcode;

// the focus (actually the interactive focus) has a vivid color
// so's we can spot it quickly
extern float focuscolor[];
extern int focuscolorcode;

// for inspection list --- color of an action box whose initial
// signaling is interesting (probably not an assignment box,
// where the right hand side value appearing will be plenty)
extern float signalcolor[];
extern int signalcolorcode;

// for inspection list --- color of a newly changed value
extern float newvaluecolor[];
extern int newvaluecolorcode;

// for inspection list --- color of a just created instance on heap
extern float newobjectcolor[];
extern int newobjectcolorcode;

// special tracing color --- leave call boxes that are waiting for
// value to return this color
extern float waitcolor[];
extern int waitcolorcode;

// when execution stops with an error box, the box identified as
// holding the error is set to special errorcolor, just for a while
extern float errorcolor[];
extern int errorcolorcode;

// jb5a
extern float stoplessoncolor[];
extern int stoplessoncolorcode;


// the console has a special background color
extern float consolecolor[];
extern int consolecolorcode;

// and the cursor background is special (the drawing color in
// the console is permanently set at black)
extern float consolecursorcolor[];
extern int consolecursorcolorcode;

// the textcolors are automatic, permanent, depend only on
// the aspect showing --- codes not needed, because these
// are not stored anywhere, except indirectly through aspect of box
extern float abstextcolor[];
extern float doctextcolor[];
extern float fulltextcolor[];
extern float valuetextcolor[];

// jb16:
extern int messagecolorcode;
extern int successcolorcode;
extern float successcolor[];

// ****************************************************************
// *********************** symbol stuff ***************************
// ****************************************************************

extern int trianglecode;  // code numbers for special symbols
                         // didn't really follow this scheme, note!
extern int littlepagecode;
extern int absimagecode;
extern int tabsymbolcode;  // jb16
extern int boxedjavacode;  // jb16

extern int minProgCode;

extern int endoffilecode;  // special symbol that doesn't conflict
                          // with 0..255 or -128..127,
													// isn't really a symbol, but can display

extern int nextkey;
extern int prevkey;
extern int backspacekey;
extern int deletekey;

extern int killkey;   

extern int wipekey;

extern int enterkey; 

extern int tabkey;   // jb16

extern int jumpkey;
extern int toggleviewskey;  

extern int upkey;
extern int downkey;

extern int firstkey;
extern int lastkey;

extern int inkey;
extern int outkey;

extern int specialspacekey; // alternative space key
extern int escapeKey;  // jb32

extern int xeroxkey;
extern int gluekey;

// old --- now using function keys for save --- int savekey = 19;
extern int quitkey;

// jb18
extern int breaktogglekey;

// new version of symbol dimensions
extern float typsymwidth; // makes for 80 across
extern float typsymheight;
extern float typsymkern;

// is the permanent, externant distance between baselines of
//  successive lines in a doc aspect
extern float baseline;  

extern int maxsymcode;  // 0..255 plus 256

extern float minpadsym;

extern float symdepth;

extern float hpad, vpad;  // padding between boundaries
                                     // and parts
          // was 2, 2
extern float vbpad;  // viewer border pad 

extern float arrowpad;  // standard arrow length

extern float arrowheadwidth;
extern float arrowheadheight;

extern float flowarrowpad;  // smaller arrow length

extern char* progChars[];

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

extern float minz;
extern float textz;
extern float breakz;  // jb18
extern float errorz;
extern float textfocusz;
// box backgrounds display from univz to univz+1,
// (asymptotically hitting -5), depending on their depth
extern float univz;
extern float maxz;

// jb26:
extern float notez;

// ***************************************************************
// *************************** codes *****************************
// ***************************************************************

extern int methodnamenotvalueerror;
extern int typesdontmatcherror;
extern int firstemptysecondnoterror;
extern int classnotfounderror;
extern int typenotfounderror;
extern int triedtonewprimerror;
extern int illegalassoperror;
extern int cantplusequalobjecterror;
extern int incomptypeserror;
extern int divbyzeroerror;
extern int nofloatmoderror;
extern int datanotiniterror;
extern int cantjumpemptyerror;
extern int idnotfounderror;
extern int valuetypeincompaterror;
extern int duplicateclassnameserror;
extern int duplicatemembernameserror;
extern int canthaveemptynameerror;
extern int missingtypeerror;
extern int parammustbeemptyerror;
extern int badprimvalueerror;
extern int objectrefnotemptyerror;
extern int duplicatelocalnameserror;
extern int illegalgrowoperror;
extern int notintegererror;
extern int nosuchclassmembererror;
extern int nosuchinstancedataerror;
extern int namecantbereservederror;
extern int notvalidheapaddresserror;
extern int emptyidnameerror;
extern int illegalmethodreferror;
extern int argsnotequalparamserror;
extern int incompatargparamerror;
extern int nosuchinstancemembererror;
extern int canthavemethodreferror;
extern int mustreturnvalueerror;
extern int argnotlegalinterror;
extern int negsizerequestederror;
extern int nosuchtypeerror;
extern int notagridboxerror;
extern int nonemptysecondfor1derror;
extern int colindextoolargeerror;
extern int rowindextoolargeerror;
extern int illegaloperatorerror;
extern int illegaloperanderror;
extern int cantcoercetofloaterror;
extern int invalidbooleanerror;
extern int badbifnameerror;
extern int need2argserror;
extern int illegalcursorlocationerror;
extern int need1argerror;
extern int nosuchcharerror;
extern int indextoolargeerror;
extern int neednoargserror;
extern int cantbearrayerror;
extern int negativeargerror;
extern int notastringerror;
extern int notfloatformerror;
extern int portrangeerror;
extern int openerrornot3args;
extern int openerrorportbusy;
extern int openerroriochoice;
extern int portnotreadingerror;
extern int endoffileerror;
extern int portnotwritingerror;
extern int notcharerror;
extern int portnotopenerror;
extern int badliteralformaterror;
extern int notintegerformerror;
extern int badstringindexerror;
extern int badfirstsymbolerror;
extern int badtargetlocationerror;
extern int argoutofrangeerror;
extern int notfloaterror;
extern int notachararrayerror;
extern int progcharrangeerror;
extern int badprogcharstringerror;
extern int returningwrongtypeerror;
extern int intrangeerror;
extern int notininstancemethoderror;
extern int nosuchinstanceerror;
extern int erroralreadyrequested;
extern int notarraytypeerror;
extern int illegaltypeerror;

// operator codes (includes built in "methods")
extern int noopcode;  // nonexistent or illegal operation
extern int eqcode;  // relational ops
extern int ltcode;
extern int gtcode;
extern int lecode;
extern int gecode;
extern int necode;
extern int pluscode;  // arithmetic ops
extern int minuscode;  
extern int timescode;
extern int divcode;
extern int modcode;
extern int andcode;  // logical ops
extern int orcode;
extern int notcode;  // unary ops
extern int oppcode;

extern int moveTocode;
extern int printcode;
extern int charcode;
extern int asciicode;
extern int getcode;
extern int getLinecode;
extern int clearcode;
extern int randomcode;
extern int timecode;  // jb2006a:  adding _time
extern int abscode;
extern int sqrtcode;
extern int sincode;
extern int coscode;
extern int atancode;
extern int floorcode;
extern int ceilcode;
extern int intToStringcode;
extern int floatToStringcode;
extern int charToStringcode;
extern int charsToStringcode;
extern int isIntcode;
extern int isFloatcode;
extern int toIntcode;
extern int toFloatcode;
extern int lengthcode;
extern int charAtcode;
// extern int copycode;
extern int opencode;
extern int closecode;
extern int fgetcode;
extern int fputcode;
extern int eofcode;
extern int tancode;
extern int asincode;
extern int acoscode;
extern int logcode;
extern int expcode;
extern int seedcode;
extern int progCharcode;
extern int whatRowcode;
extern int whatColcode;
extern int hideCursorcode;
extern int showCursorcode;
extern int haltcode;
extern int nullcode;  // jb17
extern int thiscode;
extern int destroycode;

// states of execution
extern int initialstate;
extern int waittastate;  
extern int waittpvstate;
extern int waittplstate;
extern int waittocallstate;
extern int waittoreturnstate;
extern int returnstate;
extern int waittogetinputstate;
extern int javawaitstate;       // jb16

  // the modes of operation:
  extern int interactivemode;
  extern int pausedmode;
  extern int executingmode;
  extern int errormode;
  extern int input1mode;
  extern int inputlinemode;
  extern int haltmode;
	extern int consoleviewmode;        // v11
	extern int pausedconsoleviewmode;
	extern int runtobreakmode;         // jb18

extern int interactiveviewer;
extern int executionviewer;
extern int consoleviewer;
extern int portsviewer;

// kinds of boxes
extern int databox;
extern int univbox;
extern int classbox;
extern int classdatabox;
extern int classmethodsbox;
extern int instancedatabox;
extern int instancemethodsbox;
extern int methodbox;
extern int paramsbox;
extern int localsbox;
extern int seqbox;
extern int emptybox;
extern int branchbox;
extern int dobox;
extern int whilebox;
extern int forbox;
extern int idbox;
// deprecated extern int literalbox = 18; // deprecated --- 
extern int callbox;
extern int memberbox;
extern int arraybox;
extern int returnbox;
extern int opbox;
extern int unopbox;
extern int assbox;
extern int growbox;
extern int array2dbox;
extern int newopbox;
extern int classmethodbox;
extern int instancemethodbox;
extern int stackbox;
extern int heapbox;
extern int instancebox;
extern int consolebox;
extern int valuebox;   // cell in a gridbox on heap
extern int gridbox;
extern int portsbox;
extern int portbox;
extern int intbox;    // these 5 replace literalbox
extern int floatbox;
extern int charbox;
extern int booleanbox;
extern int stringbox;

extern int retvalbox;   //v10
extern int allclassdatabox;
extern int classdatacopybox;

extern int javabox;   // jb16

// jb21
extern int lessonbox;  // jb19
extern int parbox;
extern int combox;
extern int presbox;
extern int notebox;
