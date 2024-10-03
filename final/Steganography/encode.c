/*
Name = Mohammaed suhan
Date = 01-10-2024
Description = LSB Steganography
*/
#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"


/* Function Definitions */
// Function to read and validate the command line arguments
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
if(strcmp(strstr(argv[2],"."),".bmp")==0) // checking the 3rd argument is bitmap file or not
{
encInfo -> src_image_fname=argv[2]; // stores the bitmap file name
}
else
{
	return e_failure;
}

if(strcmp(strstr(argv[3],"."),".txt")==0)
{	
encInfo -> secret_fname=argv[3];
strcpy(encInfo->extn_secret_file,strstr(argv[3],"."));
}
else if(strcmp(strstr(argv[3],"."),".sh")==0)
  {
  encInfo -> secret_fname=argv[3];
  strcpy(encInfo->extn_secret_file,strstr(argv[3],"."));
  }
else if(strcmp(strstr(argv[3],"."),".c")==0)
  {
  encInfo -> secret_fname=argv[3];
  strcpy(encInfo->extn_secret_file,strstr(argv[3],"."));
  }

  else
  return e_failure;
if(argv[4]!=NULL)
{
encInfo -> stego_image_fname=argv[4];  // checking, if the user has given the output file name
}
else
{
encInfo -> stego_image_fname="stego.bmp"; // creat's the default output image,if the user not passed the output image name
}
return e_success;
}
// function to check the capacity of the input bitmap file
Status check_capacity(EncodeInfo *encInfo)
{
	encInfo->image_capacity= get_image_size_for_bmp(encInfo->fptr_src_image);
	fseek(encInfo->fptr_secret,0,SEEK_END);
	encInfo->size_secret_file=ftell(encInfo->fptr_secret);
	if(encInfo->image_capacity > ((strlen("#*")+sizeof(int)+strlen(encInfo->extn_secret_file)+sizeof(int)+encInfo->size_secret_file)*8))
	return e_success;
	else
		return e_failure;
}
/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
  //  printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    //printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* 	
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

    	return e_failure;
    }

      // No failure return e_success
    return e_success;

}
// Function to copy the header from input file to output bitmap file
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
	char str[54];
	rewind(fptr_src_image); // makes the file indicator to indicate to the beginning og the file
	fread(str,sizeof(char),54,fptr_src_image); // reads the 54 bytes of the data from the source file
	fwrite(str,sizeof(char),54,fptr_dest_image); // write the 54 byte data to the output file
	return e_success;
}
// Function to encode the size of the files
 Status encode_size_to_lsb(int size, char *image_buffer)
{
	for(int i=0;i<32;i++)
	{
	image_buffer[i]=((size & (1<<(31-i)))>>(31-i)) | (image_buffer[i] & 0xFE);
	}
}
// Function to encode 1 byte data to 8 byte data by lsb encode method
 Status encode_byte_to_lsb(char data, char *image_buffer)
{
	for(int i=0;i<8;i++)
	{
	image_buffer[i]=((data & (1<<(7-i)))>>(7-i)) | (image_buffer[i] & 0xFE);
	}
}
//function to encode the data
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
	char str[8]; 
	for(int i=0;i<size;i++)
	{
	fread(str,sizeof(char),8,fptr_src_image); // reads the 8byte data from the input bitmap file
	encode_byte_to_lsb(data[i],str);    // function call to encode data into lsb
	fwrite(str,sizeof(char),8,fptr_stego_image); // write the encode data into the output file
	}
}
// function to encode the magic string
Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{ 
 	encode_data_to_image(magic_string,strlen(magic_string),encInfo->fptr_src_image,encInfo->fptr_stego_image);
	return e_success;
}
// Function to encode secret file extension size
Status encode_secret_file_extn_size(int extn_size, EncodeInfo *encInfo)
{		
	char str[32];  
	fread(str,sizeof(char),32,encInfo->fptr_src_image);  // reads 32 bytes as the size is in integer, which is 4byte
 	encode_size_to_lsb(extn_size,str);
	fwrite(str,sizeof(char),32,encInfo->fptr_stego_image); // writes the encoded bytes to the output file
	return e_success;
} 
// function to encode the secret file extension
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
 encode_data_to_image(file_extn,strlen(file_extn),encInfo->fptr_src_image, encInfo->fptr_stego_image);
 return e_success;
}
// function to encode the secret file size
Status encode_secret_file_size(int file_size, EncodeInfo *encInfo)
{
	char str[32];
	fread(str,sizeof(char),32,encInfo->fptr_src_image);
	encode_size_to_lsb(file_size, str);
	fwrite(str,sizeof(char),32,encInfo->fptr_stego_image);
	return e_success;
}
// function to encode the secret file data
 Status encode_secret_file_data(EncodeInfo *encInfo)
{
 	char secret_data[encInfo->size_secret_file];
   	rewind(encInfo->fptr_secret);
	fread(secret_data,sizeof(char),encInfo->size_secret_file,encInfo->fptr_secret);
	encode_data_to_image(secret_data,encInfo->size_secret_file,encInfo->fptr_src_image,encInfo->fptr_stego_image);
	return e_success;
}
// function to copy the remaining data to the output file
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
	char ch;
	while(fread(&ch,1,1,fptr_src)>0)  // reads the bytes from the source file till End Of File
		fwrite(&ch,1,1,fptr_dest);  // write the readed data to the ouptut file
	return e_success;
}

