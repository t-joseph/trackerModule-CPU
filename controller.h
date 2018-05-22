
//-----------------------------------------------------
// Controller logic for the tracker module
// Design Name : controller
// File Name : controller.h
// Function : This is a control module, which interacts with GPS
// GSM, Memory and Debug module
//-----------------------------------------------------

#ifndef __controller_h
#define __controller_h

#include "systemc.h"
#include <string>

using namespace std;

SC_MODULE (controller) {

  public:
  //input ports
  sc_port<sc_fifo_in_if<char> >     gps_i ;
  sc_port<sc_fifo_in_if<char> >     gsm_i ;

  //output ports
  sc_port<sc_fifo_out_if<char> >    gsm_o ;

  //Bus Periperal Interface Ports(part of CPU)
  sc_port<sc_fifo_in_if<char> >     cpu_i ;
  sc_port<sc_fifo_out_if<char> >    cpu_o ;

  //debug ports
  sc_port<sc_fifo_in_if<char> >   dbg_i ;
  sc_in<bool> 			              dbgEn_i;
  sc_port<sc_fifo_out_if<char> >  dbg_o ;
  sc_in<bool> 			              dbgCapture_i;
  sc_in<bool> 			              dbgUpdate_i;
  sc_in<bool> 			              dbgScan_i;

  // Local Variables
  string gpsFrame;
  string gsmFrame;
  string dataFrame;

  // Constructor prototype
  SC_HAS_PROCESS(controller);
  controller (sc_module_name nm);

  private:

  //signals
  sc_fifo<char> gpsRd_f;
  sc_fifo<char> gsmRd_f;
  sc_fifo<char> cpuRd_f;


  // events
  sc_event sleep_e1;
  sc_event user_request_e1;
  sc_event gpsframe_read_e2;
  sc_event gsmframe_request_e3;
  sc_event gsmframe_read_e4;
  sc_event dataframe_gen_e5;
  sc_event dataframe2gsm_write_e6;

  //sleep thread
  void sleep();

  //Read GPS frame from the FIFO
  void gpsToCpuRdThread(void);

  //Request Power data to GSM
  void cpuToGsmWrThread(void);

  //Read GSM frame from the FIFO
  void gsmToCpuRdThread(void);

  // computation thread
  void cpuCompThread(void);

  // Write Data frame to Output FIFO(GSM)
  void cpuToGsmDataFrameWrThread(void);
};

#endif
