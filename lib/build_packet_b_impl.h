/* -*- c++ -*- */
/* 
 * Copyright 2015 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_HUBSAN_BUILD_PACKET_B_IMPL_H
#define INCLUDED_HUBSAN_BUILD_PACKET_B_IMPL_H

#include <hubsan/build_packet_b.h>

namespace gr {
  namespace hubsan {

    class build_packet_b_impl : public build_packet_b
    {
     private:
      unsigned int sps;
      uint32_t a7105_id;
      uint32_t tx_id;
      pmt::pmt_t controls;
      char packet[24];
      int sample_count;

     public:
      build_packet_b_impl(unsigned int sps, uint32_t a7105_id, uint32_t tx_id);
      ~build_packet_b_impl();

      void build_packet(pmt::pmt_t msg);
      uint8_t generate_crc();
      void handle_control_msg(pmt::pmt_t msg);
    };

  } // namespace hubsan
} // namespace gr

#endif /* INCLUDED_HUBSAN_BUILD_PACKET_B_IMPL_H */

