
#ifndef INCLUDED_THALV1_MYER_OERDER_CPP_IMPL_H
#define INCLUDED_THALV1_MYER_OERDER_CPP_IMPL_H

#include <THALV1/myer_oerder_cpp.h>

namespace gr {
  namespace THALV1 {

    class myer_oerder_cpp_impl : public myer_oerder_cpp
    {
      private:
      
      int L;
      int os;
      float eps_old;
      gr_complex* MYER_OERDER_KERNEL;
      int history_for_interpolator;



      float filtercoeffs0[8] = {-0.013824, 0.054062, -0.157959, 0.616394,  0.616394, -0.157959,  0.054062, -0.013824};
      float filtercoeffs1[8] = { 0.003143,-0.019287,  0.100800,-1.226364,  1.226364, -0.100800,  0.019287, -0.003143};
      float filtercoeffs2[8] = { 0.055298,-0.216248,  0.631836,-0.465576, -0.465576,  0.631836, -0.216248,  0.055298};
      float filtercoeffs3[8] = {-0.012573, 0.077148, -0.403198, 0.905457, -0.905457,  0.403198, -0.077148,  0.012573};

      public:
      myer_oerder_cpp_impl(int os, int n_symbol);
      ~myer_oerder_cpp_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
                      gr_vector_int &ninput_items,
                      gr_vector_const_void_star &input_items,
                      gr_vector_void_star &output_items);

    };

  } // namespace THALV1
} // namespace gr

#endif /* INCLUDED_THALV1_MYER_OERDER_CPP_IMPL_H */

