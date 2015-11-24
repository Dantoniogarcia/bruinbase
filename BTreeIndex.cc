/*
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */
 
#include "BTreeIndex.h"
#include "BTreeNode.h"
#include <string.h>
#include <iostream>
#include <fstream>
using namespace std;

/*
 * BTreeIndex constructor
 */
BTreeIndex::BTreeIndex()
{
	
    rootPid = -1;
	treeHeight = 0;
	std::fill(buffer,buffer + PageFile::PAGE_SIZE,0);
	/*for(int i =0; i<PageFile::PAGE_SIZE; i++)
	{
		memcpy(buffer + i, &r, sizeof(RecordId)); //inputing the sizeof(RecordId) into the buffer
	}*/
}

/*
 * Open the index file in read or write mode.
 * Under 'w' mode, the index file should be created if it does not exist.
 * @param indexname[IN] the name of the index file
 * @param mode[IN] 'r' for read, 'w' for write
 * @return error code. 0 if no error
 */
 
 //side note, we are going to store all the data in page 0;
 //the rootpid which is the first pid will be stored at page 1;
RC BTreeIndex::open(const string& indexname, char mode)
{	
	if(pf.open(indexname, mode) != 0)
	{
		return -1;
	}
	
	//need to check if its a newly created page
	// call endid()
	RC pageChecker = pf.endPid();

	if(pageChecker != 0) //has been accessd before so need to get correct information
	{
		int check = pf.read(0, buffer);
		if(check != 0)
		{
			return check;
		}
		memcpy(&rootPid, buffer, sizeof(PageId));// stored the root pid in the first node
		cout<<"RootPid: " << rootPid << endl;
		memcpy(&treeHeight, buffer + sizeof(PageId), sizeof(int)); //stored the second root pid in the second node
		cout<<"treeheight :" << treeHeight << endl;
	}
	else
	{
		cout << "yeee"<<endl;
		rootPid = -1;
		treeHeight = 0;
		
		//not sue, maybe I should close the file
		//if(pf.write(0,buffer) == 0)
			//cout<<"ok"<<endl;
	}
    return 0;
}



/*
 * Close the index file.
 * @return error code. 0 if no error
 */
RC BTreeIndex::close()
{
	//save variables
	memcpy(buffer, &rootPid, sizeof(PageId));
	memcpy(buffer + sizeof(PageId), &treeHeight, sizeof(int));
	RC t = pf.write(0, buffer);
	pf.close();
	if(t!=0)
		return t;
	else
		return 0;
}

/*
 * Insert (key, RecordId) pair to the index.
 * @param key[IN] the key for the value inserted into the index
 * @param rid[IN] the RecordId for the record being inserted into the index
 * @return error code. 0 if no error
 */
 /*if(pf.endPid() == 0)
			rootPid =1;
		else
			rootPid = pf.endPid();*/
RC BTreeIndex::insert(int key, const RecordId& rid)
{
	//first case/test case
	if(treeHeight ==0)
	{
		BTLeafNode leaf1;
		leaf1.insert(key, rid);
		treeHeight = 1;
		//copy from above into here
		rootPid = 1;
		cout<<"Root PID: " <<rootPid<<endl;
		return leaf1.write(rootPid, pf); //error is here
	//	cout << "errorCode: " << errorCode<<endl;
	//	cout<< "not herehere5"<<endl;
	}
    return 0;
}

void BTreeIndex::printTree()
{
	if(treeHeight == 1)
	{
		BTLeafNode node;
		node.read(rootPid,pf);
		node.printNode();
	}
}

/**
 * Run the standard B+Tree key search algorithm and identify the
 * leaf node where searchKey may exist. If an index entry with
 * searchKey exists in the leaf node, set IndexCursor to its location
 * (i.e., IndexCursor.pid = PageId of the leaf node, and
 * IndexCursor.eid = the searchKey index entry number.) and return 0.
 * If not, set IndexCursor.pid = PageId of the leaf node and
 * IndexCursor.eid = the index entry immediately after the largest
 * index key that is smaller than searchKey, and return the error
 * code RC_NO_SUCH_RECORD.
 * Using the returned "IndexCursor", you will have to call readForward()
 * to retrieve the actual (key, rid) pair from the index.
 * @param key[IN] the key to find
 * @param cursor[OUT] the cursor pointing to the index entry with
 *                    searchKey or immediately behind the largest key
 *                    smaller than searchKey.
 * @return 0 if searchKey is found. Othewise an error code
 */
RC BTreeIndex::locate(int searchKey, IndexCursor& cursor)
{
    return 0;
}

/*
 * Read the (key, rid) pair at the location specified by the index cursor,
 * and move foward the cursor to the next entry.
 * @param cursor[IN/OUT] the cursor pointing to an leaf-node index entry in the b+tree
 * @param key[OUT] the key stored at the index cursor location.
 * @param rid[OUT] the RecordId stored at the index cursor location.
 * @return error code. 0 if no error
 */
RC BTreeIndex::readForward(IndexCursor& cursor, int& key, RecordId& rid)
{
    return 0;
}
