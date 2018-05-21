#ifndef __bpi_h
#define __bpi_h

#include "systemc"
using namespace sc_core;
using namespace sc_dt;
using namespace std;

#include "tlm.h"
#include "tlm_utils/simple_initiator_socket.h"


// Initiator module generating generic payload transactions

SC_MODULE (bpi)
{

  public:
  //input ports
  sc_port<sc_fifo_in_if<char> >     bpi_i ;      //
  sc_port<sc_fifo_out_if<char> >    bpi_o ;      //

  // TLM-2 socket, defaults to 32-bits wide, base protocol
  tlm_utils::simple_initiator_socket<bpi> socket;

  SC_HAS_PROCESS(bpi);
  bpi (sc_module_name nm);

  // Internal data buffer used by initiator with generic payload
  enum { addrGpsFrame = 10, addrGsmFrame = 11, frameSize = 100};

  char data[frameSize];


  private:

//signals
  sc_fifo<char> bpiRd_f;


  sc_event bpiCpuRd_e1;
  sc_event thread_process_e2;
  sc_event thread_process_e3;
  sc_event bpiCpuWr_e4;


  void thread_process_gps();
  void thread_process_gsm();
  void bpiCpuRd();
  void bpiCpuWr();

};

#endif
