%module jsupm_bacnetmstp
%include "../upm.i"
%include "stdint.i"
%include "cpointer.i"

%pointer_functions(float, floatp);

%include "bacnetmstp.h"
%{
    #include "bacnetmstp.h"
%}
