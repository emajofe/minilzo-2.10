/* compress-test.c -- very simple test program for the miniLZO library

   This file is part of the LZO real-time data compression library.

   Copyright (C) 1996-2017 Markus Franz Xaver Johannes Oberhumer
   All Rights Reserved.

   The LZO library is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   The LZO library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the LZO library; see the file COPYING.
   If not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.

   Markus F.X.J. Oberhumer
   <markus@oberhumer.com>
   http://www.oberhumer.com/opensource/lzo/
 */


#include <stdio.h>
#include <stdlib.h>

#include "minilzo.h"

/* We want to compress the file data at 'in' with length 'IN_LEN' to
 * the file data at 'out'. Because the input block may be incompressible,
 * we must provide a little more output space in case that compression
 * is not possible.
 */

#define IN_LEN      (246*32768ul) //around 8Mb
#define OUT_LEN     (IN_LEN + IN_LEN / 16 + 64 + 3)

static unsigned char __LZO_MMODEL in  [ IN_LEN ];
static unsigned char __LZO_MMODEL out [ OUT_LEN ];


/* Work-memory needed for compression. Allocate memory in units
 * of 'lzo_align_t' (instead of 'char') to make sure it is properly aligned.
 */

#define HEAP_ALLOC(var,size) \
    lzo_align_t __LZO_MMODEL var [ ((size) + (sizeof(lzo_align_t) - 1)) / sizeof(lzo_align_t) ]

static HEAP_ALLOC(wrkmem, LZO1X_1_MEM_COMPRESS);

int main(int argc, char *argv[])
{
    int r;
    lzo_uint in_len;
    lzo_uint out_len;

    size_t bytes_read;

    FILE *fp_in , *fp_out;

    if (argc < 0 && argv == NULL)   // avoid warning about unused args
        return 0;

    printf("\nLZO real-time data compression library (v%s, %s).\n\n",
           lzo_version_string(), lzo_version_date());

/*
 * Initialize the LZO library
 */
    if (lzo_init() != LZO_E_OK)
    {
        printf("Internal error - lzo_init() failed !!!\n");
        printf("(This usually indicates a compiler bug - try recompiling\nwithout optimizations, and enable '-DLZO_DEBUG' for diagnostics)\n");
        return 3;
    }

	switch(argv[1][0])
    {
        case 'c':
            /*
             * Open file in binary mode (COMPRESS from this file).
             */
            fp_in=fopen(argv[2],"rb");
            if (!fp_in) 
            {
                fprintf(stdout, "Error: Unable to open file '%s'\n", argv[2]);
                return 1;
            } 
            else 
            {
                fprintf(stdout, "Success opening the file '%s'\n", argv[2]);
            }
            
            /*
             * Open file in binary mode (COMPRESS to this file).
             */
            fp_out=fopen(argv[3],"wb");
            if (!fp_out) 
            {
                fprintf(stdout, "Error: Unable to open file '%s'\n", argv[3]);
                return 1;
            } 
            else 
            {
                fprintf(stdout, "Success opening the file '%s'\n", argv[3]);
            }

            in_len = IN_LEN;
            bytes_read = fread(in,1,in_len,fp_in);

            /*
             * Compress from 'in' to 'out' with LZO1X-1
             */
            r = lzo1x_1_compress(in,bytes_read,out,&out_len,wrkmem);

            if (r == LZO_E_OK) 
            {
                printf("\n");
                printf("Read %lu bytes from the file '%s'\n", (1*bytes_read), argv[2]);
                printf("Compressed %lu bytes into %lu bytes\n",
                    (unsigned long) bytes_read, (unsigned long) out_len);
                
                /*
                * Write compressed data to the 'out' file
                */
                fwrite(out,1,out_len,fp_out);

                fclose(fp_in);
                fclose(fp_out);
            }
            else
            {
                // this should NEVER happen
                printf("Internal error - compression failed: %d\n", r);
                fclose(fp_in);
                fclose(fp_out);
                return 2;
            }
            /* check for an incompressible block */
            if (out_len >= in_len)
            {
                printf("This file contains incompressible data.\n");
                return 0;
            }

            printf("\nminiLZO simple compression test passed.\n");
            break;
        case 'd':
            /*
             * Open file in binary mode (DECOMPRESS from this file).
             */
            fp_in=fopen(argv[2],"rb");
            if (!fp_in) 
            {
                fprintf(stdout, "Error: Unable to open file '%s'\n", argv[2]);
                return 1;
            } 
            else 
            {
                fprintf(stdout, "Success opening the file '%s'\n", argv[2]);
            }
            
            /*
             * Open file in binary mode (DECOMPRESS to this file).
             */
            fp_out=fopen(argv[3],"wb");
            if (!fp_out) 
            {
                fprintf(stdout, "Error: Unable to open file '%s'\n", argv[3]);
                return 1;
            } 
            else 
            {
                fprintf(stdout, "Success opening the file '%s'\n", argv[3]);
            }

            in_len = IN_LEN;
            bytes_read = fread(in,1,in_len,fp_in);

            /*
             * Decompress from 'in' to 'out' with LZO1X-1
             */
            r = lzo1x_decompress(in,bytes_read,out,&out_len,wrkmem);

            if (r == LZO_E_OK) 
            {
                printf("\n");
                printf("Read %lu bytes from the file '%s'\n", (1*bytes_read), argv[2]);
                printf("Decompressed %lu bytes back into %lu bytes\n",
                    (unsigned long) bytes_read, (unsigned long) out_len);
                
                /*
                * Write decompressed data to the 'out' file
                */
                fwrite(out,1,out_len,fp_out);

                fclose(fp_in);
                fclose(fp_out);
            }
            else
            {
                // this should NEVER happen
                printf("Internal error - decompression failed: %d\n", r);
                fclose(fp_in);
                fclose(fp_out);
                return 1;
            }
            printf("\nminiLZO simple decompression test passed.\n");
            break;
        default:break;
	}

    return 0;
}
