<h1>Lanbahn Hardware for the Spark Core</h1>

<h3>Schematics</h3>
... will be added soon.

<h2>Description</h2>
<h3>Signal and Turnout Board</h3>
<p>This board can drive 2 signals with 4 aspects each. It is meant for signals which need 12V (or 15V) and have a common Kathode (common GND).i</p>

<p>8 of the outputs of the Spark Core as used as "Analog" outs and drive the A2982 "source driver". 4 outputs are used as digital outputs, they drive the LB1909MC H-bridge drivers for the Tortoise(TM) turnout motors. The turnout motors need only 10V, which is regulated with a 7810. The 7805 for the 5V Spark-Core supply should be a "switched" regulator to have a better power efficiency.</p>
