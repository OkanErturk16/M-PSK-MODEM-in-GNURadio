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

#ifndef INCLUDED_THALV1_TAGGED_STREAM_RECEIVER_IMPL_H
#define INCLUDED_THALV1_TAGGED_STREAM_RECEIVER_IMPL_H

#include <THALV1/Tagged_stream_receiver.h>

namespace gr
{
  namespace THALV1
  {

    class Tagged_stream_receiver_impl : public Tagged_stream_receiver
    {
    private:
      gr_complex *frame_sync_seq;
      gr_complex *frame_sync_seq_phase;
      gr_complex *Rk;

      int n_frame_sync;
      int n_pilot;
      int n_data;
      float * CFO_coarse_list;
      //float d_multiplier;
      // Nothing to declare in this block.

    protected:
      int calculate_output_stream_length(const gr_vector_int &ninput_items);

    public:
      Tagged_stream_receiver_impl();
      ~Tagged_stream_receiver_impl();

      // Where all the action really happens
      int work(
          int noutput_items,
          gr_vector_int &ninput_items,
          gr_vector_const_void_star &input_items,
          gr_vector_void_star &output_items);
    };

  } // namespace THALV1
} // namespace gr

#endif /* INCLUDED_THALV1_TAGGED_STREAM_RECEIVER_IMPL_H */
