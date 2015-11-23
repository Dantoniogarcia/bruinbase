
#include "BTreeNode.h"
#include <string.h>
#include <iostream>
#include <stdio.h>
using namespace std;

//We need a constructor here

BTLeafNode::BTLeafNode()
{
	int size = sizeof(int) + sizeof(RecordId);
	RecordId r;
	r.pid = -1;//setting the PID to -1, not allowed for a valid PID
	r.sid = -1;
	//just added
	//std::fill(buffer,buffer + PageFile::PAGE_SIZE,0);
	//
	for(int i =0; i<PageFile::PAGE_SIZE; i+=size)
	{
		memcpy(buffer + i, &r, sizeof(RecordId)); //inputing the sizeof(RecordId) into the buffer
	}
}
void BTLeafNode::printNode()
{
	RecordId r;
	int key;
	int size = sizeof(int) + sizeof(RecordId);
	for(int i = 0; i <PageFile::PAGE_SIZE-4; i+=size)
	{
		memcpy(&r,buffer + i,sizeof(RecordId));
		if(r.pid<0)
		{
			break;
		}
		memcpy(&key, buffer + i + sizeof(RecordId), sizeof(int));
		cout<<"Key: "<< key<<endl;
		cout<<"RecordID PageID: "<< r.pid<<endl;
		cout<<"RecordID Sector ID: "<< r.sid<<endl;
		cout<<endl;
	}
}
/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read fr
 om
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::read(PageId pid, const PageFile& pf)
{ 
	int code = pf.read(pid, buffer);  //this reads the page at pageID and puts it into the buffer
	return code; 
}
    
/*
 * Write the content of the node to the page pid in the PageFile pf.
 * @param pid[IN] the PageId to write to
 * @param pf[IN] PageFile to write to
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::write(PageId pid, PageFile& pf)
{ 
	//cout<<"Error Here"<<endl;
	int code = pf.write(pid, buffer); // this writes from the buffer to the page at pid
	cout<<code<<endl;
	return code;  
}
/*
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
int BTLeafNode::getKeyCount()
{ 
	int size = sizeof(RecordId) + sizeof(int);  // size of recordId and key pair
	int count = 0;
	for(int i = 0; i<PageFile::PAGE_SIZE -4; i+=size) //check each position a key might be at starting at the offset of where the first key is at
	{
		//starting at sizeof(RecordId), is this correct?
		RecordId temp;
		memcpy(&temp,buffer + i,sizeof(RecordId));  //copy the keyValue of the node into the key
		if(temp.pid == -1) //if pageID =-1 then we have no more valid keys
		{return count;} //return the count of the keys
		else{count++;}
	}
	return count;
}

/*
 * Insert a (key, rid) pair to the node.
 * @param key[IN] the key to insert
 * @param rid[IN] the RecordId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */
RC BTLeafNode::insert(int key, const RecordId& rid)
{	
	int size = sizeof(RecordId) + sizeof(int);
	//ADDED
	int check = PageFile::PAGE_SIZE-sizeof(PageId);
	if( (this->getKeyCount()) * size == check)
	{
		return RC_NODE_FULL;
	}
	//ADDED
	for(int i = 0; i<PageFile::PAGE_SIZE-sizeof(PageId); i+=size)
	{
		RecordId tempr;
		int keyValue;
		memcpy(&tempr,buffer + i,sizeof(RecordId));
		if(tempr.pid == -1) //not a valid key here so we will input the key, rid
		{
			memcpy(buffer + i, &rid, sizeof(RecordId)); //copy in the recordID
			memcpy(buffer + (i+sizeof(RecordId)),&key, sizeof(int)); //copy in the Key
			return 0; //return 0 since the RecordId,key was successfully added
		}
		else
		{
			memcpy(&keyValue,buffer + (i+sizeof(RecordId)), sizeof(int)); // we need the keyValue here because we know this is a valid entry and we will need
																		//to compare keyValues in the next step
		}
		if(key < keyValue) //key we want to insert is less than the key that is already there so we want to insert it here
		{
			//create a temporary buffer, copy it intother and copy it all over
			char temp[PageFile::PAGE_SIZE];
			memcpy(temp, buffer, PageFile::PAGE_SIZE);
			memcpy(buffer + i, &rid, sizeof(RecordId)); // copy the recordId over
			memcpy(buffer + (i+sizeof(RecordId)), &key, sizeof(int)); //copy the key over
			for(int ii =i; ii<PageFile::PAGE_SIZE-size-4; ii+=size)
			{
				RecordId check;
				memcpy(&check, temp+ii, sizeof(RecordId));
				memcpy(buffer + (ii+size) , temp + ii, size);//copy over the buffer
			}
			return 0;
		}
	}
}

