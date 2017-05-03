/*
 ***********************************************************************
 *
 *        @version  1.0
 *        @date     03/11/2014 02:09:32 PM
 *        @author   Fridolin Pokorny <fridex.devel@gmail.com>
 *
 ***********************************************************************
 */

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <getopt.h>
#include <cassert>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

#include "gif2ipl.h"
#include "phash.h"
#include "common.h"
#include <sys/types.h>  
#include <iostream> 

using namespace std;  

int main(int argc, char* argv[])
{

    char* phash = NULL;
    char* filename = argv[1];
    if (access(filename, 2))
        return 1;

    IplImage* pImg = NULL;    
    unsigned char* out_buf = NULL;

    FILE* in_file = fopen(argv[1], "rb");
    char magic[6] = {0};
    fread(magic, 5, 1, in_file);
    rewind(in_file);

    if (memcmp(magic, "GIF", 3) == 0)
    {
        pImg = Gif2IplImage(filename);
        //cout << pImg << endl;
        phash = getphashFormMem(pImg);
    }else{
        phash = getphash(filename);
    }

    if (phash != NULL)
    {
        cout  << phash << endl;
        fclose(in_file);
        return 0;
    }
    free(phash);
    free(out_buf);
    fclose(in_file);
    return 1;
}
