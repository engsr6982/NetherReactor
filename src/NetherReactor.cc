#include "NetherReactor.h"
#include "ll/api/service/Bedrock.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/Level.h"
#include "mc/world/level/dimension/Dimension.h"
#include <string>


namespace nr {

string const NetherReactor::Minecraft_NetherReactor   = "minecraft:netherreactor";   // 下届反应核
string const NetherReactor::Minecraft_GlowingObsidian = "minecraft:glowingobsidian"; // 发光黑曜石
string const NetherReactor::Minecraft_GoldBlock       = "minecraft:gold_block";      // 金块
string const NetherReactor::Minecraft_CobbleStone     = "minecraft:cobblestone";     // 圆石

string NetherReactor::formatID(const BlockPos& pos, int dimensionId) {
    return std::to_string(dimensionId) + pos.toString();
}

NetherReactor::operator string() const { return getID(); }
string NetherReactor::getID() const { return NetherReactor::formatID(mPos, mDimensionId); }

bool NetherReactor::isCorrectPattern() const {
    auto level = ll::service::getLevel()->getDimension(mDimensionId);
    if (!level) {
        return false;
    }
    auto& bs = level->getBlockSourceFromMainChunkSource(); // 获取方块源

    // 中心方块为NetherReactor
    if (bs.getBlock(mPos).getTypeName() != Minecraft_NetherReactor) {
        return false;
    }

    // 检查金块(4个角)
    {
        auto const& a1 = bs.getBlock(mPos.x + 1, mPos.y - 1, mPos.z + 1).getTypeName();
        auto const& a2 = bs.getBlock(mPos.x - 1, mPos.y - 1, mPos.z - 1).getTypeName();
        auto const& a3 = bs.getBlock(mPos.x + 1, mPos.y - 1, mPos.z - 1).getTypeName();
        auto const& a4 = bs.getBlock(mPos.x - 1, mPos.y - 1, mPos.z + 1).getTypeName();
        if (a1 != Minecraft_GoldBlock || a2 != Minecraft_GoldBlock || a3 != Minecraft_GoldBlock
            || a4 != Minecraft_GoldBlock) {
            return false;
        }
    }

    // 检查圆石
    {
        auto const& a1  = bs.getBlock(mPos.x, mPos.y - 1, mPos.z).getTypeName(); // 底层5个圆石
        auto const& a2  = bs.getBlock(mPos.x + 1, mPos.y - 1, mPos.z).getTypeName();
        auto const& a3  = bs.getBlock(mPos.x - 1, mPos.y - 1, mPos.z).getTypeName();
        auto const& a4  = bs.getBlock(mPos.x, mPos.y - 1, mPos.z + 1).getTypeName();
        auto const& a5  = bs.getBlock(mPos.x, mPos.y - 1, mPos.z - 1).getTypeName();
        auto const& a6  = bs.getBlock(mPos.x + 1, mPos.y, mPos.z + 1).getTypeName(); // 中层4个角圆石
        auto const& a7  = bs.getBlock(mPos.x - 1, mPos.y, mPos.z - 1).getTypeName();
        auto const& a8  = bs.getBlock(mPos.x + 1, mPos.y, mPos.z - 1).getTypeName();
        auto const& a9  = bs.getBlock(mPos.x - 1, mPos.y, mPos.z + 1).getTypeName();
        auto const& a10 = bs.getBlock(mPos.x, mPos.y + 1, mPos.z).getTypeName(); // 顶层中心圆石
        auto const& a11 = bs.getBlock(mPos.x + 1, mPos.y + 1, mPos.z).getTypeName();
        auto const& a12 = bs.getBlock(mPos.x - 1, mPos.y + 1, mPos.z).getTypeName();
        auto const& a13 = bs.getBlock(mPos.x, mPos.y + 1, mPos.z + 1).getTypeName();
        auto const& a14 = bs.getBlock(mPos.x, mPos.y + 1, mPos.z - 1).getTypeName();

        if (a1 != Minecraft_CobbleStone || a2 != Minecraft_CobbleStone || a3 != Minecraft_CobbleStone
            || a4 != Minecraft_CobbleStone || a5 != Minecraft_CobbleStone || a6 != Minecraft_CobbleStone
            || a7 != Minecraft_CobbleStone || a8 != Minecraft_CobbleStone || a9 != Minecraft_CobbleStone
            || a10 != Minecraft_CobbleStone || a11 != Minecraft_CobbleStone || a12 != Minecraft_CobbleStone
            || a13 != Minecraft_CobbleStone || a14 != Minecraft_CobbleStone) {
            return false;
        }
    }
    return true;
}


} // namespace nr