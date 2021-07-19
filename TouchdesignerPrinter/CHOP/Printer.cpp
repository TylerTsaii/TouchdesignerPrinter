/* Shared Use License: This file is owned by Derivative Inc. (Derivative)
* and can only be used, and/or modified for use, in conjunction with
* Derivative's TouchDesigner software, and only if you are a licensee who has
* accepted Derivative's TouchDesigner license or assignment agreement
* (which also govern the use of this file). You may share or redistribute
* a modified version of this file provided the following conditions are met:
*
* 1. The shared file or redistribution must retain the information set out
* above and this list of conditions.
* 2. Derivative's name (Derivative Inc.) or its trademarks may not be used
* to endorse or promote products derived from this file without specific
* prior written permission from Derivative.
*/
#include "Pinter.h"
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <assert.h>
#include <iostream>

int PrintImage(HDC dc,LPCSTR FileName) {

	//下面是添加的
	LPBITMAPINFO info;
	HBITMAP      hbit;
	BITMAP       bm;
	int          nColors = 0;
	int          sizeinfo = 0;
	RGBQUAD      rgb[256];
	hbit = (HBITMAP)LoadImage(0,
		FileName,
		IMAGE_BITMAP,
		0,
		0,
		LR_CREATEDIBSECTION | LR_LOADFROMFILE);
	GetObject(hbit, sizeof(BITMAP), (LPVOID)&bm);

	nColors = (1 << bm.bmBitsPixel);
	if (nColors > 256)
		nColors = 0;

	sizeinfo = sizeof(BITMAPINFO) + (nColors * sizeof(RGBQUAD));   // This is the size required

	info = (LPBITMAPINFO)malloc(sizeinfo);                        // Storing info in memory

	info->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	info->bmiHeader.biWidth = bm.bmWidth;
	info->bmiHeader.biHeight = bm.bmHeight;
	info->bmiHeader.biPlanes = 1;
	info->bmiHeader.biBitCount = bm.bmBitsPixel * bm.bmPlanes;
	info->bmiHeader.biCompression = BI_RGB;
	info->bmiHeader.biSizeImage = bm.bmWidthBytes * bm.bmHeight;
	info->bmiHeader.biXPelsPerMeter = 0;
	info->bmiHeader.biYPelsPerMeter = 0;
	info->bmiHeader.biClrUsed = 0;
	info->bmiHeader.biClrImportant = 0;

	if (nColors <= 256)
	{
		HBITMAP hOldBitmap;
		HDC     hMemDC = CreateCompatibleDC(NULL);      // Creating an auxiliary device context

		hOldBitmap = (HBITMAP)SelectObject(hMemDC, hbit);  // Select 'hbit' in 'it'
		GetDIBColorTable(hMemDC, 0, nColors, rgb);          // Obtaining the color table information

		/*
		 * Now we pass this color information to "info" structure
		 */
		for (int iCnt = 0; iCnt < nColors; ++iCnt)
		{
			info->bmiColors[iCnt].rgbRed = rgb[iCnt].rgbRed;
			info->bmiColors[iCnt].rgbGreen = rgb[iCnt].rgbGreen;
			info->bmiColors[iCnt].rgbBlue = rgb[iCnt].rgbBlue;
		}

		SelectObject(hMemDC, hOldBitmap);
		DeleteDC(hMemDC);
	}
	StretchDIBits(dc,
		0, 0,
		bm.bmWidth ,
		bm.bmHeight ,
		0, 0,
		bm.bmWidth,
		bm.bmHeight,
		bm.bmBits,
		info,
		DIB_RGB_COLORS, SRCCOPY);

	DeleteObject(hbit);
	return 1;
	//上面是自己添加的
}



int PrintCostm(LPCSTR FileName) {


	char szprinter[80];
	char* szDevice, * szDriver, * szOutput;
	HDC hdcprint; // 定义一个设备环境句柄
	//定义一个打印作业
	static DOCINFO di = { sizeof(DOCINFO),"printer",NULL };
	std::cout << &di << "\n";
	GetProfileString("windows", "device", ",,,", szprinter, 80);

	std::cout << szprinter << "\n";
	szDevice = strtok(szprinter, ",");
	szDriver = strtok(NULL, ",");
	szOutput = strtok(NULL, ",");
	if (szDevice && szDriver && szOutput) {
		if ((hdcprint = CreateDC(szDriver, szDevice, szOutput, NULL)) != 0) {
			std::cout << hdcprint << "\n";
			if (StartDoc(hdcprint, &di) > 0) //开始执行一个打印作业
			{

				StartPage(hdcprint); //打印机走纸,开始打印
				SaveDC(hdcprint); //保存打印机设备句柄  
				PrintImage(hdcprint,FileName);
				RestoreDC(hdcprint, -1); //恢复打印机设备句柄
				EndPage(hdcprint); //打印机停纸,停止打印
				EndDoc(hdcprint); //结束一个打印作业
		//        MessageBox("打印完毕!","提示",MB_ICONINFORMATION);
			}
			// 用API函数DeleteDC销毁一个打印机设备句柄
			DeleteDC(hdcprint);
		}
		else {
			//    MessageBox("没有默认打印机,或者没有安装打印机!");
			return 0;
		}
	}
	return 1;
}



