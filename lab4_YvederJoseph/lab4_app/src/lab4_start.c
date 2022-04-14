/* Author: Yveder Joseph
 * Course: ECE3829
 * Project: Lab 4
 * Description: project for Lab 4.
 * Implements two functions
 * 1- reading switches and lighting their corresponding LED
 * 2 - Plays different notes/songs based on the button pressed and switch toggled
 * 3-  Outputs the note being played to the seven segment display
 * 4- If a song is being played a corresponding scrolling message is shown on the seven segment display
 * The songs are the Super Mario theme song and the under water variation
 *
 */


// Header Inclusions
/* x#defines.h set #defines names
 like XPAR_AXI_GPIO_0_DEVICE_ID that are referenced in you code
 each hardware module as a section in this file.
*/
//#include "x#defines.h"
/* each hardware module type as a set commands you can use to
 * configure and access it. xgpio.h defines API commands for your gpio modules
 */
#include "xgpio.h"
/* this defines the recommend types like u32 */
#include "xil_types.h"
#include "xil_printf.h"
#include "xstatus.h"
#include "sleep.h"
#include "xtmrctr.h"

void update_all_seven_seg(u32 dispA,u32 dispB,u32 dispC,u32 dispD,int *update_count,u32 *last_seg_count);
void scrollMessage(int size,u32 message[],int *pos,u32 *last_mes_count,u32 *dispA,u32 *dispB,u32 *dispC,u32 *dispD);
void play_melody(int time,u32 note,u32 *message,int size_message,int i);
void superMario(u32 sw_data);
void play_note(int time,u32 note_select,u32 mode);
void check_switches(u32 *sw_data, u32 *sw_data_old, u32 *sw_changes);
void update_LEDs(u32 led_data);
void  select_notes(u32 sw_data,u32 btn_data, u32 *target_count,u32 *dispA, u32 *dispB);
void  update_seven_seg(u32 dispA,u32 dispB,u32 *last_seg_count);
void check_buttons(u32 *btn_data, u32 *btn_data_old, u32 *btn_changes);
void update_amp2(u32 *amp2_data, u32 target_count, u32 *last_count);


// Block Design Details
/* Timer device ID
 */
#define TMRCTR_DEVICE_ID XPAR_TMRCTR_0_DEVICE_ID
#define TIMER_COUNTER_0 0


/* LED are assigned to GPIO (CH 1) GPIO_0 Device
 * DIP Switches are assigned to GPIO2 (CH 2) GPIO_0 Device
 */
#define GPIO0_ID XPAR_GPIO_0_DEVICE_ID
#define GPIO0_LED_CH 1
#define GPIO0_SW_CH 2
// 16-bits of LED outputs (not tristated)
#define GPIO0_LED_TRI 0x00000000
#define GPIO0_LED_MASK 0x0000FFFF
// 16-bits SW inputs (tristated)
#define GPIO0_SW_TRI 0x0000FFFF
#define GPIO0_SW_MASK 0x0000FFFF

/*  7-SEG Anodes are assigned to GPIO (CH 1) GPIO_1 Device
 *  7-SEG Cathodes are assined to GPIO (CH 2) GPIO_1 Device
 */
#define GPIO1_ID XPAR_GPIO_1_DEVICE_ID
#define GPIO1_ANODE_CH 1
#define GPIO1_CATHODE_CH 2
//4-bits of anode outputs (not tristated)
#define GPIO1_ANODE_TRI 0x00000000
#define GPIO1_ANODE_MASK 0x0000000F
//8-bits of cathode outputs (not tristated)
#define GPIO1_CATHODE_TRI 0x00000000
#define GPIO1_CATHODE_MASK 0x000000FF

// Push buttons are assigned to GPIO (CH_1) GPIO_2 Device
#define GPIO2_ID XPAR_GPIO_2_DEVICE_ID
#define GPIO2_BTN_CH 1
// 4-bits of push button (not tristated)
#define GPIO2_BTN_TRI 0x00000000
#define GPIO2_BTN_MASK 0x0000000F

