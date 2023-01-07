/* -*- c++ -*- */
/*
 * Copyright 2022 ITU_THAL.
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
#include <gnuradio/expj.h>
#include "Tagged_stream_receiver_impl.h"
#include <gnuradio/math.h> //include PI -> GR_M_PI
#include <volk/volk.h>
#include <limits>
/*
*/
namespace gr
{
  namespace THALV1
  {

    Tagged_stream_receiver::sptr
    Tagged_stream_receiver::make()
    {
      return gnuradio::get_initial_sptr(new Tagged_stream_receiver_impl());
    }

    /*
     * The private constructor
     */
    Tagged_stream_receiver_impl::Tagged_stream_receiver_impl()
        : gr::tagged_stream_block("Tagged_stream_receiver",
                                  gr::io_signature::make(1, 1, sizeof(gr_complex)),
                                  gr::io_signature::make(1, 1, sizeof(gr_complex)), "packet_len")
    {
      this->n_pilot      = 21;
      this->n_frame_sync = 63;
      this->n_data       = 256;
      this->Rk           = new gr_complex[this->n_frame_sync];


      //GENERATE ZADOFF-CHU SEQUENCE IS OF LENGTH "n_frame_sync"
      this->frame_sync_seq = new gr_complex[this->n_frame_sync];

      for (int ii = 0; ii < this->n_frame_sync; ii++)
      {
        float ii_f = float(ii);
        this->frame_sync_seq[ii] = gr_complex(cos(GR_M_PI * ii_f * (ii_f + 1) / this->n_frame_sync),
                                              sin(GR_M_PI * ii_f * (ii_f + 1) / this->n_frame_sync));
      }
      const int alignment_multiple = volk_get_alignment() / sizeof(gr_complex);
      set_alignment(std::max(1, alignment_multiple));
    }

    /*
     * Our virtual destructor.
     */

    Tagged_stream_receiver_impl::~Tagged_stream_receiver_impl()
    {
    }

    int Tagged_stream_receiver_impl::calculate_output_stream_length(const gr_vector_int &ninput_items)
    {
      int noutput_items = ninput_items[0];
      return noutput_items;
    }

    int Tagged_stream_receiver_impl::work(int noutput_items,
                                          gr_vector_int &ninput_items,
                                          gr_vector_const_void_star &input_items,
                                          gr_vector_void_star &output_items)
    {
      gr_complex *in           = (gr_complex *)input_items[0];
      gr_complex *out          = (gr_complex *)output_items[0];
      unsigned int len_input   = ninput_items[0];

      /////////////////////// CFO ESTIMATION-PART 1 ////////////////////////////////////////////

      int M = this->n_frame_sync;  // User defined parameter,
      gr_complex *rk = new gr_complex[M];
      volk_32fc_x2_multiply_32fc(rk, this->frame_sync_seq, in, this->n_frame_sync);

      gr_complex Rk_sum = gr_complex(0.0,0.0);
      for (int kk = 0; kk < M; kk++)
      {
        Rk[kk] = gr_complex(0.0,0.0);
        for(int ii = kk; ii<this->n_frame_sync; ii++)
        {
          Rk[kk] += rk[ii]*gr_complex(rk[ii-kk].real(), -rk[ii-kk].imag());
        }
        Rk[kk] /= (float)(this->n_frame_sync - kk);
        Rk_sum += Rk[kk]; 
      }
      float CFO_coarse = 2.0/(((float) (M+1)))*atan2(Rk_sum.imag(),Rk_sum.real());


      // COARSE CFO CORRECTION
      for (int ii = 0; ii < len_input; ii++)
      {
        out[ii] = in[ii]*gr_expj(-CFO_coarse*ii);
      }


      // FINE CFO ESTIMATION
      gr_complex pilot_diffs;
      volk_32fc_x2_conjugate_dot_prod_32fc(&pilot_diffs, out + this->n_frame_sync, out + this->n_frame_sync + this->n_data - this->n_pilot, this->n_pilot);
      float CFO_fine = -1.0/((float)(-this->n_pilot + this->n_data))*atan2(pilot_diffs.imag(),pilot_diffs.real());

      // FINE CFO COMPANSATION
      for (int ii = 0; ii < len_input; ii++)
      {
        out[ii] = out[ii]*gr_expj(-CFO_fine*ii);
      }



      // CHANNEL ESTIMATION
      // Sync word uzerinden channel estimation yapiliyor !
      gr_complex channel_coeff;
      volk_32fc_x2_dot_prod_32fc(&channel_coeff, out, this->frame_sync_seq, this->n_frame_sync);
      float channel_coeff_mag2      = 1.0/((float)this->n_frame_sync)*(channel_coeff.real()*channel_coeff.real() + channel_coeff.imag()*channel_coeff.imag());
      gr_complex channel_coeff_inv  = gr_complex(channel_coeff.real()/channel_coeff_mag2, -channel_coeff.imag()/channel_coeff_mag2);
      volk_32fc_s32fc_multiply_32fc(out,out,channel_coeff_inv,len_input);


      volk_32fc_s32fc_multiply_32fc(out,out,0.0,this->n_frame_sync);

      out = out + this->n_frame_sync;
      return ninput_items[0];
    }
  } /* namespace THALV1 */
} /* namespace gr */
