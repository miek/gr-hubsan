/* -*- c++ -*- */
/* 
 * Copyright 2015 Mike Walters.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "build_packet_b_impl.h"

#include <sys/time.h>

namespace gr {
  namespace hubsan {

    build_packet_b::sptr
    build_packet_b::make(unsigned int sps, uint32_t a7105_id, uint32_t tx_id)
    {
      return gnuradio::get_initial_sptr
        (new build_packet_b_impl(sps, a7105_id, tx_id));
    }

    /*
     * The private constructor
     */
    build_packet_b_impl::build_packet_b_impl(unsigned int sps, uint32_t a7105_id, uint32_t tx_id)
      : gr::sync_block("build_packet_b",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(1, 1, sizeof(uint8_t)))
    {
      sample_count = 0;
      this->sps = sps;
      this->a7105_id = a7105_id;
      this->tx_id = tx_id;

      controls = pmt::make_dict();
      controls = pmt::dict_add(controls, pmt::intern("throttle"), pmt::from_long(0));
      controls = pmt::dict_add(controls, pmt::intern("yaw"), pmt::from_long(127));
      controls = pmt::dict_add(controls, pmt::intern("pitch"), pmt::from_long(127));
      controls = pmt::dict_add(controls, pmt::intern("roll"), pmt::from_long(127));

      message_port_register_in(pmt::intern("in"));
      set_msg_handler(pmt::intern("in"), boost::bind(&build_packet_b_impl::handle_control_msg, this, _1));
    }

    /*
     * Our virtual destructor.
     */
    build_packet_b_impl::~build_packet_b_impl()
    {
    }

    int
    build_packet_b_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
        uint8_t *out = (uint8_t *) output_items[0];

        for (int i = 0; i < noutput_items; i++) {
          if (sample_count == 0) {
            build_packet();
          }
          
          if (sample_count < 192*sps) {
            int samp = sample_count / sps;
            out[i] = (packet[samp / 8] & (1 << (7 - (samp % 8)))) != 0;
          } else {
            out[i] = 0;
          }

          sample_count++;

          if (sample_count >= 1000*sps) {
            sample_count = 0;
          }
        }
        return noutput_items;
    }

    void
    build_packet_b_impl::build_packet()
    {
      #define _bswap16(x) ((x << 8) | (x >> 8))

      pmt::pmt_t ref;

      *(uint32_t*)&packet[0] = 0xAAAAAAAA;
      *(uint32_t*)&packet[4] = __builtin_bswap32(a7105_id);
      packet[8] = 0x20;

      ref = pmt::dict_ref(controls, pmt::intern("throttle"), pmt::PMT_NIL);
      *(uint16_t*)&packet[9] = _bswap16(pmt::to_long(ref));

      ref = pmt::dict_ref(controls, pmt::intern("yaw"), pmt::PMT_NIL);
      *(uint16_t*)&packet[11] = _bswap16(pmt::to_long(ref));

      ref = pmt::dict_ref(controls, pmt::intern("pitch"), pmt::PMT_NIL);
      *(uint16_t*)&packet[13] = _bswap16(pmt::to_long(ref));

      ref = pmt::dict_ref(controls, pmt::intern("roll"), pmt::PMT_NIL);
      *(uint16_t*)&packet[15] = _bswap16(pmt::to_long(ref));

      packet[17] = 0x02;
      packet[18] = 0x61;

      *(uint32_t*)&packet[19] = __builtin_bswap32(tx_id);
      packet[23] = generate_crc();

      #undef _bswap16
    }

    uint8_t
    build_packet_b_impl::generate_crc()
    {
      uint8_t crc = 0;
      for (int i = 8; i < 23; i++) {
        crc += packet[i];
      }
      return 256 - crc;
    }

    void
    build_packet_b_impl::handle_control_msg(pmt::pmt_t msg)
    {
      controls = msg;
    }

  } /* namespace hubsan */
} /* namespace gr */

