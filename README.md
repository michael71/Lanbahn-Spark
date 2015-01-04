Lanbahn-Spark
=============

Lanbahn software for controlling Spark.io cores - see http://www.lanbahn.net/spark 

<h2>Installation:</h2>
<pre>1) copy all files in a directory
2) install nodejs 
3) install the node modules "dateformat" and "request" 
4) change settings (your access_token, your spark-server url) in sparkconfig.js
5) start program with: <i>node lanbahn2spark.js</i>
</pre>

<h2>Pre-Requisites</h2>
<pre>1) lanbahn hardware with Spark Core (see http://www.lanbahn.net/spark )
2) Spark Core supplied with your wifi keys (with android SW on spark.io site)
3) "firmware_A121.bin" installed on Spark Core
4) adapt the config file lb2spark.json which defines the mappings between 
   Lanbahn commands and command for the sparks

and, if you want to have a fast response to commands: 

5) a spark-server in your local network (see documentation on http://docs.spark.io/ )
   and the keys of this server on your cores.
</pre>

<h2>How it works</h2>
<p>After starting the software with "node lanbahn2spark.js" a UDP multicast client 
is listening to UDP messages on the LANBAHN port 27027 and LANBAHN multicast group. 
When the client receives a "SET" messages with an address that can be matched to one 
of the connected spark cores, the spark core will be sent a http request with the 
appropriate command to change it outputs. </p>
