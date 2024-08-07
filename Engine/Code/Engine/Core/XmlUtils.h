﻿// Bradley Christensen - 2022-2023
#pragma once
#include "ThirdParty/tinyxml2/tinyxml2.h"
#include "Engine/Math/Vec2.h"
#include "Engine/Renderer/Rgba8.h"
#include <string>



//----------------------------------------------------------------------------------------------------------------------
// XmlUtils
//
typedef tinyxml2::XMLAttribute	XmlAttribute;
typedef tinyxml2::XMLDocument	XmlDocument;
typedef tinyxml2::XMLElement	XmlElement;
typedef tinyxml2::XMLError		XmlError;



//----------------------------------------------------------------------------------------------------------------------
// Parse Xml attributes by name
//
std::string ParseXmlAttribute(XmlElement const& element, char const* attributeName, char const*		   defaultValue);
std::string ParseXmlAttribute(XmlElement const& element, char const* attributeName, std::string const& defaultValue);
float ParseXmlAttribute(XmlElement const& element, char const* attributeName, float defaultValue);
bool ParseXmlAttribute(XmlElement const& element, char const* attributeName, bool defaultValue);
int ParseXmlAttribute(XmlElement const& element, char const* attributeName, int defaultValue);
Vec2 ParseXmlAttribute(XmlElement const& element, char const* attributeName, Vec2 const& defaultValue);
Rgba8 ParseXmlAttribute(XmlElement const& element, char const* attributeName, Rgba8 const& defaultValue);