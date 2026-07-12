# Copilot Instructions

## Project Guidelines
- For time-related string formatting in this codebase, always use Time::GetDisplayString(double seconds) instead of creating custom StringF calls. This utility automatically chooses appropriate units (ms, sec, min, hr, day, year) and precision.