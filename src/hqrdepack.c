/** @file hqrdepack.c
	@brief 
	This file contains High Quality Resource (HQR) decompress routines.
	
	Prequengine: a Little Big Adventure engine
	
	Copyright (C) 2008 Prequengine team \n
	Copyright (C) 2002-2007 The TwinEngine team \n

	This program is free software; you can redistribute it and/or \n
	modify it under the terms of the GNU General Public License \n
	as published by the Free Software Foundation; either version 2 \n
	of the License, or (at your option) any later version. \n

	This program is distributed in the hope that it will be useful, \n
	but WITHOUT ANY WARRANTY; without even the implied warranty of \n
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the \n
	GNU General Public License for more details. \n

	You should have received a copy of the GNU General Public License \n
	along with this program; if not, write to the Free Software \n
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hqrdepack.h"
#include "filereader.h"

filereader fr;

/** Decompress entry based in Yaz0r and Zink decompression code
	@param dst destination pointer where will be the decompressed entry
	@param src compressed data pointer
	@decompsize real file size after decompression
	@mode compression mode used */
void hqr_decompress_entry(unsigned char * dst, unsigned char * src, int decompsize, int mode)
{
	unsigned char b;
	int lenght, d,i;
 	unsigned short int offset;
	unsigned char *ptr;

	do{
		b = *(src++);
		for(d=0; d < 8; d++)
		{
			if(!(b & (1 << d)))
			{
				offset = *(unsigned short int*)(src);
				src+=2;
				lenght = (offset & 0x0F) + (mode+1);
				ptr = dst - (offset >> 4) - 1;
				for(i=0;i<lenght;i++)
					*(dst++)=*(ptr++);
			}
			else
			{
				lenght = 1;
				*(dst++) = *(src++);
			}
			decompsize-=lenght;
			if(decompsize<=0)
				return;
		}
	}while(decompsize);
}

/** Decompress entry based in the original expand lzss lba code
	@param dst destination pointer where will be the decompressed entry
	@param src compressed data pointer
	@decompsize real file size after decompression
	@mode compression mode used */
void hqr_decompress_lz_entry(unsigned char * dst, unsigned char * src, int decompsize, int mode)
{
	unsigned short int offset;
	int lenght;
	unsigned char *ptr;

	while( decompsize > 0 )
	{
       unsigned char bits;
       unsigned char type = *(src++);
       for( bits=1; bits!=0; bits<<=1 )
       {
           if(!(type&bits))
           {
				offset = *(unsigned short int*)(src);
				src+=2;
				lenght = (offset & 0x0F) + (mode+1);
				ptr = dst - (offset >> 4) - 1;
				if(offset == 0) {
					memset(dst, *ptr, lenght);
				}
				else {
					if ((ptr+lenght) >= dst ) {
						signed long n;
						unsigned char *tmp = dst;
						for(n=0; n < lenght; n++)
							*tmp++ = *ptr++;
					}
					else {
						memcpy(dst,ptr,lenght);
					}
				}
				dst += lenght;
           }
		   else
		   {
			   lenght = 1;
               *(dst++) = *(src++);
           }
		   decompsize -= lenght;
           if(decompsize<=0)
               return;
       }
   }
}

/** Get a HQR entry pointer
	@param ptr pointer to save the entry
	@param filename HQR file name
	@param index entry index to extract
	@return entry real size*/
int hqr_get_entry(unsigned char * ptr, char *filename, int index)
{
	unsigned int headerSize;
	unsigned int offsetToData;
	unsigned int realSize;
	unsigned int compSize;
	unsigned short int mode;

	if (!filename)
		return 0;
	
	if(!fropen(&fr,filename))
		printf("HQR: %s can't be found !\n", filename);
 
	frread(&fr,&headerSize,4);

	if((unsigned int)index >= headerSize/4)
	{
		printf("\nHQR WARNING: Invalid entry index!!\n");
		frclose(&fr);
		return 0;
	}

	frseek(&fr,index*4);
	frread(&fr,&offsetToData,4);

	frseek(&fr,offsetToData);
	frread(&fr,&realSize,4);
	frread(&fr,&compSize,4);
	frread(&fr,&mode,2);

	if(!ptr)
		ptr =(unsigned char*)malloc(realSize);

	if(!ptr)
	{
		printf("\nHQR WARNING: Unable to allocate memory!!\n");
		frclose(&fr);
		return 0;
	}

	// uncompressed
	if(mode==0) 
	{
		frread(&fr,ptr,realSize);
	}
	// compressed: modes (1 & 2)
	else 
	{
		unsigned char* compDataPtr=0;
		compDataPtr=(unsigned char*)malloc(compSize);
		frread(&fr,compDataPtr,compSize);
		hqr_decompress_entry(ptr,compDataPtr,realSize,mode);
		free(compDataPtr);
	}

	frclose(&fr);

	return realSize;
}

/** Get a HQR entry pointer
	@param filename HQR file name
	@param index entry index to extract
	@return entry real size */
int hqr_entry_size(char *filename, int index)
{
	unsigned int headerSize;
	unsigned int offsetToData;
	unsigned int realSize;
	//FILE *fd;

	if (!filename)
		return 0;

	if (!fropen(&fr,filename)) {
		printf("HQR: %s can't be found !\n", filename);
		exit(1);
	}
 
	frread(&fr,&headerSize,4);
 
	if((unsigned int)index >= headerSize/4)
	{
		printf("\nHQR WARNING: Invalid entry index!!\n");
		//fclose(fd);
		frclose(&fr);
		return 0;
	}
 
	frseek(&fr,index*4);
	frread(&fr,&offsetToData,4);

	frseek(&fr,offsetToData);
	frread(&fr,&realSize,4);

	frclose(&fr);

	return realSize;
}

/** Get a HQR total number of entries
	@param filename HQR file name
	@return total number of entries */
int hqr_num_entries(char *filename)
{
	unsigned int headerSize;

	if (!filename)
		return 0;

	if (!fropen(&fr,filename)) {
		printf("HQR: %s can't be found !\n", filename);
		exit(1);
	}
 
	frread(&fr,&headerSize,4);
 
	return headerSize/4;
}

/** Get a HQR entry pointer with memory allocation
	@param ptr pointer to save the entry
	@param filename HQR file name
	@param index entry index to extract
	@return entry real size */
int hqr_getalloc_entry(unsigned char ** ptr, char *filename, int index)
{
	int size;
	size = hqr_entry_size(filename,index);
	//if(*ptr)
		//free(ptr);
	*ptr = (unsigned char*)malloc(size*sizeof(unsigned char));
	if(!*ptr)
	{
		printf("HQR WARNING: unable to allocate entry memory!!\n");
		return 0;
	}
	hqr_get_entry(*ptr, filename, index);

	return size;
}
