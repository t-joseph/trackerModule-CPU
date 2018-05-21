
//-----------------------------------------------------
// Controller test bench module
// Design Name : tb_controller
// File Name : tb_controller.h
// Function : This is the header file of test bench for controller mod
//-----------------------------------------------------

#ifndef __tb_controller_h
#define __tb_controller_h

#include "systemc.h"
#include <string>
#include "tlm.h"
#include "tlm_utils/simple_target_socket.h"

#define SC_INCLUDE_DYNAMIC_PROCESSES

using namespace sc_core;
using namespace sc_dt;
using namespace std;

SC_MODULE (tb_controller) {

   tlm_utils::simple_target_socket<tb_controller> socket;

  public:
  //output ports
  sc_port<sc_fifo_out_if<char> >     tb_gps_o ;      //
  sc_port<sc_fifo_out_if<char> >     tb_gsm_o ;      //

  //input ports
  sc_port<sc_fifo_in_if<char> >    tb_gsm_i ;     //

// Local Variables
  string dataFrame;
  string gsmReq;

  enum { SIZE = 100};

  char mem[SIZE][SIZE];
// Constructor prototype
//SC_CTOR(tb_controller);

  SC_HAS_PROCESS(tb_controller);
  tb_controller (sc_module_name nm);

  virtual void b_transport( tlm::tlm_generic_payload& trans, sc_time& delay )
  {

    tlm::tlm_command cmd = trans.get_command();
    sc_dt::uint64    adr = trans.get_address();
    unsigned char*   ptr = trans.get_data_ptr();
    unsigned int     len = trans.get_data_length();
    unsigned char*   byt = trans.get_byte_enable_ptr();
    unsigned int     wid = trans.get_streaming_width();


    if (adr >= sc_dt::uint64(SIZE) || byt != 0 || len > SIZE || wid < len)
     SC_REPORT_ERROR("TLM-2", "Target does not support given generic payload transaction");

      // Obliged to implement read and write commands
    if ( cmd == tlm::TLM_READ_COMMAND )
    {
        memcpy(ptr, mem[adr], len);
    }
    else if ( cmd == tlm::TLM_WRITE_COMMAND )
    {
        memcpy(mem[adr], ptr, len);
    }

      // Obliged to set response status to indicate successful completion
    trans.set_response_status( tlm::TLM_OK_RESPONSE );
  }

  private:
//signals
  sc_fifo<char> tb_gsmRd_f;

  sc_event tb_read_e1;
  sc_event thread_process_e2;
  sc_event thread_process_e3;
  sc_event thread_process_e4;


  void test();
};

#endif