// AMP2 pins are assigned to GPIO (CH1 1) GPIO_3 device
#define GPIO3_ID XPAR_GPIO_3_DEVICE_ID
#define GPIO3_AMP2_CH 1
#define GPIO3_AMP2_TRI 0xFFFFFFF4
#define GPIO3_AMP2_MASK 0x00000001
//super Mario Notes
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978
//Lab Notes
#define C3  (1.0/(2.0*130.81*10e-9))
#define D3  (1.0/(2.0*146.83*10e-9))
#define E3  (1.0/(2.0*164.81*10e-9))
#define F3  (1.0/(2.0*174.61*10e-9))
#define G3  (1.0/(2.0*196*10e-9));
#define A3  (1.0/(2.0*220*10e-9));
#define B3  (1.0/(2.0*246.94*10e-9))
#define OFF  0
#define C4  (1.0/(2.0*261.63*10e-9))
#define D4  (1.0/(2.0*293.66*10e-9))
#define E4  (1.0/(2.0*329.63*10e-9))
#define F4  (1.0/(2.0*349.23*10e-9))
#define G4  (1.0/(2.0*392*10e-9))
#define A4  (1.0/(2.0*440*10e-9))
#define B4  (1.0/(2.0*493.88*10e-9))
#define C5  (1.0/(2.0*523.25*10e-9))
#define D5  (1.0/(2.0*587.33*10e-9))
//Seven segment constants for displaying characters
#define  one  0b11111001
#define  two  0b10100100
#define  three   0b10110000
#define  four   0b10011001
#define five   0b10010010
#define six  0b10000010
#define seven  0b11111000
#define eight  0b10000000
#define nine   0b10010000
#define A  0b10001000
#define B   0b10000011
#define C   0b11000110
#define  D  0b10100001
#define E   0b10000110
#define F  0b10001110
#define zero  0b11000000
#define U  0b11100000
#define H  0b10001001
#define P  0b10001100
#define L  0b11000111
#define NONE  0b11111111














// Timer Device instance
XTmrCtr TimerCounter;

// GPIO Driver Device
XGpio device0;
XGpio device1;
XGpio device2;
XGpio device3;

