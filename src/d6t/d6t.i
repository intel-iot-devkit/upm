#ifdef SWIGPYTHON
%module (package="upm") d6t 
#endif

%include "../common_top.i"

/* BEGIN Java syntax  ------------------------------------------------------- */
#ifdef SWIGJAVA
JAVA_JNI_LOADLIBRARY(javaupm_d6t)
#endif
/* END Java syntax */

/* BEGIN Common SWIG syntax ------------------------------------------------- */
%{
#include "d6t.hpp"
%}

%include "d6t.hpp"
/* END Common SWIG syntax */
