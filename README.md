The art and science of hiding information by embedding messages within other, seemingly harmless messages. Bits of unused data are replaced by bits of valuable information using LSB mechanism. Sender and receiver will have individual key / secret based on which they will be able to extract the actual data from the image. This project also gives basic level understanding of image processing methodologies.


To Run the code
First Save in the system and run using the command by gcc *.c 
For encoding pass the argument as ./a.out -e beautiful.bmp secret.txt stego.bmp
For decoding pass the argument as ./a.out -d stego.bmp
