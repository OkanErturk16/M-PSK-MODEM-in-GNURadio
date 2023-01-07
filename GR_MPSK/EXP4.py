#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: Not titled yet
# GNU Radio version: 3.8.4.0

from distutils.version import StrictVersion

if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print("Warning: failed to XInitThreads()")

from PyQt5 import Qt
from gnuradio import qtgui
from gnuradio.filter import firdes
import sip
from gnuradio import blocks
from gnuradio import channels
from gnuradio import filter
from gnuradio import gr
import sys
import signal
from argparse import ArgumentParser
from gnuradio.eng_arg import eng_float, intx
from gnuradio import eng_notation
import M_PSK_EMBED
import THALV1
import myLib  # embedded python module

from gnuradio import qtgui

class EXP4(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "Not titled yet")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Not titled yet")
        qtgui.util.check_set_qss()
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

        self.settings = Qt.QSettings("GNU Radio", "EXP4")

        try:
            if StrictVersion(Qt.qVersion()) < StrictVersion("5.0.0"):
                self.restoreGeometry(self.settings.value("geometry").toByteArray())
            else:
                self.restoreGeometry(self.settings.value("geometry"))
        except:
            pass

        ##################################################
        # Variables
        ##################################################
        self.samp_rate_0 = samp_rate_0 = int(1e6)
        self.samp_rate = samp_rate = 32000
        self.n_zero = n_zero = 8
        self.n_time_sync = n_time_sync = 64
        self.n_frame_sync = n_frame_sync = 63
        self.n_data = n_data = 256

        ##################################################
        # Blocks
        ##################################################
        self.qtgui_freq_sink_x_0 = qtgui.freq_sink_c(
            1024, #size
            firdes.WIN_BLACKMAN_hARRIS, #wintype
            0, #fc
            samp_rate, #bw
            "", #name
            1
        )
        self.qtgui_freq_sink_x_0.set_update_time(0.10)
        self.qtgui_freq_sink_x_0.set_y_axis(-140, 10)
        self.qtgui_freq_sink_x_0.set_y_label('Relative Gain', 'dB')
        self.qtgui_freq_sink_x_0.set_trigger_mode(qtgui.TRIG_MODE_FREE, 0.0, 0, "")
        self.qtgui_freq_sink_x_0.enable_autoscale(False)
        self.qtgui_freq_sink_x_0.enable_grid(False)
        self.qtgui_freq_sink_x_0.set_fft_average(1.0)
        self.qtgui_freq_sink_x_0.enable_axis_labels(True)
        self.qtgui_freq_sink_x_0.enable_control_panel(False)



        labels = ['', '', '', '', '',
            '', '', '', '', '']
        widths = [1, 1, 1, 1, 1,
            1, 1, 1, 1, 1]
        colors = ["blue", "red", "green", "black", "cyan",
            "magenta", "yellow", "dark red", "dark green", "dark blue"]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
            1.0, 1.0, 1.0, 1.0, 1.0]

        for i in range(1):
            if len(labels[i]) == 0:
                self.qtgui_freq_sink_x_0.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_freq_sink_x_0.set_line_label(i, labels[i])
            self.qtgui_freq_sink_x_0.set_line_width(i, widths[i])
            self.qtgui_freq_sink_x_0.set_line_color(i, colors[i])
            self.qtgui_freq_sink_x_0.set_line_alpha(i, alphas[i])

        self._qtgui_freq_sink_x_0_win = sip.wrapinstance(self.qtgui_freq_sink_x_0.pyqwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_freq_sink_x_0_win)
        self.qtgui_const_sink_x_0 = qtgui.const_sink_c(
            1024, #size
            "", #name
            1 #number of inputs
        )
        self.qtgui_const_sink_x_0.set_update_time(0.10)
        self.qtgui_const_sink_x_0.set_y_axis(-2, 2)
        self.qtgui_const_sink_x_0.set_x_axis(-2, 2)
        self.qtgui_const_sink_x_0.set_trigger_mode(qtgui.TRIG_MODE_FREE, qtgui.TRIG_SLOPE_POS, 0.0, 0, "")
        self.qtgui_const_sink_x_0.enable_autoscale(False)
        self.qtgui_const_sink_x_0.enable_grid(False)
        self.qtgui_const_sink_x_0.enable_axis_labels(True)


        labels = ['', '', '', '', '',
            '', '', '', '', '']
        widths = [1, 1, 1, 1, 1,
            1, 1, 1, 1, 1]
        colors = ["blue", "red", "red", "red", "red",
            "red", "red", "red", "red", "red"]
        styles = [0, 0, 0, 0, 0,
            0, 0, 0, 0, 0]
        markers = [0, 0, 0, 0, 0,
            0, 0, 0, 0, 0]
        alphas = [1.0, 1.0, 1.0, 1.0, 1.0,
            1.0, 1.0, 1.0, 1.0, 1.0]

        for i in range(1):
            if len(labels[i]) == 0:
                self.qtgui_const_sink_x_0.set_line_label(i, "Data {0}".format(i))
            else:
                self.qtgui_const_sink_x_0.set_line_label(i, labels[i])
            self.qtgui_const_sink_x_0.set_line_width(i, widths[i])
            self.qtgui_const_sink_x_0.set_line_color(i, colors[i])
            self.qtgui_const_sink_x_0.set_line_style(i, styles[i])
            self.qtgui_const_sink_x_0.set_line_marker(i, markers[i])
            self.qtgui_const_sink_x_0.set_line_alpha(i, alphas[i])

        self._qtgui_const_sink_x_0_win = sip.wrapinstance(self.qtgui_const_sink_x_0.pyqwidget(), Qt.QWidget)
        self.top_layout.addWidget(self._qtgui_const_sink_x_0_win)
        self.freq_xlating_fir_filter_xxx_0_0 = filter.freq_xlating_fir_filter_ccc(1, [1], -0.25, 1)
        self.freq_xlating_fir_filter_xxx_0 = filter.freq_xlating_fir_filter_ccc(1, [1], 0.25, 1)
        self.channels_fading_model_0 = channels.fading_model( 8, 1/120000/4, True, 1, 0 )
        self.channels_channel_model_0 = channels.channel_model(
            noise_voltage=0.03*0,
            frequency_offset=0.0025/8,
            epsilon=1+20/1e6*1e-5,
            taps=[0,0,0,1,0.],
            noise_seed=0,
            block_tags=False)
        self.blocks_throttle_0 = blocks.throttle(gr.sizeof_gr_complex*1, 3200000,False)
        self.blocks_null_sink_0 = blocks.null_sink(gr.sizeof_gr_complex*1)
        self.THALV1_tagged_stream_trimmer_0 = THALV1.tagged_stream_trimmer()
        self.THALV1_myer_oerder_cpp_0 = THALV1.myer_oerder_cpp(4, 64)
        self.THALV1_correlate_and_tag_0 = THALV1.correlate_and_tag(n_frame_sync,n_data,0.5)
        self.THALV1_Tagged_stream_receiver_2 = THALV1.Tagged_stream_receiver()
        self.PSF_filter_0 = filter.fir_filter_ccc(2, myLib.x)
        self.PSF_filter_0.declare_sample_delay(0)
        self.PSF_filter = filter.fir_filter_ccc(1, myLib.x)
        self.PSF_filter.declare_sample_delay(0)
        self.M_PSK_EMBED = M_PSK_EMBED.blk(m=2, n_symbol=n_data, n_zero=n_zero, n_time_sync=n_time_sync, n_frame_sync=n_frame_sync, os=8)


        ##################################################
        # Connections
        ##################################################
        self.connect((self.M_PSK_EMBED, 0), (self.blocks_throttle_0, 0))
        self.connect((self.PSF_filter, 0), (self.freq_xlating_fir_filter_xxx_0, 0))
        self.connect((self.PSF_filter, 0), (self.qtgui_freq_sink_x_0, 0))
        self.connect((self.PSF_filter_0, 0), (self.THALV1_myer_oerder_cpp_0, 0))
        self.connect((self.THALV1_Tagged_stream_receiver_2, 0), (self.qtgui_const_sink_x_0, 0))
        self.connect((self.THALV1_correlate_and_tag_0, 1), (self.THALV1_tagged_stream_trimmer_0, 0))
        self.connect((self.THALV1_correlate_and_tag_0, 0), (self.blocks_null_sink_0, 0))
        self.connect((self.THALV1_myer_oerder_cpp_0, 0), (self.THALV1_correlate_and_tag_0, 0))
        self.connect((self.THALV1_tagged_stream_trimmer_0, 0), (self.THALV1_Tagged_stream_receiver_2, 0))
        self.connect((self.blocks_throttle_0, 0), (self.PSF_filter, 0))
        self.connect((self.channels_channel_model_0, 0), (self.freq_xlating_fir_filter_xxx_0_0, 0))
        self.connect((self.channels_fading_model_0, 0), (self.channels_channel_model_0, 0))
        self.connect((self.freq_xlating_fir_filter_xxx_0, 0), (self.channels_fading_model_0, 0))
        self.connect((self.freq_xlating_fir_filter_xxx_0_0, 0), (self.PSF_filter_0, 0))


    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "EXP4")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()

    def get_samp_rate_0(self):
        return self.samp_rate_0

    def set_samp_rate_0(self, samp_rate_0):
        self.samp_rate_0 = samp_rate_0

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.qtgui_freq_sink_x_0.set_frequency_range(0, self.samp_rate)

    def get_n_zero(self):
        return self.n_zero

    def set_n_zero(self, n_zero):
        self.n_zero = n_zero
        self.M_PSK_EMBED.n_zero = self.n_zero

    def get_n_time_sync(self):
        return self.n_time_sync

    def set_n_time_sync(self, n_time_sync):
        self.n_time_sync = n_time_sync
        self.M_PSK_EMBED.n_time_sync = self.n_time_sync

    def get_n_frame_sync(self):
        return self.n_frame_sync

    def set_n_frame_sync(self, n_frame_sync):
        self.n_frame_sync = n_frame_sync
        self.M_PSK_EMBED.n_frame_sync = self.n_frame_sync

    def get_n_data(self):
        return self.n_data

    def set_n_data(self, n_data):
        self.n_data = n_data
        self.M_PSK_EMBED.n_symbol = self.n_data





def main(top_block_cls=EXP4, options=None):

    if StrictVersion("4.5.0") <= StrictVersion(Qt.qVersion()) < StrictVersion("5.0.0"):
        style = gr.prefs().get_string('qtgui', 'style', 'raster')
        Qt.QApplication.setGraphicsSystem(style)
    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls()

    tb.start()

    tb.show()

    def sig_handler(sig=None, frame=None):
        Qt.QApplication.quit()

    signal.signal(signal.SIGINT, sig_handler)
    signal.signal(signal.SIGTERM, sig_handler)

    timer = Qt.QTimer()
    timer.start(500)
    timer.timeout.connect(lambda: None)

    def quitting():
        tb.stop()
        tb.wait()

    qapp.aboutToQuit.connect(quitting)
    qapp.exec_()

if __name__ == '__main__':
    main()
