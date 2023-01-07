import numpy as np
from gnuradio import gr


###################### INTERPOLATOR ICIN YAZILMIS OLAN CLASS YAPISI ####################################
class THAL_farrow_interpolator:
    #------> Class constructor
    def __init__(self):
        self.C = np.array([[ -0.013824,  0.003143,   0.055298,  -0.012573],
                            [ 0.054062, -0.019287,  -0.216248,   0.077148],
                            [-0.157959,  0.100800,   0.631836,  -0.403198],
                            [ 0.616394, -1.226364,  -0.465576,   0.905457],
                            [ 0.616394,  1.226364,  -0.465576,  -0.905457],
                            [-0.157959, -0.100800,   0.631836,   0.403198],
                            [ 0.054062,  0.019287,  -0.216248,  -0.077148],
                            [-0.013824, -0.003143,   0.055298,  0.012573]], dtype = np.complex64)
        self.m = np.array([0,1,2,3],dtype = np.int64)

    #------> Interpolation method
    def interpolate(self,x,alpha):
        y_m = np.sum(x[:,np.newaxis]*self.C,axis = 0)
        z   = np.sum(alpha**(self.m)*y_m)
        return z
########################################################################################################



####################### MYER OERDER TIMING SYNC ICIN KULLANILACAK ######################################
class myer_oerder_timing_sync(gr.basic_block):
    #------> Class constructor
    def __init__(self):
        gr.basic_block.__init__(self,
                                name    = "myer_oerder_timing_sync",
                                in_sig  = [np.complex64],
                                out_sig = [np.complex64])

        # Number of symbol for each frame.
        self.L  = 64

        # over sampling factor
        self.os = 8

        #initial value for timing error.
        self.eps_old = 0

        # A kernel 
        self.MYER_OERDER_KERNEL  = np.exp(-1j*2*np.pi*np.arange(0,self.L*self.os)/self.os)

        # Define a fractional interpolator object
        self.farrow_interpolator = THAL_farrow_interpolator()


        # Interpolasyon yapmak icin gerekli
        self.history_for_interpolator = 4 + 4 + self.os + 1
        #                               |   |       |      
        #                               |   |       |      
        #                               |   |       \____ +- os/2 durumlari icin bellek 
        #                               |   \____________ memory for forward  interpolation
        #                               \________________ memory for backward interpolation    


        # History set edildi. NOT: history = 1 iken hic bellek elemani sayisi 0 olur...
        self.set_history(self.history_for_interpolator + 1)

        # frame uzunlugu
        self.set_output_multiple(self.L*self.os)


    #------> Class constructor
    def forecast(self, noutput_items, ninput_items_required):
        for i in range(len(ninput_items_required)):
            #history elemani kadar fazla eleman eslenmesi gerekiyor...
            ninput_items_required[i] = noutput_items + self.history_for_interpolator 


    def general_work(self, input_items, output_items):

        # eleman sayisi
        len_out   = len(output_items[0][:])

        # giris elemanlarini "in0" elemanina kopyala
        in0          = input_items[0][:] 

        # bir frame'in sample uzunlugu
        len_frame = self.L*self.os

        # input elemanlarinda kac adet "frame" oldugunu hesapla
        num_of_frame = len_out//len_frame

        #########################################################################################3S
        # okunan arrayin referans baslangic noktasi, bu noktadan onceki elemanlar memory olacak
        # 4 cikarilmasinin sebebi ise interpolasyon icin interpole edilecek noktanin onu ve arkasindan "4"er eleman gerekli
        # (history_for_interpolator = os + 4 + 4) olarak secildi.-> Bunun sebebi, eger eps = -os/2 civarinda olursa o noktadan once -4 eleman daha gerekli.
        #                              |   |   |
        #                              |   |   \____ memory for backward interpolation
        #                              |   \________ memory for forward  interpolation
        #                              \____________ +- os/2 durumlari icin bellek
        ii_init   = self.history_for_interpolator - 4
        ########################################################################################

        # counter of output items
        kk = 0
        for nn in range(num_of_frame):
            # incelenecek olan frame kesiti
            in0_nn = in0[(ii_init + nn*self.L*self.os):(ii_init + (nn+1)*self.L*self.os)]

            #MYER OERDER ALGORITMASINI OLUSTUR
            x_k = np.abs(in0_nn)**2
            X_m = np.sum(x_k*self.MYER_OERDER_KERNEL)
            eps_m = (-0.5/np.pi*np.angle(X_m))*self.os
            


            ###########
            if (eps_m - self.eps_old) < (self.os*0.5):
                eps_m += self.os
            elif (eps_m - self.eps_old) > (self.os*1.5):
                eps_m -= self.os
            
            while eps_m<(self.os*self.L):

                #integer timing offset
                eps_m_int   = np.round(eps_m)

                #fractional timing offset
                eps_m_frac  = eps_m - eps_m_int

                #find where the optim
                idx_optimal_int = int(eps_m_int + ii_init + nn*len_frame)

                #fractional interpolation
                if eps_m_frac >= 0:
                    output_items[0][kk] = self.farrow_interpolator.interpolate(in0[(idx_optimal_int-3):(idx_optimal_int+5)],eps_m_frac -0.5)
                else:
                    output_items[0][kk] = self.farrow_interpolator.interpolate(in0[(idx_optimal_int-4):(idx_optimal_int+4)],eps_m_frac +0.5)

                kk += 1
                eps_m += self.os
            
            self.eps_old  = eps_m
            self.eps_old -= self.os*(self.L+1)


            
        self.consume_each(num_of_frame*len_frame)
        return kk
