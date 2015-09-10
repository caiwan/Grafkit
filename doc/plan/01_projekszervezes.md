# Projektszervezés

## File szinten

- `src/` könyvtárba megy minden source
	- `src/api` köynvtár az api könyvtára
	- `src/idogep` könyvtár a szerkesztõé
	- **TODO** ide kell még valahova minden egyéb apróság kódja, python szkriptek, html/js toolok, minden ilyen
		- Textúra generátor
		- A BMFont-ból kialakított textúra generátor library
- `test/`
	- unitesztek - valószínûleg gteszt lesz
	- funkcionális tesztek - API egyes részeit teszteli le, valószínûleg robot framework-kel
	- engine teszt - az apiban levõ player funcionális tesztjei
	- **TODO** - ezeket el kell választani valahogy egymástól
	
- `build/` 
	- ide szemetel a fordító
	
- `demos/`
	- az api-val készült demók, releasek
	
- `tools`
	- *subrepository* minden 3rd party cucc ide jön + a fordításukhoz szükséges MSVC projektek, illetve szkriptek
	
**TODO** - a shader effekteknek is kell valahova valamilyen könyvtár, amiben ellennének, ne az assetsben legyen külön. 



## API szinten

### Névterek

#### `FWcore`

- Az.
	
#### `FWrender`

- Az.

#### `FWmodel`

- Az.

#### `FWmath`

- Az.

#### `FWutils`

- Az.

#### `FWplayer`

- Az.