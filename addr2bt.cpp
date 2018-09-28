#include <iostream>
#include <vector>
#include <map>
#include "BacktraceMap.h"


int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("Usage: %s <mapFileName> <addr>\n", argv[0]);
        return 0;
    }

    std::string mapFileName = argv[1];
    BacktraceMap* map = new BacktraceMap(mapFileName);        
    if (!map->Build()) {
        printf("parse map file failed!\n");
        return 0;
    }

    std::vector<uintptr_t> offsets;
    std::vector<std::string> libs;
    std::vector<std::pair<uintptr_t, std::string>> bt;
    for (int i = 2; i < argc; ++i) {
        uintptr_t addr = strtoll(argv[i], nullptr, 16);
        const backtrace_map_t* t = map->Find(addr);
        if (t == NULL) {
            printf("can't find addr %p in map file!\n", (void*)addr);
            continue;
        }

        uintptr_t offset = addr - t->start;
        bt.emplace_back(offset, t->name);
    }

    if (bt.size() == 0)
        return 0;

    printf("============================= backtrace ================================\n");
    for (auto& p : bt) {
        printf("%50s \t\t%08lx\n", p.second.c_str(), p.first);
    }

    return 0;
}



