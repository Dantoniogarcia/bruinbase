/**
 * Copyright (C) 2008 by The Regents of the University of California
 * Redistribution of this file is permitted under the terms of the GNU
 * Public License (GPL).
 *
 * @author Junghoo "John" Cho <cho AT cs.ucla.edu>
 * @date 3/24/2008
 */

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include "Bruinbase.h"
#include "SqlEngine.h"
#include "BTreeNode.h"
#include "BTreeIndex.h"
using namespace std;

// external functions and variables for load file and sql command parsing 
extern FILE* sqlin;
int sqlparse(void);

RC SqlEngine::run(FILE* commandline)
{
  //printf("help me");
  /*BTLeafNode h;
  RecordId r1;
  r1.pid= 1;
  r1.sid = 0;
  RecordId r2;
  r2.pid = 2;
  r2.sid=0;
  h.insert(10,r1);
  //h.printNode();
  h.insert(7,r2);
  h.insert(15,r2);
  h.insert(0,r2);
  h.insert(5,r2);
  BTLeafNode h2;
  int key1;
  //cout<<h2.getKeyCount()<<endl;
  h.insertAndSplit(3,r2,h2,key1);
  h.printNode();
  cout<< "Next Node: "<<endl;
  cout<<endl;
  h2.printNode();*/
  /*int eid;
  h.locate(15,eid);
  cout<<eid<<endl;*/
  /*int key;
  RecordId recordid;
  h.readEntry(3, key, recordid);
  cout<<"Key: "<<key<<endl;
  cout<<"RecordId-Pid: "<<recordid.pid<<endl;
  cout<<"RecordId-Sid: "<<recordid.sid<<endl;
  int s = sizeof(RecordId) + sizeof(int);
  cout<<"Size s: " << s <<endl;*/
  
  //Test Cases nonleaf nodes
  /*
  BTNonLeafNode nonleaf1;
  //insert 1st pair
  int key1 = 1;
  PageId pid1 = 1;
  nonleaf1.insert(key1,pid1);
  //insert second pair
  int key2 = 3;
  PageId pid2 = 3;
  //insert 3rd pair
  nonleaf1.insert(key2, pid2);
  int key3 = 5;
  PageId pid3 = 5;
  nonleaf1.insert(key3,pid3);
 
  //insert 4th pair
  int key4 = 7;
  PageId pid4 = 7;
  nonleaf1.insert(key4, pid4);
  nonleaf1.printNode();
  cout<<"Number of Keys: " << nonleaf1.getKeyCount()<<endl;
  int nonLeafPid = -5;
  nonleaf1.locateChildPtr(5,nonLeafPid);
  cout<<nonLeafPid<<endl;
  */
  
/*  BTreeIndex index;
  string name = "antonio4.txt";
  RecordId ss;
  ss.pid = 3;
  ss.sid = 1;
  index.open(name,'w');
  cout<<"1" <<endl;
  cout<<"stupid"<<endl;
  RC shit = index.insert(1,ss);
  cout<<"Not here 1"<<endl;
  index.printTree();
  index.close();*/
  fprintf(stdout, "Bruinbase> ");
  // set the command line input and start parsing user input
  sqlin = commandline;
  sqlparse();  // sqlparse() is defined in SqlParser.tab.c generated from
               // SqlParser.y by bison (bison is GNU equivalent of yacc)
  		   
  return 0;
}

RC SqlEngine::select(int attr, const string& table, const vector<SelCond>& cond)
{
  RecordFile rf;   // RecordFile containing the table
  RecordId   rid;  // record cursor for table scanning

  RC     rc;
  int    key;     
  string value;
  int    count;
  int    diff;

  // open the table file
  if ((rc = rf.open(table + ".tbl", 'r')) < 0) {
    fprintf(stderr, "Error: table %s does not exist\n", table.c_str());
    return rc;
  }

  // scan the table file from the beginning
  rid.pid = rid.sid = 0;
  count = 0;
  while (rid < rf.endRid()) {
    // read the tuple
    if ((rc = rf.read(rid, key, value)) < 0) {
      fprintf(stderr, "Error: while reading a tuple from table %s\n", table.c_str());
      goto exit_select;
    }

    // check the conditions on the tuple
    for (unsigned i = 0; i < cond.size(); i++) {
      // compute the difference between the tuple value and the condition value
      switch (cond[i].attr) {
      case 1:
	diff = key - atoi(cond[i].value);
	break;
      case 2:
	diff = strcmp(value.c_str(), cond[i].value);
	break;
      }
      // skip the tuple if any condition is not met
      switch (cond[i].comp) {
      case SelCond::EQ:
	if (diff != 0) goto next_tuple;
	break;
      case SelCond::NE:
	if (diff == 0) goto next_tuple;
	break;
      case SelCond::GT:
	if (diff <= 0) goto next_tuple;
	break;
      case SelCond::LT:
	if (diff >= 0) goto next_tuple;
	break;
      case SelCond::GE:
	if (diff < 0) goto next_tuple;
	break;
      case SelCond::LE:
	if (diff > 0) goto next_tuple;
	break;
      }
    }

    // the condition is met for the tuple. 
    // increase matching tuple counter
    count++;

    // print the tuple 
    switch (attr) {
    case 1:  // SELECT key
      fprintf(stdout, "%d\n", key);
      break;
    case 2:  // SELECT value
      fprintf(stdout, "%s\n", value.c_str());
      break;
    case 3:  // SELECT *
      fprintf(stdout, "%d '%s'\n", key, value.c_str());
      break;
    }

    // move to the next tuple
    next_tuple:
    ++rid;
  }

  // print matching tuple count if "select count(*)"
  if (attr == 4) {
    fprintf(stdout, "%d\n", count);
  }
  rc = 0;

  // close the table file and return
  exit_select:
  rf.close();
  return rc;
}
RC SqlEngine::load(const string& table, const string& loadfile, bool index)
{
  /* your code here */
  
  //variable declaration
  RecordFile recordfile;
  RecordId id;
  int key;
  string value;
  //create the Record File or opens it if it is already created
  recordfile.open((table + ".tbl").c_str(),'w');
  //assuming the index is false for part a
  //opening the file
  string line;
  ifstream file (loadfile.c_str()); //have to add .c_str() since a c-string
  if(file.is_open())
  {
	  while ( getline(file, line)) // read each line
	  {
		  parseLoadLine(line, key, value); //call the parse function
		  recordfile.append(key, value, id); //append the data to the end of the table/record
	  }
	  
	  file.close();
  }
  else cout<< "Can't Open";
  
  return 0;
}

RC SqlEngine::parseLoadLine(const string& line, int& key, string& value)
{
    const char *s;
    char        c;
    string::size_type loc;
    
    // ignore beginning white spaces
    c = *(s = line.c_str());
    while (c == ' ' || c == '\t') { c = *++s; }

    // get the integer key value
    key = atoi(s);

    // look for comma
    s = strchr(s, ',');
    if (s == NULL) { return RC_INVALID_FILE_FORMAT; }

    // ignore white spaces
    do { c = *++s; } while (c == ' ' || c == '\t');
    
    // if there is nothing left, set the value to empty string
    if (c == 0) { 
        value.erase();
        return 0;
    }

    // is the value field delimited by ' or "?
    if (c == '\'' || c == '"') {
        s++;
    } else {
        c = '\n';
    }

    // get the value string
    value.assign(s);
    loc = value.find(c, 0);
    if (loc != string::npos) { value.erase(loc); }

    return 0;
}
