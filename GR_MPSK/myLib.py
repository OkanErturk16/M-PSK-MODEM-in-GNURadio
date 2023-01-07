# this module will be imported in the into your flowgraph
import numpy as np
from gnuradio.filter import firdes

os = 8
rrc_filter_alpha = 0.5
n_frame_sync     = 127
MF_len           = 4




#NRZ filter
#x = np.ones(os,dtype = np.complex64)/np.sqrt(2)

#RRC filter
x = firdes.root_raised_cosine(25, os, 1, rrc_filter_alpha,MF_len*os)
x = x/np.sqrt(np.sum(np.abs(x)**2))


# Frame_sync_seq
frame_sync_seq   = np.exp(-1j*np.pi*np.arange(n_frame_sync)*(np.arange(n_frame_sync)+1)/n_frame_sync)

# Correlator filter taps
corr_filter_taps = np.concatenate((np.zeros(n_frame_sync,dtype = np.complex64),np.ones(n_frame_sync,dtype = np.complex64)))


snr_filter_taps = np.ones(8)/8
