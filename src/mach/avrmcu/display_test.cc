// EPOS-- AVRMCU_Display Test Program

// This work is licensed under the Creative Commons 
// Attribution-NonCommercial-NoDerivs License. To view a copy of this license, 
// visit http://creativecommons.org/licenses/by-nc-nd/2.0/ or send a letter to 
// Creative Commons, 559 Nathan Abbott Way, Stanford, California 94305, USA.


#include <utility/ostream.h>
#include <display.h>

__USING_SYS

int main()
{
    OStream cout;

    cout << "AT90S_Display test\n";

    AVRMCU_Display display;

    return 0;
}
