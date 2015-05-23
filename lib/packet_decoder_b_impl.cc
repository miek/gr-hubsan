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
    {}

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

        // Do <+signal processing+>

        // Tell runtime system how many output items we produced.
        return noutput_items;
    }

  } /* namespace hubsan */
} /* namespace gr */