/*
 * Insert the (key, rid) pair to the node
 * and split the node half and half with sibling.
 * The first key of the sibling node is returned in siblingKey.
 * @param key[IN] the key to insert.
 * @param rid[IN] the RecordId to insert.
 * @param sibling[IN] the sibling node to split with. This node MUST be EMPTY when this function is called.
 * @param siblingKey[OUT] the first key in the sibling node after split.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::insertAndSplit(int key, const RecordId& rid, 
                              BTLeafNode& sibling, int& siblingKey)
{
	// 1.
	//check if sibling parameter is empty
	//
	
	if(sibling.getKeyCount() != 0)
	{
		cout<<sibling.getKeyCount()<<endl;
		cout<<"screwed up"<<endl;
		return RC_INVALID_ATTRIBUTE;
	}
	
	//2.
	//variables
	//
	int numberOfKeys = this->getKeyCount();
	cout<<numberOfKeys<<endl;
	int middle = (numberOfKeys)/ 2; // find the middle number
	int size = sizeof(RecordId) + sizeof(int); //pair size
		
	//3.
	//COPY over the second half of this node to the new node
	//
	for(int i = middle+1; i<=numberOfKeys; i++) //middle +1 because eids start at 1;
	{
		RecordId r;
		int k;
		readEntry(i,k,r); // this reads the data at that eid
		sibling.insert(k,r); // insert the data into the new node;
	}
	
	//memorymiddle variable;
	int memoryMiddle = middle*size;
	
	// 4.
	//"deleting" the entrys that were moved
	//
	//done by entering pids of -1 for them
	for(int i = memoryMiddle; i<PageFile::PAGE_SIZE-4; i+=size)
	{
		RecordId r2;
		r2.pid = -1;
		r2.sid = -1;
		memcpy(buffer + i,&r2,sizeof(RecordId)); // copied of a pair with that has a pageID from the RecordId = -1 to indicate it has not been set yet
	}
	
	RecordId whoCares;
	int keyCheck;
	//5.
	//insert the new key into one of the nodes
	this->readEntry(middle, keyCheck ,whoCares);
	if(key <keyCheck){
		this->insert(key, rid);
	}
	else{
		sibling.insert(key,rid);
	}
	
	//6.
	//nextNode pid pointer
	//set the new nodes next pointer = to this nodes next pointer
	PageId id = this->getNextNodePtr(); 
	sibling.setNextNodePtr(id);
	
	//7.
	//set sibling key to the first key of the new node
	
	sibling.readEntry(1, siblingKey, whoCares);
	
	return 0; 
}

/**
 * If searchKey exists in the node, set eid to the index entry
 * with searchKey and return 0. If not, set eid to the index entry
 * immediately after the largest index key that is smaller than searchKey,
 * and return the error code RC_NO_SUCH_RECORD.
 * Remember that keys inside a B+tree node are always kept sorted.
 * @param searchKey[IN] the key to search for.
 * @param eid[OUT] the index entry number with searchKey or immediately
                   behind the largest key smaller than searchKey.
 * @return 0 if searchKey is found. Otherwise return an error code.
 */
RC BTLeafNode::locate(int searchKey, int& eid)
{
	int size = sizeof(RecordId) + sizeof(int);
	int keyCount = 0;
	for(int i = 0; i<PageFile::PAGE_SIZE-4; i+=size)
	{
		RecordId temp;
		int keyValue;
		memcpy(&temp,buffer + i,sizeof(RecordId));
		if(temp.pid == -1) //not a valid key here so we will input the key, rid
		{
			//memcpy(buffer[i], &rid, sizeof(RecordId)); //copy in the recordID
			//memcpy(buffer[i+sizeof(RecordId)],&key, sizeof(int)); //copy in the Key
			keyCount++;
			eid = keyCount;
			return RC_NO_SUCH_RECORD; //return 0 since the RecordId,key was successfully added
		
		}
		else
		{
			memcpy(&keyValue, buffer + (i+sizeof(RecordId)), sizeof(int)); // we need the keyValue here because we know this is a valid entry and we will need
																		//to compare keyValues in the next step
		}
		if(searchKey == keyValue)
		{
			eid = keyCount + 1;
			return 0;
		}
		if(searchKey < keyValue) //key we want to insert is less than the key that is already there so we want to insert it here
		{
			eid = keyCount + 1;
			return RC_NO_SUCH_RECORD;
		}
		keyCount++;
	}
	eid = keyCount+1;
	return 0; 
}

