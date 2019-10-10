#include "MicroBit.h"

// Radio signal to car
// James Sheaf-Morrison   ID: 1314151
// Tamahau Brown	  ID: 1314934

MicroBit uBit;
 
uint8_t radioGroup = 88;
uint8_t buffer[15];
int park = 0;
int lastLetter = 7;
char parkLetter[] ={'A','B','C','D','E','F','G','H'}; 
bool needPark = true;

ManagedString rxdata;

void onButtonBhold(MicroBitEvent)
{
		//Sends the location from the buffer
		uBit.display.scroll("send");
		int groupNum = (radioGroup+1) * 256 + (radioGroup+1);
		*((int *)buffer) = 0x11 * 256 + 0x11;
		*((int *)(buffer+2)) = 0x12 * 256 + 0x12;
		*((int *)(buffer+4)) = 0x00 * 256 + park;
		uBit.radio.datagram.send(buffer, 6);
  		needPark = false;
		return;
}

// The three following methods are for manual park entry
void setXonButtonA(MicroBitEvent)
{
  if(park == lastLetter){
    park = 0;
  }
  else{
    park++;
  }
}

void setYonButtonB(MicroBitEvent)
{
  if(park == 0){
    park = lastLetter;
  }
  else{
    park--;
  }
}

int main()
{
    // Initialise the micro:bit runtime.
    uBit.init();
    uBit.radio.enable();
    uBit.radio.setGroup(radioGroup);
    uBit.radio.setTransmitPower(7);
    
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_A, MICROBIT_BUTTON_EVT_CLICK, setXonButtonA);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_CLICK, setYonButtonB);
    uBit.messageBus.listen(MICROBIT_ID_BUTTON_B, MICROBIT_BUTTON_EVT_HOLD, onButtonBhold);
    uBit.display.print("Lulu");
    // Set parking lot
    while (needPark){
	    //uBit.display.print(clear);
	    //uBit.display.print(i, locX, locY);
	    uBit.display.print(parkLetter[park]);
	    uBit.sleep(100);
    }
    
    release_fiber();
}
