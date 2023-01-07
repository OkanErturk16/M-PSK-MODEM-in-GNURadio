/* -*- c++ -*- */
/*
 * Copyright 2022 THAL.
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

#ifndef INCLUDED_THALV1_MYER_OERDER_CPP_H
#define INCLUDED_THALV1_MYER_OERDER_CPP_H

#include <THALV1/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace THALV1 {

    /*!
     * \brief <+description of block+>
     * \ingroup THALV1
     *
     */
    class THALV1_API myer_oerder_cpp : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<myer_oerder_cpp> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of THALV1::myer_oerder_cpp.
       *
       * To avoid accidental use of raw pointers, THALV1::myer_oerder_cpp's
       * constructor is in a private implementation
       * class. THALV1::myer_oerder_cpp::make is the public interface for
       * creating new instances.
       */
      static sptr make(int os, int n_symbol);
    };

  } // namespace THALV1
} // namespace gr

#endif /* INCLUDED_THALV1_MYER_OERDER_CPP_H */

