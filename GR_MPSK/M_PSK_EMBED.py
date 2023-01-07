
import numpy as np
from gnuradio import gr


class circBuffer:
    def __init__(self, len_max=int(1e6), data_type=np.float32):
        self.len_max = len_max
        self.data_type = data_type
        self.buffer = np.zeros(self.len_max, dtype=self.data_type)
        self.RD_ptr = 0
        self.WRT_ptr = 0
        self.elements_in_buffer = 0

    def WRT(self, x):
        #x: data (np.array)
        len_x = len(x)
        self.elements_in_buffer += len_x

        if (len_x + self.WRT_ptr) < self.len_max:
            self.buffer[self.WRT_ptr:(self.WRT_ptr+len_x)] = x
            self.WRT_ptr += len_x

        else:
            len_x1 = self.len_max - self.WRT_ptr
            len_x2 = len_x - len_x1
            self.buffer[-len_x1:] = x[:len_x1]
            self.buffer[:len_x2] = x[len_x1:]
            self.WRT_ptr = len_x2

    def RD(self, len_x):
        self.elements_in_buffer -= len_x
        if (len_x + self.RD_ptr) < self.len_max:
            x = self.buffer[self.RD_ptr:(self.RD_ptr+len_x)]
            self.RD_ptr += len_x
        else:
            x = np.zeros(len_x, dtype=self.data_type)
            len_x1 = self.len_max - self.RD_ptr
            len_x2 = len_x - len_x1

            x[:len_x1] = self.buffer[-len_x1:]
            x[len_x1:] = self.buffer[:len_x2]
            self.RD_ptr = len_x2
        return x

    def is_RD_available(self, len_x):
        if self.elements_in_buffer >= len_x:
            return True
        else:
            return False

    def is_WRT_available(self, x):
        if (self.len_max - self.elements_in_buffer) >= len(x):
            return True
        else:
            return False

    def reset_buffer(self):
        self.WRT_ptr = self.RD_ptr
        self.elements_in_buffer = 0

    def extend_buffer(self, len_extension):
        self.buffer = np.concatenate((self.buffer[:(self.WRT_ptr+1)], np.zeros(
            len_extension, dtype=self.data_type), self.buffer[(self.WRT_ptr+1):]))
        if self.WRT_ptr < self.RD_ptr:
            self.RD_ptr += len_extension


class blk(gr.sync_block):  # other base classes are basic_block, decim_block, interp_block

    def __init__(self, m=4, n_symbol=100, n_zero=20, n_time_sync=128, n_frame_sync=63, os=4):

        gr.sync_block.__init__(
            self,
            name=' My M-PSK Modulator',
            in_sig=[],
            out_sig=[np.complex64]
        )

        self.n_symbol = n_symbol       # number of symbols
        self.n_zero = n_zero         # len of zero padding
        self.n_time_sync = n_time_sync    # +-1 alternating sequence length
        # Zadoff-Chu sequence length (odd values are better)
        self.n_frame_sync = n_frame_sync
        self.os = os                   # oversampling FACTOR
        self.m = m                    # bit/symbol
        self.M = int(2**m)            # M of M-PSK
        self.constellation = np.exp( 1j*2.0*np.pi*np.arange(0, self.M)/self.M)  # constellation points
        
        # zero padding between the transmit pulses
        self.zero_padding_seq = np.zeros(self.n_zero, dtype=np.complex64)
        self.time_sync_seq = np.tile(np.array([-1, 1], dtype=np.complex64), int(n_time_sync/2))
        self.frame_sync_seq = 1.4414*np.exp(-1j*np.pi*np.arange(self.n_frame_sync)
                                     * (np.arange(self.n_frame_sync)+1)/self.n_frame_sync)
        self.pilot_seq = 1j*np.array([-1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1], dtype=np.complex64)
        
        # it is predefined and will be used in "bit_to_symbol_mapper" method
        self.power_list = 2**(np.arange(self.m))[:, np.newaxis]
        
        # the singal not consumed will be stored in this buffer
        self.signal_buffer = circBuffer(int(1e6), data_type=np.complex64)

    def bit_to_symbol_mapper(self, B):
        d = np.sum(B*self.power_list, axis=0)
        a = self.constellation[d]
        return a

    def pulse_train_generator(self, a):
        data_plus_plot = np.concatenate((self.pilot_seq,
                                        a[len(self.pilot_seq):-
                                        len(self.pilot_seq)],
                                        self.pilot_seq))

        a = np.concatenate((self.zero_padding_seq,
                            self.time_sync_seq,
                            self.frame_sync_seq,
                            data_plus_plot))

        a_os = np.zeros(len(a)*self.os, dtype=np.complex64)
        a_os[::self.os] = a
        return a_os

    def transmit_signal_generate(self):
        st0 = np.random.get_state()
        np.random.seed(0)
        B = np.random.randint(0, high=2, size=(self.m, self.n_symbol))
        np.random.set_state(st0)
        a = self.bit_to_symbol_mapper(B)
        a_os = self.pulse_train_generator(a)
        return a_os

    # This is the method triggered by the GNU Radio scheduler
    def work(self, input_items, output_items):
        len_out = len(output_items[0])

        flag = True
        while flag:
            if not self.signal_buffer.is_RD_available(len_out):
                generated_signal = self.transmit_signal_generate()
                if self.signal_buffer.is_WRT_available(generated_signal):
                    self.signal_buffer.WRT(generated_signal)
                else:
                    print('Buffer Dolu!')
            else:
                output_items[0][:] = self.signal_buffer.RD(len_out)
                flag = False

        return len(output_items[0])
