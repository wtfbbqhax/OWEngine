#include <math.h>
void Sys_SnapVector( float* v )   // bk001213 - see win32/win_shared.c
{
    // bk001213 - old linux
    v[0] = rint( v[0] );
    v[1] = rint( v[1] );
    v[2] = rint( v[2] );
}
