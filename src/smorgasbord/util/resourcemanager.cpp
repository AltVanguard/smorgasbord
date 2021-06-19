#include "resourcemanager.hpp"

#include "log.hpp"

#include <fstream>
#include <sstream>

Smorgasbord::ResourceReference::ResourceReference()
{ }

Smorgasbord::ResourceReference::ResourceReference(
	std::shared_ptr<Smorgasbord::ResourceManager> resourceManager,
	const std::string &relativePath)
	: rmRef(resourceManager), path(relativePath)
{ }

Smorgasbord::ResourceReference Smorgasbord::ResourceReference::Get(
	std::string relativePath)
{
	/// absolute path within ResourceManager, not necessarily truly absolute
	std::string absolutePath;
	
	/// ':' is there because ResourceManager later might support
	/// packeged assets so the absolute path could even be like
	/// base/path/path/to.package:relative/path/to.asset
	size_t lastSlashPos = this->path.find_last_of("\\/:");
	if (lastSlashPos == std::string::npos)
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

std::unique_ptr<std::istream> Smorgasbord::ResourceReference::OpenRead()
{
	std::shared_ptr<ResourceManager> rm = this->rmRef.lock();
	
	if (static_cast<bool>(rm))
	{
		return rm->OpenRead(this->path);
	}
	
	LogE("Couldn't open resource: ResourceManager no longer available");
	return { };
}

void Smorgasbord::ResourceReference::ReadInto(std::stringstream &stream)
{
	std::unique_ptr<std::istream> file = OpenRead();
	if (static_cast<bool>(file))
	{
		stream << file->rdbuf();
	}
	else
	{
		LogE("Couldn't open resource for reading");
	}
}

std::string Smorgasbord::ResourceReference::GetTextContents()
{
	std::unique_ptr<std::istream> file = OpenRead();
	if (static_cast<bool>(file))
	{
		return static_cast<std::stringstream const&>(
			std::stringstream() << file->rdbuf()).str();
	}
	else
	{
		LogE("Couldn't open resource for reading");
		return "";
	}
}

std::vector<uint8_t> Smorgasbord::ResourceReference::GetBinaryContents()
{
	std::unique_ptr<std::istream> file = OpenRead();
	if (static_cast<bool>(file))
	{
		return std::vector<uint8_t>(
			std::istreambuf_iterator<char>(*file),
			std::istreambuf_iterator<char>());
	}
	else
	{
		LogE("Couldn't open resource for reading");
		return { };
	}
}

Smorgasbord::ResourceManager::ResourceManager(std::string basepath)
{
	this->basepath = basepath;
}

std::string Smorgasbord::ResourceManager::GetPath(std::string path)
{
	return basepath + path;
}

std::unique_ptr<std::istream> Smorgasbord::ResourceManager::OpenRead(std::string path)
{
	std::string relativePath = GetPath(path);
	
	std::unique_ptr<std::ifstream> file =
		std::make_unique<std::ifstream>(relativePath.c_str(), std::ios_base::binary);
	
	if (!file->is_open())
	{
		LogE("Cannot open file {0}", relativePath);
		return { };
	}
	
	return file;
}