/*
 * Read the (key, rid) pair from the eid entry.
 * @param eid[IN] the entry number to read the (key, rid) pair from
 * @param key[OUT] the key from the entry
 * @param rid[OUT] the RecordId from the entry
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::readEntry(int eid, int& key, RecordId& rid)
{ 
	int size = sizeof(RecordId) + sizeof(int);
	int counter = 0;
	for(int i =0; i<PageFile::PAGE_SIZE-4; i+=size) // looping through the node
	{
		counter ++; //counts the amount of symbols, THE FIRST SYMBOL IS SYMBOL 1 NOT SYMBOL 0
		if(eid == counter)
		{
			RecordId temp;
			memcpy(&temp,buffer + i,sizeof(RecordId)); //copy the bytes at RecordID
			if(temp.pid == -1) // this means that no node exists with that eid
				return -1 ;
			else
			{
				memcpy(&rid,buffer + i,sizeof(RecordId)); // copy the bytes into rid
				memcpy(&key, buffer + (i+sizeof(RecordId)), sizeof(int)); //copy the bytes into key 
				return 0;
			}
		}
	}
	return -1;
}

/*
 * Return the pid of the next slibling node.
 * @return the PageId of the next sibling node 
 */
PageId BTLeafNode::getNextNodePtr()
{ 
	//return a pid holding the memory of the last 4 bytes
	PageId id;
	memcpy(&id,buffer + PageFile::PAGE_SIZE-sizeof(PageId),sizeof(PageId));
	return id;
	}

/*
 * Set the pid of the next slibling node.
 * @param pid[IN] the PageId of the next sibling node 
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTLeafNode::setNextNodePtr(PageId pid)
{ 
	// make sure the pid is valid aka it is greater than 0
	if(pid<0) //maybe I should return a negative number
		return -1;
	else
	{
		memcpy(buffer + (PageFile::PAGE_SIZE-sizeof(PageId)),&pid, sizeof(PageId));
		return 0;
	}
}

/*
 * Read the content of the node from the page pid in the PageFile pf.
 * @param pid[IN] the PageId to read
 * @param pf[IN] PageFile to read from
 * @return 0 if successful. Return an error code if there is an error.
 */
BTNonLeafNode::BTNonLeafNode()
{
	//std::fill(buffer,buffer + PageFile::PAGE_SIZE,0);
	int size = sizeof(int) + sizeof(PageId);
	PageId id= -1;
	for(int i =0; i<PageFile::PAGE_SIZE; i+=size)
	{
		memcpy(buffer + i, &id, sizeof(PageId)); //inputing the sizeof(RecordId) into the buffer
	}
}
RC BTNonLeafNode::read(PageId pid, const PageFile& pf)
{ return pf.read(pid, buffer); 
}
    
/*
 * Write the content of the node to the page pid in the PageFile pf.
 * @param pid[IN] the PageId to write to
 * @param pf[IN] PageFile to write to
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::write(PageId pid, PageFile& pf)
{ return pf.write(pid, buffer);}//exact same as leafNode

/*
 * Return the number of keys stored in the node.
 * @return the number of keys in the node
 */
int BTNonLeafNode::getKeyCount()
{ 
	int size = sizeof(PageId) + sizeof(int);  // size of recordId and key pair
	int count = 0;
	for(int i = 2*size; i<PageFile::PAGE_SIZE-4; i+=size) //check each position a key might be at starting at the offset of where the first key is at
	{
		//starting at sizeof(RecordId), is this correct?
		PageId temp;
		memcpy(&temp,buffer+i,sizeof(PageId));  //copy the PageId of the node into the key
		//cout<<temp<<endl;
		if(temp < 0) //if pageID =-1 then we have no more valid keys
		{return count;} //return the count of the keys
		else{count++;}
	}
	return count;
 }


