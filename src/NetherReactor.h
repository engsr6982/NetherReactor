#pragma once
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/BlockPos.h"
#include <string>
#include <unordered_map>
#include <vector>


namespace nr {

using string = std::string;

class NetherReactor {
public:
    NetherReactor()                                = default;
    NetherReactor(NetherReactor&&)                 = delete;
    NetherReactor(const NetherReactor&)            = delete;
    NetherReactor& operator=(NetherReactor&&)      = delete;
    NetherReactor& operator=(const NetherReactor&) = delete;

    static NetherReactor& getInstance();

    void init();
    void release();

    // 辅助函数
    bool checkStructure(Player& player, BlockPos const& pos); // 检查结构

    // 常量
    string const RecipeID                  = "NetherReactor:reactor";     // 合成表ID
    string const Minecraft_NetherReactor   = "minecraft:netherreactor";   // 下届反应核
    string const Minecraft_GlowingObsidian = "minecraft:glowingobsidian"; // 发光黑曜石
    string const Minecraft_GoldBlock       = "minecraft:gold_block";      // 金块
    string const Minecraft_CobbleStone     = "minecraft:cobblestone";     // 圆石

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
