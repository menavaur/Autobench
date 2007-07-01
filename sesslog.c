/*******************************************************************
 * sesslog - creates a session log suitable for use with the       *
 * --wsesslog option of httperf from an NCSA Common Log Format     *
 * or combined log format log.                                     *
 *                                                                 *
 * Usage: sesslog [<infile>] [<outfile>]                           *
 *                                                                 *
 * Copyright (C) 2001 Julian T. J. Midgley <jtjm@xenoclast.org>    *
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


#define BUFF_SIZE 1024
#include <stdio.h>

enum boolean { FALSE, TRUE }; 

int 
main(int argc, char *argv[])
{
  char *infile, *outfile;
  char method[BUFF_SIZE];
  char *p;
  int bm, em, bu, eu; /* Flags to indicate beginning and end of 
                             the HTTP method and the URL */
  FILE *ifs, *ofs;    /* Streams for input and output */
  char ch, pch;

  ifs = ofs = stderr;      /* Purely to avoid compile time errors of the  */
  infile = outfile = "";   /* "'foo' might be used uninitialized" variety */  

  switch (argc) {
  case 1: 
    ifs = stdin; ofs = stdout; break;
  case 2: 
    infile = argv[1]; ofs = stdout; break;
  case 3: 
    infile = argv[1]; outfile = argv[2]; break;
  default: 
    fprintf(stderr, "Usage: cr_sesslog [<infile>] [<outfile>]\n");
    return(-1); 
    break;
  }
  
  if (ifs != stdin && ((ifs = fopen(infile,"r")) == NULL)) {
    perror(infile);
    return(-1);
  }
  if (ofs != stdout && ((ofs = fopen(outfile,"w")) == NULL)) {
    perror(outfile);
    return(-1);
  }  
  
  bm = em = bu = eu = FALSE;
  pch = '*';
  p = method;

  /*****************************************************************************
   * The following is essentially the equivalent of the following Perl snippet *
   *     while (<IN>) {                                                        *
   *         if (m/\"(\S+) (\S+)/) {                                           *
   *             print OUT "$2 method=$1\n";                                   *
   *         }                                                                 *
   *     }                                                                     *
   * I've implemented this in C rather then Perl purely because it is          *
   * marginally faster.                                                        *
   *****************************************************************************/
  while ((ch = fgetc(ifs)) != EOF) {
    bm = ( bm || pch == '"');
    em = ( em || (bm && ch == ' '));
    bu = ( bu || (em && pch == ' '));
    eu = ( eu || (bu && ch == ' '));
    if ( bm && !em ) { 
      *p++ = ch;
    }
    if ( (bu && !eu) ) {
      fputc (ch, ofs);
    }
    pch = ch;
    if (ch == '\n') {
      *p = '\0';
      fputs(" method=",ofs);
      fputs (method, ofs);
      bm = em = bu = eu = FALSE;
      fputc(ch, ofs);
      p = method;
      pch = '*';
    }
  }
  
  fclose (ifs);
  fclose (ofs);
  return(0);
}
