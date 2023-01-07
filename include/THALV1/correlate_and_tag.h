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

#ifndef INCLUDED_THALV1_CORRELATE_AND_TAG_H
#define INCLUDED_THALV1_CORRELATE_AND_TAG_H

#include <THALV1/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace THALV1 {

    /*!
     * \brief <+description of block+>
     * \ingroup THALV1
     *
     */
    class THALV1_API correlate_and_tag : virtual public gr::sync_block
    {
     public:
      typedef boost::shared_ptr<correlate_and_tag> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of THALV1::correlate_and_tag.
       *
       * To avoid accidental use of raw pointers, THALV1::correlate_and_tag's
       * constructor is in a private implementation
       * class. THALV1::correlate_and_tag::make is the public interface for
       * creating new instances.
       */
      static sptr make(int n_frame_sync, int n_data, float threshold);
    };

  } // namespace THALV1
} // namespace gr

#endif /* INCLUDED_THALV1_CORRELATE_AND_TAG_H */

