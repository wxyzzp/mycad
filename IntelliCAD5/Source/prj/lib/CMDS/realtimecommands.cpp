/* REALTIMECOMMANDS.CPP
 * Copyright (C) 1997-1998 Visio Corporation. All rights reserved.
 *
 * Abstract
 *
 *
 */
// THIS FILE HAS BEEN GLOBALIZED!

#include "cmds.h"
#include "IcadView.h"
//4M Bugzilla 77987 19/02/02->
#include "preferences.h"
//<-4M 19/02/02
short
cmd_rtzoom(void)
{
//4M Bugzilla 77987 19/02/02->
/*
	SDS_CURVIEW->m_RTMotion = ZOOM;
*/
   int PrvCur = SDS_SetCursor(IDC_ZOOM);
   SDS_CURVIEW->SetRTMotion(ZOOM);
   sds_printf("\nENTER, Right click or Esc to complete...\n");
   sds_point resultp;
   while(SDS_AskForPoint(NULL,"",resultp)==RTNORM);
   SDS_CURVIEW->SetRTMotion(NO);
   SDS_CURVIEW->DeleteRealTime();
   SDS_SetCursor(PrvCur);
//<-4M 19/02/02
    return RTNORM;
}

short
cmd_rtpan(void)
{
//4M Bugzilla 77987 19/02/02->
/*
	SDS_CURVIEW->m_RTMotion = PAN;
*/
   int PrvCur = SDS_SetCursor(IDC_ICAD_GRABHAND);
   SDS_CURVIEW->SetRTMotion(PAN);
   sds_printf("\nENTER, Right click or Esc to complete...\n");
   sds_point resultp;
   while(SDS_AskForPoint(NULL,"",resultp)==RTNORM);
   SDS_CURVIEW->SetRTMotion(NO);
   SDS_CURVIEW->DeleteRealTime();
   SDS_SetCursor(PrvCur);
//<-4M 19/02/02
    return RTNORM;
}

short
cmd_rtrotx(void)
{
//4M Bugzilla 77987 19/02/02->
/*
	SDS_CURVIEW->m_RTMotion = ROT_CYL_X;
*/
   int PrvCur = SDS_SetCursor(IDC_RT_CYLINDER_X);
   SDS_CURVIEW->SetRTMotion(ROT_CYL_X);
   if( bool(UserPreference::ContinuousMotionPreference))
      sds_printf(ResourceString(IDC_REALTIMECOMMANDS_RIGHTCLICK,"\n>>Right click to complete...\n"));
   else
      sds_printf(ResourceString(IDC_REALTIMECOMMANDS_ENTER_ESC_OR_RIGHTCLICK,"\n>>ENTER, Right click or Esc to complete...\n"));
   sds_point resultp;
   while(SDS_AskForPoint(NULL,"",resultp)==RTNORM);
   SDS_CURVIEW->SetRTMotion(NO);
   SDS_CURVIEW->DeleteRealTime();
   SDS_SetCursor(PrvCur);
//<-4M 19/02/02
    return RTNORM;
}

short
cmd_rtroty(void)
{
//4M Bugzilla 77987 19/02/02->
/*
	SDS_CURVIEW->m_RTMotion = ROT_CYL_Y;
*/
   int PrvCur = SDS_SetCursor(IDC_RT_CYLINDER_Y);
   SDS_CURVIEW->SetRTMotion(ROT_CYL_Y);
   if( bool(UserPreference::ContinuousMotionPreference))
      sds_printf(ResourceString(IDC_REALTIMECOMMANDS_RIGHTCLICK,"\n>>Right click to complete...\n"));
   else
      sds_printf(ResourceString(IDC_REALTIMECOMMANDS_ENTER_ESC_OR_RIGHTCLICK,"\n>>ENTER, Right click or Esc to complete...\n"));
   sds_point resultp;
   while(SDS_AskForPoint(NULL,"",resultp)==RTNORM);
   SDS_CURVIEW->SetRTMotion(NO);
   SDS_CURVIEW->DeleteRealTime();
   SDS_SetCursor(PrvCur);
//<-4M 19/02/02
    return RTNORM;
}

short
cmd_rtrotz(void)
{
//4M Bugzilla 77987 19/02/02->
/*
	SDS_CURVIEW->m_RTMotion = ROT_CYL_Z;
*/
   int PrvCur = SDS_SetCursor(IDC_RT_CYLINDER_Z);
   SDS_CURVIEW->SetRTMotion(ROT_CYL_Z);
   if( bool(UserPreference::ContinuousMotionPreference))
      sds_printf(ResourceString(IDC_REALTIMECOMMANDS_RIGHTCLICK,"\n>>Right click to complete...\n"));
   else
      sds_printf(ResourceString(IDC_REALTIMECOMMANDS_ENTER_ESC_OR_RIGHTCLICK,"\n>>ENTER, Right click or Esc to complete...\n"));
   sds_point resultp;
   while(SDS_AskForPoint(NULL,"",resultp)==RTNORM);
   SDS_CURVIEW->SetRTMotion(NO);
   SDS_CURVIEW->DeleteRealTime();
   SDS_SetCursor(PrvCur);
	return RTNORM;
//<-4M 19/02/02
}

short
cmd_rtrot(void)
{
//4M Bugzilla 77987 19/02/02->
/*
	SDS_CURVIEW->m_RTMotion = ROT_SPH;
*/
   int PrvCur = SDS_SetCursor(IDC_RT_SPHERE);
   SDS_CURVIEW->SetRTMotion(ROT_SPH);
   if( bool(UserPreference::ContinuousMotionPreference))
      sds_printf(ResourceString(IDC_REALTIMECOMMANDS_RIGHTCLICK,"\n>>Right click to complete...\n"));
   else
      sds_printf(ResourceString(IDC_REALTIMECOMMANDS_ENTER_ESC_OR_RIGHTCLICK,"\n>>ENTER, Right click or Esc to complete...\n"));
   sds_point resultp;
   while(SDS_AskForPoint(NULL,"",resultp)==RTNORM);
   SDS_CURVIEW->SetRTMotion(NO);
   SDS_CURVIEW->DeleteRealTime();
   SDS_SetCursor(PrvCur);
//<-4M 19/02/02
    return RTNORM;
}
