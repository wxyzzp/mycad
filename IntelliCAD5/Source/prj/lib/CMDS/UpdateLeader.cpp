// Implementation of the CUpdateLeader class

#include "cmds.h"/*DNT*/
// EBATECH(watanabe)-[update leader according to modifying annotation
#include "UpdateLeader.h"/*DNT*/
#include "dimstyletabrec.h"/*DNT*/
#include "dbmtext.h"

CUpdateLeader*
CUpdateLeaderFactory::CreateUpdateLeader(db_leader* pLeader)
{ 
  CUpdateLeader* p = new CUpdateLeader(pLeader);
  return p;
}

CUpdateLeader::CUpdateLeader(db_leader* pLeader)
{
  m_leader = pLeader;
}

bool CUpdateLeader::Redraw(
  db_handitem* p // = NULL
)
{
  if (p == NULL)
    {
      p = m_leader;
    }

  sds_name name;
  name[0] = (long)p;
  name[1] = (long)SDS_CURDWG;
  sds_redraw(name, IC_REDRAW_DRAW);
  return true;
}

bool CUpdateLeader::ModifyLeader(db_objhandle* pObjHandle)
{
  if (m_leader == NULL)
    {
        return false;
    }

  db_handitem* hip = SDS_CURDWG->handent(*pObjHandle);
  if (hip)
    {
      // hide leader
      sds_name name;
      name[0] = (long)hip;
      name[1] = (long)SDS_CURDWG;
      sds_redraw(name, IC_REDRAW_UNDRAW);

      sds_point ptAnno;
      hip->get_10(ptAnno);

      int cnt;
      m_leader->get_76(&cnt);

      sds_point* parray = new sds_point[cnt];
      m_leader->get_10(parray, cnt);

      sds_point ptLast;
      int type = hip->ret_type();
      if (type == DB_MTEXT)
        {
          CDbMText* mtext = (db_mtext*)hip;

          sds_point p0, p1;
#ifdef _USE_DISP_OBJ_PTS_3D_
//#pragma message("_USE_DISP_OBJ_PTS_3D_ defined in UpdateLeader.cpp")
		  CD3 minPoint, maxPoint;
		  CDoBase::getPlanarBoundingBox(SDS_CURGRVW, mtext, minPoint, maxPoint);
		  memcpy(p0, (sds_real*)minPoint, 3 * sizeof(sds_real));
		  memcpy(p1, (sds_real*)maxPoint, 3 * sizeof(sds_real));
#else
          mtext->get_extent(p0, p1);
#endif
          sds_real height = fabs(p0[1] - p1[1]);
          sds_real width  = fabs(p0[0] - p1[0]);

          int attach;
          mtext->get_71(&attach);

          sds_point ptLeft, ptRight;
          switch (attach)
            {
              case 1:   // left   top
              case 4:   // left   middle
              case 7:   // left   bottom
                  ptLeft [0] = 0;
                  ptRight[0] = width;
                  break;
              case 2:   // center top
              case 5:   // center middle
              case 8:   // center bottom
                  ptLeft [0] = -width / 2;
                  ptRight[0] = width / 2;
                  break;
              case 3:   // right  top
              case 6:   // right  middle
              case 9:   // right  bottom
                  ptLeft [0] = -width;
                  ptRight[0] = 0;
                  break;
            }

          switch (attach)
            {
              case 1:   // left   top
              case 2:   // center top
              case 3:   // right  top
                  ptLeft[1] = -height / 2;
                  break;
              case 4:   // left   middle
              case 5:   // center middle
              case 6:   // right  middle
                  ptLeft[1] = 0;
                  break;
              case 7:   // left   bottom
              case 8:   // center bottom
              case 9:   // right  bottom
                  ptLeft[1] = height / 2;
                  break;
            }
          ptRight[1] = ptLeft[1];
          ptRight[2] = ptLeft[2] = 0;

          // hook dir
          sds_point dir;
          m_leader->get_211(dir);

          ptLeft[0]  += ptAnno[0];
          ptLeft[1]  += ptAnno[1];
          ptLeft[2]  += ptAnno[2];
          ptRight[0] += ptAnno[0];
          ptRight[1] += ptAnno[1];
          ptRight[2] += ptAnno[2];

          // get DIMGAP from leader
          sds_real dimgap;
          int      dimtad;
		  //Bugzilla No. 78069 ; 01-04-2002 
          //GetDimgap(dimgap, dimtad);
		  sds_real dimscale;
		  m_leader->GetDimvars(dimgap,dimscale,dimtad); 	
          dimgap = fabs(dimgap);

          double dLeft  = sds_distance(ptLeft,  parray[cnt - 2]);
          double dRight = sds_distance(ptRight, parray[cnt - 2]);

          sds_point ptMtext;
          if (dLeft < dRight)
            {
              sds_point_set(ptLeft, ptMtext);
              attach = 4;   // left   middle
            }
          else
            {
              sds_point_set(ptRight, ptMtext);
              attach = 6;   // right  middle
              dimgap = -dimgap;
            }

          mtext->set_10(ptMtext);
          mtext->set_71(attach);
          Redraw(mtext);

          // set leader last point
          sds_point_set(ptMtext, ptLast);
		  //Bugzilla No. 78069 ; 01-04-2002 [
          //ptLast[0] -= dir[0] * dimgap;
          //ptLast[1] -= dir[1] * dimgap;
          //ptLast[2] -= dir[2] * dimgap;
		  ptLast[0] -= dir[0] * dimgap * dimscale;
          ptLast[1] -= dir[1] * dimgap * dimscale;
          ptLast[2] -= dir[2] * dimgap * dimscale;
		  //Bugzilla No. 78069 ; 01-04-2002 ]

          if (dimtad != 0)
            {
              ptLast[1] -= height / 2;
			  //Bugzilla No. 78069 ; 01-04-2002 
              //ptLast[1] -= fabs(dimgap);
			  ptLast[1] -= fabs(dimgap) * dimscale;
            }
        }
      else if (type == DB_TOLERANCE)
        {
          sds_point_set(ptAnno, ptLast);
        }
      else if (type == DB_INSERT)
        {
          sds_point ptOffset;
		  //Bugzilla No. 78044 ; 25-03-2002 
          //m_leader->get_offsettoblockinspt(ptOffset);
		  m_leader->get_212(ptOffset);

          ptLast[0] = ptAnno[0] + ptOffset[0];
          ptLast[1] = ptAnno[1] + ptOffset[1];
          ptLast[2] = ptAnno[2] + ptOffset[2];
        }
      else
        {
          return false;
        }

	  
	  //Bugzilla No. 78013 ; 01-04-2002 [
	  sds_point txtoffset;
	  m_leader->get_213(txtoffset);

      ptLast[0] += txtoffset[0];
      ptLast[1] += txtoffset[1];
      ptLast[2] += txtoffset[2];

	  //Bugzilla No. 78013 ; 01-04-2002 ]

      parray[cnt - 1][0] = ptLast[0];
      parray[cnt - 1][1] = ptLast[1];
      parray[cnt - 1][2] = ptLast[2];

      m_leader->set_10(parray, cnt);
      delete [] parray;
    }
  return true;
}

//Bugzilla No. 78013 ; 01-04-2002
//moved and modified "GetDimgap(...)" to DB/leader.cpp
//functrion GetDimgap(...) now is GetDimvars(...) in file DB/leader.cpp class db_leader
