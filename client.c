/* This page contains a client program that can request a file from the server program
 * on the next page. The server responds by sending the whole file.
 */

#include "file-server.h"
#include <sys/fcntl.h>

int main(int argc, char **argv)
{
  int c, s, i, bytes, fd, debug;
  int compareWFlag = 0;
  int compareDash = 0;
  int wFlag = 0, startRangeFlag = 0, endRangeFlag = 0, rangeFlag = 0;
  int endByte, startByte;
  
  char buf[BUF_SIZE];		/* buffer for incoming file */
  int nameLength = strlen(argv[2]);
  char fileName[strlen(argv[2])-2];  /* buffer for file name in case it has the write flag */
  struct hostent *h;		/* info about server */
  struct sockaddr_in channel;		/* holds IP address */

   char passedFileName[nameLength];
   
   
  if (argc < 3 || argc > 8) 
	  fatal("Usage: server-name -s Start Byte -e End Byte -w(if writing to server) file-name");
  
  if(argv[2][0] == '-' && argv[2][1] == 's')
	  startRangeFlag = 1;
	  startByte = atoi(argv[3]);
  if(argv[4][0] == 's' && argv[4][1] == 'e'){
	  endRangeFlag = 1;
	  endByte = atoi(argv[5]);
  }
  if(startRangeFlag == 1 && endRangeFlag == 1)
	  rangeFlag = 1;
  if(argv[2][0] == '-' && argv[2][1] == 'w')
	  wFlag = 1;
  
  // test printing- remove later
  printf("argv2 reads as %s, argv3 reads as %s. Range Flag is %d and wFlag is %d\n", argv[2], argv[3], rangeFlag, wFlag);
		  
  if (wFlag == 1 && rangeFlag == 0){
			
	  for (i = 0; i < strlen(argv[3]); i++){
		fileName[i] = argv[3][i];
		
	  }	
  }
  
  if (wFlag == 1 && rangeFlag == 1) {
	  for (i = 0; i < strlen(argv[8]); i++){
		fileName[i] = argv[8][i];
		
	  }
  }

  printf("%s", fileName);
  printf("Enter 1 if you want to use Debugging Mode or enter 0 otherwise:\n");
  scanf("%d", &debug);
  
  
  h = gethostbyname(argv[1]);		/* look up host's IP address */
  if (!h) 
	  fatal("gethostbyname failed");
  


  s = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (s < 0) 
	fatal("socket");

  memset(&channel, 0, sizeof(channel));
  channel.sin_family= AF_INET;
  memcpy(&channel.sin_addr.s_addr, h->h_addr, h->h_length);
  channel.sin_port= htons(SERVER_PORT);

  c = connect(s, (struct sockaddr *) &channel, sizeof(channel));
  if (c < 0) fatal("connect failed");
  
  if (wFlag == 0){

	  /* Connection is now established. Send file name including 0 byte at end. */
	  write(s, argv[2], strlen(argv[2])+1);
	  
	  // send the debug flag
	  // can we just send the debug integer to the server?
	  write(s, &debug, sizeof(debug));

	  /* Go get the file and write it to standard output. */
	  while (1) {
			bytes = read(s, buf, BUF_SIZE);	/* read from socket */
			if (bytes <= 0) exit(0);		/* check for end of file */
			write(1, buf, bytes);		/* write to standard output */
	  }
  }
  
  if (wFlag == 1){
	  
	/* Get the file. */
    fd = open(fileName, O_RDONLY);	/* open the file to be sent to server */
        if (fd < 0) fatal("open failed");
		
	while (1) {
		
                bytes = read(fd, fileName, BUF_SIZE);	/* read from file */
                if (bytes <= 0) break;		/* check for end of file */
				
				// i just changed fileName to argv[2] and need to test still
                write(s, argv[2], bytes);		/* write bytes to socket */
				
				
        }
        close(fd);			/* close file */  
	  
  }
}
