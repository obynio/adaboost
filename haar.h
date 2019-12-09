#include "SDLToMatrix.h"

#ifndef _HAAR_H
#define _HAAR_H

/*types: (#=positive, _=negative
1:	_#
	_#

2:	##
	__

3: _#_

4:	#_
	_#

*/

//output haar caracteristic, the positions are in a 24x24 grid
//type: from 1 to 4, the different patterns

float haar(Matrix* m, int type, int xmin, int ymin, int xmax,int ymax);

//same result as haar but in [-1;1], relative to the surface size
float relativeHaar(Matrix* m, int type, int xmin, int ymin, int xmax, int ymax);

#endif
