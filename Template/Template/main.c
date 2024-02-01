#include <p24fxxxx.h>
#include <stdio.h>
#include <string.h>

#define MAX_SAMPLES 100
//------------------------Global Variables------------------
int counter=0;
int seconds=0;
//----------------------Funciton Declaration------------------------------
void ADC_init();
void LED_init();
void UART_init();
void Timer_init();
void LED_Blinking();
void readString(char* str);
void sendString(char *str);
int ADC_read(int chanel);
void Channel_configuration(char * str,  int * inputs);
int Samples_number(char * str);
int Frequency_configuration(char * str);
int Virtual_channel(char * str);
int Bidiretional_channel(char * str);
void Configuration(int * channels_to_sample, int frequency, int samples_num, int virtual_ch, int  bidiretional_ch);
// Configuration Bits
#ifdef __PIC24FJ64GA004__ //Defined by MPLAB when using 24FJ64GA004 device
_CONFIG1( JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF & ICS_PGx1 & IOL1WAY_ON) 
_CONFIG2( FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMOD_HS & FNOSC_PRI & I2C1SEL_SEC)
#else
_CONFIG1( JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF & ICS_PGx2) 
_CONFIG2( FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMOD_HS & FNOSC_PRI)
#endif


int main(void){
		
	int frequency = 1;
	int samples_num = 1; 
	int virtual_ch= 0;
    int bidiretional_ch = 0;
	int lastSeconds = 0; 
	int j=0;
	int* channels_to_sample;
	int samples_counter=0; 
	
	int A2[MAX_SAMPLES];
	int A3[MAX_SAMPLES];
	int A4[MAX_SAMPLES];
	int A5[MAX_SAMPLES];


	char str[100];
	char buffer[100];
	char num_buffer[100];
    
	ADC_init();
	LED_init();
	UART_init();
	Timer_init();
	
	sendString("Please configure the following parameter of the device:\n");
	sendString(">Channels to Sample,\n>Frequency,\n>Number of Samples,\n>Virtual Channel,\n>Bidirectional Channel,\n");
	while(1){   

		Configuration(channels_to_sample, frequency, samples_num, virtual_ch, bidiretional_ch);

		if ( (seconds - lastSeconds) >= frequency ){
				
				if(channels_to_samples[0] == 1){
					A2[samples_counter]  =ADC_read(0x0002);
				}
				if(channels_to_samples[1] == 1){
					A3[samples_counter]  =ADC_read(0x0003);
				}
				if(channels_to_samples[2] == 1){
					A4[samples_counter]  =ADC_read(0x0004);
				}	
				if(channels_to_samples[3] == 1){
					A5[samples_counter]  =ADC_read(0x0005);
				}
			samples_counter++;	
			
			lastSeconds = seconds;
		}


		if(samples_counter==samples_num){
			//escrever
			strcpy(str,"{");  //começar chavetas
	
			for(j=0;j< MAX_ANALOG;j++){
				if(channels_to_sample[j] == 1){
	
					sprintf(buffer,"'A%d':[",(j+2));
					strcat(str,buffer);
					for(k=0;k<samplesNum;k++){
						sprintf(num_buffer"%d",);
					}
					int arrayCpy[100];
					switch(j+2)
	
						case 2:	memcpy(arrayCpy,A2);
								break;
						case 3: memcpy(arrayCpy,A3);
								break;
						case 4: memcpy(arrayCpy,A4);
								break;
						case 5: memcpy(arrayCpy,A5);
								break;
					for(k=0;k<samplesNum;k++){
						sprintf(num_buffer"%d",arrayCpy[k]);
						strcat(str,buffer);
	        			if (i < samplesNum - 1) {
	            			strcat(str, ",");
	        			}
					}
	
				}	
	
			} 
	   		strcat(str, "]}");
			sendString(str);
			//fechar chavetas
			for(j = 0;j < samplesNum; j++){
				A2[j] = 0;
				A3[j] = 0;
				A4[j] = 0;
				A5[j] = 0;
			}
			samples_counter=0;
		}
	}
}

void Configuration(int *channels_to_sample, int frequency, int samples_num, int virtual_ch, int  bidiretional_ch){
	
	char str[100];

	readString(str);
	sendString(str);

	switch (str[2]){
		case'A':Channel_configuration(str, channels_to_sample);
				break;

		case'f':frequency = Frequency_configuration(str);
				break;

		case'n':samples_num = Samples_number(str);
				break;

		case'v':virtual_ch = Virtual_channel(str);
				break;

		case'b':bidiretional_ch = Bidiretional_channel(str);
				break;
	}
}

void Channel_configuration(char * str, int * inputs){

    sscanf(str, "{\"A2\":%d, \"A3\":%d, \"A4\":%d, \"A5\":%d, \"D6\":%d, \"D7\":%d, \"DB\":%d, \"DV\":%d}",
           &inputs[0], &inputs[1], &inputs[2], &inputs[3], &inputs[4], &inputs[5], &inputs[6],&inputs[7]);
			//[0,1,0,1,1,0]
}

