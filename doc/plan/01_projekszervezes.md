# Projektszervez�s

## File szinten

- `src/` k�nyvt�rba megy minden source
	- `src/api` k�ynvt�r az api k�nyvt�ra
	- `src/idogep` k�nyvt�r a szerkeszt��
	- **TODO** ide kell m�g valahova minden egy�b apr�s�g k�dja, python szkriptek, html/js toolok, minden ilyen
		- Text�ra gener�tor
		- A BMFont-b�l kialak�tott text�ra gener�tor library
- `test/`
	- unitesztek - val�sz�n�leg gteszt lesz
	- funkcion�lis tesztek - API egyes r�szeit teszteli le, val�sz�n�leg robot framework-kel
	- engine teszt - az apiban lev� player funcion�lis tesztjei
	- **TODO** - ezeket el kell v�lasztani valahogy egym�st�l
	
- `build/` 
	- ide szemetel a ford�t�
	
- `demos/`
	- az api-val k�sz�lt dem�k, releasek
	
- `tools`
	- *subrepository* minden 3rd party cucc ide j�n + a ford�t�sukhoz sz�ks�ges MSVC projektek, illetve szkriptek
	
**TODO** - a shader effekteknek is kell valahova valamilyen k�nyvt�r, amiben ellenn�nek, ne az assetsben legyen k�l�n. 



## API szinten

### N�vterek

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