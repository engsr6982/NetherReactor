#pragma once
#include "NetherReactor.h"
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/levelgen/structure/StructureTemplate.h"
#include <ctime>
#include <memory>
#include <string>
#include <string_view>
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

    static std::string_view const             NetherTowerFilePath;
    static std::unique_ptr<StructureTemplate> mReactorTowerTemplate;

    static NetherReactorManager& getInstance();

    std::unordered_map<string, time_t>        mClickEventStabilization; // 点击事件防抖
    std::unordered_map<string, NetherReactor> mReactors;                // 下界反应核

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
    std::vector<string> const Recipe_NetherRector_Ingredients = {
        NetherReactor::Minecraft_IronIngot,
        NetherReactor::Minecraft_Diamond
    };
};


} // namespace nr