// IP Tutorial  Main
int main() {

	u32 btn_data = 0b000;
	u32 btn_data_old = 0b000;
	u32 sw_data = 0;
	u32 sw_data_old = 0;
	u32 dispA = zero;
	u32 dispB = zero;


	// bit[3] = SHUTDOWN_L and bit[1] = GAIN, bit[0] = Audio Input
	u32 amp2_data = 0x8;
	u32 target_count = 0xffffffff;
	u32 sw_changes = 0;
	u32 btn_changes = 0;
	u32 last_count = 0;
	u32 last_seg_count = 0;
	XStatus status;


	//Initialize timer
	status = XTmrCtr_Initialize(&TimerCounter, XPAR_TMRCTR_0_DEVICE_ID);
	if (status != XST_SUCCESS) {
		xil_printf("Initialization Timer failed\n\r");
		return 1;
	}
	//Make sure the timer is working
	status = XTmrCtr_SelfTest(&TimerCounter, TIMER_COUNTER_0);
	if (status != XST_SUCCESS) {
		xil_printf("Initialization Timer failed\n\r");
		return 1;
	}
	//Configure the timer to Autoreload
	XTmrCtr_SetOptions(&TimerCounter, TIMER_COUNTER_0, XTC_AUTO_RELOAD_OPTION);
	//Initialize your timer values
	//Start your timer
	XTmrCtr_Start(&TimerCounter, TIMER_COUNTER_0);



	// Initialize the GPIO devices
	status = XGpio_Initialize(&device0, GPIO0_ID);
	if (status != XST_SUCCESS) {
		xil_printf("Initialization GPIO_0 failed\n\r");
		return 1;
	}
	status = XGpio_Initialize(&device1, GPIO1_ID);
	if (status != XST_SUCCESS) {
		xil_printf("Initialization GPIO_1 failed\n\r");
		return 1;
	}
	status = XGpio_Initialize(&device2, GPIO2_ID);
	if (status != XST_SUCCESS) {
		xil_printf("Initialization GPIO_2 failed\n\r");
		return 1;
	}
	status = XGpio_Initialize(&device3, GPIO3_ID);
	if (status != XST_SUCCESS) {
		xil_printf("Initialization GPIO_3 failed\n\r");
		return 1;
	}

	// Set directions for data ports tristates, '1' for input, '0' for output
	XGpio_SetDataDirection(&device0, GPIO0_LED_CH, GPIO0_LED_TRI);
	XGpio_SetDataDirection(&device0, GPIO0_SW_CH, GPIO0_SW_TRI);
	XGpio_SetDataDirection(&device1, GPIO1_ANODE_CH, GPIO1_ANODE_TRI);
	XGpio_SetDataDirection(&device1, GPIO1_CATHODE_CH, GPIO1_CATHODE_TRI);
	XGpio_SetDataDirection(&device2, GPIO2_BTN_CH, GPIO2_BTN_TRI);
	XGpio_SetDataDirection(&device3, GPIO3_AMP2_CH, GPIO3_AMP2_TRI);

	xil_printf("Demo initialized successfully\n\r");

	XGpio_DiscreteWrite(&device3, GPIO3_AMP2_CH, amp2_data);

	// this loop checks for changes in the input switches
	// if they changed it updates the LED outputs to match the switch values.
	// target_count = (period of sound)/(2*10nsec)), 10nsec is the processor clock frequency
	// example count is middle C (C4) = 191110 count (261.62 Hz)
//target_count = (1.0/(2.0*261.62*10e-9));
	//plays the  opening sequence
	int i;
	for(i = 1; i <= 4;i++){
		play_note(50000000,i,0);
	}
	while (1) {

		check_switches(&sw_data, &sw_data_old, &sw_changes);
		check_buttons(&btn_data, &btn_data_old, &btn_changes);

		if(btn_changes)select_notes(sw_data,btn_data, &target_count,&dispA,&dispB);



		if (sw_changes) update_LEDs(sw_data);
		update_seven_seg(dispA,dispB,&last_seg_count);
		update_amp2(&amp2_data, target_count, &last_count);
	}

}

// reads the value of the input switches and outputs if there were changes from last time
void check_switches(u32 *sw_data, u32 *sw_data_old, u32 *sw_changes) {
	*sw_data = XGpio_DiscreteRead(&device0, GPIO0_SW_CH);
	*sw_data &= GPIO0_SW_MASK;
	*sw_changes = 0;
	if (*sw_data != *sw_data_old) {
		// When any bswitch is toggled, the LED values are updated
		//  and report the state over UART.
		*sw_changes = *sw_data ^ *sw_data_old;
		*sw_data_old = *sw_data;
	}
}
//read input from buttons
void check_buttons(u32 *btn_data, u32 *btn_data_old, u32 *btn_changes){
	*btn_data = XGpio_DiscreteRead(&device2, GPIO2_BTN_CH);
		*btn_data &= GPIO2_BTN_MASK;
		*btn_changes = 0;
		if (*btn_data != *btn_data_old) {
			*btn_changes = *btn_data ^ *btn_data_old;
			*btn_data_old = *btn_data;
		}
	}