// These functions are basic C function, which the DLL loader can find
// much easier than finding a C++ Class.
// The DLLEXPORT prefix is needed so the compile exports these functions from the .dll
// you are creating
extern "C"
{

DLLEXPORT
void
FillCHOPPluginInfo(CHOP_PluginInfo *info)
{
	// Always set this to CHOPCPlusPlusAPIVersion.
	info->apiVersion = CHOPCPlusPlusAPIVersion;

	// The opType is the unique name for this CHOP. It must start with a 
	// capital A-Z character, and all the following characters must lower case
	// or numbers (a-z, 0-9)
	info->customOPInfo.opType->setString("Customsignal");

	// The opLabel is the text that will show up in the OP Create Dialog
	info->customOPInfo.opLabel->setString("Custom Signal");

	// Information about the author of this OP
	info->customOPInfo.authorName->setString("Author Name");
	info->customOPInfo.authorEmail->setString("email@email.com");

	// This CHOP can work with 0 inputs
	info->customOPInfo.minInputs = 0;

	// It can accept up to 1 input though, which changes it's behavior
	info->customOPInfo.maxInputs = 1;
}

DLLEXPORT
CHOP_CPlusPlusBase*
CreateCHOPInstance(const OP_NodeInfo* info)
{
	// Return a new instance of your class every time this is called.
	// It will be called once per CHOP that is using the .dll
	return new WorkshopCHOP(info);
}

DLLEXPORT
void
DestroyCHOPInstance(CHOP_CPlusPlusBase* instance)
{
	// Delete the instance here, this will be called when
	// Touch is shutting down, when the CHOP using that instance is deleted, or
	// if the CHOP loads a different DLL
	delete (WorkshopCHOP*)instance;
}

};


WorkshopCHOP::WorkshopCHOP(const OP_NodeInfo* info) : myNodeInfo(info)
{

}

WorkshopCHOP::~WorkshopCHOP()
{

}

void
WorkshopCHOP::getGeneralInfo(CHOP_GeneralInfo* ginfo, const OP_Inputs* inputs, void* reserved1)
{
	// This will cause the node to cook every frame
	ginfo->cookEveryFrameIfAsked = true;

	// Note: To disable timeslicing you'll need to turn this off, as well as ensure that
	// getOutputInfo() returns true, and likely also set the info->numSamples to how many
	// samples you want to generate for this CHOP. Otherwise it'll take on length of the
	// input CHOP, which may be timesliced.
	ginfo->timeslice = true;

	ginfo->inputMatchIndex = 0;
}

bool
WorkshopCHOP::getOutputInfo(CHOP_OutputInfo* info, const OP_Inputs* inputs, void* reserved1)
{

		info->numChannels = 1;

		// Since we are outputting a timeslice, the system will dictate
		// the numSamples and startIndex of the CHOP data
		//info->numSamples = 1;
		//info->startIndex = 0

		// For illustration we are going to output 120hz data
		info->sampleRate = 120;
		return true;
}

void
WorkshopCHOP::getChannelName(int32_t index, OP_String *name, const OP_Inputs* inputs, void* reserved1)
{
	name->setString("print_kun");
}

void
WorkshopCHOP::execute(CHOP_Output* output,
							  const OP_Inputs* inputs,
							  void* reserved)
{

	inputs->enablePar("Printfilename", 1);

	//double speed = inputs->getParDouble("Printfilename");
	Printfilename = inputs->getParString("Printfilename");
	///std::cout << Printfilename<<"\n";

}

int32_t
WorkshopCHOP::getNumInfoCHOPChans(void * reserved1)
{
	// We return the number of channel we want to output to any Info CHOP
	// connected to the CHOP. In this example we are just going to send one channel.
	return 2;
}

void
WorkshopCHOP::getInfoCHOPChan(int32_t index,
										OP_InfoCHOPChan* chan,
										void* reserved1)
{
	// This function will be called once for each channel we said we'd want to return
	// In this example it'll only be called once.


}

bool		
WorkshopCHOP::getInfoDATSize(OP_InfoDATSize* infoSize, void* reserved1)
{
	infoSize->rows = 2;
	infoSize->cols = 3;
	// Setting this to false means we'll be assigning values to the table
	// one row at a time. True means we'll do it one column at a time.
	infoSize->byColumn = false;
	return true;
}

void
WorkshopCHOP::getInfoDATEntries(int32_t index,
										int32_t nEntries,
										OP_InfoDATEntries* entries, 
										void* reserved1)
{
	char tempBuffer[4096];

	if (index == 0)
	{
		// Set the value for the first column
		entries->values[0]->setString("executeCount");

		// Set the value for the second column
#ifdef _WIN32
		sprintf_s(tempBuffer, "%d", myExecuteCount);
#else // macOS
        snprintf(tempBuffer, sizeof(tempBuffer), "%d", myExecuteCount);
#endif
		entries->values[1]->setString(tempBuffer);
	}

	if (index == 1)
	{
		// Set the value for the first column
		entries->values[0]->setString("offset");

		// Set the value for the second column

	}
}

void
WorkshopCHOP::setupParameters(OP_ParameterManager* manager, void *reserved1)
{
	// speed
	{
		OP_StringParameter	np;

		np.name = "Printfilename";
		np.label = "PrintFileName";
		
		OP_ParAppendResult res = manager->appendString(np);
		assert(res == OP_ParAppendResult::Success);
	}



	// pulse
	{
		OP_NumericParameter	np;

		np.name = "Reset";
		np.label = "Reset";
		
		OP_ParAppendResult res = manager->appendPulse(np);
		assert(res == OP_ParAppendResult::Success);
	}

}

void 
WorkshopCHOP::pulsePressed(const char* name, void* reserved1)
{
	if (!strcmp(name, "Reset"))
	{
		PrintCostm(Printfilename);

	}
}




