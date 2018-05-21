
//-----------------------------------------------------
// Controller logic for the tracker module
// Design Name : controller
// File Name : controller.cpp
// Function : This is a control module, which interacts with GPS
// GSM,Memory and Debug module
//-----------------------------------------------------

#include "controller.h"

using namespace sc_core;
using namespace sc_dt;
using namespace std;

// Constructor for the controller

controller::controller(sc_module_name nm)
           : sc_module(nm)
  {
	  SC_THREAD(sleep);
    SC_THREAD(gpsToCpuRdThread);
    SC_THREAD(cpuToGsmWrThread);
    SC_THREAD(gsmToCpuRdThread);
    SC_THREAD(cpuCompThread);
    SC_THREAD(cpuToGsmDataFrameWrThread);

    //binds the bus-cpu fifo to the export


  } // End of Constructor


//sleep mode
void controller:: sleep()
{
  while(1)
  {
    cout<<"@" << sc_time_stamp() <<" :: <FuncBlock> Entering Sleep Mode "
    <<endl;
    wait(2000, SC_NS);
    cout<<"@" << sc_time_stamp() <<" :: <FuncBlock> Entering Active Mode "
    <<endl;
    gpsframe_read_e2.notify();
    wait(sleep_e1);
  }
}


//Reading GPS frame from the FIFO and writing to to BPI
void controller::gpsToCpuRdThread ()
{
  int i;
  while(1)
  {
	  wait(gpsframe_read_e2);
    cout<<"@" << sc_time_stamp() <<" :: <FuncBlock> Reading GPS frame from GPS mod"
    <<", num_available"<< gps_i->num_available()
    <<endl;
    while(gps_i->num_available() != 0)
    {
	    gpsRd_f = gps_i->read();
      gpsFrame.push_back(gpsRd_f);
    }
    cout<<gpsFrame<<endl;

    cout<<"@" << sc_time_stamp() <<" :: <FuncBlock> Writing GPS Frame to BPI"
    <<endl;
    for(i = 0; gpsFrame[i]; i++)
    {
      cpu_o->write(gpsFrame[i]);
    }
    gpsFrame.clear();
    gsmframe_request_e3.notify();
  }
}

//Request Power data to GSM Module
void controller::cpuToGsmWrThread ()
{
  int i;
  char request[] = "power?";
  while(1)
  {
	  wait(gsmframe_request_e3);
    cout<<"@" << sc_time_stamp() <<" :: <FuncBlock> Request for GSM frame to GSM mod"
    <<endl;
    for(i = 0; request[i]; i++)
    {
      gsm_o->write(request[i]);
    }
    gsmframe_read_e4.notify();
  }
}


//Reading GSM frame from the FIFO and Writing to BPI
void controller::gsmToCpuRdThread()
{
  int i;
  while(1)
  {
  	wait(gsmframe_read_e4);
	  wait(100, SC_NS);
    cout<<"@" << sc_time_stamp() <<" :: <FuncBlock> Reading GSM frame from GSM mod"
    <<", num_available"<< gsm_i->num_available()
    <<endl;
    while(gsm_i->num_available() != 0)
    {
	    gsmRd_f = gsm_i->read();
      gsmFrame.push_back(gsmRd_f);
    }
    cout<<gsmFrame<<endl;

    cout<<"@" << sc_time_stamp() <<" :: <FuncBlock> Writing GSM Frame to BPI"
    <<endl;
    for(i = 0; gsmFrame[i]; i++)
    {
      cpu_o->write(gsmFrame[i]);
    }
    gsmFrame.clear();
    wait(2500, SC_NS);
    dataframe_gen_e5.notify();
  }
}


//Reading GPS and GSM Data Frame from BPI and data frame computation
void controller::cpuCompThread()
{
  while(1)
  {
	  wait(dataframe_gen_e5);
    cout<<"@" << sc_time_stamp() <<" :: <FuncBlock> Reading GPS frame from BPI"
    <<", num_available"<< cpu_i->num_available()
    <<endl;
    while(cpu_i->num_available() != 0)
    {
  	  cpuRd_f = cpu_i->read();
      dataFrame.push_back(cpuRd_f);
    }

    wait(100, SC_NS);

    cout<<"@" << sc_time_stamp() <<" :: <FuncBlock> Reading GSM frame from BPI"
    <<", num_available"<< cpu_i->num_available()
    <<endl;
    while(cpu_i->num_available() != 0)
    {
  	  cpuRd_f = cpu_i->read();
      dataFrame.push_back(cpuRd_f);
    }

    cout<<"@" << sc_time_stamp() <<" :: <FuncBlock> Computed data frame -"
    <<dataFrame
    <<endl;

    dataframe2gsm_write_e6.notify();
  }
}


// Write Data frame to Output FIFO(GSM)
void controller::cpuToGsmDataFrameWrThread()
{
  int i;
  while(1)
  {
	  wait(dataframe2gsm_write_e6);
	  cout<<"@" << sc_time_stamp() <<" :: <FuncBlock> Writing data frame "
    <<endl;
	  for(i = 0; dataFrame[i]; i++)
    {
      gsm_o->write(dataFrame[i]);
    }
    dataFrame.clear();
    wait(100, SC_NS);
    sleep_e1.notify();
  }
}
