#include "NetherReactor.h"
#include "NetherReactorManager.h"
#include "gmlib/world/Level.h"
#include "ll/api/service/Bedrock.h"
#include "mc/enums/BlockUpdateFlag.h"
#include "mc/network/packet/UpdateBlockPacket.h"
#include "mc/world/item/enchanting/EnchantUtils.h"
#include "mc/world/item/registry/ItemStack.h"
#include "mc/world/level/BlockPos.h"
#include "mc/world/level/BlockSource.h"
#include "mc/world/level/Level.h"
#include "mc/world/level/Spawner.h"
#include "mc/world/level/block/Block.h"
#include "mc/world/level/dimension/Dimension.h"
#include <string>


namespace nr {

string const NetherReactor::Minecraft_NetherReactor   = "minecraft:netherreactor";   // 下届反应核
string const NetherReactor::Minecraft_GlowingObsidian = "minecraft:glowingobsidian"; // 发光黑曜石
string const NetherReactor::Minecraft_GoldBlock       = "minecraft:gold_block";      // 金块
string const NetherReactor::Minecraft_CobbleStone     = "minecraft:cobblestone";     // 圆石
string const NetherReactor::Minecraft_IronIngot       = "minecraft:iron_ingot";      // 铁锭
string const NetherReactor::Minecraft_Diamond         = "minecraft:diamond";         // 钻石
string const NetherReactor::Minecraft_NetherRock      = "minecraft:netherrack";      // 下界岩
string const NetherReactor::Minecraft_Obsidian        = "minecraft:obsidian";        // 黑曜石

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
    return mIsActive;
}
bool NetherReactor::isDepleted() const {
    if (!isRaw()) {
        return false;
    }
    return mIsDepleted;
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
    mIsActive = true;

    // 更新状态
    _generateNetherTower();        // 生成地狱塔
    _replaceWithGlowingObsidian(); // 替换为发光黑曜石

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
        auto it1 = ItemStack(Minecraft_IronIngot, 6);
        auto it2 = ItemStack(Minecraft_Diamond, 3);
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
    auto& bl = getBlock();

    // 更新进度
    if (mProgress <= REACTOR_MAX_TICK) {
        mProgress++;
    } else {
        // 反应完成
        _replaceWithObsidian();  // 替换为黑曜石
        _crumblingNetherTower(); // 塔开始崩塌
        return;
    }

    // Todo:
}


// private:
void NetherReactor::_generateNetherTower() const {
    NetherReactorManager::mReactorTowerTemplate->placeInWorld(
        ll::service::getLevel()->getDimension(mDimensionId)->getBlockSourceFromMainChunkSource(),
        BlockPos(mPos.x - NetherReactor::REACTOR_TOWER_OFFSET, mPos.y - 2, mPos.z - NetherReactor::REACTOR_TOWER_OFFSET)
    );
}

void NetherReactor::_spawnItems() const {}

void NetherReactor::_spawnMonsters() const {}

