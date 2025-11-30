//4M Item:25 Item:28->
template<>
AFX_INLINE UINT AFXAPI HashKey(CSdsName& key)
{   
   return HashKey((long)key.ename[0]);// + HashKey((long)key.ename[1]);
/*   
   char s1[9];
   strcpy(s1,(char *)key.ename[0]);
   s1[4]='\0';
   char s2[5];
   strcpy(s2,(char *)key.ename[1]);
   s2[4]='\0';
   strcat(s1,s2);
   char *s=s1;
   return HashKey((LPCSTR)s);*/

#if 0
   long l1=key.ename[0];
   long l2=key.ename[1];
   BYTE s1[8];
   s1[3]=(unsigned char)l1;
   s1[2]=(unsigned char)l1>>8;
   s1[1]=(unsigned char)l1>>8;
   s1[0]=(unsigned char)l1>>8;
   s1[7]=(unsigned char)l2;
   s1[6]=(unsigned char)l2>>8;
   s1[5]=(unsigned char)l2>>8;
   s1[4]=(unsigned char)l2>>8;
   BYTE *s=s1;
   size_t len=8;
   unsigned long hsize=65497;
   register unsigned long h = 0;

    /*
     * This is INCREDIBLY ugly, but fast.  We break the string up into
     * 8 byte units.  On the first time through the loop we get the
     * "leftover bytes" (strlen % 8).  On every other iteration, we
     * perform 8 HASHC's so we handle all 8 bytes.  Essentially, this
     * saves us 7 cmp & branch instructions.  If this routine is
     * heavily used enough, it's worth the ugly coding.
     *
     * OZ's original sdbm hash, copied from Margo Seltzers db package.
     */

    /*
     * Even more speed:
     * #define HASHC   h = *s++ + 65599 * h
     * Because 65599 = pow(2, 6) + pow(2, 16) - 1 we multiply by shifts
     */
#define HASHC   htmp = (h << 6);  \
        h = *s++ + htmp + (htmp << 10) - h

    unsigned long htmp;

    h = 0;

    /*
     * This was an implementation of "Duff's Device".
     */
    switch (len & (8 - 1)) {
    case 7:     HASHC;
    case 6:     HASHC;
    case 5:     HASHC;
    case 4:     HASHC;
    case 3:     HASHC;
    case 2:     HASHC;
    case 1:     HASHC;
    default:    break;
    }

    if (len > (8 - 1)) {
        register size_t loop = len >> 3;
        do {
            HASHC;
            HASHC;
            HASHC;
            HASHC;
            HASHC;
            HASHC;
            HASHC;
            HASHC;
        } while (--loop);
    }

    if (h >= hsize)
        h %= hsize;
    return h;
#endif
}

MNodeData::MNodeData()
{
   points=NULL;
   next=NULL;
}

MNodeData::MNodeData(int NoPointsPerBlock)
{
   points=new sds_point[NoPointsPerBlock];
   next=NULL;
}

MNodeData::~MNodeData()
{
   if (points){
      delete [] points;
      points=NULL;
   }
}

MNodeData::MNodeData(MNodeData const &MD)
{
   operator=(MD);
}

MNodeData& MNodeData::operator=(MNodeData const &MD)
{
   if (points){
      delete [] points;
      points=NULL;
   }
   int NoPoints=sizeof(MD.points)/sizeof(sds_point);
   if (NoPoints>0){
      points=new sds_point[NoPoints];
      for (int i=0;i<NoPoints;i++){
         ic_ptcpy(points[i],MD.points[i]);
      }
   }
   next=NULL;//SHOULD BE ARRANGED BY A PARENT CALL FUNCTION
   return *this;
}

int MNodeData::Add(int index, sds_point cen)
{
   if (points){
      ic_ptcpy(points[index-1],cen);
      return RTNORM;
   }
   return RTERROR;
}

int MNodeData::Get(int index,sds_point p)const
{
   if (points){
      ic_ptcpy(p,points[index-1]);
      return RTNORM;
   }
   return RTERROR;
}

/*************************************************************/

MEntNodesArray::MEntNodesArray()
{
   NoPointsPerBlock=0;
   NoPoints=0;
   NoBlocks=0;
   pNodes=NULL;
   pLastNode=NULL;
}

MEntNodesArray::~MEntNodesArray()
{
   RemoveAll();
}

MEntNodesArray::MEntNodesArray(MEntNodesArray const & n)
{
   operator=(n);
}

MEntNodesArray &MEntNodesArray::operator=(MEntNodesArray const &n)
{
   RemoveAll();
   SetNoPointsPerBlock(n.NoPointsPerBlock);
   sds_point pt;
   for (int i=1;i<=n.GetCount();i++){
      n.Get(i,pt);
      Add(pt);
   }
   return *this;
}

void MEntNodesArray::RemoveAll()
{
   MNodeData * pNode=pNodes;
   MNodeData * pNodeToDelete=NULL;
   while (pNode){
      pNodeToDelete=pNode;
      pNode=pNode->next;
      delete pNodeToDelete;
   }
   NoBlocks=0;
   NoPoints=0;
   pNodes=NULL;
   pLastNode=NULL;
   NoPointsPerBlock=0;
}

void MEntNodesArray::SetNoPointsPerBlock(int noPointsPerBlock)
{
   if (!pNodes){
      NoPointsPerBlock=noPointsPerBlock;
      pNodes=new MNodeData(NoPointsPerBlock);
      pLastNode=pNodes;
      NoBlocks++;
   }
}

