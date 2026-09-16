1. Make swirl entity
2. When you try to place a tower ~directly~ on top of an existing tower, create a swirl request
3. Swirl limit starts at 1, cannot swirl
4. Make Run modifier that adds 1 to max swirl count, now u can swirl 2 flavors
5. When you swirl 2 flavors together:
-- Destroy the first tower
-- place a Swirl entity down
-- give the Swirl entity the tags of both flavors
-- give the Swirl the abilities of both towers (for now, later we may combine them in some way)
-- give the swirl Placeable the total cost of both flavors and the swirl cost
6. Pixilart: Make 2 sprites, 1 for each color in the swirl, and render them both on top of each other with tint matching the flavor
7. (low prio) Make modifiers for swirl tower damage (requires swirl upgrade)
8. (low prio) Make modifiers for non-swirl tower damage
