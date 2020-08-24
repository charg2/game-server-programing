#include "PathFinder.h"



PathFinder::PathFinder()
{
	m_pOpenList = NULL;
	m_pClosedList = NULL;
	m_pCurPath = NULL;
	m_pStack = (STACK*)calloc(1, sizeof(STACK));
	//m_pTileMap = NULL;
}

////////////////////////////////////////////////////////////////////////////////

PathFinder::~PathFinder()
{
	FreeNodes();
	free(m_pStack);
#ifdef MAPDATANOCOPY
	m_pTileMap = NULL;
#else
	if (m_pTileMap != NULL)
	{
		delete m_pTileMap;
		m_pTileMap = NULL;
	}
#endif
}

////////////////////////////////////////////////////////////////////////////////
//                             Public Member Functions                        //
////////////////////////////////////////////////////////////////////////////////


BOOL PathFinder::InitAstarMap(BYTE* pMap, INT32 w, INT32 h)
{
	m_nColCnt = w;
	m_nRowCnt = h;
	m_nTotalTiles = m_nRowCnt * m_nColCnt;
#ifdef MAPDATANOCOPY
	m_pTileMap = pMap;
#else
	if (m_pTileMap != NULL)
	{
		delete m_pTileMap;
		m_pTileMap = NULL;
	}
	unsigned long BufSize;
	BufSize = (m_nTotalTiles + 7) >> 3;
	m_pTileMap = new BYTE[BufSize];
	memcpy(m_pTileMap, pMap, BufSize);
#endif

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////

BOOL PathFinder::NewPath(int sx, int sy, int dx, int dy)
{
	if (IsTileAviable(dx, dy) && IsTileAviable(sx, sy) && (GetTileNum(sx, sy) != GetTileNum(dx, dy)))
	{
		FreeNodes();
		FindPath(sx, sy, dx, dy);
	}

	return m_pCurPath != NULL;
}

////////////////////////////////////////////////////////////////////////////////
BOOL PathFinder::IsReached(void) // check it's return value before getting
{
	if (m_pCurPath->Parent == NULL)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL PathFinder::PathNextNode(void)
{
	if (m_pCurPath->Parent != NULL)
	{
		m_pCurPath = m_pCurPath->Parent;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

INT32 PathFinder::NodeGetX()
{
	return m_pCurPath->x;
}

int PathFinder::NodeGetY(void)
{
	return m_pCurPath->y;
}

////////////////////////////////////////////////////////////////////////////////

int PathFinder::GetTileNum(int x, int y)
{
	if (x < 0 || x >= m_nColCnt || y < 0 || y >= m_nRowCnt)
	{
		return 0;
	}

	return (y * m_nColCnt + x);
}

////////////////////////////////////////////////////////////////////////////////

int PathFinder::IsTileAviable(int x, int y)
{
	if (x < 0 || x >= m_nColCnt || y < 0 || y >= m_nRowCnt)
	{
		return 0;
	}

	int bytes, bits, val, index;
	index = y * m_nColCnt + x;
	bytes = index >> 3;
	bits = index & 0x07;
	val = m_pTileMap[bytes] >> (7 - bits);
	if ((val & 0x01) == 0 && m_pTileMap[index] == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void PathFinder::FreeNodes(void)
{
	NODE* Node, * OldNode;
	if (m_pOpenList != NULL)
	{
		Node = m_pOpenList->NextNode;
		while (Node != NULL)
		{
			OldNode = Node;
			Node = Node->NextNode;
			free(OldNode);
		}
	}

	if (m_pClosedList != NULL)
	{
		Node = m_pClosedList->NextNode;
		while (Node != NULL)
		{
			OldNode = Node;
			Node = Node->NextNode;
			free(OldNode);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
//                               A* Algorithm                                 //
////////////////////////////////////////////////////////////////////////////////
void PathFinder::FindPath(int sx, int sy, int dx, int dy)
{
	NODE* Node, * BestNode;
	int TileNumDest;

	TileNumDest = GetTileNum(sx, sy);
	m_pOpenList = (NODE*)calloc(1, sizeof(NODE));
	m_pClosedList = (NODE*)calloc(1, sizeof(NODE));

	Node = (NODE*)calloc(1, sizeof(NODE));
	Node->g = 0;
	Node->h = (dx - sx) * (dx - sx) + (dy - sy) * (dy - sy);
	Node->f = Node->g + Node->h;
	Node->NodeNum = GetTileNum(dx, dy);
	Node->x = dx;
	Node->y = dy;
	m_pOpenList->NextNode = Node;
	for (;;)
	{
		BestNode = GetBestNode();
		if (BestNode == NULL)
		{
			break;
		}
		else if (BestNode->NodeNum == TileNumDest)
		{
			break;
		}

		GenerateSuccessors(BestNode, sx, sy); //£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿£¿(dx,dy)
	}
	m_pCurPath = BestNode;
}

////////////////////////////////////////////////////////////////////////////////

PathFinder::NODE
* PathFinder::GetBestNode(void)
{
	NODE* tmp;
	if (m_pOpenList->NextNode == NULL)
	{
		return NULL;;
	}

	// Pick Node with lowest f, in this case it's the first node in list
	// because we sort the OPEN list wrt lowest f. Call it BESTNODE.
	tmp = m_pOpenList->NextNode;   // point to first node on OPEN
	m_pOpenList->NextNode = tmp->NextNode;    // Make OPEN point to nextnode or NULL.

	// Next take BESTNODE (or temp in this case) and put it on CLOSED
	tmp->NextNode = m_pClosedList->NextNode;
	m_pClosedList->NextNode = tmp;

	return(tmp);
}

////////////////////////////////////////////////////////////////////////////////

void PathFinder::GenerateSuccessors(NODE* BestNode, int dx, int dy)
{
	int x, y;

	// Upper-Left
	if (IsTileAviable(x = BestNode->x - TILESIZE, y = BestNode->y - TILESIZE))
	{
		GenerateSucc(BestNode, x, y, dx, dy);
	}
	// Upper
	if (IsTileAviable(x = BestNode->x, y = BestNode->y - TILESIZE))
	{
		GenerateSucc(BestNode, x, y, dx, dy);
	}
	// Upper-Right
	if (IsTileAviable(x = BestNode->x + TILESIZE, y = BestNode->y - TILESIZE))
	{
		GenerateSucc(BestNode, x, y, dx, dy);
	}
	// Right
	if (IsTileAviable(x = BestNode->x + TILESIZE, y = BestNode->y))
	{
		GenerateSucc(BestNode, x, y, dx, dy);
	}
	// Lower-Right
	if (IsTileAviable(x = BestNode->x + TILESIZE, y = BestNode->y + TILESIZE))
	{
		GenerateSucc(BestNode, x, y, dx, dy);
	}
	// Lower
	if (IsTileAviable(x = BestNode->x, y = BestNode->y + TILESIZE))
	{
		GenerateSucc(BestNode, x, y, dx, dy);
	}
	// Lower-Left
	if (IsTileAviable(x = BestNode->x - TILESIZE, y = BestNode->y + TILESIZE))
	{
		GenerateSucc(BestNode, x, y, dx, dy);
	}
	// Left
	if (IsTileAviable(x = BestNode->x - TILESIZE, y = BestNode->y))
	{
		GenerateSucc(BestNode, x, y, dx, dy);
	}
}

////////////////////////////////////////////////////////////////////////////////

void PathFinder::GenerateSucc(NODE* BestNode, int x, int y, int dx, int dy)
{
	int g, TileNumS, c = 0;
	NODE* Old, * Successor;

	g = BestNode->g + 1;	  // g(Successor)=g(BestNode)+cost of getting from BestNode to Successor
	TileNumS = GetTileNum(x, y); // identification purposes

	if ((Old = CheckOPEN(TileNumS)) != NULL) // if equal to NULL then not in OPEN list, else it returns the Node in Old
	{
		for (c = 0; c < 8; c++)
			if (BestNode->Child[c] == NULL) // Add Old to the list of BestNode's Children (or Successors).
			{
				break;
			}
		BestNode->Child[c] = Old;

		if (g < Old->g)  // if our new g value is < Old's then reset Old's parent to point to BestNode
		{
			Old->Parent = BestNode;
			Old->g = g;
			Old->f = g + Old->h;
		}
	}
	else if ((Old = CheckCLOSED(TileNumS)) != NULL) // if equal to NULL then not in CLOSE list, else it returns the Node in Old
	{
		for (c = 0; c < 8; c++)
			if (BestNode->Child[c] == NULL) // Add Old to the list of BestNode's Children (or Successors).
			{
				break;
			}
		BestNode->Child[c] = Old;

		if (g < Old->g)  // if our new g value is < Old's then reset Old's parent to point to BestNode
		{
			Old->Parent = BestNode;
			Old->g = g;
			Old->f = g + Old->h;
			PropagateDown(Old);  // Since we changed the g value of Old, we need
			// to propagate this new value downwards, i.e.
			// do a Depth-First traversal of the tree!
		}
	}
	else
	{
		Successor = (NODE*)calloc(1, sizeof(NODE));
		Successor->Parent = BestNode;
		Successor->g = g;
		Successor->h = (x - dx) * (x - dx) + (y - dy) * (y - dy); // should do sqrt(), but since we don't really
		Successor->f = g + Successor->h;   // care about the distance but just which branch looks
		Successor->x = x;                 // better this should suffice. Anyayz it's faster.
		Successor->y = y;
		Successor->NodeNum = TileNumS;
		Insert(Successor);     // Insert Successor on OPEN list wrt f
		for (c = 0; c < 8; c++)
			if (BestNode->Child[c] == NULL) // Add Old to the list of BestNode's Children (or Successors).
			{
				break;
			}
		BestNode->Child[c] = Successor;
	}
}

////////////////////////////////////////////////////////////////////////////////

PathFinder::NODE
* PathFinder::CheckOPEN(int tilenum)
{
	NODE* tmp;

	tmp = m_pOpenList->NextNode;
	while (tmp != NULL)
	{
		if (tmp->NodeNum == tilenum)
		{
			return (tmp);
		}
		else
		{
			tmp = tmp->NextNode;
		}
	}
	return(NULL);
}

////////////////////////////////////////////////////////////////////////////////

PathFinder::NODE
* PathFinder::CheckCLOSED(int tilenum)
{
	NODE* tmp;

	tmp = m_pClosedList->NextNode;

	while (tmp != NULL)
	{
		if (tmp->NodeNum == tilenum)
		{
			return(tmp);
		}
		else
		{
			tmp = tmp->NextNode;
		}
	}
	return(NULL);
}

////////////////////////////////////////////////////////////////////////////////

void PathFinder::Insert(NODE* Successor)
{
	NODE* tmp1, * tmp2;
	int f;

	if (m_pOpenList->NextNode == NULL)
	{
		m_pOpenList->NextNode = Successor;
		return;
	}

	f = Successor->f;
	tmp1 = m_pOpenList;
	tmp2 = m_pOpenList->NextNode;

	while ((tmp2 != NULL) && (tmp2->f < f))
	{
		tmp1 = tmp2;
		tmp2 = tmp2->NextNode;
	}

	Successor->NextNode = tmp2;
	tmp1->NextNode = Successor;
}

////////////////////////////////////////////////////////////////////////////////

void PathFinder::PropagateDown(NODE* Old)
{
	int c, g;
	NODE* Child, * Father;

	g = Old->g;            // alias.
	for (c = 0; c < 8; c++)
	{
		if ((Child = Old->Child[c]) == NULL) // create alias for faster access.
		{
			break;
		}
		if (g + 1 < Child->g)
		{
			Child->g = g + 1;
			Child->f = Child->g + Child->h;
			Child->Parent = Old;           // reset parent to new path.
			Push(Child);                 // Now the Child's branch need to be
		}     // checked out. Remember the new cost must be propagated down.
	}

	while (m_pStack->pNextStack != NULL)
	{
		Father = Pop();
		for (c = 0; c < 8; c++)
		{
			if ((Child = Father->Child[c]) == NULL) // we may stop the propagation 2 ways: either
			{
				break;
			}
			if (Father->g + 1 < Child->g) // there are no children, or that the g value of
			{
				// the child is equal or better than the cost we're propagating
				Child->g = Father->g + 1;
				Child->f = Child->g + Child->h;
				Child->Parent = Father;
				Push(Child);
			}
		}
	}
}


////////////////////////////////////////////////////////////////////////////////
//                              STACK FUNCTIONS                               //
////////////////////////////////////////////////////////////////////////////////
void PathFinder::Push(NODE* Node)
{
	STACK* tmp;
	tmp = (STACK*)calloc(1, sizeof(STACK));
	tmp->pNode = Node;
	tmp->pNextStack = m_pStack->pNextStack;
	m_pStack->pNextStack = tmp;
}

////////////////////////////////////////////////////////////////////////////////
PathFinder::NODE
* PathFinder::Pop(void)
{
	NODE* tmp;
	STACK* tmpSTK;

	tmpSTK = m_pStack->pNextStack;
	tmp = tmpSTK->pNode;
	m_pStack->pNextStack = tmpSTK->pNextStack;
	free(tmpSTK);
	return(tmp);
}
