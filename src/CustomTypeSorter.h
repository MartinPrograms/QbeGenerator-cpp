#ifndef QBEIR_CUSTOMTYPESORTER_H
#define QBEIR_CUSTOMTYPESORTER_H
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <functional>

#include "Types.h"

namespace Qbe {
    class CustomTypeSorter {
    public:
        static std::vector<CustomType*> SortTypes(const std::vector<CustomType*>& types) {
            std::pmr::unordered_map<CustomType*, std::vector<CustomType*>> graph;

            for (auto* type : types) {
                graph[type];
            }

            for (auto* type : types) {
                for (const auto& field : type->fields) {
                    if (auto* fieldType = dynamic_cast<CustomType*>(field.value)) {
                        graph[type].push_back(fieldType);
                    }
                }
            }

            std::vector<CustomType*> sorted;
            std::unordered_set<CustomType*> visited;
            std::unordered_set<CustomType*> visiting;

            std::function<void(CustomType*)> visit = [&](CustomType* type) {
                if (visited.count(type)) {
                    return;
                }
                if (visiting.count(type)) {
                    throw std::runtime_error("Cyclic dependency detected in custom types");
                }

                visiting.insert(type);
                for (auto* neighbor : graph[type]) {
                    visit(neighbor);
                }
                visiting.erase(type);
                visited.insert(type);
                sorted.push_back(type);
            };

            for (auto* type : types) {
                visit(type);
            }

            return sorted;
        }
    };
}

#endif //QBEIR_CUSTOMTYPESORTER_H