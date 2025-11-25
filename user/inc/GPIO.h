#ifndef __GPIO_H__
#define __GPIO_H__

void LED_Init(void);
void Delay(unsigned int nCount);
void LED_Turn(int id);
void KEY_Init(void);
u8 KEY_Scan(void);

extern int key_map[16];
extern int input_key;
extern bool input_key_flag ;
extern int KEYBOARD_cnt;

void KEYBOARD_Init(void);
int which_key(int high,int low);
char KEYBOARD_Scan(void);

extern int digit_map[17];
extern int digit_pos;
extern int digit[4];

void DIGIT_Init(void);
void DIGIT_display(int digit,int position);

#endif
