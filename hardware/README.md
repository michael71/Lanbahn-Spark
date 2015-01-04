<h1>Lanbahn Hardware for the Spark Core</h1>

<h3>Schematics</h3>
... will be added soon.

<h2>Description</h2>
<h3>Signal and Turnout Board</h3>
<p>This board can drive 2 signals with 4 aspects each. My signals need 12V to 15V and have a common Kathode (common ground).</p>

<p>8 of the outputs of the Spark Core are used in "Analog" mode and drive the A2982 source driver. 4 outputs are used in digital mode, they drive the LB1909MC H-bridges for the Tortoise&reg; turnout motors. The turnout motors need only 10V, which is regulated with a 7810. The 7805 for the 5V Spark-Core supply should be a "switched" regulator to have a better power efficiency.</p>
