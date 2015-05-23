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


#ifndef INCLUDED_HUBSAN_BUILD_PACKET_B_H
#define INCLUDED_HUBSAN_BUILD_PACKET_B_H

#include <hubsan/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace hubsan {

    /*!
     * \brief <+description of block+>
     * \ingroup hubsan
     *
     */
    class HUBSAN_API build_packet_b : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<build_packet_b> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of hubsan::build_packet_b.
       *
       * To avoid accidental use of raw pointers, hubsan::build_packet_b's
       * constructor is in a private implementation
       * class. hubsan::build_packet_b::make is the public interface for
       * creating new instances.
       */
      static sptr make(unsigned int sps, uint32_t a7105_id, uint32_t tx_id);
    };

  } // namespace hubsan
} // namespace gr

#endif /* INCLUDED_HUBSAN_BUILD_PACKET_B_H */

