/* include */
#include "kprint.h" /* terminal */

/* entry point function -- called by kernel loader */
int kmain() {

	/* infinitely print junk characters */
	int frame = 100000;

	while (1) {
	
		if (frame == 0) {
		
			/* print char and reset frame value */
			kprintc('a');
			frame = 100000;
		}

		/* decrease value */
		--frame;
	}
	
	return 0;
}
