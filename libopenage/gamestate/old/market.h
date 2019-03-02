// Copyright 2017-2019 the openage authors. See copying.md for legal info.

#pragma once

#include "resource.h"

namespace openage {

class Player;

constexpr double MARKET_PRICE_D            = 3.0;
constexpr double MARKET_PRICE_MIN          = 20.0;
constexpr double MARKET_PRICE_MAX          = 9999.0;
constexpr double MARKET_TRANSACTION_AMOUNT = 100.0;

/**
 * The global market prices.
 *
 * Price calculation is documented at doc/reverse_engineering/market.md#prices
 */
class Market {
public:
	Market();

	/**
	 * The given player sells the given resource for gold.
	 * Returns true when the transaction is successful.
	 */
	bool sell(Player &player, const game_resource res);

	/**
	 * The given player buys the given resource with gold.
	 * Returns true when the transaction is successful.
	 */
	bool buy(Player &player, const game_resource res);

	/**
	 * Get the selling prices for a given player.
	 */
	ResourceBundle get_buy_prices(const Player &player) const;

	/**
	 * Get the buying prices for a given player.
	 */
	ResourceBundle get_sell_prices(const Player &player) const;

protected:

	/**
	 * The getBuyPrices and getSellPrices are redirected here.
	 */
	ResourceBundle get_prices(const Player &player, const bool is_buy) const;

	/**
	 * Get the multiplier for the base prices
	 */
	double get_multiplier(const Player &player, const bool is_buy) const;

private:

	/**
	 * Stores the base price values of each resource.
	 *
	 * The ResourceBundle is used to represent the prices instead of the amounts
	 * for each resource.
	 */
	ResourceBundle base_prices;

};

} // openage
