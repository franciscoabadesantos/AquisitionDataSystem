#include <p24fxxxx.h>
#include <stdio.h>
#include <string.h>

#define MAX_SAMPLES 100
#define MAX_ANALOG 4
#define MAX_CHANNELS 8
//------------------------Global Variables------------------
int counter=0;
int seconds=0;
int MessageEnded =0;
int n=0;
char Message[100];
//----------------------Funciton Declaration------------------------------
void ADC_init();
void LED_init();
void UART_init();
void Timer_init();
void I2C_init();
int DigitalOutputA0_Conf(char * str);
int DigitalOutputA1_Conf(char * str);
void digitalOutputA0_On();
void digitalOutputA0_Off();
void digitalOutputA1_On();
void digitalOutputA1_Off();
void bidiretionalOutput_On();
void bidiretionalOutput_Off();
void readString(char* str);
void sendString(char *str);
int ADC_read(int chanel);
void Channel_configuration(char * str,  int * inputs);
int Samples_number(char * str);
int Frequency_configuration(char * str);
int Virtual_channel(char * str);
int Bidiretional_channel(char * str);
void Configuration(int * channels_to_sample, int *frequency, int *samples_num, int *virtual_ch, int *bidiretional_ch);
int readDigitalChannel();
void I2CReceive();
void I2CSend();
// Configuration Bits
#ifdef __PIC24FJ64GA004__ //Defined by MPLAB when using 24FJ64GA004 device
_CONFIG1( JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF & ICS_PGx1 & IOL1WAY_ON) 
_CONFIG2( FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMOD_HS & FNOSC_PRI & I2C1SEL_SEC)
#else
_CONFIG1( JTAGEN_OFF & GCP_OFF & GWRP_OFF & COE_OFF & FWDTEN_OFF & ICS_PGx2) 
_CONFIG2( FCKSM_CSDCMD & OSCIOFNC_OFF & POSCMOD_HS & FNOSC_PRI)
#endif


