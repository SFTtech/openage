# C Structs

A collection of C structs for commands and actions.

## Sync

### 16BC41

```c
struct 16bc41 {
  uint32_t network_source_id;
  uint32_t network_dest_id;
  uint8_t command;
  uint8_t option1;
  uint8_t option2;
  uint8_t option3;
  uint32_t individual_counter;
};
```

### 16BC31

```c
struct 16bc31 {
  uint32_t network_source_id;
  uint32_t network_dest_id;
  uint8_t command;
  uint8_t option1; // = 0x00
  uint8_t option2; // = 0x00
  uint8_t option3; // = 0x00
  uint32_t time_passed;
};
```

### 16BC32

```c
struct 16bc32 {
  uint32_t network_source_id;
  uint32_t network_dest_id;
  uint8_t command;
  uint8_t option1;
  uint8_t option2;
  uint8_t option3;
  uint32_t time_passed;
};
```

### 24BC35

Too many unknown bytes.

### 26BC53

Too many unknown bytes.

### 32BC44

Too many unknown bytes.

### 56BC4D

Too many unknown bytes.

### 24BC51

Too many unknown bytes.

### 24BC52

Too many unknown bytes.

## Actions

### 00 - Primary Action

```c
struct primary_action {
    uint8_t action_identifier;
    uint8_t player_id;
    uint32_t target_id;
    uint8_t selection_count;
    float x_coord;
    float y_coord;
    uint32_t selected_ids[selection_count];
};
```

### 01 - Stop

```c
struct stop {
    uint8_t action_identifier;
    uint8_t selection_count;
    uint32_t selected_ids[selection_count];
};
```

### 02 - AI Primary Action

```c
struct ai_primary_action {
    uint8_t action_identifier;
    uint32_t target_id;
    uint8_t selection_count;
    float x_coord;
    float y_coord;
    uint32_t selected_ids[selection_count];
};
```

### 03 - Move

```c
struct move {
  	uint8_t action_identifier;
  	uint8_t player_id;
  	uint32_t constant; // = 0xFFFFFFFF
  	uint32_t selection_count;
  	float x_coord;
  	float y_coord;
  	uint32_t selected_ids[selection_count];
};
```

### 0a - AI Move

Too many unknown bytes.

### 0b - Resign

```c
struct resign {
    uint8_t action_identifier;
    uint8_t player_number;
    uint8_t player_id;
    uint32_t disconnect;
};
```

### 10 - Waypoint

```c
struct waypoint {
  uint8_t action_identifier;
  uint8_t player_id;
  uint8_t selection_count;
  uint8_t x_coord;
  uint8_t y_coord;
  uint32_t selected_ids[selection_count];
};
```

### 12 - Stance

```c
struct stance {
  uint8_t action_identifier;
  uint8_t selection_count;
  uint8_t stance;
  uint32_t selected_ids[selection_count];
};
```

### 13 - Guard

```c
struct guard {
  uint8_t action_identifier;
  uint8_t selection_count;
  uint32_t guarded_id;
  uint32_t selected_ids[selection_count];
};
```

### 14 - Follow

```c
struct follow {
  uint8_t action_identifier;
  uint8_t selection_count;
  uint32_t followed_id;
  uint32_t selection_ids[selection_count];
};
```

### 15 - Patrol

```c
struct patrol {
  uint8_t action_identifier;
  uint8_t selection_count;
  uint8_t patrol_waypoint_count;
  float x_coords[10];
  float y_coords[10];
  uint32_t selection_ids[selection_count];
};
```

### 17 - Formation

```c
struct formation {
  uint8_t action_identifier;
  uint8_t selection_count;
  uint8_t player_number;
  uint8_t unknown; // = 0x00
  uint8_t formation_type;
  uint32_t selected_ids[selection_count];
};
```

### 1b - Save

Too many unknown bytes.

### 1f - AI Waypoints

```c
struct ai_waypoints {
  uint8_t action_identifier;
  uint8_t selection_count;
  uint8_t waypoint_count;
  uint32_t selected_ids[selection_count];
  uint8_t x_coords[waypoint_count];
  uint8_t y_coords[waypoint_count];
};
```

### 64 - AI Training

