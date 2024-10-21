#pragma once
#include "NetherReactor.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/BlockPos.h"
#include <string>
#include <unordered_map>
#include <vector>


namespace nr {

using string = std::string;


class NetherReactorManager {
public:
    NetherReactorManager()                                       = default;
    NetherReactorManager(NetherReactorManager&&)                 = delete;
    NetherReactorManager(const NetherReactorManager&)            = delete;
    NetherReactorManager& operator=(NetherReactorManager&&)      = delete;
    NetherReactorManager& operator=(const NetherReactorManager&) = delete;

    static NetherReactorManager& getInstance();

    std::unordered_map<string, NetherReactor> mReactors; // 下界反应核

    void init();
    void release();


    bool hasReactor(string const& ID) const;
    bool hasReactor(BlockPos const& pos, int dimensionId) const;

    NetherReactor* getReactor(string const& ID);
    NetherReactor* getReactor(BlockPos const& pos, int dimensionId);

    bool addReactor(NetherReactor reactor);
    bool removeReactor(string const& ID);
    bool removeReactor(BlockPos const& pos, int dimensionId);

    // 下界反应核合成表
    //   铁锭 钻石 铁锭
    //   铁锭 钻石 铁锭
    //   铁锭 钻石 铁锭
    string const              Recipe_NetherRector_ID          = "NetherReactorManager:reactor"; // 合成表ID
    std::vector<string> const Recipe_NetherRector_Shape       = {"ABA", "ABA", "ABA"};
    std::vector<string> const Recipe_NetherRector_Ingredients = {"minecraft:iron_ingot", "minecraft:diamond"};
};


} // namespace nr