int main(void){

	int lastSeconds = 0; 
	int j=0, k=0;
	int virgula = 0;;

	int samples_counter=0; 
	
	int A2[MAX_SAMPLES];
	int A3[MAX_SAMPLES];
	int A4[MAX_SAMPLES];
	int A5[MAX_SAMPLES];
	int D6[MAX_SAMPLES];
	int D7[MAX_SAMPLES];
	int DB[MAX_SAMPLES];
	int DV[MAX_SAMPLES];
	
	char str[1000];
	char buffer[100];
	char num_buffer[100];

	//Default Configuration
	int frequency = 1;
	int samples_num = 5; 
	int virtual_ch= 0;
	int bidiretional_ch = 1;	
	int channels_to_sample[] = {1,1,1,1,1,1,1,0};  
	//{"A2":1, "A3":1, "A4":1, "A5":1, "D6":1, "D7":1, "DB":1, "DV":0}
	
	int A2Value;
	int A3Value;

	ADC_init();
	LED_init();
	UART_init();
	Timer_init();
	I2C_init();

	while(1){   
		
		int a,b;
		for (a=0;a<2000;a++)
			for (b=0;b<300;b++);

		if ( !PORTDbits.RD13 ){
			/*sendString("Please configure the following parameter of the device:\n(If copying from the paper -> \"\" <- won't work. Need to write them manually\n ");
			sendString(" - Channels to Sample,\n - Frequency,\n - Number of Samples,\n - Virtual Channel,\n - Bidirectional Channel\n -> ");
			
			Configuration(channels_to_sample, &frequency, &samples_num, &virtual_ch, &bidiretional_ch);	*/	
		}

		if(MessageEnded == 1){
			n=0;
			MessageEnded = 0;
			
			Configuration(channels_to_sample, &frequency, &samples_num, &virtual_ch, &bidiretional_ch);
		}
		
		if ( (seconds - lastSeconds) >= frequency ){
				I2CSend();	
				I2CReceive(&A2Value,&A3Value);		
		
				if(channels_to_sample[0] == 1){
					A2[samples_counter]  = A2Value;
				}
				if(channels_to_sample[1] == 1){
					A3[samples_counter]  = A3Value;
				}
				if(channels_to_sample[2] == 1){
					A4[samples_counter]  =ADC_read(0x0004);
				}	
				if(channels_to_sample[3] == 1){
					A5[samples_counter]  =ADC_read(0x0005);
				}
				if(channels_to_sample[4] == 1 && virtual_ch == 0){
					D6[samples_counter] = PORTDbits.RD6;
				}
				if(channels_to_sample[5] == 1 && virtual_ch == 0){
					D7[samples_counter] = PORTDbits.RD7;
				}
				if(channels_to_sample[6] == 1 && bidiretional_ch == 1){
					DB[samples_counter] = PORTAbits.RA7;
				}
				if(channels_to_sample[7] == 1 && virtual_ch == 1){
					DV[samples_counter] = readDigitalChannel();
				}
			
			samples_counter++;	
			
			lastSeconds = seconds;
		}

		// enviar apenas quando todos os samples estão registados
		if(samples_counter==samples_num){
			virgula = 0;
			strcpy(str,"{"); 		
	
			for(j=0;j< MAX_CHANNELS;j++){
				if(channels_to_sample[j] == 1){
				
					if (virgula == 1) {
	            		strcat(str, ",");
	        		}
					
					if((j+2) == 6 || (j+2) == 7 || (j+2) == 8 || (j+2) == 9){
						if((j+2) == 8 ){
							sprintf(buffer,"'DB':[");
							strcat(str,buffer);
						}
						if((j+2) == 9 ){
							sprintf(buffer,"'DV':[");
							strcat(str,buffer);
						}
						if((j+2) == 6 ){
							sprintf(buffer,"'D6':[");
							strcat(str,buffer);
						}
						if((j+2) == 7 ){						
							sprintf(buffer,"'D7':[");
							strcat(str,buffer);	
						}
					}else{
						sprintf(buffer,"'A%d':[",(j+2));
						strcat(str,buffer);					
					}
					
					int arrayCpy[100];
				
					switch(j+2){
						case 2:	memcpy(arrayCpy,A2, sizeof(A2));
								break;
						case 3: memcpy(arrayCpy,A3, sizeof(A3));
								break;
						case 4: memcpy(arrayCpy,A4, sizeof(A4));
								break;
						case 5: memcpy(arrayCpy,A5, sizeof(A5));
								break;
						case 6: memcpy(arrayCpy,D6, sizeof(D6));			
								break;
						case 7:	memcpy(arrayCpy,D7, sizeof(D7));		
								break;
						case 8: memcpy(arrayCpy,DB, sizeof(DB));
								break;
						case 9: memcpy(arrayCpy,DV, sizeof(DV));
								break;
					}
					for(k=0;k<samples_num;k++){
					
						sprintf(num_buffer, "%d",arrayCpy[k]);
						strcat(str,num_buffer);
	        			if (k < samples_num - 1) {
	            			strcat(str, ",");
	        			}
					}
				
					strcat(str, "]");
					virgula = 1;			
				}	
			} 
	   		strcat(str, "}\n");
			sendString(str);
//			sendString("\n");
		
			for(j = 0;j < samples_num; j++){
				A2[j] = 0;
				A3[j] = 0;
				A4[j] = 0;
				A5[j] = 0;
				D6[j] = 0;
				D7[j] = 0;
				DB[j] = 0;
				DV[j] = 0;
			}
			samples_counter=0;
		}
	}
}
/*
*Function: Configuration - Reads the message from the user and processes it. It also applys the changes to the system 
*
*Input: Recieves the channels_to_sample,frequency, samples_num, virtual_ch, *bidiretional_ch
*
*Output:None
*/
void Configuration(int *channels_to_sample, int *frequency, int *samples_num, int *virtual_ch, int *bidiretional_ch){
	
/*	sendString("Entered in Configuration \n");

	sendString(Message);*/
		
	switch (Message[2]){
		case'A':Channel_configuration(Message, channels_to_sample);
				*bidiretional_ch = channels_to_sample[6];
				
				if(*bidiretional_ch == 0){
					TRISAbits.TRISA7 = 0;
					bidiretionalOutput_On();
				}else{	
					TRISAbits.TRISA7 = 1;
					bidiretionalOutput_Off();
				}
		
				*virtual_ch = channels_to_sample[7];
				
				if(*virtual_ch ==1){
					channels_to_sample[4] = 0;
					channels_to_sample[5] = 0;
				}
				
				break;

		case'f':*frequency = Frequency_configuration(Message);
				break;

		case'n':*samples_num = Samples_number(Message);
				break;

		case'v':*virtual_ch = Virtual_channel(Message);
				channels_to_sample[7] = *virtual_ch;

				if(*virtual_ch ==1){
					channels_to_sample[4] = 0;
					channels_to_sample[5] = 0;
				}
				if(*virtual_ch ==0){
					channels_to_sample[4] = 1;
					channels_to_sample[5] = 1;
				}
				break;

		case'b':*bidiretional_ch = Bidiretional_channel(Message);
				channels_to_sample[6] = *bidiretional_ch;

				if(*bidiretional_ch == 0){
					TRISAbits.TRISA7 = 0;
					bidiretionalOutput_On();
				}else{	
					TRISAbits.TRISA7 = 1;
					bidiretionalOutput_Off();
				}
				break;
		
		case'D': if(Message[3] == '0' ){
					if(DigitalOutputA0_Conf(Message)){
						digitalOutputA0_On();
					}else{						
						digitalOutputA0_Off();
					}
				 }
		
				 if(Message[3] == '1'){
					if(DigitalOutputA1_Conf(Message)){
						digitalOutputA1_On();
					}else{
						digitalOutputA1_Off();
					}
				 }
	}
}
/*
*Function: DigitalOutputA0_Conf - Reads and processes the message that configures the digital output A0
*
*Input: The string to be read
*
*Output: returns the values that defines if the digital output is ative or inactive
*/
int DigitalOutputA0_Conf(char * str){
	int input;

    sscanf(str,"{\"D0\":%d}",&input);
	
	return input;
}
/*
*Function: DigitalOutputA1_Conf - Reads and processes the message that configures the digital output A1
*
*Input: The string to be read
*
*Output: returns the values that defines if the digital output is ative or inactive
*/
int DigitalOutputA1_Conf(char * str){
	int input;

    sscanf(str,"{\"D1\":%d}",&input);
	
	return input;
}
/*
*Function: readDigitalChannel - Reads the individual inputs from digital input 6 and 7 and returns a int values
								with the 2 bit channel outpu
*
*Input:None
*
*Output: 0, 1, 2, 3 as ints for the virtual channel output
*/
int readDigitalChannel(){
	int number=0;
	int biValues[2]={0,0};

	biValues[0]=PORTDbits.RD6;
	biValues[1]=PORTDbits.RD7;
	
	if(biValues[0] == 0 && biValues[1] == 0 ){
		number = 3;
	}
	if(biValues[0] == 0 && biValues[1] == 1 ){
		number = 2;
	}
	if(biValues[0] == 1 && biValues[1] == 0 ){
		number = 1;
	}
	if(biValues[0] == 1 && biValues[1] == 1 ){
		number = 0;
	}

	return number;
}
/*
*Function: Channel_configuration - Reads and processes the message that configures the channels to read
*
*Input:The string to be processed and where the input channels to read are going to be stored 
*
*Output:None
*/
void Channel_configuration(char * str, int * inputs){
	//sendString("Entered in Channel Configuration \n");
    sscanf(str, "{\"A2\":%d, \"A3\":%d, \"A4\":%d, \"A5\":%d, \"D6\":%d, \"D7\":%d, \"DB\":%d, \"DV\":%d}",
           &inputs[0], &inputs[1], &inputs[2], &inputs[3], &inputs[4], &inputs[5], &inputs[6],&inputs[7]);
}
/*
*Function: Frequency_configuration - Reads and processes the message that configures the frequency
*
*Input: The string to be read
*
*Output: the number of the frequency
*/
int Frequency_configuration(char * str){
	int input;

    sscanf(str,"{\"f\":%d}",&input);
	
	return input;
}
/*
*Function: Samples_number - Reads and processes the message that configures the numeber of samples
*
*Input:The string to be read
*
*Output:the numbers to sample
*/
int Samples_number(char * str){
	int input;

    sscanf(str, "{\"n\":%d}",&input);

	return input;
}
/*
*Function: Virtual_channel - Reads and processes the message that configures the virtual channel
*
*Input:The string to be read
*
*Output: 1 or 0 that defines of the channel is an input or an output
*/
int Virtual_channel(char * str){
	int input;

    sscanf(str, "{\"v\":%d}",&input);

	return input;
}
/*
*Function: Bidirectional_channel-Reads and processes the message that configures the bidirectional channel
*
*Input: The string to be read
*
*Output: 1 or 0 that defines of the channel is an input or an output
*/
int Bidiretional_channel(char * str){
	int input;

    sscanf(str, "{\"b\":%d}",&input);

	return input;
}

