# Score system

## Age of Empires I

The score is based on 5 categories: military, economy, religion, technology and
survival/wonders.

In case of equality there should not be any player that gets a bonus for the
largest number. However, the game engine does pick one player that gets the
bonus. The player that gets the bonus is defined by a couple of factors:

* If player X gets the largest number before player Y does, X gets the bonus
  before Y does and keeps the bonus.
* If both players get the largest number at the same gametick the player with
  the lowest index (i.e. the game engine's internal assigned identifier) gets
  the bonus.

### Military

Task                           | Score
-------------------------------|-----------------------------------------------------------------
Kills                          | 0.5 points per unit
Buildings destroyed            | 1 point per building
Generalship                    | number of kills minus number of losses (value must be positive)
Most military units and towers | 25 point bonus

### Economy

Task                        | Score           | Notes
----------------------------|-----------------|---------
Gold from mining and trade  | 1/100 of value  |
Net resources tributed      | 1/60 of value   |
Villager population         | 1 per villager  | includes trade, transport, and fishing vessels
Largest villager population | 25 point bonus  | includes trade, transport, and fishing vessels
Exploration                 | 1 per 3% of map |
Largest area explored       | 25 point bonus  |

### Religion

Task                              | Score
----------------------------------|-------------------------
Conversion                        | 2 points per conversion
Most conversions                  | 25 point bonus
Temples built                     | 3 points per temple
Ruins controlled                  | 10 points per ruin
Artifacts controlled              | 10 points per artifact
Control of all Ruins or Artifacts | 50 point bonus

### Technology

Task                             | Score
---------------------------------|-------------------------
Technologies researched          | 2 points per technology
Most techonologies researched    | 50 point bonus
First civilization to Bronze Age | 25 point bonus
First civilization to Iron Age   | 25 point bonus

### Survival/Wonders

Task                         | Score
-----------------------------|-----------------------
Still alive before game ends | 100 points
Wonders held                 | 100 points per wonder
