%include "std_string.i"
%include "cpointer.i"
%include "stdint.i"
%include "typemaps.i"
%include "upm_exception.i"

/* Import additional SWIG helps (not exposed in wrapper) */
%import _upm.i

%{
#include "version.hpp"
%}
%include "version.hpp"

%apply int { speed_t };
%apply int { mraa_result_t };
%apply int { mraa::Result };
%apply int { upm_result_t };

#if (SWIG_JAVASCRIPT_V8)
%{
/* Because there's no guarantee that v8 will ever call garbage collection,
 * we're adding a function that will allow a user to call it manually
 */
void cleanUp()
{
    /* Call the v8 garbage collector as long as there is memory to clean up
     * See https://codereview.chromium.org/412163003 for this API change
     */
#if (V8_MAJOR_VERSION-0) < 4 && (SWIG_V8_VERSION < 0x032838)
    while (!v8::V8::IdleNotificationDeadline())
#else
    while (!v8::Isolate::GetCurrent()->IdleNotificationDeadline(1000))
#endif
    {;}
}
%}
void cleanUp();
#endif

#if (SWIGJAVA)
%typemap(jtype) jobject runnable "java.lang.Runnable"
%typemap(jstype) jobject runnable "java.lang.Runnable"
#endif

// Disable nested struct warnings
#pragma SWIG nowarn=312,325

#if SWIGPYTHON
/* Apply all uints as 'unsigned int' for python */
%apply unsigned int {uint};
#endif

/* The pyupm_doxy2swig.i file is created via doxy2swig.py from doxygen xml
 * output during doc build.  This file is used by swig to provide native
 * python module documentation.
 */
#if defined(SWIGPYTHON) && defined(BUILDDOC)
%include "pyupm_doxy2swig.i"
#endif
