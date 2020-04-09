#pragma once

//all the window related exception throw macros are here
//will be included in to multiple files to avoid dulication.

#define CUWND_EXCEPT( hr ) Window::HrException( __LINE__,__FILE__,(hr) )
#define CUWND_LAST_EXCEPT() Window::HrException( __LINE__,__FILE__,GetLastError() )
#define CUWND_NOGFX_EXCEPT() Window::NoGfxException( __LINE__,__FILE__ )