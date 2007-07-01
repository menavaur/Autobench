/*******************************************************************
 * crfile - create a file of the specified size containing random  *
 * ASCII characters.                                               *
 *                                                                 *
 * Copyright (C) 2001 Julian T. J. Midgley <jtjm@xenoclast.org>    *
 *                                                                 *
 * Usage: crfile -f <filename> -s <size> [-n]                      *
 *                                                                 *
 *  where <filename> is the name of the file to create             *
 *        <size>     is the size of the file in bytes              *
 *        -n         if present, specifies that the file should    *
 *                   be padded with nulls (enables the creation of *
 *                   large files that don't actually take up much  *
 *                   much space on disk)                           *
 *        -w[n]      Wrap the output at n characters, or at 80     *
 *                   characters if n omitted                       *
 *                                                                 *
 * This program is free software; you can redistribute it and/or   *
 * modify it under the terms of the GNU General Public License as  *
 * published by the Free Software Foundation; either version 2 of  *
 * the License, or (at your option) any later version.             *
 *                                                                 *
 * This program is distributed in the hope that it will be         *
 * useful, but WITHOUT ANY WARRANTY; without even the implied      * 
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR         *
 * PURPOSE.  See the GNU General Public License for more details.  *
 *                                                                 *
 * You should have received a copy of the GNU General Public       *
 * License along with this program; if not, write to the Free      *
 * Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,  *
 * MA  02111-1307  USA                                             *
 *                                                                 * 
 * A copy of version 2 of the GNU General Public License may be    * 
 * found in the file "LICENCE" in the autobench source tarball.    *
 *                                                                 *
 *******************************************************************/
          
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>

#define VERSION "1.0"
#define WORDWRAP 80
#define OPTSTRING "f:s:nw::"
#define BUFFER_SIZE 1048576

typedef struct {
  char buffer[BUFFER_SIZE]; 
  long ptr;                 /* Position of the next free space in the buffer */
  int flushed;              /* 1 if buffer empty, 0 otherwise                */
} wbuffer;

int
init_buffer(wbuffer *buf)
{
  /* Initialise the buffer, and set the flushed flag */
  buf->ptr = 0;
  buf->flushed = 1;
  return(1);
}


int
put_buffer(wbuffer *buf, char c)
{
  /* Push c onto the buffer, and reset the flushed flag */
  *(buf->buffer + buf->ptr) = c;
  buf->ptr++;
  buf->flushed = 0;
  return (buf->ptr == BUFFER_SIZE);
} 


int
write_buffer(wbuffer *buf, int fd)
{
  /* Write the contents of the buffer to file descriptor fd */
  write(fd, buf->buffer, buf->ptr);
  init_buffer(buf);
  return(1);
}


int
usage(void)
{
  const char * version = VERSION;
  fprintf(stderr,"crfile %s, by Julian T. J. Midgley <jtjm@xenoclast.org>\n\n",version);
  fprintf(stderr,"Usage: crfile -f <filename> -s <size> [-n -w[<l>]]\n");
  fprintf(stderr,"    -f <filename> : Create file <filename>\n");
  fprintf(stderr,"    -s <size>     : of size <size> bytes\n");
  fprintf(stderr,"    -n            : pad file with nulls\n");
  fprintf(stderr,"    -w[<l>]       : Wrap the output at <l> characters, or\n");
  fprintf(stderr,"                    80 characters if <l> omitted\n");
  return(0);
}
  

char
rand_ascii(void) 
{
  return(32+(int) ((rand()/(RAND_MAX+1.0))*95.0));
}

int
main(int argc, char **argv)
{
  char *filename; //, *optarg;
  long size = 0, i, l, line_length = WORDWRAP;
  char c;
  int fd, opt_f = 0, opt_s = 0, opt_n = 0, opt_w = 0;
  char opt = 0;
  wbuffer buf;

  filename = argv[0]; /* Initialised purely to avoid warnings on compile */

  /* Parse arguments */
  while (opt != -1) {
    opt = getopt(argc, argv, OPTSTRING);
    if (opt == 'f') {
      filename = optarg;
      opt_f = 1;
    }
    if (opt == 's') {
      size = strtol(optarg, NULL, 10);
      opt_s = 1;
    }
    if (opt == 'n') {
      opt_n = 1;
    }
    if (opt == 'w') {
      opt_w = 1;
      if (optarg) {
	line_length = strtol(optarg, NULL, 10);
	if (line_length < 2) {
	  fprintf(stderr, "Argument to -w must be greater than 2\n");
	  exit(1);
	}
      }
      else {
	line_length = WORDWRAP;
      }
    }
  }
  
  if (!(opt_s && opt_f)) {
    usage();
    exit(1);
  }
  
  printf("Creating file '%s', of size %ld bytes\n",filename,size);

  fd = open(filename, O_CREAT|O_WRONLY|O_EXCL,0666);
  if (fd == -1) {
    /* Error in opening file, fail gracefully */
    perror("Cannot open file");
    exit (1);
  }
  
  /* Initialise random number generator with current time */
  srand(time(NULL)+getpid()); 

  /* Initialise buffer */
  init_buffer(&buf);
  
  if (opt_n) {
    /* Create a file padded with nulls */
    c = rand_ascii();
    lseek(fd, size-1, SEEK_SET);
    write(fd, &c, 1);
  }
  else {
    /* Fill the file with random ASCII characters */
    l = 0;
    for (i=0; i<size; i++) {
      c = rand_ascii(); /* Generate ASCII character */
      if (put_buffer(&buf,c)) {
	write_buffer(&buf,fd);
      }
      l++;
      if (opt_w && (l > (line_length-2)) && (i < size-1)) {
	if (put_buffer(&buf,'\n')) {
	  write_buffer(&buf,fd);
	}
	l=0;
	i++;
      }
    }
  }
  
  /* If the buffer hasn't been flushed, flush it */
  if (! buf.flushed) {
    write_buffer(&buf,fd);
  }
  close (fd);  
  exit(0);
}
