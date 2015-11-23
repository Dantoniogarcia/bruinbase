I am trying to do part c and I am 
having a tough time inserting the first
 node through the index.  I wrote the 
write and read function in btreeIndex.cc 
and I wrote the most basic part of the 
insert function so that it could insert 
just the first root node.  However I am 
having memory issues when I run it.  
It stay stacksmashing.  My code to test 
is in SqlEngine.cc.  I also tested it 
with valgrind like my professor suggest 
and I could not find the error. 
I narrowed it down to something I am 
accessing has not bee defined yet and 
I think it has to do with 
leaf1.write(rootPid, pf) at like line 111 
in BTreeIndex.cc.  If you do not have
 time I completely understand.  
