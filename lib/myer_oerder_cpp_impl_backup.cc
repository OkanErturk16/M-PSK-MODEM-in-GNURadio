#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


// TO DO: +-4 degerlerini interpolator pre/post history olarak duzenle! const int olarak
#include <gnuradio/io_signature.h>
#include "myer_oerder_cpp_impl.h"
#include <gnuradio/math.h> //include PI -> GR_M_PI

#include <chrono>

using namespace std;
using namespace std::chrono;
namespace gr {
  namespace THALV1 {

    myer_oerder_cpp::sptr
    myer_oerder_cpp::make(int os, int n_symbol)
    {
      return gnuradio::get_initial_sptr
        (new myer_oerder_cpp_impl(os, n_symbol));
    }


    /*
     * The private constructor
     */
    myer_oerder_cpp_impl::myer_oerder_cpp_impl(int os, int n_symbol)
      : gr::block("myer_oerder_cpp",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(1, 1, sizeof(gr_complex)))
    {
      // DEFINE LENGTH OF ELEMENTS TO BE PROCESSED AS A CHUNK/////////
      this->os      = os;       // oversampling factor (sample/symbol)
      this->L       = n_symbol; // how many elements to be used in a chunk
      this->eps_old = 0.0;      // initial value for timing error
      ////////////////////////////////////////////////////////////////


      /////////////////// FOURIER SERIES KERNEL///////////////////////6tt
      this->MYER_OERDER_KERNEL = new gr_complex[this->os*this->L];
      float os_f = float(this->os);
      float ii_f = 0;
      for(int ii = 0; ii<(this->os*this->L);ii++)
      {
        this->MYER_OERDER_KERNEL[ii] = gr_complex(cos(GR_M_TWOPI*ii_f/os_f),-sin(GR_M_TWOPI*ii_f/os_f));
        ii_f += 1.0;
      }
      ////////////////////////////////////////////////////////////////

      ////////////////////// HISTORY MANAGEMENT///////////////////////
      this->history_for_interpolator = 4 + 4 + this->os + 1;
      this->set_history(this->history_for_interpolator + 1);
      this->set_output_multiple(this->os*this->L); 
      ////////////////////////////////////////////////////////////////
    }

    /*
     * Our virtual destructor.
     */
    myer_oerder_cpp_impl::~myer_oerder_cpp_impl()
    {
    }

    void
    myer_oerder_cpp_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = noutput_items + this->history_for_interpolator;
    }

