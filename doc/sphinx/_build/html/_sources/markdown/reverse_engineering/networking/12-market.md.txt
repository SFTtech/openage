# Market actions

## Sell

Used for selling resources at the market.

### Definition

```ruby
def Sell
  int8 :action_identifier
  int8 :player_number
  int8 :resource_type
  int8 :amount
  int32 :market_id
end
```

### Description

*:action_identifier*<br/>
Always has the value `0x7a`.

*:player_number*<br/>
The ID of the player who sells the resources (`0x01` - `0x08`).

*:resource_type*<br/>
The resource type that is sold.

Hex Value | Resource
----------|---------
0x00      | Food
0x01      | Wood
0x02      | Stone

*:amount*<br/>
The amount being sold in hundreds. Can be either `0x01` or `0x05`.

*:market_id*<br/>
The ID of the market.

### Examples

`7a 01 02 01 46 1a 00 00`

>`7b` &mdash; action_identifier<br/>
>`01` &mdash; player_number<br/>
>`02` &mdash; resource_type<br/>
>`01` &mdash; amount<br/>
>`46 1a 00 00` &mdash; market_id

## Buy

Used for buying resources at the market.

### Definition

```ruby
def Buy
  int8 :action_identifier
  int8 :player_number
  int8 :resource_type
  int8 :amount
  int32 :market_id
end
```

### Description

*:action_identifier*<br/>
Always has the value `0x7b`.

*:player_number*<br/>
The ID of the player who buys the resources (`0x01` - `0x08`).

*:resource_type*<br/>
The resource type that is sold.

Hex Value | Resource
----------|---------
0x00      | Food
0x01      | Wood
0x02      | Stone

*:amount*<br/>
The amount being bought in hundreds. Can be either `0x01` or `0x05`.

*:market_id*<br/>
The ID of the market.

### Examples

`7b 01 01 05 46 1a 00 00`

>`7b` &mdash; action_identifier<br/>
>`01` &mdash; player_id<br/>
>`01` &mdash; resource_type<br/>
>`05` &mdash; amount<br/>
>`46 1a 00 00` &mdash; market_id
