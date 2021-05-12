#include <windows.h>
#include <stdio.h>
#include <conio.h>
#define    BUFFERLENGTH 256
int main(int argc, char *argv[])
{
	HANDLE hComm;
	DWORD MORO;
	char   *pcCommPort = "COM5";
	BOOL Write_Status;
	DCB dcbSerialParams;					// Initializing DCB structure
	COMMTIMEOUTS timeouts = { 0 };
	BOOL  Read_Status;                      // Status of the various operations
	DWORD dwEventMask;						// Event mask to trigger
	char  TempChar = "";                        // Temperory Character
	char  *SerialBuffer[BUFFERLENGTH+1];               // Buffer Containing Rxed Data
	DWORD NoBytesRead;                     // Bytes read by ReadFile()
	int i = 0;


	printf("\n\n +==========================================+");
	printf("\n |  Serial Transmission (Win32 API)         |");
	printf("\n +==========================================+\n");

	hComm = CreateFileA(pcCommPort,
		GENERIC_READ | GENERIC_WRITE,
		0,    // must be opened with exclusive-access
		NULL, // no security attributes
		OPEN_EXISTING, // must use OPEN_EXISTING
		0,    // not overlapped I/O
		NULL  // hTemplate must be NULL for comm devices
	);

	if (hComm == INVALID_HANDLE_VALUE)
	{

		if (GetLastError() == ERROR_FILE_NOT_FOUND)
		{
			puts("cannot open port!");
			return;
		}

		puts("invalid handle value!");
		return;
	}
	else
	  printf("opening serial port successful");

	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

	Write_Status = GetCommState(hComm, &dcbSerialParams);     //retreives  the current settings

	if (Write_Status == FALSE) {
		printf("\n   Error! in GetCommState()");
		CloseHandle(hComm);
		return 1;
	}


	dcbSerialParams.BaudRate = CBR_115200;//CBR_57600;      // Setting BaudRate = 9600
    dcbSerialParams.ByteSize = 8;             // Setting ByteSize = 8
											  //dcbSerialParams.
	dcbSerialParams.StopBits = ONESTOPBIT;    // Setting StopBits = 1
	dcbSerialParams.Parity = NOPARITY;//ODDPARITY;      // Setting Parity = None

	Write_Status = SetCommState(hComm, &dcbSerialParams);  //Configuring the port according to settings in DCB

	if (Write_Status == FALSE)
	{
		printf("\n   Error! in Setting DCB Structure");
		CloseHandle(hComm);
		return 1;
	}
	else
	{
		printf("\n   Setting DCB Structure Successful\n");
		printf("\n       Baudrate = %d", dcbSerialParams.BaudRate);
		printf("\n       ByteSize = %d", dcbSerialParams.ByteSize);
		printf("\n       StopBits = %d", dcbSerialParams.StopBits);
		printf("\n       Parity   = %d", dcbSerialParams.Parity);
	}

	// Set COM port timeout settings
	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;
	timeouts.WriteTotalTimeoutConstant = 50;
	timeouts.WriteTotalTimeoutMultiplier = 10;
	if (SetCommTimeouts(hComm, &timeouts) == 0)
	{
		printf("Error setting timeouts\n");
		CloseHandle(hComm);
		return 1;
	}


	/*----------------------------- Writing a Character to Serial Port----------------------------------------*/
	char lp[] = "RDGFIELD? 1\r\n";		// lpBuffer should be  char or byte array, otherwise write wil fail

	DWORD  NumWritten;
	DWORD  dNoOFBytestoWrite;              // No of bytes to write into the port
	DWORD  dNoOfBytesWritten = 0;          // No of bytes written to the port

	dNoOFBytestoWrite = sizeof(lp); // Calculating the no of bytes to write into the port


	if (!WriteFile(hComm, lp, dNoOFBytestoWrite,
		&dNoOfBytesWritten, NULL))
	{
		printf("Error writing text to %s\n", pcCommPort);
	}
	else
	{
			printf("\n %d bytes written to %s\n",
			dNoOfBytesWritten, pcCommPort);
	}

	if (Write_Status == TRUE)
		printf("\n\n    %s - Written to %s", lp, pcCommPort);
	else
		printf("\n\n   Error %d in Writing to Serial Port", GetLastError());

	/*------------------------------------ Setting Receive Mask ----------------------------------------------*/

	Read_Status = SetCommMask(hComm, EV_RXCHAR); //Configure Windows to Monitor the serial device for Character Reception

	if (Read_Status == FALSE)
		printf("\n\n    Error! in Setting CommMask");
	else
		printf("\n\n    Setting CommMask successfull");


	/*------------------------------------ Setting WaitComm() Event   ----------------------------------------*/

	printf("\n\n    Waiting for Data Reception");

	Read_Status = WaitCommEvent(hComm, &dwEventMask, NULL); //Wait for the character to be received

	 /*-------------------------- Program will Wait here till a Character is received ------------------------*/

	if (Read_Status == FALSE)
	{
		printf("\n    Error! in Setting WaitCommEvent()");
	}
	else //If  WaitCommEvent()==True Read the RXed data using ReadFile();
	{
		printf("\n\n    Characters Received \t");



		do
		{
			//Read_Status = ReadFile(hComm, &TempChar, sizeof(TempChar), &NoBytesRead, NULL);
			if (!ReadFile(hComm, SerialBuffer, BUFFERLENGTH, &NoBytesRead, NULL))
			{
				printf("wrong character");
			}
			SerialBuffer[i] = TempChar;
			printf("%c", SerialBuffer[i]);
			i++;
		} while (NoBytesRead > 0);



		/*------------Printing the RXed String to Console----------------------*/

		printf("\n\n    ");
		int j = 0;
		for (j = 0; j < i - 1; j++)		// j < i-1 to remove the dupliated last character
			printf("%c",SerialBuffer[j]);

	}


	CloseHandle(hComm);//Closing the Serial Port
	printf("\n ==========================================\n");


	return 0;
}