/*
*Function:Handles the interrupt off the timer. Increments the counter and the seconds
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

void __attribute__((__interrupt__, auto_psv)) _U2RXInterrupt(void)
{
	IFS1bits.U2RXIF = 0;

	Message[n] = U2RXREG;	

	if(Message[n]  == '\n'){
		MessageEnded =1;
	}
	n++;
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
	TRISAbits.TRISA7 = 1;
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

	//receive interrups from pic
	IEC1bits.U2RXIE =1;
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
*Function: readString - Reads a string from UART. Requires UART initialization
*
*Input:String
*
*Output:None
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
*Function: digitalOutputA0_On - Turns on the digital Output A0
*
*Input:None
*
*Output:None
*/
void digitalOutputA0_On(){

	LATAbits.LATA0 = 1;

}

/*
*Function: digitalOutputA0_Off - Turns off the digital Output A0
*
*Input:None
*
*Output:None
*/
void digitalOutputA0_Off(){
	
	LATAbits.LATA0 = 0;

}
/*
*Function: digitalOutputA1_On - Turns on the digital Output A1
*
*Input:None
*
*Output:None
*/
void digitalOutputA1_On(){
	
	LATAbits.LATA1 = 1;

}
/*
*Function: digitalOutputA1_Off - Turns off the digital Output A1
*
*Input:None
*
*Output:None
*/
void digitalOutputA1_Off(){
	
	LATAbits.LATA1 = 0;

}
/*
*Function: bidiretionalOutput_On - Turns on the LED when the bidirectional channel is defined as output
*
*Input:None
*
*Output:None
*/
void bidiretionalOutput_On(){
	LATAbits.LATA7 = 1 ;
}
/*
*Function: bidiretionalOutput_Off - Turns off the LED when the bidirectional channel is defined as output
*
*Input:None
*
*Output:None
*/
void bidiretionalOutput_Off(){
	LATAbits.LATA7 = 0 ;
}


