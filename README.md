# Simple-Compiled-Text
A simple and straightfoward way to handle strings files

Version 1 (October 25, 2018): 

  In order to avoid file text parsing at Run-Time when dealing with the extraction of strings from files for example to avoid embedding text lines in the code i came with this very simple idea and implementation: Texts are preprocessed offline to generate a binary file that can be loaded quickly at run-time.
  
Text File Text Format:
The plain text file from where the tool will generate the binary must have its strings in the next format:
<AaBbCcDdEe/>
The tool will proccess the data between "<" and "/>".
As there is not any kind of identification for each string, the first string in the file should be considered the "string[0]", the next "string[1]" and so. It can also be comments in the file, you comments can be used to indentify the texts:
0. <AaBbCcDdEe/>
1. <AAABBBCCCDDDEEE/>
2. <aabbccddee/>
Data outside de "<" and "/>" boundaries are ignored.
"/>" is a reserved symbol, so texts shouldn't have any "/>".

Enconding Tool:
The tool have very simple usage:
By just entering the files paths it will generate the respectives binary files, compiling all its processable strings all together in a single file (.stc).
stc file1.txt file2.txt file3.txt
By entering the flag -d, it will decode the entered binary file, generating its plain text counterpart (.stcdec).
stc -d file1.sct file2.sct file3.sct

The Binary file (.sct):
Once generated, this file will containt all the processed strings plus their size (in bytes) from the file text and can be decoded to extract its data.

Decoding API (sctdec.h):
Using the API to decode the .sct file fills Data structures that contains the strings and its buffer sizes.
SCT_Decode_file will fill a SCT_Decode_Struct structure with data.
SCT_Decode_Struct contains the number of strings extracted and an array of Strings (SCT_String_Struct).
SCT_String_Struct contains the size (in bytes) of the string and the array of Chars that is the string itself.
Once loaded the binary, strings can be accessed using the function SCT_Get_String.
Then the structure can be cleaned by SCT_Decode_clear or even destroyed if it was dynamically allocated by SCT_Decode_free
The strings are extracted in FIFO order, and with no identification at all, so it is important to keep a record of the strings order (decoding the file using the tool can help to figure it).