void NetherReactor::_replaceWithGlowingObsidian() const {
    auto a1  = BlockPos(mPos.x + 1, mPos.y - 1, mPos.z + 1); // 底下3x3
    auto a2  = BlockPos(mPos.x + 1, mPos.y - 1, mPos.z - 1);
    auto a3  = BlockPos(mPos.x - 1, mPos.y - 1, mPos.z + 1);
    auto a4  = BlockPos(mPos.x - 1, mPos.y - 1, mPos.z - 1);
    auto a5  = BlockPos(mPos.x + 1, mPos.y - 1, mPos.z);
    auto a6  = BlockPos(mPos.x - 1, mPos.y - 1, mPos.z);
    auto a7  = BlockPos(mPos.x, mPos.y - 1, mPos.z + 1);
    auto a8  = BlockPos(mPos.x, mPos.y - 1, mPos.z - 1);
    auto a9  = BlockPos(mPos.x, mPos.y - 1, mPos.z);
    auto a10 = BlockPos(mPos.x + 1, mPos.y, mPos.z + 1); // 中心4角
    auto a11 = BlockPos(mPos.x + 1, mPos.y, mPos.z - 1);
    auto a12 = BlockPos(mPos.x - 1, mPos.y, mPos.z + 1);
    auto a13 = BlockPos(mPos.x - 1, mPos.y, mPos.z - 1);
    auto a14 = BlockPos(mPos.x, mPos.y + 1, mPos.z); // 顶部十字
    auto a15 = BlockPos(mPos.x + 1, mPos.y + 1, mPos.z);
    auto a16 = BlockPos(mPos.x - 1, mPos.y + 1, mPos.z);
    auto a17 = BlockPos(mPos.x, mPos.y + 1, mPos.z + 1);
    auto a18 = BlockPos(mPos.x, mPos.y + 1, mPos.z - 1);

    Block const& bl = Block::tryGetFromRegistry(Minecraft_GlowingObsidian);
    auto&        bs = ll::service::getLevel()->getDimension(mDimensionId)->getBlockSourceFromMainChunkSource();

    bs.setBlock(a1, bl, (int)BlockUpdateFlag::AllPriority, nullptr, nullptr);
    bs.setBlock(a2, bl, (int)BlockUpdateFlag::AllPriority, nullptr, nullptr);
    bs.setBlock(a3, bl, (int)BlockUpdateFlag::AllPriority, nullptr, nullptr);
    bs.setBlock(a4, bl, (int)BlockUpdateFlag::AllPriority, nullptr, nullptr);
    bs.setBlock(a5, bl, (int)BlockUpdateFlag::AllPriority, nullptr, nullptr);
    bs.setBlock(a6, bl, (int)BlockUpdateFlag::AllPriority, nullptr, nullptr);
    bs.setBlock(a7, bl, (int)BlockUpdateFlag::AllPriority, nullptr, nullptr);
    bs.setBlock(a8, bl, (int)BlockUpdateFlag::AllPriority, nullptr, nullptr);
    bs.setBlock(a9, bl, (int)BlockUpdateFlag::AllPriority, nullptr, nullptr);
    bs.setBlock(a10, bl, (int)BlockUpdateFlag::AllPriority, nullptr, nullptr);
    bs.setBlock(a11, bl, (int)BlockUpdateFlag::AllPriority, nullptr, nullptr);
    bs.setBlock(a12, bl, (int)BlockUpdateFlag::AllPriority, nullptr, nullptr);
    bs.setBlock(a13, bl, (int)BlockUpdateFlag::AllPriority, nullptr, nullptr);
    bs.setBlock(a14, bl, (int)BlockUpdateFlag::AllPriority, nullptr, nullptr);
    bs.setBlock(a15, bl, (int)BlockUpdateFlag::AllPriority, nullptr, nullptr);
    bs.setBlock(a16, bl, (int)BlockUpdateFlag::AllPriority, nullptr, nullptr);
    bs.setBlock(a17, bl, (int)BlockUpdateFlag::AllPriority, nullptr, nullptr);
    bs.setBlock(a18, bl, (int)BlockUpdateFlag::AllPriority, nullptr, nullptr);
}

void NetherReactor::_replaceWithObsidian() const {
    auto a1  = BlockPos(mPos.x + 1, mPos.y - 1, mPos.z + 1); // 底下3x3
    auto a2  = BlockPos(mPos.x + 1, mPos.y - 1, mPos.z - 1);
    auto a3  = BlockPos(mPos.x - 1, mPos.y - 1, mPos.z + 1);
    auto a4  = BlockPos(mPos.x - 1, mPos.y - 1, mPos.z - 1);
    auto a5  = BlockPos(mPos.x + 1, mPos.y - 1, mPos.z);
    auto a6  = BlockPos(mPos.x - 1, mPos.y - 1, mPos.z);
    auto a7  = BlockPos(mPos.x, mPos.y - 1, mPos.z + 1);
    auto a8  = BlockPos(mPos.x, mPos.y - 1, mPos.z - 1);
    auto a9  = BlockPos(mPos.x, mPos.y - 1, mPos.z);
    auto a10 = BlockPos(mPos.x + 1, mPos.y, mPos.z + 1); // 中心4角
    auto a11 = BlockPos(mPos.x + 1, mPos.y, mPos.z - 1);
    auto a12 = BlockPos(mPos.x - 1, mPos.y, mPos.z + 1);
    auto a13 = BlockPos(mPos.x - 1, mPos.y, mPos.z - 1);
    auto a14 = BlockPos(mPos.x, mPos.y + 1, mPos.z); // 顶部十字
    auto a15 = BlockPos(mPos.x + 1, mPos.y + 1, mPos.z);
    auto a16 = BlockPos(mPos.x - 1, mPos.y + 1, mPos.z);
    auto a17 = BlockPos(mPos.x, mPos.y + 1, mPos.z + 1);
    auto a18 = BlockPos(mPos.x, mPos.y + 1, mPos.z - 1);

    Block const& bl = Block::tryGetFromRegistry(Minecraft_Obsidian);
    auto&        bs = ll::service::getLevel()->getDimension(mDimensionId)->getBlockSourceFromMainChunkSource();

    bs.setBlock(a1, bl, (int)BlockUpdateFlag::AllPriority, nullptr, nullptr);
    bs.setBlock(a2, bl, (int)BlockUpdateFlag::AllPriority, nullptr, nullptr);
    bs.setBlock(a3, bl, (int)BlockUpdateFlag::AllPriority, nullptr, nullptr);
    bs.setBlock(a4, bl, (int)BlockUpdateFlag::AllPriority, nullptr, nullptr);
    bs.setBlock(a5, bl, (int)BlockUpdateFlag::AllPriority, nullptr, nullptr);
    bs.setBlock(a6, bl, (int)BlockUpdateFlag::AllPriority, nullptr, nullptr);
    bs.setBlock(a7, bl, (int)BlockUpdateFlag::AllPriority, nullptr, nullptr);
    bs.setBlock(a8, bl, (int)BlockUpdateFlag::AllPriority, nullptr, nullptr);
    bs.setBlock(a9, bl, (int)BlockUpdateFlag::AllPriority, nullptr, nullptr);
    bs.setBlock(a10, bl, (int)BlockUpdateFlag::AllPriority, nullptr, nullptr);
    bs.setBlock(a11, bl, (int)BlockUpdateFlag::AllPriority, nullptr, nullptr);
    bs.setBlock(a12, bl, (int)BlockUpdateFlag::AllPriority, nullptr, nullptr);
    bs.setBlock(a13, bl, (int)BlockUpdateFlag::AllPriority, nullptr, nullptr);
    bs.setBlock(a14, bl, (int)BlockUpdateFlag::AllPriority, nullptr, nullptr);
    bs.setBlock(a15, bl, (int)BlockUpdateFlag::AllPriority, nullptr, nullptr);
    bs.setBlock(a16, bl, (int)BlockUpdateFlag::AllPriority, nullptr, nullptr);
    bs.setBlock(a17, bl, (int)BlockUpdateFlag::AllPriority, nullptr, nullptr);
    bs.setBlock(a18, bl, (int)BlockUpdateFlag::AllPriority, nullptr, nullptr);
}


