//This file is used when compiling on Windows
#ifndef GETOPT_H
#define GETOPT_H
extern char *optarg;
int getopt(int nargc, char * const nargv[], const char *ostr) ;
#endif