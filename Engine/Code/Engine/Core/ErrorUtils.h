// Bradley Christensen - 2022-2025
#pragma once
#include <string>



//----------------------------------------------------------------------------------------------------------------------
// ErrorUtils
//
// Helpful functions for error handling
//



//----------------------------------------------------------------------------------------------------------------------
enum class MsgSeverityLevel
{
    INFORMATION,
    QUESTION,
    WARNING,
    FATAL
};



//----------------------------------------------------------------------------------------------------------------------
__declspec(noreturn) void FatalError(char const* filePath, char const* functionName, int lineNum, std::string const& reasonForError, char const* conditionText = nullptr);
void SystemDialogue_Okay(std::string const& messageTitle, std::string const& messageText, MsgSeverityLevel severity);
bool SystemDialogue_YesNo(std::string const& messageTitle, std::string const& messageText, MsgSeverityLevel severity);
bool SystemDialogue_OkayCancel(std::string const& messageTitle, std::string const& messageText, MsgSeverityLevel severity);
int SystemDialogue_YesNoCancel(std::string const& messageTitle, std::string const& messageText, MsgSeverityLevel severity);



//----------------------------------------------------------------------------------------------------------------------
#define ERROR_AND_DIE(errorMessageText)															    \
{																									\
    FatalError(__FILE__,  __FUNCTION__, __LINE__, errorMessageText);								\
}



//----------------------------------------------------------------------------------------------------------------------
#define ERROR_RECOVERABLE(errorMessageText)														    \
{																									\
    RecoverableWarning(__FILE__,  __FUNCTION__, __LINE__, errorMessageText);						\
}



//----------------------------------------------------------------------------------------------------------------------
#define GUARANTEE_OR_DIE(condition, errorMessageText)												\
{																									\
    if(!(condition))																				\
    {																								\
        char const* conditionText = #condition;														\
        FatalError(__FILE__,  __FUNCTION__, __LINE__, errorMessageText, conditionText);			    \
    }																								\
}



//----------------------------------------------------------------------------------------------------------------------
#if defined(DISABLE_ASSERTS)
#define ASSERT_OR_DIE(condition, errorMessageText) { (void)(condition); }
#else
#define ASSERT_OR_DIE(condition, errorMessageText)												    \
{																									\
    if(!(condition))																				\
    {																								\
        char const* conditionText = #condition;														\
        FatalError(__FILE__,  __FUNCTION__, __LINE__, errorMessageText, conditionText);			    \
    }																								\
}
#endif