```c
struct ai_train {
  uint8_t action_identifier;
  uint32_t building_id;
  uint16_t player_id;
  uint16_t unit_id;
  uint32_t constant; // = 0xFFFFFFFF
};
```

### 65 - Research

```c
struct research {
  uint8_t action_identifier;
  uint32_t building_id;
  uint8_t player_number;
  uint16_t technology_id;
  uint32_t constant; // = 0xFFFFFFFF
};
```

### 66 - Build

```c
struct build {
  uint8_t action_identifier;
  uint8_t selection_count;
  uint8_t player_number;
  float x_coord;
  float y_coord;
  uint16_t building_id;
  uint32_t constant; // = 0xFFFFFFFF
  uint8_t sprite_id;
  uint32_t selected_ids[selection_count];
};
```

### 67 - Multipurpose

Too many unknown bytes.

### 69 - Build Wall

```c
struct build_wall {
  uint8_t action_identifier;
  uint8_t selection_count;
  uint8_t player_number;
  uint8_t start_x_coord;
  uint8_t start_y_coord;
  uint8_t end_x_coord;
  uint8_t end_y_coord;
  uint16_t building_id;
  uint32_t constant; // = 0xFFFFFFFF
  uint32_t selected_ids[selection_count];
};
```

### 6a - Delete

```c
struct delete {
  uint8_t action_identifier;
  uint32_t object_id;
  uint8_t player_number;
};
```

### 6b - Attack ground

```c
struct attack_ground {
	uint8_t action_identifier;
	uint8_t selection_count;
	float x_coord;
	float y_coord;
	uint32_t selected_ids[selection_count];
};
```

### 6c - Attack ground

```c
struct attack_ground {
  uint8_t action_identifier;
  uint8_t source_player_number;
  uint8_t target_player_number;
  uint8_t resource_type;
  float amount;
  float transaction_fee;
};
```

### 6e - Repair

```c
struct repair {
  uint8_t action_identifier;
  uint8_t selection_count;
  uint32_t repaired_id;
  uint32_t selected_ids[selection_count];
};
```

### 6f - Ungarrison

```c
struct ungarrison {
  uint8_t action_identifier;
  uint8_t selection_count;
  float x_coord;
  float y_coord;
  uint8_t ungarrison_type;
  uint32_t release_id;
  uint32_t selected_ids[selection_count];
};
```

### 72 - Toggle Gate

```c
struct toggle_gate {
  uint8_t action_identifier;
  uint32_t gate_id;
};
```

### 73 - Flare

```c
struct flare {
  uint8_t action_identifier;
  uint32_t constant; // = 0xFFFFFFFF
  uint8_t receiving[8];
  float x_coord;
  float y_coord;
  uint8_t player_number;
  uint8_t player_id;
};
```

### 75 - Garrison

```c
struct garrison {
  uint8_t action_identifier;
  uint8_t selection_count;
  uint32_t building_id;
  uint8_t garrison_type;
  float x_coord;
  float y_coord;
  uint32_t constant; // = 0xFFFFFFFF
  uint32_t selected_ids[selection_count];
};
```

### 77 - Training

```c
struct train {
  uint8_t :action_identifier;
  uint32_t :building_id;
  uint16_t :unit_id;
  uint16_t :train_count;
};
```

### 78 - Rally Point

```c
struct rally_point {
  uint8_t action_identifier;
  uint8_t selection_count;
  uint32_t target_id;
  uint32_t target_unit_id;
  float x_coord;
  float y_coord;
  uint32_t selected_ids[selection_count];
};
```

### 7a - Sell

```c
struct sell {
  uint8_t action_identifier;
  uint8_t player_number;
  uint8_t resource_type;
  uint8_t amount;
  uint32_t market_id;
};
```

### 7b - Buy

```c
struct buy {
  uint8_t action_identifier;
  uint8_t player_number;
  uint8_t resource_type;
  uint8_t amount;
  uint32_t market_id;
};
```

### 7e - Drop Relic

```c
struct drop_relic {
  uint8_t action_identifier;
  uint32_t monk_id;
};
```

### 7f - Townbell

```c
struct townbell {
  uint8_t action_identifier;
  uint32_t building_id;
  uint8_t active;
};
```

### 80 - Back to work

```c
struct back_to_work {
  uint8_t action_identifier;
  uint32_t building_id;
};
```
