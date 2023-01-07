/* -*- c++ -*- */
/*
 * Copyright 2022 gr-THALV1 author.
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

#ifndef INCLUDED_THALV1_CORRELATE_AND_TAG_IMPL_H
#define INCLUDED_THALV1_CORRELATE_AND_TAG_IMPL_H

#include <THALV1/correlate_and_tag.h>

namespace gr {
  namespace THALV1 {

    class correlate_and_tag_impl : public correlate_and_tag
    {
     private:
     int d_n_frame_sync;
     int d_n_data;
     float d_threshold;
     int WRT_counter;
     gr_complex* frame_sync_seq;
      // Nothing to declare in this block.

     public:
      correlate_and_tag_impl(int n_frame_sync, int n_data, float threshold);
      ~correlate_and_tag_impl();

      // Where all the action really happens
      int work(
              int noutput_items,
              gr_vector_const_void_star &input_items,
              gr_vector_void_star &output_items
      );
    };

  } // namespace THALV1
} // namespace gr

#endif /* INCLUDED_THALV1_CORRELATE_AND_TAG_IMPL_H */

