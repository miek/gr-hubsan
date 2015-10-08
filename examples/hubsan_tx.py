#!/usr/bin/env python2
##################################################
# GNU Radio Python Flow Graph
# Title: Hubsan Controller
# Author: Mike Walters
# Description: Based on gr-fsk-burst by Tim O'Shea
# Generated: Thu Oct  8 21:59:39 2015
##################################################

if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print "Warning: failed to XInitThreads()"

from PyQt4 import Qt
from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import filter
from gnuradio import gr
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
import es
import hubsan
import numpy
import osmosdr
import pmt
import pyqt
import sys
import time


class hubsan_tx(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "Hubsan Controller")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Hubsan Controller")
        try:
             self.setWindowIcon(Qt.QIcon.fromTheme('gnuradio-grc'))
        except:
             pass
        self.top_scroll_layout = Qt.QVBoxLayout()
        self.setLayout(self.top_scroll_layout)
        self.top_scroll = Qt.QScrollArea()
        self.top_scroll.setFrameStyle(Qt.QFrame.NoFrame)
        self.top_scroll_layout.addWidget(self.top_scroll)
        self.top_scroll.setWidgetResizable(True)
        self.top_widget = Qt.QWidget()
        self.top_scroll.setWidget(self.top_widget)
        self.top_layout = Qt.QVBoxLayout(self.top_widget)
        self.top_grid_layout = Qt.QGridLayout()
        self.top_layout.addLayout(self.top_grid_layout)

        self.settings = Qt.QSettings("GNU Radio", "hubsan_tx")
        self.restoreGeometry(self.settings.value("geometry").toByteArray())

        ##################################################
        # Variables
        ##################################################
        self.samp_rate = samp_rate = 800e3
        self.dev = dev = 186e3
        self.data_rate = data_rate = 100e3
        self.sps = sps = samp_rate/data_rate
        
        self.rrc_taps = rrc_taps = firdes.root_raised_cosine(1.0, samp_rate, dev*2, 0.35, 21)
          
        self.interp = interp = 10

        ##################################################
        # Blocks
        ##################################################
        self.pyqt_time_plot_0 = pyqt.time_plot("")
        self._pyqt_time_plot_0_win = self.pyqt_time_plot_0;
        self.top_layout.addWidget(self._pyqt_time_plot_0_win)
        self.osmosdr_sink_0 = osmosdr.sink( args="numchan=" + str(1) + " " + "" )
        self.osmosdr_sink_0.set_sample_rate(samp_rate*interp)
        self.osmosdr_sink_0.set_center_freq(2.43e9, 0)
        self.osmosdr_sink_0.set_freq_corr(0, 0)
        self.osmosdr_sink_0.set_gain(14, 0)
        self.osmosdr_sink_0.set_if_gain(30, 0)
        self.osmosdr_sink_0.set_bb_gain(30, 0)
        self.osmosdr_sink_0.set_antenna("", 0)
        self.osmosdr_sink_0.set_bandwidth(0, 0)
          
        self.interp_fir_filter_xxx_0 = filter.interp_fir_filter_ccc(interp, (rrc_taps))
        self.interp_fir_filter_xxx_0.declare_sample_delay(0)
        self.hubsan_joystick_source_f_0 = hubsan.joystick_source_f()
        self.hubsan_build_packet_b_0 = hubsan.build_packet_b(1, 1025600234, 1179779243)
        self.fsk_modulator_0 = pyqt.pdu_lambda(lambda x: numpy.tile(x,[sps,1]).T.reshape([1,len(x)*sps]))
        self.fsk_modulator = pyqt.pdu_lambda(lambda x: numpy.array(numpy.exp(1j*2*numpy.pi*((dev*x*numpy.arange(len(x)))/samp_rate)),dtype="complex64"))
        self.es_source_0 = es.source(1*[gr.sizeof_gr_complex], 1, 2)
        self.blocks_message_strobe_0 = blocks.message_strobe(pmt.intern("FLY"), 10)
        self.bits_to_symbols = pyqt.pdu_lambda(lambda x: numpy.array(x, dtype=numpy.float32)*2-1)

        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.bits_to_symbols, 'pdus'), (self.fsk_modulator_0, 'pdus'))    
        self.msg_connect((self.bits_to_symbols, 'pdus'), (self.pyqt_time_plot_0, 'pdus'))    
        self.msg_connect((self.blocks_message_strobe_0, 'strobe'), (self.hubsan_build_packet_b_0, 'generate'))    
        self.msg_connect((self.fsk_modulator, 'pdus'), (self.es_source_0, 'schedule_event'))    
        self.msg_connect((self.fsk_modulator_0, 'pdus'), (self.fsk_modulator, 'pdus'))    
        self.msg_connect((self.hubsan_build_packet_b_0, 'pdus'), (self.bits_to_symbols, 'pdus'))    
        self.msg_connect((self.hubsan_joystick_source_f_0, 'out'), (self.hubsan_build_packet_b_0, 'in'))    
        self.connect((self.es_source_0, 0), (self.interp_fir_filter_xxx_0, 0))    
        self.connect((self.interp_fir_filter_xxx_0, 0), (self.osmosdr_sink_0, 0))    

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "hubsan_tx")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.set_sps(self.samp_rate/self.data_rate)
        self.fsk_modulator.set_fn(lambda x: numpy.array(numpy.exp(1j*2*numpy.pi*((self.dev*x*numpy.arange(len(x)))/self.samp_rate)),dtype="complex64"))
        self.osmosdr_sink_0.set_sample_rate(self.samp_rate*self.interp)

    def get_dev(self):
        return self.dev

    def set_dev(self, dev):
        self.dev = dev
        self.fsk_modulator.set_fn(lambda x: numpy.array(numpy.exp(1j*2*numpy.pi*((self.dev*x*numpy.arange(len(x)))/self.samp_rate)),dtype="complex64"))

    def get_data_rate(self):
        return self.data_rate

    def set_data_rate(self, data_rate):
        self.data_rate = data_rate
        self.set_sps(self.samp_rate/self.data_rate)

    def get_sps(self):
        return self.sps

    def set_sps(self, sps):
        self.sps = sps
        self.fsk_modulator_0.set_fn(lambda x: numpy.tile(x,[self.sps,1]).T.reshape([1,len(x)*self.sps]))

    def get_rrc_taps(self):
        return self.rrc_taps

    def set_rrc_taps(self, rrc_taps):
        self.rrc_taps = rrc_taps
        self.interp_fir_filter_xxx_0.set_taps((self.rrc_taps))

    def get_interp(self):
        return self.interp

    def set_interp(self, interp):
        self.interp = interp
        self.osmosdr_sink_0.set_sample_rate(self.samp_rate*self.interp)


if __name__ == '__main__':
    parser = OptionParser(option_class=eng_option, usage="%prog: [options]")
    (options, args) = parser.parse_args()
    from distutils.version import StrictVersion
    if StrictVersion(Qt.qVersion()) >= StrictVersion("4.5.0"):
        Qt.QApplication.setGraphicsSystem(gr.prefs().get_string('qtgui','style','raster'))
    qapp = Qt.QApplication(sys.argv)
    tb = hubsan_tx()
    tb.start(10000)
    tb.show()

    def quitting():
        tb.stop()
        tb.wait()
    qapp.connect(qapp, Qt.SIGNAL("aboutToQuit()"), quitting)
    qapp.exec_()
    tb = None  # to clean up Qt widgets
