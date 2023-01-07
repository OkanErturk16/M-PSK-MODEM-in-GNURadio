#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include <gnuradio/math.h> //include PI -> GR_M_PI
#include "correlate_and_tag_impl.h"
#include <time.h>    // calisma zamani olcmek icin
#include <pmt/pmt.h> // polymorphic type kutuphanesi
#include <volk/volk.h>


#include <chrono>

using namespace std;
using namespace std::chrono;
namespace gr
{
  namespace THALV1
  {

    correlate_and_tag::sptr
    correlate_and_tag::make(int n_frame_sync, int n_data, float threshold)
    {
      return gnuradio::get_initial_sptr(new correlate_and_tag_impl(n_frame_sync, n_data, threshold));
    }

    /*
      * The private constructor
      */
    correlate_and_tag_impl::correlate_and_tag_impl(int n_frame_sync, int n_data, float threshold)
        : gr::sync_block("correlate_and_tag",
                        gr::io_signature::make(1, 1, sizeof(gr_complex)),
                        gr::io_signature::make2(2, 2, sizeof(gr_complex), sizeof(gr_complex)))
    {
      this->d_n_frame_sync = n_frame_sync;
      this->d_n_data = n_data;
      this->d_threshold = threshold;
      this->WRT_counter = 0;

      // Memory allocation for frame sync sequence
      this->frame_sync_seq = new gr_complex[this->d_n_frame_sync];

      // Generate frame sync sequence
      for (int ii = 0; ii < this->d_n_frame_sync; ii++)
      {
        float ii_f = float(ii);
        this->frame_sync_seq[ii] = gr_complex(cos(GR_M_PI * ii_f * (ii_f + 1) / this->d_n_frame_sync),
                                              sin(GR_M_PI * ii_f * (ii_f + 1) / this->d_n_frame_sync));
      }


      const int alignment_multiple = volk_get_alignment() / sizeof(gr_complex);
      set_alignment(std::max(1, alignment_multiple));
      set_history(this->d_n_frame_sync);
      //set_output_multiple(20000);
    }

    /*
      * Our virtual destructor.
      */
    correlate_and_tag_impl::~correlate_and_tag_impl()
    {
    }

    int correlate_and_tag_impl::work(int noutput_items,
                                      gr_vector_const_void_star &input_items,
                                      gr_vector_void_star &output_items)
    {
      auto start = high_resolution_clock::now();

      // clock sayaci baslat
      //clock_t start = clock();

      const gr_complex *in = (const gr_complex *)input_items[0];
      gr_complex *out0     = (gr_complex *)output_items[0];
      gr_complex *out1     = (gr_complex *)output_items[1];



      float *in_abs = new float[noutput_items + this->d_n_frame_sync - 1];
      volk_32fc_magnitude_32f(in_abs, in, noutput_items + this->d_n_frame_sync - 1);
      int ii = 0;
      int jj = 0;


      gr_complex correlation_ii;

      gr_complex sum_in = gr_complex(1e-5,0);
      for (jj = 0; jj < d_n_frame_sync; jj++)
      {
        sum_in += in_abs[jj];
      }

      for (ii = 0; ii < noutput_items; ii++)
      {

        //calculate correlation
        volk_32fc_x2_dot_prod_32fc(&correlation_ii, in + ii, this->frame_sync_seq, d_n_frame_sync);
        out0[ii] = correlation_ii;
        out0[ii] = abs(out0[ii] /sum_in);
        sum_in = sum_in - in_abs[ii] + in_abs[ii+d_n_frame_sync+1];

        if (out0[ii].real() < d_threshold)
        {
          this->WRT_counter += 1;
        }
        else
        {
          if (this->WRT_counter > (d_n_frame_sync + d_n_data))
          {

            add_item_tag(0,                                        // Port number
                         nitems_written(0) + ii,                   // Offset
                         pmt::mp("packet_len"),                    // Key
                         pmt::from_long(d_n_frame_sync + d_n_data) // Value
            );
            add_item_tag(1,                                        // Port number
                         nitems_written(0) + ii,                   // Offset
                         pmt::mp("packet_len"),                    // Key
                         pmt::from_long(d_n_frame_sync + d_n_data) // Value
            );

            this->WRT_counter = 0;
          }
          else
          {
            this->WRT_counter += 1;
          }
        }
      }

      memcpy(out1, &in[0], sizeof(gr_complex) * noutput_items);


      return noutput_items;
    }

  } /* namespace THALV1 */
} /* namespace gr */
