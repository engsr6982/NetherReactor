#include "NetherReactor.h"
#include "gmlib/mod/recipe/CustomRecipe.h"
#include "mc/deps/core/string/HashedString.h"
#include "mc/enums/CreativeItemCategory.h"
#include "mc/world/item/Item.h"
#include "mc/world/item/ItemStackBase.h"
#include "mc/world/item/registry/ItemRegistry.h"
#include "mc/world/item/registry/ItemRegistryManager.h"
#include "mc/world/item/registry/ItemRegistryRef.h"
#include "mc/world/level/block/utils/VanillaBlockTypeIds.h"


namespace nr {
NetherReactor& NetherReactor::getInstance() {
    static NetherReactor instance;
    return instance;
}

void NetherReactor::init() {
    // 注册自定义配方
    _initRecipe();
}

void NetherReactor::release() {
    // 卸载自定义配方
    gmlib::mod::recipe::RecipeRegistry::unregisterRecipe(RECIPE_ID);
}


void NetherReactor::_initRecipe() {
    gmlib::mod::recipe::RecipeRegistry::registerShapedCraftingTableRecipe(
        RECIPE_ID,
        MINECRAFT_NETHER_REACTOR_RECIPE_shape,
        MINECRAFT_NETHER_REACTOR_RECIPE_ingredients,
        MINECRAFT_NETHER_REACTOR,
        1
    );
}


} // namespace nr
