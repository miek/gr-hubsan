/* -*- c++ -*- */
/* 
 * Copyright 2015 Mike Walters
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

#ifndef INCLUDED_JOYSTICK_JOYSTICK_SOURCE_F_IMPL_H
#define INCLUDED_JOYSTICK_JOYSTICK_SOURCE_F_IMPL_H

#include <joystick/joystick_source_f.h>
#include <libevdev-1.0/libevdev/libevdev.h>
#include <boost/thread.hpp>
#include <pmt/pmt.h>
#include <sys/time.h>

namespace gr {
  namespace joystick {

    class joystick_source_f_impl : public joystick_source_f
    {
     private:
      pmt::pmt_t out_port;
      int fd;
      struct libevdev *dev = NULL;
      boost::thread controls_thread;
      volatile float controls[4] = {50};
      uint16_t controls_map[4] = {ABS_THROTTLE, ABS_RZ, ABS_Y, ABS_X};
      volatile int stop = 0;
      uint32_t work_count = 0;
      struct timeval last_event, last_update;

      float calib_offset[4] = {17, 0, 35, 0};
      float calib_mult[4] = {-1.8, 0.65, 1.3, -1.3};


     public:
      joystick_source_f_impl();
      ~joystick_source_f_impl();

      // Where all the action really happens
      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);

      void update_controls();
    };

  } // namespace joystick
} // namespace gr

#endif /* INCLUDED_JOYSTICK_JOYSTICK_SOURCE_F_IMPL_H */