// function for the Encoding process
Status do_encoding(EncodeInfo *encInfo)
{
	printf("## Encoding Procedure Started ##\n");
	printf("INFO: Checking for secret.txt size\n");
	if(open_files(encInfo)==e_success) // checking the required files are opened or not
	{
	printf("INFO: Not Empty\n");
	}
	else
	{
	printf("INFO: files are not opened successsfully\n");
	return e_failure;
	}
	printf("INFO: Checking for SkeletonCode/beautiful.bmp capacity to handle secret.txt\n");
	if(check_capacity(encInfo)==e_success) // checks for the image capacity to encode
	{
	printf("INFO: Done. Found OK\n");
	}
	else
	{
	printf("INFO: failed to check the capacity\n");
	return e_failure;
	}
	printf("INFO: Copying Image Header\n");
	if(copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image)==e_success)  // check for status of copying the header data
	{
	printf("INFO: Done\n");
	}
	else
	{
	 printf("INFO: failed to copy header bytes\n");
	return e_failure;
	}
	printf("INFO: Encoding Magic String\n");
	if(encode_magic_string(MAGIC_STRING,encInfo)==e_success) // check for the status of the encoding of the magic string
	{
	printf("INFO: Done\n");
	}		
	else
	{
	printf("INFO: failed to encode magic string succesfully\n");	
	return e_failure;
	}
	/*
	if(encode_secret_file_extn_size(strlen(encInfo->extn_secret_file),encInfo)==e_success) // checks status of the secret file exten size encoding
	{
		printf("encoded sec file extn size successfully\n");
	}
	else
	{	
		printf("failed to encode sec file extn size\n");
		return e_failure;
	}
	*/	
	printf("INFO: Encoding secret.txt File Extension\n");
  	if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo)==e_success)  // checks status of the secret file exten encoding			
	{
	printf("INFO: Done\n");
	}
	else
	{
	printf("INFO: failed to encode sec file extn\n");
	return e_failure;
	}
	printf("INFO: Encoding secret.txt File size\n");
	if(encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_success)  // checks status of the secret file size encoding
	{
	printf("INFO: Done\n");
	}
	else
	{
	printf("INFO: failed to encode secret file size\n");
	return e_failure;
	}

	printf("INFO: Encoding secret.txt File Data\n");
	if(encode_secret_file_data(encInfo)==e_success)  // checks status of the secret data encoding
	{
	printf("INFO: Done\n");
	}
	else
	{
	printf("INFO: failed to encode secret file data\n");
	return e_failure;
	}

	printf("INFO: Copying Left Over Data\n");
	if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)  // checks status of the copying the ramaining data function
	{
	printf("INFO: Done\n");
	}
	else
	{
	printf("INFO: failed to copy remaining data\n");
	return e_failure;
	}
	

	return e_success;
}
