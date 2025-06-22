# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create exchange objects for trading in AoC.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject
from .....value_object.conversion.forward_ref import ForwardRef

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.converter_object import ConverterObjectGroup
    from .....entity_object.conversion.aoc.genie_object_container import GenieObjectContainer

BUY_MODE_PARENT = "engine.util.exchange_mode.type.Buy"
SELL_MODE_PARENT = "engine.util.exchange_mode.type.Sell"
EXCHANGE_RATE_PARENT = "engine.util.exchange_rate.ExchangeRate"
PRICE_POOL_PARENT = "engine.util.price_pool.PricePool"
PRICE_MODE_PARENT = "engine.util.price_mode.type.Dynamic"
EXCHANGE_OBJECTS_LOCATION = "data/util/resource/"


def generate_exchange_objects(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate objects for market trading (ExchangeResources).

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    generate_exchange_modes(full_data_set, pregen_converter_group)
    generate_exchange_rates(full_data_set, pregen_converter_group)
    generate_price_pools(full_data_set, pregen_converter_group)
    generate_price_modes(full_data_set, pregen_converter_group)


def generate_exchange_modes(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate exchange modes for trading in AoC.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    # =======================================================================
    # Exchange mode Buy
    # =======================================================================
    exchange_mode_ref_in_modpack = "util.resource.market_trading.MarketBuyExchangeMode"
    exchange_mode_raw_api_object = RawAPIObject(exchange_mode_ref_in_modpack,
                                                "MarketBuyExchangePool",
                                                api_objects,
                                                EXCHANGE_OBJECTS_LOCATION)
    exchange_mode_raw_api_object.set_filename("market_trading")
    exchange_mode_raw_api_object.add_raw_parent(BUY_MODE_PARENT)

    # Fee (30% on top)
    exchange_mode_raw_api_object.add_raw_member("fee_multiplier",
                                                1.3,
                                                "engine.util.exchange_mode.ExchangeMode")

    pregen_converter_group.add_raw_api_object(exchange_mode_raw_api_object)
    pregen_nyan_objects.update({exchange_mode_ref_in_modpack: exchange_mode_raw_api_object})

    # =======================================================================
    # Exchange mode Sell
    # =======================================================================
    exchange_mode_ref_in_modpack = "util.resource.market_trading.MarketSellExchangeMode"
    exchange_mode_raw_api_object = RawAPIObject(exchange_mode_ref_in_modpack,
                                                "MarketSellExchangeMode",
                                                api_objects,
                                                EXCHANGE_OBJECTS_LOCATION)
    exchange_mode_raw_api_object.set_filename("market_trading")
    exchange_mode_raw_api_object.add_raw_parent(SELL_MODE_PARENT)

    # Fee (30% reduced)
    exchange_mode_raw_api_object.add_raw_member("fee_multiplier",
                                                0.7,
                                                "engine.util.exchange_mode.ExchangeMode")

    pregen_converter_group.add_raw_api_object(exchange_mode_raw_api_object)
    pregen_nyan_objects.update({exchange_mode_ref_in_modpack: exchange_mode_raw_api_object})


def generate_exchange_rates(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate exchange rates for trading in AoC.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    # =======================================================================
    # Exchange rate Food
    # =======================================================================
    exchange_rate_ref_in_modpack = "util.resource.market_trading.MarketFoodExchangeRate"
    exchange_rate_raw_api_object = RawAPIObject(exchange_rate_ref_in_modpack,
                                                "MarketFoodExchangeRate",
                                                api_objects,
                                                EXCHANGE_OBJECTS_LOCATION)
    exchange_rate_raw_api_object.set_filename("market_trading")
    exchange_rate_raw_api_object.add_raw_parent(EXCHANGE_RATE_PARENT)

    # Base price
    exchange_rate_raw_api_object.add_raw_member("base_price",
                                                1.0,
                                                EXCHANGE_RATE_PARENT)

    # Price adjust methods
    pa_buy_forward_ref = ForwardRef(pregen_converter_group,
                                    "util.resource.market_trading.MarketBuyPriceMode")
    pa_sell_forward_ref = ForwardRef(pregen_converter_group,
                                     "util.resource.market_trading.MarketSellPriceMode")
    price_adjust = {
        api_objects["engine.util.exchange_mode.type.Buy"]: pa_buy_forward_ref,
        api_objects["engine.util.exchange_mode.type.Sell"]: pa_sell_forward_ref
    }
    exchange_rate_raw_api_object.add_raw_member("price_adjust",
                                                price_adjust,
                                                EXCHANGE_RATE_PARENT)

    # Price pool
    pool_forward_ref = ForwardRef(pregen_converter_group,
                                  "util.resource.market_trading.MarketFoodPricePool")
    exchange_rate_raw_api_object.add_raw_member("price_pool",
                                                pool_forward_ref,
                                                EXCHANGE_RATE_PARENT)

    pregen_converter_group.add_raw_api_object(exchange_rate_raw_api_object)
    pregen_nyan_objects.update({exchange_rate_ref_in_modpack: exchange_rate_raw_api_object})

    # =======================================================================
    # Exchange rate Wood
    # =======================================================================
    exchange_rate_ref_in_modpack = "util.resource.market_trading.MarketWoodExchangeRate"
    exchange_rate_raw_api_object = RawAPIObject(exchange_rate_ref_in_modpack,
                                                "MarketWoodExchangeRate",
                                                api_objects,
                                                EXCHANGE_OBJECTS_LOCATION)
    exchange_rate_raw_api_object.set_filename("market_trading")
    exchange_rate_raw_api_object.add_raw_parent(EXCHANGE_RATE_PARENT)

    # Base price
    exchange_rate_raw_api_object.add_raw_member("base_price",
                                                1.0,
                                                EXCHANGE_RATE_PARENT)

    # Price adjust methods
    pa_buy_forward_ref = ForwardRef(pregen_converter_group,
                                    "util.resource.market_trading.MarketBuyPriceMode")
    pa_sell_forward_ref = ForwardRef(pregen_converter_group,
                                     "util.resource.market_trading.MarketSellPriceMode")
    price_adjust = {
        api_objects["engine.util.exchange_mode.type.Buy"]: pa_buy_forward_ref,
        api_objects["engine.util.exchange_mode.type.Sell"]: pa_sell_forward_ref
    }
    exchange_rate_raw_api_object.add_raw_member("price_adjust",
                                                price_adjust,
                                                EXCHANGE_RATE_PARENT)

    # Price pool
    pool_forward_ref = ForwardRef(pregen_converter_group,
                                  "util.resource.market_trading.MarketWoodPricePool")
    exchange_rate_raw_api_object.add_raw_member("price_pool",
                                                pool_forward_ref,
                                                EXCHANGE_RATE_PARENT)

    pregen_converter_group.add_raw_api_object(exchange_rate_raw_api_object)
    pregen_nyan_objects.update({exchange_rate_ref_in_modpack: exchange_rate_raw_api_object})

    # =======================================================================
    # Exchange rate Stone
    # =======================================================================
    exchange_rate_ref_in_modpack = "util.resource.market_trading.MarketStoneExchangeRate"
    exchange_rate_raw_api_object = RawAPIObject(exchange_rate_ref_in_modpack,
                                                "MarketStoneExchangeRate",
                                                api_objects,
                                                EXCHANGE_OBJECTS_LOCATION)
    exchange_rate_raw_api_object.set_filename("market_trading")
    exchange_rate_raw_api_object.add_raw_parent(EXCHANGE_RATE_PARENT)

    # Base price
    exchange_rate_raw_api_object.add_raw_member("base_price",
                                                1.3,
                                                EXCHANGE_RATE_PARENT)

    # Price adjust methods
    pa_buy_forward_ref = ForwardRef(pregen_converter_group,
                                    "util.resource.market_trading.MarketBuyPriceMode")
    pa_sell_forward_ref = ForwardRef(pregen_converter_group,
                                     "util.resource.market_trading.MarketSellPriceMode")
    price_adjust = {
        api_objects["engine.util.exchange_mode.type.Buy"]: pa_buy_forward_ref,
        api_objects["engine.util.exchange_mode.type.Sell"]: pa_sell_forward_ref
    }
    exchange_rate_raw_api_object.add_raw_member("price_adjust",
                                                price_adjust,
                                                EXCHANGE_RATE_PARENT)

    # Price pool
    pool_forward_ref = ForwardRef(pregen_converter_group,
                                  "util.resource.market_trading.MarketStonePricePool")
    exchange_rate_raw_api_object.add_raw_member("price_pool",
                                                pool_forward_ref,
                                                EXCHANGE_RATE_PARENT)

    pregen_converter_group.add_raw_api_object(exchange_rate_raw_api_object)
    pregen_nyan_objects.update({exchange_rate_ref_in_modpack: exchange_rate_raw_api_object})


def generate_price_pools(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate price pools for trading in AoC.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    # =======================================================================
    # Market Food price pool
    # =======================================================================
    exchange_pool_ref_in_modpack = "util.resource.market_trading.MarketFoodPricePool"
    exchange_pool_raw_api_object = RawAPIObject(exchange_pool_ref_in_modpack,
                                                "MarketFoodPricePool",
                                                api_objects,
                                                EXCHANGE_OBJECTS_LOCATION)
    exchange_pool_raw_api_object.set_filename("market_trading")
    exchange_pool_raw_api_object.add_raw_parent(PRICE_POOL_PARENT)

    pregen_converter_group.add_raw_api_object(exchange_pool_raw_api_object)
    pregen_nyan_objects.update({exchange_pool_ref_in_modpack: exchange_pool_raw_api_object})

    # =======================================================================
    # Market Wood price pool
    # =======================================================================
    exchange_pool_ref_in_modpack = "util.resource.market_trading.MarketWoodPricePool"
    exchange_pool_raw_api_object = RawAPIObject(exchange_pool_ref_in_modpack,
                                                "MarketWoodPricePool",
                                                api_objects,
                                                EXCHANGE_OBJECTS_LOCATION)
    exchange_pool_raw_api_object.set_filename("market_trading")
    exchange_pool_raw_api_object.add_raw_parent(PRICE_POOL_PARENT)

    pregen_converter_group.add_raw_api_object(exchange_pool_raw_api_object)
    pregen_nyan_objects.update({exchange_pool_ref_in_modpack: exchange_pool_raw_api_object})

    # =======================================================================
    # Market Stone price pool
    # =======================================================================
    exchange_pool_ref_in_modpack = "util.resource.market_trading.MarketStonePricePool"
    exchange_pool_raw_api_object = RawAPIObject(exchange_pool_ref_in_modpack,
                                                "MarketStonePricePool",
                                                api_objects,
                                                EXCHANGE_OBJECTS_LOCATION)
    exchange_pool_raw_api_object.set_filename("market_trading")
    exchange_pool_raw_api_object.add_raw_parent(PRICE_POOL_PARENT)

    pregen_converter_group.add_raw_api_object(exchange_pool_raw_api_object)
    pregen_nyan_objects.update({exchange_pool_ref_in_modpack: exchange_pool_raw_api_object})


def generate_price_modes(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate price modes for trading in AoC.

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    pregen_nyan_objects = full_data_set.pregen_nyan_objects
    api_objects = full_data_set.nyan_api_objects

    # =======================================================================
    # Buy Price mode
    # =======================================================================
    price_mode_ref_in_modpack = "util.resource.market_trading.MarketBuyPriceMode"
    price_mode_raw_api_object = RawAPIObject(price_mode_ref_in_modpack,
                                             "MarketBuyPriceMode",
                                             api_objects,
                                             EXCHANGE_OBJECTS_LOCATION)
    price_mode_raw_api_object.set_filename("market_trading")
    price_mode_raw_api_object.add_raw_parent(PRICE_MODE_PARENT)

    # Change value
    price_mode_raw_api_object.add_raw_member("change_value",
                                             0.03,
                                             PRICE_MODE_PARENT)

    # Min price
    price_mode_raw_api_object.add_raw_member("min_price",
                                             0.3,
                                             PRICE_MODE_PARENT)

    # Max price
    price_mode_raw_api_object.add_raw_member("max_price",
                                             99.9,
                                             PRICE_MODE_PARENT)

    pregen_converter_group.add_raw_api_object(price_mode_raw_api_object)
    pregen_nyan_objects.update({price_mode_ref_in_modpack: price_mode_raw_api_object})

    # =======================================================================
    # Sell Price mode
    # =======================================================================
    price_mode_ref_in_modpack = "util.resource.market_trading.MarketSellPriceMode"
    price_mode_raw_api_object = RawAPIObject(price_mode_ref_in_modpack,
                                             "MarketSellPriceMode",
                                             api_objects,
                                             EXCHANGE_OBJECTS_LOCATION)
    price_mode_raw_api_object.set_filename("market_trading")
    price_mode_raw_api_object.add_raw_parent(PRICE_MODE_PARENT)

    # Change value
    price_mode_raw_api_object.add_raw_member("change_value",
                                             -0.03,
                                             PRICE_MODE_PARENT)

    # Min price
    price_mode_raw_api_object.add_raw_member("min_price",
                                             0.3,
                                             PRICE_MODE_PARENT)

    # Max price
    price_mode_raw_api_object.add_raw_member("max_price",
                                             99.9,
                                             PRICE_MODE_PARENT)

    pregen_converter_group.add_raw_api_object(price_mode_raw_api_object)
    pregen_nyan_objects.update({price_mode_ref_in_modpack: price_mode_raw_api_object})
