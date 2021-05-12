//https://forum.slackware.pl/viewtopic.php?t=18121


#include <windows.h>
#include <stdio.h>

int main()
{
    unsigned int timeRead=0;
    int alfaRead=0;
    double timeWrite=0, alfaWrite;

    const char *output_file = "C:\\chwilowy\\Serial.txt";

    FILE *file_stream;

    file_stream = fopen(output_file, "w");
    if(file_stream == NULL)
    {
        fprintf(stderr, "Could not open the output file");
        exit(EXIT_FAILURE);
    }

   HANDLE hCom;    //uchwyt portu
   DCB dcb;        //konfiguracja portu
   BOOL fSuccess;  //flaga pomocnicza
   BYTE RS_buf;    //bufor danej
   DWORD RS_ile;   //ilosc bitow wyslanych

   //otwarcie portu COM1 z prawami RW
   hCom = CreateFile( TEXT("COM5"), GENERIC_READ | GENERIC_WRITE,
        0,    // exclusive access
        NULL, // default security attributes
        OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

   if (hCom == INVALID_HANDLE_VALUE)
   {
        printf("CreateFile failed with error %d.\n", GetLastError());

        return 1;
   }

   //pobranie aktualnych ustawien portu
    fSuccess = GetCommState(hCom, &dcb);
    if (!fSuccess)
    {
      printf ("GetCommState failed with error %d.\n", GetLastError());
      return 2;
    }

   //ustawienie naszej konfiguracji
   dcb.BaudRate = CBR_115200;     // predkosc transmisji
   dcb.ByteSize = 8;             // ilosc bitow danych
   dcb.Parity = NOPARITY;        // brak bitu parzystosci
   dcb.StopBits = TWOSTOPBITS;    // dwa bity stopu

   fSuccess = SetCommState(hCom, &dcb);

   if (!fSuccess)
   {
      printf ("SetCommState failed with error %d.\n", GetLastError());
      return 3;
   }

   //wyslanie czegos by wyzerowac czas
   int i;
   for (i=0;i<2;i++)
   {
       RS_buf='4';
        fSuccess = WriteFile( hCom, &RS_buf, 1, &RS_ile, 0);
        if (!fSuccess)
        {
          printf ("WriteFile failed with error %d.\n", GetLastError());
          return 4;
        }
        RS_buf=0;
    fSuccess = WriteFile( hCom, &RS_buf, 1, &RS_ile, 0);
   }


    int alfaOverflow = -1;
    int alfaOld = 0;

   while(1)//(timeRead < 60000)
   {
       unsigned char strTime[5];
       int i=0;

       while(RS_buf)
       {
            fSuccess = ReadFile( hCom, &RS_buf, 1, &RS_ile, 0);
            if (!fSuccess)
            {
                printf ("ReadFile failed with error %d.\n", GetLastError());
                return 5;
            }

            if(RS_ile==1)
            {
                if(RS_buf == 1)   break;
                printf("%c",RS_buf);
                strTime[i++] = RS_buf;
            }
       }
       RS_buf=1;

       //printf("%s\t",str);
       timeRead = (unsigned int)strtol(strTime,NULL,10);
       //printf("%d\t",timeRead);
       timeWrite = (double)timeRead / 1000;
        //printf("%f\t",timeWrite);

       unsigned char strAlfa[6];
        i=0;
       while(RS_buf)
       {
            fSuccess = ReadFile( hCom, &RS_buf, 1, &RS_ile, 0);
            if (!fSuccess)
            {
                printf ("ReadFile failed with error %d.\n", GetLastError());
                return 5;
            }

            if(RS_ile==1)
            {
                if(RS_buf == 1)   break;
                printf("%c",RS_buf);
                strAlfa[i++] = RS_buf;
            }
       }
       RS_buf=1;

       //printf("%s\t",strAlfa);
       alfaRead = (int)strtol(strAlfa,NULL,10);
        //printf("%d\t",alfaRead);

       if(alfaOverflow == -1)
       {
           alfaOverflow=0;
           alfaOld=alfaRead;
       }

       //dopisac overflo³a !!!!!!!!!!!!!!!!!!

       //printf("%d\t",alfaRead);
       alfaWrite = (alfaRead * 6.28) / 8400;
       //printf("%f\n",alfaWrite);

       fprintf(file_stream,"%f;%f\n",timeWrite,alfaWrite);
       printf("\n");
   }

   fclose(file_stream);
   return 0;
}