// writes the value of led_data to the LED pins
void update_LEDs(u32 led_data) {
	led_data = (led_data) & GPIO0_LED_MASK;
	XGpio_DiscreteWrite(&device0, GPIO0_LED_CH, led_data);
}
//scrolls supplied message across the screen by using a timer
void scrollMessage(int size,u32 message[],int *pos,u32 *last_mes_count,u32 *dispA,u32 *dispB,u32 *dispC,u32 *dispD){
	u32 scroll_rate = 70000000;

	u32 current_count = XTmrCtr_GetValue(&TimerCounter, TIMER_COUNTER_0);
	if((current_count - *last_mes_count) > scroll_rate){
				*pos +=1;
				 *last_mes_count = current_count;
			}
			if(*pos > (size -1) - 3){
				*pos = 0;
			}
	*dispA = message[*pos];
	*dispB = message[*pos+1];
	*dispC = message[*pos+2];
	*dispD = message[*pos+3];
}
//Updates Display C and D of seven segment display
void update_seven_seg(u32 dispA,u32 dispB,u32 *last_seg_count){
	u32 anode_select = 0b1101;
	u32 refresh_rate = 5000;
	u32 disp_info = dispA;
	u32 current_count = XTmrCtr_GetValue(&TimerCounter, TIMER_COUNTER_0);
	if((current_count - *last_seg_count) > refresh_rate){
		 anode_select =  0b1110;
		 disp_info = dispB;
		 *last_seg_count = current_count;

	}

	anode_select = (anode_select) &  GPIO1_ANODE_MASK;
	disp_info = (disp_info) &  GPIO1_CATHODE_MASK;
	XGpio_DiscreteWrite(&device1, GPIO1_ANODE_CH, anode_select);
	XGpio_DiscreteWrite(&device1, GPIO1_CATHODE_CH, disp_info);


}
//Turns all seven segment displays on
void update_all_seven_seg(u32 dispA,u32 dispB,u32 dispC,u32 dispD,int *update_count,u32 *last_seg_count){
		u32 anode_select = 0;
		u32 refresh_rate = 5000;
		u32 disp_info = dispA;
		u32 current_count = XTmrCtr_GetValue(&TimerCounter, TIMER_COUNTER_0);
		if((current_count - *last_seg_count) > refresh_rate){
			*update_count +=1;
			*last_seg_count = current_count;
		}
		if(*update_count > 3){
			*update_count = 0;
		}
		switch(*update_count){
		case 0:
			 anode_select =  0b0111;
			 disp_info = dispA;
			break;
		case 1:
			 anode_select =  0b1011;
			 disp_info = dispB;
			break;
		case 2:
			 anode_select =  0b1101;
			 disp_info = dispC;
			break;
		case 3:
			 anode_select =  0b1110;
			 disp_info = dispD;
			break;
		}
		anode_select = (anode_select) &  GPIO1_ANODE_MASK;
		disp_info = (disp_info) &  GPIO1_CATHODE_MASK;
		XGpio_DiscreteWrite(&device1, GPIO1_ANODE_CH, anode_select);
		XGpio_DiscreteWrite(&device1, GPIO1_CATHODE_CH, disp_info);
}
//Plays Super Mario Theme song by looping trough all the notes and tempos
void superMario(u32 sw_data){
	u32 melody[] = {
				  NOTE_E7, NOTE_E7, 0, NOTE_E7,
				  0, NOTE_C7, NOTE_E7, 0,
				  NOTE_G7, 0, 0,  0,
				  NOTE_G6, 0, 0, 0,

				  NOTE_C7, 0, 0, NOTE_G6,
				  0, 0, NOTE_E6, 0,
				  0, NOTE_A6, 0, NOTE_B6,
				  0, NOTE_AS6, NOTE_A6, 0,

				  NOTE_G6, NOTE_E7, NOTE_G7,
				  NOTE_A7, 0, NOTE_F7, NOTE_G7,
				  0, NOTE_E7, 0, NOTE_C7,
				  NOTE_D7, NOTE_B6, 0, 0,

				  NOTE_C7, 0, 0, NOTE_G6,
				  0, 0, NOTE_E6, 0,
				  0, NOTE_A6, 0, NOTE_B6,
				  0, NOTE_AS6, NOTE_A6, 0,

				  NOTE_G6, NOTE_E7, NOTE_G7,
				  NOTE_A7, 0, NOTE_F7, NOTE_G7,
				  0, NOTE_E7, 0, NOTE_C7,
				  NOTE_D7, NOTE_B6, 0, 0};

	int tempo[] = {
	  12, 12, 12, 12,
	  12, 12, 12, 12,
	  12, 12, 12, 12,
	  12, 12, 12, 12,

	  12, 12, 12, 12,
	  12, 12, 12, 12,
	  12, 12, 12, 12,
	  12, 12, 12, 12,

	  9, 9, 9,
	  12, 12, 12, 12,
	  12, 12, 12, 12,
	  12, 12, 12, 12,

	  12, 12, 12, 12,
	  12, 12, 12, 12,
	  12, 12, 12, 12,
	  12, 12, 12, 12,

	  9, 9, 9,
	  12, 12, 12, 12,
	  12, 12, 12, 12,
	  12, 12, 12, 12,
	};

	int underworld_tempo[] = {
	  12, 12, 12, 12,
	  12, 12, 6,
	  3,
	  12, 12, 12, 12,
	  12, 12, 6,
	  3,
	  12, 12, 12, 12,
	  12, 12, 6,
	  3,
	  12, 12, 12, 12,
	  12, 12, 6,
	  6, 18, 18, 18,
	  6, 6,
	  6, 6,
	  6, 6,
	  18, 18, 18, 18, 18, 18,
	  10, 10, 10,
	  10, 10, 10,
	  3, 3, 3
	};

	int i = 0;
	int time = 0;
	float note = OFF;

	int size = sizeof(melody) / sizeof(melody[0]);
	//loops through the melody list and tempo list dependent on sw input and plays the tone
	for(i = 0;i < size;i++){

		note = (1.0/(2.0*melody[i]*10e-9));
		if(sw_data == 0){

		 time = 150000000/ tempo[i];
		 u32 message[] = {H,E,L,P,NONE,P,E,A,C,H};
		 int size_message = sizeof(message) / sizeof(message[0]);
		 play_melody(time,note,message,size_message,i);

		}else if(sw_data == 1){
			u32 message[] = {F,one,five,H,NONE,B,zero,one,NONE};
			int size_message = sizeof(message) / sizeof(message[0]);
			time = 130000000/ underworld_tempo[i];
			play_melody(time,note,message,size_message,i);

		}


	}

}
//Function for selecting which notes to play and what to display to the screen and for playing Mario Theme
void select_notes(u32 sw_data,u32 btn_data, u32 *target_count,u32 *dispA, u32 *dispB){
	//switch statement based on the btn inputs

	switch(btn_data){
	case 0:
		*target_count = OFF;
		*dispA = zero;
		*dispB = zero;
		break;
	case 1:
		*dispA = C;
		if(sw_data == 0){
			*target_count = C3;
			*dispB = three;
		}else if(sw_data == 1){
			*target_count = C4;
			*dispB = four;
		}else if(sw_data == 2){
			*target_count = C5;
			*dispB = five;
		}

		break;
	case 2:
		*dispA = D;
		if(sw_data == 0){
		    *target_count = D3;
		    *dispB = three;
		}
		else if(sw_data == 1){
			*target_count = D4;
			*dispB = four;
		}
		else if(sw_data == 2){
			*target_count = D5;
			*dispB = five;
		}

		break;
	case 3:

		if(sw_data == 0){
			*target_count = E3;
			*dispA = E;
			*dispB = three;
		}
		else if(sw_data == 1){
			*target_count = E4;
			*dispA = E;
			*dispB = four;
			}
		else{
			*target_count = OFF;
			*dispA = zero;
			*dispB = zero;
		}

		break;
	case 4:

		if(sw_data == 0){
			*target_count = F3;
			*dispA = F;
			*dispB = three;
		}
		else if(sw_data == 1){
			*target_count = F4;
			*dispA = F;
			*dispB = four;
		}else{
			*target_count = OFF;
			*dispA = zero;
			*dispB = zero;
		}


		break;
	case 5:

		if(sw_data == 0){
			*target_count = G3;
			*dispA = nine;
			*dispB = three;
		}
		else if(sw_data == 1){
			*target_count = G4;
			*dispA = nine;
			*dispB = four;
		}
		break;
	case 6:

		if(sw_data == 0){
			*target_count = A3;
			*dispA = A;
			*dispB = three;
		}
		else if(sw_data == 1){
			*target_count = A4;
			*dispA = A;
			*dispB = four;
		}else{
			*target_count = OFF;
			*dispA = zero;
			*dispB = zero;
		}
		break;
	case 7:

		if(sw_data == 0){
			*target_count = B3;
			*dispA = B;
			*dispB = three;}
		else if(sw_data == 1){
			*target_count = B4;
			*dispA = B;
			*dispB = four;}
		else{
			*target_count = OFF;
			*dispA = zero;
			*dispB = zero;
				}
			break;
	case 8:
		//reset timer for playing Mario themes
		XTmrCtr_Initialize(&TimerCounter, XPAR_TMRCTR_0_DEVICE_ID);
		XTmrCtr_SetOptions(&TimerCounter, TIMER_COUNTER_0, XTC_AUTO_RELOAD_OPTION);
		XTmrCtr_Start(&TimerCounter, TIMER_COUNTER_0);

		superMario(sw_data);
		break;






	}

}
//play notes for mario and calls function to pass message to screen
void play_melody(int time,u32 note,u32 *message,int size_message,int i){
	u32 last_count = 0;
	static int pos = 0;
	int update_count = 0;
	static int counter = 0;
	static u32 last_seg_count;
	static u32 last_mes_count;
	int current_count = 0;
	u32 amp2_data = 0x8;
	u32 dispA;
	u32 dispB;
	u32 dispC;
	u32 dispD;
	//fixes error where counter for scroll message is being incremented one time too
	if(i == 0){
		counter+= 1;
	}
	if(counter > 1 && i == 0){
		pos = -1;
	}

	int last_note_count = XTmrCtr_GetValue(&TimerCounter, TIMER_COUNTER_0);
	while((current_count - last_note_count) <  time) {

		scrollMessage(size_message,message,&pos,&last_mes_count,&dispA,&dispB,&dispC,&dispD);
		update_all_seven_seg(dispA,dispB,dispC,dispD,&update_count,&last_seg_count);
		update_amp2(&amp2_data, note, &last_count);
		current_count = XTmrCtr_GetValue(&TimerCounter, TIMER_COUNTER_0);
		}


}