/*
*Function: I2CSend - Responsable for starting I2C communication. Sends the command "AC" for slave to read sensor values. 
*					 Requires I2C initialization
*Input:None
*
*Output:None
*/
void I2CSend(){

		I2C2CONbits.SEN=1;

		while(I2C2CONbits.SEN);//Esperara que a linha volte a zero para comecar a comunicacao
		
        I2C2TRN = (0x48<<1) + 0;//enviar address		
		//enviar r/w bit juntamente com o address

		while(I2C2STATbits.TBF); //Verificamos de o o hardware acabou de enviar os dados

		while(I2C2STATbits.TRSTAT); //hardware clear at end of slave Acknowledge

    	if( I2C2STATbits.ACKSTAT)//recebemos ack		
		{
			sendString("ACK not recieved.\n");
		}
	//	else
			//sendString("ACK recieved.\n");
		I2C2TRN=0xAC; //enviamos o commando
		
		while(I2C2STATbits.TBF); //Verificamos de o o hardware acabou de enviar os dados

		while(I2C2STATbits.TRSTAT); //hardware clear at end of slave Acknowledge
		
		if( I2C2STATbits.ACKSTAT)//recebemos ack		
		{
			sendString("ACK not recieved.\n");
		}
		//else
		//	sendString("ACK recieved.\n");
		I2C2CONbits.PEN=1;
		while(I2C2CONbits.PEN)
		{};
}
/*
*Function: I2CReceive - Responsable for receiving sensor values from I2C communication. 
*					    Requires I2C initialization
*Input:None
*
*Output:None
*/
void I2CReceive(int *A2Value,int *A3Value){		

		I2C2CONbits.SEN=1;

		while(I2C2CONbits.SEN);//Esperara que a linha volte a zero para comecar a comunicacao
		
        I2C2TRN = (0x48<<1) + 1;//enviar address		
		//enviar r/w bit juntamente com o address

		while(I2C2STATbits.TBF); //Verificamos de o o hardware acabou de enviar os dados

		while(I2C2STATbits.TRSTAT); //hardware clear at end of slave Acknowledge

    	if( I2C2STATbits.ACKSTAT)//recebemos ack		
		{
			sendString("ACK not recieved.\n");
		}
	//	else
		//	sendString("ACK recieved.\n");
			

		char data[5]={0,0,0,0,0};
		int i;
		for(i=0;i<5;i++){
	
		    I2C2CONbits.RCEN =1;
			while(!I2C2STATbits.RBF); //Verificamos de o o hardware acabou de enviar os dados 		
	
			data[i] = I2C2RCV;	//recebemos dados	
			while(I2C2CONbits.RCEN);

			if(i<4)
				I2C2CONbits.ACKDT = 0;
			else
				I2C2CONbits.ACKDT = 1;
			I2C2CONbits.ACKEN = 1;
			while(I2C2CONbits.ACKEN);
		}

		I2C2CONbits.PEN=1;
		while(I2C2CONbits.PEN)
		{};

		char str3[10];
		/**sprintf(str3, "Data1_1: %d,%d\n",data[1], data[2]);		
		sendString(str3);*/		

		*A2Value= (data[1] << 8) | data[2];
		char str1[10];
		char str2[10];
		
		/*sprintf(str1, "Data1: %d\n",*A2Value);
		sendString(str1);*/

		*A3Value = (data[3] << 8) | data[4];

		/*sprintf(str2, "Data2: %d\n",*A3Value);
		sendString(str2);*/	
}
/*
*Function: I2C_init - Handels the initialization to use the I2C communication. 
*					  
*Input:None
*
*Output:None
*/
void I2C_init(){
	I2C2BRG=39; //Baud rate value set to 100 KHz 
	I2C2CONbits.I2CEN =1; //I2C enable
	
}
