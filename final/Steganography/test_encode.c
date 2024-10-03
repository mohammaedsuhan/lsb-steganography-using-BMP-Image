/*
Name = Mohammaed suhan
Date = 01-10-2024
Description = LSB Steganography
*/

#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

//#include "secret.txt"

int main(int argc,char *argv[])
{
	int res=check_operation_type(argv);
	EncodeInfo encInfo;
	if(res==e_encode){
	printf("INFO: Opening required files\n");

	printf("INFO: Opend SkeletonCode/beautiful.bmp\n");
	printf("INFO: Opend secret.txt\n");
	printf("INFO: Opend steged_img.bmp\n");
	if((read_and_validate_encode_args(argv, &encInfo))==e_success){
	printf("INFO: Done\n");
	if(do_encoding(&encInfo)==e_success)
	{
	printf("INFO: ## Encoding is Done Successfully ##\n");
	}
	else
	{
	printf("INFO: Encoding is not done successfully\n");
	}
	}
	else{
	printf("INFO: Not Done Opening files\n");
	}
	}
	else if(res==e_decode){
              //structure pointer
        DecodeInfo decInfo;
            //validating input files
         if ( read_and_validate_decode_args( argv, &decInfo ) == e_success )
         {
         
             //function call for decoding
         if ( do_decoding( &decInfo ) == e_success )
         {
         printf( "INFO: ## Decoding Done Succesfully ##\n" );
         }
         else
         {
         printf( "INFO: Failed to decode\n" );
         }
     	}
	}
	else{
	printf("Error:Please pass the arguments\n");
	}
}
 OperationType check_operation_type(char *argv[])
{
	if(strcmp(argv[1],"-e")==0)// condition to check whether user wants to do encoding or decoding
	{  
	return e_encode;
	}
	else if(strcmp(argv[1],"-d")==0)// condition to check whether user wants to do encoding or decoding
	{		
	return e_decode;
	}
	else
	{
	return e_unsupported;   // if user enters either encode or decodd option, then its unsupported
	}
}
