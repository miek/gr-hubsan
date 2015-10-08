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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "joystick_source_f_impl.h"

namespace gr {
  namespace hubsan {

    joystick_source_f::sptr
    joystick_source_f::make()
    {
      return gnuradio::get_initial_sptr
        (new joystick_source_f_impl());
    }

    /*
     * The private constructor
     */
    joystick_source_f_impl::joystick_source_f_impl()
      : block("joystick_source_f",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0))
    {
      gettimeofday(&last_update, NULL);
      gettimeofday(&last_event, NULL);
      
      out_port = pmt::mp("out");
      message_port_register_out(out_port);

      fd = open("/dev/input/by-id/usb-Logitech_Inc._WingMan_Force_3D-event-joystick", O_RDONLY|O_NONBLOCK);
      int rc = 1;
      rc = libevdev_new_from_fd(this->fd, &this->dev);
      if (rc < 0) {
        fprintf(stderr, "Failed to init libevdev (%s)\n", strerror(-rc));
        exit(1);
      }

      printf("Input device name: \"%s\"\n", libevdev_get_name(dev));
      printf("Input device ID: bus %#x vendor %#x product %#x\n",
              libevdev_get_id_bustype(dev),
              libevdev_get_id_vendor(dev),
              libevdev_get_id_product(dev));

      controls_thread = boost::thread(&joystick_source_f_impl::update_controls, this);
    }

    /*
     * Our virtual destructor.
     */
    joystick_source_f_impl::~joystick_source_f_impl()
    {
      stop = 1;
      controls_thread.join();

      libevdev_free(dev);
      close(fd);
    }

    int
    joystick_source_f_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {

      float *out;
      for (int j = 0; j < 1; j++) {
        for (int i = 0; i < 4; i++) {
          out = (float*)output_items[i];
          out[j] = controls[i];
        }
      }
      work_count++;

      // Tell runtime system how many output items we produced.
      return 1;
    }

    void joystick_source_f_impl::update_controls()
    {
      int rc = 1;
      do {
        struct input_event ev;
        rc = libevdev_next_event(this->dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);
        if (rc == 0 && ev.type == EV_ABS) {
          for (int i = 0; i < 4; i++) {
            if (ev.code == this->controls_map[i]) {
              // Centre value around 0 so it can be inverted if required
              float value = ev.value - 127.0f;
              // Apply calibration values
              value = (value + calib_offset[i]) * calib_mult[i];
              // Clamp to 0 to 255
              value = std::max(0.0f, std::min(value + 127.0f, 255.0f));
              controls[i] = value;
            }
          }
        }

        gettimeofday(&last_event, NULL);
        long timediff = (last_event.tv_sec - last_update.tv_sec)*1000000L + last_event.tv_usec - last_update.tv_usec;
        if (timediff < 100000) continue;
        gettimeofday(&last_update, NULL);

        pmt:: pmt_t p_dict = pmt::make_dict();

        p_dict = pmt::dict_add(p_dict, pmt::intern("throttle"), pmt::from_long(controls[0]));
        p_dict = pmt::dict_add(p_dict, pmt::intern("yaw"), pmt::from_long(controls[1]));
        p_dict = pmt::dict_add(p_dict, pmt::intern("pitch"), pmt::from_long(controls[2]));
        p_dict = pmt::dict_add(p_dict, pmt::intern("roll"), pmt::from_long(controls[3]));

        message_port_pub(out_port, p_dict);

      } while (stop == 0 && (rc == 1 || rc == 0 ||  rc == -EAGAIN));
      printf("Thread died %d", rc);
    }
  } /* namespace hubsan */
} /* namespace gr */

