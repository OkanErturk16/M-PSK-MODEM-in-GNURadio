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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "tagged_stream_trimmer_impl.h"
#include <time.h> // calisma zamani olcmek icin

namespace gr
{
  namespace THALV1
  {

    tagged_stream_trimmer::sptr
    tagged_stream_trimmer::make()
    {
      return gnuradio::get_initial_sptr(new tagged_stream_trimmer_impl());
    }

    /*
     * The private constructor
     */
    tagged_stream_trimmer_impl::tagged_stream_trimmer_impl()
        : gr::block("tagged_stream_trimmer",
                    gr::io_signature::make(1, 1, sizeof(gr_complex)),
                    gr::io_signature::make(1, 1, sizeof(gr_complex)))
    {
      this->previous_tag_n_remainder = 0;
      //set_output_multiple(2000);
      set_tag_propagation_policy(TPP_DONT);
    }

    /*
     * Our virtual destructor.
     */
    tagged_stream_trimmer_impl::~tagged_stream_trimmer_impl()
    {
    }

    void tagged_stream_trimmer_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = noutput_items;
    }

    int
    tagged_stream_trimmer_impl::general_work(int noutput_items,
                                             gr_vector_int &ninput_items,
                                             gr_vector_const_void_star &input_items,
                                             gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *)input_items[0];
      gr_complex *out = (gr_complex *)output_items[0];

      // CALCULATE THE OUTPUT LENGTH
      int len_out = noutput_items;
      int len_out2 = ninput_items[0];
      if (len_out > len_out2)
      {
        len_out = len_out2;
      }

      // store the number of produced output items in the general_work() function
      int out_produced = 0;
      //->Write the remaining data of the previous tag
      if (this->previous_tag_n_remainder > 0)
      {
        // if the number of input element is sufficient for previous tag remainders
        if (this->previous_tag_n_remainder < len_out)
        {
          for (int ii = 0; ii < this->previous_tag_n_remainder; ii++)
          {
            out[ii] = in[ii];
          }
          out_produced += this->previous_tag_n_remainder;
          this->previous_tag_n_remainder = 0;
          // the RETURN is at the end of the general_work() function
        }
        else // previous_tag_n_remainder >= len_out
        {
          for (int ii = 0; ii < len_out; ii++)
          {
            out[ii] = in[ii];
          }
          this->previous_tag_n_remainder -= len_out;
          consume_each(len_out);
          return len_out;
        }
      }

      // READ TAGS AND PARSE THE RECEIVED STREAM
      std::vector<tag_t> tags;
      get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0) + len_out);

      // IF THERE IS TAG CAPTURED IN THE INPUT ITEMS
      if (tags.size() > 0)
      {
        // DO FOR EACH TAGS IN THE INPUT STREAM
        for (int ii = 0; ii < tags.size(); ii++)
        {
          // TO DO: read "tag name" and check whether it is valid or not!
          int tag_len = pmt::to_long(tags[ii].value);
          int tag_pos = tags[ii].offset - nitems_read(0); // tag.offset: absolute position of the tag
          std::string tag_key = pmt::symbol_to_string(tags[ii].key);
          // IF TAGGED BLOCK CAN BE WRITTEN TO THE OUTPUT STREAM
          if ( tag_key == "packet_len") // check if the tag has the appropriate name!
          {
            if ((tag_pos + tag_len) < len_out)
            {

              memcpy(out + out_produced, in + tag_pos, sizeof(gr_complex) * tag_len);

              //TRANSFER THE TAG
              add_item_tag(0,                                // Port number
                          nitems_written(0) + out_produced, // Offset
                          tags[ii].key,                     // Key
                          tags[ii].value                    // Value
              );
              // INCREASE THE NUMER OF PRODUCED ELEMENTS
              out_produced += tag_len;
            }
            else // IF TAGGED BLOCK CAN'T BE WRITTEN TO THE OUTPUT STREAM
            {
              // CALCULATE HOW MANY OF THE TAGGED BLOCK ELEMENT CAN BE WRITTEN
              int n_item_to_wrt = len_out - tag_pos;
              memcpy(out + out_produced, in + tag_pos, sizeof(gr_complex) * n_item_to_wrt);
              // for (int jj = 0; jj < n_item_to_wrt; jj++)
              // {
              //   out[out_produced + jj] = in[tag_pos + jj];
              // }
              add_item_tag(0,                                // Port number
                          nitems_written(0) + out_produced,  // Offset
                          tags[ii].key,                      // Key
                          tags[ii].value                     // Value
              );
              // SET THE NUMBER OF REMAINING ELEMENTS OF THIS TAG
              this->previous_tag_n_remainder = tag_len - n_item_to_wrt;

              // INCREASE THE NUMER OF PRODUCED ELEMENTS
              out_produced += n_item_to_wrt;

              consume_each(len_out);
              return out_produced;
            }
          }
        }
      }

      // IF THERE IS NO TAG EXIST
      consume_each(len_out);
      return out_produced;
    }

  } /* namespace THALV1 */
} /* namespace gr */
