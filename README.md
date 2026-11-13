# ESP32_V6_myref

Dit is mijn referentieproject voor de ESP32-S3 Mini. Ik gebruik het als
verzameling van mijn eigen, herbruikbare ESP-IDF-componenten. Nieuwe componenten
kunnen hier worden toegevoegd en in dit project worden uitgeprobeerd.

## Componenten

| Naam | Bestaat | Map |
|---|---|---|
| `myGPIO` | Ja | `components/basic/` |
| `myADC` | Ja | `components/basic/` |
| `myUltrasoon_HCSR04` | Ja | `components/basic/` |
| `myRMT` | Ja | `components/basic/` |
| `myPWM` | Ja | `components/basic/` |
| `myUART` | Ja | `components/communication/` |
| `myUSB` | Ja | `components/communication/` |
| `myESP_S3_mini_board` | Ja | `components/boards/` |

### myPWM

Voeg `myPWM` toe aan `REQUIRES` in de `CMakeLists.txt` van de component die
de functies gebruikt. Stel eerst de vier PWM-pinnen bovenaan `myPWM.c` in;
standaard zijn dit GPIO4, GPIO5, GPIO6 en GPIO8. Kies verschillende pinnen
die beschikbaar zijn op je bord en niet door andere onderdelen worden gebruikt.
De bijbehorende kanalen en timers staan er vast ingesteld; je hoeft ze niet
mee te geven aan de functies. Geef telkens het GPIO-nummer mee:

```c
#include "myPWM.h"
#include "esp_check.h"

void pwm_voorbeeld(void)
{
    // Deze pinnen moeten overeenkomen met de instellingen in myPWM.c.
    ESP_ERROR_CHECK(myPWM_init(4, MYPWM_TOON_DO));
    ESP_ERROR_CHECK(myPWM_Duty(4, 50));               // 50% dutycycle
    ESP_ERROR_CHECK(myPWM_setFreq(4, MYPWM_TOON_LA)); // 440 Hz

    ESP_ERROR_CHECK(myPWM_init(5, 1000));             // onafhankelijke PWM op GPIO5
    ESP_ERROR_CHECK(myPWM_Duty(5, 25));
    ESP_ERROR_CHECK(myPWM_Duty(4, 0));                // uitgang uit
}
```

De toonladder bevat `MYPWM_TOON_DO`, `RE`, `MI`, `FA`, `SOL`, `LA`, `SI`
en `DO_HOOG`, telkens met het volledige voorvoegsel `MYPWM_TOON_`. De tonen
zijn C4 tot en met C5, afgerond op hele hertz.

`myPWM_init` start met 0% duty; `myPWM_Duty` accepteert 0 tot en met 100
procent. De frequentie is in Hz en moet groter zijn dan 0. De vaste resolutie
is 10 bit; afhankelijk van de gebruikte klok kan de hardware bepaalde
frequenties niet maken of de gevraagde frequentie slechts benaderen.
Controleer de `esp_err_t`-retourwaarde van elke functie. Een pin die niet in
de instellingen staat, een pin opnieuw initialiseren of een niet-geinitialiseerde
pin gebruiken levert een fout op.

Elke pin gebruikt een eigen LEDC-timer en -kanaal, zodat de frequenties
onafhankelijk instelbaar zijn. De ESP32-S3 heeft vier LEDC-timers: gebruik
daarom maximaal vier PWM-pinnen en reserveer de gebruikte low-speed
LEDC-timers en -kanalen 0 tot en met 3 voor deze component. Deze component biedt geen
vrijgave van pinnen en de functies mogen niet gelijktijdig vanuit meerdere
tasks worden aangeroepen.


## Als je hier een component van wil gebruiken

Als je van dit project een component wil overnemen, maak dan in de map main een nieuwe file "idf_component.yml" aan 
(als je dat nog niet had)

VOOR HTTPS:
En voeg onderstaande tekst hierin toe (dit is het voorbeeldje voor de gpio files)

```
dependencies:

  myGPIO:
    path: components/basic/myGPIO
    git: https://github.com/KNaelaerts/ESP32_V6_pubref.git

```


VOOR SSH:
En voeg onderstaande tekst hierin toe (dit is het voorbeeldje voor de gpio files)

```
dependencies:

  myGPIO:
    path: components/basic/myGPIO
    git: git@github.com:KNaelaerts/ESP32_V6_pubref.git

```

## omgang moet git
### 1ste keer
* ga naar github en maak een nieuwe repo aan (en copy de link)
* "git init"    dit maakt een verborgen folder met alle eigenschappen van git aan
* "git remote add origin +url"  dit legt de link tussen jouw project en de repo die je hebt aangemaakt

### als je veranderingen wil doorvoeren
* "git add ."
* "git commit -m +info"
* "git branch -M main"          -hier kies je de branch die je wil
* "git push -u origin main"

### als je een nieuwe branch wil starten
* "git switch main"                 -ga eerst naar de branch waarvan je vertrekt
* "git pull"                        -zorg dat die up-to-date is
* "git switch -c +naam"             -maakt de nieuwe branch aan en gaat er meteen naartoe
* werk zoals gewoonlijk: "git add ." en "git commit -m +info"
* "git push -u origin +naam"        -publiceert de nieuwe branch op github

### pullen
* "git pull"    haalt de nieuwste wijzigingen van github op en voegt ze meteen samen met je huidige branch
* het is eigenlijk een "git fetch" gevolgd door een "git merge"
* doe dit best voor je aan iets nieuws begint, en commit eerst je eigen werk

### fetchen
* "git fetch"   haalt de nieuwste wijzigingen van github op, maar past je eigen bestanden niet aan
* zo kan je eerst bekijken wat er veranderd is, bv. met "git log HEAD..origin/main" of "git diff origin/main"
* "git branch -a" toont alle branches, ook die van github
* wil je de wijzigingen daarna toepassen, gebruik dan "git merge origin/main" (of gewoon "git pull")

### twee branches samenvoegen (mergen)
* "git switch main"                 -ga naar de branch waarin je wil samenvoegen (de ontvanger)
* "git pull"                        -zorg dat die up-to-date is
* "git merge +naam"                 -voegt de andere branch toe aan de huidige branch
* "git push"                        -publiceert het resultaat
* bij een conflict zegt git welke bestanden problemen geven: open ze, kies in de markeringen (`<<<<<<<`, `=======`, `>>>>>>>`) wat je wil behouden,
  en doe dan "git add ." en "git commit"
* "git merge --abort"               -breekt de merge af als je terug wil naar het begin

## myflow

Ik probeer bij elke component een voorbeeld toe te voegen in de vorm van een flowchart.
Die flowcharts maak ik met de myflow-extensie. Deze extensie is privé, omdat ze nog maar pas gemaakt is.

Heb je interesse of vragen over myflow? Spreek mij gerust aan.
