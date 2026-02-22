Enemies spawn in a combination of generated and curated waves, potentially using player modifiers to affect the generation process.

##### Level Parameters
- X (float) cooldown between waves
- Y (int) number of waves total
- Chances for spawning certain wave types. (vector of WaveDef and float 0-1, all chances add to 1)
- Guaranteed waves, like a boss. (vector of Wave)

##### Player Parameters
- X% more '  Tag' enemies, where Tag=(Small, Medium, Large, Flying, etc...)

##### WaveDef structure:
- vector of structure
	- Entity Def to spawn
	- (int) how many to spawn
	- (float) over how long

##### Wave structure:
- X (int) number of Y (EntityDef) enemy.
- Over Z (float) seconds.
- e.g. 50 ants and 50 pigs over 10s
- which wave this is (int)

##### Wave spawning
- Each enemy type handled separately.
- So with 50 ants and 50 pigs over 10s, you will spawn 1 ant and 1 pig every .2 seconds.
- With 1 boss and 100 ants over 10s, the boss will spawn on the first frame and each ant will spawn every .1 seconds.

##### Wave Generation
- For 1-Y (number of waves)
	- If Level Parameters contains a wave for this number
		- copy that wave.
	- Else
		- Make a Wave structure
		- Roll a 0-1, choose which WaveDef to spawn.
		- Make a wave from the WaveDef.
		- Apply any player parameters to the wave