int MEntNodesArray::Add(sds_point cen)
{
   if (NoBlocks==0)
      return RTERROR;
   int Residue=(NoBlocks*NoPointsPerBlock)-NoPoints;
   if (Residue ==0){
      MNodeData * pNewNode=new MNodeData(NoPointsPerBlock);
      pLastNode->next=pNewNode;
      pLastNode=pNewNode;
      NoBlocks++;
   }
   int NewIndex=NoPoints+1-((NoBlocks-1)*NoPointsPerBlock);
   pLastNode->Add(NewIndex,cen);
   NoPoints++;
   return RTNORM;
}

int MEntNodesArray::Get(int index,sds_point p)const
{
   if (index>NoPoints)
      return RTERROR;
   int NodeIndex=(index-1) / NoPointsPerBlock;
   MNodeData *pNode=pNodes;
   for (int i=1;i<=NodeIndex;i++)
      pNode=pNode->next;

   int newIndex=index-(NodeIndex*NoPointsPerBlock);
   pNode->Get(newIndex,p);
   return RTNORM;
}


NodePoints::NodePoints()
{
   Points.SetNoPointsPerBlock(10);
//   NoPoints=0;
}

NodePoints::NodePoints(sds_point pt)
{
   Points.SetNoPointsPerBlock(10);
//   NoPoints=0;
   Add(pt);
}

NodePoints::NodePoints(const NodePoints &n)
{
   operator=(n);
}

NodePoints::~NodePoints()
{
   Points.RemoveAll();
//   NoPoints=0;
}

void NodePoints::Add(sds_point pt)
{
   Points.Add(pt);
/*   if (NoPoints<10){
      ic_ptcpy(Points[NoPoints],pt);
      NoPoints++;
   }*/
}

NodePoints& NodePoints::operator=(const NodePoints &n)
{
   Points=n.Points;
/*   for (int i=1;i<=n.NoPoints;i++){
      ic_ptcpy(Points[i-1],n.Points[i-1]);
   }
   NoPoints=n.NoPoints;*/
   return *this;
}

int NodePoints::GetSize()const
{
   return Points.GetCount();
//   return NoPoints;
}

/************************************************************/
#define NO_GRIPS_PER_BLOCK 1024


CGripNodesArray::CGripNodesArray()
{
   GripNodes.SetNoPointsPerBlock(NO_GRIPS_PER_BLOCK);
}

CGripNodesArray::~CGripNodesArray()
{
   GripNodes.RemoveAll();
}

void CGripNodesArray::Reset()
{
   GripNodes.RemoveAll();
   GripNodes.SetNoPointsPerBlock(NO_GRIPS_PER_BLOCK);
}

void CGripNodesArray::Add(sds_point p)
{
   GripNodes.Add(p);
}


int CGripNodesArray::GetNoGrips()const
{
   return GripNodes.GetCount();
}

/************************************************************/
#define HASH_TABLE_SIZE 65497

SDS_NodeList_New::SDS_NodeList_New()
{
   EntitiesWithNodes.InitHashTable(HASH_TABLE_SIZE);
}

SDS_NodeList_New::~SDS_NodeList_New()
{
	removeAll();
}

void SDS_NodeList_New::removeAll()
{
   EntitiesWithNodes.RemoveAll();
}

BOOL SDS_NodeList_New::find( CSdsName handle, NodePoints &Nodes)
{
   return EntitiesWithNodes.Lookup(handle,Nodes);
}

void SDS_NodeList_New::add( sds_name handle, sds_point pt )
{
	/*DG - 16.4.2003*/// This line with operator[] does the same as that two ones with NodePoints::NodePoints(sds_point) and SetAt;
	// and we don't want calling LookUp because we don't want get the object actually (which invokes calling operator= !).
#if 1
	EntitiesWithNodes[CSdsName(handle)].Add(pt);
#else
	CSdsName	Handle(handle);
	NodePoints	Nodes;
	if(EntitiesWithNodes.Lookup(Handle, Nodes))
		EntitiesWithNodes[Handle].Add(pt);
	else
	{
		NodePoints	NewNodes(pt);
		EntitiesWithNodes.SetAt(Handle, NewNodes);
	}
#endif
}

void SDS_NodeList_New::remove( sds_name handle )
{
	EntitiesWithNodes.RemoveKey(CSdsName(handle));
}

void SDS_NodeList_New::begin()
{
	m_pos = EntitiesWithNodes.GetStartPosition();
	if (m_pos != NULL)
	{
		EntitiesWithNodes.GetNextAssoc(m_pos, m_curhandle, m_CurNodePoints);
      m_NoPoints=m_CurNodePoints.GetSize();
   }
   else{
      m_curhandle=CSdsName();
      m_NoPoints=0;
   }
	m_ptIdx = 0;
}

BOOL SDS_NodeList_New::getPair( sds_name ename, sds_point pt )
{
	while ((m_pos!=NULL)||(m_NoPoints>0))
	{
		int numOfPoints = m_CurNodePoints.GetSize();
		if ( m_ptIdx < numOfPoints )
		{
         m_curhandle.Get(ename);
			m_CurNodePoints.GetAt(m_ptIdx+1,pt);
			m_ptIdx++;
         m_NoPoints--;
			return TRUE;
		}
		m_ptIdx = 0;
		EntitiesWithNodes.GetNextAssoc(m_pos, m_curhandle, m_CurNodePoints);
      m_NoPoints=m_CurNodePoints.GetSize();
	}
	return FALSE;
}

BOOL SDS_NodeList_New::getEntityName( sds_name ename)
{
	if (m_NoPoints>0)
	{
      m_curhandle.Get(ename);
      if (m_pos){
		   EntitiesWithNodes.GetNextAssoc(m_pos, m_curhandle, m_CurNodePoints);
         m_NoPoints=1;
      }
      else
         m_NoPoints=0;
		return TRUE;
	}
	return FALSE;
}
//<-4M Item:25 Item:28