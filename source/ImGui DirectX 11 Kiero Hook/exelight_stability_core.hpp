#pragma once
#include <cstdint>
#include <cmath>
#include <optional>
#include <string>
#include <unordered_map>
#include <mutex>
#include <vector>

namespace Exelight {
template<class Tracer> inline void AppendBoundedTracer(std::vector<Tracer>& tracers,std::recursive_mutex& mutex,const Tracer& tracer) {
    std::lock_guard<std::recursive_mutex> lock(mutex);
    if(tracers.size()>=256)tracers.erase(tracers.begin());
    tracers.push_back(tracer);
}
template<class Reader> inline std::optional<std::uintptr_t> ClientBaseCandidate(std::int64_t raw,Reader&& read) {
    constexpr std::int64_t adjustment=3294544;
    if(raw<adjustment)return {};
    const auto candidate=static_cast<std::uintptr_t>(raw-adjustment);
    int local=-1;
    if(!candidate||!read(candidate,local)||local<0||local>=18)return {};
    return candidate;
}
template<class Point,class Reader> inline bool AcceptTracer(bool enabled,bool all,int source,Point start,Point end,Reader&& readLocal) {
    if(!enabled||source<0||source>=2048)return false;
    const auto finite=[](Point p){return std::isfinite(p.x)&&std::isfinite(p.y)&&std::isfinite(p.z);};
    if(!finite(start)||!finite(end))return false;
    int local=-1;
    return readLocal(local)&&local>=0&&local<18&&(all||source==local);
}
inline const char* StableNumberText(int number) {
    // Stable addresses across later calls and rehashes; repeated requests reuse storage.
    thread_local std::unordered_map<int,std::string> values;
    return values.try_emplace(number,std::to_string(number)).first->second.c_str();
}
}
