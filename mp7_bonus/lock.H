/*
this file is degigned for creating locks
author:	Liuyi Jin
*/

#ifndef _LOCK
#define _LOCK

class Lock{

private:

public:
	
	bool locked ;

	Lock(){
		locked = false;
	}
	
	 void lock(){
		while(locked);
		locked = true;
	}

	 void unlock(){
		locked = false;
	}

};


#endif