//play notes for boot sequence
void play_note(int time,u32 note_select,u32 mode){
	u32 last_count = 0;
	u32 last_seg_count = 0;
	int current_count = 0;
	u32 dispA = zero;
	u32 dispB = zero;
	u32 target_count = OFF;
	u32 amp2_data = 0x8;
	int last_note_count = XTmrCtr_GetValue(&TimerCounter, TIMER_COUNTER_0);
	while((current_count - last_note_count) <  time) {
		select_notes(mode,note_select, &target_count,&dispA,&dispB);
		update_seven_seg(dispA,dispB,&last_seg_count);
		update_amp2(&amp2_data, target_count, &last_count);
		current_count = XTmrCtr_GetValue(&TimerCounter, TIMER_COUNTER_0);
		}


}
// if the current count is - last_count > target_count toggle the amp2 output
void update_amp2(u32 *amp2_data, u32 target_count, u32 *last_count) {
	u32 current_count = XTmrCtr_GetValue(&TimerCounter, TIMER_COUNTER_0);
	if ((current_count - *last_count) > target_count) {
		// toggling the LSB of amp2 data
		*amp2_data = ((*amp2_data & 0x01) == 0) ? (*amp2_data | 0x1) : (*amp2_data & 0xe);
		XGpio_DiscreteWrite(&device3, GPIO3_AMP2_CH, *amp2_data );
		*last_count = current_count;
	}
}
