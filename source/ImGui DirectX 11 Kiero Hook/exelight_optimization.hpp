#pragma once
#include <array>
#include <atomic>
#include <cstdint>
#include <cstddef>
#include <limits>
#include <mutex>
#include <vector>
#include <string>
#include <string_view>

namespace Exelight::Optimization {
class FilteredRows {
    std::vector<std::size_t> indices;
    std::uint64_t revision=0;std::string query;bool initialized=false;
public:
    template<class Items,class Predicate> const std::vector<std::size_t>& Update(const Items& items,std::uint64_t current,std::string_view filter,Predicate&& matches) {
        if(!initialized||revision!=current||query!=filter) {
            indices.clear();indices.reserve(items.size());
            for(std::size_t i=0;i<items.size();++i)if(matches(items[i]))indices.push_back(i);
            revision=current;query=filter;initialized=true;
        }
        return indices;
    }
};
class ScopedFlag {
    bool& flag;bool entered;
public:
    explicit ScopedFlag(bool& value):flag(value),entered(!value){if(entered)flag=true;}
    ~ScopedFlag(){if(entered)flag=false;}
    explicit operator bool()const{return entered;}
    ScopedFlag(const ScopedFlag&)=delete;
};
template<class Tracer> void AppendSequencedTracer(std::vector<Tracer>& items,std::recursive_mutex& mutex,Tracer item,std::uint64_t& serial) {
    std::lock_guard<std::recursive_mutex> lock(mutex);
    item.sequence=++serial;
    if(items.size()>=256)items.erase(items.begin());
    items.push_back(item);
}
template<class Tracer> void PublishProjections(std::vector<Tracer>& items,std::recursive_mutex& mutex,const std::vector<Tracer>& projected) {
    std::lock_guard<std::recursive_mutex> lock(mutex);
    std::size_t current=0,old=0;
    while(current<items.size()&&old<projected.size()) {
        if(items[current].sequence<projected[old].sequence){++current;continue;}
        if(projected[old].sequence<items[current].sequence){++old;continue;}
        auto& item=items[current++];const auto& output=projected[old++];
        item.start_screen=output.start_screen;item.end_screen=output.end_screen;
        item.projected=output.projected;item.radius=output.radius;
    }
}
class PeriodicTask {
    std::uint64_t last=0;bool initialized=false;
public:
    bool Due(std::uint64_t now,std::uint64_t interval) {
        if(initialized&&now>=last&&now-last<interval)return false;
        initialized=true;last=now;return true;
    }
    void Reset(){initialized=false;}
};
class AtomicCooldown {
    std::atomic<std::uint64_t> next{0};
public:
    bool Try(std::uint64_t now,std::uint64_t interval) {
        auto value=next.load(std::memory_order_relaxed);
        while(now>=value) {
            if(next.compare_exchange_weak(value,now+interval,std::memory_order_acq_rel))return true;
        }
        return false;
    }
};
template<std::size_t Capacity=128> class PeerCooldown {
    struct Entry {std::uint64_t peer=0,kind=0,last=0;bool used=false;};
    std::array<Entry,Capacity> entries{};std::mutex mutex;
public:
    bool Allow(std::uint64_t peer,std::uint64_t kind,std::uint64_t now,std::uint64_t interval) {
        std::lock_guard<std::mutex> lock(mutex);
        Entry* oldest=&entries[0];
        for(auto& entry:entries) {
            if(entry.used&&entry.peer==peer&&entry.kind==kind) {
                if(now>=entry.last&&now-entry.last<interval)return false;
                entry.last=now;return true;
            }
            if(!entry.used){oldest=&entry;break;}
            if(entry.last<oldest->last)oldest=&entry;
        }
        *oldest=Entry{peer,kind,now,true};return true;
    }
};
enum class PacketKind {Other,Lobby,Popup,RemoteCommand};
inline PacketKind LegacyPacketKind(const unsigned char* data,std::size_t size) {
    if(!data||size<6)return PacketKind::Other;
    switch(data[5]) {
    case 'h':return PacketKind::Lobby;
    case 'f':return PacketKind::Popup;
    case 'e':return PacketKind::RemoteCommand;
    default:return PacketKind::Other;
    }
}
template<class Read,class Accept,class Clear> bool ReadAcceptedPacket(Read&& read,Accept&& accept,Clear&& clear) {
    // A blocked packet must not make a valid following packet look unavailable.
    // A finite budget prevents an endless drain while another peer keeps sending.
    for(int i=0;i<16;++i) {
        if(!read()){clear();return false;}
        if(accept())return true;
    }
    clear();return false;
}
inline std::uint64_t Fingerprint(const char* bytes,std::size_t size) {
    std::uint64_t hash=1469598103934665603ULL;
    for(std::size_t i=0;i<size;++i)hash=(hash^static_cast<unsigned char>(bytes[i]))*1099511628211ULL;
    return hash;
}
class MessageScope {
    struct Key {std::uint64_t sender,hash;std::size_t size;};
    inline static thread_local std::array<Key,8> stack{};
    inline static thread_local std::size_t depth=0;
    bool entered=false;
public:
    MessageScope(std::uint64_t sender,const char* data,std::size_t size) {
        if(!data||size<2||size>2050||depth>=stack.size())return;
        const auto hash=Fingerprint(data,size);
        for(std::size_t i=0;i<depth;++i)if(stack[i].sender==sender&&stack[i].hash==hash&&stack[i].size==size)return;
        stack[depth++]={sender,hash,size};entered=true;
    }
    ~MessageScope(){if(entered)--depth;}
    explicit operator bool()const{return entered;}
    MessageScope(const MessageScope&)=delete;
    MessageScope& operator=(const MessageScope&)=delete;
};
inline int ScaledDamage(int value,int multiplier) {
    const auto result=std::int64_t(value)*multiplier;
    if(result>std::numeric_limits<int>::max())return std::numeric_limits<int>::max();
    if(result<std::numeric_limits<int>::min())return std::numeric_limits<int>::min();
    return static_cast<int>(result);
}
}
