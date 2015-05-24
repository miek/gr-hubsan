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
#include "packet_decoder_b_impl.h"
#include "util.h"

namespace gr {
  namespace hubsan {

    packet_decoder_b::sptr
    packet_decoder_b::make()
    {
      return gnuradio::get_initial_sptr
        (new packet_decoder_b_impl());
    }

    /*
     * The private constructor
     */
    packet_decoder_b_impl::packet_decoder_b_impl()
      : gr::sync_block("packet_decoder_b",
              gr::io_signature::make(1, 1, sizeof(uint8_t)),
              gr::io_signature::make(0, 0, 0))
    {
        out_port = pmt::mp("out");
        message_port_register_out(out_port);
    }

    /*
     * Our virtual destructor.
     */
    packet_decoder_b_impl::~packet_decoder_b_impl()
    {
    }

    int
    packet_decoder_b_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
        const uint8_t *in = (const uint8_t *) input_items[0];

        for (int i = 0; i < noutput_items; i++) {
          shift_data_in(in[i]);
          check_valid_packet();
        }

        return noutput_items;
    }

    void packet_decoder_b_impl::shift_data_in(bool bit) {
        int temp, carry = bit;
        for (int i = 23; i >= 0; i--) {
            temp = !!(data[i] & 0x80);
            data[i] = (data[i] << 1) | carry;
            carry = temp;
        }
    }

    void packet_decoder_b_impl::check_valid_packet() {
        uint32_t preamble = *(uint32_t*)&data[0];
        if ((preamble == 0xAAAAAAAA || preamble == 0x55555555) && generate_checksum(&data[8], 15) == data[23]) {
            pmt::pmt_t packet = pmt::make_dict();

            uint32_t a7105_id   = __builtin_bswap32(*(uint32_t*)&data[4]);
            uint32_t tx_id      = __builtin_bswap32(*(uint32_t*)&data[19]);

            uint16_t throttle   = _bswap16(*(uint16_t*)&data[9]);
            uint16_t yaw        = _bswap16(*(uint16_t*)&data[11]);
            uint16_t pitch      = _bswap16(*(uint16_t*)&data[13]);
            uint16_t roll       = _bswap16(*(uint16_t*)&data[15]);

            packet = pmt::dict_add(packet, pmt::intern("a7105_id"), pmt::from_long(a7105_id));
            packet = pmt::dict_add(packet, pmt::intern("tx_id"), pmt::from_long(tx_id));

            packet = pmt::dict_add(packet, pmt::intern("throttle"), pmt::from_long(throttle));
            packet = pmt::dict_add(packet, pmt::intern("yaw"), pmt::from_long(yaw));
            packet = pmt::dict_add(packet, pmt::intern("pitch"), pmt::from_long(pitch));
            packet = pmt::dict_add(packet, pmt::intern("roll"), pmt::from_long(roll));

            message_port_pub(out_port, packet);
        }
    }
  } /* namespace hubsan */
} /* namespace gr */

