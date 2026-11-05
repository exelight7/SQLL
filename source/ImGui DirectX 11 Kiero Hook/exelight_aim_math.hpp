#pragma once
#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

namespace Exelight::AimMath {
struct Vec2 {float x{},y{};};
struct Vec3 {float x{},y{},z{};};
inline bool Finite(Vec2 v) {return std::isfinite(v.x)&&std::isfinite(v.y);}
inline bool Finite(Vec3 v) {return std::isfinite(v.x)&&std::isfinite(v.y)&&std::isfinite(v.z)&&std::abs(v.x)<10000000&&std::abs(v.y)<10000000&&std::abs(v.z)<10000000;}
inline float Dot(Vec3 a,Vec3 b) {return a.x*b.x+a.y*b.y+a.z*b.z;}
inline float DistanceSquared(Vec2 a,Vec2 b) {const auto x=a.x-b.x,y=a.y-b.y;return x*x+y*y;}
struct Camera {
    Vec3 origin,forward,left,up;
    float tanHalfFovX{},tanHalfFovY{};
};
inline bool ValidCamera(const Camera& c) {
    if(!Finite(c.origin)||!Finite(c.forward)||!Finite(c.left)||!Finite(c.up)||!std::isfinite(c.tanHalfFovX)||!std::isfinite(c.tanHalfFovY))return false;
    if(c.tanHalfFovX<.05f||c.tanHalfFovX>10||c.tanHalfFovY<.05f||c.tanHalfFovY>10)return false;
    for(auto v:{c.forward,c.left,c.up})if(std::abs(Dot(v,v)-1)>.15f)return false;
    return std::abs(Dot(c.forward,c.left))<.15f&&std::abs(Dot(c.forward,c.up))<.15f&&std::abs(Dot(c.left,c.up))<.15f;
}
inline bool Project(const Camera& c,Vec3 world,Vec2 display,Vec2& out) {
    if(!ValidCamera(c)||!Finite(world)||!Finite(display)||display.x<=0||display.y<=0)return false;
    const Vec3 delta{world.x-c.origin.x,world.y-c.origin.y,world.z-c.origin.z};
    const auto depth=Dot(delta,c.forward);
    if(!std::isfinite(depth)||depth<=.1f)return false;
    const auto x=-Dot(delta,c.left)/(depth*c.tanHalfFovX);
    const auto y=-Dot(delta,c.up)/(depth*c.tanHalfFovY);
    out={(1+x)*display.x*.5f,(1+y)*display.y*.5f};
    return Finite(out)&&out.x>=0&&out.x<=display.x&&out.y>=0&&out.y<=display.y;
}
// Use the entity bounds when available; this is an estimated body point, not a bone lookup.
inline bool BodyPoint(Vec3 origin,Vec3 minimum,Vec3 maximum,bool upper,Vec3& out) {
    if(!Finite(origin))return false;
    float bottom=origin.z,height=72;
    if(Finite(minimum)&&Finite(maximum)) {
        const auto h=maximum.z-minimum.z;
        const bool local=std::abs(minimum.x)<80&&std::abs(minimum.y)<80&&std::abs(maximum.x)<80&&std::abs(maximum.y)<80&&std::abs(minimum.z)<160&&std::abs(maximum.z)<160;
        const bool world=std::abs(minimum.x-origin.x)<80&&std::abs(maximum.x-origin.x)<80&&std::abs(minimum.y-origin.y)<80&&std::abs(maximum.y-origin.y)<80&&std::abs(minimum.z-origin.z)<160&&std::abs(maximum.z-origin.z)<160;
        if(h>=12&&h<=110&&(local||world)) {height=h;}
    }
    out={origin.x,origin.y,bottom+height*(upper?.88f:.52f)};
    return Finite(out);
}
struct Candidate {int id{},team{},health{};bool alive{};Vec2 screen;};
inline int Select(const std::vector<Candidate>& candidates,int localId,int localTeam,Vec2 center,float radius,int retained=-1) {
    if(localId<0||localTeam<0||localTeam>2||!Finite(center)||!std::isfinite(radius)||radius<=0)return -1;
    float best=radius*radius;int result=-1;float retainedScore=std::numeric_limits<float>::infinity();
    for(const auto& c:candidates) {
        if(c.id<0||c.id>=18||c.id==localId||!c.alive||c.health<=0||c.team<0||c.team>2||!Finite(c.screen))continue;
        if(localTeam!=0&&c.team==localTeam)continue;
        const auto score=DistanceSquared(c.screen,center);
        if(!std::isfinite(score)||score>radius*radius)continue;
        if(c.id==retained)retainedScore=score;
        if(result==-1||score<best) {best=score;result=c.id;}
    }
    // Keep a held target unless the new candidate is substantially closer to the crosshair.
    if(std::isfinite(retainedScore)&&retainedScore<=radius*radius&&best>=retainedScore*.55f)return retained;
    return result;
}
struct MouseStep {long x{},y{};};
struct Controller {
    Vec2 remainder{};int target=-1;
    void Reset() {remainder={};target=-1;}
    MouseStep Step(Vec2 error,float dt,float speed,float gain,int id) {
        if(id<0||!Finite(error)||std::abs(error.x)>65536||std::abs(error.y)>65536||!std::isfinite(dt)||dt<=0||dt>.1f||!std::isfinite(speed)||speed<=0||!std::isfinite(gain)||gain<=0) {Reset();return {};}
        if(target!=id) {remainder={};target=id;}
        const auto distance=std::hypot(error.x,error.y);
        if(distance<=1.5f) {remainder={};return {};}
        const auto alpha=1-std::exp(-std::min(speed,40.0f)*dt);
        Vec2 move{error.x*alpha*std::min(gain,3.0f),error.y*alpha*std::min(gain,3.0f)};
        const auto length=std::hypot(move.x,move.y),limit=2000*dt;
        if(length>limit) {move.x*=limit/length;move.y*=limit/length;}
        move.x+=remainder.x;move.y+=remainder.y;
        const auto x=std::lround(move.x),y=std::lround(move.y);
        remainder={move.x-x,move.y-y};
        return {x,y};
    }
};
}