void NetherReactor::_crumblingNetherTower() const {
    auto min = BlockPos(mPos.x - REACTOR_TOWER_OFFSET, mPos.y - 1, mPos.z - REACTOR_TOWER_OFFSET);
    auto max =
        BlockPos(mPos.x + REACTOR_TOWER_OFFSET, mPos.y + REACTOR_TOWER_HEIGHT - 1, mPos.z + REACTOR_TOWER_OFFSET);

    Block const& air = Block::tryGetFromRegistry("minecraft:air");
    auto&        bs  = ll::service::getLevel()->getDimension(mDimensionId)->getBlockSourceFromMainChunkSource();
    for (short i = 0; i < 30; i++) {
        auto point = BlockPos(RandomInt(min.x, max.x), RandomInt(min.y, max.y), RandomInt(min.z, max.z));
        if (isPointInReactor(point, mPos)) {
            continue;
        }
        bs.setBlock(point, air, (int)BlockUpdateFlag::AllPriority, nullptr, nullptr);
    }
}


// static
bool NetherReactor::isAdjacent(NetherReactor const& reactor1, NetherReactor const& reactor2) {
    return false; // Todo:
}

bool NetherReactor::isPointInReactor(BlockPos const& pos, BlockPos const& reactorpos) {
    auto a1  = BlockPos(reactorpos.x + 1, reactorpos.y - 1, reactorpos.z + 1); // 底下3x3
    auto a2  = BlockPos(reactorpos.x + 1, reactorpos.y - 1, reactorpos.z - 1);
    auto a3  = BlockPos(reactorpos.x - 1, reactorpos.y - 1, reactorpos.z + 1);
    auto a4  = BlockPos(reactorpos.x - 1, reactorpos.y - 1, reactorpos.z - 1);
    auto a5  = BlockPos(reactorpos.x + 1, reactorpos.y - 1, reactorpos.z);
    auto a6  = BlockPos(reactorpos.x - 1, reactorpos.y - 1, reactorpos.z);
    auto a7  = BlockPos(reactorpos.x, reactorpos.y - 1, reactorpos.z + 1);
    auto a8  = BlockPos(reactorpos.x, reactorpos.y - 1, reactorpos.z - 1);
    auto a9  = BlockPos(reactorpos.x, reactorpos.y - 1, reactorpos.z);
    auto a10 = BlockPos(reactorpos.x + 1, reactorpos.y, reactorpos.z + 1); // 中心4角
    auto a11 = BlockPos(reactorpos.x + 1, reactorpos.y, reactorpos.z - 1);
    auto a12 = BlockPos(reactorpos.x - 1, reactorpos.y, reactorpos.z + 1);
    auto a13 = BlockPos(reactorpos.x - 1, reactorpos.y, reactorpos.z - 1);
    auto a14 = BlockPos(reactorpos.x, reactorpos.y + 1, reactorpos.z); // 顶部十字
    auto a15 = BlockPos(reactorpos.x + 1, reactorpos.y + 1, reactorpos.z);
    auto a16 = BlockPos(reactorpos.x - 1, reactorpos.y + 1, reactorpos.z);
    auto a17 = BlockPos(reactorpos.x, reactorpos.y + 1, reactorpos.z + 1);
    auto a18 = BlockPos(reactorpos.x, reactorpos.y + 1, reactorpos.z - 1);
    return pos == a1 || pos == a2 || pos == a3 || pos == a4 || pos == a5 || pos == a6 || pos == a7 || pos == a8
        || pos == a9 || pos == a10 || pos == a11 || pos == a12 || pos == a13 || pos == a14 || pos == a15 || pos == a16
        || pos == a17 || pos == a18;
}
int NetherReactor::RandomInt(int min, int max) {
    static std::mt19937                rng(std::random_device{}());
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

} // namespace nr