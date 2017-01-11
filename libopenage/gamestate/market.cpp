// Copyright 2017-2017 the openage authors. See copying.md for legal info.

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
	double mult = this->getMultiplier(player, false);

	if (player.deduct(res, MARKET_TRANSACTION_AMOUNT)) {
		// if deduct was successful
		// calc the gold received from selling MARKET_TRANSACTION_AMOUNT of res
		double amount = this->base_prices.get(res) * mult;
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
	double mult = this->getMultiplier(player, true);

	// calc the gold needed to buy MARKET_TRANSACTION_AMOUNT of res
	double price = this->base_prices.get(res) * mult;
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

std::shared_ptr<ResourceBundle> Market::getBuyPrices(const Player &player) const {
	return this->getPrices(player, true);
}

std::shared_ptr<ResourceBundle> Market::getSellPrices(const Player &player) const {
	return this->getPrices(player, false);
}

std::shared_ptr<ResourceBundle> Market::getPrices(const Player &player, const bool is_buy) const {
	double mult = this->getMultiplier(player, is_buy);

	auto rb = std::make_shared<ResourceBundle>(this->base_prices);
	*rb.get() *= mult;
	return rb;
}

double Market::getMultiplier(const Player &/*player*/, const bool is_buy) const {
	double base = 0.3;
	// TODO chnage multiplier based on civ bonuses and player researched techs
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
