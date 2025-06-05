# Kalkulacka s premennymi a zatvorkami | 2025

---

## Popis

Projekt sa skladá z jednej triedy `Calculator`, ktorá obsahuje metódy na vyhodnocovanie výrazov s premennými a zátvorkami.

---

## Calculator

Trieda `Calculator` obsahuje:

### Inštancie:

- `std::string expression`
  - výraz, ktorý sa má vyhodnotiť (nemusí mať validný tvar)
  - výraz môže obsahovať čísla, operátory `+`, `-`, `*`, `/`, `^`, `%`, zátvorky, jednoznakové premenné alebo premennú `res` odkazujúcu sa na posledný vypočítaný výsledok a znak `;`
  - na rýchlejšie vypočítanie viacerých výrazov je možné v rámci jedného priradenia vložiť viacero výrazov, ktoré sú oddelené znakom `;`
  - tieto výrazy sa vyhodnocujú v poradí, v akom boli zadané
  - validné vstupy:
    - `x = 5` // priradenie priamej hodnoty do premennej
    - `y = x + 3` // priradenie výrazu do premennej
    - `(x + 3) * 2` // výraz
    - `x = 5; (x + 3) * 2; y = x + 3` // viacero výrazov oddelených znakom `;`

- `std::map<std::string, int> variables`
  - kľúč je meno vloženej validnej premennej, hodnota je hodnota premennej

- `std::vector<std::string> expressionList`
  - vstupný výraz po validácii, rozdelený na jednotlivé operandy a operátory

- `std::string result`
  - hodnota výrazu po vyhodnotení

- `std::string historyFile`
  - názov súboru, do ktorého sa ukladajú platné premenné a výsledok po každom vyhodnotení

- `std::vector<char> localVariables`
  - zoznam premenných, ktoré sa vyskytujú vo vyhodnocovanom výraze

- `int expressionType`
  - typ výrazu, ktorý sa vyhodnocuje:
    - `0`  // priradenie hodnoty do premennej
    - `1`  // výraz
    - `-1` // default hodnota

- `writtenInHistory`
  - počítadlo, ktoré hovorí, koľko výsledkov bolo zapísaných do `historyFile`

---

### Konštruktor:

- `Calculator(std::string const &history = "history.txt");`
  - vytvorí inštanciu triedy `Calculator` a zresetuje všetky hodnoty
  - `history` je názov súboru, do ktorého sa ukladajú platné premenné a výsledok po každom vyhodnotení

---

### Metódy:

#### Private:

- `bool const oper(char const o)`
  - vráti `true`, ak znak je operátor

- `int const priority(char const op)`
  - pre danú operáciu vráti jej prioritu výpočtu

- `void addVariable(char const variable, int const value)`
  - pridá premennú do mapy premenných

- `valid const calculation(std::string const &expression)`
  - vyhodnotí výraz, ktorý je v poli `expressionList`
  - vráti hodnotu: `INVALID`, `FAIL`, `ZERO_DIVISION` alebo `VALID`

- `std::vector<std::string> const toPostfix(bool const variable = false)`
  - prepíše validný výraz do postfixovej notácie a vráti ho

- `std::string const operation(std::string const &a, std::string const &b, std::string const &op)`
  - vyhodnotí operáciu medzi operandami `a` a `b`

- `valid const calculate()`
  - prechádza všetky výrazy v `expression` a vyhodnocuje ich pomocou `calculation()`

- `void removeVariable(char const var)`
  - vymaže premennú zo zoznamu

- `void removeVariable(std::vector<char> const var)`
  - vymaže všetky premenné z vstupného zoznamu

- `valid loadFromHistory(int const step)`
  - načíta premenné a výsledok zo súboru `historyFile`

- `valid loadFromFile(std::string const &fileName)`
  - načítava riadky zo súboru `fileName` a vyhodnocuje ich

- `bool const isNumber(std::string const &str)`
  - skontroluje, či vstupný reťazec je číslo

#### Public:

- `void operator=(const std::string& expr)`
  - priradí výraz do premennej `expression`

- `int const getResult()`
  - vráti číselnú hodnotu výsledku

- `valid const writeHistory()`
  - zapíše do `historyFile` všetky platné premenné a výsledok

- `valid clearAll()`
  - zresetuje všetky hodnoty triedy `Calculator` a vymaže `historyFile`

- `std::string const getVariables()`
  - vráti reťazec všetkých platných premenných a ich hodnôt

- `valid validExpression(std::string const &expression)`
  - skontroluje validitu jedného výrazu

- `valid validExpression()`
  - kontroluje validitu všetkých výrazov v `expression`

---

## Mimo triedy Calculator

### Enumerácie:

- **States**:
  - `VAR`, `OPER`, `NUM`, `LEFTB`, `RIGHTB`, `EQUAL`, `DEFAULT = -1`

- **TypeOfExpression**:
  - `ASSIGNMENT`, `EXPRESSION`, `NONE = -1`

- **Valid**:
  - `INVALID`, `VALID`, `FAIL`, `SUCCESS`, `ZERO_DIVISION`

- **Operations**:
  - `ADD`, `SUB`, `MUL`, `DIV`, `POW`, `MOD`, `LBR`, `RBR`, `EQ`, `RES`

### Konštanty:

- `std::string signs[]`
  - obsahuje všetky operátory, znak priradenia a meno hodnoty premennej `result`

- `std::string EMPTY_STRING`
  - prázdny reťazec `""`

---

## Testy

K projektu je 50 testov v súbore `test.cpp`, ktoré testujú všetky metódy triedy `Calculator`.
Pre spustenie testov je potrebné stiahnuť si GoogleTest pre testovanie v C++.

Testy sú rozdelené do 6 skupín:

- **Validation**
  - testujú validnosť výrazov

- **Variables**
  - testujú pridávanie a mazanie premenných

- **AdvancedVariables**
  - testy na komplikovanejšie výrazy s premennými

- **Testing**
  - všeobecné testy (mazanie, postfix notácia, multivýrazový vstup)

- **Files**
  - testy na načítanie z histórie a zo súboru

- **Results**
  - testy na výpočet výrazov rôznej zložitosti
