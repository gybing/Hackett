#pragma once
#include "AltQFileIndexer.h"
#include "Singletons.h"
#include <set>
#include <string>
#include <vector>

class AltQFileIndexerFast : public Singletons<AltQFileIndexerFast>, public AltQFileIndexer
{
protected:
	virtual std::vector<std::pair<std::string, int>> GetIndexDirs();
	virtual std::set<std::string> GetIndexExts();
	virtual bool IsTempIndex(){ return true; }

protected:
	std::vector<std::pair<std::string, int>> GetSystemDefineDirs();
	std::vector<std::pair<std::string, int>> GetRegistryDirs();

	std::string GetSysDir(int id);

protected:
	friend class Singletons<AltQFileIndexerFast>;
};


class AltQFileIndexerWhole : public Singletons<AltQFileIndexerWhole>, public AltQFileIndexer
{
protected:
	virtual std::vector<std::pair<std::string, int>> GetIndexDirs();
	virtual std::set<std::string> GetIndexExts();
	virtual int IndexInterval(){ return 1; }

protected:
	friend class Singletons<AltQFileIndexerWhole>;
};