    int
    myer_oerder_cpp_impl::general_work (int noutput_items,
                      gr_vector_int &ninput_items,
                      gr_vector_const_void_star &input_items,
                      gr_vector_void_star &output_items)
    {
      // auto start = high_resolution_clock::now();
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out      = (gr_complex *)       output_items[0];

      int len_frame    = this->os*this->L;
      int num_of_frame = noutput_items/len_frame;

      // Shift the start point of the input elements. Therefore, the negative indices correspond to the "memory" elements.
      in += this->history_for_interpolator - 4;



      int kk = 0;
      gr_complex* x_k = new gr_complex[len_frame];
      gr_complex  X_m = gr_complex(0,0);
      float eps_m;
      gr_complex c0, c1, c2, c3;
      for(int nn; nn < num_of_frame; nn ++)
      {
        //---------- FIND TIMING ERROR
        gr_complex Xk = gr_complex(0.0,0.0);
        for(int ii = 0; ii<len_frame; ii++ )
        {
          Xk += this->MYER_OERDER_KERNEL[ii]*(in[ii].real()*in[ii].real() + in[ii].imag()*in[ii].imag());
        }

        eps_m = -0.5/GR_M_PI*atan2(Xk.imag(),Xk.real())*this->os;  // "eps_m" is the timing error
        //---------- END OF "FIND TIMING ERROR"


        //-----------LOOP CONTROL-----------------
        if((eps_m - this->eps_old) < (this->os*0.5))
        {
          eps_m += this->os;
        }
        else if ((eps_m - this->eps_old) > (this->os*1.5))
        {
          eps_m -= this->os;
        }
        //--------------END OF LOOP CONTROL
        while (eps_m<len_frame)
        {
          int eps_m_int       = round(eps_m);
          float eps_m_frac    = eps_m - float(eps_m_int);
          int ii_opt          = eps_m_int + len_frame*nn;

          /////////////////////FRACTIONAL INTERPOLATION //////////////////////////////////
          // THIS SECTION CAN BE OPTIMIZED AS WELL !!!!
          if(eps_m_frac >= 0.0)
          {
          eps_m_frac -= 0.5;
          c0 =  gr_complex(-0.013824*in[ii_opt-3].real(),-0.013824*in[ii_opt-3].imag()) + 
                gr_complex( 0.054062*in[ii_opt-2].real(), 0.054062*in[ii_opt-2].imag()) +
                gr_complex(-0.157959*in[ii_opt-1].real(),-0.157959*in[ii_opt-1].imag()) +
                gr_complex( 0.616394*in[ii_opt+0].real(), 0.616394*in[ii_opt+0].imag()) +
                gr_complex( 0.616394*in[ii_opt+1].real(), 0.616394*in[ii_opt+1].imag()) +
                gr_complex(-0.157959*in[ii_opt+2].real(),-0.157959*in[ii_opt+2].imag()) +
                gr_complex( 0.054062*in[ii_opt+3].real(), 0.054062*in[ii_opt+3].imag()) +
                gr_complex(-0.013824*in[ii_opt+4].real(),-0.013824*in[ii_opt+4].imag());

          c1 =  gr_complex( 0.003143*in[ii_opt-3].real(), 0.003143*in[ii_opt-3].imag()) + 
                gr_complex(-0.019287*in[ii_opt-2].real(),-0.019287*in[ii_opt-2].imag()) +
                gr_complex( 0.100800*in[ii_opt-1].real(), 0.100800*in[ii_opt-1].imag()) +
                gr_complex(-1.226364*in[ii_opt+0].real(),-1.226364*in[ii_opt+0].imag()) +
                gr_complex( 1.226364*in[ii_opt+1].real(), 1.226364*in[ii_opt+1].imag()) +
                gr_complex(-0.100800*in[ii_opt+2].real(),-0.100800*in[ii_opt+2].imag()) +
                gr_complex( 0.019287*in[ii_opt+3].real(), 0.019287*in[ii_opt+3].imag()) +
                gr_complex(-0.003143*in[ii_opt+4].real(),-0.003143*in[ii_opt+4].imag());

          c2 =  gr_complex( 0.055298*in[ii_opt-3].real(), 0.055298*in[ii_opt-3].imag()) + 
                gr_complex(-0.216248*in[ii_opt-2].real(),-0.216248*in[ii_opt-2].imag()) +
                gr_complex( 0.631836*in[ii_opt-1].real(), 0.631836*in[ii_opt-1].imag()) +
                gr_complex(-0.465576*in[ii_opt+0].real(),-0.465576*in[ii_opt+0].imag()) +
                gr_complex(-0.465576*in[ii_opt+1].real(),-0.465576*in[ii_opt+1].imag()) +
                gr_complex( 0.631836*in[ii_opt+2].real(), 0.631836*in[ii_opt+2].imag()) +
                gr_complex(-0.216248*in[ii_opt+3].real(),-0.216248*in[ii_opt+3].imag()) +
                gr_complex( 0.055298*in[ii_opt+4].real(), 0.055298*in[ii_opt+4].imag());
          
          c3 =  gr_complex(-0.012573*in[ii_opt-3].real(),-0.012573*in[ii_opt-3].imag()) + 
                gr_complex( 0.077148*in[ii_opt-2].real(), 0.077148*in[ii_opt-2].imag()) +
                gr_complex(-0.403198*in[ii_opt-1].real(),-0.403198*in[ii_opt-1].imag()) +
                gr_complex( 0.905457*in[ii_opt+0].real(), 0.905457*in[ii_opt+0].imag()) +
                gr_complex(-0.905457*in[ii_opt+1].real(),-0.905457*in[ii_opt+1].imag()) +
                gr_complex( 0.403198*in[ii_opt+2].real(), 0.403198*in[ii_opt+2].imag()) +
                gr_complex(-0.077148*in[ii_opt+3].real(),-0.077148*in[ii_opt+3].imag()) +
                gr_complex( 0.012573*in[ii_opt+4].real(), 0.012573*in[ii_opt+4].imag());


          out[kk] = c0 + eps_m_frac*c1 + eps_m_frac*eps_m_frac*c2 + eps_m_frac*eps_m_frac*eps_m_frac*c3;
          }
          else
          {
          eps_m_frac += 0.5;

          c0 =  gr_complex(-0.013824*in[ii_opt-4].real(),-0.013824*in[ii_opt-4].imag()) + 
                gr_complex( 0.054062*in[ii_opt-3].real(), 0.054062*in[ii_opt-3].imag()) +
                gr_complex(-0.157959*in[ii_opt-2].real(),-0.157959*in[ii_opt-2].imag()) +
                gr_complex( 0.616394*in[ii_opt-1].real(), 0.616394*in[ii_opt-1].imag()) +
                gr_complex( 0.616394*in[ii_opt+0].real(), 0.616394*in[ii_opt+0].imag()) +
                gr_complex(-0.157959*in[ii_opt+1].real(),-0.157959*in[ii_opt+1].imag()) +
                gr_complex( 0.054062*in[ii_opt+2].real(), 0.054062*in[ii_opt+2].imag()) +
                gr_complex(-0.013824*in[ii_opt+3].real(),-0.013824*in[ii_opt+3].imag());

          c1 =  gr_complex( 0.003143*in[ii_opt-4].real(), 0.003143*in[ii_opt-4].imag()) + 
                gr_complex(-0.019287*in[ii_opt-3].real(),-0.019287*in[ii_opt-3].imag()) +
                gr_complex( 0.100800*in[ii_opt-2].real(), 0.100800*in[ii_opt-2].imag()) +
                gr_complex(-1.226364*in[ii_opt-1].real(),-1.226364*in[ii_opt-1].imag()) +
                gr_complex( 1.226364*in[ii_opt+0].real(), 1.226364*in[ii_opt+0].imag()) +
                gr_complex(-0.100800*in[ii_opt+1].real(),-0.100800*in[ii_opt+1].imag()) +
                gr_complex( 0.019287*in[ii_opt+2].real(), 0.019287*in[ii_opt+2].imag()) +
                gr_complex(-0.003143*in[ii_opt+3].real(),-0.003143*in[ii_opt+3].imag());

          c2 =  gr_complex( 0.055298*in[ii_opt-4].real(), 0.055298*in[ii_opt-4].imag()) + 
                gr_complex(-0.216248*in[ii_opt-3].real(),-0.216248*in[ii_opt-3].imag()) +
                gr_complex( 0.631836*in[ii_opt-2].real(), 0.631836*in[ii_opt-2].imag()) +
                gr_complex(-0.465576*in[ii_opt-1].real(),-0.465576*in[ii_opt-1].imag()) +
                gr_complex(-0.465576*in[ii_opt+0].real(),-0.465576*in[ii_opt+0].imag()) +
                gr_complex( 0.631836*in[ii_opt+1].real(), 0.631836*in[ii_opt+1].imag()) +
                gr_complex(-0.216248*in[ii_opt+2].real(),-0.216248*in[ii_opt+2].imag()) +
                gr_complex( 0.055298*in[ii_opt+3].real(), 0.055298*in[ii_opt+3].imag());
          
          c3 =  gr_complex(-0.012573*in[ii_opt-4].real(),-0.012573*in[ii_opt-4].imag()) + 
                gr_complex( 0.077148*in[ii_opt-3].real(), 0.077148*in[ii_opt-3].imag()) +
                gr_complex(-0.403198*in[ii_opt-2].real(),-0.403198*in[ii_opt-2].imag()) +
                gr_complex( 0.905457*in[ii_opt-1].real(), 0.905457*in[ii_opt-1].imag()) +
                gr_complex(-0.905457*in[ii_opt+0].real(),-0.905457*in[ii_opt+0].imag()) +
                gr_complex( 0.403198*in[ii_opt+1].real(), 0.403198*in[ii_opt+1].imag()) +
                gr_complex(-0.077148*in[ii_opt+2].real(),-0.077148*in[ii_opt+2].imag()) +
                gr_complex( 0.012573*in[ii_opt+3].real(), 0.012573*in[ii_opt+3].imag());

          out[kk] = c0 + eps_m_frac*c1 + eps_m_frac*eps_m_frac*c2 + eps_m_frac*eps_m_frac*eps_m_frac*c3;
          }
          ///////////////////// END OF FRACTIONAL INTERPOLATION //////////////////////////////////
          
          // increase the num of items produced by 1.
          kk += 1;

          // increase the "timing error" as "os" for the next symbol
          eps_m += this->os;
        }

        // reinit the "timing error" for the next chunk
        this->eps_old = eps_m;
        this->eps_old -= this->os*(this->L + 1);
      }

      // Tell how many input elements are consumed during the processing. 
      consume_each(num_of_frame*len_frame);

      // Tell runtime system how many output items we produced.
      // auto end      = high_resolution_clock::now();
      // auto duration = duration_cast<microseconds>(end - start);
      // float duration_mu_s = (float)duration.count();
      // std::cout << "Elapsed time in microseconds: "<< ((float) num_of_frame*len_frame)/duration_mu_s*1e6/8 << " µs" << std::endl;
      return kk;
    }

  } /* namespace THALV1 */
} /* namespace gr */

