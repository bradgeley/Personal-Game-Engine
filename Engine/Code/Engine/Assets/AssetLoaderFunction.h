// Bradley Christensen - 2022-2025
#pragma once
#include "Engine/Core/Name.h"
#include <functional>



//----------------------------------------------------------------------------------------------------------------------
typedef std::function<class Asset* (Name)> AssetLoaderFunction;