$
$ This danish translation is made by Arne Vajh�j (ARNE@KO.HHS.DK)
$ 30/12/94 partly from the swedish translation.
$

$quote "

$
$ Welcome message...
$

1 "Velkommen til Gophers vidunderlige verden!\n\n"
2 "Gopher har begr�nsninger i sit anvendelsesomr�de og\n"
3 "har ingen garanti.  Kig venligst i filen 'Copyright'\n"
4 "som f�lger med distributionen.\n\n"

5 "Copyright 1991-2000 by the Regents of the University of Minnesota\n\n"
6 "Interrnet Gopher Information Client %s.%s patch%d (%s)\n"
7 "Tryk RETURN for at forts�tte\n"

$ 
$ Errors in .gopherrc
$
8 "Kan ikke skrive til gophers konfigurations fil!!"

$
$ Help for command line 
$

9 "Anvendelse: %s [-sSbDr] [-T type] [-p sti] [-t titel] [v�rt port]+\n"
10 "     -s        Sikker modus, brugere uden egen konto\n"
11 "     -S        Sikker modus, brugere med egen konto\n"
12 "     -p sti    Sti til f�rste objekt\n"
13 "     -T type   Type for f�rste objekt\n"
14 "     -i        S�gekriterium (for -T 7)\n"
15 "     -b        Bogm�rke f�rst\n"
16 "     -r        Fjernbrugere\n"
17 "     -D        Fejlfindingsmodus\n"

$
$ Sider help..
$

18 "Bladringshj�lp"
19 "u, ^G, venstre  : G� tilbage til menu"
20 "mellemrum, ned  : G� til n�ste side"
21 "b, op           : G� til foreg�ende side"
22 "/               : S�g text"
23 "m               : send aktuelle dokument med email"
24 "s               : gem aktuelle dokument"
25 "p               : udskriv aktuelle dokument"
26 "D               : downloade aktuelle dokument"

$ 
$ New Application Menu
$ 
27 "Content-Type Navn"
28 "Applikation for visning"
29 "Applikation for udskrift"

$ 
$ Top Level Options
$ 
30 "Generelle indstilninger"
31 "Indstil applikationer for visning"
32 "Indstil applikationer for udskrift"
33 "Definer ny Content-Type"

$
$ Telnet connection
$
34 "Advarsel!!!!!, du forlader nu Internet"
35 "Gopher programmet og forbinder til en anden v�rt."
36 "Hvis du sidder fast s� kan du presse ctrl og"
37 "] tasterne, og siden skrive quit"
38 "^ tasterne, og siden skrive q."
39 "Forbinder op %.40s, port %d med %s."
40 "Forbinder op %.40s med %s."
41 "Anvend kontonavnet \"%.40s\" for at logge ind"
$
$ HTML menu
$
42 "Advarsel!!!!!, du forlader nu Internet Gopher"
43 "klienten og forbinder dig op mod en HTML klient. Anvend"
44 "klientens hj�lp, hvis du ikke kender programmets kommandoer."
45 "N�r du forlader programmet, kommer du tilbage til"
46 "Internet Gopher klienten."
$
$ Other html msgs
$
47 "Beklager, denne klient underst�tter ikke HTML"

$
$ gopher.c
$
48 "Beklager, kan ikke finde en administartor for dette objekt!"

