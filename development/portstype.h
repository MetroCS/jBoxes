// interface for portstype class

const int numports = 4;
const int buffersize = 24;
const int unusedPortStatus = 0;
const int readingPortStatus = 1;
const int writingPortStatus = 2;
const int emptyPortStatus = 3;

class portstype
{
  private:

    FILE* files[numports]; // hold the file handles

		int statuses[numports];  // 0=unhooked, 1=input, 2=output

	public:

    portstype();
	  int getStatus( int whichPort );
    int attemptOpen( int whichPort, mystring* iochoice,
		                        mystring* filestring );
    void closeAll();
		FILE* getFile( int whichPort );
    void noteEmpty( int whichPort );
    void closePort( int whichPort );
};
