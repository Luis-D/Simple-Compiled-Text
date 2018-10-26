/* 
	Header para decodificación de archivos de texto SCT (.sct)
	El archivo se lee y se obtiene un arreglo con estructuras con los textos y sus tamaños.
 	- Luis Delgado. 24 de Octubre. 2018

    	Header to read the .sct files
	The file is read and structures with the texts and their sizes are obtained
*/

/*sctdec.h*/

/*
MIT License

Copyright (c) 2018 Luis Delgado

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef _INCL_SCT_DEC_H_
#define _INCL_SCT_DEC_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

 #define VER "SCT1"
 #define EXT "sct"

typedef struct SCT_String_Struct
{
    int Size;
    char * Buffer;
} SCT_String_Struct;

typedef struct SCT_Decode_Struct
{
    int StringsCount;
    SCT_String_Struct * Strings;

}SCT_Decode_Struct;

typedef struct SCT_Header_Struct
{
      char SCTVer[4];
      int StringsCount; 
}SCT_Header_Struct; 







/*-----------------------------------------------------------------------------
 *  FUNC DECL
 *-----------------------------------------------------------------------------*/

void SCT_Decode_clear(SCT_Decode_Struct * _SCT_Decode_Struct);
void SCT_Decode_free(SCT_Decode_Struct * _SCT_Decode_Struct);
int SCT_Decode_file(SCT_Decode_Struct * _SCT_Decode_Struct,const char * File_Path);
char * SCT_Get_String(SCT_Decode_Struct * _SCT_Decode_Struct,int String_Number, int * BufferSize);






/*-----------------------------------------------------------------------------
 *  FUNC DEF
 *-----------------------------------------------------------------------------*/

void SCT_Decode_clear(SCT_Decode_Struct * _SCT_Decode_Struct)
{ 
    SCT_String_Struct* STR = _SCT_Decode_Struct->Strings;
    SCT_String_Struct* END = STR+_SCT_Decode_Struct->StringsCount;
     _SCT_Decode_Struct->StringsCount = 0;

    for(;STR<END;STR++)
    {
	free(STR->Buffer);
    } 

    free(_SCT_Decode_Struct->Strings);
}

void SCT_Decode_free(SCT_Decode_Struct * _SCT_Decode_Struct)
{
    SCT_Decode_clear(_SCT_Decode_Struct);
    free(_SCT_Decode_Struct);
}

int SCT_Decode_file(SCT_Decode_Struct * _SCT_Decode_Struct,const char * File_Path)
{
    FILE * file = fopen(File_Path,"rb");
    if(file == NULL){return -1;}

    SCT_Header_Struct H;
    
    fread(&H,8,1,file);

    char CHKVER [5] = {0,0,0,0,0};memcpy(CHKVER,&H.SCTVer,4);
    if(strcmp(CHKVER,VER)){return 0;} 

    _SCT_Decode_Struct->StringsCount = H.StringsCount;
    _SCT_Decode_Struct->Strings = (SCT_String_Struct *) malloc(sizeof(SCT_String_Struct)*H.StringsCount);
    
    for(int i = 0;i<H.StringsCount;i++)
    {
	SCT_String_Struct * HS = _SCT_Decode_Struct->Strings+i;
	fread(&(HS->Size),4,1,file);
	HS->Buffer = (char *) malloc(HS->Size);
	fread(HS->Buffer, HS->Size,1,file);
    }

    fclose(file);
    return 1;
}

char * SCT_Get_String(SCT_Decode_Struct * _SCT_Decode_Struct,int String_Number, int * BufferSize)
{
	SCT_String_Struct * pointer = _SCT_Decode_Struct->Strings+String_Number;
	*BufferSize = pointer->Size;
	return pointer->Buffer;
}

#endif
