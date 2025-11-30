/* PRJ\ICADLIB\CheckFP.H
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 * 
 * Abstract
 * 
 * Check the floating point control word and reset if necessary.
 * The application assumes floating point errors will be masked and
 * NAN values returned.	Some drivers will reset this control, though, 
 * causing exceptions in the presence of those drivers.
 *
 * This function can be called in the message loop or wherever necessary
 * to preserve the desired status.
 * 
 */ 

void CheckFPControlWord();

