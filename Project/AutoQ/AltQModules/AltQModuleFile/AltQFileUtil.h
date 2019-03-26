#pragma once
#include "json.h"
#include "AltQModuleFileDefine.h"

void AltQFileToJson(const AltQFile& file, std::string& jsonstring);
bool JsonToAltQFile(const std::string& jsonstring, AltQFile& file);

void SubstractFileName(const std::string& file, std::string& filename, std::string& fileext);
