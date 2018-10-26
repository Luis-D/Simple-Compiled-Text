/*
	Código fuente para Compilar archivos con Texto Compilado Simple
	"Simple Compiled Text" (.sct)
 	Consiste en un archivo binario que contiene los textos sin cifrado alguno
 	El archivo es creado a partir de un archivo de texto plano
	 El binario compilado sólo guarda los "strings", sin ninguna ID
    	- Luis Delgado. 24 de Octubre. 2018

	Source code of the Simple Compiled Text tool (.sct)
	A .sct file consist on a binary file that contains the texts without any cypher at all
	This file is created from a plain text file
	The generated binary only contains the strings with no ID for them.
*/



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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sctdec.h"
#define ReadLineBuffer 256;

#define DecExtension "sctdec"

/*-----------------------------------------------------------------------------
 *  Leer el archivo entero en modo de texto (Hay un error en algún lado)
 *-----------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 *  Read the entire file in text mode (I think there is an error somewhere)
 *-----------------------------------------------------------------------------*/

char * ReadFullText(FILE * file, int * String_Lenght_Return)
{
    char * DynamicString=NULL;
      *String_Lenght_Return=0;

    char fieof = !feof(file);

    if(fieof)
    {
        char eor=0;
        int maxl = ReadLineBuffer;
        int buffsize = maxl;
        int leng, LENG = 0;
        int index =0;

        DynamicString =(char*) malloc(maxl * sizeof(char));
        do
        {
            fgets (DynamicString+index , buffsize , file);
            fieof = !feof(file);
            eor=1;
            leng = strlen(DynamicString+index)-1;
            if(leng >= buffsize-1 || fieof)
            {
                maxl*=2;
                DynamicString = (char*) realloc(DynamicString,maxl);
                eor=0;
                index+=leng+1;
            }

            LENG += leng;

        }
        while(eor==0);
        *String_Lenght_Return = LENG+4;
}

    return DynamicString;
}


/*-----------------------------------------------------------------------------
 *  Structs
 *-----------------------------------------------------------------------------*/

typedef SCT_Header_Struct Header;

struct Strings_LL_ 
{
    int SizeofString; // + \0
    char * String;
    struct Strings_LL_ *Next;
};
typedef struct Strings_LL_ Strings_LL;

/*-----------------------------------------------------------------------------
 *  Main
 *-----------------------------------------------------------------------------*/

int main(int argc, char * argv[])
{
    char Extension [] = EXT;
    char STCVer [] = VER;
    int ExtL = strlen(Extension)+1;
	int DecExtL = strlen(DecExtension)+1;

    if(argc>1)
    {	
		FILE * file=NULL; FILE * out=NULL;	
		int i = 1;

		int FLAG=0;

		for(;i<argc;i++)
		{

			char * Fname = argv[i];

			if(!strcmp(Fname,"-d"))
			{
			FLAG |= 0x01; continue; 
			}

			file = fopen(argv[i],"r");
			if(file == NULL){printf("ERROR: %s ??\n",Fname);continue;}	    
			
			int TextCounter = 0;
			Strings_LL * Strings = NULL;
			Strings_LL * Exp;
			Strings_LL * Cur = Strings;

			int tmplen = strlen(Fname)+1;
			char * Output = Fname+tmplen-1;
			for(;Output >= Fname;Output--)
			{if(*Output == '.'){break;}tmplen--;}

			/*COMP*/
			if((FLAG & 0x01) != 0x01)
			{ 

				int FullLenght;
				char * FullText = ReadFullText(file,&FullLenght);
				char * Start = FullText;
				char * End = FullText+FullLenght;
				char * tmp; 
				int tmpLeng; 
		
				for(;FullText<End;FullText++)
				{	//	printf("%c",*FullText);
		
					if(*FullText == '<')
					{
						tmpLeng = 0;
						FullText++;
						tmp = FullText;

						while(FullText<End && *FullText != '/' && *(FullText+1) != '>')
						{
						//printf("%c",*FullText);
						tmpLeng++;
						FullText++;	
						}
						
						if(tmpLeng > 0)
						{
						tmpLeng++;
						Exp = (Strings_LL*) malloc (sizeof(Strings_LL));
						Exp->Next = NULL;Exp->SizeofString = tmpLeng; 
						Exp->String = (char*) malloc(sizeof(char) * tmpLeng);
						memcpy(Exp->String,tmp,tmpLeng-1);			
						Exp->String[tmpLeng-1] = 0;	
						//printf("%s\n",Exp->String);
				
						if(TextCounter == 0)
						{
							Strings = Exp;
							Cur = Strings;
						}
						else
						{
							Cur->Next= Exp;
							Cur = Cur->Next;
						}
			
						
						TextCounter++;
						}
					}

				}

				if(TextCounter > 0)
				{
				
				Output = (char*) malloc (tmplen+ExtL); 
				memcpy(Output,Fname,tmplen);	
				memcpy(Output+tmplen,Extension,ExtL);

				out = fopen(Output,"wb");
				if(out == NULL){printf("ERROR: %s\n",Output);continue;}

				Header H; 
				memcpy(&H.SCTVer,STCVer,4);
				H.StringsCount = TextCounter;	

				fwrite(&H,8,1,out);

				Exp = Strings;
				while(Exp != NULL)
				{
					fwrite(&Exp->SizeofString,4,1,out);
					fwrite(Exp->String,Exp->SizeofString,1,out);		    
					
				//    printf("%s\n",Exp->String);
					Cur = Exp;
					Exp = Exp->Next;
					free(Cur);
				}

				fclose(out); 
				}
								
				free(Start);	    
			}
			else /*DEC*/
			{
				SCT_Decode_Struct Decoded;

				if(SCT_Decode_file(&Decoded,Fname) != 1){printf("ERROR @ %s\n",Fname);continue;}

				Output = (char*) malloc (tmplen+DecExtL); 
				memcpy(Output,Fname,tmplen);	
				memcpy(Output+tmplen,DecExtension,DecExtL);
				
				out = fopen(Output,"w");
				if(out == NULL){printf("ERROR: DEC > %s\n",Output);continue;}

				for(int i = 0;i < Decoded.StringsCount;i++)
				{
					fprintf(out,"%d.<%s/>\n",i,Decoded.Strings[i].Buffer);
				}

				fclose(out);
				SCT_Decode_clear(&Decoded);		
			}

			
			if(file!=NULL){fclose(file);}
		
			
		}	
	
    }
    else
    {
	printf("Luis Delgado. 2018\n");
	printf("ToSCT\n");
	printf("Format:\n");
	printf("#(0,INF). <AaBbCcDd/>\n");
	printf("Usage: \n");
	printf("ToSCT \"File1\" \"File2\" \"File3\" \"...\"\n");
	printf("ToSCT -d \"File1.sct\" \"File2.sct\" \"...\"\n");
    }
	
    return 1;
}
