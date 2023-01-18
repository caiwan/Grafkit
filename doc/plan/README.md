# Jegyzetek 
Ide azok az ötletek jönnek, amik a kód átnyálazása/hackaton közben jönnek elő.

## Újrahasznosítás
- Renderable és Drawable doglot kidobni a fenébe. Erre semmi különösebb szükség nincs. 
- Az ArrayPool nem kell egyáltalán -> STL tárókat használunk minden helyett
- A Refcounter annál inkább kell; de az a verzió, ami a M$ recounterével is kompatibilis
- Logger kell. 
- A fa + iterable, iterator típusokat át kellene emelni + TDD-vel unittesztet írni hozzá `->` piros könyv
- A külső alap shaderekből kell egy libet pakolni valahova, hogy elérhetőek legyenek
	- Sprite shader
	- Fullscreen rect vertex shader 
	- Phong-blinn / Lambert
	- Copy -> 2d FX 
	- *Etc...* minden, ami a builtin databan van. 

## Újratervezés
- Legyen egy effekt composer. Ebbe dobálhatjuk bele az effetjeinket, illetve az őket előállító render objektumot
	- a hackelt változat mehet a kukába, teljesne új kell majd 
- Legyen rendes scenegraph. Ez képezze a kompozitálás szerves részét. Ennek részleteit a Three.js-ből és a hieroglyphből szedjük át.

# Dolgok
## Effect composer
Az. 

## Scenegraph - dolog. 

**Beágyazási / kompozit hierarchia**
- **Scenegraph** - Rendereli a fát, amit tartalmaz.
	- Node - a Fa egy eleme, ami Actor típusú -> erre van fatípus, ezt meg kell írni + unittesztet generálni hozzá -> DDT van-e m$-hez
	
- **Actor** - összefogja, és rendereli az alábbiakat:
	- **Mesh**, ami csak vertex adatokat tartalmaz, semmi mást.
	- **Material** 
		- **Textúra**
	- **Shader**, ami a materialt és a textúrát ráheggeszti a meshre
	
# Kutatás során talált dolgok
- Data driven teszting [M$ unitteszthez](https://msdn.microsoft.com/en-us/library/ms182527.aspx) **Ez C# **
