<h1>Spark Core Firmware</h1>

<p>The firmware is similar to the "tinker.ino" firmware, besides digitalWrite() and digitalRead() it adds a function to the sparks:<p>

<pre><i>lanbahnfun(String command);</i></pre>

<p>The command string consists of an output channel plus a value for it, like "S 1 Y" (set signal 1 to yellow). The standard hardware has 2 signal outputs (4 aspects, R=red, G=green, Y=yellow, F=feather (this was built for our UK layout <a href="http://www.oscale.net/en/lonstokewest" target="_blank">"Lonstoke West"</a>) and 4 turnout outputs (H-Bridge for Tortoise) "T 0", "T 1", "T 2", "T 3" with output values T=thrown and C=closed. A turnout command to set turnout output 2 to "thrown" is "T 2 T" and to set it to closed:"T 2 C".<p>

<p>Two variables are defined:</p>

<pre><i>rssi</i> (returns rssi value)
<i>version</i> (returns a string with firmware version info)</pre>

<p>For details have a look into tinkerplus_A121.ino on this page and into the documentation on the spark.io site <a href="http://docs.spark.io/firmware/">http://docs.spark.io/firmware/</a>.</p>
