# Waypoints

[Source](https://www.youtube.com/watch?v=vaBoq9OHdWM)

## Mechanics

Waypoints are placed if the player holds down the `SHIFT`-Key while right-clicking. A flag appears at the position where the waypoint is set. If the player continues to use `SHIFT + Right-Mouse-Button`, more waypoints will be added to a queue. A final press on `Right-Mouse-Button` without `SHIFT` initiates movement. Units will now follow the waypoints from the queue.

This works for normal movement as well as for patrolling and "Attack Move", with the expected behavior from these commands. It also works when right-clicking on the minimap.

## Limitation

Although the flags on the ground start to disappear after 10 waypoints, there is no limit to how much waypoints can be added to the queue. This only applies to normal movement though. "Patrol" and "Attack"-Mode have a queue limit of 10 waypoints. If these modes are selected, the 10th `SHIFT + Right-Mouse-Button` automatically initiates movement.

## Weird AoE2 Quirks

* Normal movement waypoints cannot be canceled, even if the unit is deselected.
* Buildings seem to support waypoints too because multiple flags can be placed down while holding `SHIFT`. However, the waypoints are not used by units produced in the building. Instead the last waypoint placed down becomes the rally point for the building.
* The final waypoint cannot be on top of the selected unit.
