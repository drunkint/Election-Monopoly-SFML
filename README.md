# Election-Monopoly-SFML
This Election monopoly-like game aims to arouse political awareness among Taiwanese Students.

The following are the instructions of the game: (for the Mandarin version, please see the release notes) The game board is written in Mandarin. Don't worry if you couldn't understand though; The prompts are still written in English.

Important stuff to know:
1. There are 2 players: CuteCat and LingChieh (my professor for this course)
2. There will be 15 days(rounds) before election day.
3. There is a monopoly-like board, with cities/counties of Taiwan as its locations.
4. Each city/county will have a local support ratio (in percentage) (player1_support_rate : player2_support_rate). Default is 50:50.

Goal:
5. Each city/county will have a different amount of votes (which mostly depends on the number of people living there). 
This game assumes Taiwan does elections the same way as America: On election day, for each location we see whose local support percentage is higher. 
The winner in that particular area gets all the votes in that region. The same rule applies to all the regions. 
If the local support ratio is 50:50 then no one wins in that area. Add up all the votes to see who wins! 
(btw in reallity Taiwan uses another election system different from America)

Flow:

6. The game starts at menu. Press space to continue.

7. Then a simplified version in Mandarin of the rules will pop up. Press space to continue.

8. And the game is on! Press space for player one to roll the dice.

9. Player one might end up on one of the 3 different kinds of Locations:

       City. Then we will need to choose among one of the three options:
          a. Beg door to door for votes: spend 100 thousand ntd, local support rate increases by 5%~10%.
          b. Give a fundraising speech: get 100 to 300 thousand ntd.
          c. Bribe: spend 800 thousand, local support rate skyrockets by 30%~60%
          
       News. Then we will need to choose if we want to defame our opponent:
          a. Yes: Spend 1 million, and a random city/county will 100% vote for you.
          b. No: Skip
          
       Special cases. These are:
          a. Office of the President: get 500 thousand ntd
          b. Independent Country of Miaoli: "Going abroad during COVID-19? Quarintine for 3 days"
          c. NTU Hospital: Stay for 3 days
          d. Lyudao prison: money cuts half
          e. Police: If you bribed during the past 10 days, teleport to Lyudao prison and stay there for 3 days.
          
10. Press space to pass the dice to the other player.
11. Repeat this for 15 days(rounds)
12. The game ends on election day. Please see 5. for details.



