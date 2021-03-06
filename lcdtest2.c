 
#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <linux/i2c-dev.h>  
#include <fcntl.h>  
#include <sys/ioctl.h>  
  
  
  
  
#define I2C_BUS    "/dev/i2c-2" // I2C bus device on a Raspberry Pi 3  
#define I2C_ADDR    0x27         // I2C slave address for the LCD module  
#define NUMBER_OF_CHAR_PER_LINE 16  
#define NUMBER_OF_LINE 2  
int lcd_backlight;  
int debug;  
char address;  
char out[4];  
int i2cFile;  
int cursor_pos;  
  
  
  
  
  
  
  
  
void i2c_start() {  
   if((i2cFile = open(I2C_BUS, O_RDWR)) < 0) {  
      printf("Error failed to open I2C bus [%s].\n", I2C_BUS);  
      exit(-1);  
   }  
   // set the I2C slave address for all subsequent I2C device transfers  
   if (ioctl(i2cFile, I2C_SLAVE, I2C_ADDR) < 0) {  
      printf("Error failed to set I2C address [%s].\n", I2C_ADDR);  
      exit(-1);  
   }  
}  
  
  
void i2c_stop() 
{ 
	close(i2cFile);
	 }   
  
  
char convert_to_nibbels(char foo) 
{   
  
char n1,n2,ln1,ln2,mask1,mask2,out1,out2,out3,out4;  
  
  
ln1 = 0b11111101;  
ln2 = 0b11111001;  
  
  
mask1 = 0b11110000;  
mask2 = 0b00001111;  
  
  
n1 = foo & mask1;  
n1 = n1 | mask2;  
//printf("n1 is : 0x%x \r\n",n1);  
  
  
n2 = foo & mask2;  
n2 = n2<<4;        
n2 = n2 | mask2;  
//printf("n2 is : 0x%x \r\n",n2);  
    
out1 = n1 & ln1 ;  
//printf("out1 is : 0x%x \r\n",out1);  
out2 = n1 & ln2;  
//printf("out2 is : 0x%x \r\n",out2);  
  
  
out3 = n2 & ln1 ;  
//printf("out3 is : 0x%x \r\n",out3);  
out4 = n2 & ln2 ;  
//printf("out4 is : 0x%x \r\n",out4);  
out[0] = out1;  
out[1] = out2;  
out[2] = out3;  
out[3] = out4;  
  
}  


char convert_cmd_to_nibbels(char foo) 

{   
  
char n1,n2,ln1,ln2,mask1,mask2,out1,out2,out3,out4;  
  
  
ln1 = 0b11111100;  
ln2 = 0b11111000;  
  
  
mask1 = 0b11110000;  
mask2 = 0b00001111;  
  
  
n1 = foo & mask1;  
n1 = n1 | mask2;  
//printf("n1 is : 0x%x \r\n",n1);  
  
  
n2 = foo & mask2;  
n2 = n2<<4;        
n2 = n2 | mask2;  
//printf("n2 is : 0x%x \r\n",n2);  
  
  
  
  
  
  
out1 = n1 & ln1 ;  
//printf("out1 is : 0x%x \r\n",out1);  
out2 = n1 & ln2;  
//printf("out2 is : 0x%x \r\n",out2);  
  
  
out3 = n2 & ln1 ;  
//printf("out3 is : 0x%x \r\n",out3);  
out4 = n2 & ln2 ;  
//printf("out4 is : 0x%x \r\n",out4);  
out[0] = out1;  
out[1] = out2;  
out[2] = out3;  
out[3] = out4;  
  
}  
  
   
  
