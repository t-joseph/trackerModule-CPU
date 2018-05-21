#include "bpi.h"

using namespace sc_core;
using namespace sc_dt;
using namespace std;

using namespace std;

bpi::bpi(sc_module_name nm)
           : socket("socket")  // Construct and name socket
{
  SC_THREAD(bpiCpuRd);
  SC_THREAD(thread_process_gps);
  SC_THREAD(thread_process_gsm);
  SC_THREAD(bpiCpuWr);
}

void bpi::bpiCpuRd()
{
  int i;
  int available;
  while(1)
  {
    wait(2500, SC_NS);
    available = bpi_i->num_available();
    memset(data, 0, frameSize);
    i = 0;
    while(bpi_i->num_available() != 0)
    {
      bpiRd_f = bpi_i->read();
      data[i] = bpiRd_f;
      i++;
      //data.push_back(bpiRd_f);
    }
    data[i] = '\0';

    if(available == 64)
    {
      cout<<"@" << sc_time_stamp() <<" :: <BPI> Reading GPS frame from FuncBlock"
      <<", num_available"<< available
      <<endl;
      thread_process_e2.notify();
    }
    else if(available == 46)
    {
      cout<<"@" << sc_time_stamp() <<" :: <BPI> Reading GSM frame from FuncBlock"
      <<", num_available"<< available
      <<endl;
      thread_process_e3.notify();
    }
    wait(bpiCpuRd_e1);
  }
}


void bpi::thread_process_gps()
{
  while(1)
  {
    wait(thread_process_e2);
    // TLM-2 generic payload transaction, reused across calls to b_transport
    tlm::tlm_generic_payload* trans = new tlm::tlm_generic_payload;
    sc_time delay = sc_time(10, SC_NS);

    // loop for writing all gps frame characters
    //for (i = 0; i < 64; i++)
    {
      tlm::tlm_command cmd = tlm::TLM_WRITE_COMMAND;

      // Initialize 8 out of the 10 attributes, byte_enable_length and extensions being unused
      trans->set_command( cmd );
      trans->set_address( addrGpsFrame );
      trans->set_data_ptr( reinterpret_cast<unsigned char*>(data) );
      trans->set_data_length( frameSize );
      trans->set_streaming_width( frameSize ); // = data_length to indicate no streaming
      trans->set_byte_enable_ptr( 0 ); // 0 indicates unused
      trans->set_dmi_allowed( false ); // Mandatory initial value
      trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE ); // Mandatory initial value


      socket->b_transport( *trans, delay );  // Blocking transport call

      // Initiator obliged to check response status and delay
      if ( trans->is_response_error() )
        SC_REPORT_ERROR("TLM-2", "Response error from b_transport");

      cout << "<BPI> trans = { " << 'W' << ", " << addrGpsFrame
           << " } , data = " << data << " at time " << sc_time_stamp()
           << " delay = " << delay << endl;
      // Realize the delay annotated onto the transport call
      wait(delay);
    }
    bpiCpuRd_e1.notify();
  }
}


void bpi::thread_process_gsm()
{
  while(1)
  {
    wait(thread_process_e3);
    // TLM-2 generic payload transaction, reused across calls to b_transport
    tlm::tlm_generic_payload* trans = new tlm::tlm_generic_payload;
    sc_time delay = sc_time(10, SC_NS);


    {
      tlm::tlm_command cmd = tlm::TLM_WRITE_COMMAND;

      // Initialize 8 out of the 10 attributes, byte_enable_length and extensions being unused
      trans->set_command( cmd );
      trans->set_address( addrGsmFrame );
      trans->set_data_ptr( reinterpret_cast<unsigned char*>(data) );
      trans->set_data_length( frameSize );
      trans->set_streaming_width( frameSize ); // = data_length to indicate no streaming
      trans->set_byte_enable_ptr( 0 ); // 0 indicates unused
      trans->set_dmi_allowed( false ); // Mandatory initial value
      trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE ); // Mandatory initial value

      socket->b_transport( *trans, delay );  // Blocking transport call

      // Initiator obliged to check response status and delay
      if ( trans->is_response_error() )
        SC_REPORT_ERROR("TLM-2", "Response error from b_transport");

      cout << "<BPI> trans = { " << 'W' << ", " << addrGsmFrame
           << " } , data = " << data << " at time " << sc_time_stamp()
           << " delay = " << delay << endl;
      // Realize the delay annotated onto the transport call
      wait(delay);
    }
    bpiCpuWr_e4.notify();
  }
}


void bpi::bpiCpuWr()
{
  int i;
  while(1)
  {
    wait(bpiCpuWr_e4);
    // TLM-2 generic payload transaction, reused across calls to b_transport
    tlm::tlm_generic_payload* trans = new tlm::tlm_generic_payload;
    sc_time delay = sc_time(10, SC_NS);

    {
      tlm::tlm_command cmd = tlm::TLM_READ_COMMAND;

      // Initialize 8 out of the 10 attributes, byte_enable_length and extensions being unused
      trans->set_command( cmd );
      trans->set_address( addrGpsFrame );
      trans->set_data_ptr( reinterpret_cast<unsigned char*>(data) );
      trans->set_data_length( frameSize );
      trans->set_streaming_width( frameSize ); // = data_length to indicate no streaming
      trans->set_byte_enable_ptr( 0 ); // 0 indicates unused
      trans->set_dmi_allowed( false ); // Mandatory initial value
      trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE ); // Mandatory initial value

      socket->b_transport( *trans, delay );  // Blocking transport call

      // Initiator obliged to check response status and delay
      if ( trans->is_response_error() )
        SC_REPORT_ERROR("TLM-2", "Response error from b_transport");

      cout << "<BPI> trans = { " << 'R' << ", " << addrGpsFrame
           << " } , data = " << data << " at time " << sc_time_stamp()
           << " delay = " << delay << endl;
      // Realize the delay annotated onto the transport call
      wait(delay);
    }

    cout<<"@" << sc_time_stamp() <<" ::<BPI> Writing GPS frame to FuncBlock"
    <<endl;
	  for(i = 0; data[i]; i++)
    {
      bpi_o->write(data[i]);
    }
    memset(data, 0, frameSize);

    {
      tlm::tlm_command cmd = tlm::TLM_READ_COMMAND;

      // Initialize 8 out of the 10 attributes, byte_enable_length and extensions being unused
      trans->set_command( cmd );
      trans->set_address( addrGsmFrame );
      trans->set_data_ptr( reinterpret_cast<unsigned char*>(data) );
      trans->set_data_length( frameSize );
      trans->set_streaming_width( frameSize ); // = data_length to indicate no streaming
      trans->set_byte_enable_ptr( 0 ); // 0 indicates unused
      trans->set_dmi_allowed( false ); // Mandatory initial value
      trans->set_response_status( tlm::TLM_INCOMPLETE_RESPONSE ); // Mandatory initial value

      socket->b_transport( *trans, delay );  // Blocking transport call

      // Initiator obliged to check response status and delay
      if ( trans->is_response_error() )
        SC_REPORT_ERROR("TLM-2", "Response error from b_transport");

        cout << "<BPI> trans = { " << 'R' << ", " << addrGsmFrame
        << " } , data = " << data << " at time " << sc_time_stamp()
        << " delay = " << delay << endl;

        // Realize the delay annotated onto the transport call
        wait(delay);
    }

    cout<<"@" << sc_time_stamp() <<" ::<BPI> Writing GSM frame to FuncBlock"
    <<endl;
    for(i = 0; data[i]; i++)
    {
      bpi_o->write(data[i]);
    }
    bpiCpuRd_e1.notify();
  }
}
