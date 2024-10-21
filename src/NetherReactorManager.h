#pragma once
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/block/Block.h"
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

    void init();
    void release();

    // 辅助函数
    bool checkStructure(Player& player, BlockPos const& pos); // 检查结构

    // 常量
    string const RecipeID                       = "NetherReactorManager:reactor";   // 合成表ID
    string const Minecraft_NetherReactorManager = "minecraft:NetherReactorManager"; // 下届反应核
    string const Minecraft_GlowingObsidian      = "minecraft:glowingobsidian";      // 发光黑曜石
    string const Minecraft_GoldBlock            = "minecraft:gold_block";           // 金块
    string const Minecraft_CobbleStone          = "minecraft:cobblestone";          // 圆石

    // 下界反应核合成表
    //   铁锭 钻石 铁锭
    //   铁锭 钻石 铁锭
    //   铁锭 钻石 铁锭
    std::vector<string> const MINECRAFT_NETHER_REACTOR_RECIPE_shape       = {"ABA", "ABA", "ABA"};
    std::vector<string> const MINECRAFT_NETHER_REACTOR_RECIPE_ingredients = {
        "minecraft:iron_ingot",
        "minecraft:diamond"
    };
};


} // namespace nr
