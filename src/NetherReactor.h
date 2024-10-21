#pragma once
#include "mc/world/actor/player/Player.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/block/Block.h"
#include <ctime>
#include <string>


namespace nr {

using string = std::string;


class NetherReactor {
public:
    // 基础结构
    BlockPos mPos;         // 下界反应核位置
    int      mDimensionId; // 维度ID

    // 状态
    string      mActivatedBy;   // 激活者
    std::time_t mActivatedTime; // 激活时间
    short       mReplaceCount;  // 替换次数(0: 默认, 1: 发光黑曜石, 2: 黑曜石)
    short       mState;         // 状态(0: 默认, 1: 激活, 2: 耗尽)

    NetherReactor(BlockPos const& pos, int dimensionId) : mPos(pos), mDimensionId(dimensionId) {}
    // ~NetherReactor();

    bool isRaw() const;              // 是否为默认状态
    bool isActive() const;           // 是否激活
    bool isDepleted() const;         // 是否耗尽
    bool isCorrectPattern() const;   // 是否符合结构
    bool isCorrectDimension() const; // 是否在正确的维度
    bool isCorrectTime() const;      // 是否在激活时间内

    bool canBeActivated(Player& player) const; // 是否可以被激活
    bool canDestroy(Player& player) const;     // 是否可以被破坏
    bool canSpawnItems() const;                // 是否可以生成掉落物

    void activate(Player& player); // 激活
    void destroy(Player& player);  // 破坏(精准掉落自身、非精准掉落原料)

    short           getReplaceCount() const;  // 获取替换次数
    short           getState() const;         // 获取状态
    string          getActivatedBy() const;   // 获取激活者
    time_t          getActivatedTime() const; // 获取激活时间
    BlockPos const& getPos() const;           // 获取位置
    Block*          getBlock() const;         // 获取方块
    int             getDimensionId() const;   // 获取维度ID
    string          getID() const;            // 获取ID

    void tick();

    operator string() const; // 转换为字符串

    // private:
    void _generateNetherTower() const; // 生成下界塔

    void _spawnItems() const; // 生成掉落物(激活后)

    void _spawnMonsters() const; // 生成怪物(激活后)

    void _replaceWithGlowingObsidian() const; // 将下界反应核替换为发光黑曜石(激活后)

    void _replaceWithObsidian() const; // 将下界反应核替换为黑曜石(耗尽后)

    void _crumblingNetherTower() const; // 下界塔破损(耗尽后)


    // static
    static bool isAdjacent(NetherReactor const& reactor1, NetherReactor const& reactor2); // 是否相邻

    static string formatID(BlockPos const& pos, int dimensionId); // 格式化ID

    static string const Minecraft_NetherReactor;   // 下届反应核
    static string const Minecraft_GlowingObsidian; // 发光黑曜石
    static string const Minecraft_GoldBlock;       // 金块
    static string const Minecraft_CobbleStone;     // 圆石
};

} // namespace nr