#pragma once
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


    void _initRecipe();


    string const RECIPE_ID                 = "NetherReactor:reactor";     // 合成表ID
    string const MINECRAFT_NETHER_REACTOR  = "minecraft:netherreactor";   // 下届反应核
    string const MINECRAFT_GLOWINGOBSIDIAN = "minecraft:glowingobsidian"; // 发光黑曜石

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