/*
 * Insert a (key, pid) pair to the node.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @return 0 if successful. Return an error code if the node is full.
 */

 RC BTNonLeafNode::insert(int key, PageId pid)
{ 
	int size = sizeof(int) + sizeof(PageId);
	//ADDED
	/*int check = PageFile::PAGE_SIZE-sizeof(PageId);
	if( (this->getKeyCount()) * size == check)
	{
		return RC_NODE_FULL;
	}*/
	//ADDED
	//skip the first 8 bytes we are now storing the root pid and the treeheight here
	//we are also skipping the first pid
	//char *temp = buffer + size + sizeof(PageId); //we also want to ignore the first pid
	for(int i = size + sizeof(PageId); i<PageFile::PAGE_SIZE; i+=size)
	{
		PageId tempId;
		int keyValue;
		memcpy(&tempId,buffer + i + sizeof(int),sizeof(PageId));
		if(tempId == -1) //not a valid key here so we will input the key, rid
		{
			memcpy(buffer + i, &key, sizeof(int)); //copy in the key
			memcpy(buffer + (i+sizeof(int)),&pid, sizeof(PageId)); //copy in the pageId
			return 0; //return 0 since the RecordId,key was successfully added
		}
		else
		{
			memcpy(&keyValue,buffer + i, sizeof(int)); // we need the keyValue here because we know this is a valid entry and we will need

	//to compare keyValues in the next step
		}
		if(key < keyValue) //key we want to insert is less than the key that is already there so we want to insert it here
		{
			//create a temporary buffer, copy it into it and then and copy it all over
			char temp[PageFile::PAGE_SIZE];
			memcpy(temp, buffer, PageFile::PAGE_SIZE);
			memcpy(buffer + i, &key, sizeof(int)); // copy the recordId over
			memcpy(buffer + (i+sizeof(int)), &pid, sizeof(PageId)); //copy the key over
			for(int ii =i; ii<PageFile::PAGE_SIZE-size; ii+=size)
			{
				memcpy(buffer + (ii+size) , temp + ii, size);//copy over the buffer
			}
			//simpler, just copy over the rest of the buffer 
			//memcpy(buffer + i + size,temp PageFile::PAGE_SIZE -
			return 0;
		}
	}
}

/*
 * Insert the (key, pid) pair to the node
 * and split the node half and half with sibling.
 * The middle key after the split is returned in midKey.
 * @param key[IN] the key to insert
 * @param pid[IN] the PageId to insert
 * @param sibling[IN] the sibling node to split with. This node MUST be empty when this function is called.
 * @param midKey[OUT] the key in the middle after the split. This key should be inserted to the parent node.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::insertAndSplit(int key, PageId pid, BTNonLeafNode& sibling, int& midKey)
{ return 0; }

/*
 * Given the searchKey, find the child-node pointer to follow and
 * output it in pid.
 * @param searchKey[IN] the searchKey that is being looked up.
 * @param pid[OUT] the pointer to the child node to follow.
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::locateChildPtr(int searchKey, PageId& pid)
{
	//check the first key
	int key1;
	int size = sizeof(PageId) + sizeof(int);
	int placeholder;
	for(int i = size + sizeof(PageId); i<PageFile::PAGE_SIZE; i+=size)
	{
		memcpy(&key1, buffer + i, sizeof(int));
		//cout<< "KEY: "<<key1<<endl;
		if(searchKey < key1 || searchKey == key1)
		{
			memcpy(&pid, buffer + i + sizeof(int), sizeof(PageId));
			return 0;
		}
		placeholder = i;
	}
	memcpy(&pid, buffer + placeholder + sizeof(int), sizeof(PageId));
	return 0; 
}

/*
 * Initialize the root node with (pid1, key, pid2).
 * @param pid1[IN] the first PageId to insert
 * @param key[IN] the key that should be inserted between the two PageIds
 * @param pid2[IN] the PageId to insert behind the key
 * @return 0 if successful. Return an error code if there is an error.
 */
RC BTNonLeafNode::initializeRoot(PageId pid1, int key, PageId pid2)
{ 
	memcpy(buffer, &pid1, sizeof(PageId));
	memcpy(buffer + sizeof(PageId), &key, sizeof(int));
	memcpy(buffer + sizeof(PageId) + sizeof(int), &pid2, sizeof(PageId));
	return 0; 
}

void BTNonLeafNode::printNode()
{
	/*int key;
	PageId pid;
	int size = sizeof(int) + sizeof(PageId);
	for(int i = size + sizeof(PageId); i <PageFile::PAGE_SIZE; i+=size)
	{
		memcpy(&pid,buffer + i+sizeof(int),sizeof(PageId));
		if(pid<0)
		{
			break;
		}
		memcpy(&key, buffer + i, sizeof(int));
		cout<<"Key: "<< key<<endl;
		cout<<"PageID: "<<pid<<endl;
		cout<<endl;
	}*/
	
}