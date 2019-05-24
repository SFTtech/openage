# Build speed

[Source](https://www.youtube.com/watch?v=I-WvtZWLLc0)

Adding villagers to a construction site decreases the time it takes to finish the building. However, the increase in build speed is not linear. The more villagers are assigned to the building, the lesser the increase is.

## Math

    (time for completion) = (3 * (build time)) / ((number of villagers) + 2)

## Relation to the building's HP

The *status of completion* is independent from the HP of a building. This ensures that a building can be completed in the same amount of time, even if the construction site is constantly attacked by enemies.
