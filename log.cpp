#include "StdAfx.h"
#include ".\log.h"

static const int debugALL = 0xFFFFFFFF;

eculog::eculog(void)
{
	hLogfile = NULL;
	logMask = debugALL;
	bLogFile = true;
	bLogDebug = false;
	bTimeStamp = true;
	bTimeRelative = true;
	tLogStart = 0;
	tLogLast = 0;
	bLogDialog = true;
	pDialog = NULL;
}

eculog::~eculog(void)
{
}

void eculog::set_timestamp(bool _timestamp)
{
	bTimeStamp = _timestamp;
}

void eculog::set_filename(string _filename)
{
	filename = _filename;
}

void eculog::set_mask(int _mask)
{
	logMask = _mask;
}

bool eculog::open()
{
	if (hLogfile)
		close();

	if (bLogFile && !filename.empty())
	{
		if (NULL == (hLogfile = fopen(filename.c_str(),"a+")))
		{
			printf("can't open ram output file.\n");
			return false;
		}

		time_t ttNow;
		time(&ttNow);
		CTime ctTime(ttNow);
		fprintf(hLogfile,"\necuFlash [%s]\n",ctTime.Format("%d/%m/%y @ %H:%m:%S"));
	}

	return true;
}

bool eculog::close()
{
	if (!hLogfile)
		return false;

	fclose(hLogfile);
	hLogfile = NULL;
	return true;
}

#define SERIAL_DBGPRINT_BUFSIZE 16384

bool eculog::print(IN ULONG Level, PCHAR Format, ...)
{
	va_list arglist;
	ULONG Mask;

   if (Level > 31)
        Mask = Level;
   else
      Mask = 1 << Level;

   if ((Mask & logMask) == 0)
      return true;

   int cb;
	char buffer[SERIAL_DBGPRINT_BUFSIZE];
	char *pbuf = buffer;

	int tCur = GetTickCount();
	int tDelta;
	if (tLogStart == 0)
	{
		tLogStart = tCur;
		tLogLast = tCur;
	}
	if (bTimeRelative)
		tDelta = tCur-tLogLast;
	else
		tDelta = tCur-tLogStart;
	tLogLast = tCur;

	if (bTimeStamp)
		pbuf += sprintf(buffer,"[%05.3f] ",tDelta/1000.0);

	va_start(arglist, Format);
	cb = _vsnprintf(pbuf, sizeof(buffer)-(pbuf-buffer), Format, arglist);

	if (cb == -1)
	{
		buffer[sizeof(buffer) - 2] = '\n';
		buffer[sizeof(buffer) - 1] = '\0';
	}

	if (bLogDebug)
		OutputDebugString(buffer);

	if (bLogFile && hLogfile)
		fprintf(hLogfile,buffer);

	if(bLogDialog)
		pDialog->UpdateProgress(buffer);

   va_end(arglist);

   return true;
   //	bool ret = print(Format,arglist);
//	va_end(arglist);
//	return ret;
}

bool eculog::print(PCHAR Format, ...)
{
	va_list arglist;
	int cb;
	char buffer[SERIAL_DBGPRINT_BUFSIZE];
	char *pbuf = buffer;

	int tCur = GetTickCount();
	int tDelta;
	if (tLogStart == 0)
	{
		tLogStart = tCur;
		tLogLast = tCur;
	}
	if (bTimeRelative)
		tDelta = tCur-tLogLast;
	else
		tDelta = tCur-tLogStart;
	tLogLast = tCur;

	if (bTimeStamp)
		pbuf += sprintf(buffer,"[%05.3f] ",tDelta/1000.0);

	va_start(arglist, Format);

	cb = _vsnprintf(pbuf, sizeof(buffer)-(pbuf-buffer), Format, arglist);

	if (cb == -1)
	{
		buffer[sizeof(buffer) - 2] = '\n';
		buffer[sizeof(buffer) - 1] = '\0';
	}

	if (bLogDebug)
		OutputDebugString(buffer);

	if (bLogFile && hLogfile)
		fprintf(hLogfile,buffer);

	if(bLogDialog)
		pDialog->UpdateProgress(buffer);

   va_end(arglist);

   return true;
}

void eculog::set_dialog(dialogEcuFlash* lpDialog)
{
	pDialog = lpDialog;
}
