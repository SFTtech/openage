# Ram Speed

[Source](https://www.youtube.com/watch?v=aPK2RnpEgPA)

## Garrison Capacity

Basically all infantry and foot archer type units can garrison inside a ram. Exceptions are monks, petards and kings. In the *Forgotten Empires* Expansion villagers are also able to be garrisoned inside.

Battering and Capped Rams have a total capacity of 4, while the Siege Ram increases the capacity to 6 units.

## Speed

The ungarrisoned ram has a *base speed* which is shown below.

    Battering/Capped Ram  0.5 tiles/s
    Siege Ram             0.6 tiles/s

Every garrisoned infantry increases the speed of the ram by 0.05 tiles/s regardless of the speed of the unit. Foot archers, skirmishers and hand cannoneers **do not** increase the speed of the ram.

| Ram Type         |0 Units|1 Unit|2 Units|3 Units|4 Units|5 Units|6 Units|
| -----------------|:-----:|:----:|:-----:|:-----:|:-----:|:-----:|:-----:|
| Battering/Capped | 0.5   | 0.55 | 0.6   | 0.65  | 0.7   | X     | X     |
| Siege            | 0.6   | 0.65 | 0.7   | 0.75  | 0.8   | 0.85  | 0.9   |

Modifiers on the speed of siege units are only applied to the *base speed* of the ram and not to the speed contributed by garrisoned units.

### Math

    (total ram speed) = (base speed) * (speed modifiers) + 0.05 * (units garrisoned)
