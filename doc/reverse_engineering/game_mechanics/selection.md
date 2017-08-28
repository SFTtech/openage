# Selecting and Deselecting

This documents shows the methods of selecting and deselecting units.

[Source](https://www.youtube.com/watch?v=mQf8GDttbDw)

## Single units

The player doesn't have to click directly on the sprite of a unit because there is a tolerance factor involved. The tolerance factor is roughly `1 unit width` horizontally and `1 unit height` vertically measured from the center of the unit. This means that clicking in the general area of a unit will be accepted as a valid selection most of the time.

If two units' areas of tolerance overlap, the unit that is in the front seems to be preferred, but clicking directly on the sprite of one of the player's own units will select the unit that is pointed at. When the area of tolerance overlaps with an enemy (or ally) unit, the unit of the player will always be preferred, even if he clicks directly on the sprite of the enemy unit.

Relics and animals also have an area of tolerance regarding selection, while resource spots like trees, bushes, gold/stone mines as well as buildings don't.

## Multiple units

AoE2 allows the player to select up to 40 units (60 in AoE2 HD) at once. This can be done by doing either of these actions:

* Drawing a selection box around the units which should be selected
* Double-clicking on a unit
* Pressing the one of the number buttons `0-9` to select a previously establish group
* Using `CTRL + MOUSE-BUTTON` to add or remove single units

All of these have a specific purpose and will now be explained further.

### Selection Box

The selection box is the easiest way to select multiple units of any type in AoE2. When the player draws the box around the units he wants to select, the units inside the box will be added to the selection queue, **going from the top to the bottom of the box** until the limit of 40 units is reached.

The tolerance factor is also used here, which can result in units which are slightly outside of the selection box to be selected.

### Double-clicking

Double-clicking on one unit will add all units of the same type that are shown on screen to the selection queue. Units that are hidden under the HUD bars at the top and the bottom will be ignored. Furthermore, the tolerance factor is not used for this selection method. Only units which have their sprite shown (partially) in the rendering frame are qualified. Units are inserted into the selection queue by adding the units **sorted from oldest to youngest** until the queue has reached the limit of 40.

### Grouping units

Any selection can be made a group by pressing `CTRL + [0-9]`. A group can be quickly selected by pressing the corresponding number key. Units that are part of a group, show their group reference number to the left of their selection indicator.

When the group is reestablished by pressing the number key, the units are added to the selection queue by **going from oldest to youngest** unit.

Groups in AoE2 are fixed, which means there can be no units added or removed unless the group number is reassigned.

## Selection queue actions

There are multiple commands that can be used on the icons of the units in the HUD bar:

* `CTRL + MOUSE-BUTTON`: Remove the unit from the queue
* `SHIFT + MOUSE-BUTTON`: Keep only units of the same type in the queue
* `CTRL + SHIFT + MOUSE-BUTTON`: Remove all units of the same type from the queue

These commands also work for garrison queues.

## Weird AoE2 Quirks

* The tolerance factor when selecting single units can lead to two units being selected simultaneously with a single click, if their ares of tolerance overlap
* Double-Clicking uses the `LineID` to determine whether units are of the same type. This can lead to a situation where different units of the same line can be part of the selection (e.g. militia and champions). A common occurrence of this behavior can be seen when a unit is converted and its tech level is frozen.