void i2c_send_byte(unsigned char data) 
{  
   unsigned char byte[1];  
   byte[0] = data;  
   write(i2cFile, byte, sizeof(byte));   
   /* -------------------------------------------------------------------- * 
    * Below wait creates 1msec delay, needed by display to catch commands  * 
    * -------------------------------------------------------------------- */  
   usleep(1000);  
}  
  
  
void clear_display()  
{  
   /* -------------------------------------------------------------------- * 
    * Display clear, cursor home                                           * 
    * -------------------------------------------------------------------- */  
   usleep(40);                // wait 40usec  
   i2c_send_byte(0b00000100); //  
   i2c_send_byte(0b00000000); // D7-D4=0  
   i2c_send_byte(0b00010100); //  
   i2c_send_byte(0b00010000); // D0=display_clear  
  
  
   /* -------------------------------------------------------------------- * 
    * Turn on the display                                                  * 
    * -------------------------------------------------------------------- */  
   usleep(40);                // wait 40usec  
   i2c_send_byte(0b00001100); //  
   i2c_send_byte(0b00001000); // D7-D4=0  
   i2c_send_byte(0b11101100); //  
   i2c_send_byte(0b11101000); // D3=1 D2=display_on, D1=cursor_on, D0=cursor_blink  
   cursor_pos =1 ;  
}  
  
  
void send_cmd_char_to_lcd(char foo)  
{  
int i;  
convert_cmd_to_nibbels(foo);  
for(i=0;i<4;i++)  
{  
   i2c_send_byte(out[i]);  
}  
}  
  
  
void go_to(int line, char position)  
{  
if(line==1)  
{  
cursor_pos = position;  
position = 0x80+position;  
}  
else if(line==2)  
{  
cursor_pos = position+40;  
position = 0xC0+position;  
}  
send_cmd_char_to_lcd(position);  
}  
  
  
void send_char_to_lcd(char foo)  
{  
int i;  
convert_to_nibbels(foo);  
for(i=0;i<4;i++)  
{  
   i2c_send_byte(out[i]);  
}  
   cursor_pos++;  
}  
  
  
void send_string_to_lcd(int argc,char *str[])  
{  
  
  
int i, counter;  
for(counter=1;counter<argc;counter++)  
{  
for(i=0;str[counter][i]!=0x00;i++)  
{   
   send_char_to_lcd(str[counter][i]);  
   if(cursor_pos==(NUMBER_OF_CHAR_PER_LINE+1))  
{  
   go_to(2,0);    
}  
}  
}  
}  
  
  
void send_simple_string_to_lcd(char *str)  
{  
  
  
int i;  
for(i=0;(str[i]!=0x00);i++)  
{  
    send_char_to_lcd(str[i]);  
}  
  
  
}  
  
  
  
  
void main() {   
   i2c_start();   
   debug=1;  
   cursor_pos =1 ;  
  
  
  
   /* -------------------------------------------------------------------- * 
    * Initialize the display, using the 4-bit mode initialization sequence * 
    * -------------------------------------------------------------------- */  
  // if(debug) printf("Init Start:\n");  
  // if(debug) printf("D7 D6 D5 D4 BL EN RW RS\n");  
  
  
  
  
   sleep(0.4);             // wait 40msec  
   i2c_send_byte(0b00110100); // D7=0, D6=0, D5=1, D4=1, RS,RW=0 EN=1  
   i2c_send_byte(0b00110000); // D7=0, D6=0, D5=1, D4=1, RS,RW=0 EN=0  
  
  
   sleep(0.1);              // wait 10msec  
   i2c_send_byte(0b00110100); //   
   i2c_send_byte(0b00110000); // same  
   sleep(0.1);               // wait 10msec  
   i2c_send_byte(0b00110100); //  
   i2c_send_byte(0b00110000); // 8-bit mode init complete  
   sleep(0.1);              // wait 10msec  
   i2c_send_byte(0b00100100); //  
   i2c_send_byte(0b00100000); // switched now to 4-bit mode  
  
  
  
  
   /* -------------------------------------------------------------------- * 
    * 4-bit mode initialization complete. Now configuring the function set * 
    * -------------------------------------------------------------------- */  
   usleep(100);                // wait 100usec  
   i2c_send_byte(0b00100100); //  
   i2c_send_byte(0b00100000); // keep 4-bit mode  
   i2c_send_byte(0b10000100); //  
   i2c_send_byte(0b10000000); // D3=2lines, D2=char5x8  
  
  
   /* -------------------------------------------------------------------- * 
    * Next turn display off                                                * 
    * -------------------------------------------------------------------- */  
   usleep(100);                // wait 100usec  
   i2c_send_byte(0b00000100); //  
   i2c_send_byte(0b00000000); // D7-D4=0  
   i2c_send_byte(0b10000100); //  
   i2c_send_byte(0b10000000); // D3=1 D2=display_off, D1=cursor_off, D0=cursor_blink  
  
  
   /* -------------------------------------------------------------------- * 
    * Display clear, cursor home                                           * 
    * -------------------------------------------------------------------- */  
   usleep(100);                // wait 100usec  
   i2c_send_byte(0b00000100); //  
   i2c_send_byte(0b00000000); // D7-D4=0  
   i2c_send_byte(0b00010100); //  
   i2c_send_byte(0b00010000); // D0=display_clear  
  
  
   /* -------------------------------------------------------------------- * 
    * Set cursor direction                                                 * 
    * -------------------------------------------------------------------- */  
   usleep(100);                // wait 100usec  
   i2c_send_byte(0b00000100); //  
   i2c_send_byte(0b00000000); // D7-D4=0  
   i2c_send_byte(0b01100100); //  
   i2c_send_byte(0b01100000); // print left to right  
  
  
   /* -------------------------------------------------------------------- * 
    * Turn on the display                                                  * 
    * -------------------------------------------------------------------- */  
   usleep(100);                // wait 100usec  
   i2c_send_byte(0b00001100); //  
   i2c_send_byte(0b00001000); // D7-D4=0  
   i2c_send_byte(0b11101100); //  
   i2c_send_byte(0b11101000); // D3=1 D2=display_on, D1=cursor_on, D0=cursor_blink  
  
  
   sleep(1);  
  
  

  
clear_display();  
  
  
//  send_string_to_lcd(argc,argv);  
//   go_to(2,0);  
//   send_cmd_char_to_lcd(0x0C); // cursor off   
send_simple_string_to_lcd("HELLO ");  
go_to(2,0);  
send_simple_string_to_lcd("MLX ");  
    
  
  
 i2c_stop();   
} 
