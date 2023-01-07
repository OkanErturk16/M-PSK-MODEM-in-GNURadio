/* -*- c++ -*- */

#define THALV1_API

%include "gnuradio.i"           // the common stuff

//load generated python docstrings
%include "THALV1_swig_doc.i"

%{
#include "THALV1/Tagged_stream_receiver.h"
#include "THALV1/correlate_and_tag.h"
#include "THALV1/tagged_stream_trimmer.h"
#include "THALV1/myer_oerder_cpp.h"
%}

%include "THALV1/Tagged_stream_receiver.h"
GR_SWIG_BLOCK_MAGIC2(THALV1, Tagged_stream_receiver);

%include "THALV1/correlate_and_tag.h"
GR_SWIG_BLOCK_MAGIC2(THALV1, correlate_and_tag);
%include "THALV1/tagged_stream_trimmer.h"
GR_SWIG_BLOCK_MAGIC2(THALV1, tagged_stream_trimmer);

%include "THALV1/myer_oerder_cpp.h"
GR_SWIG_BLOCK_MAGIC2(THALV1, myer_oerder_cpp);
