#define NOMINMAX
#include <windows.h>
#include <cassert>
#include <cstdio>
#include <deque>
#include <thread>
#include <vector>
#include <memory>
#include "exelight_optimization.hpp"
using namespace Exelight::Optimization;
struct Packet {unsigned char data[6]{};unsigned size=6;int sequence=-1;};
struct Screen {float x=0,y=0;};
struct Trace {std::uint64_t sequence=0;Screen start_screen,end_screen;bool projected=false;int radius=0;};
unsigned SimulatePeer(unsigned identity) {
    std::deque<Packet> input;
    for(int i=0;i<10000;++i) {
        Packet blocked;blocked.data[5]=i%2?'f':'e';input.push_back(blocked);
        Packet good;good.data[5]='h';good.sequence=i;input.push_back(good);
    }
    Packet current;unsigned valid=0,reads=0;
    while(ReadAcceptedPacket([&]{++reads;if(input.empty())return false;current=input.front();input.pop_front();return true;},
        [&]{return LegacyPacketKind(current.data,current.size)==PacketKind::Lobby;},[&]{current.size=0;})) {
        assert(current.sequence==static_cast<int>(valid));
        const char payload[]={'1','h',static_cast<char>(identity),static_cast<char>(valid)};
        MessageScope handling(identity,payload,sizeof(payload));assert(handling);
        MessageScope reentry(identity,payload,sizeof(payload));assert(!reentry);
        ++valid;
    }
    assert(valid==10000&&input.empty()&&reads==20001&&current.size==0);
    return valid;
}
int main() {
    auto pages=static_cast<unsigned char*>(VirtualAlloc(nullptr,8192,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));assert(pages);
    DWORD previous{};assert(VirtualProtect(pages+4096,4096,PAGE_NOACCESS,&previous));
    for(unsigned length=0;length<6;++length)assert(LegacyPacketKind(pages+4096-length,length)==PacketKind::Other);
    assert(VirtualFree(pages,0,MEM_RELEASE));
    int nativeReads=0;unsigned length=1;
    assert(!ReadAcceptedPacket([&]{++nativeReads;return true;},[]{return false;},[&]{length=0;})&&nativeReads==16&&length==0);
    int remainingBlocked=20;bool goodPending=true,goodCurrent=false;
    auto readNext=[&]{if(remainingBlocked){--remainingBlocked;goodCurrent=false;return true;}if(goodPending){goodPending=false;goodCurrent=true;return true;}return false;};
    assert(!ReadAcceptedPacket(readNext,[&]{return goodCurrent;},[]{}));
    assert(ReadAcceptedPacket(readNext,[&]{return goodCurrent;},[]{}));
    const char message[]="1hfixture";
    {MessageScope a(1,message,sizeof(message));assert(a);MessageScope duplicate(1,message,sizeof(message));assert(!duplicate);MessageScope other(2,message,sizeof(message));assert(other);}
    {MessageScope again(1,message,sizeof(message));assert(again);}
    assert(!MessageScope(1,nullptr,12));assert(!MessageScope(1,message,2051));
    std::vector<std::unique_ptr<MessageScope>> scopes;
    for(int i=0;i<8;++i){scopes.push_back(std::make_unique<MessageScope>(i+1,message,sizeof(message)));assert(*scopes.back());}
    assert(!MessageScope(100,message,sizeof(message)));while(!scopes.empty())scopes.pop_back();
    PeerCooldown<> notices;AtomicCooldown budget;std::atomic<int> admitted{0},waves{0};std::vector<std::thread> workers;
    for(int i=0;i<8;++i)workers.emplace_back([&]{for(int j=0;j<1000;++j){if(notices.Allow(7,1,1000,3000))++admitted;if(budget.Try(1000,500))++waves;}});
    for(auto& worker:workers)worker.join();assert(admitted==1&&waves==1);
    assert(notices.Allow(8,1,1000,3000));assert(notices.Allow(7,2,1000,3000));assert(notices.Allow(7,1,4000,3000));
    for(unsigned peer=0;peer<10000;++peer)notices.Allow(peer,3,5000,1000);
    PeriodicTask pointers,lobby,friendViews;unsigned pointerCalls=0,lobbyCalls=0,viewCalls=0;
    for(unsigned now=0;now<10000;now+=4){pointerCalls+=pointers.Due(now,50);lobbyCalls+=lobby.Due(now,2000);viewCalls+=friendViews.Due(now,250);}
    assert(pointerCalls<=201&&lobbyCalls==5&&viewCalls<=40);
    pointers.Reset();assert(pointers.Due(10000,50));
    AtomicCooldown bursts;unsigned burstCalls=0;for(unsigned now=0;now<10000;++now)burstCalls+=bursts.Try(now,500);assert(burstCalls==20);
    assert(ScaledDamage(100,5)==500&&ScaledDamage(INT_MAX,100000)==INT_MAX&&ScaledDamage(INT_MIN,-1)==INT_MAX&&ScaledDamage(INT_MIN,2)==INT_MIN);
    std::vector<Trace> traces;std::recursive_mutex traceMutex;std::uint64_t serial=0;
    for(int i=0;i<300;++i)AppendSequencedTracer(traces,traceMutex,Trace{},serial);
    auto stale=traces;for(auto& item:stale){item.projected=true;item.start_screen.x=static_cast<float>(item.sequence);}
    for(int i=0;i<10;++i)AppendSequencedTracer(traces,traceMutex,Trace{},serial);
    PublishProjections(traces,traceMutex,stale);
    for(const auto& item:traces)assert(item.sequence<=300?item.projected&&item.start_screen.x==item.sequence:!item.projected);
    std::atomic<int> finished{};workers.clear();
    for(int i=0;i<8;++i)workers.emplace_back([&]{for(int j=0;j<1000;++j)AppendSequencedTracer(traces,traceMutex,Trace{},serial);++finished;});
    unsigned frames=0;
    while(finished!=8||frames<20){
        std::vector<Trace> snapshot;{std::lock_guard<std::recursive_mutex> lock(traceMutex);snapshot=traces;}
        for(auto& item:snapshot){item.start_screen.x=static_cast<float>(item.sequence);item.projected=true;}
        PublishProjections(traces,traceMutex,snapshot);
        {std::lock_guard<std::recursive_mutex> lock(traceMutex);assert(traces.size()<=256);std::uint64_t last=0;for(const auto& item:traces){assert(item.sequence>last);last=item.sequence;if(item.projected)assert(item.start_screen.x==item.sequence);}}
        ++frames;
    }
    for(auto& worker:workers)worker.join();
    bool flag=false;{ScopedFlag outer(flag);assert(outer&&flag);{ScopedFlag nested(flag);assert(!nested&&flag);}assert(flag);}assert(!flag);
    FilteredRows filtered;std::vector<int> rows(200,1);int scans=0;
    const auto match=[&](int value){++scans;return value==1;};
    assert(filtered.Update(rows,1,"",match).size()==200&&scans==200);
    filtered.Update(rows,1,"",match);assert(scans==200);
    rows.resize(5);const auto& shown=filtered.Update(rows,2,"",match);assert(shown.size()==5&&shown.back()==4);
    rows[0]=0;assert(filtered.Update(rows,2,"changed",match).size()==4);
    unsigned first=0,second=0;std::thread a([&]{first=SimulatePeer(1);}),b([&]{second=SimulatePeer(2);});a.join();b.join();assert(first+second==20000);
    printf("PASS: 20000 normal synthetic lobby messages forwarded by two independent peers, interleaved with 20000 filtered packets; no reentrant duplicate dispatch; normal traffic has no rate limiter.\n");
    printf("PASS: short packet guard pages; finite 16-read drain; eight-thread notice/burst gates; saturating damage.\n");
    printf("PASS: sequenced projection publication ignored replaced traces; 8000 concurrent insertions and snapshot publication retained correct identities; recursive projection scope rejected.\n");
    printf("PASS: filtered row cache invalidated after hidden-list shrink and query change; cached frames performed no new filtering scans.\n");
    printf("Synthetic 10-second 250-Hz workload: pointer refresh %u versus 2500; lobby batches %u versus 2500; friend view rebuilds %u versus 2500; nuke bursts 20 versus 10000. These are operation counts, not measured game FPS.\n",pointerCalls,lobbyCalls,viewCalls);
}
