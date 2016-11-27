### Repair

Buildings, siege weapons and ships can be repaired by villagers.

## Speed

Buildings are repaired at the base repairing speed of *750 hp/min* which is not affected by the total hp of the building/unit.

Siege weapons and ships are repaired at *25%* of the base speed (188 hp/min).

After the first villager, each villager repairs at *50%* of the base speed (375 hp/min for building and 188 hp/min for siege weapons and ships).

## Cost

For each hp repaired, resources equal to the half of corresponding build cost are deducted.

```
(repair 1 hp cost) = 0.5 * (build cost) / (build max hp)
```

## Exceptions

* Town center repair costs no stone.

## Math

```
750 hp/min = 750/60000 hp/msec = 0.0125 hp/msec => 80 msec/hp
```