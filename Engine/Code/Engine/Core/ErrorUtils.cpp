// Bradley Christensen - 2022-2025
#include "Engine/Core/ErrorUtils.h"
#include "Engine/Core/StringUtils.h"
#include <mutex>



#ifdef _WIN32
#define PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif



//-----------------------------------------------------------------------------------------------
// Converts a SeverityLevel to a Windows MessageBox icon type (MB_etc)
//
#if defined(PLATFORM_WINDOWS)
UINT GetWindowsMessageBoxIconFlagForSeverityLevel(MsgSeverityLevel severity)
{
	switch(severity)
	{
		case MsgSeverityLevel::INFORMATION:		return MB_ICONASTERISK;		// blue circle with 'i' in Windows 7
		case MsgSeverityLevel::QUESTION:		return MB_ICONQUESTION;		// blue circle with '?' in Windows 7
		case MsgSeverityLevel::WARNING:			return MB_ICONEXCLAMATION;	// yellow triangle with '!' in Windows 7
		case MsgSeverityLevel::FATAL:			return MB_ICONHAND;			// red circle with 'x' in Windows 7
		default:								return MB_ICONEXCLAMATION;
	}
}
#endif



//----------------------------------------------------------------------------------------------------------------------
__declspec(noreturn) void FatalError(char const* filePath, char const* functionName, int lineNum, std::string const& reasonForError, char const* conditionText)
{
	static std::mutex errorMutex;
	std::unique_lock scopeLock(errorMutex);
	
    std::string errorMessage = reasonForError;
	if(reasonForError.empty())
	{
		if(conditionText)
			errorMessage = StringUtils::StringF("ERROR: \"%s\" is false!", conditionText);
		else
			errorMessage = "Unspecified fatal error";
	}

	std::string appName = "Game"; // todo: get the correct app name
	std::string fullMessageTitle = appName + " :: Error";
	std::string fullMessageText = errorMessage;
	fullMessageText += "\n\nThe application will now close.\n";
	bool isDebuggerPresent = (IsDebuggerPresent() == TRUE);
	if(isDebuggerPresent)
	{
		fullMessageText += "\nDEBUGGER DETECTED!\nWould you like to break and debug?\n  (Yes=debug, No=quit)\n";
	}

	fullMessageText += "\n---------- Debugging Details Follow ----------\n";
	if (conditionText)
	{
		fullMessageText += StringUtils::StringF("\nThis error was triggered by a run-time condition check:\n  %s\n  from %s(), line %i in %s\n",
			conditionText, functionName, lineNum, filePath);
	}
	else
	{
		fullMessageText += StringUtils::StringF("\nThis was an unconditional error triggered by reaching\n line %i of %s, in %s()\n",
			lineNum, filePath, functionName);
	}

	#if defined(PLATFORM_WINDOWS)
	{
		if (isDebuggerPresent)
		{
			bool isAnswerYes = SystemDialogue_YesNo(fullMessageTitle, fullMessageText, MsgSeverityLevel::FATAL);

			ShowCursor(TRUE);
			if (isAnswerYes)
			{
				__debugbreak();
			}
		}
		else
		{
			SystemDialogue_Okay(fullMessageTitle, fullMessageText, MsgSeverityLevel::FATAL);
			ShowCursor(TRUE);
		}
	}
	#endif

	exit(0);
}



//----------------------------------------------------------------------------------------------------------------------
void SystemDialogue_Okay(std::string const& messageTitle, std::string const& messageText, MsgSeverityLevel severity)
{
	#if defined(PLATFORM_WINDOWS)
	{
		ShowCursor(TRUE);
		UINT dialogueIconTypeFlag = GetWindowsMessageBoxIconFlagForSeverityLevel(severity);
		MessageBoxA(NULL, messageText.c_str(), messageTitle.c_str(), MB_OK | dialogueIconTypeFlag | MB_TOPMOST);
		ShowCursor(FALSE);
	}
	#endif
}



//----------------------------------------------------------------------------------------------------------------------
bool SystemDialogue_YesNo(std::string const& messageTitle, std::string const& messageText, MsgSeverityLevel severity)
{
	bool isAnswerYes = true;

	#if defined(PLATFORM_WINDOWS)
	{
		ShowCursor(TRUE);
		UINT dialogueIconTypeFlag = GetWindowsMessageBoxIconFlagForSeverityLevel(severity);
		int buttonClicked = MessageBoxA(NULL, messageText.c_str(), messageTitle.c_str(), MB_YESNO | dialogueIconTypeFlag | MB_TOPMOST);
		isAnswerYes = (buttonClicked == IDYES);
		ShowCursor(FALSE);
	}
	#endif

	return isAnswerYes;
}



//----------------------------------------------------------------------------------------------------------------------
bool SystemDialogue_OkayCancel(std::string const& messageTitle, std::string const& messageText,
	MsgSeverityLevel severity)
{
	bool isAnswerOkay = true;

	#if defined(PLATFORM_WINDOWS)
	{
		ShowCursor(TRUE);
		UINT dialogueIconTypeFlag = GetWindowsMessageBoxIconFlagForSeverityLevel(severity);
		int buttonClicked = MessageBoxA(NULL, messageText.c_str(), messageTitle.c_str(), MB_OKCANCEL | dialogueIconTypeFlag | MB_TOPMOST);
		isAnswerOkay = (buttonClicked == IDOK);
		ShowCursor(FALSE);
	}
	#endif

	return isAnswerOkay;
}



//----------------------------------------------------------------------------------------------------------------------
int SystemDialogue_YesNoCancel(std::string const& messageTitle, std::string const& messageText,
	MsgSeverityLevel severity)
{
	bool isAnswerYes = true;

	#if defined(PLATFORM_WINDOWS)
	{
		ShowCursor(TRUE);
		UINT dialogueIconTypeFlag = GetWindowsMessageBoxIconFlagForSeverityLevel(severity);
		int buttonClicked = MessageBoxA(NULL, messageText.c_str(), messageTitle.c_str(), MB_YESNO | dialogueIconTypeFlag | MB_TOPMOST);
		isAnswerYes = (buttonClicked == IDYES);
		ShowCursor(FALSE);
	}
	#endif

	return isAnswerYes;
}
