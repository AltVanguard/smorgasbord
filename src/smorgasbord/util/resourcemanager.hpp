#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

#include <string>
#include <memory>
#include <vector>

/// TODO: ResourceReference::Get(): support paths which start with '/' or ':'.
///		The latter could signal an absolute path within the same package file
/// TODO: Support for some package archive format (gzip?). Then paths could
///		be like this: "basepath/path/to.package:path/within/package/to.asset"

namespace Smorgasbord {

class ResourceManager;
class Device;

class ResourceReference
{
	std::weak_ptr<ResourceManager> rmRef;
	std::string path;
	
public:
	ResourceReference();
	ResourceReference(
		std::shared_ptr<ResourceManager> resourceManager,
		const std::string &relativePath);
	
	/// Get resource relative to the stored path
	ResourceReference Get(std::string relativePath);
	std::unique_ptr<std::istream> OpenRead();
	void ReadInto(std::stringstream &stream);
	std::string GetTextContents();
	std::vector<uint8_t> GetBinaryContents();
	
	operator std::unique_ptr<std::istream>()
	{
		return OpenRead();
	}
};

class ResourceManager : public std::enable_shared_from_this<ResourceManager>
{
private:
	std::string basepath;
	
public:
	ResourceManager(std::string basepath = "");
	
	std::string GetPath(std::string path);
	std::unique_ptr<std::istream> OpenRead(std::string path);
	
	std::string GetTextContents(std::string path)
	{
		return Get(path).GetTextContents();
	}
	
	std::vector<uint8_t> GetBinaryContents(std::string path)
	{
		return Get(path).GetBinaryContents();
	}
	
	ResourceReference Get(std::string path)
	{
		return { shared_from_this(), path };
	}
};

struct InternalBase
{
	std::shared_ptr<ResourceManager> r;
	std::shared_ptr<Device> d;
	
	InternalBase(
		std::shared_ptr<ResourceManager> resourceManager,
		std::shared_ptr<Device> device)
		: r(resourceManager), d(device)
	{ }
};

}

#endif // RESOURCEMANAGER_HPP
