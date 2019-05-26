// Copyright 2017-2019 the openage authors. See copying.md for legal info.

#include "player.h"
#include "market.h"

namespace openage {

Market::Market() {
	// the default prices when a game starts
	this->base_prices[game_resource::wood] = 100;
	this->base_prices[game_resource::food] = 100;
	this->base_prices[game_resource::stone] = 130;
}

// Price calculation is documented at doc/reverse_engineering/market.md#prices

bool Market::sell(Player &player, const game_resource res) {
	// deduct the standard MARKET_TRANSACTION_AMOUNT of the selling res
	if (player.deduct(res, MARKET_TRANSACTION_AMOUNT)) {
		// if deduct was successful
		// calc the gold received from selling MARKET_TRANSACTION_AMOUNT of res
		double amount = this->get_sell_prices(player).get(res);
		player.receive(game_resource::gold, amount);

		// decrease the price
		this->base_prices[res] -= MARKET_PRICE_D;
		if (this->base_prices.get(res) < MARKET_PRICE_MIN) {
			this->base_prices[res] = MARKET_PRICE_MIN;
		}
		return true;
	}
	return false;
}

bool Market::buy(Player &player, const game_resource res) {
	// calc the gold needed to buy MARKET_TRANSACTION_AMOUNT of res
	double price = this->get_buy_prices(player).get(res);
	if (player.deduct(game_resource::gold, price)) {
		// if deduct was successful
		player.receive(res, MARKET_TRANSACTION_AMOUNT);

		// increase the price
		this->base_prices[res] += MARKET_PRICE_D;
		if (this->base_prices.get(res) > MARKET_PRICE_MAX) {
			this->base_prices[res] = MARKET_PRICE_MAX;
		}
		return true;
	}
	return false;
}

ResourceBundle Market::get_buy_prices(const Player &player) const {
	return this->get_prices(player, true);
}

ResourceBundle Market::get_sell_prices(const Player &player) const {
	return this->get_prices(player, false);
}

ResourceBundle Market::get_prices(const Player &player, const bool is_buy) const {
	double mult = this->get_multiplier(player, is_buy);

	auto rb = ResourceBundle(this->base_prices);
	rb *= mult;
	rb.round(); // round to nearest integer
	return rb;
}

double Market::get_multiplier(const Player &/*player*/, const bool is_buy) const {
	double base = 0.3;
	// \todo change multiplier based on civ bonuses and player researched techs
	double mult = base;

	if (is_buy) {
		mult = 1 + mult;
	}
	else {
		mult = 1 - mult;
	}

	return mult;
}

} // openage
