# Copyright 2025-2025 the openage authors. See copying.md for legal info.

"""
Create exchange objects for trading in SWGB.
"""
from __future__ import annotations
import typing

from .....entity_object.conversion.converter_object import RawAPIObject
from .....value_object.conversion.forward_ref import ForwardRef
from ...aoc.pregen.exchange import EXCHANGE_OBJECTS_LOCATION, EXCHANGE_RATE_PARENT, \
    PRICE_POOL_PARENT
from ...aoc.pregen import exchange as aoc_pregen_exchange

if typing.TYPE_CHECKING:
    from .....entity_object.conversion.converter_object import ConverterObjectGroup
    from .....entity_object.conversion.aoc.genie_object_container import GenieObjectContainer


def generate_exchange_objects(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate objects for market trading (ExchangeResources).

    :param full_data_set: Storage for all converted objects and metadata.
    :param pregen_converter_group: Stores all pregenerated nyan objects.
    """
    aoc_pregen_exchange.generate_exchange_modes(full_data_set, pregen_converter_group)
    _generate_exchange_rates(full_data_set, pregen_converter_group)
    _generate_price_pools(full_data_set, pregen_converter_group)
    aoc_pregen_exchange.generate_price_modes(full_data_set, pregen_converter_group)


def _generate_exchange_rates(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate exchange rates for trading in SWGB.

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
    # Exchange rate Carbon
    # =======================================================================
    exchange_rate_ref_in_modpack = "util.resource.market_trading.MarketCarbonExchangeRate"
    exchange_rate_raw_api_object = RawAPIObject(exchange_rate_ref_in_modpack,
                                                "MarketCarbonExchangeRate",
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
                                  "util.resource.market_trading.MarketCarbonPricePool")
    exchange_rate_raw_api_object.add_raw_member("price_pool",
                                                pool_forward_ref,
                                                EXCHANGE_RATE_PARENT)

    pregen_converter_group.add_raw_api_object(exchange_rate_raw_api_object)
    pregen_nyan_objects.update({exchange_rate_ref_in_modpack: exchange_rate_raw_api_object})

    # =======================================================================
    # Exchange rate Ore
    # =======================================================================
    exchange_rate_ref_in_modpack = "util.resource.market_trading.MarketOreExchangeRate"
    exchange_rate_raw_api_object = RawAPIObject(exchange_rate_ref_in_modpack,
                                                "MarketOreExchangeRate",
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
                                  "util.resource.market_trading.MarketOrePricePool")
    exchange_rate_raw_api_object.add_raw_member("price_pool",
                                                pool_forward_ref,
                                                EXCHANGE_RATE_PARENT)

    pregen_converter_group.add_raw_api_object(exchange_rate_raw_api_object)
    pregen_nyan_objects.update({exchange_rate_ref_in_modpack: exchange_rate_raw_api_object})


def _generate_price_pools(
    full_data_set: GenieObjectContainer,
    pregen_converter_group: ConverterObjectGroup
) -> None:
    """
    Generate price pools for trading in SWGB.

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
    # Market Carbon price pool
    # =======================================================================
    exchange_pool_ref_in_modpack = "util.resource.market_trading.MarketCarbonPricePool"
    exchange_pool_raw_api_object = RawAPIObject(exchange_pool_ref_in_modpack,
                                                "MarketCarbonPricePool",
                                                api_objects,
                                                EXCHANGE_OBJECTS_LOCATION)
    exchange_pool_raw_api_object.set_filename("market_trading")
    exchange_pool_raw_api_object.add_raw_parent(PRICE_POOL_PARENT)

    pregen_converter_group.add_raw_api_object(exchange_pool_raw_api_object)
    pregen_nyan_objects.update({exchange_pool_ref_in_modpack: exchange_pool_raw_api_object})

    # =======================================================================
    # Market Ore price pool
    # =======================================================================
    exchange_pool_ref_in_modpack = "util.resource.market_trading.MarketOrePricePool"
    exchange_pool_raw_api_object = RawAPIObject(exchange_pool_ref_in_modpack,
                                                "MarketOrePricePool",
                                                api_objects,
                                                EXCHANGE_OBJECTS_LOCATION)
    exchange_pool_raw_api_object.set_filename("market_trading")
    exchange_pool_raw_api_object.add_raw_parent(PRICE_POOL_PARENT)

    pregen_converter_group.add_raw_api_object(exchange_pool_raw_api_object)
    pregen_nyan_objects.update({exchange_pool_ref_in_modpack: exchange_pool_raw_api_object})