$ 
$ Gripe form msgs
$ 
49 "Tryk p� Enter for at sende din meddelelse."
50 "Tryk p� Tab tasten ved enden af hver linie du skriver."
51 "�mne"
52 "Problem"
53 "Kan ikke sende mail..."
54 "Sender mail til %s..."
55 "Ord at s�ge efter"
56 "  Side: %d/%d"
57 " for Hj�lp, "
58 " for at Afslutte"
59 " for at g� en menu op"
60 "%s: Intet modtaget for hovedmenuen, kan ikke forts�tte\n"
61 "%sm%s for mail, %sD%s for at downloade, %ss%s for at gemme, eller %sp%s for udskrift:"
62 ", %sm%s for mail, %sD%s for at downloade:"
63 ", <m> for mail, <D> for at downloade:"
64 ",\n   <m> for mail, <D> for at downloade, <s> for at gemme, eller <p> for udskrift:"
65 "Adresse syntaxen forkert..."
66 "Fede s�gord i den indbyggede bladrer"
67 "Bogm�rke"
68 "Kan ikke �bne gammel fil.."
69 "Kan ikke skrive til SYS$SCRATCH!"
70 "Kan ikke skrive til /tmp biblioteket!"
71 "Kan ikke �bne den �nskede fil.."
72 "Kan ikke forbinde op til v�rten %.40s, port %d."
73 "V�lg en dokumenttyp"
74 "V�lg en metod til at downloade"
75 "Forbind til en ny Gopher Server"
76 "Forbind til anonymous FTP Server over en Gopher gateway"
77 "Forbind..."
78 "Forbindelse mislykkedes: %s."
79 "Forbindelse til v�rt mislykkedes."
80 "Kunde ikke kreere '%s'"
81 "Kunde ikke eksekvere %s"
82 "Kunde ikke finde text"
83 "Kunde ikke kreere en tempor�r fil!\n"
84 "Det var fejl ved udskrift, beklager..."
85 "Fejl ved anvendelse af TPU som bladrer!!!"
86 "Afslutte: u"
87 "Hent felt..."
88 "Filen overf�rtes ikke korrekt"
89 "Gopher biblioteks information"
90 "Gopher fejl"
91 "Gopher fil"
92 "Gopher hj�lpe fil"
93 "Gopher objekt information"
94 "Gopher indstilninger"
95 "Gopher overf�rselsfejl"
96 "Hj�lp: ?"
97 "Home Gopher server: %.59s"
98 "V�rt at sp�rge:"
99 "V�rtnavn"
100 "V�rtnavn er ukendt."
101 "Jeg kan ikke forst� din terminal type\n"
102 "Internet Gopher information klient v%s.%s.%d"
103 "Link information"
104 "Lokal dato og tid"
105 "Send aktuelle dokument med mail:"
106 "Sender fil..."
107 "Sender fil til %s..."
108 "At se p� viedeo er ikke implementerat"
109 "Navn p� dette bogm�rke:"
110 "Netv�rksfejl"
111 "Nej"
112 "Ingen bogm�rker definerde"
113 "Ingen kommando for at se p� dette er defineret!"
114 "Ingen brugere '%s'"
115 "SideNed: <MELLEMRUM>"
116 "SideNed: Mellemrum"
117 "SideOp: b"
118 "Port"
119 "Tryk "
120 "Tryk <RETURN> for at forts�tte"
121 "Tryk <RETURN> n�r du er klar\n"
122 "Udskriv aktuelle dokument"
123 "Problem ved skrivning"
124 "Virkelig afslutte (y/n) ?"
125 "Modtager bibliotek..."
126 "Modtager fil..."
127 "Modtager HTML side..."
128 "Modtager information..."
129 "Modtager MIME fil..."
130 "Modtager video..."
131 "Modtager lyd..."
132 "Modtager bildede..."
133 "Henter bibliotek..."
134 "G� tilbage til Menu: u"
135 "Hovedmenu: "
136 "Gem til fil:"
137 "Gem liste af objekter til fil:"
138 "Gemmer fil..."
139 "S�g i bibliotekstitler:"
140 "S�gningen mislykkedes..."
141 "S�g i text:"
142 "S�ger i text..."
143 "S�ger..."
144 "Afgr�nsning (valgfrit)"
145 "Vis v�rtens lokale dato og tid"
146 "St�rrelse  Sprog         Dokumenttype\n"
147 "---------- ------------- ----------------------------\n"
148 "Beklager, kan ikke vise denne fil"
149 "Beklager, kan ikke slette �verste biblioteksniveau."
150 "Beklager, kan ikke downloade det der!"
151 "Beklager, kan ikke lave et bogm�rke af bogm�rker"
152 "Beklager, kan ikke gemme filer med dennee konto"
153 "Beklager, kan ikke spawnw med denne konto"
154 "Beklager, kan ikke gemme dette objekt til en fil!"
155 "Beklager, kan ikke udskrive dette dokument"
156 "Beklager, det findes ingen metode til at udskrive dette dokument"
157 "Beklager, telnet er ikke tilg�ngeligt."
158 "Beklager, denne maskine underst�tter ikke billeder"
159 "Beklager, denne maskine underst�tter ikke lyd"
160 "Beklager, tn3270 er ikke tilg�ngligt."
161 "Beklager, du har ikke lov til at udskrive filer"
162 "Beklager, du har ikke lov til at gemme filer"
163 "Beklager, du har ikke lov til at lave indstilninger i sikker modus."
164 "Beklager, du har ikke lov til at lave dette"
165 "Beklager, denne maskine underst�tter ikke video"
166 "Beklager, findes intet program for denne type af fil...."
167 "Start af DCL mislykkedes!"
168 "Start af standard kommandofortolker (shell) mislykkedes!"
169 "Starter DCL subprocess.  Log ud for at returnere til Gopher.\n"
170 "Starter din standard kommandofortolker (shell). Skriv 'exit' for at returnere til Gopher.\n\n"
171 "Start din \"fangst\" nu...\n\n"
172 "Start downloadningen nu...\n"
173 "Problem med systemet, kan ikke sende mail"
174 "Filnavnet er:"
175 "Kan ikke kreere en netv�rkskanal (socket)."
176 "Unix \"r�rkommando\" (pipe) er �nsket.  Kontroller dine indstilninger!"
177 "Ukendt fejl."
178 "Anvend '/' for at definere en s�gning f�rst..."
179 "Kig p� objekt nummer: "
180 "Ja"
181 "Dit valg"
182 "\nDownloadning klar. %d bytes totalt, %d bytes/s\n"
183 "\nDownloadning kunde ikke fuldf�res, beklager... \n"
184 "\nTryk %sRETURN%s for at forts�tte"
185 "\nTryk <RETURN> for at forts�tte"
186 "\n\nServer information\n"
187 "anonymous FTP v�rt"
188 "gopher: Intet modtaget for hovedmenuen, kan ikke forts�tte\n"
189 "'d'elete kommandoen er kun for bogm�rker."
190 "^G      : Fortryd"
191 "OK: Enter"
192 "Tab, ^N : Flyt til n�ste felt"
193 "^P      : Flyt til forrige felt"
194 "^F      : Vis n�ste side"
195 "^B      : Vis forrige side"
196 "^-      : Hj�lp (pr�v ^/ eller ^7)"
197 "Enter   : Accepter"
198 "[Hj�lp: ^-]  [Fortryd: ^G]"
199 "[Skift v�rdier: mellemrume]  [Vis v�rdier: l]"
200 "V�lg et punkt"
$
$ Choose menu..
$
201 "Form Hj�lp"
202 "^G        : Fortryd"
203 "Down, ^N  : Flyt til n�ste valg"
204 "Up, ^P    : Flyt til forrige valg"
205 "Space, ^F : Vis n�ste side"
206 "b, ^B     : Vis forrige side"
207 "^         : Vis f�rste side"
208 "$         : Vis sidste side"
209 "0-9       : V�lg specifik valg"
210 "Enter     : V�lg nuv�rende valg"
211 "Fortryd: ^G"
212 " (standard)"
213 "Valg Dialog Hj�lp"
214 "Intet tilg�ngelig."
215 "Advarsel, forkert linie i gopherrc: %s\n"
216 "Aborterer gopher konfiguration gem!!"
217 "Tekst"
218 "En ny konfiguration er tilg�ngelig.  Brug den?"
219 "Standard system printer"
220 "ANSI tilknyttet printer"
221 "Print til"

