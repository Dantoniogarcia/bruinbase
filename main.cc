/**
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */
 
#include "Bruinbase.h"
#include "SqlEngine.h"
#include <cstdio>
/*
#ifndef BTREENODE_H
#define BTREENODE_H

#include "RecordFile.h"
#include "PageFile.h"
#include <string.h>
#include <iostream>
#include <stdio.h>
using namespace std;
class BTLeafNode {
  public:
	BTLeafNode();
    RC insert(int key, const RecordId& rid);
    RC insertAndSplit(int key, const RecordId& rid, BTLeafNode& sibling, int& siblingKey);
    RC locate(int searchKey, int& eid);
    RC readEntry(int eid, int& key, RecordId& rid);
    PageId getNextNodePtr();
    RC setNextNodePtr(PageId pid);
    int getKeyCount();
    RC read(PageId pid, const PageFile& pf);
    RC write(PageId pid, PageFile& pf);
  private:
    char buffer[PageFile::PAGE_SIZE];
	//char temp[PageFile::PAGE_SIZE];
}; 
class BTNonLeafNode {
  public:
    RC insert(int key, PageId pid);
    RC insertAndSplit(int key, PageId pid, BTNonLeafNode& sibling, int& midKey);
    RC locateChildPtr(int searchKey, PageId& pid);
    RC initializeRoot(PageId pid1, int key, PageId pid2);
    int getKeyCount();
    RC read(PageId pid, const PageFile& pf);
    RC write(PageId pid, PageFile& pf);
  private:
    char buffer[PageFile::PAGE_SIZE];
};
*/
/* 
BTLeafNode::BTLeafNode()
{
	int zeros = 0;  // putting in a value of 0 as the first key
	int size = sizeof(int) + sizeof(RecordId);
	RecordId r;
	r.pid = -1;//setting the PID to -1, not allowed for a valid PID
	r.sid = -1;
	for(int i =0; i<PageFile::PAGE_SIZE; i+=size)
	{
		memcpy(buffer + i, &r, sizeof(RecordId)); //inputing the sizeof(RecordId) into the buffer
	}
	printf("Should Print Out");
}

RC BTLeafNode::read(PageId pid, const PageFile& pf)
{ 
	int code = pf.read(pid, buffer);  //this reads the page at pageID and puts it into the buffer
	return code; 
}

RC BTLeafNode::write(PageId pid, PageFile& pf)
{ 
	int code = pf.write(pid,buffer); // this writes from the buffer to the page at pid
	return code;  
	}

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

RC BTLeafNode::insert(int key, const RecordId& rid)
{	
	RC nodeFull;
	int size = sizeof(RecordId) + sizeof(int);
	for(int i = 0; i<PageFile::PAGE_SIZE-4; i+=size)
	{
		RecordId temp;
		int keyValue;
		memcpy(&temp,buffer + i,sizeof(RecordId));
		if(temp.pid == -1) //not a valid key here so we will input the key, rid
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
			char *temp = buffer; //temporary buffer
			memcpy(buffer + i, &rid, sizeof(RecordId)); // copy the recordId over
			memcpy(buffer + (i+sizeof(RecordId)), &key, sizeof(int)); //copy the key over
			for(int ii =i; ii<PageFile::PAGE_SIZE-size-4; ii+=size)
			{
				memcpy(buffer + (ii+size) , temp + ii,size);//copy over the buffer
			}
			return 0;
		}
	}
		return 0;
}

RC BTLeafNode::insertAndSplit(int key, const RecordId& rid, 
                              BTLeafNode& sibling, int& siblingKey)
{ return 0; }

RC BTLeafNode::locate(int searchKey, int& eid)
{
int size = sizeof(RecordId) + sizeof(int);
	for(int i = 0; i<PageFile::PAGE_SIZE-4; i+=size)
	{
		RecordId temp;
		int keyValue;
		int keyCount = 0;
		memcpy(&temp,buffer + i,sizeof(RecordId));
		if(temp.pid == -1) //not a valid key here so we will input the key, rid
		{
			
			eid = keyCount;
			return RC_NO_SUCH_RECORD; //return 0 since the RecordId,key was successfully added
		
		}
		else
		{
			memcpy(&keyValue,buffer + (i+sizeof(RecordId)), sizeof(int)); // we need the keyValue here because we know this is a valid entry and we will need
																		//to compare keyValues in the next step
		}
		if(searchKey < keyValue) //key we want to insert is less than the key that is already there so we want to insert it here
		{
			eid = keyCount + 1;
			return 0;
		}
		keyCount++;
	}
	return 0; 
}

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

PageId BTLeafNode::getNextNodePtr()
{ 
	//return a pid holding the memory of the last 4 bytes
	PageId id;
	memcpy(&id,buffer + PageFile::PAGE_SIZE-sizeof(PageId),sizeof(PageId));
	return id;
	}

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

RC BTNonLeafNode::read(PageId pid, const PageFile& pf)
{ return 0; }

RC BTNonLeafNode::write(PageId pid, PageFile& pf)
{ return 0; }

int BTNonLeafNode::getKeyCount()
{ return 0; }
RC BTNonLeafNode::insert(int key, PageId pid)
{ return 0; }

RC BTNonLeafNode::insertAndSplit(int key, PageId pid, BTNonLeafNode& sibling, int& midKey)
{ return 0; }

RC BTNonLeafNode::locateChildPtr(int searchKey, PageId& pid)
{ return 0; }

RC BTNonLeafNode::initializeRoot(PageId pid1, int key, PageId pid2)
{ return 0; }
*/
int main()
{
  // run the SQL engine taking user commands from standard input (console).
  SqlEngine::run(stdin);
 // printf("sex");
 // cout<<"HELP"<<endl;
  //BTLeafNode h;
  return 0;
}
