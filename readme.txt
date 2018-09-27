Manojit Chakraborty
2018201032

This is a project for creating a Mini-Torrent System using Peer2Peer networking.
Command for compiling Client.cpp : make client
Command for compiling tracker1.cpp : make tracker1
Command for compiling tracker2.cpp : make tracker2
Command for compiling all files : make all

Command for running Client : ​ ./client <CLIENT_IP>:<UPLOAD_PORT> <TRACKER_IP_1>:<TRACKER_PORT_1>
<TRACKER_IP_2>:<TRACKER_PORT_2> <log_file>

Command for running Tracker1 : ./tracker1 <my_tracker_ip>:<my_tracker_port>
<other_tracker_ip>:<other_tracker_port> <seederlist_file> <log_file>

Command for running Tracker2 : ./tracker2 <my_tracker_ip>:<my_tracker_port>
<other_tracker_ip>:<other_tracker_port> <seederlist_file> <log_file>

Client Side :

1. Client can share a file from his local system using "share <filepath> <filename.mtorrent>.
   As for example, if a file is abc.txt, client has to write "share abc.txt abc.txt.mtorrent".
   This file will contain Tracker1 URL, Tracker2 URL, concatenated SHA1 Hash of each chunks of 
   the file, which will be a multiple of 20, filename, filesize.Then it will send a message containing
   filename, SHA1 of the concatenated SHA1 for the file, its IP, its Port to the trackers, which will
   update the details in their seederlist.

2. Client can remove a mtorrent file from his system, as well as the seederlist of the trackers
   by using "remove <filename.mtorrent>". Suppose a mtorrent file resides with client abc.txt.mtorrent,
   so the command will be "remove abc.txt.mtorrent". This command will automatically delete the 
   mtorrent file from the client, as well as delete its entries from seederlist file resided with
   each tracker by communicating with it with proper message.

3. Client can exit from the application by writing "exit". It will communicate with the trackers
   at this time. After this, all of the entries for that client will be deleted from each seederlist
   for each tracker.

4. Multiple clients can run and share file, remove torrent, exit at the same time with multiple
   trackers, which is implemented via multithreading.

Tracker Side :

1. As soon as any client shares a file, it is updated in the seederlist of the tracker, i.e 
   seederlist(1/2).txt, according to the tracker number. 

2. Multiple client can connect with multiple trackers and share file, remove torrent at the same time, which
   is implemented via multithreading.

3. Tracker1 is the main tracker which in turn connects with tracker2 with the information of sharing
   or removing a file or a torrent from client, if it is active. So, both the seederlist files are updating
   simultaneously. Otherwise tracker2 will update all the details in its seederlist.





