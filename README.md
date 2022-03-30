# Catch the bouncing led ball

First assignment of Sistem Embedded & IoT

 Info Assignment:
  1 red ledState
  
  1 potentiometer
  
  4 green ledState
  
  4 tactile buttons
  
 Scopo del gioco:
 
   Fermare la pallina che rimbalza da un ledState all'altro attraverso i pulsanti collegati ai ledState.
   
 Dettagli:
 
  Stati del gioco:
  
   -Riposo: qualsiasi tasto risveglia il gioco
   
   -In Attesa: premendo il tasto T1 il gioco parte
   
   -In gioco: lo stato continua fino a quando l'utente non sbaglia
   
   Se nello stato di attesa non viene premuto il tasto T1 entro 10 secondi il gioco entra in modalità riposo
   
Funzionamendo del gioco:

I ledState verdi si accendono e spengono in sequenza scorrendo avanti e indietro ad una velocità S e continuano per un tempo randomico T.
Allo scadere del tempo la posizione in cui il ledState resta acceso sarà la posizione P.
Collegata ad ogni posizione è presente un pulsante che dovrà essere premuto entro un determinato tempo T2 prestabilito.
Se viene premuto entro il tempo limite allora lo score incrementerà e il tempo T2 diminuirà di un fattore F
Se non viene premuto in tempo si avrà game over, ci sarà quindi una print su terminale per 10 secondi, dopodichè il gioco ripartirà dallo stato iniziale
Prima di iniziare il gioco il potenziometro può essere utilizzato per decidere il level di difficoltà da cui partire in un range da 1 a 8
Ciò su cui inciderà il potenziometro sarà il level del fattore F.
