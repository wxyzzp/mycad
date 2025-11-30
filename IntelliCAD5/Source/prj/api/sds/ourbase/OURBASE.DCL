

//  OURBASE.DCL                                                      */
//                                                                   
// .............................................................................
//
// Copyright (c) 1998 by Visio Corporation.  All rights reserved.
//
// The Software is subject to the license agreement that accompanies 
// or is included with the Software, which specifies the permitted 
// and prohibited uses of the Software. Any unauthorized duplication 
// or use of Visio Corporation Software, in whole or in part, in print, 
// or in any other storage and retrieval system is prohibited.
// 
// To the maximum extent permitted by applicable law, Visio Corporation
// and its suppliers disclaim any and all warranties and conditions,
// either express or implied, including, without limitation, implied
// warranties of merchantability, fitness for a particular purpose,
// title, and non-infringement, and those arising out of usage of trade
// or course of dealing, concerning these materials.  These materials
// are provided "as is" without warranty of any kind.
// .............................................................................



base_about : dialog {
    label = "About BASE";
    /*
    : paragraph {
        alignment = left;
        fixed_height = true;
        : text {
            label = "Copyright (c) 1997, Visio Corporation";
        }
        : text {
            label = "All Rights Reserved.";
        }
        : concatenation {
            : text_part {
                label = "Version ";
            }
            : text_part {
                label = "";
                key = "version";
                width = 50;
            }
        }
    }
    : radio_row {
	key="row1";
        : radio_button {
	     label="ALP01";
	     key="alpo1";
        }
        : radio_button {
	     key="alpo2";
	     label="ALP02";
        }
        : radio_button {
	     key="alpo3";
	     label="ALP03";
        }
        : radio_button {
	     label="ALP04 ";
	     key="alpo4";
        }
    }
    spacer_1;
    */
    ok_cancel;
}
