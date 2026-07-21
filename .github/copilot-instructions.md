# Copilot Instructions

## Project Guidelines

* For time-related string formatting in this codebase, always use Time::GetDisplayString(double seconds) instead of creating custom StringF calls. This utility automatically chooses appropriate units (ms, sec, min, hr, day, year) and precision.
* Add //---------------------------------------------------------------------------------------------------------------------- the line before every class and function definition, in both headers and cpps
* Use 3 returns separating each function call, class definition, or other things like forward declarations.
* Add a return after "public:" and "private:" within classes and structs
* Add // Bradley Christensen - 2022-2026 as the top line of every file
* always add both virtual and override keywords to virtual function overrides
* Where possible, keep includes alphabetized, with the exception of keeping the main cpp include first, then all other Game includes, then all Engine includes, then any external includes like <std>
* Also keep forward declarations alphabetized, including having classes before structs
* Remove unnecessary includes and forward declarations where possible, but always make sure to Include What You Use rather than relying on other files for includes that they bring along with them.
* Use right-const almost always

