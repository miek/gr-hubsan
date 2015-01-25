#!/usr/bin/env python
# -*- coding: utf-8 -*-
# 
# Copyright 2014 <+YOU OR YOUR COMPANY+>.
# 
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this software; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.
# 

import bitarray
import numpy
import struct
import threading
from evdev import InputDevice, categorize, ecodes
from gnuradio import gr

class build_packet_f(gr.sync_block):
    """
    docstring for block build_packet_f
    """
    def __init__(self, a7105_id, tx_id):
        gr.sync_block.__init__(self,
            name="build_packet_f",
            in_sig=None,
            out_sig=[numpy.float32])

        self.a7105_id = a7105_id
        self.tx_id = tx_id

        self.sample_count = 0

        self.control_values = { 'T': (-1, 0, 0), 'P': (-1, 0, 0), 'R': (-1, 0, 0), 'Y': (-1, 0, 0) }


        control_thread = threading.Thread(target=self.get_controls)
        control_thread.daemon = True
        control_thread.start()

    def bool_to_sym(self, b):
        return 1.0 if b else -1.0

    def work(self, input_items, output_items):
        out = output_items[0]
        for i in range(len(out)):
            if self.sample_count < 192:
                # Transmit packet
                if self.sample_count == 0:
                    self.pkt = self.build_packet(self.a7105_id, self.get_value('T'), self.get_value('Y'), self.get_value('P'), self.get_value('R'), self.tx_id)
                out[i] = self.bool_to_sym(self.pkt[self.sample_count])
            else:
                # Transmit padding between packets
                out[i] = 0.0

            self.sample_count += 1

            if self.sample_count >= 1000:
                self.sample_count = 0

        return len(output_items[0])


    def gen_checksum(self, bytes):
        s = sum(ord(c) for c in bytes)
        cs = 256 - (s % 256)
        return cs if cs < 256 else 0

    def build_packet(self, addr, throttle, rudder, elevator, aileron, tx_id):
        pkt = bitarray.bitarray()
        pkt.frombytes(struct.pack('>II', 0xAAAAAAAA, addr))

        payload = bitarray.bitarray()
        payload.frombytes(struct.pack('>B4HBBI', 0x20, throttle, rudder, elevator, aileron, 0x02, 0x61, tx_id))

        checksum = self.gen_checksum(payload.tobytes())
        pkt += payload
        pkt.frombytes(struct.pack('>B', checksum))
        return pkt

    def update_value(self, axis, value):
        current_value, min_value, max_value = self.control_values[axis]
        if current_value == -1:
            self.control_values[axis] = (value, value, value)
            return
        min_value = min(min_value, value)
        max_value = max(max_value, value)
        self.control_values[axis] = (value, min_value, max_value)

    def get_value(self, axis):
        current_value, min_value, max_value = self.control_values[axis]
        if current_value < 0:
            return 0
        if axis == 'T':
            throttle = (current_value - min_value) * 256 / (max_value - min_value + 1)
            return throttle if throttle > 10 else 0
        else:
            return ((current_value - min_value) * 128 / (max_value - min_value + 1)) + 64

    def print_values(self):
        print "T: %d P: %d R: %d Y: %d" % tuple([self.get_value(c) for c in "TPRY"])

    def get_controls(self):
        controller = InputDevice('/dev/input/by-id/usb-Logitech_Inc._WingMan_Force_3D-event-joystick')

        controller.grab()

        axes = {
            ecodes.ABS_THROTTLE: ('T', True),
            ecodes.ABS_Y: ('P', False),
            ecodes.ABS_X: ('R', True),
            ecodes.ABS_RZ: ('Y', False)
        }


        for event in controller.read_loop():
            if event.type == ecodes.EV_ABS and event.code in axes:
                axis, flip = axes[event.code]
                value = event.value
                if flip:
                    value = 255 - value
                self.update_value(axis, value)
                #self.print_values()
