/*
 * use.c
 *
 *  Created on: May 9, 2014
 *      Author: alex
 */



#include "use.h"


int user_utilization(){

		   	   	  Display *dpy;
		          int dummy;
		          CARD16 standby, suspend, off;
		          BOOL onoff;
		          CARD16 state;

		          char *disp = NULL;

		          dpy = XOpenDisplay(disp);  /*  Open display and check for success */
		          if (dpy == NULL) {
		             fprintf(stderr, "unable to open display \"%s\"\n", XDisplayName (disp));
		             exit(EXIT_FAILURE);
		          }


//		          printf("DPMS (Energy Star):\n");
		          if (DPMSQueryExtension(dpy, &dummy, &dummy)) {
		             if (DPMSCapable(dpy)) {
		             DPMSGetTimeouts(dpy, &standby, &suspend, &off);
	//	             printf ("  Standby: %d    Suspend: %d    Off: %d\n",standby, suspend, off);
		             DPMSInfo(dpy, &state, &onoff);
		             if (onoff) {
		//                 printf("  DPMS is Enabled\n");
		                 switch (state) {
		                    case XCB_DPMS_DPMS_MODE_ON:
		                         //printf("  Monitor is On\n");
		                    	 XCloseDisplay (dpy);
		                    	 return 1;
		                         break;
		                    case XCB_DPMS_DPMS_MODE_STANDBY:
		                         //printf("  Monitor is in Standby\n");
		                         XCloseDisplay (dpy);
		                         return 0;
		                         break;
		                    case XCB_DPMS_DPMS_MODE_SUSPEND:
		                         //printf("  Monitor is in Suspend\n");
		                         XCloseDisplay (dpy);
		                         return 0;
		                         break;
		                    case XCB_DPMS_DPMS_MODE_OFF:
		                         //printf("  Monitor is Off\n");
		                         XCloseDisplay (dpy);
		                         return 0;
		                         break;
		                    default:
		                         printf("  Unrecognized response from server\n");
		                 }
		             }
		             else
		                 printf("  DPMS is Disabled\n");
		              }
		             else
		                 printf ("  Display is not capable of DPMS\n");
		          }
		         else {
		             printf ("  Server does not have the DPMS Extension\n");
		          }

		          XCloseDisplay (dpy);

		          return -1;
}
