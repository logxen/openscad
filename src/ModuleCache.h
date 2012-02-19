#include <string>
#include <boost/unordered_map.hpp>

class ModuleCache
{
public:
	static ModuleCache *instance() { if (!inst) inst = new ModuleCache; return inst; }
	class Module *evaluate(const std::string &filename);
	size_t size() { return this->entries.size(); }
	void clear();

private:
	ModuleCache() {}
	~ModuleCache() {}

	static ModuleCache *inst;

	struct cache_entry {
		class Module *module;
		std::string cache_id;
	};
	boost::unordered_map<std::string, cache_entry> entries;
};
