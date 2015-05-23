/* -*- c++ -*- */

#define HUBSAN_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "hubsan_swig_doc.i"

%{
#include "hubsan/build_packet_b.h"
%}


%include "hubsan/build_packet_b.h"
GR_SWIG_BLOCK_MAGIC2(hubsan, build_packet_b);

