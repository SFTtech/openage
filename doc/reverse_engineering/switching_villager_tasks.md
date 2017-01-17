# Switching tasks

[Source](https://www.youtube.com/watch?v=FK__AnTL400)

## Losing carried resources

In the original Age of Kings game, villagers would lose any resources they carried when they switched roles, e.g. from "farmer" to "builder". This was likely caused by AoE "replacing" a villager when they are sent to perform another task. In Age of Conquerors this was slightly changed so that builders could carry resources and would only lose them if they would not switch back to their original role.

AoE 2 : HD Edition further removed some instances where a player would lose resources. Now resources are only lost if the villager gets assigned to a resource that is different from what they are carrying. Food that was gathered in the role of a "shepard" will therefore carry over if the new role is "hunter", "gatherer" or "farmer".

Looking at the first entry from **Weird AoE2 Quirks** it can be assumed that the HD Edition probably uses a dirty workaround to prevent the player from losing resources. The food is most likely put into the newly assigned sheep/boar/deer and then immediatly "collected" again.

## Automatic task-switching for builders

When builders finish a gathering site, they will automatically switch roles depending on the type of resources that the gathering site accepts and will start working on the nearest resource pile. For example, builders constructing a lumberyard will become lumberjacks and start to chop down nearby trees. If a gathering site accepts multiple resources, e.g. town centers, villagers will be assigned to the resource they are closest to and switch to a corresponding role.

### Automatic Drop-off after building a gathering site

Builders will automatically drop off **any** resource they are carrying to the player's stockpile after they constructed a gathering site. The type of gathering site does not matter. For example, builders will drop off all food they are holding at a mining camp as soon as they finish the building. The same happens when they build a farm.

Note: This only happens if the builder was actively working on the construction site at the time it was finished. It can be assumed that this is part of the automatic task-switching algorithm.

## Weird AoE2 Quirks

* If the cheat `aegis` (villagers gather instantly from a resource) is used in a game and the role of a villager changes from "hunter" to "shepard", he will dump all of the resources he is carrying "into" the sheep.  
