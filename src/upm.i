%include "std_string.i"
%include "stdint.i"
%include "upm_exception.i"

%typemap(out) mraa_result_t = int;

#if (SWIG_JAVASCRIPT_V8)
%{
	// Because there's no guarantee that v8 will ever call garbage collection,
	// we're adding a function that will allow a user to call it manually
	void cleanUp()
	{
		// Call the v8 garbage collector as long as there is memory to clean up
		// See https://codereview.chromium.org/412163003 for this API change
#if (SWIG_V8_VERSION < 0x032838)
		while (!v8::V8::IdleNotification())
#else
		while (!v8::Isolate::GetCurrent()->IdleNotification(1000))
#endif
		{;}
	}
%}
void cleanUp();
#endif
