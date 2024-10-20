#include "NetherReactor.h"
#include "gmlib/mod/recipe/CustomRecipe.h"
#include "ll/api/event/EventBus.h"
#include "ll/api/event/ListenerBase.h"
#include "ll/api/event/player/PlayerInteractBlockEvent.h"
#include "ll/api/service/Bedrock.h"
#include "mc/world/level/BlockSource.h"


ll::event::ListenerPtr PLAYER_INTERACT_BLOCK_LISTENER;

namespace nr {
NetherReactor& NetherReactor::getInstance() {
    static NetherReactor instance;
    return instance;
}

void NetherReactor::init() {
    // 注册自定义配方
    gmlib::mod::recipe::RecipeRegistry::registerShapedCraftingTableRecipe(
        RecipeID,
        MINECRAFT_NETHER_REACTOR_RECIPE_shape,
        MINECRAFT_NETHER_REACTOR_RECIPE_ingredients,
        Minecraft_NetherReactor,
        1
    );

    // 注册事件监听器
    PLAYER_INTERACT_BLOCK_LISTENER =
        ll::event::EventBus::getInstance().emplaceListener<ll::event::PlayerInteractBlockEvent>(
            [this](ll::event::PlayerInteractBlockEvent& ev) {
                auto bl = ev.block(); // 获取玩家点击的方块
                if (bl.has_value()) {
                    if (bl->getTypeName() == Minecraft_NetherReactor) {
                        BlockPos const& pos = ev.blockPos();  // 获取玩家点击的方块位置
                        this->checkStructure(ev.self(), pos); // 检查结构
                    }
                }
            }
        );
}

void NetherReactor::release() {
    // 卸载自定义配方
    gmlib::mod::recipe::RecipeRegistry::unregisterRecipe(RecipeID);

    // 卸载事件监听器
    ll::event::EventBus::getInstance().removeListener(PLAYER_INTERACT_BLOCK_LISTENER);
}


bool NetherReactor::checkStructure(Player& player, BlockPos const& pos) {
    static string const msg     = "Not the correct pattern!";                         // 错误信息
    static string const pos_msg = "The nether reactor needs to be built lower down."; // 位置错误信息
    if (pos.y > 90) {
        player.sendMessage(pos_msg); // 玩家点击的方块位置过高
        return false;
    }
    if (player.getDimensionId() == 1) {
        player.sendMessage(msg); // 玩家在地狱
        return false;
    }

    auto& bs = player.getDimensionBlockSource(); // 获取玩家所在世界的方块源

    // 中心方块为NetherReactor
    if (bs.getBlock(pos).getTypeName() != Minecraft_NetherReactor) {
        player.sendMessage(msg);
        return false;
    }

    // 检查金块(4个角)
    {
        auto const& a1 = bs.getBlock(pos.x + 1, pos.y - 1, pos.z + 1).getTypeName();
        auto const& a2 = bs.getBlock(pos.x - 1, pos.y - 1, pos.z - 1).getTypeName();
        auto const& a3 = bs.getBlock(pos.x + 1, pos.y - 1, pos.z - 1).getTypeName();
        auto const& a4 = bs.getBlock(pos.x - 1, pos.y - 1, pos.z + 1).getTypeName();
        if (a1 != Minecraft_GoldBlock || a2 != Minecraft_GoldBlock || a3 != Minecraft_GoldBlock
            || a4 != Minecraft_GoldBlock) {
            player.sendMessage(msg);
            return false;
        }
    }

    // 检查圆石
    {
        auto const& a1  = bs.getBlock(pos.x, pos.y - 1, pos.z).getTypeName(); // 底层5个圆石
        auto const& a2  = bs.getBlock(pos.x + 1, pos.y - 1, pos.z).getTypeName();
        auto const& a3  = bs.getBlock(pos.x - 1, pos.y - 1, pos.z).getTypeName();
        auto const& a4  = bs.getBlock(pos.x, pos.y - 1, pos.z + 1).getTypeName();
        auto const& a5  = bs.getBlock(pos.x, pos.y - 1, pos.z - 1).getTypeName();
        auto const& a6  = bs.getBlock(pos.x + 1, pos.y, pos.z + 1).getTypeName(); // 中层4个角圆石
        auto const& a7  = bs.getBlock(pos.x - 1, pos.y, pos.z - 1).getTypeName();
        auto const& a8  = bs.getBlock(pos.x + 1, pos.y, pos.z - 1).getTypeName();
        auto const& a9  = bs.getBlock(pos.x - 1, pos.y, pos.z + 1).getTypeName();
        auto const& a10 = bs.getBlock(pos.x, pos.y + 1, pos.z).getTypeName(); // 顶层中心圆石
        auto const& a11 = bs.getBlock(pos.x + 1, pos.y + 1, pos.z).getTypeName();
        auto const& a12 = bs.getBlock(pos.x - 1, pos.y + 1, pos.z).getTypeName();
        auto const& a13 = bs.getBlock(pos.x, pos.y + 1, pos.z + 1).getTypeName();
        auto const& a14 = bs.getBlock(pos.x, pos.y + 1, pos.z - 1).getTypeName();

        if (a1 != Minecraft_CobbleStone || a2 != Minecraft_CobbleStone || a3 != Minecraft_CobbleStone
            || a4 != Minecraft_CobbleStone || a5 != Minecraft_CobbleStone || a6 != Minecraft_CobbleStone
            || a7 != Minecraft_CobbleStone || a8 != Minecraft_CobbleStone || a9 != Minecraft_CobbleStone
            || a10 != Minecraft_CobbleStone || a11 != Minecraft_CobbleStone || a12 != Minecraft_CobbleStone
            || a13 != Minecraft_CobbleStone || a14 != Minecraft_CobbleStone) {
            player.sendMessage(msg);
            return false;
        }
    }

    player.sendMessage("Active!");

    return true;
}


} // namespace nr
