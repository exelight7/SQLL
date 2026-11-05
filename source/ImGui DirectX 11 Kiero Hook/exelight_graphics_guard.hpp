#pragma once
#include <atomic>
#include <mutex>
namespace Exelight {
inline std::recursive_mutex graphicsMutex;
inline std::atomic<unsigned> graphicsResizing{0};
struct GraphicsResizeScope {
    GraphicsResizeScope(){graphicsResizing.fetch_add(1,std::memory_order_acq_rel);}
    ~GraphicsResizeScope(){graphicsResizing.fetch_sub(1,std::memory_order_acq_rel);}
    GraphicsResizeScope(const GraphicsResizeScope&)=delete;
    GraphicsResizeScope& operator=(const GraphicsResizeScope&)=delete;
};
}
