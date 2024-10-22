#include "NetherReactor.h"
#include "Macros.h"
#include "gmlib/world/Level.h"
#include "ll/api/service/Bedrock.h"
#include "mc/nbt/CompoundTag.h"
#include "mc/nbt/Tag.h"
#include "mc/network/packet/UpdateBlockPacket.h"
#include "mc/world/item/enchanting/EnchantUtils.h"
#include "mc/world/item/registry/ItemStack.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/Level.h"
#include "mc/world/level/Spawner.h"
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

bool NetherReactor::isRaw() const { return getBlock().getTypeName() == Minecraft_NetherReactor; }
bool NetherReactor::isActive() const {
    if (!isRaw()) {
        return false;
    }
    return getBlock().getSerializationId().contains("IsInitialized", Tag::Type::Byte);
}
bool NetherReactor::isDepleted() const {
    if (!isRaw()) {
        return false;
    }
    return getBlock().getSerializationId().contains("HasFinished", Tag::Type::Byte);
}
bool NetherReactor::isCorrectDimension() const { return mDimensionId != 1; }

bool NetherReactor::canBeActivated() const {
    return isRaw() && isCorrectDimension() && isCorrectPattern() && !isActive() && !isDepleted();
}
bool NetherReactor::canSpawnItems() const { return isRaw() && isActive() && !isDepleted(); }

void NetherReactor::activate(Player& player) {
    if (!canBeActivated()) {
        player.sendMessage("不正确的组合!");
        return;
    }
    auto& bl  = getBlock();
    auto& nbt = const_cast<CompoundTag&>(bl.getSerializationId());

    // 更新Nbt
    if (!nbt.contains("IsInitialized", Tag::Type::Byte)) {
        nbt.putByte("IsInitialized", 1);
    } else {
        nbt.at("IsInitialized") = 1;
    }

    // 刷新方块
    _refreshBlock();

    // 更新状态
    _replaceWithGlowingObsidian(); // 替换为发光黑曜石
    _generateNetherTower();        // 生成地狱塔

    tick();

    player.sendMessage("NetherReactor已激活!");
}
void NetherReactor::destroy(Player& player) {
    if (!isRaw()) {
        return;
    }
    auto& item    = player.getSelectedItem();
    auto& bs      = player.getDimensionBlockSource();
    auto& spawner = player.getLevel().getSpawner();

    if (EnchantUtils::hasEnchant(::Enchant::Type::MiningSilkTouch, item)) {
        // 精准采集
        ItemStack it = ItemStack(Minecraft_NetherReactor, 1);
        spawner.spawnItem(bs, it, &player, mPos);

    } else {
        // 破坏
        auto it1 = ItemStack("minecraft:iron_ingot", 6);
        auto it2 = ItemStack("minecraft:diamond", 3);
        spawner.spawnItem(bs, it1, &player, mPos);
        spawner.spawnItem(bs, it2, &player, mPos);
    }
}

BlockPos const& NetherReactor::getPos() const { return mPos; }
int             NetherReactor::getDimensionId() const { return this->mDimensionId; }
Block const&    NetherReactor::getBlock() const {
    return ll::service::getLevel()
        ->getDimension(this->mDimensionId)
        ->getBlockSourceFromMainChunkSource()
        .getBlock(this->mPos);
}

void NetherReactor::tick() {
    auto& bl  = getBlock();
    auto& nbt = const_cast<CompoundTag&>(bl.getSerializationId());

    // 更新进度
    if (!nbt.contains("Progress", Tag::Type::Short)) {
        nbt.putShort("Progress", 0); // 初始化进度
    }
    auto& progress = nbt.at("Progress").get<ShortTag>().data;
    if (progress <= REACTOR_MAX_TICK) {
        progress++;
    } else {
        // 反应完成
        // progress = 0;
        nbt.putByte("HasFinished", 1);
        _refreshBlock();
        _replaceWithObsidian();  // 替换为黑曜石
        _crumblingNetherTower(); // 塔开始崩塌
        return;
    }

    // Todo:
}


// private:
void NetherReactor::_generateNetherTower() const {}

void NetherReactor::_spawnItems() const {}

void NetherReactor::_spawnMonsters() const {}

void NetherReactor::_replaceWithGlowingObsidian() const {}

void NetherReactor::_replaceWithObsidian() const {}

void NetherReactor::_crumblingNetherTower() const {}

void NetherReactor::_refreshBlock() const {
    UpdateBlockPacket
        pkt(mPos, (uint)UpdateBlockPacket::BlockLayer::Extra, getBlock().getRuntimeId(), (uchar)BlockUpdateFlag::All);
    gmlib::world::Level::getLevel()->sendPacketRawToDimension(pkt, mDimensionId);
}


// static
bool NetherReactor::isAdjacent(NetherReactor const& reactor1, NetherReactor const& reactor2) {
    return false; // Todo:
}


} // namespace nr