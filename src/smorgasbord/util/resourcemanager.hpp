#ifndef RESOURCEMANAGER_HPP
#define RESOURCEMANAGER_HPP

#include <string>
#include <memory>
#include <vector>

/// TODO: ResourceReference::Get(): support paths which start with '/' or ':'.
///		The latter could signal an absolute path within the same package file
/// TODO: Support for some package archive format (gzip?). Then paths could
///		be like this: "basepath/path/to.package:path/within/package/to.asset"

using namespace std;

namespace Smorgasbord {

class ResourceManager;
class Device;

class ResourceReference
{
	weak_ptr<ResourceManager> rmRef;
	string path;
	
public:
	ResourceReference();
	ResourceReference(
		shared_ptr<ResourceManager> resourceManager,
		const string &relativePath);
	
	/// Get resource relative to the stored path
	ResourceReference Get(string relativePath);
	unique_ptr<istream> OpenRead();
	void ReadInto(stringstream &stream);
	string GetTextContents();
	vector<uint8_t> GetBinaryContents();
	
	operator unique_ptr<istream>()
	{
		return OpenRead();
	}
};

class ResourceManager : public enable_shared_from_this<ResourceManager>
{
private:
	string basepath;
	
public:
	ResourceManager(string basepath = "");
	
	string GetPath(string path);
	unique_ptr<istream> OpenRead(string path);
	
	string GetTextContents(string path)
	{
		return Get(path).GetTextContents();
	}
	
	vector<uint8_t> GetBinaryContents(string path)
	{
		return Get(path).GetBinaryContents();
	}
	
	ResourceReference Get(string path)
	{
		return { shared_from_this(), path };
	}
};

struct InternalBase
{
	shared_ptr<ResourceManager> r;
	shared_ptr<Device> d;
	
	InternalBase(
		shared_ptr<ResourceManager> resourceManager,
		shared_ptr<Device> device)
		: r(resourceManager), d(device)
	{ }
};

}

#endif // RESOURCEMANAGER_HPP
