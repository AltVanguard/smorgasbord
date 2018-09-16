#include "resourcemanager.hpp"

#include "log.hpp"

#include <fstream>
#include <sstream>

Smorgasbord::ResourceReference::ResourceReference()
{ }

Smorgasbord::ResourceReference::ResourceReference(
	shared_ptr<Smorgasbord::ResourceManager> resourceManager,
	const string &relativePath)
	: rmRef(resourceManager), path(relativePath)
{ }

Smorgasbord::ResourceReference Smorgasbord::ResourceReference::Get(
	string relativePath)
{
	/// absolute path within ResourceManager, not necessarily truly absolute
	string absolutePath;
	
	/// ':' is there because ResourceManager later might support
	/// packeged assets so the absolute path could even be like
	/// base/path/path/to.package:relative/path/to.asset
	size_t lastSlashPos = this->path.find_last_of("\\/:");
	if (lastSlashPos == string::npos)
	{
		absolutePath = relativePath;
	}
	else
	{
		absolutePath = this->path.substr(0, lastSlashPos + 1)
			+ relativePath;
	}
	
	return rmRef.lock()->Get(absolutePath);
}

unique_ptr<istream> Smorgasbord::ResourceReference::OpenRead()
{
	shared_ptr<ResourceManager> rm = this->rmRef.lock();
	
	if (static_cast<bool>(rm))
	{
		return rm->OpenRead(this->path);
	}
	
	LogE("Couldn't open resource: ResourceManager no longer available");
	return { };
}

void Smorgasbord::ResourceReference::ReadInto(stringstream &stream)
{
	unique_ptr<istream> file = OpenRead();
	if (static_cast<bool>(file))
	{
		stream << file->rdbuf();
	}
	else
	{
		LogE("Couldn't open resource for reading");
	}
}

string Smorgasbord::ResourceReference::GetTextContents()
{
	unique_ptr<istream> file = OpenRead();
	if (static_cast<bool>(file))
	{
		return static_cast<stringstream const&>(
			stringstream() << file->rdbuf()).str();
	}
	else
	{
		LogE("Couldn't open resource for reading");
		return "";
	}
}

vector<uint8_t> Smorgasbord::ResourceReference::GetBinaryContents()
{
	unique_ptr<istream> file = OpenRead();
	if (static_cast<bool>(file))
	{
		return vector<uint8_t>(
			istreambuf_iterator<char>(*file),
			istreambuf_iterator<char>());
	}
	else
	{
		LogE("Couldn't open resource for reading");
		return { };
	}
}

Smorgasbord::ResourceManager::ResourceManager(string basepath)
{
	this->basepath = basepath;
}

string Smorgasbord::ResourceManager::GetPath(string path)
{
	return basepath + path;
}

unique_ptr<istream> Smorgasbord::ResourceManager::OpenRead(string path)
{
	unique_ptr<ifstream> file =
		make_unique<ifstream>(GetPath(path).c_str(), ios_base::binary);
	
	if (!file->is_open())
	{
		LogE("Cannot open file");
		return { };
	}
	
	return file;
}
