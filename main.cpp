#include "top_controller.cpp"

using namespace sc_core;
using namespace sc_dt;
using namespace std; 

int sc_main (int argc, char* argv[]) 
{
	
	top_controller top("top");
	
	sc_start(100, SC_SEC);
	
	return 0;
}