int Frequency_configuration(char * str){
	int input;

    sscanf(str,"{\"f\":%d}",&input);

	return input;
}

int Samples_number(char * str){
	int input;

    sscanf(str, "{\"n\":%d}",&input);

	return input;
}

int Virtual_channel(char * str){
	int input;

    sscanf(str, "{\"v\":%d}",&input);

	return input;
}

int Bidiretional_channel(char * str){
	int input;

    sscanf(str, "{\"b\":%d}",&input);

	return input;
}

/*
*Function:LED_init - Handles the timer interrupt when T1IF flag from IFS0 register is set to 1.
*
*Input:None
*
*Output:None
*/
void __attribute__((__interrupt__, __shadow__)) _T1Interrupt(void)
{
	//char str[10];
	counter++;
	if(counter==60){
		seconds++;
		//sprintf(str, "%d\n", seconds);
		//sendString(str);
		counter=0;
	}
	IFS0bits.T1IF = 0; //Reset Timer1 interrupt flag and Return from ISR
}

/*
*Function:Timer_init - Handles the initialization to use the timer and timer interrupt
*
*Input:None
*
*Output:None
*/
void Timer_init(){
	T1CON = 0x00; //Stops the Timer1 and reset control reg.
	TMR1 = 0x00; //Clear contents of the timer register
	PR1 = 0xFFFF; //Load the Period register with the value 0xFFFF
	IPC0bits.T1IP = 0x01; //Setup Timer1 interrupt for desired priority level
					   	  // (This example assigns level 1 priority)
	IFS0bits.T1IF = 0; //Clear the Timer1 interrupt status flag
	IEC0bits.T1IE = 1; //Enable Timer1 interrupts
	T1CONbits.TON = 1; //Start Timer1 with prescaler settings at 1:1 and
					   //clock source set to the internal instruction cycle
}

/*
*Function:LED_init - Handles the initialization to use the LEDs with index 0 and 1
*
*Input:None
*
*Output:None
*/

void LED_init(){

	TRISDbits.TRISD6 = 1;
	TRISAbits.TRISA0 = 0;
	TRISAbits.TRISA1 = 0;
}

/*
*Function:UART_init - Handles the initialization to use the UART
*
*Input:None
*
*Output:None
*/
void UART_init(){
	U2BRG=25;             //Set Baudrate
	U2STA = 0;
	U2MODE = 0x8000;      //Enable Uart for 8-bit data
	                      //no parity, 1 STOP bit
	U2STAbits.UTXEN = 1;  //Enable Transmit
}

/*
*Function:ADC_init - Handels the initialization to use the ADC
*
*Input:None
*
*Output:None
*/
void ADC_init(){

	AD1PCFG = 0xFFC3; //ffc3 para o trabalho- quais as entradas analógicas a ler
	AD1CON1 = 0x0000;

	AD1CSSL = 0;
	AD1CON3 = 0x0002;
	AD1CON2 = 0;
	AD1CON1bits.ADON = 1;

}

/*
*Function: ADC_read - Reads the input from a give ADC chanel. Requires ADC initialization
*
*Input:Number of the channel in Hexa
*
*Output:The value read by the ADC as an int
*/
int ADC_read(int channel){
	int i;
	AD1CHS = channel; //canal que queremos ler

	AD1CON1bits.SAMP=1;
	for( i = 0 ; i < 1000 ; i++){};
	AD1CON1bits.SAMP=0;
	while (!AD1CON1bits.DONE) {};
	int	ADCValue=ADC1BUF0;

	return ADCValue;
}

/*
*Function: ADC_read - Reads the input from a give ADC chanel. Requires ADC initialization
*
*Input:Number of the channel in Hexa
*
*Output:The value read by the ADC as an int
*/

void readString(char* str) {
    int i = 0;
    char letra='a';
  
    // Wait for a string to be received
    
    // Read the string
     while(letra != '\n' && i < 99){
		
        // Wait for a character to be received
        if (U2STAbits.URXDA == 1) {
        	
	        // Read the character
	        letra = U2RXREG;
	        
	        // Add the character to the string
	        str[i] = letra;
	        i++;
		}
	}
    
	str[i] = '\0';

}

/*
*Function:sendingStrings - Sends a String throught the UART N.2. Requires UART initialization
*
*Input:String
*
*Output:None
*/

void sendString(char *str){

	char *add = str;              //!!!!!!!Mudei de int para char*, ver se depois nao ha problema
	while (*str != '\0')
	{
		while (U2STAbits.UTXBF); // Wait for TX buffer to be empty
		U2TXREG = *str;          // Send character
		str++;                   // Increment pointer to next character
	}
	str=add;
}

/*
*Function:Makes the LEDS with the index 0 and 1 blink one time. Requires LED initialization 
*
*Input:None
*
*Output:None
*/
void LED_Blinking(){
	
	int j;

	LATAbits.LATA0 = 1;
	LATAbits.LATA1 = 1 ;
	for( j = 0 ; j < 20000 ; j++){};
	LATAbits.LATA1 = 0;
	LATAbits.LATA0 = 0;
	for( j = 0 ; j < 20000 ; j++){};
}