$ Default language/region gopher+ view til choose

222 "Dk_DK"
$
$  Multilingual HELP files are now possible
$                      If CONF.H defines GOPHERHELP_SECURE, make sure you
$                      have a separate file for secure users' help that
$                      doesn't document features they're not allowed til use.
$  For VMS:
$
223 "GopherP_Dir:gopher_hlp.Dk"
224 "GopherP_Dir:gopher_hlp_secure.Dk"
$
$  For Unix:
$
225 "/usr/local/lib/gopher.hlp.Dk"
226 "/usr/local/lib/gopher.hlp_secure.Dk"
$
$ Gripe Enhancements
$
227 "Beklager,kan ikke sende mail i sikker modus"
228 "Kan ikke sende mail til nul address..."
229 "Med hensyn tilf�lgende Gopher punkt:\n"
230 "Til"
231 "<Sidste linie>"

$
$ A message that asks the User if they speak this language..
$

232 "Taler du dansk?"

233 " Downloading %d bytes"
234 "    1200bps: %d minutter\n"
235 "    2400bps: %d minutter\n"
236 "   14400bps: %d minutter\n\n"

$
$ Add til post-sider option list in SecureMode and NoShellMode
$
237 ", %sD%s for at downloade:"
238 ", <D> for at downloade:"
